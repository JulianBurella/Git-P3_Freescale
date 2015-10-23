/*
------------------------------------------------------------
Copyright 2003-201x Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	mRS232.h	
Auteur et Date :	Monnerat Serge 10.09.2014

But : 

-----------------------------------------------------------------------------
Historique:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/
#ifndef __mWifi__
#define __mWifi__

#include "..\Utils\def.h"

//-----------------------------------------------------------------------------
// RS232 module setup   
//-----------------------------------------------------------------------------
void mWifi_Setup(void);

//-----------------------------------------------------------------------------
// RS232 module close       
//-----------------------------------------------------------------------------
void mWifi_Open(void);

//-----------------------------------------------------------------------------
// RS232 module close   
//-----------------------------------------------------------------------------
void mWifi_Close(void);

//-----------------------------------------------------------------------------
// String send
// The transmission stop at the char NULL
// *aDataPtr	: string address
//-----------------------------------------------------------------------------
void mWifi_WriteString(Int8 *aDataPtr);

//-----------------------------------------------------------------------------
// Byte send
// aData	: byte to send
//-----------------------------------------------------------------------------
void mWifi_WriteChar(Int8 aData);

//-----------------------------------------------------------------------------
// Envoi d'une commande wifi
//-----------------------------------------------------------------------------
bool mWifi_SendCmd(Int8 *aDataPtr);

//-----------------------------------------------------------------------------
// Buffer byte read. Rx interrupt fill the buffer
// *aBytePtr	  : pointer to return the byte
// Return 	    : true --> the buffer is empty, false the buffer is not empty
//-----------------------------------------------------------------------------
bool mWifi_SciReadDataFromBuffer(Int8 *aBytePtr);

//-----------------------------------------------------------------------------
// RX buffer commands read 
// *aFrame	  	: pointer on the RX frame
// Retour 	    : true -> the buffer is not empty
//-----------------------------------------------------------------------------
ReadFramestruct *mRS232_GetReadWifiFrame(void);

#endif
