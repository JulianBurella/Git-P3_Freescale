/*
------------------------------------------------------------
Copyright 2003-201x Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	main.c
Author and date :	Monnerat Serge 29 ao�t 2013

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/
#include "derivative.h"
#include "../Modules/mSpi.h"
#include "../Modules/mDac.h"
#include "../Modules/mAccelMagneto.h"
#include "../Modules/mWifi.h"
#include "../Modules/mTimer.h"
#include "../Modules/mCpu.h"
#include "../Modules/mSwitch.h"
#include "../Modules/mLeds.h"
#include "../Modules/mAd.h"
#include "../Modules/mDelay.h"
#include "../Modules/mRs232.h"
#include "stdio.h"
#include "..\Utils\stbp.h"
#include "string.h"
#include "math.h"

//D�finitions de tests
#define TabBegin 13
#define TabOffset 1

// Tableau contenant l'image (et diverses infos) de la camera
// digitale, le tableau test est utilis� pour l'app Labview
static UInt8 sImageTab[200];
static UInt8 sImageDeriveTab[200];

static UInt8 sUartMessage[200];

// Mesure de l'acc��rom�tre et du magn�tom�tre
static SRAWDATAEnum sAccel;				// en g
static SRAWDATAEnum sMagneto;			// en micro teslas
//static UInt8 sAccelMagnetoStatus;	
float sYaw;		// en degr�s
float sRoll;	// en degr�s
float sPitch;	// en degr�s

// Mesure de vitesse et de sens
float sSpeedMotLeft =0.0;
float sSpeedMotRight = 0.0;



//Variable contenant la vitesse des moteurs
MotorSpeed sMotorSpeed = {0.0,0.0};
MotorCmd sMotorConsi = {0.0,0.0};

MotorCmd sMotorRegu = {0.0,0.0};

//Variable contenant l'angle du Servo entre -0.95 et 0.65
float sServo_Angle=kServoDefault;


//-------------------------------------------------------------------------
// Variables et constantes utiles au monitoring
//-------------------------------------------------------------------------
// Taille des buffer RX et TX
#define kBUFFERSIZE 	500
#define kTxBUFFERSIZE	1000

// STBP message ID
#define Frame1_MSGID	1	// Identifacateur du 1er message transmis
#define Frame2_MSGID	2 // Identifacateur du 2e message transmis

// Variables utiles pour la transmission de donn�es
// selon le protocole ST et par WIFI
static CStbp sMyComm;													// Structure de contr�le d'un buffer
static CStbp * sMyCommPtr = &sMyComm;					// Pointeur sur la structure de contr�le
static UInt8 sMyCommRxBuffer[kBUFFERSIZE];		// Buffer contenant les donn�es brutes re�ues
static UInt8 sMyCommTxBuffer[kBUFFERSIZE];	// Buffer contenant les donn�es � transmettre
static UInt16 sNumberOfTransferredBytes = 0;	// Nb de byte � transmettre
UInt8 sTxMsgBuffer[kTxBUFFERSIZE] = {0};			// Buffer contenant les donn�es � transmettre
UInt8* sTxMsgBufferPtr = sTxMsgBuffer;				// Pointeur sur le Buffer contenant les donn�es � transmettre


// Structure de donn�es en r�ception WIFI --> doit-�tre identique de chaque c�t� de la transmission
typedef struct 
{
	int type;
	int temp;
	int time;
}CmdMenuStruct;

static CmdMenuStruct sFrameRxWifi;
static UInt8 *sFrameRxWifiPtr=(UInt8*)&sFrameRxWifi;
static Int16 sDlyJava;

// Test envoi de donn�es � l'app JAVA
void SendDataJava(void);

// Test r�ception de donn�es de l'app JAVA
void ReadDataJava(void);

// Init pour la comm de l'app JAVA
void InitDataJava(void);

//-------------------------------------------------------------------------
// Programme principal
//-------------------------------------------------------------------------
int main(void)
{
	static Int16 sDly;
	UInt32 i=0;
	static UInt16 aStatus;
	bool aRet;
	Int8 aChar[2];
	float aValueIntegration;
	Int8 aCharTab[50];
	static UInt16 sIntTime=25000;
	
	
	//--------------------------------------------------------------------
	// Initialisation du protocole ST pour la comm wifi
	//--------------------------------------------------------------------
	InitDataJava();
		
	//--------------------------------------------------------------------
	// Setup des p�riph�riques et de la carte
	//--------------------------------------------------------------------
	// Config de la PLL --> CPU 100MHz, bus et p�riph 50MHz
	mCpu_Setup();
	
	// Config et start des interrupteurs et poussoirs
	mSwitch_Setup();
	mSwitch_Open();
	
	// Config et start des Leds
	mLeds_Setup();
	mLeds_Open();
	
	// Config et start de l'ADC
	mAd_Setup();
	mAd_Open();
		
	// Config et start du SPI
	mSpi_Setup();
	mSpi_Open();
	
	// Config et start des delay non bloquant par PIT
	mDelay_Setup();
	mDelay_Open();
	
	// Config des timer pour la mesure de vitesse et les sorties PWM pour les servos
	mTimer_Setup();
	mTimer_Open();
	
	// Setup FXOS8700CQ 
	mAccelMagneto_Setup();
	mAccelMagneto_Open();
	
	// Config et start du DAC0 utilis� pour piloter le driver LED �clairage
	mDac_Setup();
	mDac_Open();
	
	// Setup et start de la commandes des PWMs moteur et servo ainsi que la mesure de vitesse
	mTimer_Setup();
	mTimer_Open();
	
	// UART 3 module configuration et start --> wifi autobaud
	// UART4 !!! Attention n�cessaire pour utiliser le RS232
	mWifi_Setup();
	mWifi_Open();
	
	// UART 4 monitoring image
	mRs232_Setup();
	mRs232_Open();
	
	// SPI0 --> cam�ra SPI --> reset + idle command
	mSpi_MLX75306_Reset();
	
	//--------------------------------------------------------------------
	//  Reset du pont en H
	//--------------------------------------------------------------------
	//	Table 1 Input Data Protocol
	//	Bit
	//	7 Status Register Reset: H = reset 
	//	6 OVLO: H = on, L = off
	//	5 not used
	//	4 MSB of 2bit chopper-OFF-time
	//	3 LSB of 2bit chopper-OFF-time
	//	2 PWM Operation mode: H = Fast decay, L = Slow decay
	//	1 MSB of 2 bit chopper current limit
	//	0 LSB of 2 bit chopper current limit
	aStatus= mSpi_SP0ReadWrite(kHBridgeA,0x80);
	aStatus= mSpi_SP0ReadWrite(kHBridgeB,0x80);
	//	Statut du pont en H
	//  Bit H = Error/L = no error
	//	7 Power supply fail
	//	6 not used, always H
	//	5 Short to VS or across the load
	//	4 Short to GND
	//	3 Open load
	//	2 MSB of Temperature Monitoring 
	//	1 LSB of Temperature Monitoring
	//	0 Error-Flag
	
	//--------------------------------------------------------------------
	// Init et calibration de l'acc�l�rom�tre et du magn�tom�tre
	//--------------------------------------------------------------------
#if(kWithAccel)
	// Init du FXOS8700CQ
	aRet=mAccelMagneto_Init();
	// Calibration de l'offset de l'acc�l�rom�tre
	aRet=mAccelMagneto_Accel_Calibration();
	// Calibration de l'offset du magn�tom�tre
	aRet=mAccelMagneto_Mag_Calibration();
#endif
	
	//--------------------------------------------------------------------
	// Config wifi, Adhoc --> Pour le Laptop se mettre en static IP
	// --> Laptop IP: 192.168.1.172
	// --> Subnet mask: 255.255.255.0
	// --> Gateway: 192.168.1.1
	// --> WEP, 12345ABCDE
	//--------------------------------------------------------------------
	// S�lection de la bande:
	
	aRet=mWifi_SendCmd("at+rsi_band=0\r\n");
	// Init:
	aRet=mWifi_SendCmd("at+rsi_init\r\n");
	// Network Type: AdHoc, canal 6
	// !! Chaque �quipe prend un canal diff�rent, ici 3
	aRet=mWifi_SendCmd("at+rsi_network=IBSS_SEC,1,3\r\n");
	// Set PSK: 
	aRet=mWifi_SendCmd("at+rsi_psk=12345ABCDE\r\n");
	// Create IBSS network: 
	// Chaque �quipe avec un non diff�rent
	aRet=mWifi_SendCmd("at+rsi_join=FREESCALE_CUP_JL,0,2\r\n");
	// IP Address Configuration (DHCP Manual): 
	aRet=mWifi_SendCmd("at+rsi_ipconf=0,192.168.1.176,255.255.255.0,192.168.1.1\r\n");
	// Open a server TCP socket in the module sur le port 5001
	aRet=mWifi_SendCmd("at+rsi_ltcp=5001\r\n");
	// On vide le buffer
	while(mWifi_SciReadDataFromBuffer(&aChar)==false);
	
	// Pour les tests
	sDly=mDelay_GetDelay(kPit1,500/kPit1Period);
	
	// Pour le test Wifi JAVA
	sDlyJava=mDelay_GetDelay(kPit1,100/kPit1Period);
	
	//Enclenchement de la lumi�re continuellement
	// Set DAC 0 buffer output value, entre 0 et 4095 --> driver de LEDS
	// Entre 0 et 100% --> 0 et 1.0
	mDac_SetDac0Output(1.0);
	
	for(;;)
		{	   	
			//--------------------------------------------------------------------
			// Selon la position de l'interrupteur 1 on passe de l'�tat test (si =1, voir ci-dessous) � l'�tat automatique
			//--------------------------------------------------------------------
			if(mSwitch_ReadSwitch(kSw1)==true)
				{
					//--------------------------------------------------------------------
					// Config et lecture des ponts en H
					// Slow decay mode --> freewheeling
					// Courant limit� � 4A typique
					// Chopper off time de 24us typique
					//--------------------------------------------------------------------
					aStatus= mSpi_SP0ReadWrite(kHBridgeA,0x00);
					aStatus= mSpi_SP0ReadWrite(kHBridgeB,0x00);
					//	Statut du pont en H
					//  Bit H = Error/L = no error
					//	7 Power supply fail
					//	6 not used, always H
					//	5 Short to VS or across the load
					//	4 Short to GND
					//	3 Open load
					//	2 MSB of Temperature Monitoring 
					//	1 LSB of Temperature Monitoring
					//	0 Error-Flag
					
					//--------------------------------------------------------------------
					// Start exposition � la lumi�re et lecture de l'image
					//--------------------------------------------------------------------
					//mSpi_MLX75306_StartIntegration(1);
					
#if(kWithAccel)
					//--------------------------------------------------------------------
					// Lecture de l'acc�l�rom�tre et du magn�tom�tre
					// Angle en degr�s et acc�l�ration en g
					//--------------------------------------------------------------------
					//aRet=mAccelMagneto_ReadData(&sAccel,&sMagneto,&sYaw,&sRoll,&sPitch);
#endif
					
					//-----------------------------------------------------------------------------
					// Lecture de la vitesse de rotation des moteurs
					// Moteur A = moteur gauche --> valeur n�gative = en arri�re, valeur pos=en avant
					// Moteur B = moteur droite
					//-----------------------------------------------------------------------------
					mTimer_GetSpeed(&sSpeedMotLeft,&sSpeedMotRight);
					
					// Set DAC 0 buffer output value, entre 0 et 4095 --> driver de LEDS
					// Entre 0 et 100% --> 0 et 1.0
					mDac_SetDac0Output(1.0);
					
					// ---------------------------------------------
					// Test envoi de donn�es � l'app JAVA
					// ---------------------------------------------
					//SendDataJava();
					
					// ---------------------------------------------
					// Test r�ception de donn�es de l'app JAVA
					// ---------------------------------------------
					//ReadDataJava();
				}
			
			//------------------------------------------------------------------
			// Mode test
			//------------------------------------------------------------------
			else
				{
					// Lecture de l'acc�l�rom�tre et du magn�tom�tre
					// Angle en degr�s et acc�l�ration en g
#if(kWithAccel)
					aRet=mAccelMagneto_ReadData(&sAccel,&sMagneto,&sYaw,&sRoll,&sPitch);
#endif
					
					//-----------------------------------------------------------------------------
					// Lecture de la vitesse de rotation des moteurs
					// Moteur A = moteur gauche (tr/mn)--> valeur n�gative = en arri�re, valeur pos=en avant
					// Moteur B = moteur droite (tr/mn)
					//-----------------------------------------------------------------------------
					
					
					// Selon la position des interrupteurs (interrupteur 2 et 3) on teste les poussoirs, le servo, les moteurs DC et la camera
					if(mDelay_IsDelayDone(kPit1,sDly)==true)
						{							
							// Test des LEDS
							mLeds_Toggle(kMaskLed1+kMaskLed2+kMaskLed3+kMaskLed4);
							
							mTimer_GetSpeed(&(sMotorSpeed.VitesseMoteurGauche),&(sMotorSpeed.VitesseMoteurDroite));
							
							// Tests des servo
							// Les 2 boutons poussoirs permettent de bouger dans un sens et dans l'autre
							if(mSwitch_ReadPushBut(kPushButSW1)==true)//Tourner � droite
								{
									sServo_Angle+=0.05;
									if(sServo_Angle>kServoMaxPosition)
										{
											sServo_Angle=kServoMaxPosition;
										}
									mTimer_SetServoDuty(0,sServo_Angle);
								}
							else if(mSwitch_ReadPushBut(kPushButSW2)==true)//Tourner � gauche
								{
									sServo_Angle-=0.05;
									if(sServo_Angle<kServoMinPosition)
										{
											sServo_Angle=kServoMinPosition;
										}
									mTimer_SetServoDuty(0,sServo_Angle);
								}
							else
								{
									//sServo_Angle=mTimer_ServoDefault;
									//mTimer_SetServoDuty(0,mTimer_ServoDefault);
								}
						
						// Si le switch 2 est ON les potentiom�tres pilotes les moteurs
						// sinon le pot1 pilote le temps d'exposition de la cam�ra
						// Commande du moteur, une valeur entre -1 et 1
						if(mSwitch_ReadSwitch(kSw2)==true)
							{
								// Test des moteurs
								// Pot2 moteur droit
								// Pot1 moteur gauche 
								sMotorConsi.MotorCmdLeft = 50 ;//(mAd_Read(kPot2))*100.0;
								sMotorConsi.MotorCmdRight = 50;//(mAd_Read(kPot1))*100.0;
								
								mTimer_GetSpeed(&sMotorSpeed.VitesseMoteurGauche,&sMotorSpeed.VitesseMoteurDroite);
								
								//mTimer_SetMotorDuty(mAd_Read(kPot1),mAd_Read(kPot2));
								
								mTimer_SetSameVitesseMotor(sMotorConsi, sMotorSpeed);
							}
						else
							{
								//sIntTime=mAd_ReadCamera(kPot1);
								mTimer_SetMotorDuty( 0, 0);//mAd_Read(kPot1)
							}
						

						//Quand j'active ce button la voiture va r�guler et avancer
						if(mSwitch_ReadSwitch(kSw4)){
								//Faire une lecture de la cam�ra.
								aValueIntegration = (mAd_Read(kPot1)+1)*3;
								
								mSpi_MLX75306_StartIntegration(aValueIntegration);//Une valeur de 0 -> 6
								mSpi_MLX75306_ReadPicture(sImageTab);
								
								for(i=0;i<129;i++){//142-13
										sImageDeriveTab[i] = abs(sImageTab[TabBegin+i*TabOffset]-sImageTab[TabBegin+i*TabOffset+TabOffset]);
								}
								
								mTimer_MotorMoveStraight(sImageDeriveTab,129, sMotorSpeed);
						}
						
						
						
						
						//Envoie de la vitesse sur l'UART4 pour l'afficer sur le terminal
						if(mSwitch_ReadSwitch(kSw3)){
								//sUartMessage // Buffer pour stocker le message
//								mRs232_Uart4WriteChar(0x1B);
//								mRs232_Uart4WriteChar('[');
//								mRs232_Uart4WriteChar('H');//Pour remmetre au d�but de la ligne
//								mDelay_DelayMs(20);
//								sprintf(sUartMessage, "Pot1: %.3f\tPot2: %.3f\n\rVitesse moteur Gauche: %.3f\n\rVitesse moteur droite: %.3f\n\rPosition du Servo: %.3f\n\rValeur de l'integration: %1.2f\n\n\r",mAd_Read(kPot1),mAd_Read(kPot2),sSpeedMotLeft,sSpeedMotRight,sServo_Angle,aValueIntegration);
//								mRs232_Uart4WriteString(sUartMessage);
//
//								//Efface tout le tableau sUartMessage et j'y mets comme premi�re �l�ment le nombre � l'indice 0
//								sprintf(sUartMessage,"%3d", sImageTab[0]);
//								for(i=1;i<(sizeof(sImageTab)-2);i++){
//										sprintf(aChar,",%3d", sImageTab[i]);
//										strcat(sUartMessage, aChar);
//								}
//								mRs232_Uart4WriteString("Ce que la camera voit:\n\r");
//								mRs232_Uart4WriteString(sUartMessage);
//								mRs232_Uart4WriteString("\n\n\r");
//								
//								for(i=0; i<sizeof(sUartMessage); i++){
//										sUartMessage[i]='\0';
//								}
//								
								
								//Calcul de la d�riv� !
								/*
								 * Pour cela, je vais uniquement prendre 156-16=140 points, je vais ignorer les 8 de chaque extr�mit�
								 * Calcul de la d�riv�: D�riv� = dy/dx, dans mon cas dx = 1 toujours.
								 * dy= Diff�rence entre 2 points
								 */

								//On initialise le tableau avec une valeur // 11 = 8+3, le 8 3 vient que les 3 premiers bytes ne me servent � rien
								/*sImageDeriveTab[0] =sImageTab[TabBegin]-sImageTab[TabBegin+TabOffset];
								//On initialise la le tableau pour envoyer le texte
								sprintf(sUartMessage,"%3d", sImageDeriveTab[0]);
								
								for(i=1;i<110;i++){
										sImageDeriveTab[i] = abs(sImageTab[TabBegin+i*TabOffset]-sImageTab[TabBegin+i*TabOffset+TabOffset]);
										sprintf(aChar,",%3d", sImageDeriveTab[i]);
										strcat(sUartMessage, aChar);
								}
								strcat(sUartMessage,"\n\r");
								mRs232_Uart4WriteString("Calcul de la derive sur 110 points:\n\r");
								mRs232_Uart4WriteString(sUartMessage);
								
								for(i=0; i<sizeof(sUartMessage); i++){
										sUartMessage[i]='\0';
								}			*/				
								
								
						}
						
							mDelay_ReStart(kPit1,sDly,100/kPit1Period);
							SendDataJava();																			
						}
					}
			}
	}



