// SYN 6288 Chinese TTS chip interface
// Adapted from http://www.coocox.org/driver_repo/2c951a77-c48d-4cd9-a68c-92da951085db/lib/SYN6288.c.htm

// BUSY input for SYN6288
#define SYN_BUSY PORTBbits.RB8

// custom type defines
#define uint8_t unsigned char
#define uint16_t unsigned int 

// default constants
#define SYNBackGroundMusic0 	0x00
#define SYNBackGroundMusic1 	0x08
#define SYNBackGroundMusic2 	0x10
#define SYNBackGroundMusic3 	0x20
#define SYNBackGroundMusic4 	0x30
#define SYNBackGroundMusic5 	0x40
#define SYNBackGroundMusic6 	0x50
#define SYNBackGroundMusic7 	0x60
#define SYNBackGroundMusic8 	0x70
#define SYNBackGroundMusic9 	0x80
#define SYNBackGroundMusic10 	0x90
#define SYNBackGroundMusic11 	0xA0
#define SYNBackGroundMusic12 	0xB0
#define SYNBackGroundMusic13 	0xC0
#define SYNBackGroundMusic14 	0xD0
#define SYNBackGroundMusic15 	0xE0
#define SYNTextCodecGB2312 		0x00
#define SYNTextCodecGBK    		0x01
#define SYNTextCodecBIG5   		0x02
#define SYNTextCodecUNICODE 	0x03
#define SYNBaudRate9600    		0x00
#define SYNBaudRate19200   		0x01
#define SYNBaudRate38400   		0x02

// library functions
uint8_t CheckXOR(const uint8_t * Object, uint8_t Length);
void SYN_SetBackGroundMusic(uint8_t Code);
void SYN_SetTextCodec(uint8_t Code);
void SYN_SentHeader(uint16_t DataAreaLength);
void SYN_SetBaudRate(uint8_t BaudRate);
void SYN_Say(const uint8_t * Text); // maximum 20 chars
void SYN_SpeakingPaused(void);
void SYN_SpeakingResume(void);
void SYN_SpeakingStop(void);
