/*
------------------------------------------------------------
Copyright 2003-2014 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iI2C.c
Author and date :	Monnerat Serge 7 mars 2014

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/
#include "iI2C.h"
#include "derivative.h"


#define BUSSPEED 		50000000
#define I2CBAUDRATE 400000

//------------------------------------------------------------
// I2C module config
//------------------------------------------------------------
void iI2C_Config(void)
{
	
	// I2C clock enable
	// System Clock Gating Control Register 4 (SIM_SCGC4)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 304
	SIM_SCGC4|=SIM_SCGC4_I2C0_MASK;
	
	// PTE24 --> SCL I2C0 --> Accéléromètre et magnétomètre FXOS8700CQ
	PORTE_PCR24 = 0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(5));
	// PTE25 --> SDA I2C0 --> Accéléromètre et magnétomètre FXOS8700CQ
	PORTE_PCR25 = 0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_ODE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(5));
	
	// PTB2 --> IO input --> interrupt INT1 FXOS8700CQ
	PORTB_PCR2 = 0|PORT_PCR_PS_MASK|PORT_PCR_ISF_MASK|PORT_PCR_IRQC(0xA)|(PORT_PCR_MUX(1));
	// PTB3 --> IO input --> interrupt INT2 FXOS8700CQ
	PORTB_PCR3 = 0|PORT_PCR_PS_MASK|(PORT_PCR_MUX(1));
	
	// Enable de l'interrupt INT1 de l'accéléromètre
	enable_irq(INT_PORTB-16);
				
	// Baud rate speed and I2C timing
	// I2C Frequency Divider register (I2Cx_F)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1457
	// I2C clock rate=390,625 kHz (max 400kHz)
	// SDA Hold = 0.42us (max 0.9us)
	// SCL start Hold = 1.16 us (min 0.6us)
	// SCL stop Hold = 1.3 us (min 0.6us
	I2C0_F=0;
	I2C0_F|=I2C_F_ICR(0x17)|I2C_F_MULT(0);
}


//------------------------------------------------------------
// I2C interface enable
//------------------------------------------------------------
void iI2C_Enable(void)
{

	// I2C0 enable
	// I2C Control Register 1 (I2Cx_C1)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
	I2C0_C1 |= I2C_C1_IICEN_MASK;
}

//------------------------------------------------------------
// I2C interface disable
//------------------------------------------------------------
void iI2C_stop(void)
{
	// I2C0 disable
	// I2C Control Register 1 (I2Cx_C1)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
	I2C0_C1 &= (~I2C_C1_IICEN_MASK);
}

//------------------------------------------------------------
// Set START state
//------------------------------------------------------------
void iI2C_SetStartState(void)
{
		// I2C Control Register 1 (I2Cx_C1)
		// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
		I2C0_C1 |= I2C_C1_MST_MASK;
}

//------------------------------------------------------------
// Set repeated START state
//------------------------------------------------------------
void iI2C_SetRepeatedStartSate(void)
{
		// I2C Control Register 1 (I2Cx_C1)
		// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
		I2C0_C1 |= I2C_C1_RSTA_MASK;
}

//------------------------------------------------------------
// Set STOP state
//------------------------------------------------------------
void iI2C_SetStopState(void)
{
		// I2C Control Register 1 (I2Cx_C1)
		// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
		I2C0_C1 &= (~I2C_C1_MST_MASK);
}

//------------------------------------------------------------
// Generate automatic ACK or not
//------------------------------------------------------------
void iI2C_SetAckMode(I2CAckEnum aAck)
{

	if (aAck == kAckAuto)
		{
			// I2C Control Register 1 (I2Cx_C1)
			// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
			I2C0_C1 &= (~I2C_C1_TXAK_MASK);
		}
	else if (aAck == kNoAck)
		{
			// I2C Control Register 1 (I2Cx_C1)
			// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1458
			I2C0_C1 |= I2C_C1_TXAK_MASK;
		}
}

//------------------------------------------------------------
// Select if we transmit or receive
//------------------------------------------------------------
void iI2C_TxRxSelect(I2CTransmiteModeEnum aMode)
{
	if (kTxMode == aMode)
		{
			// TX
			I2C0_C1 |= I2C_C1_TX_MASK;
		}
	else if (kRxMode == aMode)
		{
			// RX
			I2C0_C1 &= ~I2C_C1_TX_MASK;
		}
}

//------------------------------------------------------------
// Send a data
//------------------------------------------------------------
void iI2C_SendData(UInt8 aData)
{
	// I2C Data I/O register (I2Cx_D)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1461
	I2C0_D = aData;
}

//------------------------------------------------------------
// Wait End of transmit or receive
//------------------------------------------------------------
void iI2C_WaitEndOfRxOrTx(void)
{
	// Wait for IICIF flag
	// I2C Status register (I2Cx_S)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1460
	while ((I2C0_S & I2C_S_IICIF_MASK) == 0);
	// Clear the IICIF flag
	I2C0_S |= I2C_S_IICIF_MASK;
}

//------------------------------------------------------------
// Read received data, ! generate I2C clock if not in STOP mode
//------------------------------------------------------------
UInt8 iI2C_ReadData(void)
{
		UInt8 aReturnData = 0;
	
	// I2C Data I/O register (I2Cx_D)
	// K60 Sub-Family Reference Manual, Rev. 2 Jun 2012 page 1461
	aReturnData = I2C0_D;
	
	return aReturnData;
}

//------------------------------------------------------------
// Get I2C status flags
//------------------------------------------------------------
bool iI2C_ReadStatus(I2CStatusEnum aStatus)
{
	return (bool)(((I2C0_S&aStatus)==aStatus));
}

