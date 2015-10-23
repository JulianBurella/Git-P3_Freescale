/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	main.c
Author and date :	Monnerat Serge 29 août 2013

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

//Définitions de tests
#define TabBegin 25//11
#define TabOffset 1

// Tableau contenant l'image (et diverses infos) de la camera
// digitale, le tableau test est utilisé pour l'app Labview
static UInt8 sImageTab[150];
static UInt8 sImageDeriveTab[150];

static UInt8 sUartMessage[200];

// Mesure de l'accééromètre et du magnétomètre
static SRAWDATAEnum sAccel;				// en g
static SRAWDATAEnum sMagneto;			// en micro teslas
//static UInt8 sAccelMagnetoStatus;	
float sYaw;		// en degrés
float sRoll;	// en degrés
float sPitch;	// en degrés

// Mesure de vitesse et de sens
float sSpeedMotLeft;
float sSpeedMotRight;

//-------------------------------------------------------------------------
// Variables et constantes utiles au monitoring
//-------------------------------------------------------------------------
// Taille des buffer RX et TX
#define kBUFFERSIZE 	500
#define kTxBUFFERSIZE	1000

// STBP message ID
#define Frame1_MSGID	1	// Identifacateur du 1er message transmis
#define Frame2_MSGID	2 // Identifacateur du 2e message transmis

// Variables utiles pour la transmission de données
// selon le protocole ST et par WIFI
static CStbp sMyComm;													// Structure de contrôle d'un buffer
static CStbp * sMyCommPtr = &sMyComm;					// Pointeur sur la structure de contrôle
static UInt8 sMyCommRxBuffer[kBUFFERSIZE];		// Buffer contenant les données brutes reçues
static UInt8 sMyCommTxBuffer[kBUFFERSIZE];	// Buffer contenant les données à transmettre
static UInt16 sNumberOfTransferredBytes = 0;	// Nb de byte à transmettre
UInt8 sTxMsgBuffer[kTxBUFFERSIZE] = {0};			// Buffer contenant les données à transmettre
UInt8* sTxMsgBufferPtr = sTxMsgBuffer;				// Pointeur sur le Buffer contenant les données à transmettre


// Structure de données en réception WIFI --> doit-être identique de chaque côté de la transmission
typedef struct 
{
	int type;
	int temp;
	int time;
}CmdMenuStruct;
static CmdMenuStruct sFrameRxWifi;
static UInt8 *sFrameRxWifiPtr=(UInt8*)&sFrameRxWifi;
static Int16 sDlyJava;

// Test envoi de données à l'app JAVA
void SendDataJava(void);

// Test réception de données de l'app JAVA
void ReadDataJava(void);

// Init pour la comm de l'app JAVA
void InitDataJava(void);

//Add comment fort test of commit
//Maybe not enough
//I don't know if it is enough


//Tests sur l'ordi de Loïc


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
	float aDuty=mTimer_ServoDefault;
	float aValueIntegration;
	Int8 aCharTab[50];
	static UInt16 sIntTime=25000;
	static UInt16 VarInutile;
	
	//--------------------------------------------------------------------
	// Initialisation du protocole ST pour la comm wifi
	//--------------------------------------------------------------------
	InitDataJava();
		
	//--------------------------------------------------------------------
	// Setup des périphériques et de la carte
	//--------------------------------------------------------------------
	// Config de la PLL --> CPU 100MHz, bus et périph 50MHz
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
	
	// Config et start du DAC0 utilisé pour piloter le driver LED éclairage
	mDac_Setup();
	mDac_Open();
	
	// Setup et start de la commandes des PWMs moteur et servo ainsi que la mesure de vitesse
	mTimer_Setup();
	mTimer_Open();
	
	// UART 3 module configuration et start --> wifi autobaud
	// UART4 !!! Attention nécessaire pour utiliser le RS232
	mWifi_Setup();
	mWifi_Open();
	
	// UART 4 monitoring image
	mRs232_Setup();
	mRs232_Open();
	
	// SPI0 --> caméra SPI --> reset + idle command
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
	// Init et calibration de l'accéléromètre et du magnétomètre
	//--------------------------------------------------------------------
