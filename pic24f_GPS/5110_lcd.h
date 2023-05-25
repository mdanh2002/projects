// LCD Pin connections
#define SCE     LATAbits.LATA2 // Chip Enabled Pin
#define RST     LATAbits.LATA3 // Reset Pin
#define D_C     LATAbits.LATA1 // Data or Command
#define MOSI    LATBbits.LATB14 // Data In
#define SCLK    LATBbits.LATB15 // SPI Clock

void lcd_5110_send(unsigned char cmd, unsigned char type);
void lcd_5110_cls(void);
void lcd_5110_black(void);
void lcd_5110_init(void);
void lcd_string(const char *str,unsigned char row);

