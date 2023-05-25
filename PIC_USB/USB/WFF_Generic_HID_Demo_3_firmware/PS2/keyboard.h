 #ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/*
PS2 Keyboard interface
Ported from AVR microcontroller by MD

http://avrprogrammers.com/example_avr_keyboard.php
*/

// data = white = PORTD3
// gnd = black
// vcc = red
// clock = brown = INT0 (PORTD2)

extern unsigned char flags;

void keyboardInit( void );
unsigned char getKey( void );
void processKeyboardISR(void);
unsigned int sendKeyboardCommand(unsigned char command, unsigned char waitForResponse);

// keyboard connection
#define KB_DATA_DIR  TRISAbits.TRISA3
#define KB_DATA_IN   PORTAbits.RA3
#define KB_DATA_OUT  LATAbits.LATA3   
#define KB_CLK_DIR   TRISBbits.TRISB0 //clock is using INT0 interrupt
#define KB_CLK_OUT   LATBbits.LATB0
#define KB_CLK_IN    PORTBbits.RB0

#endif
