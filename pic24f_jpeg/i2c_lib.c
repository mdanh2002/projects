#include "i2c_lib.h"
#include <p24fj64ga002.h>
#include "delay.h"

// null function to provide some rest time
void i2c_dly(void)
{
	delay_us(1);
}

void i2c_start(void)
{
  SDA = 1;             // i2c start bit sequence
  i2c_dly();
  SCL = 1;
  i2c_dly();
  SDA = 0;
  i2c_dly();
  SCL = 0;
  i2c_dly();
}

void i2c_stop(void)
{
  SDA = 0;             // i2c stop bit sequence
  i2c_dly();
  SCL = 1;
  i2c_dly();
  SDA = 1;
  i2c_dly();
}

unsigned char i2c_rx(char ack)
{
	unsigned char x;
	unsigned char d=0;
	int count = 0;

  SDA = 1;  
  for(x=0; x<8; x++) {
    d <<= 1;

     do {
      SCL = 1;
	  count++;
     }
     while (SCL_IN==0 && count < 10000);
	// delay_us(1);
    if(SDA_IN) d |= 1;
    SCL = 0;
  } 
  if(ack) SDA = 0;
  else SDA = 1;
  SCL = 1;
  i2c_dly();             // send (N)ACK bit
  SCL = 0;
  SDA = 1;
  return d;
}

char i2c_write(unsigned char d)
{
char x;
	static char b;
  for(x=8; x; x--) {
    if(d&0x80) SDA = 1;
    else SDA = 0;
    SCL = 1;
    d <<= 1;
	delay_us(1);
    SCL = 0;
  }
  SDA = 1;
  SCL = 1;
  i2c_dly();
  b = SDA_IN;          // possible ACK bit
  SCL = 0;
  return b;
}
