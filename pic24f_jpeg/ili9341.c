#include "ili9341.h"
#include "spi_lib.h"
#include <p24hj128gp202.h>
#include "delay.h"
#include "math.h"
#include "font.h"
#include "uart.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FONT_SPACE 8

/*
http://arduino.cc/en/reference/constrain

Returns
x: if x is between a and b
a: if x is less than a
b: if x is greater than b 
*/
INT16U constrain(INT16U x, INT16U a, INT16U b)
{
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

void TFT_sendCMD(INT8U index)
{
    TFT_DC_LOW;
    TFT_CS_LOW;
    spi2Write(index);
    TFT_CS_HIGH;
}

void TFT_WRITE_DATA(INT8U data)
{
    TFT_DC_HIGH;
    TFT_CS_LOW;
    spi2Write(data);
    TFT_CS_HIGH;
}

void TFT_sendData(INT16U data)
{
    INT8U data1 = data>>8;
    INT8U data2 = data&0xff;
    TFT_DC_HIGH;
    TFT_CS_LOW;
    spi2Write(data1);
    spi2Write(data2);
    TFT_CS_HIGH;
}

void TFT_WRITE_Package(INT16U *data, INT8U howmany)
{
    INT16U    data1 = 0;
    INT8U   data2 = 0;

    TFT_DC_HIGH;
    TFT_CS_LOW;
    INT8U count=0;
    for(count=0;count<howmany;count++)
    {
        data1 = data[count]>>8;
        data2 = data[count]&0xff;
        spi2Write(data1);
        spi2Write(data2);
    }
    TFT_CS_HIGH;
}

INT8U TFT_Read_Register(INT8U Addr, INT8U xParameter)
{
    INT8U data=0;
    TFT_sendCMD(0xd9);                                                      /* ext command                  */
    TFT_WRITE_DATA(0x10+xParameter);                                        /* 0x11 is the first Parameter  */
    TFT_DC_LOW;
    TFT_CS_LOW;
    spi2Write(Addr);
    TFT_DC_HIGH;
    data = spi2Write(0);
    TFT_CS_HIGH;
    return data;
}


INT8U TFT_readID(void)
{
    INT8U i=0;
    INT8U data[3] ;
    INT8U ID[3] = {0x00, 0x93, 0x41};
    INT8U ToF=1;
    for(i=0;i<3;i++)
    {
        data[i]=TFT_Read_Register(0xd3,i+1);
        if(data[i] != ID[i])
        {
            ToF=0;
        }
    }
	
    if(!ToF)                                                            
    {
        // SendUARTStr("Read TFT ID failed, ID should be 0x09341, but read ID = 0x");

		/*
        for(i=0;i<3;i++)
        {
            Serial.print(data[i],HEX);
        }
        Serial.println();
		*/
    }

    return ToF;
}

void TFT_setCol(INT16U StartCol,INT16U EndCol)
{
    TFT_sendCMD(0x2A);                                                      /* Column Command address       */
    TFT_sendData(StartCol);
    TFT_sendData(EndCol);
}

void TFT_setPage(INT16U StartPage,INT16U EndPage)
{
    TFT_sendCMD(0x2B);                                                      /* Column Command address       */
    TFT_sendData(StartPage);
    TFT_sendData(EndPage);
}


void TFT_fillScreen(INT16U XL, INT16U XR, INT16U YU, INT16U YD, INT16U color)
{
    unsigned long  XY=0;
    unsigned long i=0;

    if(XL > XR)
    {
        XL = XL^XR;
        XR = XL^XR;
        XL = XL^XR;
    }
    if(YU > YD)
    {
        YU = YU^YD;
        YD = YU^YD;
        YU = YU^YD;
    }
    XL = constrain(XL, MIN_X,MAX_X);
    XR = constrain(XR, MIN_X,MAX_X);
    YU = constrain(YU, MIN_Y,MAX_Y);
    YD = constrain(YD, MIN_Y,MAX_Y);

    XY = (XR-XL+1);
    XY = XY*(YD-YU+1);

    TFT_setCol(XL,XR);
    TFT_setPage(YU, YD);
    TFT_sendCMD(0x2c);                                                  /* start to write to display ra */
                                                                        /* m                            */

    TFT_DC_HIGH;
    TFT_CS_LOW;

    INT8U Hcolor = color>>8;
    INT8U Lcolor = color&0xff;
    for(i=0; i < XY; i++)
    {
        spi2Write(Hcolor);
        spi2Write(Lcolor);
    }

    TFT_CS_HIGH;
}


//fillRectangle(poX+i*size, poY+f*size, size, size, fgcolor);
void TFT_fillRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color)
{
    TFT_fillScreen(poX, poX+length, poY, poY+width, color);
}

