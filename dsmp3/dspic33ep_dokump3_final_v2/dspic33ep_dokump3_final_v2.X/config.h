#include <libpic30.h>

#include "uart.h"
#include "st7565.h"

// default FAT_FS sector size. Check_MAX_SS and _MIN_SS in ff.h if sector size is 1024 bytes.
#define SECTOR_SIZE        512

// Highest possible clock speed, for MP3 playback
// also check mmc_pic.h for SD card speed
// The CPU needs 1 (PIC32), 2 (dsPIC33/PIC24F) or 4 (8-bit PIC) clocks per instruction cycle, but the peripherals only need 1 clock.
#define FINT                7372800ULL							// internal oscillator frequency, Hz
#define MPLLFBD_HIGH	  	(98)                                // M = PLLFBD + 2. Overclocked to ~184MHz (98). Official max. 74 (140MHz, 70MHz). Reliable overclock @ 180MHz
#define	MPLLPOST            (0)									// N2 = 2
#define	MPLLPRE             (0)									// N1 = 2
#define FOSC_HIGH           (FINT * (MPLLFBD_HIGH + 2) / 2 / 2) // FINT * M / (N1 * N2)

// Low Clock Speed, for radio management (except when recording), and for drawing clock occasionally in standby
#define MPLLFBD_LOW         (7)
#define FOSC_LOW            (FINT * (MPLLFBD_LOW + 2) / 2 / 2)		// FINT * M / (N1 * N2)

// remote control decoder constants
// refer to initTmr3() function for timer constants
#define REMOTE_TIMEOUT_MS               (1000)           // decoding will end if no activity from TFMS after this duration in ms
#define REMOTE_DEBOUNCE_MS              (1200)           // minimum wait between remote keypresses in ms
#define REMOTE_10MS_DIVIDER_FAST_CLK    (36)             // In Highest Speed mode, Timer 3 (16-bit) runs at 92MHz / 256 = 360kHz. Divide by this number to get 10ms.
#define REMOTE_10MS_DIVIDER_SLOW_CLK    (13)             // In Normal Mode, Timer 3 (16-bit) runs at 8MHz / 64 = 125kHz. Divide by this number to get 10ms.

// delay functions
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

// debug print statements, Declare char strResBuf[32] to use.
#define debugPrint(X, ...) sprintf(strResBuf, X, ##__VA_ARGS__); SendUARTStr(strResBuf)

// max & min macros
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// disable UART output in release mode
#define DEBUG_UART

// UART Speed
#define UART1_SPEED     57600

// UART Methods
void SendUARTStr(char str[]);

#ifndef DEBUG_UART
// empty method if UART DEBUG is not enabled
// if enabled, these methods are in main.c
#define SetUARTPin(X)
#define EnableUART(X)
#define SendUARTStr(X)  
#endif

// bitmap in font.h
#define DEGREE_SYMBOL_ASCII_CODE    127

// Low voltage threshold
#define LOW_BATT_VOLTAGE        (4.5)
#define INVALID_BATT_VOLTAGE    (3.0)

// Version String
#define APP_VERSION_STRING      "1.1.2"

#define BATT_ICON_POS_X         (112)
#define LCD_MAX_BOOT_LINES      (8)
#define INPUT_SRC_POS_X         (93)
#define TOP_MIDDLE_INFO_POS_X   (45)
#define TOP_MIDDLE_INFO_POS2_X  (55)
#define ALARM_ICON_POS_X        (80)
#define RIGHT_STATUS_POS_X      (92)

#define LCD_MAX_TEXT_LINES      (8)

#define STATUS_LINE_Y       56
#define MENU_LINE_Y         46

#define LCD_SCR_WIDTH       ST7565_LCD_WIDTH
#define LCD_SCR_HEIGHT      ST7565_LCD_HEIGHT

#define LCD_setAutoUpdate                                                           st7565r_setAutoUpdate
#define LCD_updateDisplay                                                           st7565r_updateDisplay
#define LCD_cls()                                                                   st7565r_clear_display(); 

#define LCD_drawLine(x0, y0, x1, y1)                                                st7565r_drawLine(x0, y0, x1, y1, 1);

#define LCD_drawRect(x, y, w, h)                                                    st7565r_drawRect(x, y, w, h, 1);
#define LCD_fillRect(x, y, w, h)                                                    st7565r_fillRect(x, y, w, h, 1);
#define LCD_clearRect(x, y, w, h)                                                   st7565r_fillRect(x, y, w, h, 0);
#define LCD_fillCircle(x, y, r)                                                    st7565r_fillCircle(x, y, r, 1);

#define LCD_drawSmallString(x, y, c, size)                                          st7565r_drawString(x, y, c,  size,  ST7565R_FONT_SMALL); st7565r_autoUpdateDisplay()            
#define LCD_drawSmallString2(x, y, c, size, startPos, len)                          st7565r_drawString2(x, y, c,  size,  startPos, len,  ST7565R_FONT_SMALL); st7565r_autoUpdateDisplay()            

