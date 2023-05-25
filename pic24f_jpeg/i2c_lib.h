// I2C connections
#define SDA_IN 	PORTBbits.RB5
#define SDA  	TRISBbits.TRISB5
#define SCL_IN 	PORTBbits.RB6
#define SCL 	TRISBbits.TRISB6

// I2C functions
void i2c_dly(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_rx(char ack);
char i2c_write(unsigned char d);
