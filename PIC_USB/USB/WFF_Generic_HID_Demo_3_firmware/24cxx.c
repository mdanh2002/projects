#include "24cxx.h"
#include "i2c.h"

void write_24Cxx(unsigned int address, char data) 
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

unsigned char read_24Cxx(unsigned int address) 
{
      unsigned char data;
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