#define LCD_drawString(x, y, c, size)                                               st7565r_drawString(x, y, c,  size,  ST7565R_FONT_MEDIUM); st7565r_autoUpdateDisplay()            
#define LCD_drawString2(x, y, c, size, startPos, len)                            st7565r_drawString2(x, y, c, size,  startPos, len,  ST7565R_FONT_MEDIUM); st7565r_autoUpdateDisplay()          
#define LCD_drawLargeString(x, y, c, size)                                          st7565r_drawString(x, y, c,  size,  ST7565R_FONT_LARGE);  st7565r_autoUpdateDisplay()
#define LCD_drawLargeString2(x, y, c, size, startPos, len)                       st7565r_drawString2(x, y, c, size,  startPos, len,  ST7565R_FONT_LARGE); st7565r_autoUpdateDisplay()          
#define LCD_drawExtraLargeString(x, y, c, size)                                     st7565r_drawString(x, y, c,  size,  ST7565R_FONT_EXTRA_LARGE); st7565r_autoUpdateDisplay()            
#define LCD_drawExtraLargeString2(x, y, c, size, startPos, len)                  st7565r_drawString2(x, y, c, size,  startPos, len,  ST7565R_FONT_EXTRA_LARGE); st7565r_autoUpdateDisplay()          

#define LCD_showBootInfo(line, col, text)                                                LCD_drawSmallString( (col > 0 ? 64 : 0), (line * 6), text, 1)
#define LCD_showBootInfo2(line, col, text)                                                LCD_drawSmallString( (col > 0 ? 64 : 0), (line * 6) + 4, text, 1)

#define LCD_drawTopLeftInfo(c)                                                         LCD_drawString(0, 1, c, 1)
#define LCD_drawTopLeftInfo2(c)                                                        LCD_drawString(20, 1, c, 1)

// update only the portion needed, 30 cols, 6 chars, e.g. 00:00 for click display
#define LCD_drawTopMiddleInfo(c)                                                       LCD_drawString(TOP_MIDDLE_INFO_POS_X, 1, c, 1)
#define LCD_updateTopMiddleInfo()                                                      st7565r_updateArea(0, TOP_MIDDLE_INFO_POS_X, TOP_MIDDLE_INFO_POS_X + 30)

#define LCD_drawTopMiddleInfo2(c)                                                       LCD_drawString(TOP_MIDDLE_INFO_POS2_X, 1, c, 1)

#define LCD_drawItemTitle(c)                                                        LCD_drawLargeString(2, 10, c, 1)
#define LCD_drawItemTitle2(c, startPos, len)                                     LCD_drawLargeString2(2, 10, c, 1, startPos, len)
#define LCD_updateItemTitle()                                                        st7565r_updateRow(1); st7565r_updateRow(2)

// update only info text line to save processing power. Bit shift of 3. Info text lines in the middle of 2 rows
#define LCD_drawInfoText(c)                                                         LCD_drawString(0, 30, c, 1)
#define LCD_drawInfoText2(c, startPos, len)                                         LCD_drawString2(0, 30, c, 1, startPos, len)
#define LCD_drawSmallInfoText(c)                                                    LCD_drawSmallString(0, 30, c, 1)
#define LCD_drawSmallInfoText2(c, startPos, len)                                               LCD_drawSmallString2(0, 30, c, 1, startPos, len)
#define LCD_updateInfoText()                                                        st7565r_updateRow(3); st7565r_updateRow(4)

#define LCD_drawMenuText(c)                                                         LCD_drawString(0, MENU_LINE_Y, c, 1)
#define LCD_drawRecDuration(c)                                                   LCD_drawString(98, MENU_LINE_Y, c, 1)
#define LCD_drawRecDuration2(c)                                                   LCD_drawString(80, MENU_LINE_Y, c, 1)
#define LCD_drawMenuText2(c, startPos, len)                                      LCD_drawString2(0, MENU_LINE_Y, c, 1, startPos, len)
#define LCD_updateMenuText()                                                      st7565r_updateRow(5); st7565r_updateRow(6)

#define LCD_drawTrackInfo(c)                                                       LCD_drawString(18, STATUS_LINE_Y, c, 1)
#define LCD_drawFolderInfo(c)                                                      LCD_drawString(54, STATUS_LINE_Y, c, 1)

// update only status text line to save power. 
#define LCD_drawStatusText(c)                                                       LCD_drawString(0, STATUS_LINE_Y, c, 1)
#define LCD_drawStatusText2(c, startPos, len)                                       LCD_drawString2(0, STATUS_LINE_Y, c, 1, startPos, len)
#define LCD_updateStatusText()                                                      st7565r_updateRow(7)

#define LCD_drawRightStatusText(c)                                                  LCD_drawString(RIGHT_STATUS_POS_X, STATUS_LINE_Y, c, 1)
#define LCD_drawHelpText(line, c)                                                   LCD_drawString(2, line  * 8, c, 1)
#define LCD_updateRightStatusText()                                                 st7565r_updateArea(7, RIGHT_STATUS_POS_X, ST7565_LCD_WIDTH)

#define LCD_drawBitmap                                                              st7565r_drawBitmap

void setFatFsIsUsb(unsigned char isUSB);
unsigned char is_sd_init_ok();
unsigned char getCurOperatingMode();