/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iDio.c
Author and date :	Monnerat Serge 7 mars 2014

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iDio.h"
#include "derivative.h"

//-----------------------------------------------------------------------------
// IO ports direction setup
// aPort: which IO port
// aMask: which port pin(s) to configure
// aDir:  input or output
//-----------------------------------------------------------------------------
void iDio_SetPortDirection(PortIOEnum aPort,IoMaskEnum aMask,IoDirectionEnum aDir)
{
	UInt32 *aPortPtr;
  
  // Port Data Direction Register (GPIOx_PDDR)
  switch(aPort)
    {
  		// GPIOA_PDDR
			case kPortA:
				aPortPtr=(UInt32 *)&GPIOA_PDDR;
			break;
			
  		// GPIOB_PDDR
      case kPortB:
        aPortPtr=(UInt32 *)&GPIOB_PDDR;
      break;
      
      // GPIOC_PDDR
			case kPortC:
				aPortPtr=(UInt32 *)&GPIOC_PDDR;
			break;
			
			// GPIOD_PDDR
			case kPortD:
				aPortPtr=(UInt32 *)&GPIOD_PDDR;
			break;
      // GPIOE_PDDR
      case kPortE:
        aPortPtr=(UInt32 *)&GPIOE_PDDR;
      break;
    }
    
  // IO direction setup 
  if(kIoOutput==aDir)
    {
      *aPortPtr|=aMask;
    }
  else if(kIoInput==aDir)
    {
      *aPortPtr &=~aMask;
    }
}

//-----------------------------------------------------------------------------
// Output pin set state
// aPort: which IO port
// aMask: which port pin(s) to configure
// aState:  output pin state
//-----------------------------------------------------------------------------
void iDio_SetPort(PortIOEnum aPort,IoMaskEnum aMask,IoStateEnum aState)
{
	UInt32 *aPortSetPtr;
	UInt32 *aPortClrPtr;
  
	// Port Data Output Register (GPIOx_PDOR)
	// Port Clear Output Register (GPIOx_PCOR)
	switch(aPort)
		{
			// GPIOA_PDOR & GPIOA_PCOR
			case kPortA:
				aPortSetPtr=(UInt32 *)&GPIOA_PDOR;
				aPortClrPtr=(UInt32 *)&GPIOA_PCOR;
			break;
			// GPIOB_PDOR & GPIOB_PCOR
			case kPortB:
				aPortSetPtr=(UInt32 *)&GPIOB_PDOR;
				aPortClrPtr=(UInt32 *)&GPIOB_PCOR;
			break;
			
			// GPIOC_PDOR & GPIOC_PCOR
			case kPortC:
				aPortSetPtr=(UInt32 *)&GPIOC_PDOR;
				aPortClrPtr=(UInt32 *)&GPIOC_PCOR;
			break;
			
			// GPIOD_PDOR & GPIOD_PCOR
			case kPortD:
				aPortSetPtr=(UInt32 *)&GPIOD_PDOR;
				aPortClrPtr=(UInt32 *)&GPIOD_PCOR;
			break;
			// GPIOE_PDOR & GPIOE_PCOR
			case kPortE:
				aPortSetPtr=(UInt32 *)&GPIOE_PDOR;
				aPortClrPtr=(UInt32 *)&GPIOE_PCOR;
			break;
		}
    
  // Set or reset the output
  if(kIoOn==aState)
    {
      *aPortSetPtr|=aMask;
    }
  else if(kIoOff==aState)
    {
      *aPortClrPtr|=aMask;
    }
}

//-----------------------------------------------------------------------------
// Read input pin state
// aPort: which IO port
// aMask: which port pin(s) to configure 
// return:  input pin state
//-----------------------------------------------------------------------------
bool iDio_GetPort(PortIOEnum aPort, IoMaskEnum aMask)
{
  bool  aRet=false;
  UInt32 *aPortPtr;
  
  // Port Data Input Register (GPIOx_PDIR)
	switch(aPort)
		{
			// GPIOA_PDIR
			case kPortA:
				aPortPtr=(UInt32 *)&GPIOA_PDIR;
			break;
			
			// GPIOB_PDIR
			case kPortB:
				aPortPtr=(UInt32 *)&GPIOB_PDIR;
			break;
			
			// GPIOC_PDIR
			case kPortC:
				aPortPtr=(UInt32 *)&GPIOC_PDIR;
			break;
			
			// GPIOD_PDIR
			case kPortD:
				aPortPtr=(UInt32 *)&GPIOD_PDIR;
			break;
			// GPIOE_PDIR
			case kPortE:
				aPortPtr=(UInt32 *)&GPIOE_PDIR;
			break;
		}
    
  // Input pin state
  aRet=(bool)((*aPortPtr&aMask)==aMask);
  
  return aRet;
}
