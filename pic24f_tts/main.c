/* 
Adapted from

Converted by MD to the PIC24 Micro-controller
SAM Software Automatic Mouth: http://simulationcorner.net/index.php?page=sam

A simple attempt with a PIC16F616 and 23K256 EEPROM:

http://mondo-technology.com/syntho.html

Also interface the SYN6288 Chinese text To speech Module

The quality of the SYN6288 is quite good, much less robotic than the TTS256 or this code.

PAM8403 is used to amplify the audio output for playback on an 8 ohm speaker without the need for an LM386 circuit.

To use only one speaker, a stereo-to-mono circuit should be used (with 3 390ohm-600ohm resistors)
to merge the 2 audio output channels before being fed into the PAM8403 for amplification.
*/

#include <p24fj64ga002.h>
#include "delay.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SAM/sam.h"
#include "SAM/reciter.h"
#include "syn6288.h"

// http://dangerousprototypes.com/2012/04/26/bpv3-ftdi-uart-demo-wiki/
// for possible headers, refer to C:\Program Files (x86)\Microchip\MPLAB C30\support\PIC24F\h\p24FJ64GA002.h
_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_OFF & POSCMOD_NONE & I2C1SEL_PRI) // Internal FRC OSC = 8 MHz x 4PLL = 32MHz, output Fosc/2 at RA3
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need

// PWM Carrier Frequency for WAVE Playback (122kHz at 128, 63kHz at 255)
#define PWM_PERIOD_WAVE 128

void initIO()
{
	CLKDIVbits.RCDIV0 =0;     	// clock divider to 0 (no divider, PIC running at 8MHz internal oscillator speed)

	// connect to MAX232
	AD1PCFG = 0xFFFF;		    // all digital
	ODCB  = 0b0000000000000000; // No Open Drain Port
	TRISB = 0b0000000001010000;	// PORTB for output, except RB4 (UART 1 RX) and RB6 (UART 2 RX). TTS PWM output at RB5 with BUSY output at RB2
	TRISA = 0b0000000000010000; // PORTA all output except RA4 (SYN6288 BUSY)

	// UART Module 1, for PC communication
	RPINR18bits.U1RXR = 4; // pin RP4 for RX
	RPOR1bits.RP3R   = 3;  // pin RP3, set to 3 for UART1 transmit
	InitUART();

	// UART Module 2, for SYN6288 Chinese TTS module
	RPINR19bits.U2RXR = 6; // pin RP6 for RX
	RPOR3bits.RP7R   = 5;  // pin RP7, set to 5 for UART2 transmit
	InitUART2();	
}

void initPWM(int frequency)
{
	// PWM on RP5. PIC24FJ64GA004 Family datasheet page 128
	RPOR2bits.RP5R = 18; // assign PWM pins

	// Initialize Output Compare Module
	OC1CONbits.OCM = 0b000; // Disable Output Compare Module

	// carrier frequency
	OC1RS = frequency / 2; // PWM Duty Cycle
	OC1R  = frequency / 2; // PWM Initial Duty Cycle, e.g. duty cycle until the first period is complete
	OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
	OC1CONbits.OCM = 0b110; // Select the Output Compare mode (PWM no fault)
	 
	// Initialize and enable Timer2
	/*
	Example 13-1, datasheet page 129
	Example for 52.08kHz PWM with 8MHz internal oscillator with PLL enabled (4x) = 32MHz
	TCY = 2 * TOSC = 2 * (1/32*10^6) = 6.25 * 10^(-8)
	PWM Period = 1 / PWM Frequency = 1/52.08kHz = 1.92*10^(-5)
	PWM Period = (PR2 + 1) * TCY * (Timer2 Prescale Value)
	1.92*10^(-5)= (PR2 + 1) * 6.25*10^(-8) * 1
	PR2 = 306.2
	
	The PWM maximum resolution is defined as the maximum number of pulses that you can pack into a PWM period.
	
	The PWM duty cycle is specified by writing to the OCxRS register. The OCxRS register can be written to at any time, but the duty cycle value is not latched into
	OCxR until a match between PRy and TMRy occurs (i.e., the period is complete). This provides a double buffer for the PWM duty cycle and is essential for glitch-
	less PWM operation. In the PWM mode, OCxR is a read-only register.
	
	Some important boundary parameters of the PWM duty cycle include:
	i If the Output Compare x register, OCxR, is loaded  with 0000h, the OCx pin will remain low (0% duty cycle).
	i If OCxR is greater than PRy (Timer Period register), the pin will remain high (100% duty cycle).
	i If OCxR is equal to PRy, the OCx pin will be low for one time base count value and high for all other count values.
	*/
	
	T2CONbits.TON = 0; // Disable Timer
	T2CONbits.TCS = 0; // Select internal instruction cycle clock
	T2CONbits.TGATE = 0; // Disable Gated Timer mode
	T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
	TMR2 = 0x00; // Clear timer register
	PR2 = frequency; // Load the period value
	OC1RS = 0; 	// set PWM output to 0 so that a LED at the PWM position can be used to indicate speaking activity
}

