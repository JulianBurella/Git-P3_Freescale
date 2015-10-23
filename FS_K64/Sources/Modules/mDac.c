/*
------------------------------------------------------------
Copyright 2003-2012 Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mDac.c
Author and date :	Monnerat Serge 28 f�vr. 2012

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mDac.h"
#include "../Interfaces/iDac.h"

//-----------------------------------------------------------------------
// Dac 0 setup
//-----------------------------------------------------------------------
void mDac_Setup(void)
{
	// DAC 0 setup
	iDac_Config(kDac0,kNonBuffVRefIn);
}

//-----------------------------------------------------------------------
// Dac 0 open
//-----------------------------------------------------------------------
void mDac_Open(void)
{}

//-----------------------------------------------------------------------
// Set DAC 0 buffer output value
// Entre 0 et 1 --> 0 et 100%
//-----------------------------------------------------------------------
void mDac_SetDac0Output(float aVal)
{
	// Set DAC buffer value
	iDac_SETDACBUFFER(kDac0,0,aVal);
}
