/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iAd.c
Author and date :	Monnerat Serge 28 févr. 2012

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iAd.h"
#include "derivative.h"

//------------------------------------------------------------
// Configuration du convertisseur AD
//------------------------------------------------------------
void iAd_Config(void)
{	
	// Lets calibrate the ADC. 1st setup how the channel will be used.
	disable_irq(INT_ADC1-16);   
	
	// ADC configuration register 1 (ADCx_CFG1)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 757
	// ADLPC=0, Normal power configuration 
	// ADIV=0,The divide ratio is 1 and the clock rate is (input clock)/1
	// ADLSMP=0,short sample time
	// MODE=11,It is single-ended 16-bit conversion
	// ADICLK=0,Bus clock divided by 1 --> 50MHz
	ADC1_CFG1=0;
	ADC1_CFG1|=(ADC_CFG1_ADIV(0)|ADC_CFG1_MODE(3)|ADC_CFG1_ADICLK(0));
	
	// Configuration register 2 (ADCx_CFG2)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 759
	// MUXSEL=0,ADxxa channels are selected
	// ADACKEN=0,Asynchronous clock output disabled
	// ADHSC=1,high speed conversion sequence selected
	// ADLSTS=3,2 extra ADCK cycles; 6 ADCK cycles total sample time
	ADC1_CFG2=0|ADC_CFG2_ADHSC_MASK|ADC_CFG2_ADLSTS(3);
	
	// --> une conversion 5us --> 128 pixels: 640us
	
	// Status and control register 2 (ADCx_SC2)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 762
	// ADTRG=0,Software trigger selected
	// ACFE=0,Compare function disabled
	// ACFGT=0,Configures less than threshold, outside range not inclusive and inside range not inclusive functionality
	// based on the values placed in the CV1 and CV2 registers
	// ACREN=0,Range function disabled. Only the compare value 1 register (CV1) is compared
	// DMAEN=0,DMA is disabled
	// REFSEL=0,Default voltage reference pin pair (external pins VREFH and VREFL) 
	ADC1_SC2=0;
	
	// Status and control register 2 (ADCx_SC3)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 764
	// ADCO=0, One conversion or one set of conversions if the hardware average function is enabled (AVGE=1) after initiating a conversion.
	// AVGE=0, Hardware average function disabled
	// AVGS=0, 4 samples averaged
	ADC1_SC3=0|ADC_SC3_AVGE_MASK|ADC_SC3_AVGS(0);
		
	// ADC status and control registers 1 (ADCx_SC1n)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 754
	// AIEN=0,Conversion complete interrupt disabled
	// DIFF=0,Differential mode enable -> Single-ended conversions and input channels are selected
	// ADCH=0x03,Input channel select -> POT1
	ADC1_SC1A=0;
	ADC1_SC1A=0|(ADC_SC1_ADCH(3));
}

//------------------------------------------------------------
// Lecture du résultat
// Retour: le résultat
//------------------------------------------------------------
UInt16 iAd_GetResult(void)
{
	UInt16 aVal;
	
	// Lecture du résultat
	aVal=ADC1_RA;
	
	return aVal;
}

//------------------------------------------------------------
// Sélection du canal à convertir
// aCh: quel canal
//------------------------------------------------------------
void iAd_SelectChannel(ADCChannelEnum aCh)
{
	UInt16 aVal;
	
	// Clear ADCH field
	ADC1_SC1A&=(~ADC_SC1_ADCH_MASK);
	
	// Configuration du canal
	ADC1_SC1A|=(ADC_SC1_ADCH(aCh));		
}

//------------------------------------------------------------
// Lecture du drapeau de fin de conversion
// Retour: état du flag
//------------------------------------------------------------
bool iAd_GetCompleteConvStatus(void)
{
	bool aRet;
	
	// Lecture du flag
	aRet=(ADC1_SC1A&ADC_SC1_COCO_MASK)==ADC_SC1_COCO_MASK;
	
	return aRet;
}

//------------------------------------------------------------
// Calibration du convertisseur AD
// Retour: true = OK, false = KO
//------------------------------------------------------------
bool iAd_Cal(void)
{
  UInt16 aCalVar=0;
  bool aRet;
  
  // Max ADC clock -> 4MHz
	ADC1_CFG1&=((~ADC_CFG1_ADIV_MASK)&(~ADC_CFG1_ADICLK_MASK));
	ADC1_CFG1|=(ADC_CFG1_ADIV(3)|ADC_CFG1_MODE(1)|ADC_CFG1_ADICLK(1)|ADC_CFG1_ADLSMP_MASK);
	
	// Enable Software Conversion Trigger for Calibration Process
	ADC1_SC2&=(~ADC_SC2_ADTRG_MASK);
	// Set single conversion, clear avgs bitfield for next writing
	ADC1_SC3&= ((~ADC_SC3_ADCO_MASK) & (~ADC_SC3_AVGS_MASK)); 
	// Turn averaging ON and set at max value ( 32 )
	ADC1_SC3|= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3) );  
	// Start CAL
	ADC1_SC3 |= ADC_SC3_CAL_MASK ;      
	// Wait calibration end
	while ( (ADC1_SC1A & ADC_SC1_COCO_MASK ) == false ); 
	
	// Check for Calibration fail error and return
	if ((ADC1_SC3&ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK)
		{  
			aRet=false; 
			// Clear CAL bit
			ADC1_SC3&= (~ADC_SC3_CAL_MASK); 
		}
	else
		{
			// Calculate plus-side calibration
			aCalVar = 0x00;
			
			aCalVar =  ADC1_CLP0; 
			aCalVar += ADC1_CLP1;
			aCalVar += ADC1_CLP2;
			aCalVar += ADC1_CLP3;
			aCalVar += ADC1_CLP4;
			aCalVar += ADC1_CLPS;
			aCalVar = aCalVar/2;
			aCalVar |= 0x8000; // Set MSB
			ADC1_PG= ADC_PG_PG(aCalVar);
		
			// Calculate minus-side calibration
			aCalVar = 0x00;
		
			aCalVar =  ADC1_CLM0; 
			aCalVar += ADC1_CLM1;
			aCalVar += ADC1_CLM2;
			aCalVar += ADC1_CLM3;
			aCalVar += ADC1_CLM4;
			aCalVar += ADC1_CLMS;
			aCalVar = aCalVar/2;
			aCalVar |= 0x8000; // Set MSB
			ADC1_MG = ADC_MG_MG(aCalVar); 
			
			// Clear CAL bit
			ADC1_SC3&= (~ADC_SC3_CAL_MASK); 
			
			// Return OK
			aRet=true;  
		}
  		
  return aRet;
}