#if(kWithAccel)
	// Init du FXOS8700CQ
	aRet=mAccelMagneto_Init();
	// Calibration de l'offset de l'accéléromètre
	aRet=mAccelMagneto_Accel_Calibration();
	// Calibration de l'offset du magnétomètre
	aRet=mAccelMagneto_Mag_Calibration();
#endif
	
	//--------------------------------------------------------------------
	// Config wifi, Adhoc --> Pour le Laptop se mettre en static IP
	// --> Laptop IP: 192.168.1.172
	// --> Subnet mask: 255.255.255.0
	// --> Gateway: 192.168.1.1
	// --> WEP, 12345ABCDE
	//--------------------------------------------------------------------
	// Sélection de la bande:
	
	aRet=mWifi_SendCmd("at+rsi_band=0\r\n");
	// Init:
	aRet=mWifi_SendCmd("at+rsi_init\r\n");
	// Network Type: AdHoc, canal 6
	// !! Chaque équipe prend un canal différent, ici 6
	aRet=mWifi_SendCmd("at+rsi_network=IBSS_SEC,1,6\r\n");
	// Set PSK: 
	aRet=mWifi_SendCmd("at+rsi_psk=12345ABCDE\r\n");
	// Create IBSS network: 
	// Chaque équipe avec un non différent
	aRet=mWifi_SendCmd("at+rsi_join=FREESCALE_CUP_RL,0,2\r\n");
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
	
	for(;;)
		{	   	
			//--------------------------------------------------------------------
			// Selon la position de l'interrupteur 1 on passe de l'état test (si =1, voir ci-dessous) à l'état automatique
			//--------------------------------------------------------------------
			if(mSwitch_ReadSwitch(kSw1)==true)
				{
					//--------------------------------------------------------------------
					// Config et lecture des ponts en H
					// Slow decay mode --> freewheeling
					// Courant limité à 4A typique
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
					// Start exposition à la lumière et lecture de l'image
					//--------------------------------------------------------------------
					//mSpi_MLX75306_StartIntegration(1);
					
#if(kWithAccel)
					//--------------------------------------------------------------------
					// Lecture de l'accéléromètre et du magnétomètre
					// Angle en degrés et accélération en g
					//--------------------------------------------------------------------
					//aRet=mAccelMagneto_ReadData(&sAccel,&sMagneto,&sYaw,&sRoll,&sPitch);
#endif
					
					//-----------------------------------------------------------------------------
					// Lecture de la vitesse de rotation des moteurs
					// Moteur A = moteur gauche --> valeur négative = en arrière, valeur pos=en avant
					// Moteur B = moteur droite
					//-----------------------------------------------------------------------------
					mTimer_GetSpeed(&sSpeedMotLeft,&sSpeedMotRight);
					
					// Set DAC 0 buffer output value, entre 0 et 4095 --> driver de LEDS
					// Entre 0 et 100% --> 0 et 1.0
					mDac_SetDac0Output(1.0);
					
					// ---------------------------------------------
					// Test envoi de données à l'app JAVA
					// ---------------------------------------------
					//SendDataJava();
					
					// ---------------------------------------------
					// Test réception de données de l'app JAVA
					// ---------------------------------------------
					//ReadDataJava();
				}
			
			//--------------------------------------------------------------------
			// Mode test
			//--------------------------------------------------------------------
			else
				{
					// Lecture de l'accéléromètre et du magnétomètre
					// Angle en degrés et accélération en g
#if(kWithAccel)
					aRet=mAccelMagneto_ReadData(&sAccel,&sMagneto,&sYaw,&sRoll,&sPitch);
#endif
					
					//-----------------------------------------------------------------------------
					// Lecture de la vitesse de rotation des moteurs
					// Moteur A = moteur gauche (tr/mn)--> valeur négative = en arrière, valeur pos=en avant
					// Moteur B = moteur droite (tr/mn)
					//-----------------------------------------------------------------------------
					mTimer_GetSpeed(&sSpeedMotLeft,&sSpeedMotRight);
					
					// Set DAC 0 buffer output value, entre 0 et 4095 --> driver de LEDS
					// Entre 0 et 100% --> 0 et 1.0
					mDac_SetDac0Output(1.0);
					
					// Selon la position des interrupteurs (interrupteur 2 et 3) on teste les poussoirs, le servo, les moteurs DC et la camera
					if(mDelay_IsDelayDone(kPit1,sDly)==true)
						{							
							// Test des LEDS
							mLeds_Toggle(kMaskLed1+kMaskLed2+kMaskLed3+kMaskLed4);
							
							
							
							// Tests des servo
							// Les 2 boutons poussoirs permettent de bouger dans un sens et dans l'autre
							if(mSwitch_ReadPushBut(kPushButSW1)==true)//Tourner à droite
								{
									aDuty+=0.05;
									if(aDuty>mTimer_ServoMaxPosition)
										{
											aDuty=mTimer_ServoMaxPosition;
										}
									mTimer_SetServoDuty(0,aDuty);
								}
							else if(mSwitch_ReadPushBut(kPushButSW2)==true)//Tourner à gauche
								{
									aDuty-=0.05;
									if(aDuty<mTimer_ServoMinPosition)
										{
											aDuty=mTimer_ServoMinPosition;
										}
									mTimer_SetServoDuty(0,aDuty);
								}
							else
								{
									aDuty=mTimer_ServoDefault;
									//mTimer_SetServoDuty(0,mTimer_ServoDefault);
								}
						
						// Si le switch 2 est ON les potentiomètres pilotes les moteurs
						// sinon le pot1 pilote le temps d'exposition de la caméra
						// Commande du moteur, une valeur entre -1 et 1
						if(mSwitch_ReadSwitch(kSw2)==true)
							{
								// Test des moteurs
								// Pot2 moteur droit
								// Pot1 moteur gauche 
								mTimer_SetMotorDuty( mAd_Read(kPot2),mAd_Read(kPot1));//mAd_Read(kPot1)
							}
						else
							{
								//sIntTime=mAd_ReadCamera(kPot1);
								mTimer_SetMotorDuty( 0, 0);//mAd_Read(kPot1)
							}
						
						//Faire une lecture de la caméra.
						aValueIntegration = (mAd_Read(kPot1)+1)*3;
						if(mSwitch_ReadSwitch(kSw4)){
								mSpi_MLX75306_StartIntegration(aValueIntegration);//Une valeur de 0 -> 6
								mSpi_MLX75306_ReadPicture(sImageTab);
						}
						
						
						//Envoie de la vitesse sur l'UART4 pour l'afficer sur le terminal
						if(mSwitch_ReadSwitch(kSw3)){
								//sUartMessage // Buffer pour stocker le message
//								mRs232_Uart4WriteChar(0x1B);
//								mRs232_Uart4WriteChar('[');
//								mRs232_Uart4WriteChar('H');//Pour remmetre au début de la ligne
//								mDelay_DelayMs(20);
//								sprintf(sUartMessage, "Pot1: %.3f\tPot2: %.3f\n\rVitesse moteur Gauche: %.3f\n\rVitesse moteur droite: %.3f\n\rPosition du Servo: %.3f\n\rValeur de l'integration: %1.2f\n\n\r",mAd_Read(kPot1),mAd_Read(kPot2),sSpeedMotLeft,sSpeedMotRight,aDuty,aValueIntegration);
//								mRs232_Uart4WriteString(sUartMessage);
//
//								//Efface tout le tableau sUartMessage et j'y mets comme première élément le nombre à l'indice 0
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
								
								//Calcul de la dérivé !
								/*
								 * Pour cela, je vais uniquement prendre 156-16=140 points, je vais ignorer les 8 de chaque extrémité
								 * Calcul de la dérivé: Dérivé = dy/dx, dans mon cas dx = 1 toujours.
								 * dy= Différence entre 2 points
								 */

								//On initialise le tableau avec une valeur // 11 = 8+3, le 8 3 vient que les 3 premiers bytes ne me servent à rien
								sImageDeriveTab[0] =sImageTab[TabBegin]-sImageTab[TabBegin+TabOffset];
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
								}
								
//								mTimer_MotorMoveStraight(sImageDeriveTab,110);
								
								mDelay_ReStart(kPit1,sDly,100/kPit1Period);
						}
						
						
						SendDataJava();
						
						// Start exposition à la lumière
						//mSpi_MLX75306_StartIntegration_old(sIntTime);
														
						// Test de la caméra
						//mSpi_MLX75306_ReadPictureTest(sImageTabTest);
												 
						/*mRs232_Uart4WriteString("\r\n");
						mRs232_Uart4WriteString("L:");
						
						for(i=0;i<143;i++)
							{
								sprintf(aCharTab,"%X,",sImageTabTest[13+i]);
								mRs232_Uart4WriteString(aCharTab);
							}
									
						 for(i=0;i<143;i++)
							 {
								 sprintf(aCharTab,"%X",sImageTabTest[13+i]);
								 mRs232_Uart4WriteString(aCharTab);
								 if(i==143)
									 mRs232_Uart4WriteString("\r\n");
								 else
									 mRs232_Uart4WriteString(",");
							 }*/													
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
	Int16 ConsServo;
	Int16 ConsLed;
	float ResFloat1;
	float ResFloat2;
	Int16 ResInt161;
	Int16 ResInt162;
	UInt8 ImageTab[143];
}SendFrame2Struct;

static SendFrame2Struct sFrameTxJava2;

static float sSinusTab[25]=
{
	1,
	1.248689887,
	1.481753674,
	1.684547106,
	1.844327926,
	1.951056516,
	1.998026728,
	1.982287251,
	1.904827052,
	1.770513243,
	1.587785252,
	1.368124553,
	1.125333234,
	0.874666766,
	0.631875447,
	0.412214748,
	0.229486757,
	0.095172948,
	0.017712749,
	0.001973272,
	0.048943484,
	0.155672074,
	0.315452894,
	0.518246326,
	0.751310113
};

//-----------------------------------------------------------------------
// Codage et envoi par WIFI à l'app JAVA
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
	
	// Envoi d'une donnée par le WIFI: début de la commande AT
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
	
	// Nécessaire pour terminer la commande AT
	mWifi_WriteChar(0xd);
	mWifi_WriteChar(0xa);
}

