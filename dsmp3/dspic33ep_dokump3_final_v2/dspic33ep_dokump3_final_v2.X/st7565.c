#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "st7565.h"
#include "font.h"

// Frame buffer, in EDS memory
__eds__ volatile unsigned char __attribute__((eds, section("lcdFrameBuffer"), space(ymemory))) LCD_FrameBuffer[ST7565_LCD_HT8][ST7565_LCD_WIDTH];

unsigned char isAutoUpdate = 0;

void st7565r_write_byte(unsigned char c, unsigned char rs) {
  char x;
  
  ST7565_RS = rs;
  
  for (x = 0; x < 8; x++) {
    ST7565_SCLK = 0;
    ST7565_SDAT = 0;
    Nop();              // to limit SPI speed to ~20MHz
       
    if (c & 0x80)
      ST7565_SDAT = 1;

    ST7565_SCLK = 1;
    c <<= 1;
    
    Nop();
  }
}

void st7565r_write_data(unsigned char data) {
    st7565r_write_byte(data, 1);
}

void st7565r_write_command(unsigned char cmd) {
    st7565r_write_byte(cmd, 0);
}

void st7565r_soft_reset()
{
	st7565r_write_command(ST7565R_CMD_RESET);
}

void st7565r_sleep_enable()
{
	st7565r_write_command(ST7565R_CMD_SLEEP_MODE);
}

void st7565r_sleep_disable()
{
	st7565r_write_command(ST7565R_CMD_NORMAL_MODE);
}

void st7565r_set_page_address(unsigned char address)
{
	address &= 0x0F; 
	st7565r_write_command(ST7565R_CMD_PAGE_ADDRESS_SET | address);
}
 
void st7565r_set_column_address(unsigned char address)
{
	address &= 0x7F;   
	st7565r_write_command(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB | (address >> 4));
	st7565r_write_command(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB | (address & 0x0F));
}

void st7565r_write_data_at(unsigned char x, unsigned char y, unsigned char data)
{
	st7565r_set_page_address(y);
	st7565r_set_column_address(x);
	st7565r_write_command(data);
}

void st7565r_set_display_start_line_address(unsigned char address)
{
	address &= 0x3F;
	st7565r_write_command(ST7565R_CMD_START_LINE_SET | address);
}

void st7565r_display_on()
{
	st7565r_write_command(ST7565R_CMD_DISPLAY_ON);
}

void st7565r_display_off()
{
	st7565r_write_command(ST7565R_CMD_DISPLAY_OFF);
}

void st7565r_set_all_pixels(unsigned char pixels_on)
{
	if (pixels_on) {
		st7565r_write_command(ST7565R_CMD_DISPLAY_ALL_POINTS_ON);
	} else {
		st7565r_write_command(ST7565R_CMD_DISPLAY_ALL_POINTS_OFF);
	}
}

void st7565r_set_contrast(unsigned char contrast)
{
	if (contrast < ST7565R_DISPLAY_CONTRAST_MIN) {
		contrast = ST7565R_DISPLAY_CONTRAST_MIN;
	}
	if (contrast > ST7565R_DISPLAY_CONTRAST_MAX) {
		contrast = ST7565R_DISPLAY_CONTRAST_MAX;
	}
	st7565r_write_command(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);
	st7565r_write_command(ST7565R_CMD_ELECTRONIC_VOLUME(contrast));
}

void st7565r_display_invert_enable()
{
	st7565r_write_command(ST7565R_CMD_DISPLAY_REVERSE);
}

void st7565r_display_invert_disable()
{
	st7565r_write_command(ST7565R_CMD_DISPLAY_NORMAL);
}

void resetFrameBuffer()
{
    unsigned char x, y;
	for (y=0; y < (ST7565_LCD_HT8); y++)
	{		   
		for (x=0; x < ST7565_LCD_WIDTH; x++) 
        {
            LCD_FrameBuffer[y][x] = 0;
        }
    }            
}

