/*
Code taken from
(1) http://www.riadesign.com/index.php?option=com_content&view=article&id=97&Itemid=112&lang=en
(2) http://www.winpicprog.co.uk/pic_tutorial_i2c_board.htm
(3) http://www.robot-electronics.co.uk/acatalog/I2C_Tutorial.html
(4) http://ghmicro.com/index.php?option=com_content&view=article&id=38:nokia-5110-lcd&catid=15:display

Reading an 24C64 EEPROM, output to Hyperterminal, and display text on a Nokia 3510i LCD display or on a Nokia 5110 LCD Display

// EEPROM Contents:
// Offset 0: ASCII Font table for Nokia 3510i, each character is 8x12
// Offset 1140: Sample chinese character for testing, 16x16 each
// Offset 1356 (length 1600): Sample color bitmap for testing
// Offset 2956 (length 480): ASCII Font table for Nokia 5110, each character is 5x8
// Offset 3436 (length 400): 80x40 monochrone bitmap for testing of Nokia 5110 LCD

Pin RA2: specify which type of LCD (low is N3510, high is N5110)
*/

//*****
//include header files

#include	<pic.h>
#include	<conio.h>
#include	<stdlib.h>
#include 	<stdio.h>
#include	<ctype.h>
#include	"serial.h"
#include	"lcd.h"
#include	"eeprom.h"
#include	"display.h"
#include 	"5110_lcd.h"

// config words
__CONFIG(FOSC_HS & WDTE_OFF & BOREN_OFF & LVP_OFF & DEBUG_OFF);
__CONFIG(IESO_OFF & FCMEN_OFF);

void LCD_Test(void)
{
	  int  i;

	  //256 color
	  //0b11100000 0xe0 red
	  //0b00011100 0x1c green
  	//0b00000011 0x03 blue

		/* PIC Initialization */
		TRISB = 0x00;				
		TRISA = 0x00;
		ANSEL = 0x00;
		ADCON1 = 0x00;
		ADCON0 = 0x00;
	
		LCD_Initialize();  		//LCD Initialization	 
		LCD_ColorSet(0); 		//0: 256 color mode,1: 4096 color moede
		LCD_Clear(255,0);	  //Clear		
		addset(0,0,97,66);		//Set start Coordinate, start X, start Y, end X, end Y
					
		for(int y = 0; y < 67; y ++)
		{
			for(int x = 0; x < 98; x ++)
				{
					switch(x / 40)
					{
						case 0:
							//red
							send(0b11100000, 1);
							break;
						case 1:
							//green
							send(0b00011100, 1);
							break;
						case 2:	
							//blue
							send(0b00000011, 1);
							break;
					}
				}
		}			
}

void EEPROM_Test1(void)
{
	putst("Dumping EEPROM contents method 1.");

	ADCON0=0;
	ADCON1=0;
	ANSEL=0; 

	PORTA = 0xFF;
	PORTB = 0xFF;

	PORTA = 0;
	PORTB = 0;

	putch(10);	
	putch(13);

	i2c_start();              // send start sequence
    i2c_write(I2C_eeprom);
	int offset = 0;
	int totalToRead = 8192;
	i2c_write(offset);	//read first byte
	i2c_write(offset);
	i2c_start(); 	//restart
    i2c_write(I2C_eeprom+1); //start reading

	for (int i=offset;i<offset+totalToRead;i++)
	{
		char data=i2c_rx(1);
		putch(data);
	}	

	i2c_stop();

	putst("Done.");
	putch(10);	
	putch(13);
}

void EEPROM_Test2(void)
{
	putst("Dumping EEPROM contents method 2.");

	ADCON0=0;
	ADCON1=0;
	ANSEL=0; 

	PORTA = 0xFF;
	PORTB = 0xFF;

	PORTA = 0;
	PORTB = 0;

	putch(10);	
	putch(13);

	//Output all EEPROM contents to serial
	int i = 0;
	while (i < 8192)
	{
		char byte0 = read_24Cxx(i);  
		putch(byte0);
		i++;
	}

	putst("Done.");
	putch(10);	
	putch(13);
}

void main(void)
{
	/* PIC Initialization */
	TRISB3 = 0;
	TRISB4 = 0;
	TRISB6 = 0;
	TRISB7 = 0;
				
	ANSEL = 0x00;
	ADCON1 = 0x00;
	ADCON0 = 0x00;
	TRISA2 = 1;
	TRISA3 = 0;
	
	//EEPROM_Test1();
	//EEPROM_Test2();
	//LCD_Test();

	if (RA2 == 1) //RA2 high, using Nokia 5110 LCD
	{
		SCE = 1;
    	SCLK = 0;
    	RST = 1;
    	lcd_5110_init();

		while(1)
		{
			lcd_5110_string("Coded by  futz",0);
			lcd_5110_string("Modified by MD",1);
			lcd_5110_string("Nokia 5110 LCD",2);
			lcd_5110_string("Using PIC16F88",3);
			lcd_5110_string("Copyright 2011",4);
			lcd_5110_string("It is working!",5);
			
			delayms(2000);
			
			lcd_5110_cls();
			
			lcd_5110_string("Bitmap Testing",5);	
			lcd_5110_bitmap();
			
			delayms(2000);
		}
	}
	else //RA2 low, using nokia 3510i LCD
	{
		LCD_Initialize();  		//LCD Initialization	 
		LCD_ColorSet(0); 		//0: 256 color mode,1: 4096 color moede

		while (1)
		{
			LCD_Clear(255,0);	  //Clear		
			
			// Display the background image
			displayBitmap();
			
			// Display English words
			showASCIIString(10,55,"NOKIA 3510I",0x03,255);  
			showASCIIString(68,5,"GOOD",0,255);	
			showASCIIString(75,17,"LCD",0,255);	
			
			// Display chinese words
			showhanzi(0,40,4,0xe0,255); 
			showhanzi(14,40,5,0xe0,255);
			showhanzi(28,40,6,0xe0,255);
			showhanzi(42,40,7,0xe0,255);
			showhanzi(56,40,8,0xe0,255);
			showhanzi(70,40,9,0xe0,255);
			showhanzi(84,40,10,0xe0,255); 
			showhanzi(0,5,0,0x03,255);
			showhanzi(14,5,1,0x03,255);
			showhanzi(0,19,2,0x03,255);
			showhanzi(14,19,3,0x03,255);

			delayms(5000);
		}
	}	
}