void TFT_fillScreen2(void)
{
	INT16U i;

    TFT_setCol(0, 239);
    TFT_setPage(0, 319);
    TFT_sendCMD(0x2c);                                                  /* start to write to display ra */
                                                                        /* m                            */

    TFT_DC_HIGH;
    TFT_CS_LOW;
    for(i=0; i<38400; i++)
    {
        spi2Write(0);
        spi2Write(0);
        spi2Write(0);
        spi2Write(0);
    }
    TFT_CS_HIGH;
}

unsigned char TFT_init (void)
{
    TFT_CS_HIGH;
    TFT_DC_HIGH;
    INT8U i=0, TFTDriver=0;
    for(i=0;i<3;i++)
    {
        TFTDriver = TFT_readID();
    }

	// LCD not detected
	if (TFTDriver == 0)
		return 0;

    delay_ms(500);
    TFT_sendCMD(0x01);
    delay_ms(200);

    TFT_sendCMD(0xCF);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x8B);
    TFT_WRITE_DATA(0X30);

    TFT_sendCMD(0xED);
    TFT_WRITE_DATA(0x67);
    TFT_WRITE_DATA(0x03);
    TFT_WRITE_DATA(0X12);
    TFT_WRITE_DATA(0X81);

    TFT_sendCMD(0xE8);
    TFT_WRITE_DATA(0x85);
    TFT_WRITE_DATA(0x10);
    TFT_WRITE_DATA(0x7A);

    TFT_sendCMD(0xCB);
    TFT_WRITE_DATA(0x39);
    TFT_WRITE_DATA(0x2C);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x34);
    TFT_WRITE_DATA(0x02);

    TFT_sendCMD(0xF7);
    TFT_WRITE_DATA(0x20);

    TFT_sendCMD(0xEA);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x00);

    TFT_sendCMD(0xC0);                                                      /* Power control                */
    TFT_WRITE_DATA(0x1B);                                                   /* VRH[5:0]                     */

    TFT_sendCMD(0xC1);                                                      /* Power control                */
    TFT_WRITE_DATA(0x10);                                                   /* SAP[2:0];BT[3:0]             */

    TFT_sendCMD(0xC5);                                                      /* VCM control                  */
    TFT_WRITE_DATA(0x3F);
    TFT_WRITE_DATA(0x3C);

    TFT_sendCMD(0xC7);                                                      /* VCM control2                 */
    TFT_WRITE_DATA(0XB7);

    TFT_sendCMD(0x36);                                                      /* Memory Access Control        */
    TFT_WRITE_DATA(0x08);

    TFT_sendCMD(0x3A);
    TFT_WRITE_DATA(0x55);

    TFT_sendCMD(0xB1);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x1B);

    TFT_sendCMD(0xB6);                                                      /* Display Function Control     */
    TFT_WRITE_DATA(0x0A);
    TFT_WRITE_DATA(0xA2);


    TFT_sendCMD(0xF2);                                                      /* 3Gamma Function Disable      */
    TFT_WRITE_DATA(0x00);

    TFT_sendCMD(0x26);                                                      /* Gamma curve selected         */
    TFT_WRITE_DATA(0x01);

    TFT_sendCMD(0xE0);                                                      /* Set Gamma                    */
    TFT_WRITE_DATA(0x0F);
    TFT_WRITE_DATA(0x2A);
    TFT_WRITE_DATA(0x28);
    TFT_WRITE_DATA(0x08);
    TFT_WRITE_DATA(0x0E);
    TFT_WRITE_DATA(0x08);
    TFT_WRITE_DATA(0x54);
    TFT_WRITE_DATA(0XA9);
    TFT_WRITE_DATA(0x43);
    TFT_WRITE_DATA(0x0A);
    TFT_WRITE_DATA(0x0F);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x00);

    TFT_sendCMD(0XE1);                                                      /* Set Gamma                    */
    TFT_WRITE_DATA(0x00);
    TFT_WRITE_DATA(0x15);
    TFT_WRITE_DATA(0x17);
    TFT_WRITE_DATA(0x07);
    TFT_WRITE_DATA(0x11);
    TFT_WRITE_DATA(0x06);
    TFT_WRITE_DATA(0x2B);
    TFT_WRITE_DATA(0x56);
    TFT_WRITE_DATA(0x3C);
    TFT_WRITE_DATA(0x05);
    TFT_WRITE_DATA(0x10);
    TFT_WRITE_DATA(0x0F);
    TFT_WRITE_DATA(0x3F);
    TFT_WRITE_DATA(0x3F);
    TFT_WRITE_DATA(0x0F);

    TFT_sendCMD(0x11);                                                      /* Exit Sleep                   */
    delay_ms(120);
    TFT_sendCMD(0x29);                                                      /* Display on                   */
    TFT_fillScreen2();

	return 1;
}

