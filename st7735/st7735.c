#include <p24hj128gp202.h>
#include "delay.h"
#include "st7735.h"
#include "spi_lib.h"
#include "font.h"
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"
#include "uart.h"
#include "colors.h"

char madctl;

void write_spi_byte(unsigned char c){
	// using software SPI
	char x;
	for(x=0;x<8;x++){       //clock the byte out
		LCD_SCK = 0;
		LCD_SDO = 0;
		if(c & 0x80)
		LCD_SDO = 1;
		LCD_SCK = 1;
		c <<= 1;
	}

	// using hardware SPI
	// spi2Write(cmd);
}


void writecommand(unsigned char c)
{
	LCD_RS = 0;
	LCD_CS = 0;
	write_spi_byte(c);
	LCD_CS = 1;
}

void writedata(unsigned char c)
{
	LCD_RS = 1;
	LCD_CS = 0;
	write_spi_byte(c);
	LCD_CS = 1;
}

void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(x0+0);   // XSTART 
	writedata(0x00);
	writedata(x1+0);   // XEND

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(y0+0);    // YSTART
	writedata(0x00);
	writedata(y1+0);    // YEND

	writecommand(ST7735_RAMWR);  // write to RAM
}

void ST7735_pushColor(unsigned int color)
{
	LCD_RS = 1;
	LCD_CS = 0;
	write_spi_byte(color >> 8);
	write_spi_byte(color);
	LCD_CS = 1;
}

void ST7735_drawPixel(unsigned char x, unsigned char y, unsigned int color)
{
	setAddrWindow(x, y, x+1, y+1);

	LCD_RS = 1;
	LCD_CS = 0;

	write_spi_byte(color >> 8);    
	write_spi_byte(color);   

	LCD_CS = 1;
}

void ST7735_fillScreen(unsigned int color)
{
	unsigned char x;
	unsigned char y;

	setAddrWindow(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);

	// setup for data
	LCD_RS = 1;
	LCD_CS = 0;
	
	unsigned char colorB = color >> 8;

	for (x=0; x < SCREEN_WIDTH; x++) {
		for (y=0; y < SCREEN_HEIGHT; y++) {
			write_spi_byte(colorB);     
			write_spi_byte(color);  
		}
	}

	LCD_CS = 1;
}