// -----------------------------------------------------------------------
// Structure de test pour monitoring JAVA
// -----------------------------------------------------------------------
typedef struct
{
	float SpeedLeft;
	float SpeedRight;
	Int16 ConsSpeedLeft;
	Int16 ConsSpeedRight;
	Int16 ConsServo;
	Int16 ConsLed;
	float ResFloat1;
	float ResFloat2;
	Int16 ResInt161;
	Int16 ResInt162;
}SendFrame1Struct;

static SendFrame1Struct sFrameTxJava1;
static SendFrame1Struct sFrameRxJava1;
static UInt8 						*sFrameRxPtr=(UInt8*)&sFrameRxJava1;

typedef struct
{
	float SpeedLeft;
	float SpeedRight;
	Int16 ConsSpeedLeft;
	Int16 ConsSpeedRight;
	float ConsServo;
	Int16 ConsLed;
	float ResFloat1;
	float ResFloat2;
	Int16 ResInt161;
	Int16 ResInt162;
	UInt8 ImageTab[143];
}SendFrame2Struct;

static SendFrame2Struct sFrameTxJava2;

//-----------------------------------------------------------------------
// Codage et envoi par WIFI � l'app JAVA
//-----------------------------------------------------------------------
void stbpSendFrameJava1(SendFrame1Struct *aFrameJava1)
{
	UInt8 msgId = Frame1_MSGID;
	UInt16 i=0;
	bool aRet=false;
	// Create a Message
	Stbp_NewMsg(sMyCommPtr, ESTBP_CONF_NOSUBHEADER,ESTBP_APPID_FreescaleCup,msgId); 
	Stbp_PutString(sMyCommPtr, (CHAR*) aFrameJava1, sizeof(SendFrame1Struct));
	
	// Encode the Whole Message 
	sNumberOfTransferredBytes += Stbp_EncodeMessage(sMyCommPtr, (UINT8*)(sTxMsgBufferPtr + sNumberOfTransferredBytes));
	
	// Envoi d'une donn�e par le WIFI: d�but de la commande AT
	aRet=mWifi_SendCmd("at+rsi_snd=1,0,0,0,");
	
	i=0;
	while(sNumberOfTransferredBytes>0)
		{
			if((sTxMsgBufferPtr[i]==0xd)&&(sTxMsgBufferPtr[i+1]==0xa))
				{
					sNumberOfTransferredBytes--;
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDC);
					i++;
					i++;
				}
			else if(sTxMsgBufferPtr[i]==0xDB)
				{
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDD);
					i++;
				}
			else if((sTxMsgBufferPtr[i]==0xDB)&&(sTxMsgBufferPtr[i+1]==0xDC))
				{
					
					sNumberOfTransferredBytes--;
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDD);
					mWifi_WriteChar(0xDC);
					i++;
					i++;
				}
			else
				{
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(sTxMsgBufferPtr[i]);
					i++;
				}
		}
	
	// N�cessaire pour terminer la commande AT
	mWifi_WriteChar(0xd);
	mWifi_WriteChar(0xa);
}

