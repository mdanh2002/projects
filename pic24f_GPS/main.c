/*
Interface with the NEO6MV2 UART GPS Module
*/

#include <p24fj64ga002.h>
#include "delay.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "5110_lcd.h"
#include "adagps.h"
#include "util.h"

// http://dangerousprototypes.com/2012/04/26/bpv3-ftdi-uart-demo-wiki/
// for possible headers, refer to C:\Program Files (x86)\Microchip\MPLAB C30\support\PIC24F\h\p24FJ64GA002.h
_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_ON & POSCMOD_NONE & I2C1SEL_PRI) // Internal FRC OSC = 8 MHz x 4PLL = 32MHz, no output at RA3
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need

unsigned long timerCount = 0;
	
#define MODE_GPS_LOCATION_INFO 0 
#define MODE_GPS_DATETIME 1
	
unsigned char currentMode = MODE_GPS_LOCATION_INFO;
	
#define BUZZER_OUTPUT 		LATAbits.LATA4
#define BUTTON_1_ON		 	PORTAbits.RA4
#define BUTTON_2_ON		 	PORTBbits.RB6
#define BUTTON_3_ON		 	PORTBbits.RB7
#define BUTTON_4_ON		 	PORTBbits.RB8

void initIO()
{
	CLKDIVbits.RCDIV0 =0;     	// clock divider to 0 (no divider, PIC running at 8MHz internal oscillator speed)

	// connect to MAX232
	AD1PCFG = 0xFFFF;		    // all digital
	ODCB  = 0b0000000000000000; // No Open Drain Port B
	TRISB = 0b0000000111010100;	// PORTB for output, RB4 (UART 1 RX), RB2 (UART 2 RX), RB6, RB7, RB8 (button input with pulldown to 0V) 
	TRISA = 0b0000000000010000; // PORTA all output, except RA4 (button input with pulldown to 0V)
	ODCA  = 0b0000000000000000; // No Open Drain Port A 
	
	// interrupt 1 setup for push button
	RPINR0bits.INT1R = 6; // Assign interrupt 1 to RP6
	// Make interrupt 1 trigger on a rising edge
	// INTCON2bits.INT1EP = 0;
	INTCON2bits.INT1EP = 0;
	IFS1bits.INT1IF = 0;
	IPC5bits.INT1IP = 6;
	IEC1bits.INT1IE = 1;
}

void initUART()
{
	// configure UART Interrupt
	// IPC7bits.U2TXIP0 = 1;
	// IPC7bits.U2TXIP1 = 1;
	// IPC7bits.U2TXIP2 = 1;
	IPC7bits.U2RXIP0 = 1;
	IPC7bits.U2RXIP1 = 1;
	IPC7bits.U2RXIP2 = 1;
	// IEC1bits.U2TXIE = 1;
	IEC1bits.U2RXIE = 1;
	// IFS1bits.U1TXIF = 0;
	IFS1bits.U2RXIF = 0;
			
	// UART Module 1, for PC communication
	RPINR18bits.U1RXR = 4; // pin RP4 for RX
	RPOR1bits.RP3R   = 3;  // pin RP3, set to 3 for UART1 transmit
	InitUART();
	
	// UART Module 2, for GPS communication using NEO6MV2 module
	RPINR19bits.U2RXR = 2; // pin RP2 for RX, connected to TX of GPS module
	RPOR2bits.RP5R   = 5;  // pin RP5, set to 5 for UART2 transmit, connected to RX of GPS module
	
	// for testing UART2 GPS data feed from PC
	// RPINR19bits.U2RXR = 4; 	// pin RP4 for RX
	// RPOR1bits.RP3R   = 5;  // pin RP3, set to 5 for UART2 transmit		
	
	InitUART2();	
}

unsigned long lastPressed = 0;

void __attribute__((interrupt, auto_psv))  _INT1Interrupt(void) 
{ 
	IFS1bits.INT1IF = 0;
	
	// debouncing, no 2 presses within < 2 seconds
	if (timerCount - lastPressed < 256)
	{
		lastPressed = timerCount;
		return;	
	}
	
	lastPressed = timerCount;
				
	if (currentMode == MODE_GPS_LOCATION_INFO)
		currentMode = MODE_GPS_DATETIME;
	else
		currentMode = MODE_GPS_LOCATION_INFO;
}


