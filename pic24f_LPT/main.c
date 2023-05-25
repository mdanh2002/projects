#include <p24fj64ga002.h>
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "uart.h"
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"
#include "colors.h"
#include "st7735.h"
#include "sd_helper.h"

/*
This project tries to simulate a parallel port printer.

Connections:

Parallel port pin 2-pin 9 (DB0-DB7) - to RB8..RB15
Strobe (pin 1) - to RB3 (from computer to device)
Acknowledge (pin 10) - to RB2 (from device to computer)
Busy (pin 11) - to RA0 (from device to computer)

Paper End (pin 12) - to GND (indicates no lack of paper)
nError (pin 15) - to 5V (No Error)
Select (pin 13) - to 5V (Printer Ready)

Autofeed (pin 14), Init (pin 16), SELIN (pin 17) ignored

References:

http://courses.engr.illinois.edu/ece390/books/labmanual/parallel-comm.html
http://www.arunet.co.uk/tkboyd/ele1pp3.htm
http://pinouts.ru/ParallelPorts/ParallelPC_pinout.shtml

Interestingly, If printer ERROR pin is connected to 5V (no error) but set to offline, Tektronix 1230 Logic Analyzer will not boot up at all!
Even Power indicator will not be ON, making it look like that the device fails
*/                                                                                                                                                                                                                                                                                                             

#define LPT_STROBE PORTBbits.RB3
#define LPT_ACK LATBbits.LATB2
#define LPT_BUSY LATAbits.LATA0

// http://dangerousprototypes.com/2012/04/26/bpv3-ftdi-uart-demo-wiki/
_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_ON & POSCMOD_NONE & I2C1SEL_PRI) // Internal FRC OSC = 8 MHz x 4PLL = 32MHz
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need

/*
** _FBS (Register FBS: 0xF80000)
**  BWRP_OFF - Boot Segment Write Protect Disable
**  BSS_OFF  - Boot Segment Code Protect Disable
** --------------------------------------------------------------------
** _FGS (Register FGS: 0xF80004)
**  GWRP_OFF - General Segment Write Protect Disable
**  GCP_OFF  - General Segment Code Protect Disable
** --------------------------------------------------------------------
** _FOSCSEL (Register FOSCSEL: 0xF80006)
**  FNOSC_FRCDIV - Fast RC Oscillator With Postscale (FRCDIV)
**  FNOSC_FRCPLL - Fast RC Oscillator With PLL 
**  IESO_OFF - Two Speed Start-up Disable
** --------------------------------------------------------------------
** _FOSC (Register FOSC: 0xF80008)
**  FCKSM_CSECMD - Clock Switching Enabled, Clock Monitor Disabled
**  POSCFREQ_MS  - Mid Speed (100kHz - 8MHz)
**  OSCIOFNC_ON  - OSCO Pin Has Digital I/O Function (RA3)
**  POSCMOD_NONE - Primary disabled
**  SOSCSEL_SOSCLP - Low Power Secondary Oscillator
** --------------------------------------------------------------------
** _FWDT (Register FWDT (0xF8000A)
**  FWDTEN_OFF - Watchdog Timer (WDT) Disable
**  WINDIS_OFF - Non-Window WDT Mode
**  FWPSA_PR128 - WDT Prescale 1:128
**  WDTPS_PS32768 - WDT Postscale 1:32,768
** --------------------------------------------------------------------
** _FPOR (Register FPOR: 0xF8000C)
**  MCLRE_OFF - MCLR Disabled, RA5 Enabled
**  BORV_LPBOR - Low Power BOR (Brown Out Voltage)
**  BOREN_BOR3 - Enabled in hardware, SBOREN bit disabled
**  I2C1SEL_PRI - Use SCL1/SDA1 Pins For I2C1
**  PWRTEN_OFF - Power Up Timer Disabled
** --------------------------------------------------------------------
** _FICD (Register FICD: 0xF8000E)
**  BKBUG_OFF - Background Debugger Disabled
**  ICS_PGx3 - EMUC/EMUD share PGC3/PGD3
** --------------------------------------------------------------------
** _FDS (Register FDS: 0xF80010)
**  DSWDTEN_OFF - Deep Sleep WDT Disabled
**  DSBOREN_ON - Deep Sleep BOR Enabled
**  RTCOSC_SOSC - RTCC Oscillator select: Secondary Oscillator (SOSC)
**  DSWDTOSC_SOSC - Deep Sleep WDT Low Power RC Oscillator (LPRC)
**  DSWDTPS_DSWDTPSF - Deep Sleep WDT Postscale 1:2,147,483,648
**                     (25.7 Days)
** --------------------------------------------------------------------
*/

