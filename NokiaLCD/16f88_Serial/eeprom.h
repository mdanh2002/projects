//*************
// I2C pin connections for 24C64 EEPROM

#define SCL     TRISA0 // I2C bus
#define SDA     TRISA1 //
#define SCL_IN  RA0    //
#define SDA_IN  RA1    //

// EEPROM type and address
#define Chip_24Cxx 2464
#define I2C_eeprom 0xA0 

void i2c_dly(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_rx(char ack);
bit i2c_write(unsigned char d);
void write_24Cxx(int address, char data);
char read_24Cxx(int address);