void ShowUARTHelp()
{
	SendUARTStr("\n\nPIC24FJ64GA002 Text to Speech Demo.");
	SendUARTStr("Enter text to speak, or use the following options:");
	SendUARTStr("\n? - Show this help page");
	SendUARTStr("@ - SAM Speaking Demo 1");
	SendUARTStr("| - SAM Speaking Demo 2");
	SendUARTStr("~ - SAM Singing Demo");
	SendUARTStr("^ - SYN6288 Demo");
}

void SpeakDemoText(char* str)
{
	SendUARTStr(str);
	speakText(str);
	delay_ms(200);
}

/*
This function speaks several Chinese texts.
BIG5 Encoding is used. For the text to display properly, in MPLAB 8.63, we'll need to set the encoding
of this file to BIG5 in Edit > Property > Text.

MPLAB 8.63 does not support Unicode characters. There is also a bug (probably with string length?) that causes
the editor to malfunction (incorrect cursor position and text display when Chinese text is used, even with the
correct encoding set for the page

It is best to edit the Chinese text with an external Editor (Notepad++) and just use ANSI+ encoding in Notepad++
The Chinese characters, albeit displayed incorrectly with the default encoding, will not be affected when the file is saved.
*/
void SYN6288Demo()
{
	SendUARTStr("\nSYN6288 Demo in progress...");
		
	SYN_SetBackGroundMusic(SYNBackGroundMusic1);

	// BIG5 character encoding
	SYN_SetTextCodec(SYNTextCodecGBK);
		
	// chinese characters declared as array, last character is for string ending
	// codes taken from datasheet page 14
	// ¦t­µ¤Ñ¤U (Yu yin tianxia)
	unsigned char voice[9] = {0xD3, 0xEE, 0xD2, 0xF4, 0xCC, 0xEC, 0xCF, 0xC2, 0x00};
	SYN_Say(voice); 
	
	// chinese character as BIG5 strings
	SYN_SetTextCodec(SYNTextCodecBIG5);
	
	// Gongxi facái, wànshì rúyì, zhu li méng en (Good fortune, good luck)
	// the chip supports special tone to play before each announcement (soundxxx, msgxxx, ringxxx). Refers to Datasheet page 22.
	SYN_Say((unsigned char *)"sounda ®¥³ß¡AÉE¨Æ¦p·N¡A¥D¨½»X®¦");
	
	// numbers from 0 to 10
	SYN_Say((unsigned char *)"soundk ¹s¤@¤G¤T¥|¤­¤»¤C¤K¤E¤Q");
		
	// english characters
	SYN_Say((unsigned char *)"msga A B C D E F G H I");	
	SYN_Say((unsigned char *)"J K L M N O P Q R"); // ringb, ringk: long music
	SYN_Say((unsigned char *)"S T U V W X Y Z");
	SYN_Say((unsigned char *)"This is an English sentence");
	
	SendUARTStr("\nSYN6288 Demo completed...");	
}

void DemoSpeaking1()
{
	SendUARTStr("\nSAM Speaking Demo 1 in progress...");
	
	SpeakDemoText("Hello everyone. My name is SAM, short for Software Automatic Mouth.");
	SpeakDemoText("I am ported from SAM on the P C to a P I C 2 4 using P W M for audio.");
	SpeakDemoText("I am so small that I can fit into embedded computers such as P I C and A V R.");
	SpeakDemoText("Honestly, do my voice sound better than the T T S 2 5 6 ?");
	SpeakDemoText("Even though obviously I still sound like a robot to you.");
	SpeakDemoText("Thank you for listening to me. Have a nice day. Good bye!");

	SendUARTStr("SAM Speaking Demo 1 done...");
}

void DemoSpeaking2()
{
	SendUARTStr("\nSAM Speaking Demo 2 in progress...");
	
	SpeakDemoText("I can do many things, for example, counting numbers:");
	SpeakDemoText("0 1 2 3 4 5 6 7 8 9");
	SpeakDemoText("And spelling letters, A B C D E F G H I J");
	SpeakDemoText("K L M N O P Q R S T U V W X Y Z");
	SpeakDemoText("Although, due to limited memory, some words may not be pronounced correctly.");
	SpeakDemoText("Such as: Windows, Nineteen and Recipe.");
	SpeakDemoText("That should be: Weendows, Nine teen, Re see pee.");
	SpeakDemoText("But I am still good enough to make myself understood. Or at least I hope so");
	SpeakDemoText("Thank you and good bye!");

	SendUARTStr("SAM Speaking Demo 2 done...");
}

