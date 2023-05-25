// 24CXX EEPROM interface

// EEPROM type and address
#define Chip_24Cxx 2464
#define I2C_eeprom 0xA0 

void write_24Cxx(unsigned int address, char data);
unsigned char read_24Cxx(unsigned int address);
