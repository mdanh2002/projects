#include <p24fj64ga002.h>
#include "delay.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "spi_lib.h"
#include "st7735.h"
#include "colors.h"
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"
#include "sd_helper.h"
#include "ld3320.h"
#include "string.h"
#include "hw_LD3320.h"

// http://dangerousprototypes.com/2012/04/26/bpv3-ftdi-uart-demo-wiki/
// for possible headers, refer to C:\Program Files (x86)\Microchip\MPLAB C30\support\PIC24F\h\p24FJ64GA002.h

_CONFIG2(FNOSC_FRCPLL & OSCIOFNC_OFF & POSCMOD_NONE & I2C1SEL_PRI) // Internal FRC OSC = 8 MHz x 4PLL = 32MHz, Fosc/2 output at RA3
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) //turn off junk we don't need

// In MPLAB X, to select a different debug kit, right click project in treeview, and choose Set Configuration

void initIO() {
    CLKDIVbits.RCDIV0 = 0; // clock divider to 0 (no divider, PIC running at 8MHz internal oscillator speed)

    // connect to MAX232
    AD1PCFG = 0xFFFF; // all digital
    
    ODCB = 0b0000000000000000; // No Open Drain Port
    TRISB = 0b0001000100010100; // PORTB for output, except RP2 (LD3320 Interrupt) RB4 (UART1 RX), RB8 (SD Card Input) and RB12 (LD3320 Input)
    TRISA = 0b0000000000000010; // PORTA all output, except RA1 for MP3/ASR switch

    // interrupt 1 setup 
    RPINR0bits.INT1R = 2; // Assign interrupt 1 to RP2
    INTCON2bits.INT1EP = 1; // faling edge
    IFS1bits.INT1IF = 0;
    IPC5bits.INT1IP = 6;
    IEC1bits.INT1IE = 1;

    // UART Module 1, for PC communication
    RPINR18bits.U1RXR = 4; // pin RP4 for RX
    RPOR1bits.RP3R = 3; // pin RP3, set to 3 for UART1 transmit
    InitUART();

    // this must be done after UART init, other UART will not work
    __builtin_write_OSCCONL(OSCCON & 0xBF); //unlock RPn assign registers

    // SD Card on SPI Module 1
    RPINR20bits.SDI1R = 8; // RP8, MISO - (Master In Slave Out)
    RPOR4bits.RP9R = 7; // RP9, MOSI
    RPOR5bits.RP10R = 8; // RP10, CLK

    // LD3320 on SPI Module 2
    RPINR22bits.SDI2R = 12; // RP12, MISO - (Master In Slave Out). Connected to SDO.
    RPOR5bits.RP11R = 10; // RP11, MOSI - (Master Out Slave In). Connected to SDI.
    RPOR6bits.RP13R = 11; // RP13, CLK - Clock

    __builtin_write_OSCCONL(OSCCON | 0x40); //lock RPn assign registers

    // init SPI 2 module for the LD3320. SPI1 module will be init by the SD card MDD library automatically
    spiInit(2, 0b00011010, 2); // Secondary prescale 2:1, primary prescale 4:1. SPI operates at 8MHz
}

void readTextFile(char* filename) {
    char buf[30];
    int i;
    FSFILE * pointer;

#define BUFFER_LENGTH 128

    unsigned char buffer[BUFFER_LENGTH];

    sprintf(buf, "Loading file %s", filename);
    SendUARTStr(buf);

    pointer = FSfopen(filename, FS_READ);
    if (pointer == NULL) {
        SendUARTStr("Error opening file.");
        return;
    }

    unsigned int bytesRead = 0;
    long totalBytes = 0;

    do {
        bytesRead = FSfread(buffer, 1, BUFFER_LENGTH, pointer);

        for (i = 0; i < bytesRead; i++) {
            SendUART(buffer[i]);
            totalBytes++;
        }
    } while (bytesRead > 0);

    sprintf(buf, "Read file done (%ld bytes)", totalBytes);
    SendUARTStr(buf);

    FSfclose(pointer);
}

#define PRESET_LENGTH 7
char presets[PRESET_LENGTH][15] = {"a li ba ba", "shang hai", "chong qing", "tian jin", "bei jing", "hong kong", "tian an men"};

