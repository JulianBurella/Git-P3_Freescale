/*
------------------------------------------------------------
Copyright 2003-2014 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mCpu.c
Author and date :	Monnerat Serge 7 mars 2014

Description in the header file (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mCpu.h"
#include "../Interfaces/iCpu.h"

//-----------------------------------------------------------------------
// Low level CPU setup
//-----------------------------------------------------------------------
void mCpu_Setup(void)
{
	// PLL and crossbar configuration
	iCpu_SysInit();
}

