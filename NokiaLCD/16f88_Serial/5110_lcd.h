// LCD Pin connections
#define SCE     RB7
#define RST     RB6
#define D_C     RA3
#define MOSI    RB4
#define SCLK    RB3

void delay_ms(unsigned  int ii);
void lcd_5110_string(const char *str,unsigned char row);
void lcd_5110_char(unsigned char letter);
void lcd_5110_send(unsigned char cmd, unsigned char type);
void lcd_5110_cls(void);
void lcd_5110_init(void);
void lcd_5110_black(void);
void lcd_5110_bitmap();