unsigned char LDAsrAdd() {
    // char buf[100];
    tsLD3320AsrKey keys[PRESET_LENGTH];
    char c;

    /*
     Add the predefined pinyin text to be recognized by the module. The keycode will identify the speech if it is recognized later.
     The KeyCode may be the same and need not be continuous. If multiple text has the same keycode, they will appear as the same
     entry in the recognition result.
     */
    for (c=0;c<PRESET_LENGTH;c++)
    {
        // sprintf(buf, "Added KEY #%d: %s", c, presets[c]);
        // SendUARTStr(buf);
        
        tsLD3320AsrKey key1;
        key1.KeyCode = c+1;
        strcpy(key1.KeyString, presets[c]);
        keys[c] = key1;
    }

    return LD3320AddAsrKey(keys, PRESET_LENGTH);
}

BOOL hasInitLD = FALSE;
BOOL isInMP3Mode = TRUE; // if TRUE, in MP3 mode. If FALSE, in ASR mode

// this interrupt triggers when the LD3320 detects a voice on the microphone
void __attribute__((interrupt, auto_psv))  _INT1Interrupt(void)
{
    IFS1bits.INT1IF = 0;

    if (!hasInitLD)
        return;

    if (isInMP3Mode)
    {
        // in MP3 mode, ignore interrupt sent
        return;
    }

    // in ASR mode, process speech
    char c;
    char buf[255];

    c = LD3320AsrProcess();

    if (c > 0)
    {
        ST7735_fillRect(0, 80, 128, 40, colBlue);

        sprintf(buf, "%s", presets[c-1]);
        ST7735_drawString(0, 80, buf, colCyan, 2);

        sprintf(buf, "LD3320AsrProcess found speech code #%d for '%s'.", c, presets[c-1]);

        // single beep to indicate recognized voice
        LATAbits.LATA0 = 1;
        delay_ms(100);
        LATAbits.LATA0 = 0;
    }
    else
    {
        ST7735_fillRect(0, 80, 128, 40, colBlue);

        ST7735_drawString(0, 80, "UNKNOWN!", colLightPink, 2);
        sprintf(buf, "LD3320AsrProcess failed. Speech cannot be detected.");

        // double beep to indicate unrecognized voice
        LATAbits.LATA0 = 1;
        delay_ms(500);
        LATAbits.LATA0 = 0;
        delay_ms(500);
        LATAbits.LATA0 = 1;
        delay_ms(500);
        LATAbits.LATA0 = 0;
    }

    LD3320Reset();
    LD3320ASRInit();
    LDAsrAdd();
    
    if (LD3320AsrRun() == 1) {
        sprintf(buf, "%s LDAsrRun OK. Continue monitoring.", buf);
    } else {
        sprintf(buf, "%s LD3320AsrRun failed.", buf);
    }
    SendUARTStr(buf);
}

// this interrupt counts the duration of the MP3 playback
unsigned int timerCount = 0;
unsigned int totalSeconds = 0;
void __attribute__((interrupt, auto_psv))  _T4Interrupt (void)
{
    timerCount++;

    if (timerCount%256==0)
    {
        totalSeconds++;
        timerCount = 0;
    }

    _T4IF = 0; //Clear the Flag
}

void initTmr4()
{
    // enable timer 4
    T4CONbits.TON = 0; // Disable Timer
    T4CONbits.TCS = 0; // Select internal instruction cycle clock
    T4CONbits.TGATE = 0; // Disable Gated Timer mode
    T4CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR4 = 0x00; // Clear timer register
    PR4 = 62500; // Load the period value, 256Hz
    IPC6bits.T4IP = 0x01; // Set Timer Interrupt Priority Level
    IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
    IEC1bits.T4IE = 1; // Enable Timer interrupt
    T4CONbits.TON = 1; // Start Timer
}

