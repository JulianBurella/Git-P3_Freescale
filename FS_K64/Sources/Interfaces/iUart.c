/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iUart.c
Author and date :	Monnerat Serge 10.09.2014

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iUart.h"
#include "derivative.h"

#define kUart3BaudRate 921600
#define kUart4BaudRate 460800

// 50 bytes buffer
#define kSciRecBufSize ((UInt8)(200)) 

// Nombre de trame en lecture 
#define kReadFrameNb 20

// RX buffer control struct
static struct
{
	Int8     SciRecBuf[kSciRecBufSize];
	UInt16 	  InIndex;
	UInt16 	  OutIndex;
	UInt16    ByteCount;
	bool      BufferIsFull;
}sSciRecBufStruct;

// Etat réception de données wifi
typedef enum
	{
		kWaitStart,
		kWaitType,
		kWaitSizeLsb,
		kWaitSizeMsb,
		kWaitPayload
	}ReadFrameStateEnum;

static ReadFrameStateEnum sReadWifiState=kWaitStart;

// Structure contenant une frame reçue par wifi
static ReadFramestruct sReadFrame[kReadFrameNb];
UInt16 sReadFrameInIndex=0;
UInt16 sReadFrameOutIndex=0;
UInt16 sReadFrameCounter=0;

//-----------------------------------------------------------------------
// Reset RX buffer
//-----------------------------------------------------------------------
void iUart_ResetBuffer(void)
{
	UInt16 i;
	
	sSciRecBufStruct.InIndex=0;
	sSciRecBufStruct.OutIndex=0;
	sSciRecBufStruct.ByteCount=0;
	sSciRecBufStruct.BufferIsFull=false;
	
	for(i=0;i<kSciRecBufSize;i++)
		{
			sSciRecBufStruct.SciRecBuf[i]=0;
		}
}