void ST7735_initB(void) {
	LCD_RESET = 1;
	delay_us(500);
	LCD_RESET = 0;
	delay_us(500);
	LCD_RESET = 1;
	delay_us(500);

	LCD_CS = 0;

	writecommand(ST7735_SWRESET); // software reset
	delay_us(50);
	writecommand(ST7735_SLPOUT);  // out of sleep mode
	delay_us(500);

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color
	delay_us(10);

	writecommand(ST7735_FRMCTR1);  // frame rate control
	writedata(0x00);  // fastest refresh
	writedata(0x06);  // 6 lines front porch
	writedata(0x03);  // 3 lines backporch
	delay_us(10);

	writecommand(ST7735_MADCTL);  // memory access control (directions)
	writedata(0x08);  // row address/col address, bottom to top refresh
	madctl = 0x08;

	writecommand(ST7735_DISSET5);  // display settings #5
	writedata(0x15);  // 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
	writedata(0x02);  // fix on VTL

	writecommand(ST7735_INVCTR);  // display inversion control
	writedata(0x0);  // line inversion

	writecommand(ST7735_PWCTR1);  // power control
	writedata(0x02);      // GVDD = 4.7V 
	writedata(0x70);      // 1.0uA
	delay_us(10);
	writecommand(ST7735_PWCTR2);  // power control
	writedata(0x05);      // VGH = 14.7V, VGL = -7.35V 
	writecommand(ST7735_PWCTR3);  // power control
	writedata(0x01);      // Opamp current small 
	writedata(0x02);      // Boost frequency


	writecommand(ST7735_VMCTR1);  // power control
	writedata(0x3C);      // VCOMH = 4V
	writedata(0x38);      // VCOML = -1.1V
	delay_us(10);

	writecommand(ST7735_PWCTR6);  // power control
	writedata(0x11); 
	writedata(0x15);

	writecommand(ST7735_GMCTRP1);
	writedata(0x0f);	//writedata(0x09);  
	writedata(0x1a);  //writedata(0x16);
	writedata(0x0f);  //writedata(0x09);
	writedata(0x18);  //writedata(0x20);
	writedata(0x2f);  //writedata(0x21);
	writedata(0x28);  //writedata(0x1B);
	writedata(0x20);  //writedata(0x13);
	writedata(0x22);  //writedata(0x19);
	writedata(0x1f);  //writedata(0x17);
	writedata(0x1b);  //writedata(0x15);
	writedata(0x23);  //writedata(0x1E);
	writedata(0x37);  //writedata(0x2B);
	writedata(0x00);  //writedata(0x04);
	writedata(0x07);  //writedata(0x05);
	writedata(0x02);  //writedata(0x02);
	writedata(0x10);  //writedata(0x0E);
	writecommand(ST7735_GMCTRN1);
	writedata(0x0f);   //writedata(0x0B); 
	writedata(0x1b);   //writedata(0x14); 
	writedata(0x0f);   //writedata(0x08); 
	writedata(0x17);   //writedata(0x1E); 
	writedata(0x33);   //writedata(0x22); 
	writedata(0x2c);   //writedata(0x1D); 
	writedata(0x29);   //writedata(0x18); 
	writedata(0x2e);   //writedata(0x1E); 
	writedata(0x30);   //writedata(0x1B); 
	writedata(0x30);   //writedata(0x1A); 
	writedata(0x39);   //writedata(0x24); 
	writedata(0x3f);   //writedata(0x2B); 
	writedata(0x00);   //writedata(0x06); 
	writedata(0x07);   //writedata(0x06); 
	writedata(0x03);   //writedata(0x02); 
	writedata(0x10);   //writedata(0x0F); 
	delay_us(10);

	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(0x02);   // XSTART = 2
	writedata(0x00);
	writedata(0x81);   // XEND = 129

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(0x02);    // XSTART = 1
	writedata(0x00);
	writedata(0x81);    // XEND = 160

	writecommand(ST7735_NORON);  // normal display on
	delay_us(10);

	writecommand(ST7735_RAMWR);
	delay_us(500);

	writecommand(ST7735_DISPON);
	delay_us(500);

	LCD_CS = 1;
}

