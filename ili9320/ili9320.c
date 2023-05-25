#include "ili9320.h"
#include <p24fj64ga002.h>
#include "ili9320.h"
#include "spi_lib.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>
#include "font.h"
#include "math.h"
#include "uart.h"

void LCD_WriteIndex(uint8_t index)
{
	SPI_CS_LOW;

	/* SPI write data */
	spi2Write(SPI_START | SPI_WR | SPI_INDEX);   /* Write : RS = 0, RW = 0       */
	spi2Write(0);
	spi2Write(index);

	SPI_CS_HIGH; 
}

void LCD_WriteData( uint16_t data)
{
	SPI_CS_LOW;

	spi2Write(SPI_START | SPI_WR | SPI_DATA);    /* Write : RS = 1, RW = 0       */
	spi2Write((data >>   8));                    /* Write D8..D15                */
	spi2Write((data & 0xFF));                    /* Write D0..D7                 */

	SPI_CS_HIGH; 
}

void LCD_Write_Data_Start(void) 
{
	spi2Write(SPI_START | SPI_WR | SPI_DATA);    /* Write : RS = 1, RW = 0       */
}

void LCD_Write_Data_Only( uint16_t data) 
{
	spi2Write((data >>   8));      /* Write D8..D15                */
	spi2Write((data & 0xFF));      /* Write D0..D7                 */
}


uint16_t LCD_ReadData(void)
{ 
	uint16_t value;

	SPI_CS_LOW;

	spi2Write(SPI_START | SPI_RD | SPI_DATA);    /* Read: RS = 1, RW = 1         */
	spi2Write(0);                                /* Dummy read 1                 */
	value   = spi2Write(0);                      /* Read D8..D15                 */

	// char buf[50];
	// sprintf(buf, "%d", value);
	// SendUARTStr(buf);

	value <<= 8;
	value  |= spi2Write(0);                      /* Read D0..D7                  */

	// sprintf(buf, "%d", value);
	// SendUARTStr(buf);

	SPI_CS_HIGH; 
	
	return value;
}


void LCD_WriteReg( uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}

uint16_t LCD_ReadReg( uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);


	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();
	
	return LCD_RAM;
}


static void LCD_SetCursor( uint16_t Xpos, uint16_t Ypos )
{
	/* 0x9320 */
	LCD_WriteReg(0x0020, Xpos );     
	LCD_WriteReg(0x0021, Ypos );           
}

unsigned char LCD_Init(void)
{
	uint16_t DeviceCode;

	DeviceCode = LCD_ReadReg(0x0000);		/* Read ID	*/	

	/* Different driver IC initialization */
	if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100); /* Driver Output Contral */
		LCD_WriteReg(0x02,0x0700); /* LCD Driver Waveform Contral */
		LCD_WriteReg(0x03,0x1038); /* Set the scan mode */
		LCD_WriteReg(0x04,0x0000); /* Scalling Contral */
		LCD_WriteReg(0x08,0x0202); /* Display Contral 2 */
		LCD_WriteReg(0x09,0x0000); /* Display Contral 3 */
		LCD_WriteReg(0x0a,0x0000); /* Frame Cycle Contal */
		LCD_WriteReg(0x0c,(1<<0)); /* Extern Display Interface Contral 1 */
		LCD_WriteReg(0x0d,0x0000); /* Frame Maker Position */
		LCD_WriteReg(0x0f,0x0000); /* Extern Display Interface Contral 2 */     
		delay_ms(50); 
		LCD_WriteReg(0x07,0x0101); /* Display Contral */
		delay_ms(50);    
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4)); /* Power Control 1 */
		LCD_WriteReg(0x11,0x0007);                              /* Power Control 2 */
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));                /* Power Control 3 */
		LCD_WriteReg(0x13,0x0b00);                              /* Power Control 4 */
		LCD_WriteReg(0x29,0x0000);                              /* Power Control 7 */
		LCD_WriteReg(0x2b,(1<<14)|(1<<4)); 
		
		LCD_WriteReg(0x50,0);       /* Set X Start */
		LCD_WriteReg(0x51,239);	    /* Set X End */
		LCD_WriteReg(0x52,0);	    /* Set Y Start */
		LCD_WriteReg(0x53,319);	    /* Set Y End */
		delay_ms(50);

		LCD_WriteReg(0x60,0x2700); /* Driver Output Control */
		LCD_WriteReg(0x61,0x0001); /* Driver Output Control */
		LCD_WriteReg(0x6a,0x0000); /* Vertical Srcoll Control */
		
		LCD_WriteReg(0x80,0x0000); /* Display Position? Partial Display 1 */
		LCD_WriteReg(0x81,0x0000); /* RAM Address Start? Partial Display 1 */
		LCD_WriteReg(0x82,0x0000); /* RAM Address End-Partial Display 1 */
		LCD_WriteReg(0x83,0x0000); /* Displsy Position? Partial Display 2 */
		LCD_WriteReg(0x84,0x0000); /* RAM Address Start? Partial Display 2 */
		LCD_WriteReg(0x85,0x0000); /* RAM Address End? Partial Display 2 */
		
		LCD_WriteReg(0x90,(0<<7)|(16<<0)); /* Frame Cycle Contral */
		LCD_WriteReg(0x92,0x0000);         /* Panel Interface Contral 2 */
		LCD_WriteReg(0x93,0x0001);         /* Panel Interface Contral 3 */
		LCD_WriteReg(0x95,0x0110);         /* Frame Cycle Contral */
		LCD_WriteReg(0x97,(0<<8)); 
		LCD_WriteReg(0x98,0x0000);         /* Frame Cycle Contral */    
		LCD_WriteReg(0x07,0x0133); 
		
		delay_ms(100);  

		return 1;
	}		

	return 0;
}