//-----------------------------------------------------------------------
// UART 3 module configuration --> wifi
//-----------------------------------------------------------------------
void iUart_Config(void)
{
	UInt16 aSbr;
	UInt8 aTmp;
	UInt16 aBrfa;
		
	// UART Control Register 2 (UARTx_C2)
	// Disable TX & RX before setup
	UART3_C2&= ~(UART_C2_TE_MASK|UART_C2_RE_MASK );
	
	// IRQ Config
	// IRQ bit offset in the following registers
	aTmp=37%32;
	// DUI0553A_cortex_m4_dgug.pdf page page 222 
	// Interrupt Clear-pending Registers
	// Clear prending interrupt
	NVICICPR1|=(1<<aTmp);
	// DUI0553A_cortex_m4_dgug.pdf page page 220
	// Interrupt Set-enable Registers
	// Interrupt enable
	NVICISER1|=(1<<aTmp);
	// DUI0553A_cortex_m4_dgug.pdf page page 223
	// Interrupt Priority Registers
	// Set interrupt priority
	NVICIP9&=0xFFFF0FFF;
			
	// UART Control Register 1 (UARTx_C1)
	// Loop Mode Select=0,Normal operation 
	// UARTSWAI=0, UART clock continues to run in wait mode
	// RSRC=0,Selects internal loop back mode and receiver input is internally connected to transmitter output
	// M=0, Normal - start + 8 data bits (MSB/LSB first as determined by MSBF) + stop
	// WAKE=0,Idle-line wakeup
	// ILT=0,Idle character bit count starts after start bit
	// PE=0,Parity function disabled
	// PT=0,Even parity 
	UART3_C1= 0;	/* We need all default settings, so entire register is cleared */
	
	// UART Baud Rate Registers:High (UARTx_BDH)
	// UART Baud Rate Registers: Low (UARTx_BDL)
	// Calculate baud rate settings
	aSbr = (UInt16)((kClockPeriphkHz*1000)/(kUart3BaudRate * 16));		
	// Save off the current value of the UARTx_BDH except for the SBR field
	aTmp = UART3_BDH & ~(UART_BDH_SBR(0x1F));
	UART3_BDH= aTmp | UART_BDH_SBNS_MASK |  UART_BDH_SBR(((aSbr & 0x1F00) >> 8));
	UART3_BDL= (UInt8)(aSbr & UART_BDL_SBR_MASK);
	
	// UART Control Register 4 (UARTx_C4)
	// BRFA, Baud Rate Fine Adjust
	// Determine if a fractional divider is needed to get closer to the baud rate
	aBrfa = (((kClockPeriphkHz*32000)/(kUart3BaudRate * 16)) - (aSbr * 32));
	// Save off the current value of the UARTx_C4 register except for the BRFA field */
	aTmp = UART3_C4 & ~(UART_C4_BRFA(0x1F));
	UART3_C4= aTmp | UART_C4_BRFA(aBrfa);    
	
	// UART FIFO Parameters (UARTx_PFIFO)
	// TXFE=1,Transmit FIFO is enabled. Buffer is depth indicted by TXFIFOSIZE
	// TXFIFOSIZE=3,Transmit FIFO/Buffer Depth = 16 Datawords 
	UART3_PFIFO=0;
	UART3_PFIFO|=(UART_PFIFO_TXFE_MASK|UART_PFIFO_TXFIFOSIZE(3));
	
	// UART FIFO Control Register (UARTx_CFIFO)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1283
	// Flush RX & TX fifo
	UART3_CFIFO|=(UART_CFIFO_TXFLUSH_MASK|UART_CFIFO_RXFLUSH_MASK);
	
	// UART Control Register 2 (UARTx_C2)
	// TIE=0, TDRE interrupt and DMA transfer requests disabled
	// TCIE=0, TC interrupt requests disabled
	// RIE=1,RDRF interrupt or DMA transfer requests enabled
	// ILIE=0,IDLE interrupt requests disabled
	// TE & RE =1, Enable receiver and transmitter
	UART3_C2=0;
	UART3_C2|= (UART_C2_TE_MASK|UART_C2_RE_MASK|UART_C2_RIE_MASK);
	
	// Enable de l'interruption
	//enable_irq(INT_UART3_RX_TX-16);
	//set_irq_priority (INT_UART3_RX_TX-16, 0);
			
	// UART Control Register 2 (UARTx_C2)
	// Disable TX & RX before setup
	UART4_C2&= ~(UART_C2_TE_MASK|UART_C2_RE_MASK );
	
	// IRQ Config
	// IRQ bit offset in the following registers
	aTmp=66%32;
	// DUI0553A_cortex_m4_dgug.pdf page page 222 
	// Interrupt Clear-pending Registers
	// Clear prending interrupt
	NVICICPR2|=(1<<aTmp);
	// DUI0553A_cortex_m4_dgug.pdf page page 220
	// Interrupt Set-enable Registers
	// Interrupt enable
	NVICISER2|=(1<<aTmp);
	// DUI0553A_cortex_m4_dgug.pdf page page 223
	// Interrupt Priority Registers
	// Set interrupt priority
	NVICIP16&=0xFF0FFFFF;
			
	// UART Control Register 1 (UARTx_C1)
	// Loop Mode Select=0,Normal operation 
	// UARTSWAI=0, UART clock continues to run in wait mode
	// RSRC=0,Selects internal loop back mode and receiver input is internally connected to transmitter output
	// M=0, Normal - start + 8 data bits (MSB/LSB first as determined by MSBF) + stop
	// WAKE=0,Idle-line wakeup
	// ILT=0,Idle character bit count starts after start bit
	// PE=0,Parity function disabled
	// PT=0,Even parity 
	UART4_C1= 0;	/* We need all default settings, so entire register is cleared */
	
	// UART Baud Rate Registers:High (UARTx_BDH)
	// UART Baud Rate Registers: Low (UARTx_BDL)
	// Calculate baud rate settings
	aSbr = (UInt16)((kClockPeriphkHz*1000)/(kUart4BaudRate * 16));		
	// Save off the current value of the UARTx_BDH except for the SBR field
	aTmp = UART4_BDH & ~(UART_BDH_SBR(0x1F));
	UART4_BDH= aTmp | UART_BDH_SBNS_MASK |  UART_BDH_SBR(((aSbr & 0x1F00) >> 8));
	UART4_BDL= (UInt8)(aSbr & UART_BDL_SBR_MASK);
	
	// UART Control Register 4 (UARTx_C4)
	// BRFA, Baud Rate Fine Adjust
	// Determine if a fractional divider is needed to get closer to the baud rate
	aBrfa = (((kClockPeriphkHz*32000)/(kUart4BaudRate * 16)) - (aSbr * 32));
	// Save off the current value of the UARTx_C4 register except for the BRFA field */
	aTmp = UART4_C4 & ~(UART_C4_BRFA(0x1F));
	UART4_C4= aTmp | UART_C4_BRFA(aBrfa);    
	
	// UART FIFO Parameters (UARTx_PFIFO)
	// TXFE=1,Transmit FIFO is enabled. Buffer is depth indicted by TXFIFOSIZE
	// TXFIFOSIZE=3,Transmit FIFO/Buffer Depth = 16 Datawords 
	UART4_PFIFO=0;
	UART4_PFIFO|=(UART_PFIFO_TXFE_MASK|UART_PFIFO_TXFIFOSIZE(3));
	
	// UART FIFO Control Register (UARTx_CFIFO)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1283
	// Flush RX & TX fifo
	UART4_CFIFO|=(UART_CFIFO_TXFLUSH_MASK|UART_CFIFO_RXFLUSH_MASK);
	
	// UART Control Register 2 (UARTx_C2)
	// TIE=0, TDRE interrupt and DMA transfer requests disabled
	// TCIE=0, TC interrupt requests disabled
	// RIE=1,RDRF interrupt or DMA transfer requests enabled
	// ILIE=0,IDLE interrupt requests disabled
	// TE & RE =1, Enable receiver and transmitter
	UART4_C2=0;
	UART4_C2|= (UART_C2_TE_MASK|UART_C2_RE_MASK|UART_C2_RIE_MASK);
	
	// Enable de l'interruption
	//enable_irq(INT_UART4_RX_TX-16);
	//set_irq_priority (INT_UART4_RX_TX-16, 0);
}