void st7565r_init()
{
    // CS is not needed, just connected to GND. 
	// Reset is also useful for a hard reset but not needed if the LCD is started from a clean state (no power). 
    // All that is neede is a capacitor (2.2uF) and a resistor (68k) in series: e.g. VCC -- R -- C -- GND.
    // Feed RESET right after R. This way, there will a short low pulse when power is applied as the capacitor is charged up
    // which emulates the RST behaviour and helps save the RST pin.
    // There is also a soft reset which is not the same as a hard reset. Don't use it!    
	// st7565r_hard_reset();
    
    // can't use memset due to EDS pointer.
    resetFrameBuffer();
    
	// Set the RS pin to the default state (command)
	ST7565_RS = 0;

	// The column address is set to increasing
	st7565r_write_command(ST7565R_CMD_ADC_NORMAL);

	// Non-inverted display
	st7565r_display_invert_disable();

	// The common mode scan direction is reversed COM31->COM0
	st7565r_write_command(ST7565R_CMD_REVERSE_SCAN_DIRECTION);

	// Set the voltage bias ratio to 1/6
	st7565r_write_command(ST7565R_CMD_LCD_BIAS_1_DIV_6_DUTY33);

	// Set booster circuit, voltage regulator and voltage follower all to on
	st7565r_write_command(ST7565R_CMD_POWER_CTRL_ALL_ON);

	// Set the booster ratio to 2X,3X,4X
	st7565r_write_command(ST7565R_CMD_BOOSTER_RATIO_SET);
	st7565r_write_command(ST7565R_CMD_BOOSTER_RATIO_2X_3X_4X);

	// Set voltage resistor ratio to 1
	st7565r_write_command(ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_4);
            
	/* Set contrast */
	st7565r_set_contrast(0x10);    
    
    st7565r_set_display_start_line_address(0);
    
	// Turn on the display
	st7565r_display_on();
}

void st7565r_setAutoUpdate(unsigned char autoUpdate)
{
    isAutoUpdate = autoUpdate;
}

void st7565r_updateArea(unsigned char row, unsigned char colStart, unsigned char colEnd)
{
    unsigned char x;
    st7565r_set_page_address(row);
    st7565r_set_column_address(colStart);

    for (x=colStart; x < colEnd; x++) 
        st7565r_write_data(LCD_FrameBuffer[row][x]);
}

void st7565r_updateRow(unsigned char y)
{
    st7565r_updateArea(y, 0, ST7565_LCD_WIDTH);
}

void st7565r_updateDisplay()
{
    unsigned char y;
	for (y=0; y < (ST7565_LCD_HEIGHT >> 3); y++)
	{
		st7565r_updateRow(y);
	}
}

void st7565r_autoUpdateDisplay()
{
    if (isAutoUpdate)
    {
        st7565r_updateDisplay();
    }
}

void st7565r_clear_display() {
   resetFrameBuffer();
   
   if (!isAutoUpdate)
   {
        st7565r_updateDisplay();
   }
}

void st7565r_setPixel(uint8_t x, uint8_t y, unsigned char set)
{
	if (set) 
        LCD_FrameBuffer[y >> 3][x] |= (1 << (y%8));
    else 
        LCD_FrameBuffer[y >> 3][x] &= ~(1 << (y%8));
}

