/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iFlextimer.c
Author and date :	Monnerat Serge 29 août 2013

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iFlextimer.h"
#include "derivative.h"
#include "../Interfaces/iPit.h"

// FTM0 pour la commande des PWM des moteurs DC
#define TFC_MOTOR_SWITCHING_FREQUENCY ((float)(20000.0))
#define FTM0_MOD_VALUE	(int)((float)(PERIPHERAL_BUS_CLOCK)/TFC_MOTOR_SWITCHING_FREQUENCY)
#define FTM0_CLOCK                	PERIPHERAL_BUS_CLOCK
#define FTM0_CLK_PRESCALE          	0  		
#define FTM0_OVERFLOW_FREQUENCY 		5000


// FTM1 pour la mesure de vitesse
#define FTM1_CLK_PRESCALE 			3 	// Division par 8 du clock de 50MHz
#define FTM1_OVERFLOW_FREQUENCY (4)
#define kFreqTCNT 							((float)6250000.0)

// Nombre de pôle des aimants
#define kNbPole 2

static UInt16 sOldTimerValueChL;
static float sDeltaChL=0;
static UInt16 sOldTimerValueChR;
static float sDeltaChR=0;

static int sDelayTimeoutSpeed;
static float sSpeedLeft=0;
static float sSpeedRight=0;
static UInt32 sOverFlowCounterL;
static UInt32 sOverFlowCounterR;

// FTM2 pour la commandes des PWM des servos
#define FTM2_CLOCK              CORE_CLOCK	// Le clock est le Cpu Clock
#define FTM2_CLK_PRESCALE        7 					// Prescaler -> 7 = divide by 128
#define FTM2_OVERFLOW_FREQUENCY 100.0  			// Fréquence du PWM de 100Hz
#define SERVO_MIN_DUTY_CYCLE		(float)(.0010*FTM2_OVERFLOW_FREQUENCY)	// Largeur de pulse min = 1ms
#define SERVO_MAX_DUTY_CYCLE    (float)(.0020*FTM2_OVERFLOW_FREQUENCY)  // Largeur de pulse max = 2ms

