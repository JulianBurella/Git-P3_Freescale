/*
------------------------------------------------------------
Copyright 2003-2012 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iDac.c
Author and date :	Monnerat Serge 28 févr. 2012

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iDac.h"
#include "derivative.h"


// Set DAC register to reset value
static void iDac_SetResetVal (DacEnum aDac);


#define kDACx_SR_RESET 2
#define kDACx_C0_RESET 0
#define kDACx_C1_RESET 0
#define kDACx_C2_RESET 15 //0x0f

//------------------------------------------------------------
// DAC 0 & 1 setup
//------------------------------------------------------------
void iDac_Config(DacEnum aDac,DacModeEnum aMode)
{
	// Enable du clock du DAC
	SIM_SCGC2|=SIM_SCGC2_DAC0_MASK;
	
	switch(aMode)
		{
			case kNonBuffVRefIn:
				if(kDac0==aDac)
					{
						// Set DAC register to reset value
						iDac_SetResetVal (kDac0);
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 840
						// DAC Control Register (DACx_C0)
						// DACBBIEN =0 , buffer read pointer  bottom flag interrupt is disabled
						// DACBTIEN = 0 ,buffer read pointer top flag interrupt is disabled
						// DACBWIEN = 0, buffer water mark interrupt disabled
						// DACSWTRG=0, NO Software trigger yet 
						// LPEN = 0, high power mode
						// DACTRGSEL =1, Databuffer outputs to DACO pin whenever a write to DACDAT0
						// DACRFS=0,The DAC select DACREF_1 (1.2V) as the reference voltage,select VREF Output as reference
						// DACEN=1, DAC enabled
						DAC0_C0|=(DAC_C0_DACEN_MASK|DAC_C0_DACTRGSEL_MASK|DAC_C0_DACRFS_MASK); 
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 841
						// DAC Control Register (DACx_C1)
						// DMAEN=0, DMA disabled
						// DACBFWM=0, DAC buffer watermark select 1 words 
						// DACBFMD=0, DAC buffer work mode select -> Normal Mode
						// DACBFEN=0, Buffer read pointer disabled. The converted data is always the first word of the buffer.
						DAC0_C1=0;
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 842
						// DAC Control Register (DACx_C2)
						// DACBFRP=0,DAC buffer read pointer =0
						// DACBFUP=15,DAC buffer upper limit 
						DAC0_C2= (0|DAC_C2_DACBFRP(0)| DAC_C2_DACBFUP(0x0f));
					}
				else if(kDac1==aDac)
					{
						// Set DAC register to reset value
						iDac_SetResetVal (kDac1);
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 840
						// DAC Control Register (DACx_C0)
						// DACBBIEN =0 , buffer read pointer  bottom flag interrupt is disabled
						// DACBTIEN = 0 ,buffer read pointer top flag interrupt is disabled
						// DACBWIEN = 0, buffer water mark interrupt disabled
						// DACSWTRG=0, NO Software trigger yet 
						// LPEN = 0, high power mode
						// DACTRGSEL =1, Databuffer outputs to DACO pin whenever a write to DACDAT0
						// DACRFS=0,The DAC selets DACREF_1 as the reference voltage,select VREF Output as reference
						// DACEN=1, DAC enabled
						DAC1_C0|=(DAC_C0_DACEN_MASK|DAC_C0_DACTRGSEL_MASK); 
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 841
						// DAC Control Register (DACx_C1)
						// DMAEN=0, DMA disabled
						// DACBFWM=0, DAC buffer watermark select 1 words 
						// DACBFMD=0, DAC buffer work mode select -> Normal Mode
						// DACBFEN=0, Buffer read pointer disabled. The converted data is always the first word of the buffer.
						DAC1_C1=0;
						
						// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 842
						// DAC Control Register (DACx_C2)
						// DACBFRP=0,DAC buffer read pointer =0
						// DACBFUP=15,DAC buffer upper limit 
						DAC1_C2= (0|DAC_C2_DACBFRP(0)| DAC_C2_DACBFUP(0x0f));
					}
				
			break;
			
			case kBuffVRefIn:
				if(kDac0==aDac)
					{
						// Set DAC register to reset value
						iDac_SetResetVal (kDac0);
						
						//....
					}
				else if(kDac1==aDac)
					{
						// Set DAC register to reset value
						iDac_SetResetVal (kDac1);
						
						// ....
					}
				
			break;
		}

}

//------------------------------------------------------------
// Dac enable
// aDac: which dac (0 or 1)
//------------------------------------------------------------
void iDac_Enable (DacEnum aDac)
{
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 840
	// DAC Control Register (DACx_C0)
	// DACEN=1, DAC enabled
  switch(aDac)
		{
			case kDac0:
				DAC0_C0|=DAC_C0_DACEN_MASK;
			break;
			
			case kDac1:
				DAC1_C0|=DAC_C0_DACEN_MASK;
			break;
		}   
}

//------------------------------------------------------------
// Dac disable
// aDac: which dac (0 or 1)
//------------------------------------------------------------
void iDac_Disable (DacEnum aDac)
{
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 840
	// DAC Control Register (DACx_C0)
	// DACEN=1, DAC enabled
  switch(aDac)
		{
			case kDac0:
				DAC0_C0&=(~DAC_C0_DACEN_MASK);
			break;
			
			case kDac1:
				DAC1_C0&=(~DAC_C0_DACEN_MASK);
			break;
		}   
}

//------------------------------------------------------------
// Set DAC register to reset value
// aDac: which dac (0 or 1)
//------------------------------------------------------------
static void iDac_SetResetVal (DacEnum aDac)
{
  UInt8 i; 
  UInt16 *aPtr;
  
  switch(aDac)
		{
			case kDac0:
				aPtr=(UInt16*)&DAC0_DAT0L;
				for (i=0; i<16;i++,aPtr++)
					{
				   *aPtr=0;  
				  }
				DAC0_SR = kDACx_SR_RESET ;
				DAC0_C0 = kDACx_C0_RESET ;
				DAC0_C1 = kDACx_C1_RESET;
				DAC0_C2 = kDACx_C2_RESET;
			break;
			
			case kDac1:
				aPtr=(UInt16*)&DAC1_DAT0L;
				for (i=0; i<16;i++,aPtr++)
					{
					 *aPtr=0;  
					}
				DAC1_SR = kDACx_SR_RESET ;
				DAC1_C0 = kDACx_C0_RESET ;
				DAC1_C1 = kDACx_C1_RESET;
				DAC1_C2 = kDACx_C2_RESET;
			break;
		}   
}

//------------------------------------------------------------
// Set DAC buffer value
// aDac: which dac (0 or 1)
// aBuffIndex: which buffer position to configure
// aBuffVal: which value to set
//------------------------------------------------------------
void iDac_SETDACBUFFER(DacEnum aDac, UInt8 aBuffIndex, float aBuffVal)
{
   UInt16 *aPtr;
   UInt16 aVal;
   
   aVal=(UInt16)(1117.0*aBuffVal+372.0);
   
   if(aVal>1489)
  	 {
  		 aVal=1489;
  	 }
   if(aVal<372)
  	 {
  		 aVal=372;
  	 }
   
   switch(aDac)
   		{
   			case kDac0:
   				aPtr=(UInt16*)&DAC0_DAT0L;
   				aPtr+=aBuffIndex;
   				*aPtr=aVal&0x0fff;
   			break;
   			
   			case kDac1:
   				aPtr=(UInt16*)&DAC1_DAT0L;
   				aPtr+=aBuffIndex;
   				*aPtr=aVal&0x0fff;
   			break;
   		} 
 }