void DemoSinging()
{
	SendUARTStr("\nSAM Singing Demo in progress...");

	SpeakDemoText("I will sing the National Anthem of the United States of America");
	SpeakDemoText("The Star Spangled Banner");

	SingString(64, "ohohoh");
	SingString(76, "ohohoh");	SingString(96, "sehehehehehehehehehey");	SingString(76, "kaeaeaeaeaeaeaeaeaen");	SingString(64, "yuxuxuxuxuxuxw");	SingString(48, "siyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiy");	SingString(38, "baaaaay");	SingString(42, "dhaaaxaxaxax");	SingString(48, "daoaoaoaoaoaoaonz");	SingString(76, "ererererererer");	SingString(68, "liyiyiyiyiyiyiyiyiy");	SingString(64, "laaaaaaaaaaaaaaaaaaaaaaaaayt");	SingString(64, "whahahaht");	SingString(64, "sohohuw");	SingString(38, "praaaaaaaaaaaaaaaauwd");	SingString(42, "liyiyiy");	SingString(48, "wiyiyiyiyiyiyiyiyiy");	SingString(51, "/heheheheheheheheheheheheheheheheheheyld");	SingString(56, "aeaeaeaet");	SingString(51, "dhaaaxaxaxax");	SingString(48, "twaaaaaaaaaaaaaaiy");	SingString(48, "laaaaaaaaaaaaiyts");	SingString(64, "laeaeaeaeaeaeaeaeaest");	SingString(76, "gliyiyiyiyiyiyiyiyiym");	SingString(96, "mihihihihihihihihihihnx");	SingString(64, "/huxuxuxuxuxuxuxuxuxuxwz");	SingString(76, "braoaoaod");	SingString(96, "straaaaaaaaaaaaiyps");	SingString(76, "aeaeaeaeaeaeaeaeaeaend");	SingString(64, "braaaaaaaaaaaaaaiyt");	SingString(48, "staaaaaaaaaaaaaaaaaaaaaaaaaarz");	SingString(38, "thruxuxw");	SingString(42, "dhaaaxaxaxax");	SingString(48, "pehehehehehehehehehehr");	SingString(76, "rixixixixixixixixixixixixl");	SingString(68, "lahahahahahahahahahs");	SingString(64, "faaaaaaaaaaaaaaaaaaaaaaaaaaiyt");	SingString(64, "ohohohr");	SingString(64, "dhaaaxaxaxax");	SingString(38, "raeaeaeaeaeaeaeaeaeaeaeaem");	SingString(42, "paaaarts");	SingString(48, "wiyiyiyiyiyiyiyiyiy");	SingString(51, "waaaaaaaaaaaaaaaaaaaaaaaaaachd");	SingString(56, "werer");	SingString(51, "sohohw");	SingString(48, "gaeaeaeaeaeaeaeaeaeael");	SingString(48, "lixixixixixixixixixixixixixnt");	SingString(64, "liyiyiyiyiyiyiyiyiy");	SingString(76, "striyiyiyiyiyiyiyiyiym");	SingString(96, "mihihihihihihihihihnx");	SingString(38, "aeaeaeaeaend");	SingString(38, "dhaaaxaxaxax");	SingString(38, "raaaaaaaaaaaak");	SingString(36, "kixixixixixixixixixixixts");	SingString(32, "rehehehehehehehehehd");	SingString(32, "gleheheheheheheheheheheheheheheherer");	SingString(36, "dhaaaxaxaxax");	SingString(38, "baaaamz");	SingString(42, "bererererererst");	SingString(38, "tihihihihihihihihihnx");	SingString(36, "ihihihihihihihihihihn");	SingString(36, "eheheheheheheheheheheheheheheheheheyr");	SingString(36, "geheheheyv");	SingString(38, "pruxuxuxuxuxuxuxuxuxuxuxuxwf");	SingString(42, "thruxuxw");	SingString(48, "dhaaaxaxax");	SingString(51, "naaaaaaaaaaaaaaaaaaaaaaayiyt");	SingString(56, "dhaeaeaeaet");	SingString(51, "aaaaaauwr");	SingString(48, "flaeaeaeaeaeaeaeaeaeg");	SingString(76, "wahahahahahahahahahz");	SingString(68, "stihihihihihihihihihl");	SingString(64, "dhehehehehehehehehehehehehehehehehehehehr");	SingString(64, "ohohohohohohow");	SingString(48, "sehehehehehehehehehey");	SingString(48, "dahahahahahahahahahz");	SingString(48, "dhaeaeae");	SingString(51, "aeaeaet");	SingString(56, "staaaaaaaaaaaar");	SingString(56, "spehehehehehehehehehiynx");	SingString(56, "gaxaxaxaxaxaxaxaxaxaxaxaxld");	SingString(42, "baeaeaeaeaeaeaeaeaen");	SingString(36, "nerer");	SingString(38, "ererer");	SingString(42, "yeheheh");	SingString(48, "eheheheht");	SingString(48, "weheheheheheheheheheheh");	SingString(51, "ehehehehehehehiyiyiyv");	SingString(64, "ohohohr");	SingString(64, "dhaaaxaxaxax");	SingString(48, "laeaeaeaeaeaeaeaeaeaeaeae");	SingString(42, "aeaeaend");	SingString(38, "ahahahv");	SingString(36, "dhaaaxaxaxax");	SingString(32, "friyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiyiy");	SingString(48, "aeaeaend");	SingString(42, "dhaaaxaxaxax");	SingString(38, "/hohohohohohohohohowm");	SingString(36, "ahahahahv");	SingString(42, "dhaaaxaxaxaxaxaxaxaxaxaxaxaxax");	SingString(48, "brehehehehehehehehehehiyiyiyv");

	SpeakDemoText("Thank you for listening to me singing. Good bye!");

	SendUARTStr("\nSAM Singing Demo done...");
}

