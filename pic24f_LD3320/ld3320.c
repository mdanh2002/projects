#include "ld3320.h"
#include "hw_LD3320.h"
#include "delay.h"
#include <p24fj64ga002.h>
#include "spi_lib.h"
#include "uart.h"
#include <stdlib.h>
#include <stdio.h>
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"

// RST and CS signals
#define   LD3320_RST_H()    LD3320_RST = 1;
#define   LD3320_RST_L()    LD3320_RST = 0;
#define   LD3320_SPI_CS_H() LD3320_SPI_CS = 1;
#define   LD3320_SPI_CS_L() LD3320_SPI_CS = 0;

// variables for MP3 playback
unsigned long nMp3StartPos = 0;
unsigned long nMp3Size = 0;
unsigned long nMp3Pos = 0;
unsigned char * pMp3Data;
static FSFILE *g_pInFile;
#define MP3_BUFFER_SIZE 128
unsigned char sdBuf[MP3_BUFFER_SIZE];
unsigned char lastSDReadCount = 0;
unsigned char curSDBufInd = 0;

//*****************************************************************************
//! \brief LD3320 Write
//! \param Address -- Reg Address, Data -- cmd or data
//! \return none
//*****************************************************************************

void LD3320Write(unsigned char Address, unsigned char Data) {
    LD3320_SPI_CS_L();

    spi2Write(CMD_SPI_WRITE);
    spi2Write(Address);
    spi2Write(Data);

    LD3320_SPI_CS_H();
}

//*****************************************************************************
//! \brief LD3320 Read
//! \param Address -- Reg Address
//! \return result
//*****************************************************************************

unsigned char LD3320Read(unsigned char Address) {
    unsigned long pucTemp;

    LD3320_SPI_CS_L();

    spi2Write(CMD_SPI_READ);
    spi2Write(Address);
    pucTemp = spi2Write(0x00);

    LD3320_SPI_CS_H();

    return pucTemp;
}

void LD3320_delay(unsigned long uldata) {
    unsigned int j = 0;
    unsigned int g = 0;
    for (j = 0; j < 5; j++) {
        for (g = 0; g < uldata; g++) {
            // Delay_(120);
            // xSysCtlDelay(120);
            delay_us(120);
        }
    }
}

//*****************************************************************************
//! \brief LD3320 hardware reset
//! \param none
//! \return none
//*****************************************************************************

void LD3320Reset() {
    LD3320_RST_H();
    LD3320_delay(100);
    LD3320_RST_L();
    LD3320_delay(100);
    LD3320_RST_H();
}

//*****************************************************************************
//
//! \brief check the ASR_DSP busy
//!
//! \param none
//!
//! \return 0 - busy; !0 - IDLE
//
//*****************************************************************************

unsigned char LD3320AsrBusyCheck(void) {
    unsigned char i = 5;
    while (--i) {
        if (LD3320Read(REG_ASR_DSP_STATUS_R) == 0x21) {
            break;
        }
        LD3320_delay(10);
    }
    return i;
}

//*****************************************************************************
//! \brief LD3320 init set(General)
//! \param none
//! \return none
//*****************************************************************************

void LD3320InitCommon(void) {
    // soft reset
    LD3320Write(REG_RESET, 0x01);
    LD3320_delay(10);
    LD3320Write(REG_RESET, 0x00);
    LD3320_delay(10);
    // Initialise analog,ADC,DSP,enable sleep,disable ADC,enable dsp sleep;
    LD3320Write(REG_ANALOG_CONTORL2, CMD_ANALOG_CONTORL_INIT);
    LD3320Write(REG_SLEEP, CMD_SLEEP_EN);
    LD3320_delay(10);
    LD3320Write(REG_ADC_CONTROL, CMD_ADC_INIT);

    LD3320Write(REG_DSP_SLEEP, CMD_DSP_SLEEP_EN);
    LD3320Write(REG_DSP_SET, CMD_DSP_SLEEP);

    //LD3320_delay(1);
    LD3320Write(REG_ASR_LENGHT_ADD, 0x00);
    LD3320Write(REG_SLEEP, CMD_SLEEP_EN);
    LD3320Write(0x6F, 0xFF);
}

