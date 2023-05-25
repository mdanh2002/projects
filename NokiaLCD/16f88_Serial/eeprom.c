#include "eeprom.h"
#include "pic.h"

// null function to provide some rest time
void i2c_dly(void)
{
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
char x, d=0;
  SDA = 1; 
  for(x=0; x<8; x++) {
    d <<= 1;
    do {
      SCL = 1;
    }
    while(SCL_IN==0);    // wait for any SCL clock stretching
    i2c_dly();
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

bit i2c_write(unsigned char d)
{
char x;
static bit b;
  for(x=8; x; x--) {
    if(d&0x80) SDA = 1;
    else SDA = 0;
    SCL = 1;
    d <<= 1;
    SCL = 0;
  }
  SDA = 1;
  SCL = 1;
  i2c_dly();
  b = SDA_IN;          // possible ACK bit
  SCL = 0;
  return b;
}

void write_24Cxx(int address, char data) 
{
      char status;
      i2c_start();
      i2c_write(I2C_eeprom);
      #if (Chip_24Cxx > 2416)
        i2c_write(address>>8); // send MSB of 16 bits address
      #endif
      i2c_write(address);
      i2c_write(data);
      i2c_stop(); // start write cycle
      i2c_start();
      status=1; // wait for chip to finish write cycle
      while(status==1) { // status will return 0 when chip returns ack
        i2c_start();
        status=i2c_write(I2C_eeprom);
      }
      i2c_stop();
}

char read_24Cxx(int address) 
{
      char data;
      i2c_start();
      i2c_write(I2C_eeprom);
      #if (Chip_24Cxx > 2416)
        i2c_write(address>>8); // send MSB of 16 bits address
      #endif
      i2c_write(address);
      i2c_start();
      i2c_write(I2C_eeprom+1);
      data=i2c_rx(0);
      i2c_stop();
      return(data);
}