void TFT_setXY(INT16U poX, INT16U poY)
{
    TFT_setCol(poX, poX);
    TFT_setPage(poY, poY);
    TFT_sendCMD(0x2c);
}

void TFT_setPixel(INT16U poX, INT16U poY,INT16U color)
{
    TFT_setXY(poX, poY);
    TFT_sendData(color);
}

// the default font looks ugly, especially with lower case character in size 1. MD modifies here to use MD's font for it to look better.
void TFT_drawChar( INT8U ascii, INT16U poX, INT16U poY,INT16U size, INT16U fgcolor)
{
	unsigned char i, j;

	unsigned char letter = ascii < 0x52 ? ascii - 0x20 : ascii - 0x52;

	for (i =0; i<5; i++ ) 
	{
		unsigned char line = ascii < 0x52 ? Alpha1[letter*5+i] : Alpha2[letter*5+i];

		for (j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					TFT_setPixel(poX+i, poY+j, fgcolor);
				else {  // big size
					// TFT_fillRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color)

					TFT_fillRectangle(poX+i*size, poY+j*size, size, size, fgcolor);
				} 
			}
			line >>= 1;
		}
	}
}

void TFT_drawString(char *string,INT16U poX, INT16U poY, INT16U size,INT16U fgcolor)
{
    while(*string)
    {
        TFT_drawChar(*string, poX, poY, size, fgcolor);
        *string++;

        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
    }
}

void  TFT_drawHorizontalLine( INT16U poX, INT16U poY,
INT16U length,INT16U color)
{
	int i;

    TFT_setCol(poX,poX + length);
    TFT_setPage(poY,poY);
    TFT_sendCMD(0x2c);
    for(i=0; i<length; i++)
    TFT_sendData(color);
}