//-----------------------------------------------------------------------
// Codage et envoi par WIFI � l'app JAVA
//-----------------------------------------------------------------------
void stbpSendFrameJava2(SendFrame2Struct *aFrameJava2)
{
	UInt8 msgId = Frame2_MSGID;
	UInt16 i=0;
	bool aRet=false;
		
	// Create a Message
	Stbp_NewMsg(sMyCommPtr, ESTBP_CONF_NOSUBHEADER,ESTBP_APPID_FreescaleCup,msgId); 
//	Stbp_PutUint8(sMyCommPtr, gainMode);
//	Stbp_PutUint16(sMyCommPtr, rxGain);
//	Stbp_PutUint8(sMyCommPtr, ledsStatus);
	Stbp_PutString(sMyCommPtr, (CHAR*) aFrameJava2, sizeof(SendFrame2Struct));
	
	
	// Encode the Whole Message 
	sNumberOfTransferredBytes += Stbp_EncodeMessage(sMyCommPtr, (UINT8*)(sTxMsgBufferPtr + sNumberOfTransferredBytes));
	
	// Envoi d'une donn�e par le WIFI: d�but de la commande AT
	aRet=mWifi_SendCmd("at+rsi_snd=1,0,0,0,");
	
	i=0;
	while(sNumberOfTransferredBytes>0)
		{
			if((sTxMsgBufferPtr[i]==0xd)&&(sTxMsgBufferPtr[i+1]==0xa))
				{
					sNumberOfTransferredBytes--;
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDC);
					i++;
					i++;
				}
			else if(sTxMsgBufferPtr[i]==0xDB)
				{
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDD);
					i++;
				}
			else if((sTxMsgBufferPtr[i]==0xDB)&&(sTxMsgBufferPtr[i+1]==0xDC))
				{
					
					sNumberOfTransferredBytes--;
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(0xDB);
					mWifi_WriteChar(0xDD);
					mWifi_WriteChar(0xDC);
					i++;
					i++;
				}
			else
				{
					sNumberOfTransferredBytes--;
					mWifi_WriteChar(sTxMsgBufferPtr[i]);
					i++;
				}
		}
	
	// N�cessaire pour terminer la commande AT
	mWifi_WriteChar(0xd);
	mWifi_WriteChar(0xa);
}

