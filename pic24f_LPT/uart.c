#include "uart.h"
#include "delay.h"
#include <p24fj64ga002.h>

// http://homepage3.nifty.com/ARTWEB/pic1.files/T02-0000211c1.c

void InitUART()
{
// **************************
// (1)Define U1MODE
// **************************
    U1MODEbits.UARTEN = 0;        // Bit15 TX, RX DISABLED, ENABLE at end of func
    U1MODEbits.USIDL = 0;         // Bit13 Continue in Idle
    U1MODEbits.IREN = 0;          // Bit12 No IR translation
    U1MODEbits.RTSMD = 0;         // Bit11 Simplex Mode
    U1MODEbits.UEN0 = 0;          // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.UEN1 = 0; 
    U1MODEbits.WAKE = 0;          // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0;        // Bit6 No Loop Back
    U1MODEbits.ABAUD = 0;         // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.RXINV = 0;         // Bit4 IdleState = 1
    U1MODEbits.BRGH = 0;          // Bit3 16 clocks per bit period
    U1MODEbits.PDSEL1 = 0;        // Bits1,2 8bit, No Parity
    U1MODEbits.PDSEL0 = 0; 
    U1MODEbits.STSEL = 0;         // Bit0 One Stop Bit


// **************************
// (2)Define U1BRG
//     BAUDRATE(Register)=SYSCLK/32/BAUDRATE-1
// **************************
    U1BRG = (FOSC/32/57600-1);         // BAUDRATE 

// **************************
// (3)Define U1STA
// **************************
    U1STAbits.UTXISEL1 = 0;       //Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0;         //Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 0;       //Bit13 Other half of Bit15
    U1STAbits.UTXBRK = 0;         //Bit11 Disabled
    U1STAbits.UTXEN = 0;          //Bit10 TX pins controlled by periph
    U1STAbits.UTXBF = 0;          //Bit9 *Read Only Bit*
    U1STAbits.TRMT = 0;           //Bit8 *Read Only bit*
    U1STAbits.URXISEL0 = 0;       //Bits6,7 Int. on character recieved
    U1STAbits.URXISEL1 = 0; 
    U1STAbits.ADDEN = 0;          //Bit5 Address Detect Disabled
    U1STAbits.RIDLE = 0;          //Bit4 *Read Only Bit*
    U1STAbits.PERR = 0;           //Bit3 *Read Only Bit*
    U1STAbits.FERR = 0;           //Bit2 *Read Only Bit*
    U1STAbits.OERR = 0;           //Bit1 *Read Only Bit*
    U1STAbits.URXDA = 0;          //Bit0 *Read Only Bit*

    U1MODEbits.UARTEN = 1;        // And turn the peripheral on

    U1STAbits.UTXEN = 1;

}

void SendUART(char in_c)
{
    while(U1STAbits.UTXBF != 0);
    U1TXREG = in_c;
}

void RecUART(char *out_c)
{
    while(U1STAbits.URXDA == 0);
    *out_c = U1RXREG;
}

void SendUARTStr(char in_s[])
{
    unsigned int lp = 0;

    while(in_s[lp] != 0){
        SendUART(in_s[lp]);
        lp++; 
    }

	SendUART('\r');
	SendUART('\n');
}