//------------------------------------------------------------
// Flextimer  setup
// FTM0 --> moteur DC
// FTM1 --> mesure vitesse
// FTM2 --> servos
//------------------------------------------------------------
void iFlextimer_Config(void)
{
	// Enable the Clock to the FTM0 Module
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	// Enable the FTM functions on the the port
	PORTC_PCR1 = PORT_PCR_MUX(4);
	PORTC_PCR2 = PORT_PCR_MUX(4);   
	
	// Features Mode Selection (FTMx_MODE)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 900
	// Disable Write Protection - enables changes to QUADEN, DECAPEN, etc.  
	FTM0_MODE |= FTM_MODE_WPDIS_MASK; 
	//FTMEN is bit 0, need to set to zero so DECAPEN can be set to 0
	FTM0_MODE &= (~FTM_MODE_FTMEN_MASK);
	
	// Quadrature Decoder Control and Status (FTMx_QDCTRL)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 923
	// Set Edge Aligned PWM
	FTM0_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;  
	
	// Counter (FTMx_CNT)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 892
	// FTM Counter Value - reset counter to zero
	FTM0_CNT = 0x0; //FTM Counter Value - reset counter to zero
	
	// Modulo (FTMx_MOD)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 892
	//Set the overflow rate
	FTM0_MOD = (FTM0_CLOCK/(1<<FTM0_CLK_PRESCALE))/TFC_MOTOR_SWITCHING_FREQUENCY;
	
	// Counter Initial Value (FTMx_CNTIN)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 897
	// Set the Counter Initial Value to 0
	FTM0_CNTIN = 0;
	
	// Channel (n) Status and Control (FTMx_CSC)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 893
	// CHIE=0, Disable channel interrupts. Use software polling
	// MSB/MSA/ELSB/ELSA --> Edge-aligned PWM
	// DMA=0, Disable DMA transfers
	FTM0_C0SC=0;
	FTM0_C0SC|=(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);
	FTM0_C1SC=0;
	FTM0_C1SC|=(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);
	
	// Status and Control (FTMx_SC)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 890
	// Edit registers when no clock is fed to timer so the MOD value, gets pushed in immediately
	FTM0_SC=0;
	
	// Channel (n) Value (FTMx_CV)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 896
	// FTMx_CnV contains the captured FTM counter value, this value determines the pulse width
	// Duty at 50%
	FTM0_C0V = 0;
	FTM0_C1V = 0;
	
	// Status and Control (FTMx_SC)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 890
	// CPWMS=0,Center-aligned PWM Select -> FTM counter operates in up counting mode
	// CLKS=1, Clock Source Selection -> System clock -> 50Mhz
	// PS:3,Prescale Factor Selection -> Divide by 8 
	FTM0_SC|=(FTM_SC_CLKS(1)|FTM_SC_PS(FTM0_CLK_PRESCALE));
	
	// FTM PWM Load (FTMx_PWMLOAD)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 933
	FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK_MASK;
	
	// INH du HBridge = 1 --> enable des inputs
	GPIOB_PSOR|=1<<9;

	
	//------------------------------------------------------------
	// FTM1 --> Input Capture
	//------------------------------------------------------------
	// Input capture si:
	//• DECAPEN = 0
	//• COMBINE = 0
	//• CPWMS = 0
	//• MSnB:MSnA = 0:0, and
	//• ELSnB:ELSnA != 0:0
	// Vitesse max moteur 16'000 +- 3200 tr/mn --> ~ 20'000 tr/mn --> ~ 340 tr/s
	// Si 2 aimants --> fréq. max = 340Hz * 4 = 1360 Hz
	// Si 4 aimants --> fréq. max = 340Hz * 8 = 2720 Hz
	
	// Enable du clock
	SIM_SCGC6|=SIM_SCGC6_FTM1_MASK;
	
	// Pin attribuées au Flextimer
	PORTA_PCR12 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; 	// Moteur A vitesse A 
	PORTA_PCR13 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; 	// Moteur B vitesse A
		
	// Features Mode Selection (FTMx_MODE)
	// Disable Write Protection - enables changes to QUADEN, DECAPEN, etc.  
	FTM1_MODE |= FTM_MODE_WPDIS_MASK; 
	//FTMEN is bit 0, need to set to zero so DECAPEN can be set to 0
	FTM1_MODE &= (~FTM_MODE_FTMEN_MASK);
	
	// Quadrature Decoder Control and Status (FTMx_QDCTRL)
	// Set Edge Aligned PWM
	//FTM1_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;  
	
	// Counter (FTMx_CNT)
	// FTM Counter Value - reset counter to zero
	FTM1_CNT = 0x0; //FTM Counter Value - reset counter to zero
	
	// Modulo (FTMx_MOD)
	// Set the overflow rate
	FTM1_MOD=65535;
	
	// Counter Initial Value (FTMx_CNTIN)
	//Set the Counter Initial Value to 0
	FTM1_CNTIN = 0;
	
	// Channel (n) Status and Control (FTMx_CSC)
	// CHIE=0, Disable channel interrupts. Use software polling
	// MSB/MSA/ELSB/ELSA --> Input Capture --> rising edge only
	// DMA=0, Disable DMA transfers
	FTM1_C0SC=0;
	FTM1_C0SC|=(FTM_CnSC_ELSA_MASK|FTM_CnSC_CHIE_MASK);
	FTM1_C1SC=0;
	FTM1_C1SC|=(FTM_CnSC_ELSA_MASK|FTM_CnSC_CHIE_MASK);
	
	// Status and Control (FTMx_SC)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 890
	// CLKS=1, Clock Source Selection -> System clock -> 50Mhz
	// PS:3,Prescale Factor Selection -> Divide by 8 --> fréquence de comptage de 50MHz/8
	// --> overflow à 50MHz/8/65535 => Fréq min de 95Hz avec 2 aimants 1425 tr/mn min
	// Interruption overflow enable
	FTM1_SC=0|FTM_SC_TOIE_MASK|(FTM_SC_CLKS(1)|FTM_SC_PS(FTM1_CLK_PRESCALE));
	
	// Enable de l'interrupt
	enable_irq(INT_FTM1-16);
	
	//------------------------------------------------------------
	// FTM2 --> PWM servo
	//------------------------------------------------------------
	// Features Mode Selection (FTMx_MODE)
	// Disable Write Protection - enables changes to QUADEN, DECAPEN, etc.  
	FTM2_MODE |= FTM_MODE_WPDIS_MASK; 
	//FTMEN is bit 0, need to set to zero so DECAPEN can be set to 0
	FTM2_MODE &= (~FTM_MODE_FTMEN_MASK);
	
	// Quadrature Decoder Control and Status (FTMx_QDCTRL)
	// Set Edge Aligned PWM
	FTM2_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;  
	
	// Counter (FTMx_CNT)
	// FTM Counter Value - reset counter to zero
	FTM2_CNT = 0x0; //FTM Counter Value - reset counter to zero
	
	// Modulo (FTMx_MOD)
	// Set the overflow rate
	// 50Mhz/128/100Hz=3906
	FTM2_MOD=(PERIPHERAL_BUS_CLOCK/(1<<FTM2_CLK_PRESCALE))/FTM2_OVERFLOW_FREQUENCY;
	
	// Counter Initial Value (FTMx_CNTIN)
	//Set the Counter Initial Value to 0
	FTM2_CNTIN = 0;
	
	// Channel (n) Status and Control (FTMx_CSC)
	// CHIE=0, Disable channel interrupts. Use software polling
	// MSB/MSA/ELSB/ELSA --> Edge-aligned PWM
	// DMA=0, Disable DMA transfers
	FTM2_C0SC=0;
	FTM2_C0SC|=(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);
	FTM2_C1SC=0;
	FTM2_C1SC|=(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK);
	
	// Status and Control (FTMx_SC)
	// Edit registers when no clock is fed to timer so the MOD value, gets pushed in immediately
	FTM2_SC=0;
	
	// Channel (n) Value (FTMx_CV)
	// FTMx_CnV contains the captured FTM counter value, this value determines the pulse width
	// Duty at 50%
	FTM2_C0V = FTM2_MOD/10;
	FTM2_C1V = FTM2_MOD/10;
	
	// Status and Control (FTMx_SC)
	// CPWMS=0,Center-aligned PWM Select -> FTM counter operates in up counting mode
	// CLKS=1, Clock Source Selection -> System clock -> 50Mhz
	// PS:7,Prescale Factor Selection -> Divide by 128 
	FTM2_SC|=(FTM_SC_CLKS(1)|FTM_SC_PS(FTM2_CLK_PRESCALE));
	
	// FTM PWM Load (FTMx_PWMLOAD)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 933
	FTM2_PWMLOAD |= FTM_PWMLOAD_LDOK_MASK;
	
	// Enable TPM1 IRQ on the NVIC
	//enable_irq (INT_FTM2-16);      
	
	sDelayTimeoutSpeed = iPit_GetDelay(kPit1,100); 
	
	
}

