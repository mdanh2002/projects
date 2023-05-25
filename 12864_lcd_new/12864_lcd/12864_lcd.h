// To interface with 128x64 Graphics LCD
// Sample code taken from http://tutorial.cytron.com.my/2012/01/02/graphic-lcd-st7920-interfacing-to-skpic32/ 
// The controller is ST7920

// LCD Pin Connection in 4-bit mode
#define LCD_RS 		LATAbits.LATA3
#define	LCD_EN 		LATBbits.LATB11
#define	LCD_REST	LATAbits.LATA4
#define	LCD_DB4		LATBbits.LATB12
#define	LCD_DB5		LATBbits.LATB13
#define	LCD_DB6		LATBbits.LATB14
#define	LCD_DB7		LATBbits.LATB10

// LCD Functions
void LCD_Init(void);
void LCD_TextDisplayString(unsigned char line, char* string);
void LCD_GraphicsDisplayString(unsigned char line, char* str);
void LCD_EnableGraphics(void);
void LCD_DisableGraphics(void);
void LCD_FillScreenGraphics(const unsigned char* graphic);
void LCD_ClearGraphics(void);
void LCD_WriteData(unsigned char data);
void LCD_WriteCommand(unsigned char command);
