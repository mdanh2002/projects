/* 
Interface with the HY28A 320X240 LCD module in SPI mode with Touch Panel
LCD Controller: ILI9320
Touch Controller: ADS7843/XPT2056

Module Pinout:

http://www.hotmcu.com/28-touch-screen-tft-lcd-with-spi-interface-p-42.html

^
|
|
1	5V_IN	
2	GND
3	LCDB_SCK - RB8
4	LCDB_CS - RA2
5	LCDB_SDO - RB9
6	LCDB_SDI - RB6
7	NC
8	NC
9	NC
10	NC
11	NC
12	NC
13	NC
14	NC
15	NC
16	NC
17	NC
18	NC
19	GND
20	3V3_IN	

^
|
|
21	3V3_OUT
22	GND
23	NC
24	NC
25	NC
26	NC
27	NC
28	NC
29	NC
30	NC
31	NC
32	TP_CS - RA1 (GND?)
33	TP_SCK - RB12
34	TP_SDI - RB11
35	TP_SDO - RB10
36	TP_IRQ - RB7 (INT0)
37	NC
38	NC
39	GND
40	BL_CTRL

Note:

1. When powered from 5V supply, Pin 20 & Pin 21 ( 3V3_IN & 3V3_OUT ) provide 3.3V output.
2. When powered from 3.3V supply, 3V3_IN = 3V3OUT, 5V_IN keep NC.
3. TP_IRQ is Low level while the touch screen detects pressing
4. BL_CTRL Control the backlight brightness via PWM

The module is using 2mm pitch for its connection header (not 2.54mm). 
An improvised connector is required to plug it into a breadboard for testing.
*/

#define LCDB_CS LATAbits.LATA2

#ifndef __GLCD_H 
#define __GLCD_H

/* Private define ------------------------------------------------------------*/
#define MAX_X  240UL
#define MAX_Y  320UL  

#define SPI_START   (0x70)              /* Start byte for SPI transfer        */
#define SPI_RD      (0x01)              /* WR bit 1 within start              */
#define SPI_WR      (0x00)              /* WR bit 0 within start              */
#define SPI_DATA    (0x02)              /* RS bit 1 within start byte         */
#define SPI_INDEX   (0x00)              /* RS bit 0 within start byte         */
 
#define SPI_CS_LOW    (LCDB_CS = 0)
#define SPI_CS_HIGH   (LCDB_CS = 1)

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define uint16_t unsigned int
#define uint8_t unsigned char

#define RGB565CONVERT(red, green, blue)\
(uint16_t)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))

unsigned char LCD_Init(void);
void LCD_Clear(uint16_t Color);
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t color);
void LCD_drawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void LCD_PutChar(uint8_t ascii, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor);
void LCD_drawString(uint16_t Xpos, uint16_t Ypos, uint16_t size, char *str, uint16_t Color);
void LCD_fillRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color);
void LCD_drawRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color);
void LCD_drawCircle(int poX, int poY, int r, uint16_t color);
void LCD_fillCircle(int poX, int poY, int r, uint16_t color);

#endif
