/*
------------------------------------------------------------
Copyright 2003-20014 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iCpu.c
Author and date :	Monnerat Serge 7 mars 2014

Description in the header file iCpu.h

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iCpu.h"
#include "derivative.h"

// Peripherals clock init
static void iCpu_PeriphClkInit(void);
	
// Core and peripheral clock init
static void iCpu_PllInit(void);

// K10 pin functions configuration
static void iCpu_PinFctCfg(void);

// Internal voltage reference init (VREF0)
static void iCpu_VREFInit(void);

//------------------------------------------------------------
// Core and peripheral clock init
// External clock = 8MHz 	(20MHz)
// Core clock = 100MHz		(100MHz)
// Peripheral bus = 50MHz	(50MHz)
// Flexbus = 50MHz
// Flash = 25MHz
//------------------------------------------------------------
static void iCpu_PllInit(void)
{
	// If the internal load capacitors are being used, they should be selected
	// before enabling the oscillator. Application specific. 16pF and 8pF selected
	// in this example
	//OSC_CR = OSC_CR_SC16P_MASK | OSC_CR_SC8P_MASK;
	
	// Enable external oscillator, RANGE=2, HGO=1, EREFS=1, LP=0, IRCS=0
	MCG_C2 = MCG_C2_RANGE0(2) | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK;
	
	// after initialization of oscillator release latched state of oscillator and GPIO
	//SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
	//LLWU_CS |= LLWU_CS_ACKISO_MASK;
	
	// Select external oscilator and Reference Divider and clear IREFS to start ext osc
	// CLKS=2, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
	MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
	
	// Wait for oscillator to initialize
	while (!(MCG_S & MCG_S_OSCINIT0_MASK));
	
	// Wait for Reference clock Status bit to clear
	while (MCG_S & MCG_S_IREFST_MASK); 
	
	// Wait for clock status bits to show clock source is ext ref clk
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2); 
	
	// Now in FBE
	
	// Configure PLL Ref Divider, PLLCLKEN=0, PLLSTEN=0, PRDIV=5
	// The crystal frequency is used to select the PRDIV value. Only even frequency crystals are supported
	// that will produce a 2MHz reference clock to the PLL.
	// 3 = divide factor -> 4 --> 8MHz/4 = 2MHz
	MCG_C5 = MCG_C5_PRDIV0(3); 
	
	// Ensure MCG_C6 is at the reset default of 0. LOLIE disabled, PLL disabled, clk monitor disabled, PLL VCO divider is clear
	//MCG_C6 = 0x0;
	
	// Set the VCO divider and enable the PLL for 100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26 (2MHz * 50)
	MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(26); 
	
	// Wait for PLL status bit to set
	while (!(MCG_S & MCG_S_PLLST_MASK)); 
	
	// Wait for LOCK bit to set
	while (!(MCG_S & MCG_S_LOCK0_MASK)); 
	
	// Set up the SIM clock dividers BEFORE switching to the PLL to ensure the
	// system clock speeds are in spec.
	// core = PLL (100MHz), bus = PLL/2 (50MHz), flexbus = PLL/2 (50MHz), flash = PLL/4 (25MHz)
	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1)| SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);
	
	// Now running PBE Mode
	
	// Transition into PEE by setting CLKS to 0
	// CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
	MCG_C1 &= ~MCG_C1_CLKS_MASK;
	
	// Wait for clock status bits to update
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);
	
	// Now running PEE Mode
	
	// Set the trace clock to the core clock frequency 
	SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;
	
	// Enable the clock to the FlexBus module
	SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
}

//------------------------------------------------------------
// K10 pin functions configuration
//------------------------------------------------------------
static void iCpu_PinFctCfg(void)
{
	//enable Clocks to all ports
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
	
	// Setup Pins as GPIO pour le HBridge
	PORTA_PCR15 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 	// H-bridge MA DIR
	PORTA_PCR16 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 	// H-bridge MBDIR
	PORTA_PCR17 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 	// H-bridge DIS  
	PORTB_PCR9 	= PORT_PCR_MUX(1)| PORT_PCR_DSE_MASK; 	// H-bridge INH
	
	// Port pour les poussoirs
	PORTC_PCR16 = PORT_PCR_MUX(1);   
	PORTC_PCR17 = PORT_PCR_MUX(1);   
	
	// Ports pour les interrupteurs
	PORTC_PCR9 = PORT_PCR_MUX(1); 
	PORTC_PCR10 = PORT_PCR_MUX(1);
	PORTC_PCR11 = PORT_PCR_MUX(1); 
	PORTC_PCR12 = PORT_PCR_MUX(1);
	
	// Setup des pins pour l'UART 3
	PORTB_PCR10 = 0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(0x3));
	PORTB_PCR11 = 0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(0x3));
	
	// Port pour les poussoirs
	PORTC_PCR14 = 0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(0x3));
	PORTC_PCR15 =  0|PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK|(PORT_PCR_MUX(0x3));  
		
	// Fonction PWM du FTM2
	PORTB_PCR18 = PORT_PCR_MUX(3);
	PORTB_PCR19 = PORT_PCR_MUX(3);  
	
	// Ports pour les LEDs
	PORTB_PCR20 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR21 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR22 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR23 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	
	// Point test
	// TP4
	PORTB_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	// TP5
	PORTB_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;
	// TP8
	PORTB_PCR16 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 
	// TP9
	PORTB_PCR17 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 
	
	// Config IO pour le reset du module wifi
	PORTC_PCR13 = PORT_PCR_MUX(1);
	
	// Config des IO pour le sens de rotation
	PORTA_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 	// Sens moteur A --> gauche
	PORTA_PCR14 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 	// Sens moteur B --> droite
	
	// Setup des IO en sortie
	GPIOC_PDDR |=  ((UInt32)(1<<13));
	
	// Disable du pont en H
	GPIOA_PSOR |= ((UInt32)(1<<17));
	
	// Reset du wifi OFF
	GPIOC_PSOR |= ((UInt32)(1<<13));
}

//------------------------------------------------------------
// Peripherals clock init
//------------------------------------------------------------
void iCpu_PeriphClkInit(void)
{
	// System Clock Gating Control Register 1 (SIM_SCGC1)
	// Enable clock UART4 --> monitoring
	SIM_SCGC1|=SIM_SCGC1_UART4_MASK;
	
	// System Clock Gating Control Register 2 (SIM_SCGC2)
	// Enable du Clock du DAC0 --> LEDs
	SIM_SCGC2|=SIM_SCGC2_DAC0_MASK;
				
	// System Clock Gating Control Register 3 (SIM_SCGC3)
	// Enable du Clock du FTM2 --> Servo et ADC1
	SIM_SCGC3 |= (SIM_SCGC3_FTM2_MASK|SIM_SCGC3_ADC1_MASK); 
	SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;
	
	// System Clock Gating Control Register 4 (SIM_SCGC4)
	// UART1 3 clock enable --> wifi
	// I2C0 enable accéléromètre
	SIM_SCGC4|=(SIM_SCGC4_I2C0_MASK|SIM_SCGC4_UART3_MASK);
	
	// System Clock Gating Control Register 6 (SIM_SCGC6)
	// Clock enable for ADC0, PIT, FTM0 et FTM1
	SIM_SCGC6|=(SIM_SCGC6_DAC0_MASK|SIM_SCGC6_ADC0_MASK|SIM_SCGC6_PIT_MASK|SIM_SCGC6_SPI0_MASK|SIM_SCGC6_FTM0_MASK|SIM_SCGC6_FTM1_MASK);
	
	// System Clock Gating Control Register 7 (SIM_SCGC7)
	// Clock enable for MPU,DMA, FLEXBUS
	SIM_SCGC7|=(SIM_SCGC7_DMA_MASK|SIM_SCGC7_MPU_MASK);
}

//------------------------------------------------------------
// Internal voltage reference init (VREF0)
//------------------------------------------------------------
static void iCpu_VREFInit(void)
{
	UInt8 *aVREFTRM=(UInt8*)(0x40074000);
	
	// VREF Status and Control Register (VREF_SC)
	// Voltage Reference enabled,VREF bandgap and tight regulation buffer on
	// VREF_OUT available for internal and external use
  VREF_SC |= (VREF_SC_VREFEN_MASK|(VREF_SC_MODE_LV(2)));
  
	*aVREFTRM|=0x3f;
	
	// Wait till the VREFSC is stable
	while (!(VREF_SC & VREF_SC_VREFST_MASK));
}


//------------------------------------------------------------
// CPU configuration
//------------------------------------------------------------
void iCpu_SysInit(void)
{
	// Peripherals clock init
	iCpu_PeriphClkInit();
	
	// Disable MPU
	MPU_CESR&=(~MPU_CESR_VLD_MASK);
	
	// Core and peripheral clock init
	iCpu_PllInit();
	
	// K10 pin functions configuration
	iCpu_PinFctCfg();
	
	// Internal voltage reference init (VREF0)
	iCpu_VREFInit();
}