// UART Receive Interrupt
void __attribute__((interrupt, no_auto_psv, shadow)) _U2RXInterrupt(void) {
	if (U2STAbits.OERR == 1) {
		U2STAbits.OERR = 0;
	} else {
	     char c = GPS_read();
	     SendUART(c);
	     // SendUART2(c);
	}
	
	IFS1bits.U2RXIF = 0;
}

// UART Transmit Interrupt
/*
void __attribute__((interrupt, no_auto_psv, shadow)) _U2TXInterrupt(void) {
IFS1bits.U1TXIF = 0;
} 
*/

void __attribute__((interrupt, auto_psv))  _T4Interrupt (void) 
{ 
	timerCount++;
	
	if (timerCount > 2147483648UL)
		timerCount = 0;
	
	_T4IF = 0; //Clear the Flag
}

void initTmr4()
{
	// enable timer 4 
	T4CONbits.TON = 0; // Disable Timer
	T4CONbits.TCS = 0; // Select internal instruction cycle clock
	T4CONbits.TGATE = 0; // Disable Gated Timer mode
	T4CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
	TMR4 = 0x00; // Clear timer register
	PR4 = 62500; // Load the period value, 256Hz
	IPC6bits.T4IP = 0x01; // Set Timer Interrupt Priority Level
	IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
	IEC1bits.T4IE = 1; // Enable Timer interrupt
	T4CONbits.TON = 1; // Start Timer 
}


int main()
{	
	char buf[25];
	
	initIO();
	initUART();
	initTmr4();
	
	/*
	while(1)
	{
		SCE = 1;
		RST = 1;
		D_C = 1;
		MOSI = 1;
		SCLK = 1;
		delay_ms(1);
		SCE = 0;
		RST = 0;
		D_C = 0;
		MOSI = 0;
		SCLK = 0;		
		delay_ms(1);
	}
	*/
	
	lcd_5110_init();
	lcd_5110_black();
	lcd_5110_cls();
	
	/*
	while(1)
	{
		sprintf(buf, "%d %d %d %d", BUTTON_1_ON, BUTTON_2_ON, BUTTON_3_ON, BUTTON_4_ON);
		lcd_string(buf, 1);
	}
	*/
	
	// poweron beep
	BUZZER_OUTPUT = 1;
	delay_ms(1000);
	BUZZER_OUTPUT = 0;
		
	// reset UART buffer in case it overflow during the poweron beep
	U2STAbits.OERR = 0;	
	
	// init the GPS Module
	GPS_common_init();
	
	// SendUARTStr("\n\nPIC24FJ64GA002 GPS Demo Starting Up.");
									
	lcd_string("Acquiring  GPS", 5);
	
	while(1)
	{
			if (GPS_newNMEAreceived())
			{
				char *nmea = GPS_lastNMEA();
				
				boolean isOK = GPS_parse(nmea);
				if (isOK)
				{																	
					GPS_SIGNAL_INFO info = GPS_getSignalInfo();	
					
					sprintf(buf, "Fix: %5d sat", info.satellites);
					lcd_string(buf, 5);			

					if (info.fix)
					{						
						if (currentMode == MODE_GPS_LOCATION_INFO)
						{
							GPS_LOCATION_INFO loc = GPS_getLocationInfo();
												
							sprintf(buf, "Lon: %9s", ftoa(loc.longitude / 100));												
							lcd_string(buf, 0);
	
							sprintf(buf, "Lat: %9s", ftoa(loc.latitude / 100));												
							lcd_string(buf, 1);
													
							sprintf(buf, "Alt: %5d   m", (int)loc.altitude);						
							lcd_string(buf, 2);
	
							sprintf(buf, "Spd: %5d  kt", (int)loc.speed);						
							lcd_string(buf, 3);
							
							sprintf(buf, "Hdn: %5d deg", (int)loc.angle);
							lcd_string(buf, 4);									
						}
						else
						{
							GPS_DATE_INFO date = GPS_getDateInfo();
							
							sprintf(buf, "   %2d/%2d/%2d   ", date.day, date.month, date.year);												
							lcd_string(buf, 0);
	
							sprintf(buf, "   %2d:%2d:%2d   ", date.hour, date.minute, date.seconds);												
							lcd_string(buf, 1);
							
							lcd_string("              ", 2);
							lcd_string("              ", 3);							
							lcd_string("              ", 4);														
						}	
					}					
				}	
			}	

			/*			
			if (BUTTON_1_ON)
			{
				currentMode = !currentMode;
				delay_ms(100); // debouncing
			}
			*/
	}
}