void LCD_Clear( uint16_t Color)
{
	unsigned long index=0;
	
	LCD_SetCursor(0,0); 

	LCD_WriteIndex(0x0022);

	SPI_CS_LOW; 
	LCD_Write_Data_Start();

	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_Write_Data_Only(Color);
	}
	SPI_CS_HIGH;
}


static uint16_t LCD_BGR2RGB( uint16_t color)
{
	uint16_t  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}


uint16_t LCD_GetPoint( uint16_t Xpos, uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);

	LCD_WriteIndex(0x0022);  
	
	dummy = LCD_ReadData();   /* An empty read */
	dummy = LCD_ReadData(); 

	return  LCD_BGR2RGB( dummy );
}


void LCD_SetPoint( uint16_t Xpos, uint16_t Ypos, uint16_t color)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022, color);
}

void LCD_drawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{

    int x = x1-x0;
    int y = y1-y0;
    int dx = fabs(x), sx = x0<x1 ? 1 : -1;
    int dy = -fabs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                                                /* error value e_xy             */
    for (;;){                                                           /* loop                         */
        LCD_SetPoint(x0,y0,color);
        e2 = 2*err;
        if (e2 >= dy) {                                                 /* e_xy+e_x > 0                 */
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                                                 /* e_xy+e_y < 0                 */
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }

}

void LCD_fillRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color)
{
	uint16_t x, y;
    for (x=poX;x<poX+length;x++)
    	for (y=poY;y<poY+width;y++)
		{
			LCD_SetPoint(x, y, color);
		}
}

void LCD_drawRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color)
{
    LCD_drawLine(poX, poY, poX+length, poY, color);
    LCD_drawLine(poX, poY+width, poX+length, poY+width, color);
    LCD_drawLine(poX, poY, poX, poY+width, color);
    LCD_drawLine(poX + length, poY, poX + length, poY+width, color);

}

void LCD_drawCircle(int poX, int poY, int r, uint16_t color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        LCD_SetPoint(poX-x, poY+y,color);
        LCD_SetPoint(poX+x, poY+y,color);
        LCD_SetPoint(poX+x, poY-y,color);
        LCD_SetPoint(poX-x, poY-y,color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void LCD_fillCircle(int poX, int poY, int r, uint16_t color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {

		LCD_drawLine(poX-x, poY-y, poX-x, poY-y+2*y, color);
		LCD_drawLine(poX+x, poY-y, poX+x, poY-y+2*y, color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}

void LCD_PutChar(uint8_t ascii, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor)
{
	unsigned char i, j;
	unsigned char letter = ascii < 0x52 ? ascii - 0x20 : ascii - 0x52;

	for (i =0; i<5; i++ ) 
	{
		unsigned char line = ascii < 0x52 ? Alpha1[letter*5+i] : Alpha2[letter*5+i];

		for (j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					LCD_SetPoint(poX+i, poY+j, fgcolor);				
				else {  // big size	 		
					LCD_fillRectangle(poX+i*size, poY+j*size, size, size, fgcolor);
				} 
			}
			line >>= 1;
		}
	}
}

void LCD_drawString(uint16_t Xpos, uint16_t Ypos, uint16_t size, char *str, uint16_t Color)
{
	uint8_t TempChar;
	do
	{
		TempChar = *str++;  
		LCD_PutChar(TempChar, Xpos, Ypos, size, Color);    
		if( Xpos < MAX_X - 8 )
		{
			Xpos += 8*size;
		} 
		else if ( Ypos < MAX_Y - 16 )
		{
			Xpos = 0;
			Ypos += 16*size;
		}   
		else
		{
			Xpos = 0;
			Ypos = 0;
		}    
	}
	while ( *str != 0 );
}
