/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mRS232.c
Auteur et Date :	Monnerat Serge 26 janv. 2012

Description dans le fichier d'entête (.h)

-----------------------------------------------------------------------------
Historique:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mRS232.h"
#include "../Interfaces/iUart.h"

//-----------------------------------------------------------------------
// UART module configuration
//-----------------------------------------------------------------------
void mRs232_Setup(void)
{
}

//-----------------------------------------------------------------------
// UART module open
//-----------------------------------------------------------------------
void mRs232_Open(void)
{
}

//-----------------------------------------------------------------------
// UART 2 module close
//-----------------------------------------------------------------------
void mRs232_Close(void)
{
}

//-----------------------------------------------------------------------------
// Uart4, String send
// The transmission stop at the char NULL
// *aDataPtr	: string address
//-----------------------------------------------------------------------------
void mRs232_Uart4WriteString(UInt8 *aDataPtr)
{
	// Send char until NULL char
	while(*aDataPtr!=0)
		{
			// Wait end of transmit
			while(!iUart_Uart4GetStatus(kSciTransmitComplete));
	
			// Write data
			iUart_Uart4SetData(*aDataPtr);
			
			// Next char
			aDataPtr++;
		}
}

//-----------------------------------------------------------------------------
// Uart4 Byte send
// aData	: byte to send
//-----------------------------------------------------------------------------
void mRs232_Uart4WriteChar(UInt8 aData)
{
	// Wait end of transmit
	while(!iUart_Uart4GetStatus(kSciTransmitComplete));
		
	// Write data
	iUart_Uart4SetData(aData);	
}
