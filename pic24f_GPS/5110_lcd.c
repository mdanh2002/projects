/* 
Interface with Nokia 5110 LCD. Code taken from http://ghmicro.com/index.php?option=com_content&view=article&id=38:nokia-5110-lcd&catid=15:display.

The LCD is designed to work at 3.3V. However, it can withstand the 5V voltage from the PIC output as long as a 10k resistor is connected in series
*/

#include "5110_lcd.h"
#include "font.h"
#include <p24fj64ga002.h>

// #include "spi_lib.h"

//The 5110 LCD is monochrome, uses the PCD8544 controller and has a resolution of 48 rows × 84 columns. Each 8 pixels on a single column consumes a single byte on the LCD memory map. It takes 504 bytes to fill the entire LCD.

void lcd_5110_send(unsigned char cmd, unsigned char type){
    D_C = type;             //set char/data type
    SCE = 0;

	// using software SPI
    char x;
    for(x=0;x<8;x++){       //clock the byte out
        SCLK = 0;
        MOSI = 0;
        if(cmd & 0x80)
            MOSI = 1;
        SCLK = 1;
        cmd <<= 1;
    }

	// using hardware SPI
	// spi1Write(cmd);

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
	int i;
	for (i=0;i<50;i++); //delay for a while

    RST = 0;
	for (i=0;i<50;i++); //delay for a while

	RST = 1;   //reset display

  	//lcd_send(command + value,0);
    lcd_5110_send(0x20 + 0x01,0);    //function set - extended instructions enabled
    lcd_5110_send(0x80 + 0x40,0);    //set vop (contrast) 0 - 127
    lcd_5110_send(0x04 + 0x02,0);    //temperature control
    lcd_5110_send(0x10 + 0x03,0);    //set bias system
    lcd_5110_send(0x20 + 0x00,0);    //function set - chip active, horizontal addressing, basic instructions
    lcd_5110_send(0x08 + 0x04,0);    //display control - normal mode
    lcd_5110_cls();
}

void lcd_char(unsigned char letter){
    unsigned char col;
    if(letter < 0x52){
        letter = letter - 0x20;
        for(col=0;col<5;col++){
            lcd_5110_send(Alpha1[(letter*5)+col],1);
        }
    }
    else{
        letter = letter - 0x52;
        for(col=0;col<5;col++){
            lcd_5110_send(Alpha2[(letter*5)+col],1);
        }
    }
    lcd_5110_send(0,1);                          //send space
}  

void lcd_string(const char *str,unsigned char row)
{
    lcd_5110_send(0x40+row,0);
    lcd_5110_send(0x80,0);

    while(*str)
	{
        lcd_char(*str++);
	}	
}