unsigned long timerCount = 0;
unsigned long cardFree = 0;
unsigned long lastStrobe = 0;
FSFILE * pointer = NULL;
BOOL isPrinting = FALSE;
int totalJobs = 0;
char lastFilename[50];
unsigned long totalBytesRecv = 0;
unsigned long accuBytesRecv = 0;
BOOL isSDCardReady = FALSE;

void showPrinterStats()
{
	char buf[100];
	
	ST7735_fillRect(0, 110, 128, 60, colBlue);
	
	sprintf(buf, "Recv  : %lu b", accuBytesRecv);
	ST7735_drawString(2, 110, buf, colWhite, 1);		
		
	sprintf(buf, "Total : %d doc", totalJobs);
	ST7735_drawString(2, 120, buf, colWhite, 1);		

	sprintf(buf, "Last  : %s", lastFilename);		
	ST7735_drawString(2, 130, buf, colWhite, 1);				
				
	ST7735_fillRect(0, 150, 128, 10, colBlue);
	ST7735_drawString(2, 150, isPrinting ? "Printing..." : "READY.", colWhite, 1);		

	ST7735_fillRect(0, 90, 128, 20, colBlue);
		
	sprintf(buf, "Free  : %ld MB", cardFree / 1024 / 1024);
	ST7735_drawString(2, 90, buf, colWhite, 1);	
}
	
