// 128x64 ST7565R LCD Interface Code
// https://github.com/rgwan/st7565
// https://github.com/andygock/glcd/blob/master/controllers/ST7565R.c
// https://github.com/vlad-gheorghe/clock-with-ST7920

#ifndef ST7565R_H_INCLUDED
#define ST7565R_H_INCLUDED

// LCD Pinout. RS pin is shared with UART Debug output. 
// SCK and SDO are shared with PICKit2, which is OK because these pins are high-impedance input only.
#define ST7565_RS		LATBbits.LATB10         // 1 = Data, 0 = Command
#define ST7565_SCLK     LATBbits.LATB3          // SPI Clock
#define ST7565_SDAT     LATBbits.LATB2          // SPI Data

#define ST7565_LCD_WIDTH            128
#define ST7565_LCD_HEIGHT           64 
#define ST7565_LCD_HT8              (ST7565_LCD_HEIGHT / 8)

#define ST7565R_DISPLAY_CONTRAST_MAX 40
#define ST7565R_DISPLAY_CONTRAST_MIN 10

#define ST7565R_FONT_SMALL               0
#define ST7565R_FONT_MEDIUM              1
#define ST7565R_FONT_LARGE               2
#define ST7565R_FONT_EXTRA_LARGE         3

#define ST7565R_CMD_DISPLAY_ON                     0xAF
#define ST7565R_CMD_DISPLAY_OFF                    0xAE
#define ST7565R_CMD_ADC_NORMAL                     0xA0
#define ST7565R_CMD_ADC_REVERSE                    0xA1
#define ST7565R_CMD_PAGE_ADDRESS_SET		       0xB0
#define ST7565R_CMD_START_LINE_SET		           0x40
#define ST7565R_CMD_COLUMN_ADDRESS_SET_MSB		   0x10					
#define ST7565R_CMD_COLUMN_ADDRESS_SET_LSB         0x00
#define ST7565R_CMD_DISPLAY_NORMAL                 0xA6
#define ST7565R_CMD_DISPLAY_REVERSE                0xA7
#define ST7565R_CMD_DISPLAY_ALL_POINTS_OFF         0xA4
#define ST7565R_CMD_DISPLAY_ALL_POINTS_ON          0xA5
#define ST7565R_CMD_LCD_BIAS_1_DIV_5_DUTY33        0xA1
#define ST7565R_CMD_LCD_BIAS_1_DIV_6_DUTY33        0xA2
#define ST7565R_CMD_NORMAL_SCAN_DIRECTION          0xC0
#define ST7565R_CMD_REVERSE_SCAN_DIRECTION         0xC8
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_0       0x20
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_1       0x21
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_2       0x22
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_3       0x23
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_4       0x24
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5       0x25
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_6       0x26
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_7       0x27
#define ST7565R_CMD_POWER_CTRL_ALL_ON              0x2F
#define ST7565R_CMD_SLEEP_MODE                     0xAC
#define ST7565R_CMD_NORMAL_MODE                    0xAD
#define ST7565R_CMD_RESET                          0xE2
#define ST7565R_CMD_NOP                            0xE3
#define ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET     0x81
#define ST7565R_CMD_ELECTRONIC_VOLUME(volume)      (0x3F & (~volume))
#define ST7565R_CMD_BOOSTER_RATIO_SET              0xF8
#define ST7565R_CMD_BOOSTER_RATIO_2X_3X_4X         0x00
#define ST7565R_CMD_BOOSTER_RATIO_5X               0x01
#define ST7565R_CMD_BOOSTER_RATIO_6X               0x03
#define ST7565R_CMD_STATUS_READ                    0x00
#define ST7565R_CMD_END                            0xEE
#define ST7565R_CMD_READ_MODIFY_WRITE              0xE0

void st7565r_write_byte(unsigned char byte, unsigned char rs);
void st7565r_write_data(unsigned char data);
void st7565r_write_command(unsigned char cmd);
void st7565r_soft_reset();
void st7565r_sleep_enable();
void st7565r_sleep_disable();
void st7565r_set_page_address(unsigned char address);
void st7565r_set_column_address(unsigned char address);
void st7565r_set_display_start_line_address(unsigned char address);
void st7565r_display_on();
void st7565r_display_off();
void st7565r_set_all_pixels(unsigned char pixels_on);
void st7565r_set_contrast(unsigned char contrast);
void st7565r_display_invert_enable();
void st7565r_display_invert_disable();
void st7565r_init();
void st7565r_clear_display();
void st7565r_updateRow(unsigned char y);
void st7565r_updateArea(unsigned char row, unsigned char colStart, unsigned char colEnd);
void st7565r_updateDisplay();
void st7565r_setPixel(unsigned char x, unsigned char y, unsigned char set);
void st7565r_drawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char set);
void st7565r_fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned char set);
void st7565r_drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned char set);
void st7565r_fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char set);
void st7565r_drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char set);
void st7565r_drawLineV(unsigned char x, unsigned char y0, unsigned char y1, unsigned char set);
void st7565r_drawLineH(unsigned char x0, unsigned char x1, unsigned char y, unsigned char set);
void st7565r_drawString(unsigned char x, unsigned char y, char c[], unsigned char size, unsigned char font);
void st7565r_drawString2(unsigned char x, unsigned char y, char c[], unsigned char size, unsigned char startPos, unsigned char len, unsigned char font);
void st7565r_drawExtraLargeChar(unsigned char x, unsigned char y, char c, unsigned char size);
void st7565r_drawLargeChar(unsigned char x, unsigned char y, char c, unsigned char size);
void st7565r_drawMediumChar(unsigned char x, unsigned char y, char c, unsigned char size);
void st7565r_drawSmallChar(unsigned char x, unsigned char y, char c, unsigned char size);
void st7565r_drawBitmap(__prog__ const unsigned char bmp[], unsigned char x, unsigned char y);
void st7565r_drawBitmapRaw(__prog__ const unsigned char bmp[], int x, unsigned char y, unsigned char w, unsigned char h);
void st7565r_setAutoUpdate(unsigned char autoUpdate);
void st7565r_updateDisplay();
void st7565r_autoUpdateDisplay();

#endif