// Gameboy camera connector
/*
Pin 	Function

1 		+5V Power Supply
2 		START 		Start Input
3 		SIN 		Data Input
4 		LOAD 		Data Set Input
5 		Xrst,RESET 	System+Memory Reset Input
6 		Xck 		System Clock Input
7 		READ 		Read Image Output
8 		Vout 		Analog Signal Output
9 		Ground
*/

#define	CAM_START     LATCbits.LATC1
#define	CAM_SIN       LATDbits.LATD7
#define	CAM_LOAD      LATDbits.LATD6
#define	CAM_RESET     LATDbits.LATD5
#define CAM_CLOCK     LATDbits.LATD4
#define	CAM_READ      PORTCbits.RC0