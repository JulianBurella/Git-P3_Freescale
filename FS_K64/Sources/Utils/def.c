/*
------------------------------------------------------------
Copyright 2003-2001x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	def.c
Author and date :	Monnerat Serge 7 mars 2014

Description in the header file iCpu.h

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "def.h"
#include "derivative.h"

void enable_irq (int irq)
{   
	/* Make sure that the IRQ is an allowable number. Up to 32 is 
	 * used.
	 *
	 * NOTE: If you are using the interrupt definitions from the header
	 * file, you MUST SUBTRACT 16!!!
	 */
	if ((irq >= 0)&&(irq<32))
		{
				/* Set the ICPR and ISER registers accordingly */
				NVICICPR0 |= 1 << (irq%32);
				NVICISER0 |= 1 << (irq%32);
		}
	else if ((irq >= 32)&&(irq<64))
		{
				/* Set the ICPR and ISER registers accordingly */
				NVICICPR1 |= 1 << (irq%32);
				NVICISER1 |= 1 << (irq%32); 
		}
	else if ((irq >= 64)&&(irq<96))
		{
				/* Set the ICPR and ISER registers accordingly */
				NVICICPR2 |= 1 << (irq%32);
				NVICISER2 |= 1 << (irq%32); 
		}
	else if ((irq >= 96)&&(irq<128))
		{
				/* Set the ICPR and ISER registers accordingly */
				NVICICPR3 |= 1 << (irq%32);
				NVICISER3 |= 1 << (irq%32); 
		}
}
/***********************************************************************/
/*
 * Initialize the NVIC to disable the specified IRQ.
 * 
 * NOTE: The function only initializes the NVIC to disable a single IRQ. 
 * If you want to disable all interrupts, then use the DisableInterrupts
 * macro instead. 
 *
 * Parameters:
 * irq    irq number to be disabled (the irq number NOT the vector number)
 */

void disable_irq (int irq)
{
    
	/* Make sure that the IRQ is an allowable number. Right now up to 32 is 
	 * used.
	 *
	 * NOTE: If you are using the interrupt definitions from the header
	 * file, you MUST SUBTRACT 16!!!
	 */
	if ((irq >= 0)&&(irq<32))
		{
			/* Set the ICER register accordingly */
			NVICICER0 = 1 << (irq%32);
		}
	else if ((irq >= 32)&&(irq<64))
		{
			/* Set the ICER register accordingly */
			NVICICER1 = 1 << (irq%32);
		}
	else if ((irq >= 64)&&(irq<96))
		{
			/* Set the ICER register accordingly */
			NVICICER2 = 1 << (irq%32);
		}
	else if ((irq >= 96)&&(irq<128))
		{
			/* Set the ICER register accordingly */
			NVICICER3 = 1 << (irq%32);
		}
}
/***********************************************************************/
/*
 * Initialize the NVIC to set specified IRQ priority.
 * 
 * NOTE: The function only initializes the NVIC to set a single IRQ priority. 
 * Interrupts will also need to be enabled in the ARM core. This can be 
 * done using the EnableInterrupts macro.
 *
 * Parameters:
 * irq    irq number to be enabled (the irq number NOT the vector number)
 * prio   irq priority. 0-3 levels. 0 max priority
 */

//void set_irq_priority (int irq, int prio)
//{   
//	/*irq priority pointer*/
//	UInt8 *prio_reg;
//	UInt8 err = 0;
//	UInt8 div = 0;
//	
//	/* Make sure that the IRQ is an allowable number. Right now up to 32 is 
//	 * used.
//	 *
//	 * NOTE: If you are using the interrupt definitions from the header
//	 * file, you MUST SUBTRACT 16!!!
//	 */
//	
//	/* Determine which of the NVICIPx corresponds to the irq */
//	div = irq / 4;
//	prio_reg = (UInt8 *)((uint32)&NVIC_IP(div));
//	/* Assign priority to IRQ */
//	*prio_reg = ( (prio&0x3) << (8 - ARM_INTERRUPT_LEVEL_BITS) );             
//}