void ST7735_initR(void) {
	LCD_RESET = 1;
	delay_us(500);
	LCD_RESET = 0;
	delay_us(500);
	LCD_RESET = 1;
	delay_us(500);

	LCD_CS = 0;

	writecommand(ST7735_SWRESET); // software reset
	delay_us(150);

	writecommand(ST7735_SLPOUT);  // out of sleep mode
	delay_us(500);

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color
	delay_us(10);

	writecommand(ST7735_FRMCTR1);  // frame rate control - normal mode
	writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	writedata(0x2C); 
	writedata(0x2D); 

	writecommand(ST7735_FRMCTR2);  // frame rate control - idle mode
	writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	writedata(0x2C); 
	writedata(0x2D); 

	writecommand(ST7735_FRMCTR3);  // frame rate control - partial mode
	writedata(0x01); // dot inversion mode
	writedata(0x2C); 
	writedata(0x2D); 
	writedata(0x01); // line inversion mode
	writedata(0x2C); 
	writedata(0x2D); 

	writecommand(ST7735_INVCTR);  // display inversion control
	writedata(0x07);  // no inversion

	writecommand(ST7735_PWCTR1);  // power control
	writedata(0xA2);      
	writedata(0x02);      // -4.6V
	writedata(0x84);      // AUTO mode

	writecommand(ST7735_PWCTR2);  // power control
	writedata(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	writecommand(ST7735_PWCTR3);  // power control
	writedata(0x0A);      // Opamp current small 
	writedata(0x00);      // Boost frequency

	writecommand(ST7735_PWCTR4);  // power control
	writedata(0x8A);      // BCLK/2, Opamp current small & Medium low
	writedata(0x2A);     

	writecommand(ST7735_PWCTR5);  // power control
	writedata(0x8A);    
	writedata(0xEE);     

	writecommand(ST7735_VMCTR1);  // power control
	writedata(0x0E);  

	writecommand(ST7735_INVOFF);    // don't invert display

	writecommand(ST7735_MADCTL);  // memory access control (directions)

	// http://www.adafruit.com/forums/viewtopic.php?f=47&p=180341

	// R and B byte are swapped
	// madctl = 0xC8;

	// normal R G B order
	madctl = 0xC0;
	writedata(madctl);  // row address/col address, bottom to top refresh

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color

	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(0x00);   // XSTART = 0
	writedata(0x00);
	writedata(0x7F);   // XEND = 127

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(0x00);    // XSTART = 0
	writedata(0x00);
	writedata(0x9F);    // XEND = 159

	writecommand(ST7735_GMCTRP1);
	writedata(0x0f);
	writedata(0x1a);
	writedata(0x0f);
	writedata(0x18);
	writedata(0x2f);
	writedata(0x28);
	writedata(0x20);
	writedata(0x22);
	writedata(0x1f);
	writedata(0x1b);
	writedata(0x23);
	writedata(0x37);
	writedata(0x00);
	writedata(0x07);
	writedata(0x02);
	writedata(0x10);
	writecommand(ST7735_GMCTRN1);
	writedata(0x0f); 
	writedata(0x1b); 
	writedata(0x0f); 
	writedata(0x17); 
	writedata(0x33); 
	writedata(0x2c); 
	writedata(0x29); 
	writedata(0x2e); 
	writedata(0x30); 
	writedata(0x30); 
	writedata(0x39); 
	writedata(0x3f); 
	writedata(0x00); 
	writedata(0x07); 
	writedata(0x03); 
	writedata(0x10); 

	writecommand(ST7735_DISPON);
	delay_us(100);

	writecommand(ST7735_NORON);  // normal display on
	delay_us(10);

	LCD_CS = 1;
}

void ST7735_drawString(unsigned char x, unsigned char y, char *c, unsigned int color, unsigned char size)
{
	while (c[0] != 0) {
		ST7735_drawChar(x, y, c[0], color, size);
		x += size*6;
		c++;
		if (x + 5 >= SCREEN_WIDTH) {
			y += 10;
			x = 0;
		}
	}
}

void ST7735_drawChar(unsigned char x, unsigned char y, char c, unsigned int color, unsigned char size)
{
	unsigned char i, j;

	unsigned char letter = c < 0x52 ? c - 0x20 : c - 0x52;
	for (i =0; i<5; i++ ) {
		unsigned char line = c < 0x52 ? Alpha1[letter*5+i] : Alpha2[letter*5+i];

		for (j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
				ST7735_drawPixel(x+i, y+j, color);
				else {  // big size
					ST7735_fillRect(x+i*size, y+j*size, size, size, color);
				} 
			}
			line >>= 1;
		}
	}
}

// fill a circle
void ST7735_fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	ST7735_drawVerticalLine(x0, y0-r, 2*r+1, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7735_drawVerticalLine(x0+x, y0-y, 2*y+1, color);
		ST7735_drawVerticalLine(x0-x, y0-y, 2*y+1, color);
		ST7735_drawVerticalLine(x0+y, y0-x, 2*x+1, color);
		ST7735_drawVerticalLine(x0-y, y0-x, 2*x+1, color);
	}
}

// draw a circle outline
void ST7735_drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	ST7735_drawPixel(x0, y0+r, color);
	ST7735_drawPixel(x0, y0-r, color);
	ST7735_drawPixel(x0+r, y0, color);
	ST7735_drawPixel(x0-r, y0, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7735_drawPixel(x0 + x, y0 + y, color);
		ST7735_drawPixel(x0 - x, y0 + y, color);
		ST7735_drawPixel(x0 + x, y0 - y, color);
		ST7735_drawPixel(x0 - x, y0 - y, color);
		
		ST7735_drawPixel(x0 + y, y0 + x, color);
		ST7735_drawPixel(x0 - y, y0 + x, color);
		ST7735_drawPixel(x0 + y, y0 - x, color);
		ST7735_drawPixel(x0 - y, y0 - x, color);
		
	}
}

unsigned char ST7735_getRotation() {
	return madctl;
}

void ST7735_setRotation(unsigned char m) {
	madctl = m;
	writecommand(ST7735_MADCTL);  // memory access control (directions)
	writedata(madctl);  // row address/col address, bottom to top refresh
}

