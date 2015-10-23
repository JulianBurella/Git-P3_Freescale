/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mWifi.c
Auteur et Date :	Monnerat Serge 10.9.2014

Description dans le fichier d'entête (.h)

-----------------------------------------------------------------------------
Historique:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mWifi.h"
#include "../Interfaces/iUart.h"
#include "../Interfaces/iPit.h"
#include "../Interfaces/iDio.h"

//-----------------------------------------------------------------------
// UART 3 module configuration
//-----------------------------------------------------------------------
void mWifi_Setup(void)
{
	iUart_Config();
}

//-----------------------------------------------------------------------
// UART 3 module open, wifi autobaud
//-----------------------------------------------------------------------
void mWifi_Open(void)
{
	Int16 aDelayNb=0;
	Int8 aData;
	UInt16 i;
	Int8 aDataTab[100];
	
	// Reset du wifi ON
	iDio_SetPort(kPortC,kMaskIo13,kIoOff);
	
	// On attend au min 20 ms pour le reset
	aDelayNb=iPit_GetDelay(kPit1,20/kPit1Period);
	while(iPit_IsDelayDone(kPit1,aDelayNb)==false);
	// Reset du wifi OFF
	iDio_SetPort(kPortC,kMaskIo13,kIoOn);
	
	// On attend 200ms après le power up
	iPit_ReStart(kPit1,aDelayNb,200/kPit1Period);
	while(iPit_IsDelayDone(kPit1,aDelayNb)==false);
				
	i=0;
	do
		{
			iUart_SetData(0x1C);
			// On attends 200ms
			iPit_ReStart(kPit1,aDelayNb,200/kPit1Period);
			while(iPit_IsDelayDone(kPit1,aDelayNb)==false);
			// Check si on a reçu la réponse après 200ms
			if(iUart_IsBufferEmpty()==false)
				{
					// Lecture de la données
					aData=iUart_GetCharFromBuffer();
					if (aData==0x55)
						{
							iUart_SetData(0x55);
							i=200;
						}
				}
			i++;
		}
	while(i<100);
	
	// On attends 5s
	iPit_ReStart(kPit1,aDelayNb,5000/kPit1Period);
	while(iPit_IsDelayDone(kPit1,aDelayNb)==false);
	
	i=0;
	while(mWifi_SciReadDataFromBuffer(&aData)==false)
	{
		aDataTab[i++]=aData;
	}
	
	// Relâche du delay
	iPit_DelayRelease(kPit1,aDelayNb);
}

//-----------------------------------------------------------------------
// UART 3 module close
//-----------------------------------------------------------------------
void mWifi_Close(void)
{
}

//-----------------------------------------------------------------------------
// String send
// The transmission stop at the char NULL
// *aDataPtr	: string address
//-----------------------------------------------------------------------------
void mWifi_WriteString(Int8 *aDataPtr)
{
	// Send char until NULL char
	while(*aDataPtr!=0)
		{
			// Wait end of transmit
			while(!iUart_GetStatus(kSciTransmitComplete));
	
			// Write data
			iUart_SetData(*aDataPtr);
			
			// Next char
			aDataPtr++;
		}
}

//-----------------------------------------------------------------------------
// Byte send
// aData	: byte to send
//-----------------------------------------------------------------------------
void mWifi_WriteChar(Int8 aData)
{
	// Wait end of transmit
	while(!iUart_GetStatus(kSciTransmitComplete));
		
	// Write data
	iUart_SetData(aData);	
}

//-----------------------------------------------------------------------------
// Buffer byte read. Rx interrupt fill the buffer
// *aBytePtr	  : pointer to return the byte
// Return 	    : true --> the buffer is empty, false the buffer is not empty
//-----------------------------------------------------------------------------
bool mWifi_SciReadDataFromBuffer(Int8 *aBytePtr)
{
  bool aRet=false;
  	
	// Contrôle si le buffer est vide
  aRet=iUart_IsBufferEmpty();
	
	// Si le buffer n'est pas vide --> lecture d'un byte
	if(false==aRet)
	  {
	    // Lecture d'un byte du buffer de réception
      *aBytePtr=iUart_GetCharFromBuffer();
	  }

	return aRet;
}

//-----------------------------------------------------------------------------
// Envoi d'une commande wifi
//-----------------------------------------------------------------------------
bool mWifi_SendCmd(Int8 *aDataPtr)
{
	Int8 aDataTab[20];
	Int8 aData;
	bool aRet=false;
	UInt16 i;
	static Int16 sSendDelayNb;
	
	// Envoi de la commande
	mWifi_WriteString(aDataPtr);
	
	// On prend un delay
	sSendDelayNb=iPit_GetDelay(kPit1,500/kPit1Period);
	i=0;
	do
		{	
			// Lecture du buffer de réception
			if (mWifi_SciReadDataFromBuffer(&aData)==false)
				{
					aDataTab[i++]=aData;
				}
			
			// Check si return et linefeed reçu
			if((aDataTab[i-1]==0xa)&&(aDataTab[i-2]==0xd)/*&&(aDataTab[i-3]=='K')&&(aDataTab[i-4]=='O')*/)
				{
					aRet=true;
					break;
				}
		}
	while((iPit_IsDelayDone(kPit1,sSendDelayNb)==false));
			
	// Relâche du delay
	iPit_DelayRelease(kPit1,sSendDelayNb);	
	
	return aRet;
}

//-----------------------------------------------------------------------------
// RX buffer commands read 
// *aFrame	  	: pointer on the RX frame
// Retour 	    : true -> the buffer is not empty
//-----------------------------------------------------------------------------
ReadFramestruct *mRS232_GetReadWifiFrame(void)
{
  bool aRet=false;
  static ReadFramestruct *aFrame;
  	
	aFrame=iUart_GetReadWifiFrame();

	return aFrame;
}
