//**************************************************************
//   ******** FUNCTIONS FOR DS1621 TEMP SENSOR  *******
//**************************************************************
//Controller:	ATmega8 (1Mhz Crystal)
//Compiler:		ICCAVR
//Author:		CC Dharmani, Chennai (India)
//			www.dharmanitech.com
//Date:			Sep 2008
//**************************************************************

#include "ds1621.h"
#include "i2c.h"
#include <delays.h>

char temp_config(char data) {
   int i;
   char val;

   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xac); //access config
   i2c_write(data);
   i2c_stop();

	// short delay
   for (i=0;i<1000;i++);

	// read it back. If matched, that means the DS1621 is operating properly.
   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xAC); //access config
   i2c_start();
   i2c_write(DS1621_R);
   val =i2c_rx(0);
   i2c_stop();

	return (val==data);
}

char ds1621_init(void) {
   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xee); //start convert
   i2c_stop();

   return temp_config(0b00001000);
}

float ds1621_read_temp(void) {        ////// Returns degrees F (0-255)
   unsigned char datah;
   char datal;

   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xaa); //read temperature
   i2c_start();
   i2c_write(DS1621_R);
   datah=i2c_rx(1); //first byte is integer value
   datal=i2c_rx(0); //second byte is fractional part
   i2c_stop();

   // The second byte  would contain the value of the least significant (0.5C) bit of the temperature reading as shown in Table 
   // Note that the remaining 7 bits of this byte are set to all "0"s. 
   if (datal > 0)
	 return (float)datah + 0.5;
   else
	 return datah;
}

float ds1621_read_temp_precise(void)
{
	float temp;
	char slope;
	char counter;
	temp = ds1621_read_temp();

	// read the slope value
   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xA9); //read slope
   i2c_start();
   i2c_write(DS1621_R);
   slope =i2c_rx(0);
   i2c_stop();

	// read the counter value
   i2c_start();
   i2c_write(DS1621_W);
   i2c_write(0xA8); //read counter
   i2c_start();
   i2c_write(DS1621_R);
   counter =i2c_rx(0);
   i2c_stop();

	return (float)temp - 0.25 + ((float)(slope - counter) / (float)slope);
}