int main() {
    char c;
    char buf[100];
    unsigned char res;
    BOOL firstLoop = TRUE;
    
    hasInitLD = FALSE;
    isInMP3Mode = PORTAbits.RA1;

    initIO();
    initTmr4();

    SendUARTStr("LD3320 ASR & MP3 Demo Test.");
    
    // show test msg on LCD
    ST7735_initR();
    ST7735_fillScreen(colBlue);
    ST7735_drawString(0, 5,  "LD3320 TEST", colYellow, 2);

    ST7735_drawString(0, 40, "Detecting card ...", colWhite, 1);

    // check SD card status
    SendUARTStr("Initializing SD card...");
    BOOL isOK = initSDCard();
    ST7735_fillRect(0, 40, 128, 20, colBlue);
    if (!isOK) {
        ST7735_drawString(0, 40, "SD Card Error!", colRed, 1);
        SendUARTStr("SD Card Error. Check connections.");
    } else {
        SendUARTStr("Reading card...");

        ST7735_drawString(0, 40, "Reading card ...", colWhite, 1);

        SDCARD_INFO cardInfo = getCardInfo();

        sprintf(buf, "Volume: %s", cardInfo.volumeLabel);
        SendUARTStr(buf);

        sprintf(buf, "Format: %s", cardInfo.fsTypeStr);
        SendUARTStr(buf);

        unsigned int mbTotal = cardInfo.totalSize / 1024 / 1024;
        sprintf(buf, "Size  : %ld MB", mbTotal);
        SendUARTStr(buf);

        unsigned int mbFree = cardInfo.freeSize / 1024 / 1024;
        sprintf(buf, "Free  : %ld MB", mbFree);
        SendUARTStr(buf);

        // readTextFile("love.txt");

        sprintf(buf, "SD: %dMB (%d%% free)", mbTotal, (mbFree * 100) / mbTotal);
        ST7735_fillRect(0, 40, 128, 20, colBlue);
        ST7735_drawString(0, 40, buf, colWhite, 1);
    }

    BOOL isPlayingMP3 = FALSE;
    unsigned int lastTotalSeconds = 0;

    while (1) {
        BOOL oldMode = isInMP3Mode;
        isInMP3Mode = PORTAbits.RA1;

        if (oldMode != isInMP3Mode || firstLoop)
        {
            firstLoop = FALSE;
            
            ST7735_fillRect(0, 60, 128, 40, colBlue);
            
            if (!isInMP3Mode)
            {
                hasInitLD = FALSE;

                // init LD3320
                SendUARTStr("Initializing LD3320 for ASR.");
                LD3320Reset();
                LD3320ASRInit();
                while (!LD3320AsrBusyCheck());
                SendUARTStr("Initializing LD3320 for ASR done.");

                ST7735_drawString(0, 60, "Detecting speech ...", colWhite, 1);
                hasInitLD = TRUE;

                res = LDAsrAdd();
                if (res == 0) {
                    SendUARTStr("LD3320AddAsrKey failed.");
                } else {
                    SendUARTStr("LD3320AddAsrKey OK.");

                    if (LD3320AsrRun() == 1) {
                        SendUARTStr("LD3320 Ready.");
                    } else {
                        SendUARTStr("LD3320AsrRun failed.");
                    }
                }
                
                SendUARTStr("Ready for speech recognition.");
            }
            else
            {
                hasInitLD = FALSE;

                SendUARTStr("Initializing LD3320 for MP3.");
                
                LD3320Reset();
                LD3320MP3Init();
                delay_ms(100);
                hasInitLD = TRUE;
                totalSeconds = 0;

                // LD3320Mp3VolumeSet(2);
                
                SendUARTStr("Initializing LD3320 for MP3 done. Playing MP3 now...");
                ST7735_drawString(0, 60, "Playing MP3 ...", colWhite, 1);

                LD3320Mp3PlayFromSDCard("takes.mp3");

                isPlayingMP3 = TRUE;
                delay_ms(100);
            }
        }

        BOOL oldIsPlayingMP3 = isPlayingMP3;
        if (isPlayingMP3 && isInMP3Mode)
           isPlayingMP3 = LD3320MP3ProcessFromSDCard();

        if (totalSeconds != lastTotalSeconds && isPlayingMP3)
        {
            ST7735_fillRect(0, 80, 128, 40, colBlue);

            sprintf(buf, "%02d:%02d", totalSeconds / 60, totalSeconds % 60);
            
            ST7735_drawString(0, 80, buf, colLightPink, 2);
            
            lastTotalSeconds = totalSeconds;
        }
        
        if (isPlayingMP3 != oldIsPlayingMP3)
        {
            SendUARTStr("Playing MP3 finished.");

            ST7735_fillRect(0, 60, 128, 60, colBlue);
            ST7735_drawString(0, 60, "MP3 stopped ...", colWhite, 1);

            isPlayingMP3 = FALSE;
            delay_ms(100);
        }
    }
}