void TFT_drawLine( INT16U x0,INT16U y0,INT16U x1, INT16U y1,INT16U color)
{

    int x = x1-x0;
    int y = y1-y0;
    int dx = fabs(x), sx = x0<x1 ? 1 : -1;
    int dy = -fabs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                                                /* error value e_xy             */
    for (;;){                                                           /* loop                         */
        TFT_setPixel(x0,y0,color);
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

void TFT_drawVerticalLine( INT16U poX, INT16U poY, INT16U length,INT16U color)
{
	int i;

    TFT_setCol(poX,poX);
    TFT_setPage(poY,poY+length);
    TFT_sendCMD(0x2c);
    for (i=0; i<length; i++)
    TFT_sendData(color);
}

void TFT_drawRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width,INT16U color)
{
    TFT_drawHorizontalLine(poX, poY, length, color);
    TFT_drawHorizontalLine(poX, poY+width, length, color);
    TFT_drawVerticalLine(poX, poY, width,color);
    TFT_drawVerticalLine(poX + length, poY, width,color);

}

void TFT_drawCircle(int poX, int poY, int r,INT16U color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        TFT_setPixel(poX-x, poY+y,color);
        TFT_setPixel(poX+x, poY+y,color);
        TFT_setPixel(poX+x, poY-y,color);
        TFT_setPixel(poX-x, poY-y,color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void TFT_fillCircle(int poX, int poY, int r,INT16U color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {

        TFT_drawVerticalLine(poX-x, poY-y, 2*y, color);
        TFT_drawVerticalLine(poX+x, poY-y, 2*y, color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}

void TFT_drawTraingle( int poX1, int poY1, int poX2, int poY2, int poX3, int poY3, INT16U color)
{
    TFT_drawLine(poX1, poY1, poX2, poY2,color);
    TFT_drawLine(poX1, poY1, poX3, poY3,color);
    TFT_drawLine(poX2, poY2, poX3, poY3,color);
}

INT8U TFT_drawNumber(long long_num,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor)
{
    INT8U char_buffer[10] = "";
    INT8U i = 0;
    INT8U f = 0;

    if (long_num < 0)
    {
        f=1;
        TFT_drawChar('-',poX, poY, size, fgcolor);
        long_num = -long_num;
        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
    }
    else if (long_num == 0)
    {
        f=1;
        TFT_drawChar('0',poX, poY, size, fgcolor);
        return f;
        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
    }


    while (long_num > 0)
    {
        char_buffer[i++] = long_num % 10;
        long_num /= 10;
    }

    f = f+i;
    for(; i > 0; i--)
    {
        TFT_drawChar('0'+ char_buffer[i - 1],poX, poY, size, fgcolor);
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
    }
    return f;
}

INT8U TFT_drawFloat(float floatNumber,INT8U decimal,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor)
{
    INT16U temp=0;
    float decy=0.0;
    float rounding = 0.5;
    INT8U f=0;
	INT8U i;
	
    if(floatNumber<0.0)
    {
        TFT_drawChar('-',poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
        f =1;
    }
    for (i=0; i<decimal; ++i)
    {
        rounding /= 10.0;
    }
    floatNumber += rounding;

    temp = (INT16U)floatNumber;
    INT8U howlong=TFT_drawNumber(temp,poX, poY, size, fgcolor);
    f += howlong;
    if((poX+8*size*howlong) < MAX_X)
    {
        poX+=FONT_SPACE*size*howlong;                                   /* Move cursor right            */
    }

    if(decimal>0)
    {
        TFT_drawChar('.',poX, poY, size, fgcolor);
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
        f +=1;
    }
    decy = floatNumber-temp;                                            /* decimal part,  4             */
    for(i=0;i<decimal;i++)                                      
    {
        decy *=10;                                                      /* for the next decimal         */
        temp = decy;                                                    /* get the decimal              */
        TFT_drawNumber(temp,poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
        decy -= temp;
    }
    f +=decimal;
    return f;
}

INT8U TFT_drawFloat2(float floatNumber,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor)
{
    INT8U decimal=2;
    INT16U temp=0;
    float decy=0.0;
    float rounding = 0.5;
    INT8U f=0;
	INT8U i;

    if(floatNumber<0.0)                                                 /* floatNumber < 0              */
    {
        TFT_drawChar('-',poX, poY, size, fgcolor);                          /* add a '-'                    */
        floatNumber = -floatNumber;
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
        f =1;
    }
    for (i=0; i<decimal; ++i)
    {
        rounding /= 10.0;
    }
    floatNumber += rounding;

    temp = (INT16U)floatNumber;
    INT8U howlong=TFT_drawNumber(temp,poX, poY, size, fgcolor);
    f += howlong;
    if((poX+8*size*howlong) < MAX_X)
    {
        poX+=FONT_SPACE*size*howlong;                                   /* Move cursor right            */
    }


    if(decimal>0)
    {
        TFT_drawChar('.',poX, poY, size, fgcolor);
        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
        f +=1;
    }
    decy = floatNumber-temp;                                            /* decimal part,                */
    for(i=0;i<decimal;i++)
    {
        decy *=10;                                                      /* for the next decimal         */
        temp = decy;                                                    /* get the decimal              */
        TFT_drawNumber(temp,poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
        decy -= temp;
    }
    f += decimal;
    return f;
}
