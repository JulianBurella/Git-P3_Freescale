/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mDelay.c
Author and date :	Monnerat Serge 29 août 2013

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mDelay.h"
#include "../Interfaces/iPit.h"

//-----------------------------------------------------------------------------
// Delay module setup
//-----------------------------------------------------------------------------
void mDelay_Setup(void)
{
	// --> PIT0 utilisé pour la caméra analogique
	// --> PIT1 utilisé par les delay non bloquant
	// --> PIT2 unused 
	// --> PIT3 unused 
	
	// PIT0 delays structure setup
	iPit_InitDelay(kPit1);
	// PIT0 setup
 	iPit_Config(kPit1,kPit1Period);
}

//-----------------------------------------------------------------------------
// Delay module open   
//-----------------------------------------------------------------------------
void mDelay_Open(void)
{
	iPit_StartPit(kPit1);
}

//-----------------------------------------------------------------------------
// Delay module close 
//-----------------------------------------------------------------------------
void mDelay_Close(void)
{
}

//------------------------------------------------------------
// Get one delay
// aPit		: Wich PIT
// aDelay	: delay time in ms
// Retour	: delay number or -1 if no more free delay
// 
//------------------------------------------------------------
Int16 mDelay_GetDelay(PitEnum aPit,UInt16 aDelay)
{
	return iPit_GetDelay(aPit,aDelay);
}

//------------------------------------------------------------
// Delay release
// aPit		: which PIT
// aDelayNb	: delay number
//------------------------------------------------------------
void mDelay_DelayRelease(PitEnum aPit,UInt16 aDelayNb)
{
	iPit_DelayRelease(aPit,aDelayNb);
}

//------------------------------------------------------------
// Delay restart
// aPit			: which PIT
// aDelay		: delay time
// aDelayNb	: delay number
//------------------------------------------------------------
void mDelay_ReStart(PitEnum aPit,UInt16 aDelayNb,UInt16 aDelay)
{
	iPit_ReStart(aPit,aDelayNb,aDelay);
}


//------------------------------------------------------------
// Check if the delay is done
// aPit			: which PIT
// aDelayNb	: delay number
// Retour		: flag state
//------------------------------------------------------------
bool mDelay_IsDelayDone(PitEnum aPit,UInt16 aDelayNb)
{
	return iPit_IsDelayDone(aPit,aDelayNb);
}

void mDelay_DelayMs(UInt16 aDelay){
		UInt16 aDelayNb = mDelay_GetDelay(kPit1,aDelay);
		while(!mDelay_IsDelayDone(kPit1,aDelayNb));
		mDelay_DelayRelease(kPit1,aDelayNb);
}
