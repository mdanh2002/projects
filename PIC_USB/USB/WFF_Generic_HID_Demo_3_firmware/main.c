/************************************************************************
	main.c

	WFF USB Generic HID Demonstration 3
    usbGenericHidCommunication reference firmware 3_0_0_0
    Copyright (C) 2011 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com

************************************************************************/

#ifndef MAIN_C
#define MAIN_C

// the line on the LCD where the various status lines are shown
#define SYSTEM_TIME_LINE 		0 // RTC Status
#define USB_STATUS_LINE 		1 // USB Status (connected/disconnected)
#define TEMPERATURE_STATUS_LINE 2 // termperature & humidity
#define GAMEBOY_STATUS_LINE 	3 // gameboy camera (OK, Error)
#define DIST_HUMID_STATUS_LINE 	4 // distance finder results and humidity measurements
#define HALL_PIR_STATUS_LINE 	5 // hall effect sensor & PIR sensor result

// whether the DS1621 chip is detected
char isDS1621_OK;

// whether the DHT11 sensor is detected
char isDHT11_OK;

// whether the RTC clock is running before power up (e.g. on backup battery)
char wasClockRunning;

// the last date time received from the RTC chip
unsigned short sec, minute, hour, day, dayofweek, month, year;

// whether the RTC chip is ok.
char isRTC_OK;

// for WAV playback
unsigned char timer0Count = 0;
unsigned int eepromIndex = 0;
unsigned char eeprom_byte = 0;

// whether the game boy camera is OK
char isGameBoy_OK;

// for PS2 keyboard
char isKeybReady = 0;

// the last temperature measurement received from the 2 sensors
float lastDS1621Temp = 255.0;
unsigned char lastDHT11temp = 0xFF;
unsigned char lastDHT11humid = 0xFF;
unsigned char lastLM35temp = 0xFF;

// the last distane measurement
unsigned char distModuleRes = 0;
unsigned int lastDistance = 0;

// Global includes
// Note: string.h is required for sprintf commands for debug
#include <string.h>
#include "p18F4550.h"
#include "util.h"
#include "hcsr04.h"

// for PS2 keyboard interface
#include "./PS2/keyboard.h"
#include "./PS2/keycodes.h"

// Local includes
#include "ds1621.h"
#include "ds1307.h"
#include "HardwareProfile.h"
#include "debug.h"
#include "./LCD/5110_lcd.h"
#include "cam.h"
#include "camCom.h"
#include "uart.h"
#include "24cxx.h"
#include "dht11.h"
#include <delays.h>

#include <stdlib.h>

// Microchip Application Library includes
// (expects V2.9a of the USB library from "Microchip Solutions v2011-07-14")
//
// The library location must be set in:
// Project -> Build Options Project -> Directories -> Include search path
// in order for the project to compile.
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Ensure we have the correct target PIC device family
#if !defined(__18F4550) && !defined(__18F2550)
	#error "This firmware only supports either the PIC18F4550 or PIC18F2550 microcontrollers."
#endif

// Define the globals for the USB data in the USB RAM of the PIC18F*550
#pragma udata
#pragma udata USB_VARIABLES=0x500
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
#pragma udata

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = FLAG_TRUE;


// PIC18F4550/PIC18F2550 configuration for the WFF Generic HID test device

#pragma config PLLDIV   = 5         // 20Mhz external oscillator
#pragma config CPUDIV   = OSC1_PLL2   
#pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
#pragma config FOSC     = HSPLL_HS
#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config BOR      = OFF		// MD sets this to OFF. Set to ON causing random brown-own reset when running on USB power. RCON=0x1C
#pragma config BORV     = 3
#pragma config VREGEN   = ON
#pragma config WDT      = OFF
#pragma config WDTPS    = 32768
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
// #pragma config CCP2MX   = ON
#pragma config STVREN   = ON
#pragma config LVP      = OFF
// #pragma config ICPRT    = OFF
#pragma config XINST    = OFF
#pragma config CP0      = OFF
#pragma config CP1      = OFF
// #pragma config CP2      = OFF
// #pragma config CP3      = OFF
#pragma config CPB      = OFF
// #pragma config CPD      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
// #pragma config WRT2     = OFF
// #pragma config WRT3     = OFF
#pragma config WRTB     = OFF
#pragma config WRTC     = OFF
// #pragma config WRTD     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
// #pragma config EBTR2    = OFF
// #pragma config EBTR3    = OFF
#pragma config EBTRB    = OFF

