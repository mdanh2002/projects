#ifndef _H_XPT2046
#define _H_XPT2046

// connections
#define TP_CS LATAbits.LATA1
#define TP_IRQ PORTBbits.RB7

// calibration constant (coordinates of sample top/left/right/bottom touch points)
#define TOUCH_X0 255 
#define TOUCH_Y0 200 
#define TOUCH_X1 3968
#define TOUCH_Y1 3775


// function prototypes
unsigned int touch_Init(void);
unsigned char touch_GetCoordinates(unsigned int * pX, unsigned int * pY);

#endif
