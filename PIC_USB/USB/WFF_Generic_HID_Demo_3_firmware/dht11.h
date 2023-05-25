// interface with DHT11 temperature and humidity sensor
// adapted from  http://arduino.cc/playground/Main/DHT11Lib

// DHT11 connection pinout
#define DHT11_IN 	PORTBbits.RB1
#define DHT11_DIR 	TRISBbits.TRISB1
#define DHT11_OUT 	LATBbits.LATB1

// read the DHT11 and return 1 if OK, 0 if fails
// if ok, return the temperature in C and humidity in %
unsigned char dht11_read(unsigned char* temperature, unsigned char* humidity);