//-----------------------------------------------------------------------
// Codage et envoi par WIFI à l'app JAVA
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
	
	// Envoi d'une donnée par le WIFI: début de la commande AT
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
	
	// Nécessaire pour terminer la commande AT
	mWifi_WriteChar(0xd);
	mWifi_WriteChar(0xa);
}

//------------------------------------------------------------
// Décodage des trames RS232 de commande du PC Host
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
// Test envoi de données à l'app JAVA
//-------------------------------------------------------------------------
void SendDataJava(void)
{
	static bool sLedOn1=false;
	static UInt8 sSinIndex=0;
	UInt16 i;
		
	// ---------------------------------------------
	// Test envoi de données à l'app JAVA
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
//			// On simule des signaux à l'aide d'une sinus par-exemple
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
			sFrameTxJava2.SpeedLeft=101.1;
			sFrameTxJava2.SpeedRight=102.2;
			sFrameTxJava2.ConsSpeedLeft=1050;
			sFrameTxJava2.ConsSpeedRight=1060;
			sFrameTxJava2.ConsServo=11000;
			sFrameTxJava2.ConsLed=12000;
			sFrameTxJava2.ResFloat1=109.9;
			sFrameTxJava2.ResFloat2=111.1;
			sFrameTxJava2.ResInt161=10111;
			sFrameTxJava2.ResInt162=10222;
			for(i=0;i<143;i++)
				{
					sFrameTxJava2.ImageTab[i]=i;
				}
			
			// Codage et transmission de la trame
			stbpSendFrameJava2(&sFrameTxJava2);
			
			// On envoi tous les 100ms
			mDelay_ReStart(kPit1,sDlyJava,100/kPit1Period);
		}
}

//-------------------------------------------------------------------------
// Test réception de données de l'app JAVA
//-------------------------------------------------------------------------
void ReadDataJava(void)
{
	ReadFramestruct *aReadFrame;		
	UInt16 i=0;
	
	// Check si des données ont été reçues et lecture d'une trame
	aReadFrame=mRS232_GetReadWifiFrame();
	if(aReadFrame!=0)
		{
			// Décodage de la trame
			Stbp_DecodeMessage(sMyCommPtr, aReadFrame->Size, aReadFrame->Payload);
			sFrameRxWifiPtr=(UInt8*)&sFrameRxWifi;
			// Copie des données reçues et décodées
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
