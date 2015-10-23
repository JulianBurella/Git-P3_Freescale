/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	def.h	
Auteur et Date :	Monnerat Serge 29 août 2013

But : Some general settings

-----------------------------------------------------------------------------
Historique:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#ifndef DEF_H_
#define DEF_H_



//------------------------------------------------------------
// Constants
//------------------------------------------------------------
// Core clock
#define kClockCpu 				100.0 			// 100MHz
#define kClockCpukHz 			100000 			// 100MHz
#define kClockPeriphkHz 	50000 			// 50MHz
#define CORE_CLOCK		(100000000)	
#define PERIPHERAL_BUS_CLOCK 	(CORE_CLOCK/2)

// Caméra digitale ou analogique
#define kDigCamera 1
// PIT1 period --> 1 ms
#define kPit1Period 1 
// Avec ou sans accéléromètre, magnéto
#define kWithAccel 0

//------------------------------------------------------------
// Interrupt priority (O.highest, 16 lowest)
//------------------------------------------------------------
#define kPIT_IntPriority 	0x30	// Priority 3

//------------------------------------------------------------
// bool and standard type
//------------------------------------------------------------
typedef int bool;
typedef bool BOOL;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

typedef unsigned char				UInt8;  /*  8 bits */
typedef unsigned short int	UInt16; /* 16 bits */
typedef unsigned long int		UInt32; /* 32 bits */
typedef unsigned long long	UInt64; /* 32 bits */

typedef signed char			    			Int8;   /*  8 bits */
typedef signed short int	        Int16;  /* 16 bits */
typedef signed int		            Int32;  /* 32 bits */

typedef volatile Int8				VInt8;  /*  8 bits */
typedef volatile Int16			VInt16; /* 16 bits */
typedef volatile Int32			VInt32; /* 32 bits */
typedef long long		        Int64;  /* 32 bits */

typedef volatile UInt8			VUInt8;  /*  8 bits */
typedef volatile UInt16			VUInt16; /* 16 bits */
typedef volatile UInt32			VUInt32; /* 32 bits */

typedef volatile UInt8			VUInt8;  /*  8 bits */
typedef volatile UInt16			VUInt16; /* 16 bits */
typedef volatile UInt32			VUInt32; /* 32 bits */
typedef volatile UInt32     UINT32;
typedef volatile UInt8      UINT8;
typedef volatile Int8       INT8;
typedef volatile UInt16     UINT16;
typedef volatile UInt64     UINT64;
typedef volatile Int64      INT64;
typedef volatile Int16      INT16;
typedef volatile Int32      INT32;
typedef volatile Int8       CHAR;

// Macros
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

//------------------------------------------------------------
// Type
//------------------------------------------------------------
// PIT enum
typedef enum
{
	kPit0=0,
	kPit1=1,
	kPit2=2,
	kPit3=3
}PitEnum;

// Structure contenant une frame reçue par wifi
typedef struct
{
	UInt8 Type;
	UInt16 Size;
	UInt8 Payload[300];
	UInt16 Index;
}ReadFramestruct;

void enable_irq (int irq);

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

void disable_irq (int irq);

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

void set_irq_priority (int irq, int prio);

#endif
