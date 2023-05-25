/*
Engscope.com
JL
Created		Jul 1, 2010
Modified	Jul 1, 2010
headers for spi device drivers

http://www.engscope.com/pic24-tutorial/12-2-spi-master-usage/
*/

void spi1Init(unsigned int prescale, unsigned char ckp, unsigned char mode16);
unsigned char spi1Write( unsigned char i);
unsigned int spi1Write16(unsigned int i);

void spi2Init(unsigned int prescale, unsigned char ckp, unsigned char mode16, unsigned char cke, unsigned char smp);
unsigned char spi2Write( unsigned char i);
unsigned int spi2Write16(unsigned int i, unsigned char waitUntilDone);
