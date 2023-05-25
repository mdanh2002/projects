#include "xpt2046.h"
#include <p24fj64ga002.h>
#include "spi_lib.h"
#include "delay.h"
#include "uart.h"
#include <stdio.h>
#include "ili9320.h"

double cal_x = 1.0;
double cal_y = 1.0;

unsigned int _xpt2046_get_reading( unsigned char control )
{
	TP_CS = 0;

	unsigned char rData[3] = { 0 , 0 , 0 };

	rData[0] = spi1Write(control);
	rData[1] = spi1Write(0);
	rData[2] = spi1Write(0);

	if ( ( control & 0x08 ) == 0 ) {
		return ( rData[1] << 5 ) | ( rData[2] >> 3 );
	}
	return ( rData[1] << 4 ) | ( rData[2] >> 4 );

	TP_CS = 1;
}

unsigned int touch_Init(void)
{
	// toggle the CS line to activate the touch controller
	TP_CS = 1;
	delay_us(10);
	TP_CS = 0;

	// calculate the calibration constants
	cal_x = (TOUCH_X1 - TOUCH_X0) / MAX_X;
	cal_y = (TOUCH_Y1 - TOUCH_Y0) / MAX_Y;

	// Read a sample, leaving PENIRQ active
	return _xpt2046_get_reading( 0x90 );
}

void xpt2046GetCoordinates(unsigned int * pX, unsigned int * pY)
{
	int i;
	unsigned int allX[ 7 ] , allY[ 7 ];
	_xpt2046_get_reading( 0xd1 );
	_xpt2046_get_reading( 0x91 );
	for ( i = 0 ; i < 7 ; i ++ ) {
		allX[ i ] = _xpt2046_get_reading( 0xd1 );
		allY[ i ] = _xpt2046_get_reading( 0x91 );
	}

	int j;
	for ( i = 0 ; i < 4 ; i ++ ) {
		for ( j = i ; j < 7 ; j ++ ) {
			int temp = allX[ i ];
			if ( temp > allX[ j ] ) {
				allX[ i ] = allX[ j ];
				allX[ j ] = temp;
			}
			temp = allY[ i ];
			if ( temp > allY[ j ] ) {
				allY[ i ] = allY[ j ];
				allY[ j ] = temp;
			}
		}
	}
	_xpt2046_get_reading( 0x90 );

	*pX = allX[ 3 ];
	*pY = allY[ 3 ];
}

unsigned char xpt2046GetAverageCoordinates(unsigned int * pX , unsigned int * pY , int nSamples )
{
	int nRead = 0;
	unsigned int xAcc = 0 , yAcc = 0;
	unsigned int x , y;
	
	x = 0;
	y = 0;
	while ( nRead < nSamples ) {
		// data no longer available
		if (TP_IRQ) 
			return 0;

		xpt2046GetCoordinates(&x , &y);

		xAcc += x;
		yAcc += y;
		nRead ++;


		// sprintf(buf, "Added: %u %u (%d)", x, y, nRead);
		// SendUARTStr(buf);	

		// no more data available
		if (TP_IRQ) break;		
	}

	if (nRead >= 3)
	{
		*pX = xAcc / nRead;
		*pY = yAcc / nRead;
	
		return 1;
	}
	else
		// too few samples collected, ignore
		return 0;
}

unsigned char touch_GetCoordinates(unsigned int * pX, unsigned int * pY)
{
	// get the raw touch coordinates
	unsigned int tX = 0;
	unsigned int tY = 0;
	unsigned char isOK = xpt2046GetAverageCoordinates(&tX, &tY, 5);

	// convert to LCD coordinates
	if (isOK)
	{
		*pX = (tX - TOUCH_X0) / cal_x;
		*pY = (tY - TOUCH_Y0) / cal_y;
	}

	return isOK;
}