//*****************************************************************************
//! \brief Init the LD3320 in MP3 mode
//*****************************************************************************
void LD3320MP3Init(void) {
    LD3320InitCommon();

    // PLL setting for MP3
    LD3320Write(0x11, LD_PLL_11);
    LD3320Write(0x1E, 0x00);
    LD3320Write(0x19, LD_PLL_MP3_19);
    LD3320Write(0x1B, LD_PLL_MP3_1B);
    LD3320Write(0x1D, LD_PLL_MP3_1D);

    // mp3 init
    LD3320Write(REG_INIT_SET, CMD_INIT_MP3);
    LD3320Write(REG_DSP_SET, CMD_DSP_ACTIVE);
    LD3320_delay(10);

    // Analog set
    LD3320Write(REG_FEEDBACK_SET, 0x52);
    LD3320Write(REG_LINEOUT_SELECT, CMD_OUT_INIT);
    LD3320Write(REG_HEADSET_L, 0x10); // 0 for maximum volume
    LD3320Write(REG_HEADSET_R, 0x10);
    LD3320Write(REG_GAIN, 0xff);
    LD3320_delay(1);
    LD3320Write(REG_ANALOG_CONTORL1, 0xff);
    LD3320Write(REG_ANALOG_CONTORL2, 0xff);
    LD3320_delay(1);

    // FIFO set
    LD3320Write(REG_FIFO_DATA_LOWERL, 0x00);
    LD3320Write(REG_FIFO_DATA_LOWERH, 0x00);
    LD3320Write(REG_FIFO_DATA_UPPERL, 0xef);
    LD3320Write(REG_FIFO_DATA_UPPERH, 0x07);
    LD3320Write(REG_FIFO_DATA_MCUL, 0x77);
    LD3320Write(REG_FIFO_DATA_MCUH, 0x03);
    LD3320Write(REG_FIFO_DATA_DSPL, 0xbb);
    LD3320Write(REG_FIFO_DATA_DSPH, 0x01);
}

//*****************************************************************************
//! \brief Load the mp3 data from a file on the SD card onto the LD3320 for playback. This method is added by MD.
//! \return 0 if there is an error, 0 if successful
//*****************************************************************************
unsigned char LD3320loadMp3FromSDCard() {
    unsigned char ucStatus;
    unsigned long count;

    count = 0;

    ucStatus = LD3320Read(REG_FIFO_STATUS_R);
    while (!(ucStatus & MASK_FIFO_STATUS_AFULL) && (nMp3Pos < nMp3Size))
    {
        unsigned char dataByte = 0;
        BOOL isBufferAvail = (curSDBufInd < lastSDReadCount);

        // recharge the SD card MP3 read buffer if finished
        if (!isBufferAvail)
        {
            lastSDReadCount = FSfread(sdBuf, 1, MP3_BUFFER_SIZE, g_pInFile);
            isBufferAvail = (lastSDReadCount > 0);
            if (isBufferAvail)
            {
                curSDBufInd = 0;
            }
            else
            {
                SendUARTStr("Error reading data from SD card");
            }
        }

        // extract the data byte for the buffer
        if (isBufferAvail)
        {
            dataByte = sdBuf[curSDBufInd];
            curSDBufInd++;
            
            // send the data byte to the LD3320
            LD3320Write(REG_FIFO_DATA, dataByte);
            ucStatus = LD3320Read(REG_FIFO_STATUS_R);

            nMp3Pos++;
            count++;
        }
        else
        {
            SendUARTStr("Buffer not available.");
        }
    }

    // if buffer is full, we return 1 for the next attempt
    // otherwise we return a value depending on whether any data is available
    if (ucStatus & MASK_FIFO_STATUS_AFULL)
        return 1;
    else
    {
        // no more data, we close the file, considered finish
        if (count == 0)
            FSfclose(g_pInFile);

        return (count > 0);
    }
}

//*****************************************************************************
//! \brief Tell the LD3320 module to prepare to load the data of an MP3 file from the SD card for playback. This method is added by MD.
//! \return 0 if there is an error opening the file, 0 if successful
//*****************************************************************************
unsigned char LD3320Mp3PlayFromSDCard(const char *pFilename) {
    char buf[50];
    
    // open the file
    g_pInFile = FSfopen (pFilename, FS_READ);
    if (!g_pInFile)
    {
        return 0;
    }

    // get the file size
    FSfseek(g_pInFile, 0, SEEK_END);
    nMp3Size = FSftell(g_pInFile);
    FSfseek(g_pInFile, 0, SEEK_SET);

    sprintf(buf, "MP3 File size is %ld bytes", nMp3Size);
    SendUARTStr(buf);

    // load the file data
    nMp3Pos = 0;
    curSDBufInd = 0;
    lastSDReadCount = 0; 
    unsigned char res = LD3320loadMp3FromSDCard();

    // write the register value for playback
    LD3320Write(REG_INT_AUX, 0x00); // Clear Interrupt auxiliary bit

    LD3320Write(REG_INT_ENABLE, CMD_FIFO_INT_EN);
    LD3320Write(REG_FIFO_INT, CMD_FIFODATA_INT_EN);
    LD3320Write(REG_FEEDBACK_SET, CMD_FEEDBACK_INIT_MP3);

    LD3320Write(REG_DSP_SET, CMD_DSP_ACTIVE);
    LD3320Write(REG_MP3_CONTROL, CMD_MP3_START);

    return res;
}