void sendTestValue(char c)
{
	LATAbits.LATA2 = 1;	
	LATB = (c << 8) & 0xFF00;	
	LATAbits.LATA2 = 0;
}
	
int main()
{	
	char wordRecv[100];
	char buf[255];
	unsigned char maxWordLen = 60;
	char c;

	initIO();
	
	while(1)
	{
		sendTestValue('H');
		sendTestValue('e');			
		sendTestValue('l');			
		sendTestValue('l');					
		sendTestValue('o');					
		sendTestValue(' ');							
		sendTestValue('W');
		sendTestValue('o');			
		sendTestValue('r');			
		sendTestValue('l');					
		sendTestValue('d');					
	}
	
	/*
	while(1)
	{
		LATBbits.LATB8 = 0;			
		LATBbits.LATB8 = 1;
		// __delay32(1);
		LATBbits.LATB8 = 0;			
	}
	*/
				
	T2CONbits.TON = 0; // stop Timer 
	initPWM(PWM_PERIOD_WAVE);
	T2CONbits.TON = 1; // Start Timer 

	SpeakDemoText("Hello. This is SAM. I am starting up.");
	ShowUARTHelp();

	strcpy(wordRecv, "");
	while(1)
	{
		unsigned char shouldSpeak = 0;

		RecUART(&c);
		SendUART(c);

		unsigned char len = strlen(wordRecv);

		if (c == '\r' || c == '\n')
		{
			SendUART('\n');
			wordRecv[len] = 0;
			shouldSpeak = 1;
		}
		else if ((c == 8) && (len > 0))
		{
			// BACKSPACE key, remove last char
			if (len > 0)
				wordRecv[len-1] = 0;
		}
		else if (c == '?')
		{
			strcpy(wordRecv, "");
			ShowUARTHelp();
			continue;
		}
		else if (c == '@')
		{
			strcpy(wordRecv, "");
			DemoSpeaking1();
			continue;
		}
		else if (c == '>')
		{
			strcpy(wordRecv, "");
			ShowUARTHelp();
			continue;
		}
		else if (c == '|')
		{
			strcpy(wordRecv, "");
			DemoSpeaking2();
			continue;
		}
		else if (c == '~')
		{
			strcpy(wordRecv, "");
			DemoSinging();
			continue;
		}
		else if (c == '^')
		{
			strcpy(wordRecv, "");
			SYN6288Demo();
			continue;
		}
		else
		{
			sprintf(wordRecv, "%s%c", wordRecv, c);
		}

		len = strlen(wordRecv);

		if (len > maxWordLen)
			shouldSpeak = 1;

		if (shouldSpeak && len > 0)
		{	
			sprintf(buf, "Speaking: '%s'", wordRecv);
			SendUARTStr(buf);

			if (!speakText(wordRecv))
				SendUARTStr("TTS Error");

			strcpy(wordRecv, "");
		}	
	}
}