//-----------------------------------------------------------------------------
// Lecture de la vitesse de rotation des moteurs en tr/mn
// Moteur A = moteur gauche --> valeur négative = en arrière, valeur pos=en avant
// Moteur B = moteur droite
//-----------------------------------------------------------------------------
void iFlextimer_GetSpeed(float *aSpeedMotLeft, float *aSpeedMotRight)
{
	if(	iPit_IsDelayDone(kPit1,sDelayTimeoutSpeed))
	{
			*aSpeedMotLeft=0.0;
			*aSpeedMotRight=0.0;
	}
	else
	{
			*aSpeedMotLeft=sSpeedLeft;
			*aSpeedMotRight=sSpeedRight;
	}
}

//------------------------------------------------------------
// Routine d'interruption FTM1
//------------------------------------------------------------
void FTM1_IRQHandler(void) 
{
	UInt32 aVal;
	
	iPit_ReStart(kPit1,sDelayTimeoutSpeed,100);
	
	// Test si le compteur à fait un overflow
	if((FTM1_SC&FTM_SC_TOF_MASK)==FTM_SC_TOF_MASK)
		{
			// Reset du flag
			FTM1_SC&=(~FTM_SC_TOF_MASK);
			
			// Incrémentation des compteurs pour la calcul 
			// de la vitesse
			sOverFlowCounterL++;
			sOverFlowCounterR++;
		}
	
	// Calcul de la vitesse du moteur gauche --> teste si flanc sur input capture
	if((FTM1_C0SC&FTM_CnSC_CHF_MASK)==FTM_CnSC_CHF_MASK)
		{
			// Reset du flag
			FTM1_C0SC&=(~FTM_CnSC_CHF_MASK);
			
			// Lecture du registre INPUT CAPTURE/OUTPUT COMPARE REGISTER 1
			aVal=FTM1_C0V;
			
			if(sOverFlowCounterL>0)
				{
					sDeltaChL=(sOverFlowCounterL*65535.0)-sOldTimerValueChL+aVal;
				}
			else
				{
					sDeltaChL=aVal-sOldTimerValueChL;
				}
			
			// Calcul de la vitesse en tr/sec
			sSpeedLeft=((kFreqTCNT/sDeltaChL)/(kNbPole*2));  //* 60; pour tr/min
			
			// Test l'entrée B (PortA pin5) afin de déterminer le sens
			if ((GPIOA_PDIR&0x20)==0x0)
				{
					sSpeedLeft=sSpeedLeft*(-1);
				}
	
			// On sauve l'ancienne valeur du compteur
			sOldTimerValueChL=aVal;
			
			// Reset du compteur d'overflow
			sOverFlowCounterL=0;
		}
	
	// Calcul de la vitesse du moteur droit --> teste si flanc sur input capture
	if((FTM1_C1SC&FTM_CnSC_CHF_MASK)==FTM_CnSC_CHF_MASK)
			{
				// Reset du flag
				FTM1_C1SC&=(~FTM_CnSC_CHF_MASK);
			
				// Lecture du registre INPUT CAPTURE/OUTPUT COMPARE REGISTER 1
				aVal=FTM1_C1V;
				
				if(sOverFlowCounterR>0)
					{
						sDeltaChR=(sOverFlowCounterR*65535.0)-sOldTimerValueChR+aVal;
					}
				else
					{
						sDeltaChR=aVal-sOldTimerValueChR;
					}
				
				// Calcul de la vitesse en tr/sec
				sSpeedRight=((kFreqTCNT/sDeltaChR)/(kNbPole*2));  //* 60; pour tr/min
				
				// Test l'entrée B (PortA pin5) afin de déterminer le sens
				// A vérifier, pas encore testé!
				if ((GPIOA_PDIR&0x4000)==00)
					{
						sSpeedRight=sSpeedRight*(-1);
					}
				
				// On sauve l'ancienne valeur du compteur
				sOldTimerValueChR=aVal;
				
				// Reset du compteur d'overflow
				sOverFlowCounterR=0;
			}
}

