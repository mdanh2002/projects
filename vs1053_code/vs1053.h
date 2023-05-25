/*
Interface with VS1053B Breakout Module purchased from eBay.

Sample code taken from http://www.lighthouse.org/accessibility/design/accessible-print-design/effective-color-contrast/

	1  2  3  4  5
	*************
	*************
	10 9  8  7  6

	1.  5V
	2.  DGND
	3.  MOSI - RB2
	4.  DREQ - RB3 
	5.  XCS - RB4
	6.  XDCS - RB15 
	7.  XRST - RB14
	8.  SCK - RB13
	9.  MISO - RB12
	10. 5V

*/

#define MP3_XCS 	LATAbits.LATA3 		//Control Chip Select Pin (for accessing SPI Control/Status registers)
#define MP3_XDCS 	LATBbits.LATB15 	//Data Chip Select / BSYNC Pin
#define MP3_DREQ_IN	PORTBbits.RB3      	//Data Request Pin: Player asks for more data
#define MP3_RESET	LATBbits.LATB14     //Reset Pin

//VS10xx SCI Registers
#define SCI_MODE 0x00
#define SCI_STATUS 0x01
#define SCI_BASS 0x02

#define SCI_CLOCKF 0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA 0x05
#define SCI_WRAM 0x06
#define SCI_WRAMADDR 0x07
#define SCI_HDAT0 0x08
#define SCI_HDAT1 0x09
#define SCI_AIADDR 0x0A
#define SCI_VOL 0x0B
#define SCI_AICTRL0 0x0C
#define SCI_AICTRL1 0x0D
#define SCI_AICTRL2 0x0E
#define SCI_AICTRL3 0x0F

typedef struct _VS1053_INFO
{
	unsigned char isPresent;

	unsigned int MP3Mode;
	unsigned int MP3Status;
	unsigned int MP3Clock;
	unsigned int MP3Bass;
	unsigned int MP3DecodeTime;
	unsigned int MP3AuData;
	unsigned int MP3HDat0;
	unsigned int MP3HDat1;

	unsigned int vsVersion;
	unsigned char volLeft;
	unsigned char volRight;
} VS1053_INFO;

void vs1053_WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte);
void vs1053_WriteRegisterW(unsigned char addressbyte, unsigned int data);
unsigned int vs1053_ReadRegister (unsigned char addressbyte);
unsigned char vs1053_SetVolume(unsigned char leftchannel, unsigned char rightchannel);

VS1053_INFO vs1053_setup();

/*
Load the hard-coded Hello MP3 file
*/
void vs1053_sayHello();
