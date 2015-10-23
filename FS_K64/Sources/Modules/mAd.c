/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mAd.c
Author and date :	Monnerat Serge 28 févr. 2012

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mAd.h"
#include "../Interfaces/iAd.h"

//-----------------------------------------------------------------------
// Configuration du module ADC
//-----------------------------------------------------------------------
void mAd_Setup(void)
{
	iAd_Config();
	iAd_Cal();
	iAd_Config();
}

//-----------------------------------------------------------------------
// ADC module open
//-----------------------------------------------------------------------
void mAd_Open(void)
{}

//-----------------------------------------------------------------------
// ADC module close
//-----------------------------------------------------------------------
void mAd_Close(void)
{}


//-----------------------------------------------------------------------
// Start et lecture du résultat
// On retourne entre -1 et +1
//-----------------------------------------------------------------------
float mAd_Read(ADCInputEnum aAdcInp)
{
	UInt16 aRet;
	float aVal;
	
	switch(aAdcInp)
	{
		case kPot1:
			iAd_SelectChannel(kADC_SE3);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
		case kPot2:
			iAd_SelectChannel(kADC_SE1);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
	}
	
	// On retourne entre -1 et +1
	aVal=(aRet/-32768.0)+1.0;
	
	return aVal;
}

//-----------------------------------------------------------------------
// Start et lecture du résultat
// On retourne entre 0 et 65535
//-----------------------------------------------------------------------
float mAd_ReadCamera(ADCInputEnum aAdcInp)
{
	UInt16 aRet;
	
	switch(aAdcInp)
	{
		case kPot1:
			iAd_SelectChannel(kADC_SE3);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
		case kPot2:
			iAd_SelectChannel(kADC_SE1);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
	}

	return aRet;
}
