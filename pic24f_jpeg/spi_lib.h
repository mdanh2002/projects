/*
Engscope.com
JL
Created		Jul 1, 2010
Modified	Jul 1, 2010
headers for spi device drivers

http://www.engscope.com/pic24-tutorial/12-2-spi-master-usage/
*/

//define i2c ports so that it can be 
//called using constants instead of numbers
//declare only if ports are available
enum SPIPorts{
	SPIPORT1 = 1
	, SPIPORT2
};

//with selectable ports

unsigned char spiWrite( unsigned port, unsigned char i);

unsigned char spi1Write( unsigned char i );
void spi1Init(unsigned int prescale);

//spi port 2
unsigned char spi2Write( unsigned char i );
void spi2Init(unsigned int prescale);
