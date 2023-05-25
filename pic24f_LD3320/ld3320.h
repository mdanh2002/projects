/* 
 * File:   ld3320.h
 * Author: Minh Danh
 *
 * Created on August 16, 2014, 2:15 PM
 *
 * Purpose is to interface the LD3320 Chinese Speech Recognition & MP3 Player to the PIC24FJ64GA002
 *
 * This module is similiar to the SYNC6288, made by ICroute. Visit OEM webpage at http://www.icroute.com/ (Chinese only)
 * Code adapted from http://www.coocox.org/driver_repo/67e772c0-ccb0-4564-a536-095a6aa7ba3b/lib/CoX/Cox_Driver/
 *
 * A good English writeup of this module is found at  http://www.electrodragon.com/w/index.php?title=LD3320_Datasheet
 *
 * IC Board Pinout
 * ^ indicates active low signal
 *
 * VDD          3.3V Supply
 * GND          Ground
 * RST^         Reset Signal
 * MD           Low for parallel mode, high for serial mode.
 * INTB^        Interrupt output signal for parallel input mode
 * A0           Address or data selection for parallel mode. If high, P0-P7 indicates address, low for data.
 * CLK          Clock input for LD3320 (2-34 MHz). Board already comes with onboard 22.1184 MHz crystal
 * RDB^         Read control signal for parallel input mode
 * CSB^/SCS^    Chip select signal (parallel mode) / SPI chip select signal (serial mode).
 * WRB^/SPIS^   Write Enable (parallel input mode) / Connect to GND in serial mode
 * P0           Data bit 0 for parallel input mode / SDI pin in serial mode
 * P1           Data bit 1 for parallel input mode / SDO pin in serial mode
 * P2           Data bit 2 for parallel input mode / SDCK pin in serial mode
 * P3           Data bit 3 for parallel input mode
 * P4           Data bit 4 for parallel input mode
 * P5           Data bit 5 for parallel input mode
 * P6           Data bit 6 for parallel input mode
 * P7           Data bit 7 for parallel input mode
 * MBS          Microphone bias voltage
 * MONO         Mono Line In 
 * LINL/LINR    Stereo LineIn (Left/Right)
 * HPOL/HPOR    Headphone Output (Left/Right)
 * LOUL/LOUTR   Line Out (Left/Right)
 * MICP/MICN    Microphone Input (Pos/Neg)
 * SPOP/SPON    Speaker Ouput (Pos/Neg)
 */

#ifndef LD3320_H
#define	LD3320_H

// Reset pin
#define   LD3320_RST        LATBbits.LATB15

// SPI chip select pin
#define   LD3320_SPI_CS     LATBbits.LATB14

/* Input frequency in MHz. User need modify this value according to clock in */
#define CLK_IN                  22.1184

// MP3 PLL constants
#define LD_PLL_11               (unsigned char)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(unsigned char)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

// MP3 ASR constants
#define LD_PLL_ASR_19 		(unsigned char)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

// define a pinyin word to be recognized
typedef struct{
    unsigned char KeyCode;
    char KeyString[38];
} tsLD3320AsrKey;

// General Init Method
void LD3320InitCommon(void);

// MP3 Methods
void LD3320MP3Init(void);
void LD3320Mp3VolumeSet(unsigned char val);
unsigned char LD3320MP3ProcessFromSDCard(void);
unsigned char LD3320Mp3PlayFromSDCard(const char *pFilename);
unsigned char LD3320loadMp3FromSDCard();

// ASR Methods
unsigned char LD3320AsrBusyCheck(void);
void LD3320ASRInit(void);
unsigned char LD3320AddAsrKey(tsLD3320AsrKey *pAsrKey, unsigned char ucAsrKeyNUM);
void LD3320MicVolumeSet(unsigned char ucVolume);
unsigned char LD3320AsrRun(void);
unsigned char LD3320AsrProcess(void);

//
// If the LD3320 API can not meet your need, you can use the API below operated
// on the lowlayer.
//
void LD3320Write(unsigned char Address,unsigned char Data);
unsigned char LD3320Read(unsigned char Address);

#endif	/* LD3320_H */