//*****************************************************************************
//! \brief Call this method when an interrupt during MP3 playback occurs, which means the LD3320 needs more MP3 data for playback
//! \return 0 if there is an error, 0 if successful
//*****************************************************************************
unsigned char LD3320MP3ProcessFromSDCard(void) {
    /*
    Interrupts generated sound playback, there are three cases:
    A. All voice data is finished playing.
    B. Sound data has been sent.
    C. Sent sound data finished processing, you need to send new data.
    */

    unsigned char ucHighInt, ucLowInt;

    // save int status, mp3  0x04 and 0x01, ASR 0x10 and 0x00
    ucHighInt = LD3320Read(REG_INT_ENABLE);
    ucLowInt = LD3320Read(REG_INT_AUX);

    LD3320Write(REG_INT_ENABLE, 0);
    LD3320Write(REG_FIFO_INT, 0);
   
    if (LD3320Read(REG_INT_AUX) & CAUSE_MP3_SONG_END) // Interrupt auxiliary bit 5 = 1, mp3 player finished
    {
        SendUARTStr("MP3 Playing Finished.");
        
        // A. All voice data is finished playing.
        LD3320Write(0x2B, 0);
        LD3320Write(REG_INT_AUX, 0);

        LD3320Write(REG_FIFO_CLR, CMD_FIFO_DATA_CLR); // Empty FIFO data
        LD3320_delay(5);
        LD3320Write(0x08, 0);
        LD3320Write(REG_MP3_CONTROL, CMD_MP3_STOP);

        FSfclose(g_pInFile);
        
        return 0;
    }
    
    // C. Sent sound data finished processing, you need to send new data.
    unsigned char res = LD3320loadMp3FromSDCard();
    LD3320Write(0x29, ucHighInt);
    LD3320Write(0x02, ucLowInt);

    return res;
}

//*****************************************************************************
//! \brief Adjust speaker volume
//! \param Vol - speaker volume, 0 -- 15
//! \return none
//*****************************************************************************
void LD3320Mp3VolumeSet(unsigned char val) {
    val = ((15 - val)&0x0f) << 2; // bit[5-2]
    LD3320Write(REG_SPEAK_VOL, val | 0xc3);
    LD3320Write(REG_ANALOG_CONTORL1, CMD_SPEAK_EN);
}

//*****************************************************************************
//! \brief Init the LD3320 in ASR mode
//*****************************************************************************
void LD3320ASRInit() {
    LD3320InitCommon();

    // PLL Setting
    LD3320Write(0x11, LD_PLL_11);
    LD3320Write(0x19, LD_PLL_ASR_19);
    LD3320Write(0x1B, LD_PLL_ASR_1B);
    LD3320Write(0x1D, LD_PLL_ASR_1D);

    // ASR Init
    LD3320Write(REG_INIT_SET, CMD_INIT_ASR);
    LD3320Write(REG_DSP_SET, CMD_DSP_ACTIVE);
    LD3320_delay(5);
    LD3320Write(REG_FIFO_EXT_LOWERL, 0x80);
    LD3320Write(REG_FIFO_EXT_LOWERH, 0x07);
    LD3320Write(REG_FIFO_EXT_UPPERL, 0xff);
    LD3320Write(REG_FIFO_EXT_UPPERH, 0x07);
    LD3320Write(REG_FIFO_EXT_UPPERL, 0);
    LD3320Write(REG_FIFO_EXT_UPPERH, 8);
    LD3320Write(REG_FIFO_EXT_DSPL, 0);
    LD3320Write(REG_FIFO_EXT_DSPH, 8);   
}

//*****************************************************************************
//! \brief Add a pinyin word to be recognized via ASR
//! \param *pAsrKey -- asr key word array; ucAsrKeyNUM -- key word number
//! \return 0 fail; 1 -- successful
//*****************************************************************************
unsigned char LD3320AddAsrKey(tsLD3320AsrKey *pAsrKey, unsigned char ucAsrKeyNUM) {
    unsigned char k, flag;
    unsigned char nAsrAddLength;

    flag = 1;
    for (k = 0; k < ucAsrKeyNUM; k++) {
        if (LD3320AsrBusyCheck() == 0) {
            flag = 0;
            break;
        }
        // add asr index
        LD3320Write(REG_ASR_KEY_INDEX, pAsrKey[k].KeyCode);
        LD3320Write(REG_ASR_KEY_ADD, 0);

        // clr fifo ext
        LD3320Write(REG_FIFO_CLR, CMD_FIFO_EXT_CLR);
        LD3320Write(REG_FIFO_CLR, 0x00);
        LD3320_delay(1);

        // add asrKey to ld3320 fifo ext
        for (nAsrAddLength = 0; nAsrAddLength < 38; nAsrAddLength++) {
            if (pAsrKey[k].KeyString[nAsrAddLength] == 0)
                break;
            LD3320Write(REG_FIFO_EXT, pAsrKey[k].KeyString[nAsrAddLength]); // sRecog[k][nAsrAddLength]);

            // SendUARTStr(pAsrKey[k].KeyString);
        }
        LD3320Write(REG_ASR_LENGHT_ADD, nAsrAddLength);
        LD3320Write(REG_ASR_DSP_STATUS_R, 0xff); //?
        LD3320Write(REG_ASR_CONTROL, CMD_ASR_KEY_ADD);
    }

    return flag;
}

