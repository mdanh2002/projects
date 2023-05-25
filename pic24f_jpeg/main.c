/* 
Read JPEG file on a PIC.

Adapted from https://code.google.com/p/picojpeg/

Also interface the ILI9341 320X240 TFT LCD

Adapted from:

http://www.seeedstudio.com/wiki/2.8%27%27_TFT_Touch_Shield_V2.0
http://www.seeedstudio.com/wiki/File:Seeed_TFT_v2.0.zip
*/

#include <p24hj128gp202.h>
#include "delay.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ili9341.h"
#include "spi_lib.h"
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"
#include "jpeg_helper.h"
#include "sd_helper.h"
#include "rtcc.h"

/* Various Write Protect functions diabled */ 
_FBS(BWRP_WRPROTECT_OFF) 
_FSS(SWRP_WRPROTECT_OFF) 
_FGS(GWRP_OFF & GCP_OFF) 

/* Enable Primary Clock with PLL; Start with user selected clock source */ 
/*
Device has onboard unconfigurable brown-own reset if voltage is below certain threshold.

To if "Target Halted" happens due to brown own reset, Open View > File Registers and look for address 0740 (RCON)

bit 1 BOR:Brown-out Reset Flag bit
1= Brown-out Reset has occurred
0= Brown-out Reset has not occurred

bit 0 POR:Power-on Reset Flag bit
1= Power-on Reset has occurred
0= Power-on Reset has not occurred
*/ 
_FOSCSEL(FNOSC_FRCPLL & IESO_OFF) // Internal Fast RC with PLL, then start with user-selected oscillator source
_FOSC(POSCMD_NONE & OSCIOFNC_OFF & IOL1WAY_OFF) // Primary Oscillator Disabled, Clock Output at RA3 (Pin 10 - OSCIOFNC_ON (disabled), OSCIOFNC_OFF (enabled))
_FWDT(FWDTEN_OFF) // Watch Dog Timer is disabled
_FPOR(FPWRT_PWR128) // Power on Reset time is 128 ms
_FICD(ICS_PGD1 & JTAGEN_OFF) // ICD at PGD1, JTAG off

// PWM Carrier Frequency for WAVE Playback (122kHz at 128, 63kHz at 255)
#define PWM_PERIOD_WAVE 128

void initIO()
{
	// Set of clock frequency. Maximum freq. = 80MHz @ 40 MIPS
	// Using internal oscillator = 7.37MHz
	// Running Frequency = Fosc = 7.37 * PLLDIV / 1 / 2 / 2 
	// Output at RA3 = Fosc / 2 
	// As the onboard RC oscillator is approximate, and UART baudrate (U1BRG) is dependent on the FOSC
	// experiments should be done to find a reasonable frequency for UART to work with standard baudrate (56700, 38400, etc)
	CLKDIVbits.FRCDIV = 0;     	// FRC divide by 1
	CLKDIVbits.PLLPOST = 0;		// PLL divide by 2
	CLKDIVbits.PLLPRE = 0;		// PLL divide by 2
	PLLFBDbits.PLLDIV = 15; 	// Freq. Multiplier (default is 50)

	// connect to MAX232
	ADPCFG = 0xFFFF;			// all digital
	ODCB  = 0b0000000000001000; // open drain on RB3 (UART TX, MAX232 @ 5V)
	TRISB = 0b1010000001000000;	// PORTB for output, except RB15 (UART RX), RB6 (LCD MISO), RB13 (SD MISO)
	TRISA = 0b0000000000000000; // PORTA all output

	// UART Module
	RPINR18bits.U1RXR = 15; 	//	pin RP15 for RX
	RPOR1bits.RP3R   = 3; 		//	pin RP3, 3 for UART1 transmit, 5 for UART2 transmit
	InitUART();
}

void setSPIPins()
{
	// this must be done after UART init, other UART will not work
	__builtin_write_OSCCONL(OSCCON & 0xBF);  //unlock RPn assign registers 

	// for ILI9341 LCD module (SPI2)
	RPINR22bits.SDI2R = 6;           	//Assign MISO to RP12.
	RPOR4bits.RP8R = 10;             	//Assign MOSI to RP8.
	RPOR3bits.RP7R = 11;             	//Assign SCK2OUT to RP7

	// for SD card (SPI1)
	RPINR20bits.SDI1R = 13; 		 	// RP13, MISO - (Master In Slave Out)
	RPOR6bits.RP12R = 0x7;			 	// MOSI at RP12
	RPOR7bits.RP14R = 0x8;			 	// SCK1OUT at RP14	

	__builtin_write_OSCCONL(OSCCON | 0x40);  //lock RPn assign registers  
}

