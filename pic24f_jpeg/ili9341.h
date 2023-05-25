/* 
Interface with 2.2" TFT LCD 320x240 using ILI9341 controller with SD card socket.

LCD Pinout:

1. SDO (MISO) - RB6
2. LED
3. SCK - RB7
4. SDI (MOSI) - RB8
5. D/C - RA1
6. RESET - 5V
7. CS - RA2
8. GND
9. VCC
|
|
J2

SD Pinout:

1. SD_CS - RA0
2. SD_MOSI -  RP12
3. SD_MISO - RP13
4. SD_SCK - RP14
|
|
J4
*/

#ifndef TFTv2_h
#define TFTv2_h

#define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))   

// connections
#define TFT_DC LATAbits.LATA1
#define TFT_CS LATAbits.LATA2

// toggle
#define TFT_DC_LOW (TFT_DC = 0)
#define TFT_DC_HIGH (TFT_DC = 1)
#define TFT_CS_LOW (TFT_CS = 0)
#define TFT_CS_HIGH (TFT_CS = 1)

//Basic Colors
#define RED		0xf800
#define GREEN	0x07e0
#define BLUE	0x001f
#define BLACK	0x0000
#define YELLOW	0xffe0
#define WHITE	0xffff

//Other Colors
#define CYAN		0x07ff	
#define BRIGHT_RED	0xf810	
#define GRAY1		0x8410  
#define GRAY2		0x4208  

//TFT resolution 240*320
#define MIN_X	0
#define MIN_Y	0
#define MAX_X	239
#define MAX_Y	319

#define INT8U unsigned char
#define INT16U unsigned int

INT16U constrain(INT16U x, INT16U a, INT16U b);
void TFT_sendCMD(INT8U index);
void TFT_WRITE_DATA(INT8U data);
void TFT_sendData(INT16U data);
void TFT_WRITE_Package(INT16U *data, INT8U howmany);
INT8U TFT_Read_Register(INT8U Addr, INT8U xParameter);
INT8U TFT_readID(void);
void TFT_setCol(INT16U StartCol,INT16U EndCol);
void TFT_setPage(INT16U StartPage,INT16U EndPage);
void TFT_fillScreen(INT16U XL, INT16U XR, INT16U YU, INT16U YD, INT16U color);
void TFT_fillRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color);
void TFT_fillScreen2(void);
unsigned char TFT_init (void);
void TFT_setXY(INT16U poX, INT16U poY);
void TFT_setPixel(INT16U poX, INT16U poY,INT16U color);
void TFT_drawChar( INT8U ascii, INT16U poX, INT16U poY,INT16U size, INT16U fgcolor);
void TFT_drawString(char *string,INT16U poX, INT16U poY, INT16U size,INT16U fgcolor);
void  TFT_drawHorizontalLine( INT16U poX, INT16U poY, INT16U length,INT16U color);
void TFT_drawLine( INT16U x0,INT16U y0,INT16U x1, INT16U y1,INT16U color);
void TFT_drawVerticalLine( INT16U poX, INT16U poY, INT16U length,INT16U color);
void TFT_drawRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width,INT16U color);
void TFT_drawCircle(int poX, int poY, int r,INT16U color);
void TFT_fillCircle(int poX, int poY, int r,INT16U color);
void TFT_drawTraingle( int poX1, int poY1, int poX2, int poY2, int poX3, int poY3, INT16U color);
INT8U TFT_drawNumber(long long_num,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
INT8U TFT_drawFloat(float floatNumber,INT8U decimal,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
INT8U TFT_drawFloat2(float floatNumber,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);

#endif