// draw a rectangle
void ST7735_drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, 
unsigned int color) {
	// smarter version
	ST7735_drawHorizontalLine(x, y, w, color);
	ST7735_drawHorizontalLine(x, y+h-1, w, color);
	ST7735_drawVerticalLine(x, y, h, color);
	ST7735_drawVerticalLine(x+w-1, y, h, color);
}

void ST7735_fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, 
unsigned int color) {
	// smarter version

	setAddrWindow(x, y, x+w-1, y+h-1);

	// setup for data
	LCD_RS = 1;
	LCD_CS = 0;

	unsigned char colorB = color >> 8;
	for (x=0; x < w; x++) {
		for (y=0; y < h; y++) {
			write_spi_byte(colorB);    
			write_spi_byte(color);    
		}
	}

	LCD_CS = 1;
}

void ST7735_drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
	if (x >= SCREEN_WIDTH) return;
	if (y+length >= SCREEN_HEIGHT) length = SCREEN_HEIGHT-y-1;

	ST7735_drawFastLine(x,y,length,color,1);
}

void ST7735_drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
	if (y >= SCREEN_HEIGHT) return;
	if (x+length >= SCREEN_WIDTH) length = SCREEN_WIDTH-x-1;

	ST7735_drawFastLine(x,y,length,color,0);
}

void ST7735_drawFastLine(unsigned char x, unsigned char y, unsigned char length, 
unsigned int color, unsigned char rotflag)
{
	if (rotflag) {
		setAddrWindow(x, y, x, y+length);
	} else {
		setAddrWindow(x, y, x+length, y+1);
	}
	// setup for data
	LCD_RS = 1;
	LCD_CS = 0;

	unsigned char colorB = color >> 8;
	while (length--) {
		write_spi_byte(colorB);    
		write_spi_byte(color);    
	}
	LCD_CS = 1;
}

int abs(int val)
{
	return (val > 0) ? val : -val;
}

// bresenham's algorithm - thx wikpedia
void ST7735_drawLine(int x0, int y0, int x1, int y1, 
unsigned int color) {
	unsigned int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	unsigned int dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int err = dx / 2;
	int ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;}

	for (; x0<=x1; x0++) {
		if (steep) {
			ST7735_drawPixel(y0, x0, color);
		} else {
			ST7735_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}


void loadBitmapToLCD(char* filename)
{
	char buf[30];
	int i;
	FSFILE * pointer;

#define BUFFER_LENGTH 1024
#define BMP_HEADER_SIZE 54

	unsigned char buffer[BUFFER_LENGTH];

	sprintf(buf, "Loading file %s", filename);	
	SendUARTStr(buf);

	pointer = FSfopen (filename, FS_READ);
	if (pointer == NULL)
	{
		SendUARTStr("Error opening file.");
		return;
	}

	SendUARTStr("Showing on LCD. Assuming 24-bit bitmap");

	unsigned int bytesRead = 0;
	unsigned char lastRed;
	unsigned char lastGreen;
	unsigned char lastBlue;
	unsigned char curX = 0;
	unsigned char curY = 0;
	unsigned int batchCount = 0;
	long totalBytes = 0;
	unsigned char bitmapWidth = 128;
	unsigned char bitmapHeight = 160;

	do
	{
		bytesRead = FSfread(buffer, 1, BUFFER_LENGTH, pointer);			

		for (i=(batchCount == 0 ? BMP_HEADER_SIZE : 0); i<bytesRead;i++)
		{	
			switch (totalBytes %3)
			{
			case 0:
				lastBlue = buffer[i];
				break;
			case 1:
				lastGreen = buffer[i];
				break;
			default:
				lastRed = buffer[i];

				unsigned int color = RGB565(lastRed, lastGreen, lastBlue);
				ST7735_drawPixel(curX, bitmapHeight-curY-1, color);

				curX++;
				
				if (curX == bitmapWidth)
				{
					curY++;
					curX = 0;
				}

				break;
			}

			totalBytes++;
		}

		batchCount++;
	}
	while (bytesRead > 0);

	curX++;
	sprintf(buf, "Read file done (%ld bytes)", totalBytes);	
	SendUARTStr(buf);

	FSfclose (pointer);
}
