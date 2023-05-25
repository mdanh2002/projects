/* 
Interface with Nokia 5110 LCD. Code taken from http://ghmicro.com/index.php?option=com_content&view=article&id=38:nokia-5110-lcd&catid=15:display.

The LCD is designed to work at 3.3V. However, it can withstand the 5V voltage from the PIC output as long as a 10k resistor is connected in series
*/

#include "5110_lcd.h"
#include "pic.h"
#include "eeprom.h"
#include "serial.h"

// delay for approximately the given amount of msec
// correct on PIC16F88 @ 20MHz
void delay_ms(unsigned  int ii)
{
	unsigned int i,x;
	for (x=0;x<ii;x++)
	{
	for (i=0;i<230;i++);
	}
}

void lcd_5110_string(const char *str,unsigned char row){ 
    lcd_5110_send(0x40 + row,0); //Y address
    lcd_5110_send(0x80, 0);       //X address

	//STRANGE, need to reset PORTA output, otherwise it can only display first character of a string
	//subsequent read from EEPROM will fail.
	PORTA = 0xFF;
	PORTA = 0;

    while(*str)
	{
        lcd_5110_char(*str++);
		
		//same as above
		PORTA = 0xFF;
		PORTA = 0;
	}
}        

void lcd_5110_char(unsigned char letter){
    unsigned char col;

	//prepare to read the 5110 font set (8x5) from the EEPROM first, 5 byte per char
	i2c_start();              
    i2c_write(I2C_eeprom);
	int offset = 2956 + (letter-32)*5;
	i2c_write(offset>>8); 
	i2c_write(offset);	
	i2c_start(); 	
    i2c_write(I2C_eeprom+1); 

    for(col=0;col<5;col++){
		//read the byte and ack, except for the last byte
		char ack = (col==4 ? 0 : 1);
		char data=i2c_rx(ack);

		//write to display memory
        lcd_5110_send(data,1);
    }

    lcd_5110_send(0,1);                          //send space
	
	i2c_stop();
}        

// display a test monochrone bitmap on the 5110 LCD read from EEPROM at offset 3436
void lcd_5110_bitmap()
{	
	PORTA = 0xFF;
	PORTA = 0;

	for (int i=0; i<80;i++)
	{
    	unsigned char row;
		for (row=0;row<5;row++)
		{
			char data=read_24Cxx(3436 + i * 5 + row);

    		lcd_5110_send(0x40 + row,0); //Y address
		    lcd_5110_send(0x80 + i,0);   //X address

			//write to display memory
       		lcd_5110_send(data,1);

			PORTA = 0xFF;
			PORTA = 0;
		}
	}

	i2c_stop();
}

void lcd_5110_send(unsigned char cmd, unsigned char type){
    char x;
    D_C = type;             //set char/data type
    SCE = 0;
    for(x=0;x<8;x++){       //clock the byte out
        SCLK = 0;
        MOSI = 0;
        if(cmd & 0x80)
            MOSI = 1;
        SCLK = 1;
        cmd <<= 1;
    }
    SCE = 1;
}
        
void lcd_5110_cls(void){
    unsigned int i;
    lcd_5110_send(0x40,0);       //Y address
    lcd_5110_send(0x80,0);       //X address
    for(i=0;i<0x504;i++){   //write zeros to display RAM
        lcd_5110_send(0x00,1);
    }
}

void lcd_5110_black(void){
    unsigned int i;
    lcd_5110_send(0x40,0);       //Y address
    lcd_5110_send(0x80,0);       //X address
    for(i=0;i<0x504;i++){   //write black to display RAM
        lcd_5110_send(0xFF,1);
    }
}

void lcd_5110_init(void){
    delay_ms(15);
    RST = 0;delay_ms(20);RST = 1;   //reset display
  //lcd_send(command + value,0);
    lcd_5110_send(0x20 + 0x01,0);    //function set - extended instructions enabled
    lcd_5110_send(0x80 + 0x40,0);    //set vop (contrast) 0 - 127
    lcd_5110_send(0x04 + 0x02,0);    //temperature control
    lcd_5110_send(0x10 + 0x03,0);    //set bias system
    lcd_5110_send(0x20 + 0x00,0);    //function set - chip active, horizontal addressing, basic instructions
    lcd_5110_send(0x08 + 0x04,0);    //display control - normal mode
    lcd_5110_cls();
}