//------------------------------------------------------------
// D�codage des trames RS232 de commande du PC Host
//------------------------------------------------------------
static UInt8 DecodeFrame(UInt8 *aBuffPtr,UInt8 aSize,UInt8 *aNewBuffPtr)
{
  bool aNext = false;
  UInt8 aNewSize=0;
  
  while(aSize>0)
    {
      if ((*aBuffPtr != 0x00) && (*aBuffPtr != 0x01))
        {
          if (aNext == false)
            {
              if (*aBuffPtr ==0x02)
                {
                  aNext = true;
                }
              else
                {
                  *aNewBuffPtr=*aBuffPtr;
                  *aNewBuffPtr-=3;
                  aNewBuffPtr++;
                  aNewSize++;
                }
            }
          else
            {
              *aNewBuffPtr=*aBuffPtr;
              aNewBuffPtr++;
              aNewSize++;
              aNext = false;
            }
        }
      
      aBuffPtr++;
      aSize--; 
    }
  
  return aNewSize; 
}

//-------------------------------------------------------------------------
// Test envoi de donn�es � l'app JAVA
//-------------------------------------------------------------------------
void SendDataJava(void)
{
	static bool sLedOn1=false;
	static UInt8 sSinIndex=0;
	UInt16 i;
		
	// ---------------------------------------------
	// Test envoi de donn�es � l'app JAVA
	// ---------------------------------------------
	if(mDelay_IsDelayDone(kPit1,sDlyJava))
		{
			if(sLedOn1==true)
				{
					sLedOn1=false;
				}
			else
				{
					sLedOn1=true;
				}
			
//			// ------------------------------------------------------------
//			// Test d'envoi d'une trame WIFI avec le protocole ST
//			// ------------------------------------------------------------
//			// On simule des signaux � l'aide d'une sinus par-exemple
//			sFrameTxJava1.SpeedLeft=sSinusTab[sSinIndex];
//			sFrameTxJava1.SpeedRight=sSinusTab[sSinIndex]*1.3;
//			sFrameTxJava1.ConsSpeedLeft+=50;
//			sFrameTxJava1.ConsSpeedRight+=60;
//			sFrameTxJava1.ConsServo+=1;
//			sFrameTxJava1.ConsLed+=2000;
//			sFrameTxJava1.sYaw=sSinusTab[sSinIndex]*1.5;
//			sFrameTxJava1.sRoll=sSinusTab[sSinIndex]*1.7;
//			sFrameTxJava1.sPitch=sSinusTab[sSinIndex]*2;
//			sFrameTxJava1.AccelX=sSinusTab[sSinIndex]*2.2;
//			sFrameTxJava1.AccelY=sSinusTab[sSinIndex]*2.4;
//			sFrameTxJava1.AccelZ=sSinusTab[sSinIndex]*2.6;
//			sFrameTxJava1.ResFloat1=sSinusTab[sSinIndex]*2.8;
//			sFrameTxJava1.ResFloat2=sSinusTab[sSinIndex]*3;
//			sFrameTxJava1.ResFloat3=sSinusTab[sSinIndex]*3.2;
//			sFrameTxJava1.ResFloat4=sSinusTab[sSinIndex]*3.4;
//			sFrameTxJava1.ResInt161+=12;
//			sFrameTxJava1.ResInt162+=22;
//			sFrameTxJava1.ResInt163=0;
//			sFrameTxJava1.ResInt164=0xFFFF;
//			
//			sSinIndex++;
//			if(sSinIndex>=25)
//				{
//					sSinIndex=0;
//				}
//			
//			// Codage et transmission de la trame
//			stbpSendFrameJava1(&sFrameTxJava1);
			
			// 2e frame avec l'image
			sFrameTxJava2.SpeedLeft= sMotorSpeed.VitesseMoteurGauche;
			sFrameTxJava2.SpeedRight= sMotorSpeed.VitesseMoteurDroite;
			sFrameTxJava2.ConsSpeedLeft=sMotorConsi.MotorCmdLeft;
			sFrameTxJava2.ConsSpeedRight=sMotorConsi.MotorCmdRight;
			sFrameTxJava2.ConsServo=sServo_Angle;
			sFrameTxJava2.ConsLed=0;
			sFrameTxJava2.ResFloat1=sMotorRegu.MotorCmdLeft;
			sFrameTxJava2.ResFloat2=sMotorRegu.MotorCmdRight;
			sFrameTxJava2.ResInt161=0;
			sFrameTxJava2.ResInt162=0;
			for(i=0;i<143;i++)
				{
					sFrameTxJava2.ImageTab[i]= sImageTab[i+13];
				}
			
			// Codage et transmission de la trame
			stbpSendFrameJava2(&sFrameTxJava2);
			
			// On envoi tous les 100ms
			mDelay_ReStart(kPit1,sDlyJava,100/kPit1Period);
		}
}

//-------------------------------------------------------------------------
// Test r�ception de donn�es de l'app JAVA
//-------------------------------------------------------------------------
void ReadDataJava(void)
{
	ReadFramestruct *aReadFrame;		
	UInt16 i=0;
	
	// Check si des donn�es ont �t� re�ues et lecture d'une trame
	aReadFrame=mRS232_GetReadWifiFrame();
	if(aReadFrame!=0)
		{
			// D�codage de la trame
			Stbp_DecodeMessage(sMyCommPtr, aReadFrame->Size, aReadFrame->Payload);
			sFrameRxWifiPtr=(UInt8*)&sFrameRxWifi;
			// Copie des donn�es re�ues et d�cod�es
			for(i=6;i<(sMyComm.uRxBufferHead-1);i++)
				{
					*sFrameRxWifiPtr++=sMyCommRxBuffer[i];
				}
		}
}

//-------------------------------------------------------------------------
// Init pour la comm de l'app JAVA
//-------------------------------------------------------------------------
void InitDataJava(void)
{
		Stbp_InitSw(sMyCommPtr, sMyCommRxBuffer,kBUFFERSIZE,sMyCommTxBuffer,kBUFFERSIZE);
}
