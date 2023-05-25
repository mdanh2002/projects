#include "ds1307.h"
#include "i2c.h"
#include <delays.h>

unsigned short bin2bcd(unsigned short binary_value)
{
  unsigned short temp;
  unsigned short retval;

  temp = binary_value;
  retval = 0;

  while(1)
  {
    // Get the tens digit by doing multiple subtraction
    // of 10 from the binary value.
    if(temp >= 10)
    {
      temp -= 10;
      retval += 0x10;
    }
    else // Get the ones digit by adding the remainder.
    {
      retval += temp;
      break;
    }
  }

  return(retval);
}

// Input range - 00 to 99.
unsigned short bcd2bin(unsigned short bcd_value)
{
  unsigned short temp;

  temp = bcd_value;
  // Shifting upper digit right by 1 is same as multiplying by 8.
  temp >>= 1;
  // Isolate the bits for the upper digit.
  temp &= 0x78;

  // Now return: (Tens * 8) + (Tens * 2) + Ones

  return(temp + (temp >> 2) + (bcd_value & 0x0f));
} 

void ds1307_startRTC(unsigned short seconds)
{
   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x00);      // REG 0
   i2c_write(bin2bcd(seconds));     // Start oscillator with current "seconds value
   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x07);      // Control Register
   i2c_write(0x80);     // Disable squarewave output pin
   i2c_stop();
}

char ds1307_init(void)
{
   unsigned short seconds = 0;	
   unsigned char reg0val = 0;

   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x00);      // REG 0
   i2c_start();
   i2c_write(0xD1);      // RD from RTC
	reg0val = i2c_rx(0);
   seconds = bcd2bin(reg0val); // Read current "seconds" in DS1307
   i2c_stop();
	
	// Bit 7 of Register 0 is the clock halt (CH) bit. When this bit is set to 1, the oscillator is disabled.
	// which means the clock was not running before (e.g. no backup battery or previously stopped)
	// we attempt to start it
   return !(reg0val & 0b10000000);
}

void ds1307_set_date_time(unsigned short day, unsigned short mth, unsigned short year, unsigned short dow, unsigned short hr, unsigned short min, unsigned short sec)
{
  sec &= 0x7F;
  hr &= 0x3F;

  i2c_start();
  i2c_write(0xD0);            // I2C write address
  i2c_write(0x00);            // Start at REG 0 - Seconds
  i2c_write(bin2bcd(sec));      // REG 0
  i2c_write(bin2bcd(min));      // REG 1
  i2c_write(bin2bcd(hr));      // REG 2
  i2c_write(bin2bcd(dow));      // REG 3
  i2c_write(bin2bcd(day));      // REG 4
  i2c_write(bin2bcd(mth));      // REG 5
  i2c_write(bin2bcd(year));      // REG 6
  i2c_write(0x80);            // REG 7 - Disable squarewave output pin
  i2c_stop();
}

void ds1307_get_date(unsigned short *day, unsigned short *mth, unsigned short *year, unsigned short *dow)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x03);            // Start at REG 3 - Day of week
  i2c_start();
  i2c_write(0xD1);
  *dow  = bcd2bin(i2c_rx(1) & 0x7f);   // REG 3
  *day  = bcd2bin(i2c_rx(1) & 0x3f);   // REG 4
  *mth  = bcd2bin(i2c_rx(1) & 0x1f);   // REG 5
  *year = bcd2bin(i2c_rx(0));            // REG 6
  i2c_stop();
}

void ds1307_get_time(unsigned short *hr, unsigned short *min, unsigned short *sec)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);            // Start at REG 0 - Seconds
  i2c_start();
  i2c_write(0xD1);
  *sec = bcd2bin(i2c_rx(1) & 0x7f);
  *min = bcd2bin(i2c_rx(1) & 0x7f);
  *hr  = bcd2bin(i2c_rx(0) & 0x3f);
  i2c_stop();
} 