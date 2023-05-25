#include <p18F4550.h>
#include <string.h>

/*
#define RESET		0
#define BRIGHT 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7
*/

void uart_tx_data(unsigned char data1);
void uart_tx_string_ram(ram char* str, char withLineBreak);
void uart_tx_string_rom(const rom char* str, char withLineBreak);
unsigned char uart_rx_data(void);
void uart_init(void);
//void uart_setTerminalColor(int attr, int fg, int bg);