//*****************************************************************************
//! \brief Set the LD3320 ASR microphone volume
//! \param ucVolume -- 0x00 - 0x7F
//! \return 0 fail; 1 -- successful
//*****************************************************************************
void LD3320MicVolumeSet(unsigned char ucVolume) {
    LD3320Write(REG_ADC_GAIN, ucVolume);
}

//*****************************************************************************
//! \brief Start the LD3320 in ASR Speech Recognition mode
//! \return 0 fail; 1 -- successful
//*****************************************************************************
unsigned char LD3320AsrRun(void) {
    // LD3320Write(0xB3, 0);  // turn off speech segment detection
    LD3320Write(0xB6, 20); // maximum voice recognition time in multiple of 100ms (e.g. 30 for 3 seconds)
    LD3320Write(0xB5, 20); // recognition end after this amount of silence duration, in block of 100ms

    LD3320MicVolumeSet(0x10);
    LD3320Write(REG_ADC_SET, 0x09);
    LD3320Write(0xBD, 0x20);    // clr fifo data
    LD3320Write(REG_FIFO_CLR, CMD_FIFO_DATA_CLR);    // clr fifo data
    LD3320_delay(5);
    LD3320Write(REG_FIFO_CLR, 0x00);
    LD3320_delay(5);

    if (LD3320AsrBusyCheck() == 0)
    {
        return 0;
    }

    LD3320Write(REG_ASR_DSP_STATUS_R, 0xff);
    LD3320Write(REG_ASR_CONTROL, CMD_ASR_RECOGNITION_Start);
    LD3320_delay(5);
    // LD3320Write(REG_ADC_SET, CMD_ADC_EN); // MIC enable
    LD3320Write(REG_ADC_SET, 0x07); // line in
    LD3320Write(REG_INT_ENABLE, 0x10);

    LD3320Write(REG_INIT_SET, CMD_INIT_ASR);
    
    return 1;
}

//*****************************************************************************
//! \brief Call when a voice is detected to perform the speech recognition task
//! \return 0 if fail; otherwise return the key code of the detected speech (which cannot be set to 0).
//*****************************************************************************
unsigned char LD3320AsrProcess(void) {
    unsigned char nAsrResCount = 0, nAsrStatus, c;
    char buf[100];

    if ((LD3320Read(REG_INT_NUM_R)&0x10) == 0x10) // have an asr results
    {
        LD3320Write(REG_INT_ENABLE, 0); // Off ASR & FIFO interrupt
        LD3320Write(REG_FIFO_INT, 0); // Off FIFO interrupt (DATA & EXT)
        
        if (LD3320Read(REG_ASR_DSP_STATUS_R) == 0x21 && LD3320Read(REG_ASR_ASR_STATUS_R) == 0x35) {
            nAsrResCount = LD3320Read(REG_ASR_RESULT_NUM);

            if (nAsrResCount > 0 && nAsrResCount <= 4) {
                nAsrStatus = 1;

                c = LD3320Read(REG_ASR_RESULT1);

                sprintf(buf, "LD3320AsrProcess OK, REG_ASR_RESULT1: %d", c);
                SendUARTStr(buf);

                return c;

            } else {
                sprintf(buf, "nAsrResCount invalid: %d", nAsrResCount);
                SendUARTStr(buf);

                nAsrStatus = 0;
            }
        } else {
            SendUARTStr("Register values invalid.");
            nAsrStatus = 0;
        }

        LD3320Write(REG_INT_NUM_R, 0);
        LD3320Write(REG_ADC_SET, 0); // Close ADC

        LD3320Write(REG_INT_ENABLE, 0); // Disable interrupts
        LD3320Write(REG_FIFO_INT, 0); // Close fifo interrupt
        LD3320Write(REG_INT_NUM_R, 0); // Interrupt auxiliary bit
        LD3320Write(REG_INT_AUX, 0);
        LD3320Write(REG_ASR_END, 0);
        LD3320Write(REG_FIFO_CLR, CMD_FIFO_DATA_CLR); // Empty FIFO data
        LD3320Write(REG_FIFO_CLR, 0); // Empty after write
    } else {
        SendUARTStr("No ASR results.");
        nAsrStatus = 0;
    }

    return nAsrStatus;
}