//------------------------------------------------------------
// Get Uart3 flags state
// aStatus: which flag to read
// retour	: flag state
//------------------------------------------------------------
bool iUart_GetStatus(UartStatusEnum aStatus)
{
	// UART Status Register 1 (UARTx_S1)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1267
	return ((UART3_S1&aStatus)==aStatus);
}

//------------------------------------------------------------
// Get Uart3 fifo flags state
// aStatus: which flag to read
// retour	: flag state
//------------------------------------------------------------
bool iUart_GetFifoStatus(UartFifoStatusEnum aStatus)
{
	// UART Status Register 1 (UARTx_S1)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1267
	return ((UART3_SFIFO&aStatus)==aStatus);
}


//------------------------------------------------------------
// Uart3 Data register write
// aData: datas to transmit
//------------------------------------------------------------
void iUart_SetData(UInt8 aData)
{
	// UART Data Register (UARTx_D)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1274
	UART3_D=aData;
}

//------------------------------------------------------------
// Check if the buffer is empty
// Return : true --> buffer empty, false --> buffer not empty
//------------------------------------------------------------
bool iUart_IsBufferEmpty(void)
{
	bool aRet=false;
	
	if(sSciRecBufStruct.ByteCount==0)
		{
			aRet=true;
		}
	else
		{
			aRet=false;
		}

	return aRet;
}

//------------------------------------------------------------
// Byte read in the RX buffer
// Return : oldest byte in the buffer
//------------------------------------------------------------
Int8 iUart_GetCharFromBuffer(void)
{
  Int8 aChar=0;
	
	// Byte read
	aChar=sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.OutIndex];
	
	// Index inc
	sSciRecBufStruct.OutIndex++;
	
	// Turning buffer
	if(sSciRecBufStruct.OutIndex>=kSciRecBufSize)
		{
			sSciRecBufStruct.OutIndex=0;
		}
	
	// Byte counter dec
	sSciRecBufStruct.ByteCount--;
	 
	// Retourne un byte du buffer
	return aChar;
}

//------------------------------------------------------------
// RX buffer command read
// Return : pointer on the frame
//------------------------------------------------------------
ReadFramestruct *iUart_GetReadWifiFrame(void)
{
	bool aRet=false;
	static ReadFramestruct *aFramePtr;
	
	if(sReadFrameOutIndex==sReadFrameInIndex)
	  {
	  
	  	return (void*)0;
	  }
	else
		{
			// Lecture du byte
			aFramePtr=&(sReadFrame[sReadFrameOutIndex]);
			// Incrémentation de l'index
			sReadFrameOutIndex++;
			
			sReadFrameCounter--;
			
			// Buffer tournant --> remise de l'index à zéro
			sReadFrameOutIndex%=kReadFrameNb;
						
			return aFramePtr; 	
		}
}