void __attribute__((interrupt, auto_psv))  _T4Interrupt (void) 
{	
	timerCount++;
	
	// close the last printed file if no printing action for the last 4 seconds
	if (timerCount - lastStrobe > 1024 && pointer != NULL && isPrinting)
	{
		isPrinting = FALSE;
		FSfclose(pointer);
		pointer = NULL;
		
		isPrinting = FALSE;	
		totalJobs++;
		
		// update free size by subtracting the size of the data received in the previous session, once only
		cardFree -= totalBytesRecv;
		accuBytesRecv += totalBytesRecv;
		totalBytesRecv = 0;
		
		showPrinterStats();
	}

	// keep a count of how long the program has been running 
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


void initIO()
{
	// http://dangerousprototypes.com/docs/Bus_Pirate_v3_PIC_24FJ_blinking_LED_programming_tutorial#C30_Source_Code
	// http://www.pic24micro.com/general_purpose_input_output_introduction.html
	// http://www.engscope.com/pic24-tutorial/
	// http://www.ermicro.com/blog/?p=1669
	// http://dangerousprototypes.com/2012/05/09/bus-pirate-v3-pic-24-analog-to-digital-converter-example/

	CLKDIVbits.RCDIV0 =0;     	// clock divider to 0 (no divider, PIC running at 8MHz internal oscillator speed)

	// disable change notification and internal pull up pins
	/*
	CNEN1 = 0;
	CNEN2 = 0;
	CNPU1 = 0;
	CNPU2 = 0;
	*/

	AD1PCFG = 0xFFFF;			// All Digital
	TRISA = 0b0000000000000000;	// Port A all output
	ODCA  = 0b0000000000000000; // Open Drain off on port A. It does not work well on port A due to internal pullup resistor (datasheet pg 104)
	TRISB = 0b1111111100011000;	// PORTB for output, except and RB4 (SD Card Input), RB3 (LPT Strobe) and RB8-15 (printer data input)
	ODCB  = 0b0000000000000000; // No Open Drain Required													
								
	// initialize UART module. This must be done before SPI init, otherwise UART will not work
	/*
	RPINR18bits.U1RXR = 2; //pin RP2 for RX
	RPOR1bits.RP3R   = 3; //pin RP3, 3 for UART1 transmit, 5 for UART2 transmit
	InitUART();
	*/
	
	// interrupt 1 setup for STROBE line, indicating computer wants to send data
	RPINR0bits.INT1R = 3; // Assign interrupt 1 to RP3
	// Make interrupt 1 trigger on a rising edge
	// INTCON2bits.INT1EP = 0;
	INTCON2bits.INT1EP = 1; // faling edge
	IFS1bits.INT1IF = 0;
	IPC5bits.INT1IP = 6;
	IEC1bits.INT1IE = 1;
	
	// remap SPI1 pins for SD card interface
	__builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

	// Inputs 
	RPINR20bits.SDI1R = 4; 	// RP4, MISO - (Master In Slave Out)

	// Outputs
	RPOR2bits.RP5R = 7; 	// RP5, MOSI - (Master Out Slave In) 
	RPOR3bits.RP6R = 8; 	// RP6, CLK - clock

	__builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
}

void showSDCardError(BOOL isInit)
{
	ST7735_fillRect(0, 60, 128, 120, colBlue);
	ST7735_drawString(20, 90, isInit ? "SD CARD ERROR!" : "SD I/O ERROR!", colRed, 1);
	ST7735_drawString(15, 105, "Check & Restart", colRed, 1);
	ST7735_drawString(20, 120, "SYSTEM HALTED!", colRed, 1);
}

// printer STROBE signal send, we capture data
void __attribute__((interrupt, auto_psv))  _INT1Interrupt(void) 
{ 
	IFS1bits.INT1IF = 0;
	
	if (!isSDCardReady) 
		return;
		
	// set as busy so that PC will not send any more data	
	LPT_BUSY = 1;
	
	if (!isPrinting)
	{
		isPrinting = TRUE;
		
		ST7735_fillRect(0, 150, 128, 10, colBlue);
		ST7735_drawString(2, 150, "PRINTING...", colWhite, 1);			
	}
		
	// if the last strobe signal is within 4 seconds, consider the printing to belong to the same file and do not open new file
	if (timerCount - lastStrobe > 1024 || pointer == NULL)
	{
		if (pointer != NULL)
			FSfclose(pointer);
		
		sprintf(lastFilename, "PRN%lu.PRN", timerCount % 99999);		
		pointer = FSfopen (lastFilename, "w");
		totalBytesRecv = 0;
	}
	
	if (pointer == NULL)
	{	
		showSDCardError(FALSE);

		// sd card error, disable the interrupt so that no more printing is processed and program will effectively hang
		IEC1bits.INT1IE = 0;
		
		return;
	}
	
	// write the data to SD card
	unsigned char sendBuffer[] = {PORTB >> 8};
	size_t written = FSfwrite (sendBuffer, 1, 1, pointer);
	if (written != 1)
	{
		showSDCardError(FALSE);

		// sd card write error, disable the interrupt so that no more printing is processed and program will effectively hang
		IEC1bits.INT1IE = 0;
		return;
	}				
		
	lastStrobe = timerCount;
	totalBytesRecv++;
	
	// poll the acknowledge line. 1ms recommended but too slow. We try a lower delay
	LPT_ACK = 0;
	delay_us(10);
	LPT_ACK = 1;
		
	// PC free to send more data, no longer busy
	LPT_BUSY = 0;
}

int main()
{	
	BOOL isOK;
	char buf[100];
	
	initIO();
	initTmr4();
	
	LPT_BUSY = 1;
	LPT_ACK = 1;
	
	// SendUARTStr("PIC24FJ64GA002 Dummy Parallel Printer Starting Up...");
	
	ST7735_initR();
	ST7735_fillScreen(colBlue);
	
	ST7735_drawString(0, 5, "VIRTUAL LPT", colYellow, 2);
	ST7735_drawString(25, 30, "PRINTER", colYellow, 2);
	ST7735_drawHorizontalLine(0, 50, 160, colWhite);
				
	// SendUARTStr("Detecting card info.");		
	ST7735_drawString(5, 60, "Detecting card...", colWhite, 1);
												
	isOK = initSDCard();
	if (!isOK)
	{
		// SendUARTStr("SD Card Error. Check and Restart device.");
		LPT_BUSY = 1;
		showSDCardError(TRUE);
		
		while(1);
	}	
	else
	{
		ST7735_fillRect(2, 60, 128, 20, colBlue);
		ST7735_drawString(5, 60, "Reading card...", colWhite, 1);
					
		// print card information
		SDCARD_INFO cardInfo = getCardInfo();	

		ST7735_fillRect(2, 60, 128, 20, colBlue);
		sprintf(buf, "Volume: %s", cardInfo.volumeLabel);
		ST7735_drawString(2, 60, buf, colWhite, 1);	
	
		sprintf(buf, "Format: %s", cardInfo.fsTypeStr);
		ST7735_drawString(2, 70, buf, colWhite, 1);	
	
		sprintf(buf, "Size  : %ld MB", cardInfo.totalSize / 1024 / 1024);
		ST7735_drawString(2, 80, buf, colWhite, 1);	
	
		sprintf(buf, "Free  : %ld MB", cardInfo.freeSize / 1024 / 1024);
		ST7735_drawString(2, 90, buf, colWhite, 1);	
		cardFree = cardInfo.freeSize;
		
		// show startup printer info
		strcpy(lastFilename, "");
		showPrinterStats();
		
		isSDCardReady = TRUE;
		LPT_BUSY = 0;
	}
		
	while(1)
	{
	}
		
	return 0;
}
