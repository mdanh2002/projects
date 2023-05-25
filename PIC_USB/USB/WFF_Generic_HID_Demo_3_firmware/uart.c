#include "uart.h"

void uart_tx_data(unsigned char data1)
{
	TXREG=data1; // Store data in Transmit register

	// the NOP is critical, otherwise the TXIF register will always be 1
	// and the code will not wait for the transmission to be done
	// http://www.microchip.com/forums/m608851-print.aspx
	// datasheet page 247 'Polling TXIF immediately following a load of TXREG will return invalid results.'
	Nop();

	while(PIR1bits.TXIF==0); // Wait until TXIF gets low
}

void uart_tx_string_ram(ram char* str, char withLineBreak)
{
    while(*str) uart_tx_data(*str++);

	if (withLineBreak)
	{
		uart_tx_data('\r');
		uart_tx_data('\n');
	}
}

void uart_tx_string_rom(const rom char* str, char withLineBreak)
{
    while(*str) 
	{
		uart_tx_data(*str++);
	}

	// line break
	if (withLineBreak)
	{
		uart_tx_data('\r');
		uart_tx_data('\n');
	}
}

unsigned char uart_rx_data(void)
{
	while(PIR1bits.RCIF==0); // Wait until RCIF gets low
	return RCREG; // Retrieve data from reception register
}

void uart_init(void)
{
	// init the UART module
	// http://roboticsguy.com/page/index.html/_/pic-microcontrollers/pic18-microcontrollers/pic18-rs-232-serial-communication-r22
    TXSTAbits.TX9 = 0;  // 8-bit transmission
    TXSTAbits.TXEN = 1; // transmit enabled
    TXSTAbits.SYNC = 0; // asynchronous mode
    TXSTAbits.BRGH = 1; // high speed
    RCSTAbits.SPEN = 1; // enable serial port - configures RX/DT and TX/CK pins as serial port pins
    RCSTAbits.RX9 = 0;  // 8-bit reception
    RCSTAbits.CREN = 1; // enable receiver
    BAUDCONbits.BRG16 = 0; // 8-bit baud rate generator
    SPBRG = 25; 			
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
}

// code from http://linuxgazette.net/issue65/padala.html\
// and http://www.termsys.demon.co.uk/vtansi.htm
// does not have full effect when tested with Tera Term
/*
void uart_setTerminalColor(int attr, int fg, int bg)
{	char command[13];

	// Command is the control command to the terminal
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);

	uart_tx_string_ram(command, 0);
}
*/