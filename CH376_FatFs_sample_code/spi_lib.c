#include <stdio.h>
#include "config.h"
#include "spi_lib.h"
#include "uart.h"
#include <xc.h>

void spi1Init(unsigned int prescale, unsigned char ckp, unsigned char mode16){
    SPI1STATbits.SPIEN = 0; // disable the SPI module
 
    SPI1CON1bits.CKE = 0; // 0 = Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.CKP = ckp; //0 = Idle state for clock is a low level; active state is a high level
    SPI1CON1bits.DISSCK = 0; //0 = Internal SPI clock is enabled
    SPI1CON1bits.DISSDO = 0; //0 = SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = mode16; //0 = Communication is byte-wide (8 bits)
    SPI1CON1bits.MSTEN = 1;// 1 = Master mode
    SPI1CON1bits.SMP = 0; //0 = Input data sampled at middle of data output time
    SPI1CON1bits.SSEN = 0; // 0 = SSx pin not used by module.

	SPI1CON1 |= prescale;
	
    SPI1CON2bits.FRMDLY = 0; //0 = Frame sync pulse precedes first bit clock
	SPI1CON2bits.FRMEN = 0; //0 = Framed SPIx support disabled
    SPI1CON2bits.FRMPOL = 0; //0 = Frame sync pulse is active-low
    SPI1CON2bits.SPIFSD =0; // 0 = Frame sync pulse output (master)

    SPI1STATbits.SPIEN = 1; // enable the SPI module        
}	

unsigned char spi1Write( unsigned char i)
{
    SPI1BUF = i;    
   	while(!SPI1STATbits.SPIRBF);    
   	return SPI1BUF;
}

unsigned int spi1Write16(unsigned int i)
{
    SPI1BUF = i;
   	while(!SPI1STATbits.SPIRBF);    
   	return SPI1BUF;
}

void spi2Init(unsigned int prescale, unsigned char ckp, unsigned char mode16, unsigned char cke, unsigned char smp) {
    SPI2STAT = 0;
    SPI2STATbits.SPIEN = 0; // disable the SPI module
 
    SPI2CON1 = 0;
    SPI2CON1bits.CKE = cke; // 0 = Serial output data changes on transition from Idle clock state to active clock state
    SPI2CON1bits.CKP = ckp; //0 = Idle state for clock is a low level; active state is a high level
    SPI2CON1bits.DISSCK = 0; //0 = Internal SPI clock is enabled
    SPI2CON1bits.DISSDO = 0; //0 = SDOx pin is controlled by the module
    SPI2CON1bits.MODE16 = mode16; //0 = Communication is byte-wide (8 bits)
    SPI2CON1bits.MSTEN = 1;// 1 = Master mode
    SPI2CON1bits.SMP = smp; //0 = Input data sampled at middle of data output time
    SPI2CON1bits.SSEN = 0; // 0 = SSx pin not used by module.

	SPI2CON1 |= prescale;
	
    SPI2CON2bits.FRMDLY = 0; //0 = Frame sync pulse precedes first bit clock
	SPI2CON2bits.FRMEN = 0; //0 = Framed SPIx support disabled
    SPI2CON2bits.FRMPOL = 0; //0 = Frame sync pulse is active-low
    SPI2CON2bits.SPIFSD =0; // 0 = Frame sync pulse output (master)

    SPI2STATbits.SPIEN = 1; // enable the SPI module
}	

unsigned char spi2Write( unsigned char i)
{
    while (SPI2STATbits.SPITBF);     
    SPI2BUF = i;
   	while(!SPI2STATbits.SPIRBF);
   	return SPI2BUF;
}

unsigned int spi2Write16(unsigned int i, unsigned char waitUntilDone)
{
    SPI2BUF = i;
    
    if (waitUntilDone)
    {
        while(!SPI2STATbits.SPIRBF);    
        return SPI2BUF;
    }
    else {
        return 0;
    }
}
