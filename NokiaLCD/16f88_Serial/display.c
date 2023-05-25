/* Test Function for the Nokia 3510i LCD */

#include "pic.h"
#include "display.h"
#include "eeprom.h"
#include "lcd.h"
#include "serial.h"


char temp[8] = {128, 64, 32, 16, 8, 4, 2, 1};

// Display a single ACII character on the LCD at the specified location using the specified background color
void showASCIIChar(char x,char y,char value,char dcolor,char bgcolor)	
{  
	unsigned char i,j;	

	/*
	putst("Reading font from EEPROM for: ");
	putch(value);
	putch(10);
	putch(13);
	*/

	//the area to output the character
    addset(x,y,x+7,y+11);  

	//read the font set from the EEPROM first
	i2c_start();              // send start sequence
    i2c_write(I2C_eeprom);
	int offset = (value-32)*12;
	i2c_write(offset>>8); //read first byte
	i2c_write(offset);	
	i2c_start(); 	//restart
    i2c_write(I2C_eeprom+1); //start reading

	// draw the character on the screen. Using a 8x12 bitmap font
	for(j=0;j<12;j++)
	{
		//do not acknowledge the last byte
		char ack = (j==11 ? 0 : 1);
		char data=i2c_rx(ack);

		/*
		putch(data);
		if (data == 0) putst("data = 0."); 
		else if (data == 255) putst("data = 255");
		else putst("data other value.");
		*/

		for(i=0;i<8;i++)
		{ 		     
		 	if((data & temp[i]) != 0)
			{
				//putch('*');
				send(dcolor,1);
			} 
			else
			{
				//putch(' ');
				send(bgcolor,1);
			}   		
		}
	
		/*
		putch(10);
		putch(13);
		*/
	 }

	i2c_stop();

	//short delay	
	//for (int i=0; i<100; i++);

	/*
	putst("Done.");
	putch(10);
	putch(13);
	*/
}

// Display a string of ACII characters on the LCD at the specified location using the specified background color
void showASCIIString(char x, char y,const char *str,char dcolor,char bgcolor)	  
{  
	unsigned char x1,y1;
	x1=x;
	y1=y;
	while(*str!='\0')
	{	
		showASCIIChar(x1,y1,*str,dcolor,bgcolor);
		x1+=7;
		str++;
	}	
}


// Display a chinese character 
void showhanzi(unsigned char x,unsigned char y,unsigned char value,unsigned char dcolor,unsigned char bgcolor)
{  
	unsigned char i,j;

	//read the font set from the EEPROM first
	i2c_start();              // send start sequence
    i2c_write(I2C_eeprom);
	int offset = 1140 + value*18; //samples chinese character from offset 1140 in EEPROM, after the N3510I font set
	i2c_write(offset>>8); //read first byte
	i2c_write(offset);	
	i2c_start(); 	//restart
    i2c_write(I2C_eeprom+1); //start reading

    addset(x,y,x+11,y+11); 

	for(j=0;j<18;j++)
	{
		//do not acknowledge the last byte
		char ack = (j==17 ? 0 : 1);
		char data=i2c_rx(ack);

		for(i=0;i<8;i++)
		{ 		     
		 	if((data & temp[i]) != 0)
			{
				send(dcolor,1);
			} 
			else
			{
				send(bgcolor,1);
			}   
		}
	 }

	i2c_stop();
}

//display a test bitmap stored in the EEPROM
void displayBitmap()
{
	//read the bitmap set from the EEPROM first
	i2c_start();              // send start sequence
    i2c_write(I2C_eeprom);
	int offset = 1356;	  //bitmap store at offset 1356 in EEPROM, after the N3510i font set and sample chinese characters
	i2c_write(offset>>8); //read first byte
	i2c_write(offset);	
	i2c_start(); 	//restart
    i2c_write(I2C_eeprom+1); //start reading

	addset(28,0,67,39);	//Set start Coordinate, start X, start Y, end X, end Y
	for(int i=0;i<1600;i++)
	{
		//do not acknowledge the last byte
		char ack = (i==1599 ? 0 : 1);
		char data=i2c_rx(ack);

		send(data,1);	
	}

	i2c_stop();
}