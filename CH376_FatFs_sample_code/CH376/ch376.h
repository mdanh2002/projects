// Adapted from https://github.com/djuseeq/Ch376msc
// Remove all codes, except for mounting & raw sector read/write code
// To be used with FATFS (easier since CH376 does not support retrieving long file names). Accessing via LFN is OK.

#include "ch376_const.h"

#define CH376_CS                    LATBbits.LATB5                                  // chip select pin SPI
#define CH376_INT                   PORTBbits.RB0                                   // interrupt pin, same as MISO. Check ch376_init() too.

void ch376_clearvars();
void ch376_reset();
BOOL ch376_init();
void ch376_sleep();

uint8_t ch376_setMode(uint8_t mode);

BOOL ch376_pingDevice();
uint8_t ch376_getInterrupt();
uint8_t ch376_spiWaitInterrupt();
BOOL ch376_checkIntMessage(); 

uint8_t ch376_mount(); 
void ch376_driveAttach();
void ch376_driveDetach();
void ch376_setSource(uint8_t inpSource);
uint8_t ch376_getSource();

BOOL ch376_getChipStatus();
BOOL ch376_getUSBMountStatus();
BOOL ch376_getUSBConnectedStatus();

unsigned int ch376_sectorRead(uint32_t lba, uint8_t count, unsigned char* rawSector);
unsigned int ch376_sectorWrite(uint32_t lba, uint8_t count, unsigned char* rawSector);