void st7565r_drawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char set) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  while (1) {
    st7565r_setPixel(x0, y0, set);
    
    if (x0 == x1 && y0 == y1) 
        return;
    int err2 = err + err;
    if (err2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (err2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void st7565r_drawLineH(unsigned char x0, unsigned char x1, unsigned char y, unsigned char set) {
  unsigned char x;

  if (x1 > x0)
    for (x = x0; x <= x1; x++) st7565r_setPixel(x, y, set);
  else
    for (x = x1; x <= x0; x++) st7565r_setPixel(x, y, set);
}

void st7565r_drawLineV(unsigned char x, unsigned char y0, unsigned char y1, unsigned char set) {
  unsigned char y;

  if (y1 > y0)
    for (y = y0; y <= y1; y++) st7565r_setPixel(x, y, set);
  else
    for (y = y1; y <= y0; y++) st7565r_setPixel(x, y, set);
}

void st7565r_drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char set) {
  if (x >= ST7565_LCD_WIDTH || y >= ST7565_LCD_HEIGHT) 
      return;
  
  unsigned char drawVright = 1;
  if (x + w > ST7565_LCD_WIDTH) {
    w = ST7565_LCD_WIDTH - x;
    drawVright = 0;
  }
  
  if (y + h > ST7565_LCD_HEIGHT) 
      h = ST7565_LCD_HEIGHT - y;  
  else 
      st7565r_drawLineH(x, x + w - 1, y + h - 1, set);
  
  st7565r_drawLineH(x, x + w - 1, y, set);
  st7565r_drawLineV(x, y + 1, y + h - 2, set);
  
  if (drawVright) 
      st7565r_drawLineV(x + w - 1, y + 1, y + h - 2, set);
}

void st7565r_fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char set) {
  int i;

  if (x >= ST7565_LCD_WIDTH || y >= ST7565_LCD_HEIGHT) 
      return;
  
  if (x + w > ST7565_LCD_WIDTH) 
      w = ST7565_LCD_WIDTH - x;
  
  if (y + h > ST7565_LCD_HEIGHT) 
      h = ST7565_LCD_HEIGHT - y;
  
  for (i = y; i < y + h; i++) 
      st7565r_drawLineH(x, x + w - 1, i, set);
}

void st7565r_drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned char set) {
  int f = 1 - (int) r;
  int ddF_x = 1;
  int ddF_y = -2 * (int) r;
  int x = 0;
  int y = r;

  st7565r_setPixel(x0, y0 + r, 1);
  st7565r_setPixel(x0, y0 - r, 1);
  st7565r_setPixel(x0 + r, y0, 1);
  st7565r_setPixel(x0 - r, y0, 1);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    st7565r_setPixel(x0 + x, y0 + y, 1);
    st7565r_setPixel(x0 - x, y0 + y, 1);
    st7565r_setPixel(x0 + x, y0 - y, 1);
    st7565r_setPixel(x0 - x, y0 - y, 1);
    st7565r_setPixel(x0 + y, y0 + x, 1);
    st7565r_setPixel(x0 - y, y0 + x, 1);
    st7565r_setPixel(x0 + y, y0 - x, 1);
    st7565r_setPixel(x0 - y, y0 - x, 1);
  }
}

void st7565r_fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned char set) {
  st7565r_drawLineH(x0 - r, x0 - r + 2 * r + 1, y0, set);
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    st7565r_drawLineH(x0 - x, x0 - x + 2 * x + 1, y0 + y, set);
    st7565r_drawLineH(x0 - y, x0 - y + 2 * y + 1, y0 + x, set);
    st7565r_drawLineH(x0 - x, x0 - x + 2 * x + 1, y0 - y, set);
    st7565r_drawLineH(x0 - y, x0 - y + 2 * y + 1, y0 - x, set);
  }
}

void st7565r_drawSmallChar(unsigned char x, unsigned char y, char c, unsigned char size)
{
    unsigned char i, j, k, m, tColor;
    unsigned char letter = c - 32;
    unsigned char code1 = smallFont[letter*2];
    unsigned char code2 = smallFont[letter*2+1];
        
    for (j = 0; j < 5; j++)
    {
        for (i = 0; i < 3; i++)
        {
            k = (j * 3 + i);
            
            tColor = 0;
            if (k < 8)
            {
                m = 7 - k;
                tColor = (code1 & (1 << m)) >> m;
            }
            else {
                m = 14 - k;         // highest bit unused, total is 15 bits (not 16 bits)
                tColor = (code2 & (1 << m)) >> m;
            }
                                                
            if (size == 1) // default size
            {
                st7565r_setPixel(x+i, y+j, tColor);
            }
            else {  // big size
                st7565r_fillRect(x+i*size, y+j*size, size, size, tColor);
            }
        }
    }
}

