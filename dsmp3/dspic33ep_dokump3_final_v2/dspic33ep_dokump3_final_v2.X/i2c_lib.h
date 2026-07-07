// I2C connections
#define SCL_IN 	PORTAbits.RA3
#define SCL  	TRISAbits.TRISA3
#define SDA_IN 	PORTBbits.RB13
#define SDA 	TRISBbits.TRISB13

// Delay Management
#define I2C_WAIT_TIMEOUT    1000

// I2C functions
void i2c_dly(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_rx(char ack);
char i2c_write(unsigned char d);
