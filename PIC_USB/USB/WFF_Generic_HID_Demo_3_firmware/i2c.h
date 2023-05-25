// I2C connections
#define SDA_IN 	PORTBbits.RB3
#define SDA  	TRISBbits.TRISB3
#define SCL_IN 	PORTAbits.RA2
#define SCL 	TRISAbits.TRISA2

// I2C functions
void i2c_dly(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_rx(char ack);
char i2c_write(unsigned char d);