//---------------------------------------------------------------------------
// Modifie la position du servo
// Position entre -1.0 to 1.0.   
//---------------------------------------------------------------------------
// --> Utilisez les constantes SERVO_X_MIN_DUTY_CYCLE and SERVO_MAX_DUTY_CYCLE  pour calibrer les extrêmes
void iFlextimer_SetServoDuty(UInt8 aServoNumber, float aPosition)
{
	float aDutyCycle;
	
	// Saturation
	if(aPosition>1)
		{
			aPosition=1;
		}
	else if(aPosition<-1)
		{
			aPosition=-1;
		}
	
	// Calcul du duty
	aDutyCycle=(((aPosition + 1.0)/2)*(SERVO_MAX_DUTY_CYCLE - SERVO_MIN_DUTY_CYCLE))+SERVO_MIN_DUTY_CYCLE;
	
	switch(aServoNumber)
		{
			default:
			case 0:
				FTM2_C0V = FTM2_MOD * aDutyCycle;
			break;
			
			case 1:
				FTM2_C1V = FTM2_MOD * aDutyCycle;
			break;
		}
}

//---------------------------------------------------------------------------
// Modifie le duty du PWM
// Moteur A = moteur gauche
// Moteur B = moteur droite
//---------------------------------------------------------------------------
void iFlextimer_SetMotorDuty(float aMotorLeft , float aMotorRight)
{
	// Saturation du duty
	if(aMotorLeft>1.0)
		{
			aMotorLeft = 1.0;
		}
	else if(aMotorLeft<-1.0)
		{
			aMotorLeft = -1.0;
		}
	
	if(aMotorRight>1.0)
		{
			aMotorRight = 1.0;
		}
	else if(aMotorRight<-1.0)
		{
			aMotorRight = -1.0;
		}
	
	
	// Sens de rotation
	if(aMotorLeft>0)
		{
			GPIOA_PSOR|=(UInt32)(1<<15);
		}
	else
		{
			GPIOA_PCOR|=(UInt32)(1<<15);
			aMotorLeft=aMotorLeft*(-1);
		}
	
	if(aMotorRight>0)
		{
			GPIOA_PCOR |= (UInt32)(1<<16);
		}
	else
		{
			GPIOA_PSOR|=(UInt32)(1<<16);
			aMotorRight=aMotorRight*(-1);
		}
	
	// Config du duty
	FTM0_C0V = (uint16_t) ((float)FTM0_MOD * aMotorLeft);
	FTM0_C1V = (uint16_t) ((float)FTM0_MOD * aMotorRight);
}


