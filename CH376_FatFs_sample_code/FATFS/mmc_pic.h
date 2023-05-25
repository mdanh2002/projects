/*
SYNC_MODE is from SPIXCON1 register

Bit 5	  - 1 	= SPI Master Mode
Bit 4-2	  -	111 = Secondary prescale 1:1
			110 = Secondary prescale 2:1
			101 = Secondary prescale 3:1			
			100 = Secondary prescale 4:1
            011 = Secondary prescale 5:1
            010 = Secondary prescale 6:1
            001 = Secondary prescale 7:1
			000 = Secondary prescale 8:1
			
Bit 1-0   -	11 	= Primary prescale 1:1
			10 	= Primary prescale 4:1
			01 	= Primary prescale 16:1
			00 	= Primary prescale 64:1
			
F(SCK) = FCY / (Primary Prescaler * Secondary Prescaler)

Most SD cards can run up to 20MHz SPI in SYNC_MODE_FAST. SD mode supports faster speeds.
SYNC_MODE_SLOW should be around 400kHz.
 
SPI_PRESCALE_DIV is purely for information display on UI

Note: Do not set both prescales to 1:1
*/

#ifndef SPI_PRESCALE_DIV
#define   SPI_PRESCALE_DIV  7
#endif

#ifndef SYNC_MODE_FAST
#define   SYNC_MODE_FAST    0b100111
#endif

#ifndef SYNC_MODE_SLOW
#define   SYNC_MODE_SLOW    0b100000
#endif 


/* Socket controls  (Platform dependent) */
#define SD_CS_LOW         LATAbits.LATA2 = 0                             /* Chip Select Low */
#define SD_CS_HIGH        LATAbits.LATA2 = 1                             /* Chip Select High */
#define CD               1                                              /* Card detected   (yes:true, no:false, default:true) */
#define WP               0                                              /* Write protected (yes:true, no:false, default:false) */

/* SPI bit rate controls */
#define	FCLK_SLOW		spi1Init(SYNC_MODE_SLOW, 1, 0)                     /* Set slow clock, 100k-400k (Nothing to do) */
#define	FCLK_FAST		spi1Init(SYNC_MODE_FAST, 1, 0)                     /* Set fast clock, depends on the CSD (Nothing to do) */
#define sd_tx(d)        spi1Write(d)
#define sd_rx()         spi1Write(0xFF)