// Private function prototypes
static void initialisePic(void);
void processUsbCommands(void);
void applicationInit(void);
void USBCBSendResume(void);
void highPriorityISRCode();
void lowPriorityISRCode();

// Remap vectors for compatibilty with Microchip USB boot loaders
#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
#else	
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
#endif

#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER) || defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}
#endif

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR (void)
{
     _asm goto highPriorityISRCode _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR (void)
{
     _asm goto lowPriorityISRCode _endasm
}

#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER) || defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR (void)
{
     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR (void)
{
     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}
#endif

#pragma code

// High-priority ISR handling function
#pragma interrupt highPriorityISRCode
void highPriorityISRCode()
{
	// Application specific high-priority ISR code goes here
	
   // added by MD
   // if INT0 external interrupt, process PS2 keyboard
   if (INTCONbits.INT0IF == 1)
   {
      INTCONbits.INT0IF = 0; //clear flag

	  if (isKeybReady)
	  	processKeyboardISR();
   }
   else if (INTCON3bits.INT1IF == 1)
   { 
	 //INT1 external interrupt
     INTCON3bits.INT1IF = 0; //clear flag
   }
   else if (INTCON3bits.INT2IF == 1)
   { 
     //INT2 external interrupt
     INTCON3bits.INT2IF = 0; //clear flag

	 uart_tx_string_rom("INT2", 1);	
   }
	else if (INTCONbits.TMR0IF)
	{
		// timer 0, wav playback
	 	// uart_tx_string_rom("TMR0", 1);	
		timer0Count++;	

		if (timer0Count%2==0) //timer 0 runs at 16kHz, we aim at 8kHz for WAV playback
		{
			eeprom_byte = read_24Cxx(eepromIndex);
			CCPR1L = eeprom_byte;
			eepromIndex++;

			if (eepromIndex<256)
				uart_tx_data(eeprom_byte);
		}

		if (timer0Count == 255) timer0Count=0;

		INTCONbits.TMR0IF = 0;
	}
	else
	{
	#if defined(USB_INTERRUPT)
		// Perform USB device tasks
		USBDeviceTasks();
	#endif
	}
}

// Low-priority ISR handling function
#pragma interruptlow lowPriorityISRCode
void lowPriorityISRCode()
{
	// Application specific low-priority ISR code goes here
	// uart_tx_string_rom("lowPriorityISRCode", 1);	
}

// String for creating debug messages
char debugString[64];

const rom unsigned char test[5] = {'1','2','3','4','5'};

// Main program entry point
void main()
{   
	unsigned short sec2, minute2, hour2, day2, dayofweek2, month2, year2;
	unsigned int resp = 0;

	unsigned int count = 0;
	char buf[14];
	char* buf2;
	unsigned char c;

	unsigned int adc;
	
	// Initialise and configure the PIC ready to go
    initialisePic();

	SPPCONbits.SPPEN = 0;
	TRISEbits.TRISE0 = 1;	// PIR sensor input
	TRISEbits.TRISE1 = 0;	// BUSY LED output
	TRISEbits.TRISE2 = 0;

	BUSY_LED_OUTPUT = 1;

	// analog input for gameboy camera AND for LM35 analog temperature sensor
	// http://roboticsguy.com/tutorials/microcontrollers/microchip/pic18/configuring-the-pic18f4550-10-bit-adc-module
	ADCON0bits.ADON = 0; // Disable A/D module
	ADCON1bits.VCFG1 = 0; // Use VSS for Vref- source
	ADCON1bits.VCFG0 = 0; // Use VDD for Vref+ source
	ADCON1bits.PCFG0 = 1; // Make AN0 pin (gameboy), AN1 analog (LM35) and all others digital
	ADCON1bits.PCFG1 = 0;
	ADCON1bits.PCFG2 = 1;
	ADCON1bits.PCFG3 = 1;
	ADCON2bits.ADFM = 1;  // A/D result is right justified 
	ADCON2bits.ACQT0 = 1; // Acquisition time
	ADCON2bits.ACQT1 = 1;
	ADCON2bits.ACQT2 = 1;
	ADCON2bits.ADCS0 = 0; // A/D conversion clock
	ADCON2bits.ADCS1 = 1;
	ADCON2bits.ADCS2 = 1;
	TRISAbits.TRISA0 = 1; // input
	ADCON0bits.ADON = 1; // Enable A/D module

	// ultrasonic sensor
	TRISAbits.TRISA5 = 0;  //TRIG
	TRISAbits.TRISA4 = 1;  //ECHO, input

	TRISBbits.TRISB2 = 1; //input to hall effect sensor, e.g. whether magnetic field is present
	TRISBbits.TRISB4 = 0;
	TRISBbits.TRISB5 = 0;

	TRISDbits.TRISD7 = 0;
	TRISDbits.TRISD6 = 0;
	TRISDbits.TRISD5 = 0;
	TRISDbits.TRISD4 = 0;

	TRISCbits.TRISC0 = 1;
	TRISCbits.TRISC1 = 0;	
	TRISCbits.TRISC2 = 0;	// USB Connected indicator

	// turn on timer 0, for WAV playback
	/*
	T0CONbits.T0PS0 = 0; //Prescaler is divide by 256
	T0CONbits.T0PS1 = 0;
	T0CONbits.T0PS2 = 0;
	T0CONbits.PSA = 0;      //Timer Clock Source is from Prescaler
	T0CONbits.T0CS = 0;     //Prescaler gets clock from FCPU (5MHz)
	T0CONbits.T08BIT = 1;   //8 BIT MODE
	INTCONbits.TMR0IE    = 1; //enable interrupt for timer 0
	T0CONbits.TMR0ON = 0;	
	*/

	// init the PWM module
	// see http://electricallabs.lakeheadu.ca/projects/pic18f/projects/c18test.c
	/*
	PR2 = 255; 										// period register
	CCPR1L = 170;									// default duty cycle
	CCP1CONbits.DC1B1 = 0; 							// least sig. bits of duty cycle
	CCP1CONbits.DC1B0 = 0;
	CCP1CONbits.CCP1M3 = 1; 						// for PWM mode
	CCP1CONbits.CCP1M2 = 1; 						// for PWM mode
	T2CONbits.TMR2ON = 1; 							// turn timer on
	*/

	// Initialize the LCD
	lcd_5110_init();
	lcd_5110_black();
	lcd_5110_cls();

	// init the UART module
	uart_init();
	uart_tx_string_rom("PIC 18F4550 UART", 0);
	uart_tx_string_rom(" Init Done", 1);

	// UART echo test
	/*
	while (1)
	{
		char c = uart_rx_data();
		uart_tx_data(c);
	}
	*/

	// intialize the PS2 keyboard
	keyboardInit();
	isKeybReady = 1;

	// keyboard test
	/*
	TRISAbits.TRISA3 = 1;
	while(1)
	{
		if (PORTAbits.RA3)
		{
			uart_tx_data('1');
		}
		else
		{
			uart_tx_data('0');
		}
		/*
		unsigned char c = getKey();
						
		if (c!=0)
		{
			uart_tx_data('!');
			uart_tx_data(c);
		}
	}
	*/

	// Echo command, wait for response
	// expected response is 0xEE too. if not found, that means keyb not present
	// http://www.computer-engineering.org/ps2keyboard/
	/*
	resp = sendKeyboardCommand(0xEE, 1);
	sprintf(buf, (const far rom char*) "Echo Response: %d", resp);	
	uart_tx_string_ram(buf, 1);
	*/

	// keyboard ID ocmmand
	/*
	resp = sendKeyboardCommand(0xF2, 2);
	sprintf(buf, (const far rom char*) "Keyb ID: %d", resp);	
	uart_tx_string_ram(buf, 1);
	*/

	// Num, Caps, Scroll lock turn on
	/*
	sendKeyboardCommand(0xED, 0);
	sendKeyboardCommand(0b00000101, 0);
	*/
	
	isGameBoy_OK = cam_init_and_check();
	if (isGameBoy_OK)
	{
		lcd_string_rom("Camera Found!", GAMEBOY_STATUS_LINE);
	}
	else
	{
		lcd_string_rom("Camera Error!", GAMEBOY_STATUS_LINE);
	}

	// Init DS1307 RTC
	wasClockRunning = ds1307_init();

	// set the clock for the first time
	// the RTC crystal needs to be 32768Hz and have a capacitance of 12.5pF
	// breadboard capacitance is 5pF between vertical rows and 15pF between horizontal rows which could
	// upset the crystal, resulting in the clock not running or running slowly.
	// it's best to have the RTC circuit on a veroboard/PCB to have better clock acurracy.
	if (!wasClockRunning)
	{
		sec = 0;
		minute = 41;
		hour = 20;
		day = 30;
		dayofweek = 7;
		month = 6;
		year = 12;

		// set a sample time and get it back to see if the chip is there
		// since the clock is not running yet, we should get back what we write
		ds1307_set_date_time(day, month, year, dayofweek, hour, minute, sec);
		ds1307_get_date(&day2, &month2, &year2, &dayofweek2);
		ds1307_get_time(&hour2, &minute2, &sec2);
		
		isRTC_OK = (day==day2 && month==month2 && year==year2 && dayofweek == dayofweek2 && hour==hour2 && minute==minute2 && sec == sec2);

		// show RTC status on LCD
		if (isRTC_OK)
			lcd_string_rom("RTC Lost Pwr!", SYSTEM_TIME_LINE);
		else
			lcd_string_rom("RTC Error!  ", SYSTEM_TIME_LINE);
	}
	else
	{
		isRTC_OK = 1;
	}

	// initialize the DS1621 temperature sensor as well as the DHT11 temp & humidity sensor
	isDS1621_OK = ds1621_init();
	isDHT11_OK = dht11_read(&lastDHT11temp, &lastDHT11humid);

	// show on LCD that USB is connecting
	// USBCBInitEP will be called once done, when that happens we will update the LCD
	lcd_string_rom("Connecting", USB_STATUS_LINE);

	// If we are running in interrupt mode attempt to attach the USB device
    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif
	
	// Initialise the debug log functions
    debugInitialise();  

	sprintf(debugString, "USB Generic HID Demonstration 3");
	debugOut(debugString);

	sprintf(debugString, "(C)2011 Simon Inns - http://www.waitingforfriday.com");
	debugOut(debugString);
	
	sprintf(debugString, "USB Device Initialised.");
	debugOut(debugString);
	
	BUSY_LED_OUTPUT = 0;

	// Main processing loop
    while(1)
    {
        #if defined(USB_POLLING)
			// If we are in polling mode the USB device tasks must be processed here
			// (otherwise the interrupt is performing this task)
	        USBDeviceTasks();
        #endif
    	
    	// Process USB Commands
        processUsbCommands();  
        
        // update temperature reading and RTC reading every once in a while
		if (count%30000==0)
		{
			count = 0;
		
			// RTC system time
			if (isRTC_OK && wasClockRunning)
			{		
				ds1307_get_date(&day, &month, &year, &dayofweek);
				ds1307_get_time(&hour, &minute, &sec);

				sprintf(buf, (const far rom char*) "%02d:%02d:%02d %02d/%02d", hour, minute, sec, day, month);	
				lcd_string_ram(buf, SYSTEM_TIME_LINE);
			}

			// DHT11 temp and humidity sensor
			isDHT11_OK = dht11_read(&lastDHT11temp, &lastDHT11humid);
			if (isDHT11_OK)
				sprintf(buf, "%.2dC", lastDHT11temp);
			else
			{
				sprintf(buf, "--C");
			}

			// DS1621 sensor
			if (isDS1621_OK)
			{
				lastDS1621Temp = ds1621_read_temp_precise();
				buf2 = ftoa(lastDS1621Temp);

				sprintf(buf, "%s/%.4sC", buf, buf2);
			}
			else
			{
				sprintf(buf, "%s/---C", buf, buf2);
			}

			// LM35 temperature sensor 
			ADCON0bits.CHS0 = 1; // Select channel 1 - LM35
			ADCON0bits.CHS1 = 0;
			ADCON0bits.CHS2 = 0;
			ADCON0bits.CHS3 = 0;
			ADCON0bits.GO = 1;
			while (ADCON0bits.GO);
			lastLM35temp = (int) (ADRES * 2.4 / 3.0);

			// LM35 operating range from -55C to 150C, with 10mV/C, making it 0V - 1.5V for positive range
			// ADC is 10 bit, range from 0-5V, which means 0 - 307 for 0 - 1.5V or 0C - 150C
			sprintf(buf, "%s/%.3dC", buf, lastLM35temp);
			lcd_string_ram(buf, TEMPERATURE_STATUS_LINE);

			// HC-SR04 range finder module output
			distModuleRes = hc_getDistance(&lastDistance);
		
			// the distance to nearest obstacle is based on the ultrasonic sensor output
			if (distModuleRes == HC_DETECTED)
			{
				sprintf(buf, (const far rom char*) "%.6dcm", lastDistance);	
			}
			else if ( distModuleRes == HC_NOTINRANGE)
			{
				sprintf(buf, (const far rom char*) "RngError");
			}
			else
			{
				sprintf(buf, (const far rom char*) "DvcError");
			}

			// the humidity measured from above
			if (isDHT11_OK)
				sprintf(buf, "%s/%.2d%%", buf, lastDHT11humid);
			else
			{
				sprintf(buf, "%s/--C", buf);
			}pbru

			// put humidity and distance on the same line
			lcd_string_ram(buf, DIST_HUMID_STATUS_LINE);

			// hall effect sensor output, active low
			// PIR sensor output, active high
			if (!HALL_EFFECT_OUTPUT && PIR_SENSOR_OUTPUT)
			{
				lcd_string_rom("Magn&Motn Fnd", HALL_PIR_STATUS_LINE);
				BUZZER_OUTPUT = 1;
			}
			else if (!HALL_EFFECT_OUTPUT && !PIR_SENSOR_OUTPUT)
			{
				lcd_string_rom("Magnet Found ", HALL_PIR_STATUS_LINE);
				BUZZER_OUTPUT = 1;
			}
			else if (HALL_EFFECT_OUTPUT && PIR_SENSOR_OUTPUT)
			{
				lcd_string_rom("Motion Found ", HALL_PIR_STATUS_LINE);
				BUZZER_OUTPUT = 1;
			}
			else if (HALL_EFFECT_OUTPUT && !PIR_SENSOR_OUTPUT)
			{
				lcd_string_rom("Nothing Found", HALL_PIR_STATUS_LINE);
				BUZZER_OUTPUT = 0;
			}
		}

		count++;		
    }
}

// Initialise the PIC
static void initialisePic(void)
{
    // PIC port set up --------------------------------------------------------

	// Default all pins to digital
    ADCON1 = 0x0F;

	// Configure ports as inputs (1) or outputs(0)
	TRISA = 0b00000000;
	TRISB = 0b00000000;
	TRISC = 0b00000000;
#if defined(__18F4550)
	TRISD = 0b00000000;
	TRISE = 0b00000000;
#endif

	// Clear all ports
	PORTA = 0b00000000;
	PORTB = 0b00000000;
	PORTC = 0b00000000;
#if defined(__18F4550)
	PORTD = 0b00000000;
	PORTE = 0b00000000;
#endif

	// If you have a VBUS sense pin (for self-powered devices when you
	// want to detect if the USB host is connected) you have to specify
	// your input pin in HardwareProfile.h
    #if defined(USE_USB_BUS_SENSE_IO)
    	tris_usb_bus_sense = INPUT_PIN;
    #endif
    
    // In the case of a device which can be both self-powered and bus-powered
    // the device must respond correctly to a GetStatus (device) request and
    // tell the host how it is currently powered.
    //
    // To do this you must device a pin which is high when self powered and low
    // when bus powered and define this in HardwareProfile.h
    #if defined(USE_SELF_POWER_SENSE_IO)
    	tris_self_power = INPUT_PIN;
    #endif

    // Application specific initialisation
    applicationInit();
    
    // Initialise the USB device
    USBDeviceInit();
}

// Application specific device initialisation
void applicationInit(void)
{	
    // Initialize the variable holding the USB handle for the last transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}

// Process USB commands
void processUsbCommands(void)
{   
	int pixelReceived = 0;
	int i = 0;

    // Check if we are in the configured state; otherwise just return
    if((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))
    {
	    // We are not configured
	    return;
	}

	// Check if data was received from the host.
    if(!HIDRxHandleBusy(USBOutHandle))
    {   
		// Command mode    
        switch(ReceivedDataBuffer[0])
		{
			case 0x10:	// Debug information request from host
				// Copy any waiting debug text to the send data buffer
				copyDebugToSendBuffer((BYTE*)&ToSendDataBuffer[0]);
			        
		        // Transmit the response to the host
                if(!HIDTxHandleBusy(USBInHandle))
				{
					USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
				}
				break;

            case 0x83:  // Read the DS1621 temperature status
				{
					if (!isDS1621_OK) 
					{
						//DS1621 fails
						ToSendDataBuffer[0] = 0x00; 
					}
					else
					{	
						//DS1621 status ok, send temperature
						ToSendDataBuffer[0] = 0xFF; 
						ToSendDataBuffer[1] = (char)lastDS1621Temp;	//integer part
						ToSendDataBuffer[2] = (char)((lastDS1621Temp - (float)ToSendDataBuffer[1])*256); //fraction part
					}
		
					// Transmit the response to the host
		            if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
		           	break;
				}

			case 0x84:  // Get the RTC reading
				{
					if (isRTC_OK && wasClockRunning)
					{
						ToSendDataBuffer[0] = 0xFF; 

						ToSendDataBuffer[1] = year;
						ToSendDataBuffer[2] = month;
						ToSendDataBuffer[3] = day;
						ToSendDataBuffer[4] = hour;
						ToSendDataBuffer[5] = minute;
						ToSendDataBuffer[6] = sec;
					}
					else if (isRTC_OK && !wasClockRunning)
						ToSendDataBuffer[0] = 0xF0;
					else //RTC Error
						ToSendDataBuffer[0] = 0x00;

					// Transmit the response to the host
		            if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}

					break;
				}
			case 0x85:  // RTC Set time
				{
					if (isRTC_OK)
					{
            			sprintf(debugString, "Received command 0x85 from host - Set RTC");
						debugOut(debugString);

						ds1307_set_date_time(ReceivedDataBuffer[3], ReceivedDataBuffer[2], ReceivedDataBuffer[1], ReceivedDataBuffer[4], ReceivedDataBuffer[5], ReceivedDataBuffer[6], ReceivedDataBuffer[7]);

						// clear error msg about LCD 
						wasClockRunning = 1;
					}

					break;
				}
			case 0x86:  // query gameboy status
				{
					ToSendDataBuffer[0] = isGameBoy_OK;
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}

			case 0x87:  // capture image from gameboy, it will send the next 64 bytes of the picture frame
				{
					BUSY_LED_OUTPUT = 1;

					if (isGameBoy_OK)
					{
						// for first image data packet, wait until the camera is ready for the next frames
						if (ReceivedDataBuffer[1]==0 && ReceivedDataBuffer[2]==0) 
						{
							while (CAM_READ==1)
								camClockAR();

							while (!camWaitRead());							
						}

						// Transmit the image data to the host
		                if(!HIDTxHandleBusy(USBInHandle))
						{
							pixelReceived = 0;
		
							while (pixelReceived < 32)
							{	
								camClockAR();

								ADCON0bits.CHS0 = 0; // Select channel 0 (AN0 - gameboy camera)
								ADCON0bits.CHS1 = 0;
								ADCON0bits.CHS2 = 0;
								ADCON0bits.CHS3 = 0;

								ADCON0bits.GO = 1;
								while (ADCON0bits.GO);

								// send higher byte follow by lower byte
								ToSendDataBuffer[pixelReceived*2] = ADRES>>8; //first 2 bits
								ToSendDataBuffer[pixelReceived*2 + 1] = (ADRES & 0xFF); //last 8 bits

								pixelReceived++;										
							}

							USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
						}
					}	

					BUSY_LED_OUTPUT = 0;
					break;					
				}

			case 0x88: 	// toggle buzzer state

            	sprintf(debugString, "Received command 0x88 from host - Toggle Buzzer");
				debugOut(debugString);

				BUZZER_OUTPUT = !BUZZER_OUTPUT;

            	break;

			case 0x89:  // read buzzer state
				{
					ToSendDataBuffer[0] = BUZZER_OUTPUT;
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}

			case 0x92:  // read a char from the keyboard buffer
				{
					char bufferCount = 0;
					unsigned char c = 0;
					do
					{
						c = getKey();
						
						if (c!=0)
						{
							bufferCount++;
							ToSendDataBuffer[bufferCount] = c;
						}
					}
					while (bufferCount < 63 && c!=0);

					ToSendDataBuffer[0] = bufferCount;
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}

			case 0x93:  // read the DHT11 temp & humidity sensor
				{
					if (!isDHT11_OK) 
					{
						//DHT11 fails
						ToSendDataBuffer[0] = 0x00; 
					}
					else
					{	
						//DHT11 status ok, send temperature
						ToSendDataBuffer[0] = 0xFF; 
						ToSendDataBuffer[1] = lastDHT11temp;
						ToSendDataBuffer[2] = lastDHT11humid;
					}
		
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}
			case 0x94:  // pronounce the current time using timer 0
				{
    	        	sprintf(debugString, "Received command 0x94 from host - Say Time");
					debugOut(debugString);

					T0CONbits.TMR0ON = 1;
	            	break;
				}
			case 0x95:  // query magnetic field status
				{
					ToSendDataBuffer[0] = !HALL_EFFECT_OUTPUT; //active low
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}
			case 0x96:  // query distance module
				{
					ToSendDataBuffer[0] = distModuleRes;
					ToSendDataBuffer[1] = lastDistance >> 8; //high byte
					ToSendDataBuffer[2] = lastDistance & 0xFF; //low byte

					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}
			case 0x97:  // read PIR sensor status
				{
					ToSendDataBuffer[0] = PIR_SENSOR_OUTPUT; 
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}
			
			case 0x98:  // read LM35 measurements
				{
					ToSendDataBuffer[0] = lastLM35temp;
					
					// Transmit the response to the host
	                if(!HIDTxHandleBusy(USBInHandle))
					{
						USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
					}
	            	break;
				}

            default:	// Unknown command received
           		break;
		}
		 
        // Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
  	}
}

// USB Callback handling routines -----------------------------------------------------------

// Call back that is invoked when a USB suspend is detected
void USBCBSuspend(void)
{
	lcd_string_rom("USB Suspended!", USB_STATUS_LINE);

	USB_LED_OUTPUT = 0;
}

// This call back is invoked when a wakeup from USB suspend is detected.
void USBCBWakeFromSuspend(void)
{
	
}

// The USB host sends out a SOF packet to full-speed devices every 1 ms.
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here. Callback caller is already doing that.
}

// The purpose of this callback is mainly for debugging during development.
// Check UEIR to see which error causes the interrupt.
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// We are not configured
	lcd_string_rom("USB Conn. Err!", USB_STATUS_LINE);

	USB_LED_OUTPUT = 0;
}

