/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	iAd.h	
Auteur et Date :	Monnerat Serge 8.5.20xx

But : Interface du périphérique ADC 

Modifications
Date		Faite	Ctrl		Description
------------------------------------------------------------
*/

#ifndef __IAD__
#define __IAD__

#include "..\Utils\def.h"

// ADC channel enum
typedef enum
{
	kADC_SE0=0,
	kADC_SE1=1,
	kADC_SE2=2,
	kADC_SE3=3,
	kADC_SE4a=4,
	kADC_SE5a=5,
	kADC_SE6a=6,
	kADC_SE7a=7,
	kADC_SE4b=4,
	kADC_SE5b=5,
	kADC_SE6b=6,
	kADC_SE7b=7,
	kADC_SE8=8,
	kADC_SE9=9,
	kADC_SE11=11,
	kADC_SE12=12,
	kADC_SE13=13,
	kADC_SE14=14,
	kADC_SE15=15
}ADCChannelEnum;

//------------------------------------------------------------
// Configuration du convertisseur AD
//------------------------------------------------------------
void iAd_Config(void);

//------------------------------------------------------------
// Lecture du résultat
// Retour: le résultat
//------------------------------------------------------------
UInt16 iAd_GetResult(void);

//------------------------------------------------------------
// Sélection du canal à convertir
// aCh: quel canal
//------------------------------------------------------------
void iAd_SelectChannel(ADCChannelEnum aCh);

//------------------------------------------------------------
// Lecture du drapeau de fin de conversion
// Retour: état du flag
//------------------------------------------------------------
bool iAd_GetCompleteConvStatus(void);

//------------------------------------------------------------
// Calibration du convertisseur AD
// Retour: true = OK, false = KO
//------------------------------------------------------------
bool iAd_Cal(void);

#endif
