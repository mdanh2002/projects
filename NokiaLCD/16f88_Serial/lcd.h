unsigned char Ver='A';  

// LCD Pin connections
#define sclk RB3
#define sdata RB4
#define rest RB6
#define cs RB7

void delayms(unsigned  int ii);
void send(unsigned char value, unsigned char cd);
//void readdata();
void LCD_Initialize();
void LCD_ColorSet(unsigned char Color);
void addset(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2);
void LCD_Clear(unsigned int value,unsigned char Color);
