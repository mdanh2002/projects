// LCD Pin connections
#define SCE     LATDbits.LATD2
#define RST     LATBbits.LATB4
#define D_C     LATDbits.LATD0
#define MOSI    LATDbits.LATD1
#define SCLK    LATDbits.LATD3

void lcd_5110_send(unsigned char cmd, unsigned char type);
void lcd_5110_cls(void);
void lcd_5110_black(void);
void lcd_5110_init(void);
void lcd_5110_char(unsigned char letter);
void lcd_string_rom(const rom char *str,unsigned char row);
void lcd_string_ram(ram char *str,unsigned char row);