void st7565r_drawMediumChar(unsigned char x, unsigned char y, char c, unsigned char size)
{
	unsigned char i, j;
    
	unsigned char letter = c - 0x20;
	for (i =0; i<5; i++ ) {
		unsigned char line = mediumFont[letter*5+i];

		for (j = 0; j<7; j++) {
            unsigned char tColor = ( (line & 0x1) ? 1 : 0 );
            
            if (size == 1) // default size
                st7565r_setPixel(x+i, y+j, tColor);
            else {  // big size
                st7565r_fillRect(x+i*size, y+j*size, size, size, tColor);
            } 

			line >>= 1;
		}
	}
}

void st7565r_drawLargeChar(unsigned char x, unsigned char y, char c, unsigned char size)
{
	unsigned char i, j, j0;

	unsigned char letter = c - 32;
	for (i =0; i<16; i++ ) {
		unsigned char line = largeFont[letter*16+i];

		for (j0 = 0; j0 < 8; j0++) {
            unsigned char tColor = ( (line & 0x1) ? 1 : 0 );
            
            j = 7 - j0;
            if (size == 1) // default size
                st7565r_setPixel(x+j, y+i, tColor);
            else {  // big size
                st7565r_fillRect(x+j*size, y+i*size, size, size, tColor);
            } 
            
			line >>= 1;
		}
	}
}

void st7565r_drawExtraLargeChar(unsigned char x, unsigned char y, char c, unsigned char size)
{
	unsigned char i, j, j0, line;

	unsigned char letter = c - 32;
	for (i = 0; i < 32; i++) {
		for (j0 = 0; j0 < 16; j0++) {            
            if (j0 == 0 || j0 == 8)
            {
                line = extraLargeFont[letter*64 + (i * 2) + (1 - j0 / 8)];            
            }
            
            unsigned char tColor = ( (line & 0x1) ? 1 : 0 );
            
            j = 16 - j0;
            if (size == 1) // default size
                st7565r_setPixel(x+j, y+i, tColor);
            else {  // big size
                st7565r_fillRect(x+j*size, y+i*size, size, size, tColor);
            } 
            
			line >>= 1;
		}
	}
}

void st7565r_drawString2(unsigned char x, unsigned char y, char c[], unsigned char size, unsigned char startPos, unsigned char len, unsigned char font)
{
    unsigned char pos = 0;
    unsigned char wd = 0;
    
    for (pos = startPos; pos < (len == 0 ? 255 : (startPos + len) ); pos++) {
        if (c[pos] == 0)
            break;
        
        if (font == ST7565R_FONT_SMALL)          // 3x5
        {
            st7565r_drawSmallChar(x, y, c[pos], size);
            wd = 4;
        }
        else if (font == ST7565R_FONT_MEDIUM)    // 5x7
        {
            st7565r_drawMediumChar(x, y, c[pos], size);
            wd = 6;
        }
        else if (font == ST7565R_FONT_LARGE)     // 8x16
        {
            st7565r_drawLargeChar(x, y, c[pos], size);
            wd = 9;
        }
        else if (font == ST7565R_FONT_EXTRA_LARGE)     // 16*32
        {
            st7565r_drawExtraLargeChar(x, y, c[pos], size);
            wd = 17;
        }        
		x += size*wd;		
	}
}

void st7565r_drawString(unsigned char x, unsigned char y, char c[], unsigned char size, unsigned char font)
{
	st7565r_drawString2(x, y, c, size, 0, 0, font);
}


void st7565r_drawBitmapRaw(__prog__ const unsigned char bmp[], int x, unsigned char y, unsigned char w, unsigned char h)
{
  unsigned char i, j8, d, b;
  
  for (j8=0; j8 < h / 8; j8++) {
    for(i=0; i<w; i++) {
      d = bmp[w*j8+i];
            
      int lastbit = h - j8 * 8;
      if (lastbit > 8) lastbit = 8;
      
      for(b=0; b<lastbit; b++) {
         st7565r_setPixel(x + i, y + j8*8 + b, d & 1);
          
         d >>= 1;
      }
    }
  }
}

void st7565r_drawBitmap(__prog__ const unsigned char bmp[], unsigned char x, unsigned char y)
{
  st7565r_drawBitmapRaw(bmp + 2, x, y, bmp[0], bmp[1]);
}