// Check other requests callback
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}

// Callback function is called when a SETUP, bRequest: SET_DESCRIPTOR request arrives.
void USBCBStdSetDscHandler(void)
{
    // You must claim session ownership if supporting this request
}

//This function is called when the device becomes initialized
void USBCBInitEP(void)
{
    // Enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    
    // Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);

	lcd_string_rom("USB Connected!", USB_STATUS_LINE);
	USB_LED_OUTPUT = 1;
}

// Send resume call-back
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    // Verify that the host has armed us to perform remote wakeup.
    if(USBGetRemoteWakeupStatus() == FLAG_TRUE) 
    {
        // Verify that the USB bus is suspended (before we send remote wakeup signalling).
        if(USBIsBusSuspended() == FLAG_TRUE)
        {
            USBMaskInterrupts();
            
            // Bring the clock speed up to normal running state
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FLAG_FALSE;

            // Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            // device must continuously see 5ms+ of idle on the bus, before it sends
            // remote wakeup signalling.  One way to be certain that this parameter
            // gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            // least 3ms from bus idle to USBIsBusSuspended() == FLAG_TRUE, yeilds
            // 5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            } while(delay_count);
            
            // Start RESUME signaling for 1-13 ms
            USBResumeControl = 1;
            delay_count = 1800U;
            do
            {
                delay_count--;
            } while(delay_count);
            USBResumeControl = 0;

            USBUnmaskInterrupts();
        }
    }
}

// USB callback function handler
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            // Application callback tasks and functions go here
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        default:
            break;
    }      
    return FLAG_TRUE; 
}

#endif
