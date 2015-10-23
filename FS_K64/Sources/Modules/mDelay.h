/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	mDelay.h	
Author and date :	Monnerat Serge 29 août 2013

Goal : 

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/
#ifndef __mDelay__
#define __mDelay__

#include "..\Utils\def.h"

//-----------------------------------------------------------------------------
// Delay module setup
//-----------------------------------------------------------------------------
void mDelay_Setup(void);

//-----------------------------------------------------------------------------
// Delay module open   
//-----------------------------------------------------------------------------
void mDelay_Open(void);

//-----------------------------------------------------------------------------
// Delay module close 
//-----------------------------------------------------------------------------
void mDelay_Close(void);

//------------------------------------------------------------
// Get one delay
// aPit		: Wich PIT
// aDelay	: delay time in ms
// Retour	: delay number or -1 if no more free delay
// 
//------------------------------------------------------------
Int16 mDelay_GetDelay(PitEnum aPit,UInt16 aDelay);

//------------------------------------------------------------
// Delay release
// aPit		: which PIT
// aDelayNb	: delay number
//------------------------------------------------------------
void mDelay_DelayRelease(PitEnum aPit,UInt16 aDelayNb);

//------------------------------------------------------------
// Delay restart
// aPit			: which PIT
// aDelay		: delay time
// aDelayNb	: delay number
//------------------------------------------------------------
void mDelay_ReStart(PitEnum aPit,UInt16 aDelayNb,UInt16 aDelay);

//------------------------------------------------------------
// Check if the delay is done
// aPit			: which PIT
// aDelayNb	: delay number
// Retour		: flag state
//------------------------------------------------------------
bool mDelay_IsDelayDone(PitEnum aPit,UInt16 aDelayNb);

void mDelay_DelayMs(UInt16 aDelay);

#endif
