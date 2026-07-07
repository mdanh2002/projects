#include <xc.h> 
#include "config.h"
#include "i2c_lib.h"
#include "24lc256.h"

void eeprom_writeByte(unsigned int address, unsigned char data)
{
    i2c_start();
    i2c_write(EEPROM_ADDRESS_W);
    i2c_write((unsigned char)(address >> 8));
    i2c_write((unsigned char)(address & 0xFF));
    i2c_write(data);
    i2c_stop();    
    
    delay_ms(EEPROM_WRITE_CYCLE_TIME_MS);
}

// Return the value of the byte read
unsigned char eeprom_readByte (unsigned int address)
{
    unsigned char data = 0;
    
    i2c_start();
    i2c_write(EEPROM_ADDRESS_W);
    i2c_write((unsigned char)(address >> 8));
    i2c_write((unsigned char)(address & 0xFF));
    i2c_start();
    i2c_write(EEPROM_ADDRESS_R);    
    data = i2c_rx(0);
    i2c_stop();
    
    return data;
}

// each page is 64 bytes
void eeprom_writePage(unsigned int address, unsigned char size, unsigned char *buffer)
{
    unsigned char i = 0;
    
    if (size > EEPROM_PAGE_SIZE)
        return;
    
    i2c_start();
    i2c_write(EEPROM_ADDRESS_W);
    i2c_write((unsigned char)(address >> 8));
    i2c_write((unsigned char)(address & 0xFF));    
    for (i = 0; i < size; i++)
    {
        i2c_write(buffer[i]);
    }    
    i2c_stop();
    
    delay_ms(EEPROM_WRITE_CYCLE_TIME_MS);
}

void eeprom_readPage (unsigned int address, unsigned char size, unsigned char *buffer)
{
    unsigned char i = 0;
    
    if (size > EEPROM_PAGE_SIZE)
        return;
    
    i2c_start();
    i2c_write(EEPROM_ADDRESS_W);
    i2c_write((unsigned char)(address >> 8));
    i2c_write((unsigned char)(address & 0xFF));
    i2c_start();
    i2c_write(EEPROM_ADDRESS_R);    
    
    for (i = 0; i < size - 1; i++)
    {
        // read with ack except last byte
        buffer[i] = i2c_rx(1);
    }
    // last byte does not need ack
    buffer[size-1] = i2c_rx(0);
    
    i2c_stop();
}