void TFT_Test()
{
	if (!TFT_init())
		SendUARTStr("LCD Not Detected.");
	else
	{
		SendUARTStr("LCD OK.");

	    TFT_fillScreen(0, 240, 0, 320, RED);

	    TFT_drawCircle(100, 100, 30, YELLOW);                // center: (100, 100), r = 30 ,color : YELLOW                
	    TFT_drawCircle(100, 200, 40, CYAN);                  // center: (100, 200), r = 10 ,color : CYAN      
	    TFT_fillCircle(200, 100, 30, RED);                   // center: (200, 100), r = 30 ,color : RED        
	    TFT_fillCircle(200, 200, 30, BLUE);                  // center: (200, 200), r = 30 ,color : BLUE                 


		TFT_drawString("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123", 0, 0, 1, BLUE);
		TFT_drawString("abcdefghijklmnopqrstuvwxyz0123", 0, 20, 1, WHITE);
		TFT_drawString("abcdefghijklmn", 0, 40, 2, YELLOW);
		TFT_drawString("abcdef", 0, 100, 3, GREEN);
	}
}

void showSDCardInfo()
{
	char buf[50];

	SendUARTStr("\nDetecting card info...");

	// get SD card information
	SDCARD_INFO cardInfo = getCardInfo();

	sprintf(buf, "Label :  %s", cardInfo.volumeLabel);
	SendUARTStr(buf);

	sprintf(buf, "Format:  %s", cardInfo.fsTypeStr);
	SendUARTStr(buf);

	sprintf(buf, "Size :   %ld MB", cardInfo.totalSize / 1024 / 1024);
	SendUARTStr(buf);

	sprintf(buf, "Free :   %ld MB", cardInfo.freeSize / 1024 / 1024);
	SendUARTStr(buf);
}

void ListCardRootDir()
{
	char buf[50];
	unsigned int filesCount = 0;
	unsigned int dirCount = 0;

	SendUARTStr("\n\nRoot Directory Contents:");

	// the volume label is the first File Record in the root directory
	SearchRec rec;
	int res = 0;
	res = FindFirst("*.*", ATTR_MASK, &rec); //8.3 filename
	// res = wFindFirst("*.*", ATTR_MASK, &rec); //Long filename
	if (res == 0)
	{
		if (rec.attributes & ATTR_VOLUME)
		{
			// volume label, ignore
		}
		else
		SendUARTStr(rec.filename);

		do
		{
			res = FindNext(&rec);
			// res = wFindNext(&rec);
			
			if (res == 0)
			{
				unsigned int year = (rec.timestamp >> 25) + 1980;
				unsigned char month = (rec.timestamp & 0x1FFFFFF) >> 21;
				unsigned char day = (rec.timestamp & 0x1FFFFF) >> 16;
				unsigned char hour = (rec.timestamp & 0xFFFF) >> 11;
				unsigned char minute = (rec.timestamp & 0x7FF) >> 5;
				unsigned char second = (rec.timestamp & 0x1F) * 2;
				
				sprintf(buf, "%s%c%12s %10ld %02d/%02d/%04d %02d:%02d:%02d", 
				(rec.attributes & ATTR_DIRECTORY) ? "\e[0;34m" : "\e[0;30m", 
				(rec.attributes & ATTR_DIRECTORY) ? '~' : ' ', rec.filename, rec.filesize, day, month, year, hour, minute, second);
				
				if (rec.attributes & ATTR_DIRECTORY)
				dirCount += 1;
				else
				filesCount += 1;

				SendUARTStr(buf);
			}
		}
		while (res == 0);

		/*
		http://stackoverflow.com/questions/15763259/unix-timestamp-to-fat-timestamp
		
		The DOS date/time format is a bitmask:
		
		24                16                 8                 0
		+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
		|Y|Y|Y|Y|Y|Y|Y|M| |M|M|M|D|D|D|D|D| |h|h|h|h|h|m|m|m| |m|m|m|s|s|s|s|s|
		+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
		\___________/\________/\_________/ \________/\____________/\_________/
		year        month       day      hour       minute        second
		
		The year is stored as an offset from 1980. 
		Seconds are stored in two-second increments. 
		(So if the "second" value is 15, it actually represents 30 seconds.)
		*/
	}

	sprintf(buf, "\e[0;31m%d file(s) %d dir(s) on card.", filesCount, dirCount);
	SendUARTStr(buf);
	SendUARTStr("\e[0;30mDone.");
}

void GetInputEntered(char* nameEntered)
{
	char c;
	char input[100];
	strcpy(input, "");
	do
	{
		RecUART(&c);
		SendUART(c);
		
		if (c == '\r' || c == '\n')
			SendUARTStr("\n");
		else if (c == 8 && strlen(input) > 0)
			input[strlen(input)-1] = 0;
		else 
			sprintf(input, "%s%c", input, c);	
	}
	while (c != '\r' && c != '\n' );

	strcpy(nameEntered, input);
}