//---------------------------------------------------------------------------
// RX interrupt wifi
//---------------------------------------------------------------------------
// Trame: AT+RSI_READ, 1byte pour le type de trame (1:TCP), 2 bytes (d'abords LSB puis MSB) pour la taille des données, puis les données
// 	[0]		'O'				
//	[1]		'A'					
//	[2]		'T'					
//	[3]		'+'		
//	[4]		'R'		
//	[5]		'S'		
//	[6]		'I'		
//	[7]		'_'		
//	[8]		'R'		
//	[9]		'E'		
//	[10]	'A'		
//	[11]	'D'		
//	[12]	0x01 	--> TCP		
//	[13]	73		--> Taille LSB des données		
//	[14]	0x00	--> Taille MSB des données		
void UART3_Status_IRQHandler(void)
{
	Int8 aVal;
	static UInt8 sStatus;
	Int8 *aPtr=(Int8 *)0x4006D007;
	static bool sOK=false;
	
	sStatus=UART3_S1;
	
	// Read the RX byte
	aVal=*aPtr;
	
	if(sStatus & UART_S1_RDRF_MASK)
		{
			// Read the RX byte
			aVal=UART3_D;
			
			// Buffer not full flag
			sSciRecBufStruct.BufferIsFull=false;
			
			// Save received data
			sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex]=aVal;
			
			switch(sReadWifiState)
					{
						// Contrôle si l'on a reçu AT+RSI_READ
						case kWaitStart:
							if((sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex]=='D')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-1]=='A')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-2]=='E')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-3]=='R')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-4]=='_')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-5]=='I')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-6]=='S')&&
								(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex-7]=='R'))
								{
									sReadWifiState=kWaitType;
								}
								
						break;
						
						case kWaitType:
							sReadFrame[sReadFrameInIndex].Type=sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex];
							sReadWifiState=kWaitSizeLsb;
						break;
						case kWaitSizeLsb:
							sReadFrame[sReadFrameInIndex].Size=sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex];
							sReadWifiState=kWaitSizeMsb;
						break;
						case kWaitSizeMsb:
							sReadFrame[sReadFrameInIndex].Size|=(sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex]<<8);
							sReadFrame[sReadFrameInIndex].Index=0;
							sReadWifiState=kWaitPayload;
						break;
						case kWaitPayload:
							sReadFrame[sReadFrameInIndex].Payload[sReadFrame[sReadFrameInIndex].Index]=sSciRecBufStruct.SciRecBuf[sSciRecBufStruct.InIndex];
							sReadFrame[sReadFrameInIndex].Index++;
							if(sReadFrame[sReadFrameInIndex].Index==sReadFrame[sReadFrameInIndex].Size)
								{
									sReadFrameInIndex++;
									sReadFrameCounter++;
									sReadFrameInIndex%=kReadFrameNb;
									sReadWifiState=kWaitStart;
								}
						break;
					}

			// Index inc
			sSciRecBufStruct.InIndex++;
			
			// Bytes counter inc	
			sSciRecBufStruct.ByteCount++;
	
			// Turning buffer
			if(sSciRecBufStruct.InIndex>=kSciRecBufSize)
				{
					sSciRecBufStruct.InIndex=0;
				}
			}
		}

//------------------------------------------------------------
// Uart4 Data register write
// aData: datas to transmit
//------------------------------------------------------------
void iUart_Uart4SetData(UInt8 aData)
{
	// UART Data Register (UARTx_D)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1274
	UART4_D=aData;
}

//------------------------------------------------------------
// Get Uart4 flags state
// aStatus: which flag to read
// retour	: flag state
//------------------------------------------------------------
bool iUart_Uart4GetStatus(UartStatusEnum aStatus)
{
	// UART Status Register 1 (UARTx_S1)
	// K10 Sub-Family Reference Manual, Rev. 6, Nov 2011 page 1267
	return ((UART4_S1&aStatus)==aStatus);
}


