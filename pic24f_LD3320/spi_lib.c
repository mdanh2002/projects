/*
NVIS Inc.
Jianyi Liu
Created		Jul 1, 2010
Modified	Jul 1, 2010
source for spi device drivers
*/
#include "spi_lib.h"
#include <p24fj64ga002.h>
#include "delay.h"
#include "spi.h"

//write using selectable ports
unsigned char spiWrite( unsigned port, unsigned char i)
{
    switch(port){
        case 2:
            return spi2Write(i);
        default:
            return spi1Write(i);
    }		
}	

// send one byte of data and receive one back at the same time
unsigned char spi1Write( unsigned char i )
{
    // write to buffer for TX, wait for transfer, read
    SPI1BUF = i;

    while(!SPI1STATbits.SPIRBF);
    return SPI1BUF;
}//spiWrite2

void spiInit(unsigned char module, unsigned int prescale, unsigned char spiMode) {
/*
From Wikipedia: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus#Clock_polarity_and_phase

In addition to setting the clock frequency, the master must also configure the clock polarity and phase with respect to the data. 
Freescale's SPI Block Guide names these two options as CPOL and CPHA respectively, and most vendors have adopted that convention:

    At CPOL=0 the base value of the clock is zero
        For CPHA=0, data are captured on the clock's rising edge (low?high transition) and data is propagated on a falling edge (high?low clock transition).
        For CPHA=1, data are captured on the clock's falling edge and data is propagated on a rising edge.
    At CPOL=1 the base value of the clock is one (inversion of CPOL=0)
        For CPHA=0, data are captured on clock's falling edge and data is propagated on a rising edge.
        For CPHA=1, data are captured on clock's rising edge and data is propagated on a falling edge.

While most SPI devices operate on Mode 0, some do on Mode 3, such as the ILI9320 LCD controller.

On the PIC, the mode is changed by setting SPIXCON bits:

Bit 8 (CKE: SPIx Clock Edge Select bit) - CPHA bit
1 = Serial output data changes on transition from active clock state to Idle clock state (see bit 6)
0 = Serial output data changes on transition from Idle clock state to active clock state (see bit 6)

bit 6 (CKP:Clock Polarity Select bit) - CPOL bit
1 = Idle state for clock is a high level; active state is a low level
0 = Idle state for clock is a low level; active state is a high level

There are 4 available SPI modes:

Mode            CPOL            CPHA            CKP		CKE
0 		0 		0		0		1
1 		0 		1		0		0
2 		1 		0		1		1
3 		1 		1		1		0

Refer to PIC24FJ64GA002 datasheet page 140.
*/

	unsigned int modeConst = 0x00;
	switch(spiMode)
	{
		case 3:	
			modeConst = 0b001100000;
			break;
		case 2:	
			modeConst = 0b101100000;
			break;
		case 1:	
			modeConst = 0b000100000;
			break;
		default: // mode 0	
			modeConst = 0b100100000;
			break;
	}
	
	if (module == 2)
		OpenSPI2(modeConst | prescale, 0x0000, 0x8000);
	else
		OpenSPI1(modeConst | prescale, 0x0000, 0x8000);
}	

// send one byte of data and receive one back at the same time
unsigned char spi2Write( unsigned char i )
{
    // write to buffer for TX, wait for transfer, read
    SPI2BUF = i;
    while(!SPI2STATbits.SPIRBF);
    return SPI2BUF;
}//spiWrite2