void LoadPhotoFile(unsigned char reduce, unsigned char showHorizontal)
{
	char buf[100];
	char filename[100];
	SendUARTStr("\nType photo name in root directory to be loaded, followed by the ENTER key.");
	
	GetInputEntered(filename);
	if (strlen(filename) > 0)
	{
		sprintf(buf, "Filename is %s", filename);
		SendUARTStr(buf);

		TFT_fillScreen(0, 240, 0, 320, BLACK);

		sprintf(buf, "Loading %s... (Reduce: %d, Horizontal: %d)", filename, reduce, showHorizontal);
		SendUARTStr(buf);
	
		JPEG_Info file_info = pjpeg_load_from_file(filename, reduce, showHorizontal);
			
		if (file_info.loadStatus != STATUS_OK)
		{
			sprintf(buf, "File %s load error %d", filename, file_info.loadStatus);
		}
		else
			sprintf(buf, "File %s (actual: %dx%d, loaded: %dx%d), Comps: %d, ScanType: %d", 
						  filename, file_info.actualWidth, file_info.actualHeight, file_info.loadedWidth, 
						  file_info.loadedHeight, file_info.colorComps, file_info.scanType);
	
		SendUARTStr(buf);
	}
	else
		SendUARTStr("No filename entered.");
}

void showRTCTime()
{
	char buf[100];
	RTCTime currentTime = getRTCTime();
	
	sprintf(buf, "\nCurrent time is %d:%d:%d %d/%d/%d", currentTime.hour,currentTime.minute, currentTime.second, currentTime.day, currentTime.month, currentTime.year);
	SendUARTStr(buf);
}

void enterRTCTime()
{
	char input[100];
	char buf[255];
	SendUARTStr("\nEnter date/time value in DDMMYYHHMMSS format:");
	GetInputEntered(input);

	if (strlen(input) > 0)
	{
		sprintf(buf, "Received: %s", input);
		SendUARTStr(buf);
	}
	else
		SendUARTStr("No value entered.");
}

void ShowUARTHelp()
{
	SendUARTStr("\n? - Show this help page");
	SendUARTStr("\\ - Clear Screen");
	SendUARTStr("| - Show SD Card Info");
	SendUARTStr("@ - List Card Root Directory");
	SendUARTStr("% - Show RTC Time");
	SendUARTStr("= - Set RTC Time");
	SendUARTStr("! - Test LCD Graphics");
	SendUARTStr("* - Load Photo on Card (Normal, Horizontal)");
	SendUARTStr("& - Load Photo on Card (Normal, Vertical)");
	SendUARTStr("[ - Load Photo on Card (Reduce, Horizontal)");
	SendUARTStr("] - Load Photo on Card (Reduce, Vertical)");
}

int main()
{	
	char c;

	initIO();

	// this has to be done after InitPWM, otherwise __builtin_write_OSCCONL will conflict with OC1CONbits in PWM setup
	// and PWM will fail to work
	setSPIPins();

	// enable secondary oscillator for the RTCC module (bit 1, LPOSCEN of OSCCON register)
	// this must be done via the unlock method, and not via an assignment.
	__builtin_write_OSCCONL(OSCCON | 0x02);

	// SPI2 for LCD. Set SPI Clock Frequency to the max. speed that the ILI9341 will operator reliably
	spi2Init(0b00011011);    // primary scaler 4:1 (bit 1-0), secondary scaler 1:1 (bit 4-2)

	SendUARTStr("\n\nPIC24HJ128GP202 Text to Speech and LCD Testing.");

	TFT_Test();
	
	BOOL isOK = initSDCard();
	if (!isOK)
	{
		SendUARTStr("SD Card Not Found!");	
	}
	else
	{
		showSDCardInfo();
	}

	ShowUARTHelp();

	// set default time
	// TODO: even if previous value is retained upon startup, the clock will not run until the time has been set (even if RTCEN bit is set)
	setRTCTime(13, 12, 31, 1, 14, 19, 0);

	while(1)
	{
		RecUART(&c);
		SendUART(c);

		switch(c)
		{
			case '\r':
				SendUART('\n');
				break;
			case '\\':
	    		TFT_fillScreen(0, 240, 0, 320, BLACK);
				break;
			case '?':
				ShowUARTHelp();
				break;
			case '*':
				LoadPhotoFile(0, 1);
				break;
			case '&':
				LoadPhotoFile(0, 0);
				break;
			case '[':
				LoadPhotoFile(1, 1);
				break;
			case ']':
				LoadPhotoFile(1, 0);
				break;
			case '@':
				ListCardRootDir();
				break;
			case '|':
				showSDCardInfo();
				break;
			case '=':
				enterRTCTime();
				break;
			case '%':
				showRTCTime();
				break;
			case '!':
				TFT_Test();
				break;
			default:
				break;
		}
	}
}
