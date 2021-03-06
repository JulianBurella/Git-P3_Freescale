/*
------------------------------------------------------------
Copyright 2003-2010 Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	mDac.h	
Author and date :	Monnerat Serge 28 f�vr. 2012

Goal : 

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/
#ifndef __mDac__
#define __mDac__

#include "..\Utils\def.h"

//-----------------------------------------------------------------------
// Dac 0 & 1 setup
//-----------------------------------------------------------------------
void mDac_Setup(void);

//-----------------------------------------------------------------------
// Dac 0 open
//-----------------------------------------------------------------------
void mDac_Open(void);

//-----------------------------------------------------------------------
// Set DAC 0 buffer output value
// Entre 0 et 1 --> 0 et 100%
//-----------------------------------------------------------------------
void mDac_SetDac0Output(float aVal);

//-----------------------------------------------------------------------
// Set DAC 1 buffer output value
// Entre 0 et 1 --> 0 et 100%
//-----------------------------------------------------------------------
void mDac_SetDac1Output(float aVal);

#endif
