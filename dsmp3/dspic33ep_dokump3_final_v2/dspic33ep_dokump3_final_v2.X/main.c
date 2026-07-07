#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "uart.h"
#include "queue.h"
#include "string.h"
#include "FATFS/diskio.h"
#include "FATFS/ff.h"
#include "FATFS/mmc_pic.h"
#include "MP3/mad.h"
#include "spi_lib.h"
#include "type_const.h"
#include "pwm.h"
#include "ch376/ch376.h"
#include "icons.h"
#include "st7565.h"
#include "uart.h"
#include "i2c_lib.h"
#include "si473x.h"
#include "radio_const.h"
#include "ssb_patch.h"
#include "dht11.h"
#include "24lc256.h"
#include "shortwave_db.h"
#include "trig.h"

// <editor-fold defaultstate="collapsed" desc="Config Bits">
// refer to C:\Program Files (x86)\Microchip\xc16\v1.21\support\dsPIC33E\h\p33EP512MC502.h for example
                                
// Power on default clock, will be switched to 31kHz upon standby

#pragma config POSCMD = NONE            // Primary Oscillator disabled
#pragma config OSCIOFNC = ON            // OSC2 is general purpose digital I/O pin
#pragma config FCKSM = CSECMD           // Clock switching is enabled, Fail-safe Clock Monitor is disabled

#pragma config FNOSC = FRCPLL            // Fast RC Oscillator with divide-by-N with PLL module (FRCPLL)
#pragma config IESO  = OFF              // Start up with user-selected oscillator source
#pragma config PWMLOCK = OFF            // PWM registers may be written without key sequence

#pragma config FWDTEN = OFF             // Watchdog timer enabled/disabled by user software
#pragma config JTAGEN = OFF             // JTAG is disabled

#pragma config ALTI2C1 = OFF            // Disable I2C1
#pragma config ALTI2C2 = OFF            // Disable I2C2

#pragma config IOL1WAY = OFF            // OFF = Allow multiple PPS reconfiguration
                                        // ON = Allow single PPS reconfiguration
// </editor-fold>

#ifndef USE_NATIVE_PWM
#error Support native PWM only
#endif

// SHDN pin to LT1763 to toggle circuit ON/OFF
#define OUTPUT_PWR_ON       LATBbits.LATB6

// TFMS5400 infrared decoder
#define TFMS_INP                PORTBbits.RB15
#define TMR3_DELAY_CNT          (TMR3)

unsigned char tfmsBitCount = 0;
unsigned char tfmsSignByte1 = 0;
unsigned char tfmsSignByte2 = 0;
unsigned char tfmsDataByte1 = 0;
unsigned char tfmsDataByte2 = 0;
REMOTE_STATE remoteState = REMOTE_IDLE;

// string buffer, mainly for use with debugPrint() macro
#define STR_RES_BUF_LEN     64
char strResBuf[STR_RES_BUF_LEN];

// maximum volume level, from 0 
unsigned char volumeLevel = MAX_VOL_LVL - 1;

// battery percentage (0-100)
#define MIN_BATT_LEVEL_REFRESH_SEC      60
unsigned char battPct = 0;
BOOL isBattError = FALSE;
BOOL hasReadBatteryLvl = FALSE;
unsigned int lastBattAdcVal = 0;
uint32_t lastReadBattLvl = 0;

// Operating Speed and Clock Mode           
unsigned char curOperatingMode = OP_MODE_FASTEST;

// input source (SD/USB/FM)
INPUT_SOURCE inputSource = SOURCE_SD;
BOOL shouldChangeSource = FALSE;

// SI473X objects
BOOL isSI473XOk = FALSE;

// FATFS objects
FATFS fs;
FIL fMediaFile;
DIR dir;
BOOL isInitFatFS = FALSE;
WORD curFileTime = 0;
WORD curFileDate = 0;

// Disk & Keyboard info
BOOL isKbAdcOK = FALSE;
char label[12];
char fsType[8];
unsigned int diskSizeMB = 0;
BOOL hasReleasedKey = FALSE;

// where the checkmark to indicate if a station is broadcasting (according to schedule) is displayed
#define CHECKMARK_LOC_X     118
#define CHECKMARK_LOC_Y     (MENU_LINE_Y - 1)
#define CHECKMARK_WIDTH     9
#define CHECKMARK_HEIGHT    8

// current file & folder info
char shortAudioFilename[256];           // used to store recording file name in radio mode as well
char fullAudioFilename[256];            // used to store station site name in shortwave mode as well
unsigned long int mediaFileSize = 0;
UINT32 ID3v2size = 0;

// name of current radio stations
#define MAX_FM_RDS_STATION_LEN      9
char curStationName[MAX_FM_RDS_STATION_LEN];    // null terminator
char lastStationName[MAX_FM_RDS_STATION_LEN];

// in media mode, name of current folder
char curFolderName[256];

// radio station info entry
#define STN_DISTANCE_INVALID        0xFFFF
BOOL shouldShowMatchCount = FALSE;
BOOL hasStationInfo = FALSE;
BOOL stnIsBroadcasting = FALSE;             // whether the station is broadcasting at this time (according to schedule)
unsigned char stnInfoArrayIndex = 0;        // index of array where station info can be found
unsigned int stnInfoEntryCurIndex = 0;      // current index of station info entry being displayed
unsigned int stnInfoEntryFirstIndex = 0;    // first index of entry where station info can be found
unsigned int stnInfoEntryLastIndex = 0;     // last index of entry where station info can be found
unsigned int stnDistanceKm = 0;             // station distance in km from current location
char stnDirection[3];                       // NE, SE, NW, SW, with null terminator
unsigned int stnStartTime = 0;
unsigned int stnEndTime = 0;
unsigned int stnPower = 0;
unsigned int stnAzimuth = 0;
unsigned int stnCtyLang = 0;
unsigned int stnNameInd = 0;
unsigned char stnOnDays = 0;
int stnLat1 = 0;
int stnLat2 = 0;
int stnLng1 = 0;
int stnLng2 = 0;
        
// overall file & folder stats
unsigned int numOfFoldersInRoot = 0;
unsigned int numOfFilesInFolder = 0;

unsigned int curFolderIndex     = 0;
unsigned int curFileIndex       = 0;

BOOL isPlayGoingNext = FALSE;

// position of file played in the other input source (USB vs SD)
unsigned int bkFolderIndex     = 0;
unsigned int bkFileIndex       = 0;
unsigned int bkPrevDuration       = 0;

// for scrolling MP3 title
unsigned char curTitlePos = 0;
unsigned char lastInfoLen = 0;
BOOL shouldUpdateTitle = FALSE;
BOOL shouldUpdateStatus = FALSE;
BOOL shouldUpdateDuration = FALSE;
BOOL shouldDrawMediaInfo = FALSE;

// flag to enable / disable config edit
BOOL isEditingConfig = FALSE;

// Radio Recording 
unsigned char radioRecordingMode = RECORD_MODE_NONE;
UINT32 totalRecWords = 0;
INT32 recAvgVal = 0;
unsigned char recAvgValCount = 0;
unsigned int lastRecDurationUpdate = 0;

// Radio Variables
BOOL hasSysPosted = FALSE;
BOOL shouldUpdateRadioScreen = FALSE;
BOOL ignorePartialRefreshFreq = FALSE;
unsigned int  radioCurFreq;
unsigned int  radioCurPresetInd = RADIO_INVALID_PRESET_IND;
unsigned int  radioMinFreq;
unsigned int  radioMaxFreq;
unsigned char radioFreqStep;            // kHz for AM, MHz / 10 for FM (e.g. 1 for 0.1MHz)
unsigned char radioCurBand;    
unsigned char radioFMRange; 
unsigned char radioFMDeemp; 
unsigned char radioFMSpacing;
unsigned char radioAMSpacing;
unsigned char radioAMBW;
unsigned char radioSSBBW;
unsigned char radioSSBMode;

int radioSSBBfoOffset = 0;
BOOL justChangedBfoOffset = FALSE;

BOOL radioSSBSync;
uint32_t lastUpdateRadioSignal = 0;
uint32_t lastUpdateRadioWeather = 0;
unsigned int lastUpdateFmRds = 0;
uint32_t lastUpdateFmRdsSuccess = 0;
unsigned char fmRdsPSIndex = 0;
BOOL hasTriedFullRDS = FALSE;
BOOL hasUpdatedFmRdsDisplay = FALSE;
BOOL isRadioFineTune = FALSE;

BOOL isRadioSeeking = FALSE;
BOOL isAutoScanningRadio = FALSE;
BOOL hasRadioBandChanged = FALSE;

// MAD decoder objects
BUFFER      buffer_desc;
U32         pcmLength = 0;
MAD_DECODER decoder_desc;
MAD_ASYNC_STATUS asyncStatus = ASYNC_DEFAULT;
MP3Info mp3info;

// MP3 file info
unsigned int outputMp3samples = 0;
unsigned int samplingRate = 0;
unsigned long int mp3BitRate = 0;
unsigned int totalDecErrors = 0;
BOOL isVBRFile = FALSE;

// for displaying of pressed remote keys    
unsigned char remoteDigitsPressed[MAX_REMOTE_DIGITS_PRESSED];
unsigned char remoteDigitsPressedCount = 0;
BOOL isShowingHelp = FALSE;
BOOL disabledBeep = FALSE;
BOOL isUsingRemote = FALSE;
BOOL isPwrBtnPressed = FALSE;
uint32_t lastPwrBtnPressed = 0;
BOOL isTFMSInt = FALSE;
KEYCODE lastRemoteChar = KEY_INVALID;

// for PCM playback
BOOL is16bitPCM = FALSE;
BOOL isPlayStereo = FALSE;
unsigned char wavBytesPerSample = 0;
unsigned long int outPcmSamples = 0;
unsigned int tmr4IntCount = 0;
int lastLeftVal = 0;
int lastRightVal = 0;    

// RTC variables
// second: 0 .. 59
// minute: 0 .. 59
// hour:   0 .. 23
// day:    1 .. 31
// month:  1 .. 12
// year:   add 2000
unsigned char curSecond, curMinute, curHour, curDay, curMonth, curYear2k;
BOOL isRTCOK = FALSE;
BOOL hasSetRTC = FALSE;
BOOL shouldSetRTC = FALSE;

// temperature measurement 
#define CUR_TEMP_HUMIDITY_INVALID   0xFF
unsigned char curTemperature = CUR_TEMP_HUMIDITY_INVALID;
unsigned char curHumidity = CUR_TEMP_HUMIDITY_INVALID;
BOOL isTempSensorOK = FALSE;

// number of seconds the device has been running without user intervention
// for screen saver purposes only, hence 16-bit is enough
unsigned int totalRunningInactiveSecs = 0;

// number of seconds the device has been running lapsed for reference only
uint32_t totalRunningSecs     = 0;                       

// whether the code should draw the full screen clock in standby mode
BOOL shouldDrawClock = FALSE;

// idle power off settings
unsigned char curIdleOffSetting = IDLE_OFF_5MIN;
unsigned int totalNotPlayingSecs   = 0;             // number of seconds the device has not been playing anything
BOOL shouldEnterStandby = FALSE;
BOOL keepPwrOnInStandby = FALSE;
BOOL shouldExitStandby = FALSE;
BOOL isInStandby = FALSE;
unsigned int idleOffAfterSecs = 0xFFFF;
            
// always power off settings
unsigned char curAlwaysOffSetting = ALWAYS_OFF_60MIN;
unsigned int alwaysOffAfterSecs   = 0xFFFF;

// power source types
unsigned char curPowerType = POWER_TYPE_6_ALKALINE;

// previous status from config files
BOOL isPendingConfig = FALSE;
BOOL isPendingSeek = FALSE;
BOOL isPendingPause = FALSE;

// playback management
BOOL isPlayingMP3 = FALSE;
BOOL isFilePlaying = FALSE;
BOOL isPaused = FALSE;
unsigned int prevDuration = 0;  // in seconds
unsigned int totalDuration = 0; // in seconds
BOOL hasShowInfo = FALSE;
unsigned char hasInitRand = FALSE;
unsigned char mediaInfoKeyIndex = 0;
#define MEDIA_INFO_KEY_COUNT        (5)
        
// play mode
unsigned char curPlayMode = PLAY_MODE_ALL_TRACKS;
BOOL isFilePlayedOK = FALSE;
BOOL forceMono = FALSE;
BOOL isFMStereo = FALSE;
unsigned char curDurationMode = DURATION_MODE_ELAPSED;

// alarm clock
unsigned char alarmHour = 0;
unsigned char alarmMin = 0;
BOOL alarmEnabled = FALSE;
BOOL isPlayingAlarm = FALSE;

// current longitude and latitude
int curMyLat1 = 0;                          
int curMyLat2 = 0;                
int curMyLng1 = 0;                             
int curMyLng2 = 0;

// current timezone
char curTimeZone = 0;

// for convenience, only a single member will be active at any time
union InputDataBuffer {
    // for 8-bit WAV
	UINT8 dataBuffer8[WAV_FILE_READ_BUFFER_SIZE];
    
    // for 16-bit WAV
	INT16 dataBuffer16[WAV_FILE_READ_BUFFER_SIZE];
    
    // for MP3 file
    UWORD mp3_buf[MP3_INPUT_BUF_SIZE];
}; 
union InputDataBuffer dataBuf;

// <editor-fold defaultstate="collapsed" desc="Forward Declarations">
void drawMenuInfo2(char menu[], unsigned char maxLen);
void fetchRadioInfoEntry();
void drawRemoteInput();
INT32 getRemoteNumericValue();
void drawBattery(BOOL updateReadings);
void resetMenuOption();
void startRadio();
int readAnalogANPinSigned(unsigned char an);
unsigned int readAnalogANPinUnsigned(unsigned char an);
void enableAudio(BOOL enabled);
void drawMenuInfo(char menu[]);
void getRadioSSBModeStr(unsigned char val, char buf[]);
void getRadioBandStr(unsigned char band, char buf[]);
void drawFullScreenClock(BOOL showAlarm);
void openFolder(unsigned int folderInd);
void gotoNextFile();
void gotoPreviousFile();
void gotoPreviousFolder();
void gotoNextFolder();
void processPlayMode();
void openFile(unsigned int fileInd);
void gotoFirstFileInFolder();
void processRemoteKey(char lastRemoteKey);
void updateRadioScreen();
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="UART Debug">
#ifdef DEBUG_UART
void SetUARTPin(unsigned char enabled)
{
    RPOR4bits.RP42R = enabled ? 0x01 : 0x00;        // UART 1 TX on RP42 if enabled, else normal GPIO                   
}
    
void EnableUART(unsigned char enabled)   
{               
    // order is important
    if (enabled)
    {        
        SetUARTPin(TRUE);       
        
        U1MODEbits.UARTEN = 1;        
        U1STAbits.UTXEN = 1;    
    }
    else {     
        U1STAbits.UTXEN = 0;            
        U1MODEbits.UARTEN = 0;        
        
        SetUARTPin(FALSE);               
    }   
}

// UART is shared with LCD R/S pin (which does nothing unless there is clock)
// We make sure to disable UART once done so that LCD can be used freely without touching the UART code.
// The UART has a 4-byte deep FIFO buffer. The transmit buffer status only caters for latest byte.
// We need to wait a bit after the last transmission before disabling UART to make sure we don't lose any characters
// If the RS pin stays low for too long, UART receive will get to break condition (not receiving)
// To recover, we must set UART to high for a short duration before sending
void SendUARTStr(char str[])  {
    // UART does not work in LPRC 32kHz mode
    if (curOperatingMode == OP_MODE_STANDBY)
        return;
    
    ST7565_RS = 1; 
    delay_ms(5);     
    EnableUART(TRUE); 
    WriteUARTStr(str); 
    delay_ms(5); 
    EnableUART(FALSE);
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Random Generator">
void initRand()
{
    srand(tmr4IntCount);
}

unsigned int getRandom(unsigned int max)
{
    return (max > 0) ? (abs(rand()) % max) : 0;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="EEPROM Data">    
// length in bytes of config object
#define CONFIG_CFG_LEN                   48                                                          

// the checksum will be calculated on data_bytes, but excluding the crc16Checksum member itself (first 2 bytes)
#define CFG_CHECKSUM_SIZE 2
    
// Configuration data structure
typedef struct _APP_CONFIG {
    unsigned int  crc16Checksum;            
        
    unsigned char curSecond : 6;
    unsigned char curMinute : 6;
    unsigned char curHour : 5;
    unsigned char curDay : 5;
    unsigned char curMonth : 4;
    unsigned char curYear2k : 7;
    
    unsigned int  sdSelectedFolder;
    unsigned int  sdSelectedFile;
    unsigned int  sdFileDuration;  
    
    unsigned int  usbSelectedFolder;
    unsigned int  usbSelectedFile;
    unsigned int  usbFileDuration;  
    
    unsigned int  radioCurFreq;
    unsigned char radioCurBand : 2;    
    unsigned char radioFMRange : 2; 
    unsigned char radioFMDeemp : 1; 
    unsigned char radioFMSpacing : 2;
    unsigned char radioAMSpacing : 1;
    unsigned char radioAMBW : 3;
    unsigned char radioSSBBW : 3;
    unsigned char radioSSBMode : 2;
    unsigned char radioSSBSync : 1;
    
    unsigned char inputSrc : 2;
    
    unsigned char curVolumeLevel : 5;    
    unsigned char curPlayMode : 4;       
    unsigned char durationMode : 4;
    unsigned char powerType : 4;
    unsigned char idleOffMode : 4;
    unsigned char alwaysOffMode : 4;
    
    unsigned char isPaused : 1; 
    unsigned char forceMono : 1;    
    unsigned char disabledBeep : 1;
    
    unsigned char alarmEnabled : 1;
    unsigned char alarmMinute : 6;
    unsigned char alarmHour : 5;
                
    // current time zone (-12, -11, ..., -1, 0, 1, 2, .. 13, 14)
    // maximum 32 values
    char timeZone : 5;
    
    // TODO: bit-trim the fields to only store the range we need
    // Range from -90 to 90 for latitude and -180 to 180 for longitude
    int myLat1;                          //  before decimal place, -12 (-90 .. 90)
    int myLat2;                          //  after decimal place, 456 for -12.456 (0 .. 99)
    int myLng1;                          //  before decimal place, (-180 .. 180)                 
    int myLng2;                          // after decimal place, (0 .. 99)
} APP_CONFIG;

// Union is used to provide access to both config properties and raw bytes for data storage
union ConfigData {
    // config object, size must be smaller than CONFIG_CFG_LEN
    APP_CONFIG config;
    
    // configuration data bytes 
    unsigned char data_bytes[CONFIG_CFG_LEN];
}; 
union ConfigData appConfigObj;

// Generate CRC16 checksum for the given array
// https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
unsigned int gen_crc16(const unsigned char* data_p, unsigned char offset, unsigned char length){
    unsigned char x;
    unsigned int crc = 0xFFFF;
    unsigned char ind = 0;

    for (ind = offset; ind < length; ind++){
        // debugPrint("%u - %02X", ind, data_p[ind]);
        
        x = crc >> 8 ^ data_p[ind];
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}

BOOL readAppConfig()
{
    eeprom_readPage(EEPROM_CFG_DATA_PAGE, CONFIG_CFG_LEN, appConfigObj.data_bytes);
    
    unsigned int crc16csum = gen_crc16(appConfigObj.data_bytes, CFG_CHECKSUM_SIZE, CONFIG_CFG_LEN);
    debugPrint("%04X-%04X", crc16csum, appConfigObj.config.crc16Checksum);    
    if (appConfigObj.config.crc16Checksum == crc16csum)
    {
        // SendUARTStr("CfOk");
        return TRUE;        
    }
    else {
        // SendUARTStr("CfEr");
        return FALSE;
    }
}

// Save the app config in configuration memory
void writeAppConfig(INPUT_SOURCE activeInputSrc)
{                   
    memset(appConfigObj.data_bytes, 0, CONFIG_CFG_LEN);

    // debugPrint("pwrt=%u", curPowerType);
    
    appConfigObj.config.curSecond = curSecond;
    appConfigObj.config.curMinute = curMinute;
    appConfigObj.config.curHour = curHour;
    appConfigObj.config.curDay = curDay;
    appConfigObj.config.curMonth = curMonth;
    appConfigObj.config.curYear2k = curYear2k;
        
    appConfigObj.config.inputSrc = inputSource;
    appConfigObj.config.curVolumeLevel = volumeLevel;
    appConfigObj.config.curPlayMode = curPlayMode;
    appConfigObj.config.isPaused = isPaused;
    appConfigObj.config.durationMode = curDurationMode;
    appConfigObj.config.forceMono = forceMono;
    appConfigObj.config.powerType = curPowerType;
    appConfigObj.config.idleOffMode = curIdleOffSetting;
    appConfigObj.config.alwaysOffMode = curAlwaysOffSetting;    
    appConfigObj.config.disabledBeep = disabledBeep;
    
    appConfigObj.config.alarmEnabled = alarmEnabled;
    appConfigObj.config.alarmHour = alarmHour;
    appConfigObj.config.alarmMinute = alarmMin;
    
    appConfigObj.config.radioCurFreq = radioCurFreq;
    appConfigObj.config.radioCurBand = radioCurBand;
    appConfigObj.config.radioFMRange = radioFMRange;
    appConfigObj.config.radioFMDeemp = radioFMDeemp;
    appConfigObj.config.radioFMSpacing = radioFMSpacing;
    appConfigObj.config.radioAMSpacing = radioAMSpacing;
    appConfigObj.config.radioAMBW = radioAMBW;
    appConfigObj.config.radioSSBBW = radioSSBBW;
    appConfigObj.config.radioSSBMode = radioSSBMode;
    appConfigObj.config.radioSSBSync = radioSSBSync;
    
    appConfigObj.config.myLat1 = curMyLat1;
    appConfigObj.config.myLat2 = curMyLat2;
    appConfigObj.config.myLng1 = curMyLng1;
    appConfigObj.config.myLng2 = curMyLng2;
    
    appConfigObj.config.timeZone = curTimeZone;
    
    if (activeInputSrc == SOURCE_SD || activeInputSrc >= SOURCE_RADIO)
    {
        // in SD mode, backup position params belongs to USB
        appConfigObj.config.sdSelectedFile =  curFileIndex;
        appConfigObj.config.sdSelectedFolder = curFolderIndex;
        appConfigObj.config.sdFileDuration = prevDuration;
        
        appConfigObj.config.usbSelectedFile =  bkFileIndex;
        appConfigObj.config.usbSelectedFolder = bkFolderIndex;
        appConfigObj.config.usbFileDuration = bkPrevDuration;
        
        // debugPrint("SD: %u %u %u, USB: %u %u %u", curFileIndex, curFolderIndex, prevDuration, bkFileIndex, bkFolderIndex, bkPrevDuration);                
    }    
    else 
    {
        // in FM or USB mode, backup position params belongs to SD
        appConfigObj.config.usbSelectedFile =  curFileIndex;
        appConfigObj.config.usbSelectedFolder = curFolderIndex;
        appConfigObj.config.usbFileDuration = prevDuration;
        
        appConfigObj.config.sdSelectedFile =  bkFileIndex;
        appConfigObj.config.sdSelectedFolder = bkFolderIndex;
        appConfigObj.config.sdFileDuration = bkPrevDuration;
        
        // debugPrint("SD: %u %u %u, USB: %u %u %u", bkFileIndex, bkFolderIndex, bkPrevDuration, curFileIndex, curFolderIndex, prevDuration);                
    }
    
    unsigned int csum = gen_crc16(appConfigObj.data_bytes, CFG_CHECKSUM_SIZE, CONFIG_CFG_LEN); 
    appConfigObj.config.crc16Checksum = csum;
        
    eeprom_writePage(EEPROM_CFG_DATA_PAGE, CONFIG_CFG_LEN, appConfigObj.data_bytes);    
    // debugPrint("%02u %02u %02u %04X", curHour, curMinute, curSecond, csum);    
}

// perform read/write self test of the EEPROM. Return 0xFF if OK, else return the failure address
#define EEPROM_SELF_TEST_OK     0xFFFF
unsigned int eeprom_selfTest()
{
    unsigned char i, c;
    unsigned char testPage[64];
        
    // byte read/write, clear first
    for (i = 0; i < EEPROM_PAGE_SIZE; i++)
    {
        eeprom_writeByte(EEPROM_SELF_TEST_PAGE + i, 0);
    }
    for (i = 0; i < EEPROM_PAGE_SIZE; i++)
    {
        eeprom_writeByte(EEPROM_SELF_TEST_PAGE + i, i);
    }
    for (i = 0; i < EEPROM_PAGE_SIZE; i++)
    {
        c = eeprom_readByte(EEPROM_SELF_TEST_PAGE + i);
        if (c != i)
        {
            debugPrint("NVS#%d:%d", i, c);
            return i;
        }
    }
    
    // page read write, clear page first
    memset(testPage, 0, sizeof(testPage));
    eeprom_writePage(EEPROM_SELF_TEST_PAGE, EEPROM_PAGE_SIZE, testPage);
    for (i = 0; i < EEPROM_PAGE_SIZE; i++)
    {
        testPage[i] = i;
    }
    eeprom_writePage(EEPROM_SELF_TEST_PAGE, EEPROM_PAGE_SIZE, testPage);
    memset(testPage, 0, sizeof(testPage));
    eeprom_readPage(EEPROM_SELF_TEST_PAGE, EEPROM_PAGE_SIZE, testPage);
    for (i = 0; i < EEPROM_PAGE_SIZE; i++)
    {
        if (testPage[i] != i)
        {
            debugPrint("NVP#%d:%d", i, c);
            return i;
        }
    }        
    
    debugPrint("NVOK");
    return EEPROM_SELF_TEST_OK;
}

unsigned int getEepromPresetOffset()
{
    unsigned int presetOffset = 0;
    switch (radioCurBand)
    {
        case RADIO_CUR_BAND_MW:
            presetOffset = EEPROM_MW_PRESETS_OFFSET;    
            break;
        case RADIO_CUR_BAND_SW:
            presetOffset = EEPROM_SW_PRESETS_OFFSET;    
            break;
        case RADIO_CUR_BAND_LW:
            presetOffset = EEPROM_LW_PRESETS_OFFSET;                
            break;
        default:
            presetOffset = EEPROM_FM_PRESETS_OFFSET;
            break;            
    }
    return presetOffset;
}

unsigned int readRadioPresetFrequency(unsigned int presetOffset, unsigned int presetInd)
{
    unsigned char lowByte = eeprom_readByte(presetOffset + presetInd * 2);
    unsigned char highByte = eeprom_readByte(presetOffset + presetInd * 2 + 1);
    unsigned int retVal = (highByte << 8) + lowByte;    
    if (retVal < radioMinFreq || retVal > radioMaxFreq)
    {
        retVal = radioMinFreq;
    }
    return retVal;
}

void writeRadioPresetFrequency(unsigned int presetOffset, unsigned int presetInd, unsigned int freq)
{
    unsigned char lowByte = (freq & 0xFF);
    unsigned char highByte = (freq >> 8);
    eeprom_writeByte(presetOffset + presetInd * 2, lowByte);
    eeprom_writeByte(presetOffset + presetInd * 2 + 1, highByte);
}

unsigned int findRadioPresetFrequency(unsigned int freq)
{
    unsigned int presetInd;
    unsigned int presetOffset = getEepromPresetOffset();        
    for (presetInd = 0; presetInd < EEPROM_MAX_BAND_PRESETS; presetInd++)
    {
        unsigned int oFreq = readRadioPresetFrequency(presetOffset, presetInd);
        if (oFreq == freq)
        {
            // debugPrint("%d-%u-%u-%u", radioCurBand, freq, presetInd, getEepromPresetOffset());
            return presetInd;
        }
    }
    
    // SendUARTStr("PrNtFnd");
    return RADIO_INVALID_PRESET_IND;
}

BOOL swUpdateMyLoc(BOOL isLat)
{
    int retLoc1 = 0; // e.g. -123
    int retLoc2 = 0; // e.g. 45 for -123.45
    BOOL isNegative = FALSE;
    
    // 012345 for 123.45, 112345 for -123.45
    signed long int num = getRemoteNumericValue();
    isNegative = (num > 99999);
    num = num % 100000;
    retLoc1 = (num / 100);
    retLoc2 = (num % 100);

    if (isNegative)
    {
        retLoc1 = retLoc1 * (-1);
    }        
    
    BOOL isValid = FALSE;
    if (isLat)
    {
        if (retLoc1 > -90 && retLoc1 < 90)
        {
            curMyLat1 = retLoc1;
            curMyLat2 = retLoc2;
            isValid = TRUE;
        }
    }
    else {
        if (retLoc2 > -180 && retLoc2 < 180)
        {
            curMyLng1 = retLoc1;
            curMyLng2 = retLoc2;
            isValid = TRUE;
        }
    }
    
    // debugPrint("OK:%d,LAT:%d (%d.%d)", isValid, isLat, retLoc1, retLoc2);
    return isValid;
}

// Check if there is information for the provided shortwave frequency (0..30000)
// Return TRUE if found, FALSE otherwise.
// Also return the array in which the first and last instance of frequency information is found (0 .. 3) and the index of the corresponding row.
BOOL swGetFreqInfo(unsigned int freq, unsigned int *minRowInd, unsigned int *maxRowInd, unsigned char *arrayInd)
{
    // debugPrint("%d %d %d %d", SW_FREQ_DB1_COUNT, SW_FREQ_DB2_COUNT, SW_FREQ_DB3_COUNT, SW_FREQ_DB4_COUNT);
            
    unsigned int rw = 0;
    unsigned char ar = 0;
    BOOL hasFound = FALSE;
    for (ar = 0; ar < 4; ar++)
    {
       for (rw = 0; rw < SW_FREQ_DB1_COUNT; rw++)
       {
           int curFreq = 0;
           if (ar == 0)
           {
               curFreq = swFreqDb1[rw][0];
           }
           else if (ar == 1)
           {
               curFreq = swFreqDb2[rw][0];
           }
           else if (ar == 2)
           {
               curFreq = swFreqDb3[rw][0];
           }
           else if (ar == 3)
           {
               curFreq = swFreqDb4[rw][0];
           }
           
           if (curFreq == freq)
           {
               if (!hasFound)
               {
                  *minRowInd = rw;
                  *maxRowInd = rw; 
                  hasFound = TRUE;
               }
               else {
                  *maxRowInd = rw; 
               }
               
               *arrayInd = ar;
           }
           else {
               if (hasFound)
               {
                   break;
               }
           }
       }   
    }    
    
    return hasFound;
}

// Return the name associated with a index in the corresponding shortwave database program memory array
// Return FALSE if index is not found.
BOOL swGetArrayElementName(unsigned int elementInd, unsigned char arrayType, char retVal[])
{
    BOOL found = FALSE;
    unsigned char cInd = 0;
    unsigned int i = 0;
    unsigned int count = 0;
    
    unsigned int arrayCount = 0;
    if (arrayType == SW_ARRAY_COUNTRIES)
    {
        arrayCount = sizeof(swCountries); 
    }
    else if (arrayType == SW_ARRAY_LANGUAGES)
    {
        arrayCount = sizeof(swLanguages); 
    }
    else if (arrayType == SW_ARRAY_SITES)
    {
        arrayCount = sizeof(swSiteNames); 
    }
    else if (arrayType == SW_ARRAY_STATIONS)
    {
        arrayCount = sizeof(swStationNames); 
    }
    
    for (i = 0; i < arrayCount; i++)
    {    
        char thisChar = 0;
        if (arrayType == SW_ARRAY_COUNTRIES)
        {
            thisChar = swCountries[i]; 
        }
        else if (arrayType == SW_ARRAY_LANGUAGES)
        {
            thisChar = swLanguages[i]; 
        }
        else if (arrayType == SW_ARRAY_SITES)
        {
            thisChar = swSiteNames[i]; 
        }
        else if (arrayType == SW_ARRAY_STATIONS)
        {
            thisChar = swStationNames[i]; 
        }
        
        // begin of needed string
        if (count == elementInd)
        {
            found = TRUE;
            retVal[cInd] = thisChar;
            cInd++;
        }
        
        // end of needed string
        if (count == elementInd + 1)
        {            
            break;
        }
        
        // new element marker
        if (thisChar == 0)
        {
            count++;
        }
    }
    
    // end of string marker
    retVal[cInd] = 0;
    
    return found;
}

// too slow to be used on 8kHz
/*
unsigned int findNearestRadioPresetFrequency(unsigned int freq)
{
    unsigned int retVal = 0;
    
    unsigned int presetInd;
    unsigned int presetOffset = getEepromPresetOffset();        
    for (presetInd = 0; presetInd < EEPROM_MAX_BAND_PRESETS; presetInd++)
    {
        // TODO: We assume that unused presets contains invalid values and readRadioPresetFrequency will return minFrequency
        // for this to work, EEPROM should have been flashed with 0xFF, otherwise the navigation will fail
        unsigned int oFreq = readRadioPresetFrequency(presetOffset, presetInd);
        if (oFreq <= freq && oFreq > radioMinFreq)
        {
            retVal = presetInd;
        }
    }
    
    return retVal;
}
*/
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RTC Code">
BOOL isLeapYear(unsigned char year2k)
{
  unsigned int year = 2000 + year2k;
	
  if (year % 400 == 0)          // Exactly divisible by 400 e.g. 1600, 2000
      return TRUE;
  else if (year % 100 == 0)     // Exactly divisible by 100 and not by 400 e.g. 1900, 2100
      return FALSE;
  else if (year % 4 == 0)       // Exactly divisible by 4 and neither by 100 nor 400 e.g. 2020
      return TRUE;
  else                          // Not divisible by 4 or 100 or 400 e.g. 2017, 2018, 2019
      return FALSE;
}

unsigned char getNumOfDaysInMonths(unsigned char month, unsigned char year2k)
{
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
        return 31;
    
    if (month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    
    if (month == 2)
    {
        if (isLeapYear(year2k))
            return 29;
        
        return 28;
    }
    
    return 0;
}

BOOL isValidTime(unsigned char hour, unsigned char min, unsigned char sec)
{
    return (hour >= 0 && hour <= 23 && min >=0 && min <= 59 && sec >=0 && sec <= 59);
}

BOOL isValidDate(unsigned char year, unsigned char month, unsigned char day)
{
    return ( (year >= 0 && year <= 99) && (month >= 1 && month <= 12) && (day >= 1 && day <= getNumOfDaysInMonths(month, 2000 + year)) );
}

void rtcAddSec()
{
	curSecond++;        
	
	// range: 0 .. 59 (inclusive)
    if (curSecond >= 60)
    {
        curSecond = 0;
        curMinute++;
    }    
    
	// range: 0 .. 59
    if (curMinute >= 60)
    {
        curMinute = 0;
        curHour++;
    }        
    
    // range: 0 .. 23
    if (curHour >= 24)
    {
        curHour = 0;             
		curDay++;
    }    
            
    // range: 1 .. maxDays (inclusive)   
	unsigned char maxDays = getNumOfDaysInMonths(curMonth, curYear2k);
    if (curDay > maxDays)
    {
	    curDay = 1;
	    curMonth++;	    
	}
	
	// range: 1 .. 12
	if (curMonth > 12)
	{
		curMonth = 1;
		curYear2k++;
		
		// Not catering for year overflow
	}	
}	

// year is 4 digit format, e.g. 2020
// https://www.hackerearth.com/blog/developers/how-to-find-the-day-of-a-week/
// Sunday = 0, Monday = 1, Tuesday = 2, etc.
unsigned char getDayOfWeek(unsigned int year4d, unsigned int month, unsigned int day)
{
  unsigned char  t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  unsigned int y = year4d - (month < 3);        // subtract a day from the first two months 
  return (y + y/4 - y/100 + y/400 + t[month-1] + day) % 7;
}

/*
void printCurrentDate()
{
    unsigned char dow = getDayOfWeek(2000 + curYear2k, curMonth, curDay);
	debugPrint("%02d:%02d:%02d %02d/%02d/20%02d (%01d, %s)", curHour, curMinute, curSecond, curDay, curMonth, curYear2k, dow, daysOfWeek[dow]);
}
*/

void setRTC(unsigned char hour, unsigned char min, unsigned char sec, unsigned char day, unsigned char month, unsigned char year2k)
{
	totalRunningSecs = 0;
	
	curSecond = sec;
	curMinute = min;
	curHour = hour;
	curDay = day;
    curMonth = month;
    curYear2k = year2k;
    
    hasSetRTC = TRUE;
    shouldSetRTC = FALSE;
    
    // printCurrentDate();
}

BOOL rtcSelfTest()
{
#define TEST_DURATION 2

	unsigned char c;
		
	// Test if RTC crystal is running
	uint32_t secNow = totalRunningSecs;
	// SendUARTStr("RTC Test");
	for (c = 0; c<TEST_DURATION;c++)
	{
		delay_ms(1000);
	}	
	
	c = totalRunningSecs - secNow;
	debugPrint("L:%ds-E:%ds", c, TEST_DURATION);
	if (abs(TEST_DURATION - c) <= 1)
	{	
        // SendUARTStr("RTC OK");
		return TRUE;
	}	
    
    // SendUARTStr("RTC Err");
    return FALSE;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Timers">
/*
The order of the brackets and calculations is very important so as not to lose accuracy or introducing potential compiler bugs in handling UINT32 data types.
Additionally in the delay_us function, we need to be very careful to minimise overheads, especially with short delay duration in 8MHz modes.
The functions have been tested and work fine as-is. Do not modify without careful re-testing.
*/ 

#define FCY_HIGH       (FOSC_HIGH/2)  
#define FCY_LOW        (FOSC_LOW/2)

void delay_us(unsigned int us)
{   
    // cannot merge into a single statement, else delay_us will be inaccurate!
    // be very careful with bracket order!
    if (curOperatingMode == OP_MODE_FASTEST)
    {
        __delay32( us * (FCY_HIGH / 1000000L) );       
    }
    else
    {
        __delay32( us * (FCY_LOW / 1000000L) );       
    }
}

void delay_ms(unsigned int ms)
{   
    unsigned int i;
    for (i = 0; i < ms; i++)
    {
        delay_us(1000);
    }
}

// Timer 4 for PCM playback
void setTmr4Freq(unsigned int freq)
{
	PR4 = (curOperatingMode == OP_MODE_FASTEST ? FOSC_HIGH : FOSC_LOW) / (freq) / 2;   
}
	
void initTmr4()
{
    // See Timers and Interrupts on the PIC32MZ post
    // https://www.aidanmocke.com/blog/2018/11/15/timers/
	// enable timer 4 
	T4CONbits.TON = 0; // Disable Timer
	T4CONbits.TCS = 0; // Select internal instruction cycle clock
	T4CONbits.TGATE = 0; // Disable Gated Timer mode
	T4CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
	TMR4 = 0x00; // Clear timer register
    
	IPC6bits.T4IP = 0x05; // Set Timer Interrupt Priority Level, 7 is highest (do not use), must match the function header declaration
	IFS1bits.T4IF = 0;   // Clear Timer Interrupt Flag
	IEC1bits.T4IE = 1;   // Enable Timer interrupt
	T4CONbits.TON = 1;   // Start Timer 
}

// Timer 1 for RTC
void initTmr1()
{
    T1CONbits.TON = 0;          // Disable Timer
    T1CONbits.TSIDL = 0;        // Continues module operation in Idle mode
    T1CONbits.TCKPS = 0b00;     // 1:1 prescaler
    
    /*
    https://microchipdeveloper.com/8bit:timer1clk
    
    Timer1 Asynchronous Mode
    When the External Clock mode is selected as the Timer1 source, it can run asynchronously to the system clock. 
    The Timer1 External Clock Input Synchronization Control bit (T1SYNC) of the T1CON register can be set to make 
    the external clock un-synchronized. The timer will then increment asynchronously to the internal phase clocks. 
    Running asynchronously allows the external clock source to continue incrementing the timer during Sleep and 
    can generate an interrupt on overflow. The interrupt will wake up the processor so an internal time-based 
    application can be updated. However, special precautions in software are needed to read/write the timer.
    */
    T1CONbits.TSYNC = 1;        // Disable for now, our PIC is always running, either at 31kHz or max. clock
    
    T1CONbits.TCS = 1;          // External clock is from pin T1CK on rising edge, connected to 32768 Hz crystal oscillator circuit
            
    IPC0bits.T1IP = 0x06;       // Interrupt priority level (highest lvl 6 for RTC). Do not use level 7, as __builtin_disi (need to write flash) will not work.
    IFS0bits.T1IF = 0;          // Clear Timer Interrupt Flag
    IEC0bits.T1IE = 1;          // Enable timer 1 interrupt
        
    PR1 = 32768;                // interrupt every 1 second (32768 count)
    T1CONbits.TON = 1;          // Enable timer
}

// Timer 3 for TFMS decoder
void initTmr3()
{
    T3CONbits.TON   = 0;        // Disable timer 3
    T3CONbits.TSIDL = 0;        // Continue in idle mode
    T3CONbits.TGATE = 0;        // Gated time accumulation is disabled
    
    if (curOperatingMode == OP_MODE_FASTEST)
    {
        // In Highest Speed mode, Timer 3 (16-bit) runs at 92MHz / 256 = 360kHz 
        T3CONbits.TCKPS = 0b11;     // 1:256 prescaler 
    }
    else {
        // In Normal Mode, Timer 3 (16-bit) runs at 8MHz / 64 = 125kHz 
        T3CONbits.TCKPS = 0b10;     // 1:64 prescaler 
    }
    
    T3CONbits.TCS   = 0;        // Source: internal clock
    T3CONbits.TON   = 1;        // Enable timer    
}

// Calculate time lapsed in 10ms.
unsigned int TMR3_LAPSED_x10MS()
{
    if (curOperatingMode == OP_MODE_FASTEST)
    {
         return (TMR3_DELAY_CNT / REMOTE_10MS_DIVIDER_FAST_CLK);                      
    }
    else {
        return (TMR3_DELAY_CNT / REMOTE_10MS_DIVIDER_SLOW_CLK);                       
    }   
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="FATFS">
UINT32 get_fattime (void)
{
    /*
       http://elm-chan.org/fsw/ff/doc/fattime.html
       https://os.mbed.com/questions/2431/How-to-set-Date-and-Time-of-SD-file/
     
        bit31:25
            Year origin from the 1980 (0..127, e.g. 37 for 2017)
        bit24:21
            Month (1..12)
        bit20:16
            Day of the month (1..31)
        bit15:11
            Hour (0..23)
        bit10:5
            Minute (0..59)
        bit4:0
            Second / 2 (0..29, e.g. 25 for 50)
     */
    UINT32 retVal;
    
    unsigned int tm_year = 2020;
    unsigned char tm_mon = 8;
    unsigned char tm_mday = 30;
    unsigned char tm_hour = 14;
    unsigned char tm_min = 30;
    unsigned char tm_sec = 40;   
    if (isRTCOK)
    {
        tm_year = 2000 + curYear2k;
        tm_mon = curMonth;
        tm_mday = curDay;
        tm_hour = curHour;
        tm_min = curMinute;
        tm_sec = curSecond;
    }
    return (UINT32)(tm_year - 1980) << 25
         | (UINT32)(tm_mon      ) << 21
         | (UINT32)(tm_mday     ) << 16
         | (UINT32)(tm_hour     ) << 11
         | (UINT32)(tm_min      ) << 5
         | (UINT32)(tm_sec/2    );
    
    
    return retVal = 0;
}

/*
BOOL diskReadWriteTest()
{
    // FATFS Write test code     
    unsigned char ind = 0;
    BOOL isTestOK = TRUE;
    
    UINT32 speedBps = 0;
                   
#define FILE_TEST_COUNT 3
    
    // read and write test to test for data integrity
    for (ind = 0; ind < FILE_TEST_COUNT; ind++)
    {
        char filename[32];
        
        // must be 256
        #define TEST_BUF_LEN        256    

        // can be changed to increase or decrease test sze
        #define TEST_REPS           6000    
                
        char buf[TEST_BUF_LEN];
        
        FIL fTestFile;         

        BOOL isFileCreated = FALSE;
        unsigned int c = 0;
              
        UINT32 testFileSize = (UINT32)TEST_REPS * (UINT32)TEST_BUF_LEN;
        
        sprintf(filename, "\\STEST%u.BIN", ind);
        debugPrint("%s-%lub", filename, testFileSize);

        // write file test
        totalRunningInactiveSecs = 0;
        FRESULT fWRes = f_open(&fTestFile, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (fWRes == FR_OK)
        {
            UINT bytesWritten = 0;    

            for (c = 0; c < TEST_BUF_LEN; c++)
            {
                // some random elements, otherwise USB will cache and return fast result!
                buf[c] = c ^ ind;
            }

            for (c = 0; c < TEST_REPS; c++)
            {
                fWRes = f_write(&fTestFile, buf, TEST_BUF_LEN, &bytesWritten);
                if (fWRes != FR_OK)
                {
                    debugPrint("fw#%u er:%u wr:%u", c, fWRes, bytesWritten);
                    
                    // test fail, break will close current file
                    isTestOK = FALSE;
                    break;
                }
            }

            fWRes = f_close(&fTestFile);
            if (fWRes != FR_OK)
            {
                debugPrint("fcEr:%u", fWRes);
                
                isTestOK = FALSE;                    
                goto finishTestStep;             
            }
            else {                                
                isFileCreated = TRUE;
            }
        }
        else {
            debugPrint("foEr:%u", fWRes);   
            
            isTestOK = FALSE;                    
            goto finishTestStep;                
        }
                        
        // calculate speed based 
        if (totalRunningInactiveSecs > 0)
        {
            speedBps = testFileSize / totalRunningInactiveSecs;
            debugPrint("w:%lub/s(%us)", speedBps, totalRunningInactiveSecs);        
        }
                
        // write OK, now read
        if (isTestOK)
        {
            totalRunningInactiveSecs = 0;
            memset(&fTestFile, 0, sizeof(fTestFile));            
            fWRes = f_open(&fTestFile, filename, FA_READ | FA_OPEN_EXISTING);
            if (fWRes == FR_OK)
            {
                for (c = 0; c < TEST_REPS; c++)
                {
                    UINT bRead = 0;
                    UINT i = 0;
                    memset(buf, 0, TEST_BUF_LEN);
                    fWRes = f_read(&fTestFile, buf, TEST_BUF_LEN, &bRead);
                    if (fWRes == FR_OK)
                    {                        
                        for (i = 0; i < bRead; i++)
                        {
                            char expected = (i % TEST_BUF_LEN) ^ ind;
                            if (buf[i] != expected)
                            {
                                debugPrint("#%u:%u!=%u", i & 0xFF, buf[i] & 0xFF, expected & 0xFF);

                                // test fail, close current file
                                isTestOK = FALSE;
                                goto endWhile;
                            }
                        }
                    }
                    else 
                    {
                        debugPrint("fr#%u er:%u,rd:%u", c, fWRes, bRead);

                        // test fail, break will close current file
                        isTestOK = FALSE;
                        break;
                    }
                }

    endWhile:            
                fWRes = f_close(&fTestFile);
                if (fWRes != FR_OK)
                {
                    debugPrint("fcEr:%u", fWRes);

                    isTestOK = FALSE;                    
                    goto finishTestStep;                
                }

                if (!isTestOK)
                    goto finishTestStep;                
            }
            else
            {
                debugPrint("foEr:%u", fWRes);   

                isTestOK = FALSE;                    
                goto finishTestStep;                
            }
                                
            // calculate speed based        
            if (totalRunningInactiveSecs > 0)
            {
                speedBps = testFileSize / totalRunningInactiveSecs;
                debugPrint("r:%lub/s(%us)", speedBps, totalRunningInactiveSecs);
            }
        }
        
finishTestStep:        
        // delete if file was created
        if (isFileCreated)
        {
            // SendUARTStr("Removing file ...");
            
            fWRes = f_unlink(filename);            
            if (fWRes != FR_OK)
            {
                debugPrint("fdEr:%u", fWRes);

                // break will exit the self-test
                isTestOK = FALSE;                    
                break;                
            }
        }
        
        // if test already fails, no need to continue
        if (!isTestOK)
        {
            break;
        }
    }
            
    debugPrint("USBOK:%d", isTestOK);
    return isTestOK;
}
*/

BOOL initFatFS()
{        
    BOOL retVal = FALSE;
    
    numOfFoldersInRoot = 0;
    numOfFilesInFolder = 0;
    curFolderIndex = 0;
    curFileIndex = 0;
    isInitFatFS = TRUE;
    
    // SendUARTStr("FATMnt");    
    FRESULT res = f_mount(&fs, "", 1);
    if (res == FR_OK)
    {
        // get file system info
        SendUARTStr("Mntd");
                 
        switch (fs.fs_type)
        {
            case FS_FAT12:
                strcpy(fsType, "FAT12");
                break;
            case FS_FAT16:
                strcpy(fsType, "FAT16");
                break;
            case FS_FAT32:
                strcpy(fsType, "FAT32");
                break;
            case FS_EXFAT:
                strcpy(fsType, "EXFAT");
                break;
            default:
                strcpy(fsType, "Unknown");
                break;                
        }
        
        DWORD tot_sect = (fs.n_fatent - 2) * fs.csize;    
        f_getlabel("", label, 0);        
        
        // default label
        if (strlen(label) == 0)
        {
            strcpy(label, "DISK");
        }
        
        // debugPrint("%ld sectors: %s (%s)", tot_sect, label, fsType);
        diskSizeMB = tot_sect / 1024 * SECTOR_SIZE / 1024;  // be careful of order to avoid overflow / inaccuracies
        
        // get root folder info
        openFolder(0);   
        
        retVal = TRUE;
    }
    else {
        SendUARTStr("DskEr");
    }    
    
    isInitFatFS = FALSE;
    return retVal;
}

// Reset FATFS variables, used every boot
void resetFatFS()
{
    memset(&fs, 0, sizeof(fs));
    isInitFatFS = FALSE;
    diskSizeMB = 0;
    curFileTime = 0;
    curFileDate = 0;    
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Interrupt Routines">
void resetTFMSVars()
{
    TMR3_DELAY_CNT = 0;
    remoteState = REMOTE_IDLE;
    tfmsBitCount = 0;
    tfmsSignByte1 = 0;
    tfmsSignByte2 = 0;
    tfmsDataByte1 = 0;
    tfmsDataByte2 = 0;
}

// Interrupt 2 for TFMS module
void __attribute__((interrupt, no_auto_psv))  _INT2Interrupt(void) 
{ 
	IFS1bits.INT2IF = 0;        // Reset Interrupt Flag
    
    isTFMSInt = TRUE;
    
    unsigned char c = 0xFF;
    
    if (TFMS_INP)
    {
        // high input, we now look for high-to-low (e.g. falling edge)
        INTCON2bits.INT2EP = 1;
        // SendUART('/');
    }
    else {
        // low input, we now look for low-to-input (e.g. rising edge)
        INTCON2bits.INT2EP = 0;
        // SendUART('*');
    }
    
    if (TFMS_INP == 0 && remoteState == REMOTE_IDLE)								// wait until low data line (begins of transmission)
    {
        // SendUART('1');
        // SendUART(dataBit ? 'W' : 'X');

        TMR3_DELAY_CNT = 0;
        remoteState = REMOTE_INIT_LOW;
    }	
    else if (TFMS_INP == 1 && remoteState == REMOTE_INIT_LOW) 					// first low phase (9ms) to second high phase
    {						
        if (TMR3_LAPSED_x10MS() > 80)
        {
            // only if initial low phase is long enough
            // SendUART('2');

            TMR3_DELAY_CNT = 0;
            remoteState = REMOTE_INIT_HIGH;	
        }		
        else {					
            // otherwise timeout, do nothing
        }	
    }
    else if (TFMS_INP == 0 && remoteState == REMOTE_INIT_HIGH)					// second high phase (4ms) to first sign data bit
    {		        
        if (TMR3_LAPSED_x10MS() > 30)
        {
            // only if second high phase is long enough
            // SendUART('3');

            TMR3_DELAY_CNT = 0;
            remoteState = REMOTE_SIGN1;		

            tfmsBitCount = 0;					
        }	
        else {
            // otherwise timeout, do nothing
        }	
    }
    else if (remoteState >= REMOTE_SIGN1 && TFMS_INP == 0)						// sign data byte & main dara byte					
    {			    
        // SendUART('4');

        // Binary 0 is represented by a pulse for 0.5ms and binary 1 by a pulse for 1.6ms.
        unsigned char bitVal = (TMR3_LAPSED_x10MS() > 12 ? 1 : 0);
        if (remoteState == REMOTE_SIGN1)
        {
            tfmsSignByte1 = tfmsSignByte1 + (bitVal << tfmsBitCount);
        }	
        else if (remoteState == REMOTE_SIGN2)
        {
            tfmsSignByte2 = tfmsSignByte2 + (bitVal << tfmsBitCount);
        }				
        else if (remoteState == REMOTE_DATABYTE1)
        {
            tfmsDataByte1 = tfmsDataByte1 + (bitVal << tfmsBitCount);
        }				
        else if (remoteState == REMOTE_DATABYTE2)
        {
            tfmsDataByte2 = tfmsDataByte2 + (bitVal << tfmsBitCount);
        }				

        TMR3_DELAY_CNT = 0;
        tfmsBitCount++;

        // exceed 8 bit, switch to next remote stage
        if (tfmsBitCount >= 8)
        {
            // SendUART('|');									
            tfmsBitCount = 0;					
            remoteState++;					
        }	
    }
    else if (remoteState == REMOTE_EXTRADATA)
    {
        // dummy transmission at the end (20ms), ignore
        // status will be reset at main upon timeout
        // SendUART('Y');
        TMR3_DELAY_CNT = 0;			
    }

    // if we are in the receive stage, validate information
	if (remoteState == REMOTE_EXTRADATA)
	{
		if (tfmsSignByte1 == 1 && tfmsSignByte2 == 254 && tfmsDataByte1 == (unsigned char)~tfmsDataByte2)
		{		
			// valid data
			c = tfmsDataByte1;	
            // SendUART('^');
		}	
        else {
            // invalid data
            // SendUART('!');
        }
        
        // debugPrint("%d %d %d %d", tfmsSignByte1, tfmsSignByte2, tfmsDataByte1, tfmsDataByte2);            		
        // debugPrint("%d", tfmsDataByte1);       
	}		
			
    if (c != 0xFF)
    {
        // convert raw remote code to associated key
        unsigned char ind = 0;
        lastRemoteChar = KEY_INVALID;
        for (ind = 0; ind < sizeof(rawRemoteCodes) / 2; ind++)           
        {
            unsigned char rawCode = rawRemoteCodes[ind*2];
            KEYCODE remoteChar = rawRemoteCodes[ind*2+1];             
            
            if (c == rawCode)
            {
                lastRemoteChar = remoteChar;
                break;
            }
        }
        
        // power off from remote control (must turn on again using hardware keypad)
        if (lastRemoteChar == KEY_RED_OFF)
        {
            isPwrBtnPressed = TRUE;
            keepPwrOnInStandby = FALSE;   
            lastRemoteChar = KEY_INVALID;
        }                
        
        // enter mixer mode (analog only) using gray off key
        if (lastRemoteChar == KEY_GRAY_OFF)
        {
            shouldEnterStandby = TRUE;
            keepPwrOnInStandby = TRUE;    
            lastRemoteChar = KEY_INVALID;            
        }
    }
    else {
        // Invalid or no key pressed yet
        lastRemoteChar = KEY_INVALID;       
    }
}

// Interrupt 1 for Push button
void __attribute__((interrupt, no_auto_psv))  _INT1Interrupt(void) 
{ 
	IFS1bits.INT1IF = 0;
        
    // power button press will be processed in main routine
    isPwrBtnPressed = TRUE;  
}

/* 
Convert PCM 16-bit signed value signed to PWM unsigned value

-32768 .. 0 .. 32767 --> 0 .. 1024 .. 2047
*/
unsigned int from16toPWM1(int inp)
{
    return ((inp + 32768) >> PWM_BITS_REM16);
}

// convert PCM 8-bit unsigned value to PWM unsigned value
unsigned int from8toPWM(int inp)
{
    return ((unsigned int)inp << PWM_BITS_REM8);
}

// timer 1 for RTC update only. Do not do anything else in this interrupt
// as it will also be used in 32kHz mode.
void __attribute__((interrupt, no_auto_psv))  _T1Interrupt (void) 
{			
    IFS0bits.T1IF = 0;
    
    // increment RTC and draw clock every minute
    rtcAddSec();
    totalRunningSecs++;    
    if (totalRunningSecs % 60 == 0)
    {
        shouldDrawClock = TRUE;
    }
    
    // marker if alarm is playing
    if (alarmEnabled && curHour == alarmHour && curMinute == alarmMin && curSecond == 0)
    {
        isPlayingAlarm = TRUE;
    }
    
    if (isInStandby)
    {
        totalRunningInactiveSecs++;
    }
}
    
// Timer 4 for PCM playback/recorder
void __attribute__((interrupt, no_auto_psv))  _T4Interrupt (void) 
{			
	IFS1bits.T4IF = 0;      // Clear the Flag        	    
    
    // if timer is exactly the sampling rate, we know 1 second has lapsed
    // this duration is approximate, and independent from the RTC which is very accurate for time keeping
    if (samplingRate > 0 && tmr4IntCount > samplingRate)
    {
        // debugPrint("%u %u %u", isFilePlaying, isPaused, totalNotPlayingSecs);
        
        tmr4IntCount = 0;
        totalRunningInactiveSecs++;
         
        // always power off if not playing after a while
        if (curIdleOffSetting != IDLE_OFF_NEVER)
        {
            if (isFilePlaying && !isPaused)
            {
                totalNotPlayingSecs = 0;
            }
            else {
                totalNotPlayingSecs++;

                if (totalNotPlayingSecs > 30 && isPWMEnabled())
                {
                    enableAudio(FALSE);
                }

                if (idleOffAfterSecs > 0 && (totalNotPlayingSecs > idleOffAfterSecs))
                {
                    // sleep in main, we can't sleep from an interrupt
                    debugPrint("ID:%us>%us", totalNotPlayingSecs, idleOffAfterSecs);
                    shouldEnterStandby = TRUE;
                }
            }
        }

        // always power off after certain duration, regardless of whether device is playing or not
        if (curAlwaysOffSetting != ALWAYS_OFF_NEVER)
        {            
            // alwaysOffAfterSecs = 300;
            if (alwaysOffAfterSecs > 0 && (totalRunningInactiveSecs > alwaysOffAfterSecs))
            {
                debugPrint("OF:%us>%us", totalRunningInactiveSecs, alwaysOffAfterSecs);
                shouldEnterStandby = TRUE;
            }
        }
    }
    tmr4IntCount++;
            
    // not handling recording or playing audio via PWM samples if clock mode is low (e.g. in radio mode)
    if (curOperatingMode != OP_MODE_FASTEST)
        return;    
    
    // recording operation, use left PCM channel only
    BOOL isRecording = (radioRecordingMode != RECORD_MODE_NONE);
    if (isRecording)
    {               
        recAvgVal += readAnalogANPinSigned(RECORDER_INPUT_AN_PIN);      // recording input on AN0 (RA0)
        recAvgValCount++;
        
        // Reduce ADC noise by oversampling and then averaging out N samples to store WAV file at a reduced sample rate
        // This is done in addition to a 44.1kHz RC Filer at the output stage to cut out inaudible frequencies which may
        // result in hissing in the recording due to aliasing caused by our low sampling rate
        // The result is sufficient recording quality at 16-bit (actually 12-bit without full-range, maybe 10-bit) with 8kHz actual rate.
        if (recAvgValCount == MOVING_AVG_COUNT)
        {
            // original code, in essence = x2.
            // int recSam = recAvgVal / recAvgValCount;         // divide by 8, for MOVING_AVG_COUNT = 8
            // queue_put_left(recSam << 4);                     // 12-bit to 16-bit, x16             
            
            // simplified code, x2 = shift 1 bit
            queue_put_left(recAvgVal << 1);
            
            recAvgValCount = 0;
            recAvgVal = 0;
        }        
    }
    
    // during recording, left queue is used for PCM recording buffer
    // audio (mostly beep) is only stored in right queue
    BOOL isRightAvail = queue_get_right(&lastRightVal);
    BOOL isLeftAvail = FALSE;
    if (isRecording)
    {
        isLeftAvail = TRUE;
        lastLeftVal = lastRightVal;
    }
    else {
        isLeftAvail = queue_get_left(&lastLeftVal);
    }
    
    if ( (isLeftAvail || isRightAvail) && volumeLevel > 0)
    {
        // queue avail and volume enabled
        outPcmSamples++;
        
        if (volumeLevel < MAX_VOL_LVL - 1)
        {
            // adjust volume below 100%
            unsigned char divLvl = MAX_VOL_LVL - volumeLevel;
            lastLeftVal = lastLeftVal / divLvl;
            lastRightVal = lastRightVal / divLvl;
        }
        else if (volumeLevel > MAX_VOL_LVL)
        {            
            // adjust volume above 100%
            unsigned char addLvl = volumeLevel - MAX_VOL_LVL + 1;
            lastLeftVal = lastLeftVal * addLvl;
            lastRightVal = lastRightVal * addLvl;
        }
    }
    else {         
        // queue empty or volume off
        // if in radio mode, set PWM to 0V so that we will not send square waves to the amplifier and waste power
        // when there is no audio
        if (inputSource >= SOURCE_RADIO || volumeLevel == 0)
        {
            lastLeftVal = 0;
            lastRightVal = 0;   
        }
    }    
                   
    // for stereo tracks, average out 2 channels if force mono is enabled
    if (forceMono && isPlayStereo)
    {
        int avgVal = lastLeftVal / 2 + lastRightVal / 2; // avoid overflow, do not use brackets!
    
        lastLeftVal = avgVal;
        lastRightVal = avgVal;
    }
    
    // need RC low pass filter for PWM output (multiple stages)
    // stage 1 - 75 ohm + 0.1 uF (PWM high components)
    // stage 2 - 750 ohm + 0.01 uF (high freq)
    // f (cut-off) = 1 / (2 * pi * R * C)
         
    if (is16bitPCM)                         
    {                   
        // 16-bit audio data, convert to PWM data
        PDC1 = from16toPWM1(lastLeftVal);
        PDC2 = from16toPWM1(lastRightVal);
    }
    else
    {  		             
        // 8-bit audio data, bit shift to match PWM value 
        PDC1 = from8toPWM(lastLeftVal);
        PDC2 = from8toPWM(lastRightVal);
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="MP3 Decoding">
MAD_FLOW mp3_get (void *data, MAD_STREAM *stream)
{
    BUFFER *buffer = data;
    U08    *destination;
    UWORD   num_get;
    UWORD   num;
    U32     rest;
   
    destination = buffer->start;                            
    rest = stream->bufend - stream->next_frame;

    // copy rest of unconsumed data to begin of buffer
    if (stream->buffer)
    {        
        memmove(stream->buffer, stream->next_frame, rest); 
        destination = destination + rest;
    }

    // try to read portion of data
    num_get = MP3_INPUT_BUF_SIZE*2 - rest;                                

    FRESULT res = f_read(&fMediaFile, destination, num_get, &num);
                
    // looks like end of file
    // we also ignore the last frame (e.g. bytes read < bytes expected) as it will just contain incomplete data
    // and might crash the decoder
    // This will also cover the cases of skipping ID3v1 + ID3v1 extended (128 + 227 = 355) 
    if (num < num_get || res != FR_OK)
    {
        SendUARTStr("fRdEr");
        return (MAD_FLOW_STOP);                             
    }
    else {
        // debugPrint("f_read: %d", num);
    }

    mad_stream_buffer(stream, buffer->start, (num + rest));

    return (MAD_FLOW_CONTINUE);
}

MAD_FLOW pcm_put (void *data, MAD_HEADER *header, MAD_PCM *pcm)
{    
    MAD_FIXED *left_ch;
    MAD_FIXED *right_ch;
    U32       i;
                       
    // print header for first frame and set timer frequency
    if (!hasShowInfo)
    {
        samplingRate = header->samplerate;
                
        mp3BitRate = header->bitrate;
        totalDuration = mediaFileSize / (header->bitrate / 8);
                
        isPlayStereo = (pcm->channels > 1);
        
        setTmr4Freq(samplingRate);
                    
        hasShowInfo = TRUE;
                     
        shouldUpdateDuration = TRUE;
            
        debugPrint("%lu %u %d %u %u %u", header->bitrate, header->samplerate, header->layer, pcm->channels, pcm->samplerate, totalDuration);        
    }
    
    // marker for VBR MP3 file if bit rate varies
    if (!isVBRFile && header->bitrate != mp3BitRate && mp3BitRate > 0 && header->bitrate > 0)
    {
        SendUARTStr("VBR");
        
        isVBRFile = TRUE;
        shouldUpdateDuration = TRUE;
    }
    
    // add PCM data to queue
    left_ch   = &pcm->samples[0][0];
    right_ch  = &pcm->samples[1][0];    
    for (i = 0; i < pcm->length; i++)
    {   
        int leftVal = *left_ch++;
        if (pcm->channels > 1)
        {
            // stereo, store both channels
            queue_put_left(leftVal);
            queue_put_right(*right_ch++);
        }
        else {
            // mono, same value for both channels
            queue_put_left(leftVal);
            queue_put_right(leftVal);
        }
    }

    pcmLength += pcm->length;
            
    return (MAD_FLOW_CONTINUE);
}

MAD_FLOW user_error (void *data, MAD_STREAM *stream, MAD_FRAME *frame)
{
    // BUFFER *buffer = data;
    // debugPrint("decErr: 0x%04x at %u", stream->error, stream->this_frame - buffer->start);
    totalDecErrors++;
    
    return (MAD_FLOW_CONTINUE);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="MP3 ID3">
void trimStringToLastSpace(char input[], unsigned char length)
{    
    if (length == 0)
        return;
    
    unsigned char k = length - 1;
    unsigned char lastSpace = 0;
    
    // MP3 ID3 tags might also be space-padded instead of zero padded
    // we trim the string at the last space
    while (k > 0)
    {
        // debugPrint("spc: %d", input[k]);
                        
        if (input[k] != 32 && input[k] != 0)
        {
            lastSpace = k;
            break;
        }
        
        if (k == 0)
            break;
        
        k--;
    }
    
    // debugPrint("sp: %d", lastSpace);
        
    // if the string contains all valid characters, we might need to truncate it at the last char
    // otherwise we will be out of bounds    
    if (lastSpace == 0)
        input[0] = 0;
    else if (lastSpace < length - 1)
        input[lastSpace + 1] = 0;
    else
        input[length-1] = 0;
}

void resetMP3Tag()
{
   strcpy(mp3info.album, "");
   strcpy(mp3info.artist, "");
   strcpy(mp3info.comment, "");
   strcpy(mp3info.tag, "");
   strcpy(mp3info.title, "");
   strcpy(mp3info.year, "");    
   mp3info.genre = 0;   
}

// remove non-displayable ASCII character
// remove also question mark as it may indicate corrupted field
void removeNonAscii(char input[], unsigned char length)
{
    unsigned char c;
    for (c=0; c<length; c++)
    {
        if (input[c] < 32 || input[c] > 126 || input[c] == '?')
        {
            input[c] = 32;
        }           
    }
}

BOOL reaID3v1Tags()
{
	BOOL isOk = FALSE;

	FIL fMp3; 
    FRESULT res = f_open(&fMp3, fullAudioFilename, FA_READ);
	
	if (res != FR_OK) 
	{
		SendUARTStr("FleEr");
	}	
	else {	
        mediaFileSize = f_size(&fMp3);
        // debugPrint("MP3 Size: %lu", mediaFileSize);
                
		res = f_lseek(&fMp3, mediaFileSize - sizeof(mp3info));
		if (res != FR_OK) 
		{
			SendUARTStr("ID1RdEr");		
		}	
		else {
            unsigned int bytesRead = 0;
			res = f_read(&fMp3, &mp3info, sizeof(mp3info), &bytesRead);
			if (res != FR_OK || bytesRead == 0) {
				SendUARTStr("ID1Er");				
			}	
			else if (strncmp(mp3info.tag, "TAG", 3) == 0) {						
                // handle for invalid data in header
                // if string length is invalid, we will ignore the tag entirely
                // accept alphanumeric up to max. length only
                removeNonAscii(mp3info.title, MAX_ID3_LEN);
                trimStringToLastSpace(mp3info.title, MAX_ID3_LEN);                
                if (strlen(mp3info.title) > 0)
                    SendUARTStr(mp3info.title);
                
                removeNonAscii(mp3info.artist, MAX_ID3_LEN);
                trimStringToLastSpace(mp3info.artist, MAX_ID3_LEN);                
                if (strlen(mp3info.artist) > 0)
                    SendUARTStr(mp3info.artist);		
                
                removeNonAscii(mp3info.album, MAX_ID3_LEN);
                trimStringToLastSpace(mp3info.album, MAX_ID3_LEN);                
                if (strlen(mp3info.album) > 0)
                    SendUARTStr(mp3info.album);		
                                                
                removeNonAscii(mp3info.comment, MAX_ID3_LEN);
                trimStringToLastSpace(mp3info.comment, MAX_ID3_LEN);                                
                if (strlen(mp3info.comment) > 0)
                    SendUARTStr(mp3info.comment);		
				
                // year will be processed later when the info is displayed
                removeNonAscii(mp3info.year, MAX_ID3_YEAR_LEN);                
                
				isOk = TRUE;
			}
			else {
				SendUARTStr("NoID1");						
			}	
		}
		
		res = f_close(&fMp3); 
		if (res != FR_OK)
		{
			SendUARTStr("MP3ClEr");				
		}	
	}	
	
    if (!isOk)
    {    
       resetMP3Tag();
    }
    
	return isOk;
}

// Find out the size of the ID3v2 header. Returns 0 if not present
// ID3v2 header are stored at the beginning of the file, unlike ID3v2 which is stored at the end
// Simple audio utilities will save both ID3v2 and ID3v1, subject to certain limitations
// The ID3v2 header needs to be skipped while playing, otherwise the code might lock to incorrect frames (sync words are not perfect)
// and will crash. This is the algorithm to find the size of the header:
// See also
// http://fileformats.archiveteam.org/wiki/ID3
// https://pypi.org/project/mp3hash/
// https://stackoverflow.com/questions/5223025/why-do-mp3-files-use-synchsafe-integers
/*
Let OFFSET = 0.
Read and remember the first 10 bytes of the file.
If bytes 0-2 are not ASCII "ID3", stop. An ID3v2 segment is not present.
Let OFFSET = 10 (for the 10-byte header).
Decode bytes 6-9 as a 32-bit "synchsafe int" (refer to any ID3v2 spec). Let OFFSET = OFFSET + this decoded int.
If the 4th bit of byte 5 is set (e.g. 0b10000), let OFFSET = OFFSET + 10 (for the footer).
*/
// Our code for now only reads ID3V1 tags (128 byte). We will skip ID3v2 tag and ignore ID3V1 extended tags.
// ID3v1 extended is 227 bytes before regular ID3v1 tag starting with ?TAG+?
// These tags does not seem to cause issue for now and will not have to be skipped while playing back
UINT32 getID3v2Size()
{
    UINT32 offset = 0;
    FIL fMp3; 
    FRESULT res = f_open(&fMp3, fullAudioFilename, FA_READ);
    if (res != FR_OK)
    {
        SendUARTStr("FleOpEr");
    }
    else
    {
        UINT bRead = 0;
        res = f_read(&fMp3, &strResBuf, 10, &bRead);
        if (res == FR_OK)
        {
            if (strResBuf[0] != 'I' || strResBuf[1] != 'D' || strResBuf[2] != '3')
            {
                SendUARTStr("NoID2");
            }
            else 
            {
                offset = 10;

                UINT32 byte0 = strResBuf[6];
                UINT32 byte1 = strResBuf[7];
                UINT32 byte2 = strResBuf[8];
                UINT32 byte3 = strResBuf[9];

                UINT32 tot = (byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3);
                offset = offset + tot;

                if (strResBuf[5] & 0b10000)
                {
                    offset += 10;
                }

                res = f_close(&fMp3); 
                if (res != FR_OK)
                {
                    SendUARTStr("FleClEr");
                }
            }
        }
    }
    
    return offset;    
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Beep via PWM">
void enableAudio(BOOL enabled)
{
    // if source is radio, we mute radio before enabling audio
    // otherwise the sum of the audio voltages will exceed the input voltage of the amplifier
    // and oscillation will result
    if (inputSource == SOURCE_RADIO && hasSysPosted)
    {
        if (!isPaused)                
        {
            si473x_setAudioMute(enabled);
        }
    }
    
    enablePWM(enabled);
        
    // if audio is disabled, set voltage on output to low
    // otherwise 3.3V will be added to the amplifier and disturb other audio sources
    if (!enabled)
    {
        LATBbits.LATB14 = 0;
        LATBbits.LATB12 = 0;
    }    
}

unsigned int countTotalBits(unsigned int n) 
{ 
    unsigned int count = 0; 
    while (n) { 
        // count += n & 1; 
        count++;
        n >>= 1; 
    } 
    return count; 
}

#define TINY_BEEP_LENGTH        100
#define SHORT_BEEP_LENGTH       200
#define LONG_BEEP_LENGTH        1000
#define BEEP_SPACE_LENGTH       500

void beep(unsigned int durationMs, BOOL shouldDelay)
{
    unsigned int j, k;
            
    if (!isPWMEnabled())
    {
        enableAudio(TRUE);
        totalNotPlayingSecs = 0;
    }
    
    // debugPrint("bms=%u dl=%u cl=%u", durationMs, shouldDelay, curOperatingMode);
    
    if (curOperatingMode == OP_MODE_FASTEST)
    {
        // no beep if sampling rate has not been set
        if (samplingRate == 0)
        {
            return;
        }

        // In Fast RC PLL mode, PWM is at 88Khz (11-bit) inaudible, we generate samples for the beep   
        unsigned int totalSamples = (samplingRate / 1000) * durationMs;
        unsigned int repeatTimes = BEEP_FREQ / (1000 / durationMs);         
        unsigned int count = totalSamples / repeatTimes;    
        unsigned char bits = countTotalBits(count);

        // debugPrint("D:%u S: %u F: %u R: %u C: %u B: %d", durationMs, totalSamples, samplingRate, repeatTimes, count, bits);

        // sound data for durationMs
        for (j = 0; j < repeatTimes; j++)
        {
            for (k = 0; k < count; k++)                        
            {
                INT16 val1 = 0;

                if (is16bitPCM)
                {
                    val1 = k << (16 - bits);
                }
                else if (bits >= 8)
                {
                    val1 = k >> (bits - 8);
                }
                else {
                    val1 = k << (8 - bits);
                }

                // reduce volume, otherwise the played sounds might cause power surge
                // speaker is designed for sine wave, not square wave or trapezoid/triangular waves which we are generated
                val1 = val1 / 4;
                
                // beep left speaker only when not recording
                if (radioRecordingMode == RECORD_MODE_NONE)
                {
                    queue_put_left(val1); 
                }
                queue_put_right(val1); 
            }
        }

        // wait for sound to finish playing
        if (shouldDelay)
        {
            delay_ms(durationMs + BEEP_SPACE_LENGTH);
        } 
    }
    else {
        if (volumeLevel > 0)
        {
            // only if not muted
            // in radio mode, PWM (low-speed square wave) is audio by itself
            // we should reduce duty cycle and therefore volume, otherwise 50% duty cycle square wave as beep will sound "harsh"
            PDC1 = PWM_VALUE / 8;
            PDC2 = PWM_VALUE / 8;

            delay_ms(durationMs);

            PDC1 = 0;
            PDC2 = 0;

            if (shouldDelay)
            {
                delay_ms(BEEP_SPACE_LENGTH);
            }

            enableAudio(FALSE);
        }
    }                        
}

void tinyBeep(BOOL delay)
{
    beep(TINY_BEEP_LENGTH, delay);
}

void shortBeep()
{    
    beep(SHORT_BEEP_LENGTH, TRUE);
}

void longBeep()
{    
    beep(LONG_BEEP_LENGTH, TRUE);
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ADC, Battery & Sensors">
void initADC()
{    
    AD1CON1bits.ADON = 0;          // A/D converter is OFF
    
    // https://turbofuture.com/misc/How-Use-ADCs-in-dspic30f-and-dspic33f    
   
    // disable input scan
    AD1CSSH = 0;                   
    AD1CSSL = 0;
        
    // 1 = 12-bit, 1-channel ADC operation                                      
    // 0 = 10-bit, 4-channel ADC operation
    AD1CON1bits.AD12B = 1;         
    
    AD1CON1bits.SSRCG = 0;         // Sample Trigger Source Group bit (see SSRC)
    AD1CON1bits.SSRC =  0b111;     // Internal counter ends sampling and starts conversion (auto-convert), when SSRCG = 0
    
    AD1CON1bits.ADSIDL = 1;        // Discontinues module operation when device enters Idle mode
    
    /*
    AD1CON1bits.FORM = 0;          // Data output in unsigned integer format by default
                                   // Will be changed to sign during recording
    */
    
    /*
    Simultaneous SAMPLING is not simultaneous CONVERSION !
    Only the sampling is simultaneous. You still have to multiplex the ADC inputs to convert the stored samples - 
    one at a time. Whether you do that polling (switch the MUX, start the conversion, wait for the conversion to complete, switch the MUX etc.) 
    or via interrupt (eventually using the conversion time to good purpose) is up to you.      
    */ 
    // AD1CON1bits.SIMSAM = 0;        // No need to samples CH0, CH1, CH2, CH3 simultaneously (10-bit mode only)
    
    AD1CON1bits.ASAM = 0;          // Sampling begins when the SAMP bit is set    
    AD1CON1bits.SAMP = 0;          // Write 0 to end sampling and start conversion, 1 to begin sampling
               
    AD1CON2bits.VCFG = 0;          // Voltage Reference, VREFL = AVSS, VREFH = AVDD    
    AD1CON2bits.CSCNA = 0;         // Disable input scan        
    
    /*
    if (!is12Bit)
    {
        AD1CON2bits.CHPS = 0b11;       // Converts CH0, CH1, CH2, CH3 (10-bit mode only)
    }
    */
    
    AD1CON2bits.ALTS = 0;          // Uses MUX A input multiplexer settings
    AD1CON2bits.BUFM = 0;          // Buffer configured as one 16-word buffer ADCBUF(15...0)
    
    AD1CON3bits.SAMC = 0b11111;    // 31 TAD for auto-sample time   
    AD1CON3bits.ADCS = 255;        // +1 = TAD for conversion clock    
    AD1CON3bits.ADRC = 1;          // Selecting Conversion clock source derived from system clock        
    AD1CON1bits.ADON = 1;          // Enable AD converter
}

unsigned int readAnalogANPinUnsigned(unsigned char an)
{                   
    AD1CHS0bits.CH0SA = an;        // AN1 as positive input to channel 0
    AD1CHS0bits.CH0NA = 0;         // VREF- as negative input to channel 0    
        
    AD1CON1bits.FORM = 0;           // unsigned
    AD1CON1bits.SAMP = 1;           // start sampling, automatic conversion will follow
    
    // wait for a while to complete the conversion
    unsigned char c = 0;
    while (AD1CON1bits.DONE == 0)
    {
        if (c > 200)
        {
            SendUARTStr("ADEr");
            return 0;            
        }
        c++;
    } 

    // Conversions are automatically stored in a dedicated 16-word buffer (ADC1BUF0 .. ADC1BUF15), allowing for multiple successive 
    // readings to be taken before software service is needed. Successive conversions are placed into sequential buffer locations.
    return ADC1BUF0;              
}

int readAnalogANPinSigned(unsigned char an)
{                   
    AD1CHS0bits.CH0SA = an;        // AN1 as positive input to channel 0
    AD1CHS0bits.CH0NA = 0;         // VREF- as negative input to channel 0    
        
    AD1CON1bits.FORM = 1;           // signed
    AD1CON1bits.SAMP = 1;           // start sampling, automatic conversion will follow
    
    // wait for a while to complete the conversion
    unsigned char c = 0;
    while (AD1CON1bits.DONE == 0)
    {
        if (c > 200)
        {
            SendUARTStr("ADEr");
            return 0;            
        }
        c++;
    } 

    // Conversions are automatically stored in a dedicated 16-word buffer (ADC1BUF0 .. ADC1BUF15), allowing for multiple successive 
    // readings to be taken before software service is needed. Successive conversions are placed into sequential buffer locations.
    return ADC1BUF0;              
}

float getRawBattVoltage(unsigned int rawBattVal)
{
    // input goes through a divider, subject to VREF (3.3V), 10-bit unsigned (0 .. 1023), connected through a 1N4148 diode to prevent reverse polarity
    // need to cater for voltage drop
    return (rawBattVal * 13.8 / MAX_12B_ADC_STEPS) + 0.55;
}

float getBattVoltAvg(unsigned int *rawVal)
{
    unsigned int adcVal = 0;
    
    // do not update battery level too often, which will just cause flickering indicator due to ADC noises
    if (hasReadBatteryLvl && (totalRunningSecs - lastReadBattLvl < MIN_BATT_LEVEL_REFRESH_SEC) )
    {
        SendUARTStr("BtCh");
        adcVal = lastBattAdcVal;
    }
    else 
    {
        SendUARTStr("BtRd");
        
#define ADC_ATTEMPTS 32    
        // take the maximum of several samples
        // this is needed because the input voltage is not regulated and may appear to decrease if there's intermittent high current consumption
        unsigned char attempt = 0;
        unsigned int tempVal = 0;
        for (attempt = 0; attempt < ADC_ATTEMPTS; attempt++) {
            // battery connected to AN1
            tempVal = readAnalogANPinUnsigned(BATTERY_INPUT_AN_PIN);
            if (tempVal > adcVal)
                adcVal = tempVal;
        }
        
        lastBattAdcVal = adcVal;
        lastReadBattLvl = totalRunningSecs;
        hasReadBatteryLvl = TRUE;
    }
        
    *rawVal = adcVal;
    return getRawBattVoltage(adcVal);
}

unsigned char readBattLevel(unsigned int *rawVal, float *vin, BOOL *isError)
{    
    *isError = FALSE;
    *vin = getBattVoltAvg(rawVal);
    
    // PIC operating voltage is 3.0 to 3.6V.
    // Voltage read are board input voltage connected through regulators. Expected to be at least 4V (4xNiMH batteries)
    // If it's less than 3V, we know there's an ADC error (hardware or software)
    if (*vin < INVALID_BATT_VOLTAGE)
    {
        *isError = TRUE;        
        return 0;
    }
            
    if (curPowerType == POWER_TYPE_DC)
    {
        // on DC, always 100%
        return 100;
    }
    else {
        unsigned char batteryCount = 6;
        if (curPowerType == POWER_TYPE_2_18650)
        {
            batteryCount = 2;
        }
        else if (curPowerType == POWER_TYPE_9V_NIMH || curPowerType == POWER_TYPE_9V_ALKALINE)
        {
            batteryCount = 1;
        }

        // divide by number of batteries
        float vBatt = *vin / batteryCount;
        if (curPowerType == POWER_TYPE_2_18650)
        {
            // 18650 batteries, 3.7V nominal per cell
            if (vBatt > 4.1)
            {
                if (vBatt > 4.3)
                {
                    *isError = TRUE;
                }
                
                return 100;
            }
            else if (vBatt > 4.0)
            {
                return 80;
            }
            else if (vBatt > 3.9)
            {
                return 60;
            }
            else if (vBatt > 3.8)
            {
                return 40;
            }
            else if (vBatt > 3.7)
            {
                return 10;
            }
            else {
                return 0;
            }
        }    
        else if (curPowerType == POWER_TYPE_6_NIMH)
        {
            // Ni-MH batteries, 1.2V nominal per cell
            if (vBatt >= 1.3)
            {
                if (vBatt > 1.45)
                {
                    *isError = TRUE;
                }
                
                return 100;
            }
            else if (vBatt >= 1.25)
            {
                return 80;
            }
            else if (vBatt >= 1.2)
            {
                return 40;
            }
            else if (vBatt >= 1.15)
            {
                return 20;
            }
            else {
                return 0;
            }
        }
        else if (curPowerType == POWER_TYPE_9V_NIMH || curPowerType == POWER_TYPE_9V_ALKALINE)
        {
            // 9V Alkaline battery
            if (vBatt > 9)
            {
                if (vBatt > 9.8)
                {
                    *isError = TRUE;
                }
                
                return 100;
            }
            else if (vBatt > 8.5)
            {
                return 90;
            }
            else if (vBatt > 8.0)
            {
                return 80;
            }
            else if (vBatt > 7.5)
            {
                return 70;
            }
            else if (vBatt > 7.0)
            {
                return 60;
            }
            else if (vBatt > 6.5)
            {
                return 40;
            }
            else if (vBatt > 6.0)
            {
                return 20;
            }
            else {
                return 0;
            }
        }
        else {
            // alkaline batteries, 1.5V nominal per cell
            if (vBatt > 1.55)
            {
                if (vBatt > 1.7)
                {
                    *isError = TRUE;
                }
                
                return 100;
            }
            else if (vBatt > 1.55)
            {
                return 80;
            }
            else if (vBatt > 1.45)
            {
                return 60;
            }
            else if (vBatt > 1.4)
            {
                return 40;
            }
            else if (vBatt > 1.35)
            {
                return 20;
            }
            else {
                return 0;
            }
        }
    }
}

BOOL formatBatteryVoltage(unsigned int rawBattVal, float vin, unsigned char battPct, BOOL isBoot, BOOL isBattError)
{
    // sprintf with float %.2f will cause crash later!
    // this is a workaround
    unsigned char vin1 = (unsigned int)(vin * 10) / 10;
    unsigned char vin2 = (unsigned int)(vin * 10) % 10;

    if (isBoot)
    {
        sprintf(strResBuf,  "BAT: %d.%dV", vin1, vin2);
    }
    else {
        if (curPowerType != POWER_TYPE_DC)
        {
            sprintf(strResBuf,  "%s (%d.%dV, %d%%)", isBattError ? "Err" : "OK", vin1, vin2, battPct);
        }
        else {
            sprintf(strResBuf,  "%s (%d.%dV)", isBattError ? "Err" : "OK", vin1, vin2);
        }
    }

    return TRUE;
}

// keypad connected via 10K resistor ladder to analog input
// see: http://www.ignorantofthings.com/2018/07/the-perfect-multi-button-input-resistor.html for details
unsigned int getKeyboardADC()
{
    if (radioRecordingMode != RECORD_MODE_NONE)
        return MAX_10B_ADC_STEPS;
    
    return readAnalogANPinUnsigned(KEYBOARD_INPUT_AN_PIN);
}

BOOL isAnalogKeyPressed() {
    return (getKeyboardADC() < ANALOG_KEY_MAX_ADC);
}

// Return the code of the key pressed, 0 if no key has been pressed
unsigned char getKeyboardInput()
{    
#define MAX_KEYB_ADC_COUNT      10
#define MAX_KEYB_ADC_COUNT_5TH  (MAX_KEYB_ADC_COUNT / 5)        
    unsigned int allVals[MAX_KEYB_ADC_COUNT];   
    unsigned int c1, c2, temp;
    unsigned char retKey = 0;
           
    // simple algorithm to reduces SDC noises
    // retrieve raw keyboard ADC values
    // if a key is released half way through, take it as no key has been pressed
    for (c1 = 0; c1 < MAX_KEYB_ADC_COUNT; c1++)
    {
        allVals[c1] = getKeyboardADC();        
        if (allVals[c1] > ANALOG_KEY_MAX_ADC)
            return retKey;
    }
    
    // sort ascending
    for (c1 = 0; c1 < MAX_KEYB_ADC_COUNT - 1; c1++)
        for (c2 = c1 + 1; c2 < MAX_KEYB_ADC_COUNT; c2++)
        {
            if (allVals[c1] > allVals[c2])
            {
                temp = allVals[c1];
                allVals[c1] = allVals[c2];
                allVals[c2] = temp;
            }
        }
        
    // take average of ADC values, ignore bottom and top (e.g. outliers)
    float avgVal = 0;
    for (c1 = MAX_KEYB_ADC_COUNT_5TH; c1 < 4 * MAX_KEYB_ADC_COUNT_5TH; c1++)
    {
        avgVal += allVals[c1];
    }                
    avgVal = ceilf(avgVal / (3 * MAX_KEYB_ADC_COUNT_5TH));
    unsigned int keybAdc = (unsigned int)avgVal;
    
    // debugPrint("KB=%u", keybAdc);    
    if (keybAdc < 100) 
    {
        retKey = KEY_RESET_POWER;
    }
    else if (keybAdc < 2400)
    {   
        retKey = KEY_APP;
    }
    else if (keybAdc < 2800)
    {
        retKey = KEY_UP;
    }
    else if (keybAdc < 3000)
    {
        retKey = KEY_DOWN;
    }    
    else if (keybAdc < 3200)
    {
        retKey = KEY_SETUP;
    }
    else if (keybAdc < 3300)
    {
        retKey = KEY_SOURCE;        
    }
    else if (keybAdc < 3400)
    {
        retKey = KEY_OK;        
    }
    else if (keybAdc < 3450)
    {
        retKey = KEY_COLOR_VOLM;
    }
    else if (keybAdc < 3500)
    {
        retKey = KEY_COLOR_VOLP;
    }
    else if (keybAdc < 3530)
    {
        retKey = KEY_LEFT;
    }
    else if (keybAdc < ANALOG_KEY_MAX_ADC)
    {
        retKey = KEY_RIGHT;
    }
    else {
        // No key pressed
        hasReleasedKey = TRUE;
    }    
    
    /*
    if (keybAdc < ANALOG_KEY_MAX_ADC)
    {
        debugPrint("keyb: %u", keybAdc);
    }
    */
    
    if (retKey != 0 && hasReleasedKey)
    {        
        hasReleasedKey = FALSE;                                    
    }
    
    return retKey; 
}

BOOL readTempHumid(unsigned char* temperature, unsigned char* humidity, BOOL atBoot)
{
    unsigned char count = 0;
    BOOL isOK = FALSE;
    
    // WARNING: DO NOT LOG DEBUG WHILE READING DHT11 DUE TO SHARED PINS!
    
    // set DHT11 to input, which will let the chip rest for a while
    // it might have been "stunned" by irrelevant data previously meant for LCD and debugging
    DHT11_DIR = 1;
    
    // if at boot, provide longer recover time for DHT module
    // as well as more attempts
    unsigned char attempts = 1;
    unsigned int delayMs = 100;    
    if (atBoot)
    {
        attempts = 10;
        delayMs = 500;
    }
    
    delay_ms(delayMs);
    
    // enable pull-up
    DHT11_PULLUP = 1;
    
    // read over sever attempts
    for (count = 0; count < attempts; count++)
    {
        isOK = dht11_read(temperature, humidity);
        
        if (isOK)
            break;
        
        if (count < attempts - 1)
        {
            delay_ms(100);
        }
    }
    
    // disable pull-up, port back to output for LCD and debug
    DHT11_PULLUP = 1;
    DHT11_DIR = 0;
    
    // write result, debug can be used now
    debugPrint("R:%d,T:%d,H:%d", isOK, *temperature, *humidity);
    return isOK;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="I/O Initialization">
void setPullUpDownStatus(BOOL enabled)
{
    // internal pull-up on RB9 (SD SPI IN), otherwise there will be issues with Disk init at times
    // pull-up is to VDD - 0.8V, not VDD, but still useful. This eliminates the need for external 10K pullup.
    CNPUBbits.CNPUB9 = enabled;
    
    // internal pull-down on TFMS5400 module. High state means module connected
    CNPDBbits.CNPDB15 = enabled;
        
    // internal pull-up high on CH376 MISO pin
    CNPUBbits.CNPUB0 = enabled;                         
}

// if keypad is analog, we will be able to tell individual key presses by reading analog voltage input. Used when PIC is on for controls.
// if keypad is digital, we will only be able to tell whether it's pressed or not (and only for certain keys when generated voltage is low enough). Used when PIC is off to power it on.
void setKeypadAnalog(BOOL enabled)
{
    // SendUARTStr(enabled ? "an" : "di");
            
    // whether RB1 is analog input
    ANSELBbits.ANSB1 = enabled;
        
    // in digital mode (e.g. standby), enable interrupt on this pin to wake PIC up when certain keys are pressed
    // in analog mode, do not enable interrupt as we will handle via ADC instead
    IEC1bits.INT1IE = !enabled;	    
}

// Init IO Pins
void initIO()
{    
    // see config.h
    PLLFBD = MPLLFBD_HIGH;                	// M = PLLFBD + 2
    CLKDIVbits.PLLPOST = MPLLPOST;     	// N2 = 2
    CLKDIVbits.PLLPRE = MPLLPRE;  		// N1 = 2
    CLKDIVbits.FRCDIV = 0;
    CLKDIVbits.DOZE   = 0;
        
    // all digital, except RA0 (recorder), RA1/AN1 analog input (battery) and RB1/AN3 (keypad)
    ANSELA = 0b0000000000000011;   
    ANSELB = 0b0000000000000010;   
        
    // all output, except RB9 (SDI1), RB15 (TFMS), RA1 (analog input), RA4 (timer 1 clock)
    // I2C pins will be configured separately as needed by I2C software lib
    // We do not use hardware I2C (can't get it to work properly)
    TRISA = 0b0000000000010011;
    TRISB = 0b1000001000000011; 

    setPullUpDownStatus(TRUE);
    
    // UART for debugging    
    InitUART(TRUE);
    
    // interrupt 1 setup for push button
	RPINR0bits.INT1R = 33;	// RPI33, RB1
	INTCON2bits.INT1EP = 1;	// Falling (negative edge)
	IFS1bits.INT1IF = 0;	// clear flag
	IPC5bits.INT1IP = 0x03;	// priority level
    // IEC1bits.INT1IE = 1; // not enabled yet, will only be enabled later 
    
    // interrupt 2 setup for TFMS module
    RPINR1bits.INT2R = 47;	// RPI47, RB15
	INTCON2bits.INT2EP = 1;	// initially falling/negative edge (from high to low when button is pressed)
	IFS1bits.INT2IF = 0;	// clear flag
	IPC7bits.INT2IP = 0x04;	// priority level
	IEC1bits.INT2IE = 1;	// enable interrupt
        
    // no need to init SPI1 for Disk, it's fixed at pin 16/17/18
            
    // SPI2 for CH376S
    RPINR22bits.SDI2R = 32;             // SPI2 Data Input      (RPI32)
    RPOR4bits.RP43R   = 0b1000;         // SPI2 Data Output     (RP43)
    RPOR1bits.RP36R   = 0b1001;         // SPI2 Clock Output    (RP36)
    
    // disable PWM fault inputs, otherwise changing FLT32/RB4 will stop PWM 
    FCLCON1bits.FLTMOD = 0b11;      
    FCLCON2bits.FLTMOD = 0b11;      
    FCLCON3bits.FLTMOD = 0b11;      
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Power Management">
void setClockMode(unsigned char mode)
{
    __builtin_disi(0x3FFF);                    
    __builtin_write_OSCCONH(mode);                      
    __builtin_write_OSCCONL(0b001);                     
    __builtin_disi(0x0);            
 
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != mode);                     

    // Wait until clock switch is complete
    while (OSCCONbits.OSWEN);           
}

void setOperatingMode(unsigned char mode)
{
    if (mode == curOperatingMode)
    {
        return;
    }
    
    if (curOperatingMode != OP_MODE_STANDBY)
    {
        debugPrint("%u->%u", curOperatingMode, mode);
    }
    
    if (mode == OP_MODE_STANDBY)
    {
        setClockMode(CLOCK_MODE_LP);
    }
    else 
    {   
        if (curOperatingMode != OP_MODE_STANDBY)
        {
            setClockMode(CLOCK_MODE_LP);       
        }        
        PLLFBD = (mode == OP_MODE_FASTEST ? MPLLFBD_HIGH : MPLLFBD_LOW);       
        setClockMode(CLOCK_MODE_FASTRCPLL);
    }
    curOperatingMode = mode;        
         
    if (mode != OP_MODE_STANDBY)
    {                
        UpdateUART_BRG(mode == OP_MODE_FASTEST);   
        setTmr4Freq(BEEP_TMR_FREQ);
        // debugPrint("%u=%u", curOperatingMode, mode);
    }
}

void togglePeripherals(BOOL isEnabled)
{
    // wait for any pending UART transmission to finish before disabling UART
    if (!isEnabled)
        delay_ms(5);    
    EnableUART(isEnabled);
        
    enableAudio(isEnabled);
    
    T4CONbits.TON = isEnabled;          // Timer4, PCM Playback
    IEC1bits.T4IE = isEnabled;          // Timer4, Interrupt

    IEC1bits.INT2IE = isEnabled;        // Interrupt 2 TFMS    
    T3CONbits.TON = isEnabled;          // Timer3, TFMS Counter 
    
    SPI1STATbits.SPIEN = isEnabled;     // SPI1 - SD
    SPI2STATbits.SPIEN = isEnabled;     // SPI2 - CH375
    
    AD1CON1bits.ADON = isEnabled;       // ADC
}

// set output pins to circuit to low (0V), otherwise they will remain at 5V and leak into the VCC terminals of these circuit
// causing wasted power consumption even if the LT1763 has turned off the circuit when /SHDN is set to low
// many of these devices have a maximum voltage that can be applied to input pins when VCC is 0V, which is usually < 0.3V
// these pins will be turned on the next time these modules are used by the respective codes, hence we do not need to turn them on manually
void toggleCsLines(BOOL isHigh)
{
    // SD CS
    if (isHigh)
    {
        SD_CS_HIGH;
    }
    else {
        SD_CS_LOW;              
    }
    
    LATBbits.LATB4 = isHigh;     // CH376 SCK 
    CH376_CS = isHigh;           // CH376 CS
    LATBbits.LATB7 = isHigh;     // SD SCK
    LATBbits.LATB8 = isHigh;     // SD SDO
    LATBbits.LATB11 = isHigh;    // CH376 SDO
}

void enterStandbyMode(BOOL keepPower)
{
    SendUARTStr("OF");
                   
    // draw screen with clock first
    LCD_cls();
    drawFullScreenClock(TRUE);
    
    // sleep peripherals before powering off
    if (inputSource == SOURCE_USB)              // USB
    {
        ch376_reset();                          // Must reset first, otherwise sleep has no effect
        delay_ms(1);                            // Wait a bit before sleeping
        ch376_sleep();
        delay_ms(1);     
    }
    else if (inputSource >= SOURCE_RADIO) {     // including recording mode
        si473x_radioPowerDown();
    }
   
    // wait for any pending peripheral access to finish 
    delay_ms(100);               
    
    // disable all interrupts and peripherals, except RTC and power button interrupt
    togglePeripherals(FALSE);
    
    // digital keypad to power on when certain key are pressed (generate interrupts)
    setKeypadAnalog(FALSE);
            
    if (!keepPower)
    {
        // only set the CS line to low if peripherals power is removed
        // if peripherals power is still supplied, setting CS line to low will result in high current consumption
        toggleCsLines(FALSE);
                
        // disable pull-up to save power
        setPullUpDownStatus(FALSE);
    }
        
    // turn off power
    OUTPUT_PWR_ON = keepPower;    
               
    shouldExitStandby = FALSE;
    isInStandby = TRUE;
       
    // switch to low power RC oscillator 
    setOperatingMode(OP_MODE_STANDBY);
}

// PIC always seems to crash when switching from LPRC to FRCPLL
// We simply reset it instead and the clock will default to FRCPLL 
// (the time is kept in NVM memory! so we'll have nothing to lose)
void resetPIC()
{
    asm ("RESET");
}

void enterSleepMode()
{
    // sleep using ASM code 
    // .equiv SLEEP_MODE, 0x0000
    // .equiv IDLE_MODE, 0x0001
    asm("PWRSAV #0");
}

void getAlarmMenuStr(unsigned char enabled, unsigned char hr, unsigned char min, char str[])
{
    if (!isValidTime(hr, min, 0))
    {
        hr = 0;
        min = 0;
    }
    
    if (enabled)
    {
        sprintf(str, "Alarm: %02d:%02d", hr, min);
    }
    else {
        sprintf(str, "Alarm: Off");
    }
}

void getPowerTypeStr(unsigned char pwrType, char str[])
{
    switch (pwrType)
	{
		case POWER_TYPE_2_18650:
			strcpy(str, "2x18650");
			break;
		case POWER_TYPE_6_NIMH:
			strcpy(str, "6xNiMH");
			break;            
		case POWER_TYPE_6_ALKALINE:
			strcpy(str, "6xAlkaline");
			break;            
		case POWER_TYPE_9V_NIMH:
			strcpy(str, "9V NiMH");
			break;               
		case POWER_TYPE_9V_ALKALINE:
			strcpy(str, "9V Alkaline");
			break;     
		default:
			strcpy(str, "DC Adapter");
			break;            
	}
}

void getIdleOffStr(unsigned char item, char str[])
{
    switch (item)
    {
        case IDLE_OFF_15MIN:
            strcpy(str, "15min");
            break;             

        case IDLE_OFF_30MIN:
            strcpy(str, "30min");
            break;                             

        case IDLE_OFF_NEVER:
            strcpy(str, "Never");
            break;         

        default:
            strcpy(str, "5min");
            break;
    }
}

void getEditConfigStr(BOOL isOn, char str[])
{
    sprintf(str, "Edit Config: %s", isOn ? "On" : "Off");
}

void getAlwaysOffStr(unsigned char item, char str[])
{
    switch (item)
    {
        case ALWAYS_OFF_120MIN:
            strcpy(str, "2hr");
            break;                        

        case ALWAYS_OFF_180MIN:
            strcpy(str, "3hr");
            break;               

        case ALWAYS_OFF_240MIN:
            strcpy(str, "4hr");
            break;         

        case ALWAYS_OFF_NEVER:
            strcpy(str, "Never");
            break;

        default:
            strcpy(str, "1hr");
            break;
    }
}

void updatePowerSaveConfig()
{  
    switch (appConfigObj.config.idleOffMode)
    {
        case IDLE_OFF_5MIN:
            idleOffAfterSecs = 300;
            break;

        case IDLE_OFF_15MIN:
            idleOffAfterSecs = 900;
            break;             

        case IDLE_OFF_30MIN:
            idleOffAfterSecs = 1800;
            break;         

        case IDLE_OFF_NEVER:
            idleOffAfterSecs = 0xFFFF;
            break;         

        default:
            idleOffAfterSecs = 60;
            break;
    }

    switch (appConfigObj.config.alwaysOffMode)
    {
        case ALWAYS_OFF_120MIN:
            alwaysOffAfterSecs = 7200;
            break;

        case ALWAYS_OFF_180MIN:
            alwaysOffAfterSecs = 10800;
            break;

        case ALWAYS_OFF_240MIN:
            alwaysOffAfterSecs = 14400;
            break;

        case ALWAYS_OFF_NEVER:
            alwaysOffAfterSecs = 0xFFFF;
            break;

        default:
            alwaysOffAfterSecs = 3600;
            break;
    }
}

unsigned char getCurOperatingMode()
{
    return curOperatingMode;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Weather & Play Mode Icons">
#define WEATHER_ICON_COLOR          colLightSeaGreen
#define WEATHER_ICON_TEXT_COLOR     colWhite
#define PLAY_MODE_ICON_COLOR        colLightGrey
#define WEATHER_ICON_INNER_COLOR    colDarkRed

void displayWeatherInfo(BOOL onlyIfNeeded, BOOL refreshData)
{    
    if (refreshData)
    {
        unsigned char newTemp = 0;
        unsigned char newHumid = 0;
    
        // read new sensor values, try once only
        isTempSensorOK = readTempHumid(&newTemp, &newHumid, FALSE);            
        if (isTempSensorOK)
        {
            // if same values, do not draw if onlyIfNeeded is set
            if (newTemp == curTemperature)
            {
                if (onlyIfNeeded)
                {
                    return;
                }
            }
            
            curTemperature = newTemp;
            curHumidity = newHumid;
        }
    }

    // display temperature and humidity
    LCD_drawTopLeftInfo("      ");
    if (curTemperature != CUR_TEMP_HUMIDITY_INVALID)
    {
        sprintf(strResBuf, "%0d%c", curTemperature, 127);           
    }
    else {
        strcpy(strResBuf,  "   ");     
    }
    LCD_drawTopLeftInfo(strResBuf);
    
    if (curHumidity != CUR_TEMP_HUMIDITY_INVALID)
    {
        sprintf(strResBuf, "%0d%%", curHumidity);           
    }
    else {
        strcpy(strResBuf,  "   ");     
    }
    LCD_drawTopLeftInfo2(strResBuf);
}

void drawInputSource()
{       
    switch (inputSource)            
    {
        case SOURCE_SD:
        {
            LCD_drawBitmap(sdIcon, INPUT_SRC_POS_X, 0);
        }
        break;
        
        case SOURCE_USB:
        {
            LCD_drawBitmap(usbIcon, INPUT_SRC_POS_X, 0);
        }
        break;
        
        case SOURCE_RADIO_REC:
        {
            LCD_drawBitmap(radioRecIcon, INPUT_SRC_POS_X, 0);
        }
        break;
        
        default:
        {
         LCD_drawBitmap(radioNoRecIcon, INPUT_SRC_POS_X, 0);   
        }
        break;
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="UI Elements">
void getInputSourceStr(INPUT_SOURCE src, char str[])
{
    switch (src)
    {
        case SOURCE_SD:
            strcpy(str, "SD");
            break;
        case SOURCE_USB:
            strcpy(str, "USB");
            break;
        case SOURCE_RADIO_REC:
            strcpy(str, "REC");
            break;
        default:
            strcpy(str, "AUX");
            break;
    }    
}

void getDurationModeStr(unsigned char mode, char str[])
{
    if (mode == DURATION_MODE_ELAPSED)
    {
        strcpy(str, "Show: Elapsed");
    }
    else if (mode == DURATION_MODE_REMAINING)
    {
        strcpy(str, "Show: Remaining");
    }
    else 
    {
        strcpy(str, "Show: Total");
    }                              
}
    
void getPlayModeStr(unsigned char mode, char str[])
{
    switch (mode)
    {
        case PLAY_MODE_REPEAT_TRACK:
            strcpy(str, "Repeat Track");
            break;
        case PLAY_MODE_REPEAT_FOLDER:
            strcpy(str, "Repeat Folder");
            break;
        case PLAY_MODE_SHUFFLE_FOLDER:
            strcpy(str, "Shuffle Folder");
            break;
        case PLAY_MODE_SHUFFLE_ALL:
            strcpy(str, "Shuffle All");
            break;
        case PLAY_MODE_SINGLE:
            strcpy(str, "Single Track");
            break;
        default:
            strcpy(str, "All Tracks");
            break;
    }    
}

// Draw Full Screen Clock, in standby (31kHz) mode
void drawFullScreenClock(BOOL showAlarm)
{
    // get latest data every 5 minute only
    // but always redraw the display
    BOOL refreshData = (curMinute % 5 == 0);        
    displayWeatherInfo(FALSE, refreshData);    
    if (refreshData)
    {
        AD1CON1bits.ADON = 1;       // enable ADC, it is disabled in standby mode
    }
    drawBattery(refreshData);
    if (refreshData)
    {
        AD1CON1bits.ADON = 0;       // disable ADC again to save power
    }
    
    // firmware version
    sprintf(strResBuf, "v%s", APP_VERSION_STRING);
    LCD_drawTopMiddleInfo2(strResBuf);
    
    // clock time
    sprintf(strResBuf, "%02d:%02d", curHour, curMinute);
    LCD_drawExtraLargeString(20, 12, strResBuf,  1);
    
    // clock date
    unsigned char dow = getDayOfWeek(2000 + curYear2k, curMonth, curDay);    
    sprintf(strResBuf, "%s %02d-%02d-%02d", daysOfWeek[dow], curDay, curMonth, 2000 + curYear2k);
    LCD_drawString(22, 42, strResBuf,  1);
    
    // alarm clock
    if (showAlarm)
    {
        if (alarmEnabled)
        {
            LCD_drawBitmap(alarmIcon, 40, STATUS_LINE_Y);

            sprintf(strResBuf, "%02d:%02d", alarmHour, alarmMin);
            LCD_drawString(52, STATUS_LINE_Y, strResBuf,  1);
        }
        else {
            LCD_drawString(38, STATUS_LINE_Y, "Alarm Off",  1);
        }
        LCD_updateDisplay();
    }
    else {
        LCD_drawString(38, STATUS_LINE_Y, "         ",  1);
        
        // if update screen is done as part of the alarm process, we'll update the screen later
    }        
}

void drawFullScreenBitmap(__prog__ const unsigned char bmp[])
{
    LCD_cls();
    LCD_drawBitmap(bmp, (LCD_SCR_WIDTH - bmp[0]) / 2, (LCD_SCR_HEIGHT - bmp[1]) / 2);
    LCD_updateDisplay();
}

void uppercaseString(char str[])
{
    unsigned char len = strlen(str);
    unsigned char rd = 0;
    for (rd = 0; rd < len; rd++)
    {
        if (str[rd] >= 'a' && str[rd] <= 'z')
        {
            str[rd] = str[rd] - 32;
        }
        else {
            str[rd] = str[rd];
        }
    }
}
#define MARKER_SIZE1     4
#define MARKER_SIZE2     8

unsigned char lngToX(float lng, unsigned char markerSize)
{
    unsigned char myLocX = (unsigned char)( (lng + 180.0) * 128.0 * 1.05 / 360.0 );   // -180 .. 180, adjust for map non-linearity
    if (myLocX < markerSize)
    {
        myLocX = markerSize;
    }
    if (myLocX > LCD_SCR_WIDTH - markerSize)
    {
        myLocX = LCD_SCR_WIDTH - markerSize;
    }    
    return myLocX;
}

unsigned char latToY(float lat, unsigned char markerSize)
{
    unsigned char myLocY = (unsigned char)( (lat + 90.0) * 64.0 / 180.0 );   // (-90 .. 90)
    if (myLocY < markerSize)
    {
        myLocY = markerSize;
    }
    if (myLocY > LCD_SCR_HEIGHT - markerSize)
    {
        myLocY = LCD_SCR_HEIGHT - markerSize;
    }
    return LCD_SCR_HEIGHT - myLocY;
}

void findStnInfo()
{
#define STN_VALUE_INVALID       0xFFFF           
    // find the best match for station entry
    // based on distance and schedule and default the display to that entry
    unsigned int bestStnIndDist = STN_VALUE_INVALID;
    unsigned int bestStnIndDistSchedule = STN_VALUE_INVALID;
    unsigned int shortestStnDist = STN_VALUE_INVALID;
    unsigned int shortestStnDistSchedule = STN_VALUE_INVALID;
    unsigned int stInd = stnInfoEntryFirstIndex;
    for (stInd = stnInfoEntryFirstIndex; stInd <= stnInfoEntryLastIndex; stInd++)
    {                         
        stnInfoEntryCurIndex = stInd;
        fetchRadioInfoEntry();   

        if (stnDistanceKm < shortestStnDist)
        {
            shortestStnDist = stnDistanceKm;
            bestStnIndDist = stInd;
        }                

        if (stnIsBroadcasting)
        {
            if (stnDistanceKm < shortestStnDistSchedule)
            {
                shortestStnDistSchedule = stnDistanceKm;
                bestStnIndDistSchedule = stInd;
            }       
        }
    }

    // default to the closest station which is broadcasting
    // if no broadcasting stations, just default to the closest stations
    // otherwise (unlikely case), default to first entry
    if (bestStnIndDistSchedule != STN_VALUE_INVALID)
    {
        stnInfoEntryCurIndex = bestStnIndDistSchedule;
    }
    else if (bestStnIndDist != STN_VALUE_INVALID) {
        stnInfoEntryCurIndex = bestStnIndDist;
    }
    else {
        stnInfoEntryCurIndex = stnInfoEntryFirstIndex;
    }    
}

void drawStnInfo()
{
    char buf[64];
        
    isShowingHelp = TRUE;
    
    // usually isRemotePressed is reset at the end of the main routine, but since drawHelp() is blocking
    // and we want to access the remote again, we must reset it here,
    resetTFMSVars();
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
        
    // station information
    LCD_cls();       
    
    // station name
    strcpy(strResBuf, "");
    swGetArrayElementName(stnNameInd - 1, SW_ARRAY_STATIONS, strResBuf);
    uppercaseString(strResBuf);
    LCD_showBootInfo(0, 0, strResBuf);
    
    // site name, store in fullAudioFilename to save space
    strcpy(buf, fullAudioFilename);
    uppercaseString(buf);
    LCD_showBootInfo(1, 0, buf);
    
    sprintf(strResBuf, "FREQ: %u kHz", radioCurFreq);
    LCD_showBootInfo2(2, 0, strResBuf);
    
    BOOL hasStnLoc = FALSE;
    if (stnLat1 > -90 && stnLat1 < 90 && stnLng1 > -180 && stnLng1 < 180)
    {
        sprintf(strResBuf, "LOC : (%d.%02d, %d.%02d)", stnLat1, stnLat2, stnLng1, stnLng2);
        hasStnLoc = TRUE;
    }
    else {
        strcpy(strResBuf,  "LOC : -");
    }
    LCD_showBootInfo2(3, 0, strResBuf);
    
    if (stnDistanceKm < 5)
    {
        strcpy(strResBuf,  "DIST: Nearby");
    }
    else if (stnDistanceKm != STN_DISTANCE_INVALID)
    {
        sprintf(strResBuf, "DIST: %uKM %s", stnDistanceKm, stnDirection);
    }
    else {
        strcpy(strResBuf,  "DIST: -");
    }
    LCD_showBootInfo2(4, 0, strResBuf);
    
    unsigned char c;
    for (c = 0; c<7; c++)
    {
        unsigned char isOn = (stnOnDays >> c) & 1;
        if (isOn)
        {
            buf[c] = 49 + c;
        }
        else {
            buf[c] = '-';
        }
    }
    buf[7] = 0;
    sprintf(strResBuf,  "DAYS: %s (SUN = 1)", buf);  // 1234567 (1=Sunday)
    LCD_showBootInfo2(5, 0, strResBuf);
    
    sprintf(strResBuf,  "TIME: %04d-%04d UTC", stnStartTime, stnEndTime);
    LCD_showBootInfo2(6, 0, strResBuf);
    
    BOOL isAzND = FALSE;
    if (stnAzimuth > 360)
    {
        // uni-directional
        strcpy(buf, "ND");    
        isAzND = TRUE;
    }
    else {
        sprintf(buf, "%u%c", stnAzimuth, 127);
    }
    sprintf(strResBuf,  "PWR : %u kW (%s)", stnPower, buf);
    LCD_showBootInfo2(7, 0, strResBuf);
    
    unsigned char ctyInd = stnCtyLang >> 8;
    BOOL hasCty = FALSE;
    if (ctyInd != 0xFF)
    {
        swGetArrayElementName(ctyInd - 1, SW_ARRAY_COUNTRIES, buf);
        uppercaseString(buf);
        if (strlen(buf) > 3)
        {
            // workaround some invalid records, e.g. country = TKS
            hasCty = TRUE;   
        }
        
    }

    unsigned char lineCount = 8;
    if (hasCty)
    {
        sprintf(strResBuf, "CTY : %s", buf);   
        LCD_showBootInfo2(lineCount, 0, strResBuf);        
        lineCount++;
    }
    
    unsigned char langInd = stnCtyLang & 0xFF;
    if (langInd != 0xFF)
    {
        swGetArrayElementName(langInd - 1, SW_ARRAY_LANGUAGES, buf);
        uppercaseString(buf);
        sprintf(strResBuf, "LANG: %s", buf);
        LCD_showBootInfo2(lineCount, 0, strResBuf);    
        lineCount++;
    }
    LCD_updateDisplay();
        
    // wait until remote or button is pressed
    resetTFMSVars();
    isTFMSInt = FALSE;
    while (!isTFMSInt && !isAnalogKeyPressed());               
    delay_ms(200);      // debouncing
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
    
    if (hasStnLoc)
    {
        // draw station and current location on world map
        LCD_cls();

        // world map
        LCD_drawBitmap(worldMapIcon, 0, 0);

        // current location, convert to (0 .. 128), assuming linear
        float myLng = curMyLng1 * 1.0 + curMyLng2 / 100.0;
        unsigned char myLocX = lngToX(myLng, MARKER_SIZE1);
        float myLat = curMyLat1 * 1.0 + curMyLat2 / 100.0;
        unsigned char myLocY = latToY(myLat, MARKER_SIZE1);
        LCD_fillRect(myLocX, myLocY, 4, 4);

        // station location
        float stnLng = stnLng1 * 1.0 + stnLng2 / 1000.0;
        unsigned char stnLocX = lngToX(stnLng, MARKER_SIZE2);
        float stnLat = stnLat1 * 1.0 + stnLat2 / 1000.0;
        unsigned char stnLocY = latToY(stnLat, MARKER_SIZE2);
        // debugPrint("az=%u", stnAzimuth);
        if (isAzND)
        {
            // unidirectional broadcast, draw circle
            LCD_fillCircle(stnLocX, stnLocY, 2);
        }
        else 
        {            
            // draw approximate station bearing, azimuth is degrees clock-wise from north
            if (stnAzimuth > 22 && stnAzimuth < 45)   
            {
                LCD_drawBitmap(az045Icon, stnLocX, stnLocX);
            }
            else if (stnAzimuth < 90)
            {
                LCD_drawBitmap(az090Icon, stnLocX, stnLocY);
            }
            else if (stnAzimuth < 135)
            {
                LCD_drawBitmap(az135Icon, stnLocX, stnLocY);
            }
            else if (stnAzimuth < 180)
            {
                SendUARTStr("h2");
                LCD_drawBitmap(az180Icon, stnLocX, stnLocY);
            }
            else if (stnAzimuth < 225)
            {
                LCD_drawBitmap(az225Icon, stnLocX, stnLocY);
            }
            else if (stnAzimuth < 270)
            {
                LCD_drawBitmap(az270Icon, stnLocX, stnLocY);
            }
            else
            {
                LCD_drawBitmap(az000Icon, stnLocX, stnLocY);
            }
            
            // station location
            // LCD_fillRect(stnLocX+2, stnLocY, 3, 3);
        }
        LCD_updateDisplay();

        // wait until key press again
        resetTFMSVars();
        isTFMSInt = FALSE;
        while (!isTFMSInt && !isAnalogKeyPressed());               
        delay_ms(200);      // debouncing    
        isTFMSInt = FALSE;
        lastRemoteChar = KEY_INVALID;
    }
    
    isShowingHelp = FALSE;          
           
    // in radio mode, simply update the screen
    LCD_cls();    
    remoteDigitsPressedCount = 0;
    drawRemoteInput();
    shouldDrawClock = TRUE;
    shouldUpdateRadioScreen = TRUE;
    updateRadioScreen();
}

void drawHelp()
{   
    enableAudio(FALSE);
    isShowingHelp = TRUE;
    
    // usually isRemotePressed is reset at the end of the main routine, but since drawHelp() is blocking
    // and we want to access the remote again, we must reset it here,
    resetTFMSVars();
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
        
    // helpMenuMedia and helpMenuRadio is in __prog__ program memory space
    // normal string operation cannot be used. We must use _strncpy_p2d16
    // Example: https://www.microchip.com/forums/m575468.aspx
    // Additionally this macro cannot work with string array pointer
    // We resort to storing the lines as a single string, with each line having a fixed length, for easier access
    LCD_cls();       
    unsigned char hLine = 0;
    _prog_addressT addr;
    if (inputSource < SOURCE_RADIO)
    {
        _init_prog_address(addr, helpMenuMedia);
    }
    else {
        _init_prog_address(addr, helpMenuRadio);
    }    
    for (hLine = 0; hLine < MAX_HELP_MENU_LINES; hLine++)
    {
        strcpy(strResBuf, "");
        _strncpy_p2d16(strResBuf, addr, HELP_LINE_LEN);        
        addr += HELP_LINE_LEN;
        strResBuf[HELP_LINE_LEN] = 0;        
        SendUARTStr(strResBuf);        
        LCD_showBootInfo(hLine, 0, strResBuf);
    }    
    LCD_updateDisplay();
        
    // wait until remote or button is pressed
    resetTFMSVars();
    isTFMSInt = FALSE;
    while (!isTFMSInt && !isAnalogKeyPressed());               
    delay_ms(200);      // debouncing
    
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
    
    enableAudio(TRUE);   
    isShowingHelp = FALSE;          
        
    if (inputSource < SOURCE_RADIO)
    {
        // in media mode, play next file to force update screen
        processRemoteKey(KEY_RIGHT);       
    }
    else {
        LCD_cls();
        
        // in radio mode, simply update the screen
        remoteDigitsPressedCount = 0;
        drawRemoteInput();
        shouldDrawClock = TRUE;
        shouldUpdateRadioScreen = TRUE;
        updateRadioScreen();
    }
}

void resetMenuLine()
{
    LCD_drawMenuText(EMPTY_MENU_LINE);
    LCD_drawRecDuration2("        ");
    
    if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_SW)
    {
        LCD_clearRect(CHECKMARK_LOC_X, CHECKMARK_LOC_Y, CHECKMARK_WIDTH, CHECKMARK_HEIGHT);
    }
}

void drawSongTime()
{
    if (totalDuration > 0)
    {
        BOOL isRemaining = (curDurationMode == DURATION_MODE_REMAINING);
        unsigned int dur = 0; 
        unsigned char durChar;
        if (isRemaining)
        {
            dur = (totalDuration - prevDuration);
            durChar = '-';
        }
        else if (curDurationMode == DURATION_MODE_ELAPSED && prevDuration > 0)
        {
            dur = prevDuration + 1; // timer starts late after song plays, so offset by 1
            durChar = ' ';
        }
        else
        {
            dur = totalDuration;
            durChar = '*';
        }

        if (isVBRFile && curDurationMode != DURATION_MODE_ELAPSED)
        {
            // in remaining or total duration, do not display if file is VBR (variable bit rate)
            strcpy(strResBuf, "   VBR");
        }
        else {
            // otherwise normal duration display
            unsigned char hour = dur / 3600;
            unsigned char min  = (dur - hour * 3600) / 60;
            unsigned char sec  = (dur - hour * 3600 - min * 60);

            // if > 60min, omit second display
            if (hour > 0)
            {
                sprintf(strResBuf, "%c%02d:%02d", durChar, hour, min);
            }
            else {
                sprintf(strResBuf, "%c%02d:%02d", durChar, min, sec);
            }
        }
        
        LCD_drawRightStatusText(strResBuf);         
    }
}

void drawTrackInfo(BOOL atLastLine)
{
   // draw count only if file isPlaying;
   unsigned int tempIndex = curFileIndex + 1;
   if (numOfFilesInFolder == 0)
       tempIndex = 0;

   // take the last 3 digits only
   // numOfFoldersInRoot does not count root folder, so we +1
   sprintf(strResBuf, "T%u/%u F%u/%u", tempIndex % 10000, numOfFilesInFolder, (curFolderIndex+1) % 10000, numOfFoldersInRoot + 1);

   if (atLastLine)
   {
        LCD_drawStatusText(EMPTY_STATUS_LINE);
        LCD_drawStatusText2(strResBuf, 0, MIN(strlen(strResBuf), MAX_TITLE_CHAR_SMALL));        
   }
   else {
        LCD_drawMenuText(EMPTY_STATUS_LINE);
        LCD_drawMenuText2(strResBuf, 0, MIN(strlen(strResBuf), MAX_TITLE_CHAR_SMALL));           
   }
}

void drawStatusBar()
{
    char buf1[8];
    char buf2[8];
    
    LCD_clearRect(0, STATUS_LINE_Y, LCD_SCR_WIDTH, LCD_SCR_WIDTH -  STATUS_LINE_Y);
    
    if (inputSource < SOURCE_RADIO)
    {
        // Play Icon
        BOOL shouldDrawPause = (isPaused && isFilePlaying);
        /*
        unsigned char pct = (totalDuration > 0 ? ((unsigned char)((prevDuration * 100.0) / totalDuration)) : 0);
        if (pct > 0)
        {
            sprintf(strResBuf, "Paused (%d%%)", pct);        
        }
        else 
        {
            strcpy(strResBuf, "Paused");        
        }
        */
        if (isPaused)
        {
            LCD_drawBitmap(shouldDrawPause ? pauseIcon : stopIcon, 0, STATUS_LINE_Y);                
        }
        else {
            LCD_drawBitmap(playIcon, 0, STATUS_LINE_Y);                
        }
                
        // Force Mono Icon        
        if (forceMono)
        {
            LCD_drawBitmap(monoIcon, 15, STATUS_LINE_Y);    
        }          
        
        // Duration
        drawSongTime();      
                    
        // Track info 
        sprintf(strResBuf, "T%d F%d", (curFileIndex+1) % 10000, (curFolderIndex+1) % 10000);
        LCD_drawTrackInfo(strResBuf);
    }
    else {
        // Play / Recording Icon
        if (!isRadioSeeking)
        {
            if (radioRecordingMode != RECORD_MODE_NONE)
            {
                // strcpy(strResBuf, radioRecordingMode == RECORD_MODE_SD ? "SD Rec" : "USB Rec");
                LCD_drawBitmap(recordingIcon, 0, STATUS_LINE_Y);
            }
            else
            {
                LCD_drawBitmap(isPaused ? pauseIcon : playIcon, 0, STATUS_LINE_Y);    
            }
            
            // radio normal mode
            
            // Stereo or Mono Icon
            if (!isPaused)
            {
                si473x_rqs_status rqs = si473x_getCurrentReceivedSignalQuality(0);        
                BOOL isRadStereo = FALSE;
                if (radioCurBand == RADIO_CUR_BAND_FM)
                {
                    isRadStereo = !forceMono && rqs.resp.PILOT;
                }
                LCD_drawBitmap( isRadStereo ? stereoIcon : monoIcon, 12, STATUS_LINE_Y);

                // Signal icon
                // Icon starts at SIGNAL_ICON_X, first 2 bars are always there
                // 1 bar consumes 2 columns (with space)
                // We draw the full signal bars first, then clear out the unused bars
        #define SIGNAL_ICON_X        31
                unsigned char signalBars = 0;
                LCD_drawBitmap(signalIcon, SIGNAL_ICON_X, STATUS_LINE_Y);
                if (rqs.resp.SNR > 30)
                {
                    signalBars = 5;
                }
                else if (rqs.resp.SNR > 20)
                {
                    signalBars = 4;
                }
                else if (rqs.resp.SNR > 16)
                {
                    signalBars = 3;
                }
                else if (rqs.resp.SNR > 12)
                {
                    signalBars = 2;
                }
                else if (rqs.resp.SNR > 4)
                {
                    signalBars = 1;
                }
                LCD_clearRect(SIGNAL_ICON_X + 4 + signalBars * 2, STATUS_LINE_Y, 10, 8);

                // Fine-tune status
                if (isRadioFineTune)
                {
                    LCD_drawBitmap(fineTuneIcon, 47, STATUS_LINE_Y);
                }

                // SSB / Sync mode
                getRadioBandStr(radioCurBand, buf2);                
                if (radioSSBMode != SSB_NONE)
                {
                    getRadioSSBModeStr(radioSSBMode, buf1);                
                    if (radioSSBSync)
                    {
                        strcpy(strResBuf, (radioSSBMode == LSB_MODE ? "SNL" : "SNU"));
                    }
                    else {
                        // append space to SSB or USB
                        strcpy(strResBuf, buf1);
                    }            
                    LCD_drawString(73, STATUS_LINE_Y, strResBuf, 1);
                }

                // FM Stereo or Signal Info
                if (isRadStereo)
                {
                    sprintf(strResBuf, "%02u/%02u/%02u", rqs.resp.SNR, rqs.resp.RSSI,rqs.resp.STBLEND);        
                }
                else {
                    sprintf(strResBuf, "%02u/%02u", rqs.resp.SNR, rqs.resp.RSSI);        
                }
                LCD_drawString(isRadStereo ? 80 : 98, STATUS_LINE_Y, strResBuf, 1);
            }
        }
    }   
}

void drawUIMessage(char msg[])
{                
    if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_SW)
    {
        LCD_clearRect(CHECKMARK_LOC_X, CHECKMARK_LOC_Y, CHECKMARK_WIDTH, CHECKMARK_HEIGHT);
    }
    
    LCD_drawMenuText(EMPTY_MENU_LINE);
    LCD_drawMenuText2(msg, 0, MIN(strlen(msg), MAX_TITLE_CHAR_SMALL));        
    LCD_updateDisplay();
        
    shouldUpdateStatus = TRUE;
}

void drawVolumeTitle() 
{    
    unsigned char volPct = volumeLevel * 100 / MAX_VOL_LVL;
    sprintf(strResBuf, "VOL: %d%%", volPct);
    SendUARTStr(strResBuf);
    
    drawUIMessage(strResBuf);        
}

void drawBattery(BOOL updateReadings)
{   
#define     BATT_WIDTH      20
#define     BATT_HEIGHT     8    
    
    if (radioRecordingMode != RECORD_MODE_NONE)
        return;
    
    if (updateReadings)
    {
        unsigned int rawBattVal = 0;
        float vin = 0;
        battPct = readBattLevel(&rawBattVal, &vin, &isBattError);
        formatBatteryVoltage(rawBattVal, vin, battPct, FALSE, isBattError);
        SendUARTStr(strResBuf);
    }

    LCD_clearRect(BATT_ICON_POS_X, 0, BATT_WIDTH, BATT_HEIGHT);    
    
    if (curPowerType != POWER_TYPE_DC)
    {       
        if (isBattError)
        {
            LCD_drawBitmap(batteryInvalidIcon, BATT_ICON_POS_X, 0);
        }
        else if (battPct > 90) {
            LCD_drawBitmap(battery100Icon, BATT_ICON_POS_X, 0);
        }
        else if (battPct > 60) {
            LCD_drawBitmap(battery66Icon, BATT_ICON_POS_X, 0);
        }
        else if (battPct > 30) {
            LCD_drawBitmap(battery33Icon, BATT_ICON_POS_X, 0);
        }
        else {
            LCD_drawBitmap(batteryEmptyIcon, BATT_ICON_POS_X, 0);
        }
    }
}

void drawTopClock()
{   
    // RTC clock (both radio and media mode)
    if (isRTCOK)
    {
        sprintf(strResBuf, "%02d:%02d", curHour, curMinute);
    }
    else {
        strcpy(strResBuf, "--:--");
    }
    LCD_drawTopMiddleInfo(strResBuf);
    
    // alarm clock icon
    if (alarmEnabled)
    {
        LCD_drawBitmap(alarmIcon, ALARM_ICON_POS_X, 0);
    }
    else {
        LCD_drawBitmap(alarmEmptyIcon, ALARM_ICON_POS_X, 0);
    }
}

void drawFolderName()
{        
    resetMenuLine();
    
    // draw folder name as info line, remove old line first
    LCD_drawMenuText2(curFolderIndex == 0 ? label : curFolderName, 0, 
                            MIN(curFolderIndex == 0 ? strlen(label) : strlen(curFolderName), 
                            MAX_TITLE_CHAR_SMALL            
            ));    
}

void drawTitle()
{
    // title display
    BOOL isUsingID3 = FALSE;
    if (isPlayingMP3)
        isUsingID3 = strlen(mp3info.title) > strlen(shortAudioFilename);
    
    unsigned char titleLength = 0;
    if (isUsingID3)
    {
        titleLength = strlen(mp3info.title);
    }
    else {
        titleLength = strlen(shortAudioFilename);
        unsigned char lastIndex = 0;
            
        // do not display file extension
        for (lastIndex = titleLength - 1; lastIndex >= 0; lastIndex--)
        {
            if (shortAudioFilename[lastIndex] == '.')
            {
                titleLength = lastIndex;
                break;
            }
        }
    }
    
    if (titleLength < MAX_TITLE_CHAR_SCROLL)
    {        
        // short title, update once only
        LCD_drawItemTitle2(isUsingID3 ? mp3info.title : shortAudioFilename, 0, titleLength); 
        shouldUpdateTitle = FALSE;
    }
    else {
        // long title, update every time
        unsigned char len = MIN(titleLength, MAX_TITLE_CHAR_SCROLL);
        LCD_drawItemTitle2(isUsingID3 ? mp3info.title : shortAudioFilename, curTitlePos, len);                 
        curTitlePos++;

        if (curTitlePos + len >= titleLength)
        {
            curTitlePos = 0;
        }
    }
}
    
void drawRemoteInput() {
    unsigned char c;
    
    strcpy(strResBuf, "");            // default values
    if (remoteDigitsPressedCount > 0)
    {
        LCD_drawTopLeftInfo2("   ");
        // draw input
        for (c=0; c<remoteDigitsPressedCount; c++)
        {
            sprintf(strResBuf, "%s%c", strResBuf, remoteDigitsPressed[c]);
        }
        for (c=remoteDigitsPressedCount; c<MAX_REMOTE_DIGITS_PRESSED; c++)
        {
            sprintf(strResBuf, "%s.", strResBuf);
        }   
        
        // space on the right
        sprintf(strResBuf, "%s ", strResBuf);
        
        SendUARTStr(strResBuf);
        LCD_drawTopLeftInfo(strResBuf);
    }
    else {
        displayWeatherInfo(TRUE, FALSE);    
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Radio Helper">
void radioSetAmOrSsbMode(BOOL pwrDown)
{
    if (pwrDown)
    {
        si473x_radioPowerDown();        
    }
    if (radioSSBMode != SSB_NONE)
    {
        // download SSB patch to non volatile memory
        SendUARTStr("SSBS");
        si473x_loadPatch(ssbPatch, sizeof(ssbPatch), radioSSBBW); 
        SendUARTStr("SSBE");
          
        si473x_setSSBMode(radioSSBMode);
    }
    else {
        // normal power on
        SendUARTStr("SSBOf");
        si473x_radioPowerUp(POWER_UP_AM);
    }
}

unsigned char getRadioAMSpacing(unsigned char option)
{
    return (option == RADIO_MW_SPACING_9kHZ ? RADIO_MW_SPACING_9kHZ_VAL : RADIO_MW_SPACING_10kHZ_VAL);
}

unsigned char getRadioFMSpacing(unsigned char option)
{
    if (option == RADIO_FM_SPACING_100kHZ)
    {
        return RADIO_FM_SPACING_100kHZ_VAL;
    }
    else if (option == RADIO_FM_SPACING_200kHZ)
    {
        return RADIO_FM_SPACING_200kHZ_VAL;
    }
    else {
        return RADIO_FM_SPACING_50kHZ_VAL;
    }
}

void getRadioBandStr(unsigned char band, char buf[])
{
    if (band == RADIO_CUR_BAND_MW)
    {
        strcpy(buf, "AM");
    }
    else if (band == RADIO_CUR_BAND_FM)
    {
        strcpy(buf, "FM");
    }
    else if (band == RADIO_CUR_BAND_SW)
    {
        strcpy(buf, "SW");
    }
    else 
    {
        strcpy(buf, "LW");
    }
}

void getRadioAMBWStr(unsigned char val, char buf[])
{
    if (val == RADIO_AM_BW_6kHz)
        strcpy(buf, RADIO_AM_BW_6kHz_VAL);    
    else if (val == RADIO_AM_BW_4kHz)
        strcpy(buf, RADIO_AM_BW_4kHz_VAL);
    else if (val == RADIO_AM_BW_3kHz)
        strcpy(buf, RADIO_AM_BW_3kHz_VAL);    
    else if (val == RADIO_AM_BW_2kHz)
        strcpy(buf, RADIO_AM_BW_2kHz_VAL);
    else if (val == RADIO_AM_BW_1kHz)
        strcpy(buf, RADIO_AM_BW_1kHz_VAL);    
    else if (val == RADIO_AM_BW_1P8kHz)
        strcpy(buf, RADIO_AM_BW_1P8kHz_VAL);        
    else if (val == RADIO_AM_BW_2P5kHz)
        strcpy(buf, RADIO_AM_BW_2P5kHz_VAL);            
}

void getRadioSSBBWStr(unsigned char val, char buf[])
{
    if (val == RADIO_SSB_BW_1P2kHz)
        strcpy(buf, RADIO_SSB_BW_1P2kHz_VAL);
    else if (val == RADIO_SSB_BW_2P2kHz)
        strcpy(buf, RADIO_SSB_BW_2P2kHz_VAL);
    else if (val == RADIO_SSB_BW_3kHz)
        strcpy(buf, RADIO_SSB_BW_3kHz_VAL);    
    else if (val == RADIO_SSB_BW_4kHz)
        strcpy(buf, RADIO_SSB_BW_4kHz_VAL);        
    else if (val == RADIO_SSB_BW_500Hz)
        strcpy(buf, RADIO_SSB_BW_500Hz_VAL);            
}

void getRadioSSBModeStr(unsigned char val, char buf[])
{
    if (val == LSB_MODE)
        strcpy(buf, "LSB");
    else if (val == USB_MODE)
        strcpy(buf, "USB");
    else
        strcpy(buf, "");
}

void getFmRangeMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    
    unsigned int fmMin = RADIO_FM_MIN_87MHz;
    if (radioFMRange == RADIO_FM_RANGE_64MHz)
    {
        fmMin = RADIO_FM_MIN_64MHz;
    }
    else if (radioFMRange == RADIO_FM_RANGE_76MHz)
    {
        fmMin = RADIO_FM_MIN_76MHz;
    }
    sprintf(buf, "%sFM: %u-%u MHz", tmp, fmMin / 100, RADIO_FM_MAX_MHz / 100);
}

void getFmDeempMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    
    sprintf(buf, "%sDe-emp: %uuS", tmp, radioFMDeemp == RADIO_FM_DEEMPH_75US ? RADIO_FM_DEEMPH_75US_VAL : RADIO_FM_DEEMPH_50US_VAL);
}

void getFmSpacingMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    
    unsigned char space = getRadioFMSpacing(radioFMSpacing);
	sprintf(buf, "%sStep: %ukHz", tmp, space * 10);
}

void getAmSpacingMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");    
    sprintf(buf, "%sStep: %ukHz", tmp, getRadioAMSpacing(radioAMSpacing));
}

void getAmBwMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    char tmp2[8];
        
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");    
    getRadioAMBWStr(radioAMBW, tmp2);
	sprintf(buf, "%sAM BW: %skHz", tmp, tmp2);
}

void getSsbBwMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    char tmp2[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    getRadioSSBBWStr(radioSSBBW, tmp2);
	sprintf(buf, "%sSSB BW: %skHz", tmp, tmp2);
}

void getSsbModeMenuStr(char buf[], BOOL withCursor)
{
    char tmp[8];
    char tmp2[8];
    
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    getRadioSSBModeStr(radioSSBMode, tmp2);
	if (strlen(tmp2) == 0)
		strcpy(tmp2, "None");
	
	sprintf(buf, "%sSSB: %s", tmp, tmp2);
}

void getSsbSyncMenu(char buf[], BOOL withCursor)
{
    char tmp[8];
    strcpy(tmp, withCursor ? MENU_CURSOR_ARROW : "");
    sprintf(strResBuf, "%sSync: %s", tmp, radioSSBSync ? "On" : "Off");
}

void updateRecWavHeader(UINT32 recWords)
{
    // 8kHz, 16-bit mono WAV header
    memset(&wavHeader, 0, sizeof(wavHeader));
            
    wavHeader.riff[0] = 'R';
    wavHeader.riff[1] = 'I';
    wavHeader.riff[2] = 'F';
    wavHeader.riff[3] = 'F';

    wavHeader.wave[0] = 'W';
    wavHeader.wave[1] = 'A';
    wavHeader.wave[2] = 'V';
    wavHeader.wave[3] = 'E';

    wavHeader.fmt_chunk_marker[0] = 'f';
    wavHeader.fmt_chunk_marker[1] = 'm';
    wavHeader.fmt_chunk_marker[2] = 't';
    wavHeader.fmt_chunk_marker[3] = ' ';

    wavHeader.length_of_fmt = 16;               // length of format data as listed above
    wavHeader.format_type = 1;                  // PCM
    wavHeader.channels = 1;                     // Mono
    wavHeader.sample_rate = RECORDING_FREQ;     // Sampling rate
    wavHeader.byterate = RECORDING_FREQ * 2;    // (Sample Rate * BitsPerSample * Channels) / 8
    wavHeader.block_align = 2;                  // NumChannels * BitsPerSample/8
    wavHeader.bits_per_sample = 16;              

    wavHeader.data_chunk_header[0] = 'd';
    wavHeader.data_chunk_header[1] = 'a';
    wavHeader.data_chunk_header[2] = 't';
    wavHeader.data_chunk_header[3] = 'a';

    wavHeader.data_size = recWords * 2;
        
    wavHeader.overall_size = 4 + (8 + wavHeader.length_of_fmt) + (8 + wavHeader.data_size);
}

void startRadioRecording(unsigned char recMode) {    
    // FATFS preparation
    char buf[16];
    getRadioBandStr(radioCurBand, buf);
        
    totalRecWords = 0;
    recAvgVal = 0;
    recAvgValCount = 0;
    lastRecDurationUpdate = 0;

    drawMenuInfo(LOADING_MENU_LINE);
    LCD_updateDisplay();
    
    // enable SPI module for USB or SD card
    if (recMode == RECORD_MODE_USB)
    {
        setFatFsIsUsb(TRUE);
        SPI2STATbits.SPIEN = 1;
        ch376_clearvars();                        
    }
    else {
        setFatFsIsUsb(FALSE);
        SPI1STATbits.SPIEN = 1;
    }    
    resetFatFS();   
    
    BOOL isStarted = FALSE;
    BOOL isOK = initFatFS();
    if (isOK)
    {
        initRand();
        
        unsigned int randN = getRandom(0xFFFF);
        if (isPaused)
        {
            sprintf(shortAudioFilename, "AUX_%04X.WAV", randN);
        }
        else
        {
            sprintf(shortAudioFilename, "%s%u_%04X.WAV", buf, radioCurFreq, randN);
        }
        
        // create recording sub dir, do not care about error (which most like happens if dir already exists)
        FRESULT fWRes1 = f_mkdir(RECORDING_DIR_NAME);
        if (fWRes1 != FR_OK)
        {
            SendUARTStr("mdEr");            
        }
        else {
            SendUARTStr("mdOk");            
        }

        // open file in sub dir and check for error
        sprintf(strResBuf, "%s/%s", RECORDING_DIR_NAME, shortAudioFilename);
        FRESULT fWRes = f_open(&fMediaFile, strResBuf, FA_WRITE | FA_CREATE_ALWAYS);
        SendUARTStr(strResBuf);
                
        if (fWRes == FR_OK)
        {     
            // default header first, size will be updated later
            updateRecWavHeader(0);                       
            UINT bytesWritten = 0;    
            fWRes = f_write(&fMediaFile, &wavHeader, sizeof(wavHeader), &bytesWritten);
            if (fWRes != FR_OK || bytesWritten != sizeof(wavHeader))
            {
                debugPrint("rHdEr:%u", fWRes);
                
                fWRes = f_close(&fMediaFile);
                if (fWRes != FR_OK)
                {
                    debugPrint("rHdClEr:%u", fWRes);
                }
            }
            else {
                isStarted = TRUE;
            }
        }
        else {
            debugPrint("rOpEr:%u", fWRes);  
        }
    }
    
    // record mode setup
    if (isStarted)
    {
        // wait for any beep to finish
        delay_ms(100);
        
        // disable audio, otherwise PWM will causes high frequency noises on the recording
        enableAudio(FALSE);
        
        // clear any unwritten data from the previous recording queue
        // at most 1 second from the stop action (not important to write down)
        queue_clear_left();
       
        // file name displayed
        drawMenuInfo2(shortAudioFilename, strlen(shortAudioFilename) - 4); // remove extensions
        radioRecordingMode = recMode;        
        
        // digital keypad, any key to interrupt recording
        setKeypadAnalog(FALSE);   
    }
    else    
    {        
        drawMenuInfo("File Err");
        
        if (recMode == RECORD_MODE_USB)
        {
            // disable CH376        
            ch376_reset();                      // Must reset first, otherwise sleep has no effect
            delay_ms(1);                        // Wait a bit before sleeping
            ch376_sleep();
            delay_ms(1);                        // Wait until all SPI transmission (hardware) is done before disabling SPI
            SPI2STATbits.SPIEN = 0;
        }
        else {
            // disable SD card SPI module
            SPI1STATbits.SPIEN = 0;
        }        
    }
    
    LCD_updateDisplay();
}

void stopAnyCurRadioRecording() {
    if (inputSource < SOURCE_RADIO || radioRecordingMode == RECORD_MODE_NONE)
        return;

    BOOL isOK = TRUE;
        
    drawMenuInfo("Saving ...");
    LCD_updateDisplay();
    
    // seek to beginning of file and update header with correct length of the recording
    updateRecWavHeader(totalRecWords);
    FRESULT fWRes;
    fWRes = f_lseek(&fMediaFile, 0);    
    if (fWRes == FR_OK)
    {
        UINT bytesWritten = 0;    
        fWRes = f_write(&fMediaFile, &wavHeader, sizeof(wavHeader), &bytesWritten);
        if (fWRes != FR_OK || bytesWritten != sizeof(wavHeader))
        {
            debugPrint("rHdWrEr:%u", fWRes);
            isOK = FALSE;
        }
    }
    else {
        debugPrint("rHdSkEr:%u", fWRes);
        isOK = FALSE;
    }
    
    // close file regardless of whether header update was OK
    fWRes = f_close(&fMediaFile);
    if (fWRes != FR_OK)
    {
        debugPrint("rClEr:%u", fWRes);        
        isOK = FALSE;
    }
    
    // update results to user
    unsigned int totalSec = totalRecWords / RECORDING_FREQ;
    unsigned int totalKB =  totalRecWords / 512;
    if (isOK)
    {
        sprintf(strResBuf, "%uKB (%02u:%02u)", totalKB, totalSec / 60, totalSec % 60);
    }
    else {
        sprintf(strResBuf, "Rec Err (%uKB)", totalKB);
    }
    drawMenuInfo(strResBuf);
    SendUARTStr(strResBuf);  
    totalRecWords = 0;    
       
    // analog keypad for normal operation
    setKeypadAnalog(TRUE);
    
    if (radioRecordingMode == RECORD_MODE_USB)
    {
        // disable CH376        
        ch376_reset();                      // Must reset first, otherwise sleep has no effect
        delay_ms(1);                        // Wait a bit before sleeping
        ch376_sleep();
        delay_ms(1);                        // Wait until all SPI transmission (hardware) is done before disabling SPI
        SPI2STATbits.SPIEN = 0;
    }
    else {
        // disable SD card SPI module
        SPI1STATbits.SPIEN = 0;
    }
        
    //delay_ms(600);
    
    radioRecordingMode = RECORD_MODE_NONE;    
    isPwrBtnPressed = FALSE;
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
    
    resetMenuLine();
    LCD_updateDisplay();  
    
    // beep to indicate end of recording
    // must be after the set of recording mode to none
    // otherwise because the T4 frequency is 64kHz and works too hard during recording
    // there is no CPU time for other processed, which might cause the system to hang 
    // or the delay to last a very long time
    tinyBeep(FALSE);
    
    resetMenuOption();
}

void updateRadioConfig() {
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        // FM
        radioMinFreq = RADIO_FM_MIN_87MHz;
        if (radioFMRange == RADIO_FM_RANGE_64MHz)
        {
            radioMinFreq = RADIO_FM_MIN_64MHz;
        }
        else if (radioFMRange == RADIO_FM_RANGE_76MHz)
        {
            radioMinFreq = RADIO_FM_MIN_76MHz;
        }        
        radioMaxFreq = RADIO_FM_MAX_MHz;
        
        // Frequency step in 0.1MHz
        radioFreqStep = getRadioFMSpacing(radioFMSpacing);
        
    }
    else if (radioCurBand == RADIO_CUR_BAND_MW)
    {
        // AM
        radioMinFreq = (radioAMSpacing == RADIO_MW_SPACING_9kHZ ? RADIO_AM_MIN_9kHz : RADIO_AM_MIN_10kHz);
        radioMaxFreq = RADIO_AM_MAX_kHz;
        radioFreqStep = getRadioAMSpacing(radioAMSpacing);         // in kHz
    }
    else if (radioCurBand == RADIO_CUR_BAND_SW)
    {
        // SW
        radioMinFreq = RADIO_SW_MIN_kHz;
        radioMaxFreq = RADIO_SW_MAX_kHz;
        radioFreqStep = RADIO_SW_SPACING;                  // in kHz
    }
    else 
    {
        // long wave
        radioMinFreq = RADIO_LW_MIN_kHz;
        radioMaxFreq = RADIO_LW_MAX_kHz;
        radioFreqStep = RADIO_LW_SPACING;                  // in kHz
    }
                
    if (radioCurFreq < radioMinFreq || radioCurFreq > radioMaxFreq)
    {
        radioCurFreq = radioMinFreq;
    }
                
    debugPrint("B:%u,%u-%u(%u),S:%u", radioCurBand, radioMinFreq, radioMaxFreq, radioCurFreq, radioFreqStep);
}

void updateRadioFrequencyDisplay()
{
    char buf[16];
    
    BOOL isValidPreset = FALSE;
    getRadioBandStr(radioCurBand, buf);        
    LCD_drawItemTitle(EMPTY_TITLE_LINE);
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        unsigned int curF1 = radioCurFreq / 100;
        unsigned int curF2 = radioCurFreq % 100;

        if (isValidPreset)
        {
            sprintf(strResBuf, "%s %u.%02u #%u", buf, curF1, curF2, radioCurPresetInd);
        }
        else {
            sprintf(strResBuf, "%s %u.%02uMHz", buf, curF1, curF2);
        }
    }
    else 
    {
        if (isValidPreset)
        {
            sprintf(strResBuf, "%s %u #%u", buf, radioCurFreq, radioCurPresetInd);
        }
        else {
            sprintf(strResBuf, "%s %ukHz", buf, radioCurFreq);
        }
    }
    LCD_drawItemTitle(strResBuf);
}

void updateRadioScreen() {
    char buf[16];
    
    if (shouldDrawClock)
    {
        shouldUpdateRadioScreen = TRUE;
        shouldDrawClock = FALSE;
    }
    
    if (radioRecordingMode != RECORD_MODE_NONE && curMenuOption == MENU_ITEM_INVALID)
    {
        if (totalRunningInactiveSecs - lastRecDurationUpdate >= 1)
        {
            // update recording duration every seconds
            lastRecDurationUpdate = totalRunningInactiveSecs;
            
            unsigned int totalSec = totalRecWords / RECORDING_FREQ;
            unsigned char hour = totalSec / 3600;
            unsigned char min  = (totalSec - hour * 3600) / 60;
            unsigned char sec  = (totalSec - hour * 3600 - min * 60);
            
            if (hour > 0)
            {
                sprintf(strResBuf, "%02u:%02u:%02u", hour, min, sec);
                LCD_drawRecDuration2(strResBuf);
            }
            else {
                sprintf(strResBuf, "%02u:%02u", min, sec);
                LCD_drawRecDuration(strResBuf);
            }
            LCD_updateMenuText();                            
        }
    }
    
    // update when requested
    if (shouldUpdateRadioScreen)
    {
        // SendUARTStr("U");                    
        drawTopClock(); 
        drawInputSource();
        drawBattery(TRUE);

        LCD_drawItemTitle(EMPTY_TITLE_LINE);  
                
        if (curMenuOption == MENU_ITEM_INVALID)
            resetMenuOption();
        
        // current frequency
        if (isRadioSeeking)
        {
            LCD_drawItemTitle("Searching");
        }
        else {
            updateRadioFrequencyDisplay();
        }

        // additional info
        if (shouldSetRTC && !isAutoScanningRadio)
        {
            LCD_drawInfoText(CLOCK_SET_STR);
            shouldSetRTC = FALSE;
        }
        
        if (justChangedBfoOffset)
        {
            sprintf(strResBuf, "BFO Offset: %dHz", radioSSBBfoOffset);
            SendUARTStr(strResBuf);
            drawMenuInfo(strResBuf);   
            
            justChangedBfoOffset = FALSE;
        }
                
        if (radioCurBand == RADIO_CUR_BAND_SW && hasStationInfo && !isAutoScanningRadio)
        {
            strcpy(strResBuf, "");
            swGetArrayElementName(stnNameInd - 1, SW_ARRAY_STATIONS, strResBuf);
            unsigned char fLen = strlen(strResBuf);
                
            if (fLen > 0)
            {
                LCD_drawInfoText(EMPTY_INFO_LINE);
                LCD_drawSmallInfoText(EMPTY_SMALL_INFO_LINE);
                                
                if (fLen > MAX_TITLE_CHAR_SMALL)
                {
                    // lower case in small font size looks ugly
                    uppercaseString(strResBuf);
                    LCD_drawSmallInfoText2(strResBuf, 0, MAX_TITLE_CHAR_TINY);                 
                }
                else {
                    // big fone size, keep lower case
                    LCD_drawInfoText2(strResBuf, 0, MAX_TITLE_CHAR_SMALL);                 
                }
            }
        }
    }
    
    // update weather info every few sec
#define WEATHER_INFO_UPDATE_SEC     60
    if (
            (lastUpdateRadioWeather == 0)
            || 
            (totalRunningSecs - lastUpdateRadioWeather > WEATHER_INFO_UPDATE_SEC) 
        )
    {
        if (remoteDigitsPressedCount == 0)    
                displayWeatherInfo(FALSE, TRUE);
        else 
            drawRemoteInput();
        
        lastUpdateRadioWeather = totalRunningSecs;
        shouldUpdateRadioScreen = TRUE;
    }
            
    unsigned char rd;                  
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        // retrieve RDS info in FM mode
        
        // we need 1/4 second resolution, hence we cannot use totalRunningSecs
        // tmr4IntCount will roll over at samplingRate, we cater for this
        unsigned int tmr4lapsed = 0;
        if (tmr4IntCount > lastUpdateFmRds)
        {
            tmr4lapsed = tmr4IntCount - lastUpdateFmRds;
        }
        else {
            // in radio mode, tmr4 runs at RECORDING_TIMER_FREQ, whether or not recording is enabled
            tmr4lapsed = (lastUpdateFmRds - tmr4IntCount) + RECORDING_TIMER_FREQ;
        }
        
        // read RDS a few times per second
        // interested in station names only. Other fields usually do not contain useful info.
        if ( tmr4lapsed > (RECORDING_TIMER_FREQ / 4) )
        {        
            // if the FM station has just been loaded, we try over multiple attempts to retrieve RDS in the hope of getting the full station name immediately
            // for subsequent attempts, try once only. this is a workaround to retrieve the full station name in the lack of RDS interrupt due to lack of pins
            unsigned char c;
            // unsigned char cMax = (hasTriedFullRDS ? 1 : 16);
            // TODO: No need for this, as multiple attempts do not seem to improve performance
            unsigned char cMax = 1;
            
            for (c = 0; c < cMax; c++)
            {
                BOOL isRDSOk = si473x_updateRdsStatus(0, 0, 0);            
                if (isRDSOk)
                {
                    unsigned char rdsPsInd = si473x_getRdsText0A(curStationName);
                    if (rdsPsInd == 0xFF)
                    {
                        // SendUARTStr("NoRDS");
                    }
                    else {
                        // station name max 8 characters, sent down in blk of 2 chars each, index returns from 0 to 3
                        // consider complete when 0/1/2/3 have been returned (total 7 for last 3 bits)
                        fmRdsPSIndex |= (1 << rdsPsInd);
                    }
                }
                else {
                    // SendUARTStr("RDSEr");
                }
                
                if (cMax > 1)
                {
                    delay_ms(1);
                }
            }
            
            lastUpdateFmRds = tmr4IntCount; 
            hasTriedFullRDS = TRUE;
        }        
    }

    // update signal info every few sec
#define RADIO_INFO_UPDATE_INT   5    
    if ( 
          shouldUpdateRadioScreen || 
          (lastUpdateRadioSignal == 0)
            || 
          (totalRunningSecs - lastUpdateRadioSignal > RADIO_INFO_UPDATE_INT) 
       )
    {                         
        drawStatusBar();
        lastUpdateRadioSignal = totalRunningSecs;   
                
        if (!shouldUpdateRadioScreen)
        {
            // if there is no need to update the entire screen,
            // we do partial update for the status bar to save power
            LCD_updateStatusText();
        }
        else {
            // otherwise the screen will be updated towards the end of this function
        }
    }
    
    // check if FM RDS display has changed and only update display if so
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        BOOL isRDSDone = (fmRdsPSIndex >= 7);
        if (isRDSDone)
        {
            BOOL hasNameChanged = (strcmp(lastStationName, curStationName) != 0);
            if (hasNameChanged)
            {
                 SendUARTStr(curStationName);

                 strcpy(lastStationName, curStationName);             
                 lastUpdateFmRdsSuccess = totalRunningSecs;           
                 hasUpdatedFmRdsDisplay = FALSE; 
            }
        }

        // only update FM RDS a short time after the latest attempt which results in changes, and not immediately
        // this prevents the screen from refreshing repeatedly as RDS characters are sent down
        // this logic will prevent scrolling names from being displayed (mostly)
        // which is a good idea anyway since our logic cannot display scrolling name effectively
        // without a dedicated interrupt logic to read RDS values immediately after any changes
        // any attempt to display scrolling names will result in garbled characters
        if ( (totalRunningSecs - lastUpdateFmRdsSuccess > 2) && !hasUpdatedFmRdsDisplay)
        {        
    #define NO_CHAR_INDEX        0xFF
            unsigned char lastSpaceIndex = NO_CHAR_INDEX;
            unsigned char lastNonSpaceIndex = NO_CHAR_INDEX;
            unsigned char countNonSpaces = 0;

            // station name in upper case
            LCD_drawInfoText(EMPTY_INFO_LINE);        
            strcpy(buf, "");
            for (rd = 0; rd < MAX_FM_RDS_STATION_LEN; rd++)
            {
                if (lastStationName[rd] >= 'a' && lastStationName[rd] <= 'z')
                {
                    buf[rd] = lastStationName[rd] - 32;
                }
                else {
                    buf[rd] = lastStationName[rd];
                }

                if (lastStationName[rd] == ' ')
                {
                    lastSpaceIndex = rd;
                }
                else {
                    lastNonSpaceIndex = rd;
                    countNonSpaces++;
                }

                if (lastStationName[rd] == 0)
                {
                    break;
                }                
            }                                 

            // display only if name is completed
            // e.g. no more than two space in the middle
            BOOL shouldDisplay = TRUE;        
            if (lastSpaceIndex > 0 && lastSpaceIndex != NO_CHAR_INDEX)    
            {
                for (rd = 0; rd < lastSpaceIndex - 1; rd++)
                {
                    if (buf[rd] == 32 && buf[rd+1] == 32)
                    {
                        shouldDisplay = FALSE;
                        break;
                    }            
                }
            }

            // do not display names beginning with space or names with only 2 characters
            // which is likely an indication that the name is not complete as most stations don't do this         
            if (lastSpaceIndex == 0 && countNonSpaces > 0)
            {
                shouldDisplay = FALSE;
            }        
            if (lastNonSpaceIndex == 2)
            {
                shouldDisplay = FALSE;
            }

            if (shouldDisplay)
            {
                LCD_drawInfoText(buf);        
                hasUpdatedFmRdsDisplay = TRUE;                

                // partial update only, no need to update the entire screen
                // otherwise in 8MHz mode, updating the entire screen will take some time and cause us to lose the next few RDS characters
                // shouldUpdateRadioScreen = TRUE;        
                LCD_updateInfoText();
            }
        }
    }
    
    if (shouldUpdateRadioScreen)
    {
        LCD_updateDisplay();
    }
    
    shouldUpdateRadioScreen = FALSE;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Radio Navigation">
void setFmStereoMode()
{
    if (forceMono)
    {
        si473x_setFmStereoOff();
    }
    else {
        si473x_setFmStereoOn();
    }
}

void setRadioVolume(unsigned char vol)
{
    // our volume from 0-30, SI from 0-64
    si473x_setVolume(vol*2 + 3);   
}

void fetchRadioInfoEntry()
{
    if (hasStationInfo)
    {
        // debugPrint("%u(%u/%u)[%d]", stnInfoEntryCurIndex, stnInfoEntryFirstIndex, stnInfoEntryLastIndex, stnInfoArrayIndex);
                
        // station info params
        unsigned int stnSiteInd = 0;        
        if (stnInfoArrayIndex == 0)
        {
            stnNameInd = swFreqDb1[stnInfoEntryCurIndex][1];        
            stnSiteInd = swFreqDb1[stnInfoEntryCurIndex][2];        
            stnLat1 = swFreqDb1[stnInfoEntryCurIndex][3];        
            stnLat2 = swFreqDb1[stnInfoEntryCurIndex][4];        
            stnLng1 = swFreqDb1[stnInfoEntryCurIndex][5];        
            stnLng2 = swFreqDb1[stnInfoEntryCurIndex][6];      
            stnStartTime = swFreqDb1[stnInfoEntryCurIndex][7];
            stnEndTime = swFreqDb1[stnInfoEntryCurIndex][8];
            stnOnDays = (unsigned char)swFreqDb1[stnInfoEntryCurIndex][9];
            stnPower = swFreqDb1[stnInfoEntryCurIndex][10];
            stnAzimuth = swFreqDb1[stnInfoEntryCurIndex][11];
            stnCtyLang = swFreqDb1[stnInfoEntryCurIndex][12];
        }
        else if (stnInfoArrayIndex == 1)
        {
            stnNameInd = swFreqDb2[stnInfoEntryCurIndex][1];
            stnSiteInd = swFreqDb2[stnInfoEntryCurIndex][2];        
            stnLat1 = swFreqDb2[stnInfoEntryCurIndex][3];        
            stnLat2 = swFreqDb2[stnInfoEntryCurIndex][4];        
            stnLng1 = swFreqDb2[stnInfoEntryCurIndex][5];        
            stnLng2 = swFreqDb2[stnInfoEntryCurIndex][6];        
            stnStartTime = swFreqDb2[stnInfoEntryCurIndex][7];
            stnEndTime = swFreqDb2[stnInfoEntryCurIndex][8];
            stnOnDays = (unsigned char)swFreqDb2[stnInfoEntryCurIndex][9];
            stnPower = swFreqDb2[stnInfoEntryCurIndex][10];
            stnAzimuth = swFreqDb2[stnInfoEntryCurIndex][11];
            stnCtyLang = swFreqDb2[stnInfoEntryCurIndex][12];
        }
        else if (stnInfoArrayIndex == 2)
        {
            stnNameInd = swFreqDb3[stnInfoEntryCurIndex][1];
            stnSiteInd = swFreqDb3[stnInfoEntryCurIndex][2];        
            stnLat1 = swFreqDb3[stnInfoEntryCurIndex][3];        
            stnLat2 = swFreqDb3[stnInfoEntryCurIndex][4];        
            stnLng1 = swFreqDb3[stnInfoEntryCurIndex][5];        
            stnLng2 = swFreqDb3[stnInfoEntryCurIndex][6];  
            stnStartTime = swFreqDb3[stnInfoEntryCurIndex][7];
            stnEndTime = swFreqDb3[stnInfoEntryCurIndex][8];
            stnOnDays = (unsigned char)swFreqDb3[stnInfoEntryCurIndex][9];
            stnPower = swFreqDb3[stnInfoEntryCurIndex][10];
            stnAzimuth = swFreqDb3[stnInfoEntryCurIndex][11];
            stnCtyLang = swFreqDb3[stnInfoEntryCurIndex][12];
        }
        else 
        {
            stnNameInd = swFreqDb4[stnInfoEntryCurIndex][1];
            stnSiteInd = swFreqDb4[stnInfoEntryCurIndex][2];        
            stnLat1 = swFreqDb4[stnInfoEntryCurIndex][3];                
            stnLat2 = swFreqDb4[stnInfoEntryCurIndex][4];        
            stnLng1 = swFreqDb4[stnInfoEntryCurIndex][5];
            stnLng2 = swFreqDb4[stnInfoEntryCurIndex][6];       
            stnStartTime = swFreqDb4[stnInfoEntryCurIndex][7];
            stnEndTime = swFreqDb4[stnInfoEntryCurIndex][8];
            stnOnDays = (unsigned char)swFreqDb4[stnInfoEntryCurIndex][9];
            stnPower = swFreqDb4[stnInfoEntryCurIndex][10];
            stnAzimuth = swFreqDb4[stnInfoEntryCurIndex][11];
            stnCtyLang = swFreqDb4[stnInfoEntryCurIndex][12];
        }
                        
        // site name stored in fullAudioFilename variable (we don't use it in radio mode otherwise)
        strcpy(fullAudioFilename, "");
        swGetArrayElementName(stnSiteInd - 1, SW_ARRAY_SITES, fullAudioFilename);
        
        // broadcast schedule
        // For dow (below function)
        // Sun=0, Mon=1, Tue=2, Wed=3, Thu=4, Fri=5, Sat=6
        unsigned char dow = getDayOfWeek(2000 + curYear2k, curMonth, curDay);
        stnIsBroadcasting = FALSE;
        BOOL isOnDay = (stnOnDays >> dow) & 1;
        if (isOnDay)
        {
#define FULL_DAY_HRS        2400
            
            // current time to compare broadcast schedule, using timezone to adjust back to UTC
            // startTime: 0900 hrs, endTime: 1700 hrs (UTC)
            unsigned int curTime = (curHour * 100) + curMinute;
            if (curTimeZone >= 0)
            {
                // curHour=10, curMinute=00, curTime=1000
                // positive timezone, minus back for UTC
                unsigned int timezoneAdj = curTimeZone * 100;
                if (curTime >= timezoneAdj)
                {
                    // curTimeZone = +0700, curTime = 0300 (UTC), same day
                    curTime -= timezoneAdj;
                }
                else {
                    // curTimezone = +1100, curTime = 2300 (UTC), the day before
                    curTime = curTime + FULL_DAY_HRS - timezoneAdj;
                }
            }
            else {
                // negative timezone, add back for UTC
                unsigned int timezoneAdj = (-curTimeZone) * 100;
                curTime += timezoneAdj;
                if (curTime > FULL_DAY_HRS)
                {
                    // curTime = 1900, curTimezone = +0800 => curTime = 0300 (UTC)
                    curTime -= FULL_DAY_HRS;
                }
            }
            
            if (stnStartTime < stnEndTime && curTime >= stnStartTime && curTime <= stnEndTime)
            {
                // stnStartTime = 0800, stnEndTime = 1600, curTime = 1200
                stnIsBroadcasting = TRUE;
            }            
            else if (stnStartTime > stnEndTime && (curTime >= stnStartTime || curTime <= stnEndTime) )
            {
                // stnStartTime = 2100, stnEndTime = 1600, curTime = 0900
                stnIsBroadcasting = TRUE;                
            }
            
            // debugPrint("%04u(%04u-%04u)[%d]-%d", curTime, stnStartTime, stnEndTime, curTimeZone, stnOnDays);
        }
        // debugPrint("%d[%d,%d](%04u-%04u)", stnIsBroadcasting, stnOnDays, dow, stnStartTime, stnEndTime);
                
        // my coordinates, exactly 2 decimal places
        strcpy("", stnDirection);        
        if (stnLat1 > -90 && stnLat1 < 90 && stnLng1 > -180 && stnLng1 < 180)
        {
            float myLat = curMyLat1 + curMyLat2 / 100.0;
            float myLng = curMyLng1 + curMyLng2 / 100.0;

            // station coordinates, exactly 3 decimal places
            float stnLat = stnLat1 + stnLat2 / 1000.0;       
            float stnLng = stnLng1 + stnLng2 / 1000.0;       

            // distance
            stnDistanceKm = distanceFromPoint(myLat, myLng, stnLat, stnLng);
            compassDirFromPoint(myLat, myLng, stnLat, stnLng, stnDirection);
        }
        else {
            stnDistanceKm = STN_DISTANCE_INVALID;
        }
    }
}

void showLoadedPreset(unsigned int pr)
{
    if (pr != RADIO_INVALID_PRESET_IND)
    {
        // workaround to make sure that preset input display will be shown for at least a while
        // (e.g. reset weather refresh count);
        lastUpdateRadioWeather = totalRunningSecs;
                
        LCD_drawTopLeftInfo2("   ");
        sprintf(strResBuf, "Loaded #%u", pr);        
        sprintf(strResBuf, "#%04d ", pr);
        LCD_drawTopLeftInfo(strResBuf);
        LCD_updateDisplay();
    }    
}

BOOL gotoRadioFreq(unsigned int freq)
{
    BOOL retVal = FALSE;
    
    if (isPaused) {
        startRadio();
    }
    
    if (radioCurBand == RADIO_CUR_BAND_SW && hasStationInfo && !isAutoScanningRadio)
    {
        LCD_drawInfoText(EMPTY_INFO_LINE);
        LCD_drawSmallInfoText(EMPTY_SMALL_INFO_LINE);
        LCD_updateInfoText();
                        
        resetMenuLine();
        LCD_updateMenuText();
    }
    
    shouldShowMatchCount = FALSE;    
    stopAnyCurRadioRecording();    
    radioCurFreq = freq;
    
    // debugPrint("p%u %d", radioCurPresetInd, ignorePartialRefreshFreq);
    LCD_drawMenuText(EMPTY_MENU_LINE);            
    radioCurPresetInd = RADIO_INVALID_PRESET_IND;
   
    // SendUARTStr("ud1");
    if (!ignorePartialRefreshFreq)
    {
        // SendUARTStr("ud");
        
        updateRadioFrequencyDisplay();
        LCD_updateItemTitle();
        LCD_updateMenuText();        
    }
    
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        debugPrint("FM=%u", freq);    
        
        // FM Mode
         if (hasRadioBandChanged)
         {
            retVal = si473x_setFM(radioMinFreq, radioMaxFreq, radioCurFreq, radioFreqStep);
         }
         else {
             retVal = si473x_setFrequency(radioCurFreq);                     
         }
         si473x_setFMDeEmphasis(radioFMDeemp + 1);  
         
         // RDS configuration. Refer to method comments for explanation.
         si473x_setRdsConfig(1, 2, 2, 2, 2);         
         memset(curStationName, 0, sizeof(curStationName));
         memset(lastStationName, 0, sizeof(lastStationName));
         lastUpdateFmRds = 0;
         lastUpdateFmRdsSuccess = 0;
         fmRdsPSIndex = 0;
         hasUpdatedFmRdsDisplay = FALSE;
         hasTriedFullRDS = FALSE;
    }
    else if (radioSSBMode != SSB_NONE) {
         debugPrint("SSB=%u", freq);      
         
         // SSB Mode
         if (hasRadioBandChanged)
         {
            retVal = si473x_setSSB(radioMinFreq, radioMaxFreq, radioCurFreq, radioFreqStep, radioSSBMode);
         }
         else {
             retVal = si473x_setFrequency(radioCurFreq);                     
         }
         si473x_setSSBAudioBandwidth(radioSSBBW);         
         si473x_setSSBSync(radioSSBSync);
         si473x_setSSBMode(radioSSBMode);         
    }
    else {
         debugPrint("AM=%u", freq);
         
         // AM Mode         
         si473x_setSSBMode(radioSSBMode);                  
         if (hasRadioBandChanged)
         {
            retVal = si473x_setAM(radioMinFreq, radioMaxFreq, radioCurFreq, radioFreqStep);
         }
         else {
             retVal = si473x_setFrequency(radioCurFreq);                     
         }
         si473x_setBandwidth(radioAMBW, 0);
    }
    
    // si473x_setAutomaticGainControl(0, 0);
                 
    hasRadioBandChanged = FALSE;
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        setFmStereoMode();
    }
    
    // clear any previous FM RDS station names
    if (!ignorePartialRefreshFreq)
    {
        if (!isAutoScanningRadio)
        {
            LCD_drawInfoText(EMPTY_INFO_LINE);
        }
    }
    
    ignorePartialRefreshFreq = FALSE;
    shouldUpdateRadioScreen = TRUE;    
    lastUpdateRadioSignal = 0;
    
    // SW station information, if any
    if (radioCurBand == RADIO_CUR_BAND_SW)
    {
        hasStationInfo = swGetFreqInfo(freq, &stnInfoEntryFirstIndex, &stnInfoEntryLastIndex, &stnInfoArrayIndex);
        if (hasStationInfo)
        {
            if (curOperatingMode == OP_MODE_FASTEST)
            {
                // in fast mode, search for best match station info entry immediately
                findStnInfo();
            }
            else {
                // operate in slow mode, just default to first entry
                // otherwise searching will be too slow for 8MHz
                stnInfoEntryCurIndex = stnInfoEntryFirstIndex;
            }
            
            // fetch the info for the default entrystnInfoEntryLastIndex
            // debugPrint("%u %u %u", stnInfoEntryCurIndex, stnInfoEntryFirstIndex, stnInfoEntryLastIndex);
            fetchRadioInfoEntry();
        }
        else {
            SendUARTStr("NoStIn");
        }
    }
        
    return retVal;
}

void startRadio()
{        
    if (radioSSBMode != SSB_NONE)
    {
        drawMenuInfo(LOADING_MENU_LINE);
    }
    
    isPaused = FALSE;
    isFilePlaying = TRUE;
    shouldUpdateRadioScreen = TRUE;
    updateRadioScreen();
    LCD_updateDisplay();
    
    if (radioCurBand != RADIO_CUR_BAND_FM)
    {            
        radioSetAmOrSsbMode(FALSE);    
    }
    else
    {
        si473x_radioPowerUp(POWER_UP_FM);
    }
        
    delay_ms(500);
    hasRadioBandChanged = TRUE;
    gotoRadioFreq(radioCurFreq);
    delay_ms(500);
    setRadioVolume(volumeLevel);
    
    if (radioSSBMode != SSB_NONE)
    {
        resetMenuOption();
        LCD_updateDisplay();
    }
}

void stopRadio() {
    si473x_radioPowerDown();
    
    isPaused = TRUE;
    isFilePlaying = FALSE;
}

void gotoNextRadioBand()
{
    stopAnyCurRadioRecording();
                   
    radioCurBand = (radioCurBand + 1) % MAX_RADIO_BANDS;
    radioCurFreq = 0;

    radioSSBMode = SSB_NONE;
    radioSSBSync = FALSE;

    LCD_drawInfoText(EMPTY_INFO_LINE);
    LCD_drawSmallInfoText(EMPTY_SMALL_INFO_LINE);
    
    updateRadioConfig();
    hasRadioBandChanged = TRUE;
    gotoRadioFreq(radioCurFreq);
    shouldUpdateRadioScreen = TRUE;    
}

void gotoNextSwBand()
{
    unsigned char c;
    unsigned int retVal = swBands[0];
    BOOL isLastBand = TRUE;
    for (c = 0; c < sizeof(swBands) / 4; c++)           // array of integer (2 bytes each), each band consumes 2 integer each (start-end freq);
    {
        unsigned int bStart = swBands[c*2];
        if (radioCurFreq < bStart)
        {
            isLastBand = FALSE;
            
            // switch to next beginning of band
            retVal = bStart;
            break;
        }
    }    
    
    if (isLastBand)
    {
        // end of SW, go to next band 
        gotoNextRadioBand();
    }
    else {
        // go to next shortwave band
        gotoRadioFreq(retVal);
    }
}

void gotoRadioPrevFreq()
{
    // loop around
    unsigned char step = isRadioFineTune ?  1 : radioFreqStep; // Fine-tune: 10kHz in FM mode, 1kHZ otherwise
    unsigned int newFreq = radioCurFreq - step;
    gotoRadioFreq( (newFreq >= radioMinFreq) ? newFreq : radioMaxFreq);
}

void gotoRadioNextFreq()
{
    // loop around
    unsigned char step = isRadioFineTune ?  1 : radioFreqStep; // Fine-tune: 10kHz in FM mode, 1kHZ otherwise
    unsigned int newFreq = radioCurFreq + step;
    gotoRadioFreq( (newFreq < radioMaxFreq) ? newFreq : radioMinFreq);
}

// In theory the SI473X can communicate with MCU via I2C (just 2 pin)
// However station seeking will generate an interrupt at the end of the process. 
// While seeking the MCU cannot send any clock cycles on the I2C bus, otherwise seeking will terminate prematurely
// The original code waits for around 10-20ms after seeking before querying the SI for the current frequency
// This works fine if available stations are close (e.g. crowded FM bands) but will not work well if there are only a few available signals 
// The documented way is to use an interrupt so that our MCU will be informed when the seek ends,
// but even with this, our MCU has no way to know which frequency the seek is currently at if the seek takes long (e.g. entire shortwave bands)
// Our workaround is to only seek within a short portion of the band, estimate how long it takes,
// delay for this amount of time and query the radio for the current frequency. If no new frequencies
// are found, we will continue seeking                             
// If isContinuous is FALSE, the function will search up to the next available frequency and go to that frequency
// If isContinuous is TRUE, the function will search up to the next available frequency but does not go to that frequency.
// Pass TRUE to implement entire band searching feature from caller function
// If fullSWSearch is TRUE, function will search entire SW bands. Otherwise, it will only search segments meant for broadcast frequencies.
// Return the next frequency found, 0xFFFF otherwise.
unsigned int seekStation(unsigned char seek_up, BOOL fullSWSearch, BOOL isContinuous, unsigned int *totalFreqsSearched, unsigned int maxFreqCount, BOOL *isKeybCancelled)
{
    char buf[16];
    
    // seek command does not work for SSB
    if (radioSSBMode != SSB_NONE)
        return RADIO_INVALID_FREQUENCY;

    memset(curStationName, 0, sizeof(curStationName));
    memset(lastStationName, 0, sizeof(lastStationName));
        
    // show SEEK stars
    BOOL hasSearchedWrapped = FALSE;
    isPaused = FALSE;
    isFilePlaying = TRUE;
    isRadioSeeking = TRUE;
    shouldUpdateRadioScreen = TRUE;    
    
    if (!isContinuous)
    {
        updateRadioScreen(); 
    }
    
    LCD_clearRect(0, STATUS_LINE_Y, LCD_SCR_WIDTH, LCD_SCR_WIDTH -  STATUS_LINE_Y);    
    LCD_drawBitmap(seek_up ? forwardIcon : backwardIcon, 0, STATUS_LINE_Y);        
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        sprintf(strResBuf, "%5u-%-5u", radioMinFreq / 100, radioMaxFreq / 100);
        LCD_drawString( (LCD_SCR_WIDTH - 6 * strlen(strResBuf) ) / 2 - 11, STATUS_LINE_Y, strResBuf, 1);        
    }    
    LCD_updateDisplay();
       
    /* 
    AN332 page 248 and 249.
    Worst case seek time per channel: tSTC = 200 ms (AM) and 60ms (FM).
    Worst case seek time complete: ((TOP - BOTTOM) / SPACING + 1) * tSTC
    
    e.g. 6.2sec for USA FM (87.50 - 107.90 MHz, 20MHz spacing) and 24.0s for USA AM (520 - 1710 kHz, 10kHz spacing)
    */            
    
    // seek begin    
    unsigned int newFreq = 0;
    unsigned int seekTempFreq = radioCurFreq;    
    unsigned int seekTopFreq = seek_up ? radioMinFreq : radioMaxFreq;
    unsigned int seekBottomFreq = seek_up ? radioMaxFreq : radioMinFreq;
    unsigned int seekTimeMs = 0;
    unsigned char seekFreqBlock = 0;
    unsigned char tSTC = 0;
    BOOL isFreqValid = FALSE;
    BOOL justWrapped = FALSE;
       
    // length of each frequency block for searching
    // AM takes a longer time to search so the block is shorter
    if (radioCurBand == RADIO_CUR_BAND_FM)
    {
        tSTC = 60;
        seekFreqBlock = 100;
    }
    else {
        tSTC = 200;
        
        if (radioCurBand == RADIO_CUR_BAND_LW)
        {
            seekFreqBlock = 10;
        }
        else {
            seekFreqBlock = radioFreqStep * 5;
        }
    }
    
    // make keyboard digital so that we can now look for an interrupt when the key is pressed to end tuning, instead of having
    // to sample ADC value constantly, which is not possible due to the delay in tuning
    setKeypadAnalog(FALSE);          
        
    // seek in blocks until new stations are found or timeout
    // higher number of blocks will result in faster seek time if there are no stations and slower seek time if there are a lot of stations in the band
    isTFMSInt = FALSE;
    lastRemoteChar = KEY_INVALID;
    if (!isContinuous)
    {
        si473x_setAudioMute(TRUE);
    }
    BOOL hasShownFreq = FALSE;
    unsigned int seekTimeCnt = 0;
    do
    {
        // top frequency for seeking
        if (radioCurBand == RADIO_CUR_BAND_SW || radioCurBand == RADIO_CUR_BAND_LW)
        {
            // round to multiples 5KHz (SW) or 1kHz (LW) starting after current frequency
            if (radioCurBand == RADIO_CUR_BAND_LW)
            {
                if (seek_up)
                {
                    seekTopFreq = seekTempFreq + RADIO_LW_SPACING;            
                }
                else 
                {
                    seekTopFreq = seekTempFreq - RADIO_LW_SPACING;
                }
            }
            else {
                // debugPrint("%u %u", seekTempFreq, radioFreqStep);
                if (seek_up)
                {
                    seekTopFreq = (seekTempFreq / radioFreqStep + 1) * radioFreqStep;            
                }
                else 
                {
                    seekTopFreq = (seekTempFreq / radioFreqStep - 1) * radioFreqStep;               
                }
            }
            seekFreqBlock = radioFreqStep * 2;         
        } 
        else 
        {
            if (!justWrapped)
            {
                // for AM or FM, take into account beginning frequency and frequency spacing             
                if (seek_up)
                {
                    while (seekTopFreq <= seekTempFreq)
                    {
                        // debugPrint("top=%u", seekTopFreq);
                        seekTopFreq += radioFreqStep;
                    }
                }
                else {
                    while (seekTopFreq >= seekTempFreq)
                    {
                        // debugPrint("top=%u", seekTopFreq);
                        seekTopFreq -= radioFreqStep;
                    }
                }
            }
            else {
                justWrapped = FALSE;
            }
            seekFreqBlock = radioFreqStep * 5;         
        }
        if (seek_up)
        {
            seekTopFreq = MIN(seekTopFreq, radioMaxFreq);
        }
        else {
            seekTopFreq = MAX(seekTopFreq, radioMinFreq);
        }
        
        // if in shortwave, only seek within commercial bands
        // adjust top frequency if outside commercial bands
        // TODO: Switching of bands might not be entirely accurately for the last scanning block at the end of the band, which is not really important.
        unsigned int swCurBandMin = RADIO_SW_MIN_kHz;
        unsigned int swCurBandMax = RADIO_SW_MAX_kHz;
        if (!fullSWSearch)
        {
            BOOL isWithinBands = FALSE;
            if (radioCurBand == RADIO_CUR_BAND_SW)
            {
                unsigned char ind = 0;
                unsigned int smallestBStarts = 0;
                unsigned int largestBEnds = 0;
                for (ind = 0; ind < sizeof(swBands) / 4; ind++)           // array of integer (2 bytes each), each band consumes 2 integer each (start-end freq);
                {
                    unsigned int bStarts = swBands[ind*2];
                    unsigned int bEnds = swBands[ind*2+1];                
                    if (seekTopFreq >= bStarts && seekTopFreq <= bEnds)
                    {
                        // debugPrint("%u/B:%u-%u", seekTopFreq, bStarts, bEnds);
                        isWithinBands = TRUE;
                        swCurBandMin = bStarts;
                        swCurBandMax = bEnds;

                        break;
                    }

                    if (smallestBStarts == 0 && seekTopFreq < bStarts && seekTopFreq < bEnds)
                    {
                        smallestBStarts = bStarts;
                    }

                    if (bEnds > largestBEnds && seekTopFreq > bStarts && seekTopFreq > bEnds)
                    {
                        largestBEnds = bEnds;
                    }

                }

                if (!isWithinBands)
                {
                    if (seek_up && smallestBStarts > 0)
                    {
                        seekTopFreq = smallestBStarts;
                    }

                    if (!seek_up && largestBEnds > 0)
                    {
                        seekTopFreq = largestBEnds;
                    }

                    debugPrint("ADT=%u", seekTopFreq);
                }
            }
        }
        
        // bottom frequency for seeking
        if (seek_up)
        {
            seekBottomFreq = MIN(seekTopFreq + seekFreqBlock, radioMaxFreq);
        }
        else {
            seekBottomFreq = MAX(seekTopFreq - seekFreqBlock, radioMinFreq);
        }
                             
        if (!fullSWSearch)
        {
            if (radioCurBand == RADIO_CUR_BAND_SW)
            {
                // reach end of commercial shortwave bands
                unsigned int topSwFreq = swBands[0];
                unsigned int bottomSwFreq = swBands[sizeof(swBands)/2 - 1];
                // debugPrint("rg:%u-%u",topSwFreq,bottomSwFreq);
                if ( 
                     (seek_up && seekBottomFreq > bottomSwFreq) ||
                     (!seek_up && seekTopFreq < topSwFreq) 
                   )
                {
                    break;
                }
            }
        }
        
        // worse case seek time
        unsigned int freqCount = (abs(seekBottomFreq - seekTopFreq) / radioFreqStep) + 1;
        seekTimeMs = freqCount * tSTC;
        seekTimeCnt += seekTimeMs;
                
        // set seek limits and threshold
        debugPrint("SK:%u-%u,S:%u,%ums", seekTopFreq, seekBottomFreq, radioFreqStep, seekTimeMs);  
                        
        if (seekTimeCnt > 3000 || !hasShownFreq)
        {
            unsigned int freqCursor = seekBottomFreq;                         
            if (radioCurBand == RADIO_CUR_BAND_SW && swCurBandMin > 0 && swCurBandMax > 0)
            {
                sprintf(strResBuf, "%5u-%-5u", swCurBandMin, swCurBandMax); // trailing space for band end
                LCD_drawString( (LCD_SCR_WIDTH - 6 * strlen(strResBuf) ) / 2 - 11, STATUS_LINE_Y, strResBuf, 1);   
                
                // temp fix for inaccurate search cursor which is slightly out of range
                if (freqCursor > swCurBandMax)
                {
                    freqCursor = swCurBandMax;
                }
            }            
            
            if (radioCurBand == RADIO_CUR_BAND_FM)
            {
                sprintf(buf, "%u", freqCursor / 100);    

            }
            else {
                sprintf(buf, "%u", freqCursor);    
            }            
            sprintf(strResBuf, "%6s", buf);
            LCD_drawString(92, STATUS_LINE_Y, strResBuf, 1);    
            
            
            LCD_updateDisplay();
            
            hasShownFreq = TRUE;
            seekTimeCnt = 0;
        }        
        
        if (
                (hasSearchedWrapped) && ( 
                    (seek_up && seekTopFreq > radioCurFreq) || (!seek_up && seekBottomFreq < radioCurFreq) 
                )
           )
        {
            // SendUARTStr("WrapEnds");
            break;
        }        
                
        // debugPrint("frs=%d", radioFreqStep);
        if (radioCurBand == RADIO_CUR_BAND_FM)
        {
            si473x_setSeekFmLimits(seek_up ? seekTopFreq : seekBottomFreq, seek_up ? seekBottomFreq : seekTopFreq);    
            si473x_setSeekFmSpacing(radioFreqStep);
            si473x_setSeekFmSrnThreshold(1);
            si473x_setSeekFmRssiThreshold(5);
        }   
        else {
            si473x_setSeekAmLimits(seek_up ? seekTopFreq : seekBottomFreq, seek_up ? seekBottomFreq : seekTopFreq);
            si473x_setSeekAmSpacing(radioFreqStep);
            si473x_setSeekAmSrnThreshold(1);
            si473x_setSeekAmRssiThreshold(5);
        }

        // start seeking
        si473x_seekStation(seek_up, TRUE);
        
        // delay for the worse case scenario before querying, add some safety margin
        delay_ms(seekTimeMs + 10);          

        // get new frequency
        newFreq = si473x_getFrequency(&isFreqValid);
        
        unsigned int deltaCnt = (abs(
                            (isFreqValid ? newFreq : seekBottomFreq)
                            - seekTopFreq) / radioFreqStep) + 1;
        *totalFreqsSearched += deltaCnt;
        // debugPrint("%u", *totalFreqsSearched);
        
        // reach max freq count in continuous mode
        if (isContinuous &&  ((*totalFreqsSearched) >= maxFreqCount) )
        {
            break;
        }
                
        if (abs(seekBottomFreq - seekTopFreq) < seekFreqBlock)
        {
            // end of band reached
            if (hasSearchedWrapped)
            {
                // already wrapped, do nothing
                // SendUARTStr("EndBand");
                break;
            }
            else {
                // wrap
                // SendUARTStr("Wrap");                
                seekTempFreq = seek_up ? radioMinFreq : radioMaxFreq;
                seekTopFreq = seekTempFreq;
                
                hasSearchedWrapped = TRUE;
                justWrapped = TRUE;
            }
        }
        else {
            // next block
            seekTempFreq = seekBottomFreq;           
        }
                
        // press remote or power btn to end
        if (isTFMSInt || isPwrBtnPressed)
        {
            isTFMSInt = FALSE;
            lastRemoteChar = KEY_INVALID;
            isPwrBtnPressed = FALSE;
            *isKeybCancelled = TRUE;
            break;            
        }
    }
    while (!isFreqValid);
        
    // normal keyboard analog operation
    setKeypadAnalog(TRUE);          
    delay_ms(10);
    isTFMSInt = FALSE;
    isPwrBtnPressed = FALSE;
    lastRemoteChar = KEY_INVALID;
            
    if (!isContinuous)
    {
        if (isFreqValid)
        {
            // found new frequency, keep it
            debugPrint("FN=%u", newFreq);                
            radioCurFreq = newFreq;
        }
        else {
            // no frequency found, go to last search frequency
            SendUARTStr("NoFr");
            gotoRadioFreq(seek_up ? seekBottomFreq : seekTopFreq);
        }        
        si473x_setAudioMute(FALSE);
    }
        
    isRadioSeeking = FALSE;
    shouldUpdateRadioScreen = TRUE;    
    lastUpdateRadioSignal = 0;
    
    return (isFreqValid ? newFreq : RADIO_INVALID_FREQUENCY);
}

void scanPreviousStation()
{
    // wrap around if we reached end of bands
    if (radioCurBand == RADIO_CUR_BAND_SW)
    {
        unsigned int topSwFreq    = swBands[0];
        unsigned int bottomSwFreq = swBands[sizeof(swBands)/2 - 1];
        if (radioCurFreq <= topSwFreq)
        {
             gotoRadioFreq(bottomSwFreq);
        }
    }    
    else if (radioCurFreq <= radioMinFreq)
    {
        gotoRadioFreq(radioMaxFreq);
    }

    LCD_drawInfoText(EMPTY_INFO_LINE);
    LCD_updateDisplay();
    
    unsigned int totalFreqs = 0;
    BOOL isKeybCancelled = FALSE;
    seekStation(SEEK_DOWN, FALSE, FALSE, &totalFreqs, 0, &isKeybCancelled);
}

void scanNextStation()
{
    if (radioCurBand == RADIO_CUR_BAND_SW)
    {
        unsigned int topSwFreq    = swBands[0];
        unsigned int bottomSwFreq = swBands[sizeof(swBands)/2 - 1];
        if (radioCurFreq >= bottomSwFreq)
        {
             gotoRadioFreq(topSwFreq);
        }
    }    
    else if (radioCurFreq >= radioMaxFreq)
    {
         gotoRadioFreq(radioMinFreq);
    }
    
    LCD_drawInfoText(EMPTY_INFO_LINE);
    LCD_updateDisplay();
    
    unsigned int totalFreqs = 0;
    BOOL isKeybCancelled = FALSE;
    seekStation(SEEK_UP, FALSE, FALSE, &totalFreqs, 0, &isKeybCancelled);
}

void scanEntireBand(BOOL fullSwSearch)
{
    si473x_setAudioMute(TRUE);
 
    // starting frequency for the search
    unsigned int startFreq = 0;
    unsigned int maxTotalFreqs = 0;
    unsigned char ind;
    if (radioCurBand == RADIO_CUR_BAND_SW)
    {
        if (fullSwSearch)
        {
            startFreq = radioMinFreq;
            maxTotalFreqs = ((radioMaxFreq - radioMinFreq) / radioFreqStep) + 1;            
        }
        else {
            startFreq = swBands[0];              
            for (ind = 0; ind < sizeof(swBands) / 4; ind++)           // array of integer (2 bytes each), each band consumes 2 integer each (start-end freq);
            {
                unsigned int bStarts = swBands[ind*2];
                unsigned int bEnds = swBands[ind*2+1];                   
                
                maxTotalFreqs += ((bEnds - bStarts) / radioFreqStep) + 1;
            }
        }      
    }    
    else 
    {
        maxTotalFreqs = ((radioMaxFreq - radioMinFreq) / radioFreqStep) + 1;
        startFreq = radioMinFreq;        
    }
    gotoRadioFreq(startFreq);
    
    LCD_drawInfoText(EMPTY_INFO_LINE);
    LCD_drawItemTitle(EMPTY_TITLE_LINE);
    LCD_drawItemTitle("Searching");
    LCD_updateDisplay();
         
    // search until the number of searched frequencies are greater than the number of available frequencies for the band
    BOOL shouldStop = FALSE;
    unsigned int foundCount = 0;
    unsigned int foundFreq = RADIO_INVALID_FREQUENCY;
    unsigned int freqsSearched = 0; 
    unsigned int lastFoundFreq = RADIO_INVALID_FREQUENCY;
    isAutoScanningRadio = TRUE;
    while (!shouldStop)
    {
        BOOL isKeybCancelled = FALSE;   
        foundFreq = seekStation(SEEK_UP, fullSwSearch, TRUE, &freqsSearched, maxTotalFreqs, &isKeybCancelled);
        if (foundFreq != RADIO_INVALID_FREQUENCY)
        {
            unsigned int presetOffset = getEepromPresetOffset(); 
            writeRadioPresetFrequency(presetOffset, foundCount, foundFreq);
            
            if (radioCurBand == RADIO_CUR_BAND_FM)
            {
                sprintf(strResBuf, "%u.%02u MHz (#%u)", foundFreq / 100, foundFreq % 100, foundCount);
            }
            else {
                sprintf(strResBuf, "%u kHz (#%u)", foundFreq, foundCount);
            }
            
            SendUARTStr(strResBuf);            
            LCD_drawInfoText(EMPTY_MENU_LINE);
            LCD_drawInfoText(strResBuf);
            LCD_updateInfoText();
            foundCount++;
            
            ignorePartialRefreshFreq = TRUE;
            gotoRadioFreq(foundFreq);
            lastFoundFreq = foundFreq;
        }
        if (freqsSearched >= maxTotalFreqs || foundCount > EEPROM_MAX_BAND_PRESETS || isKeybCancelled)
        {
            shouldStop = TRUE;
        }        
        drawTopClock();
        debugPrint("SK:%u/%u", freqsSearched, maxTotalFreqs);
    }
    isAutoScanningRadio = FALSE;
    // SendUARTStr("SKE");
        
    if (lastFoundFreq != RADIO_INVALID_FREQUENCY)
    {
        gotoRadioFreq(lastFoundFreq);
    }
    else {
        gotoRadioFreq(startFreq);
    }
    
    si473x_setAudioMute(FALSE); 
    
    shouldUpdateRadioScreen = TRUE;
    updateRadioScreen(); 
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Main UI Routines">
void showPOSTProgress(unsigned char pct)
{
#define BAR_HEIGHT  3
    unsigned char width;
    
    // otherwise div by zero!
    if (pct == 0)
        pct = 5;    
    
    width = (unsigned int)(LCD_SCR_WIDTH * pct) / 100;
    if (width > LCD_SCR_WIDTH)
        width = LCD_SCR_WIDTH;
    
    LCD_clearRect(0, LCD_SCR_HEIGHT - BAR_HEIGHT, LCD_SCR_WIDTH, BAR_HEIGHT);
    LCD_drawRect(0, LCD_SCR_HEIGHT - BAR_HEIGHT, LCD_SCR_WIDTH, BAR_HEIGHT);
    LCD_fillRect(0, LCD_SCR_HEIGHT - BAR_HEIGHT, width, BAR_HEIGHT);
    LCD_updateDisplay();
}

// Use the Goertzel algorithm to calculate the magnitude of a given sample over a given data set
// Adapted to test the sound system of the board by calculating the magnitude of BEEP_FREQ (2kHz), samples taken from the ADC
// See also:
// https://stackoverflow.com/questions/11579367/implementation-of-goertzel-algorithm-in-c
// https://hackaday.io/project/16615-spectrum-analyser-code
// https://hackaday.io/project/16615-spectrum-analyser-code/log/48061-an-integer-version-of-goertzel-algorithm
unsigned int goertzel_mag()
{
    // simplified hard-coded calculations based on current constant value, otherwise usage of float functions
    // will cause const linker section to grow large and allocated outside x-memory, which will cause crash
    // 8000 samples per second, tiny beep last for 100ms, 
#define NUM_SAMPLES         800
        
    // 8000 samples per section, each sample is approximate 125ms
    // reduce value to cater for calculation time
    // adjust value a bit for highest return magnitude for the sampling frequency 
    // around 104-106, peak at 105 (for current code only)
#define SAMPLE_DELAY_US     104
    
    // float coeff = 2.0 * cos(2.0 * 3.14 * TARGET_FREQUENCY / SAMPLING_RATE);
    // TARGET_FREQUENCY is 2000, SAMPLING_RATE is 8000, so cos() is just 0!
#define COEFF           0.0
        
    float s_prev = 0.0;
    float s_prev2 = 0.0;
    float s = 0.0;
    
    // if numSamples is calculated correctly, this for will last approximately the duration of the beep
    unsigned int i;
    for (i=0; i < NUM_SAMPLES; i++) {
      s = readAnalogANPinSigned(RECORDER_INPUT_AN_PIN) + COEFF * s_prev - s_prev2;     // recording input on RA0
      s_prev2 = s_prev;
      s_prev = s;
            
      delay_us(SAMPLE_DELAY_US);
    }

    return (unsigned int) (2 * sqrt(s_prev2*s_prev2 + s_prev*s_prev - COEFF*s_prev*s_prev2) / ((float)NUM_SAMPLES));   
}

// Perform Power On Self Test (POST) routines and show result on LCD
// Return TRUE if the system can proceed, FALSE if system should be halted due to an unrecoverable error (disk/battery issues)
BOOL performPOSTTests()
{        
    char buf[16];
    
   unsigned int sysResultCode = 0;
   unsigned char testCount = 0;
                  
    BOOL isOK = FALSE;
           
    hasSysPosted = FALSE;
    
    // max volume level so that any POST beep can be heard
    // and also for the audio test relying on the POST beep
    volumeLevel = MAX_VOL_LVL - 1;
    
    LCD_cls();          
    LCD_setAutoUpdate(TRUE);
    
    // important, otherwise SD_CS will remain low and SD card will enter into corrupt state thinking there's bus activity
    SD_CS_HIGH; 
    
    showPOSTProgress(5);
        
    // enable timer for PCM playback, recorder or time measurements
    initTmr4();
    samplingRate = BEEP_TMR_FREQ;
    setTmr4Freq(samplingRate);    
    enableAudio(FALSE);                      
             
    if (curOperatingMode == OP_MODE_FASTEST)
    {
        // operating in fast mode (always the case)
        // tiny beep to indicate beginning of POST
        // beep is via PWM samples buffer
        // we beep with no delay and use the beep waveform to test the audio system
        tinyBeep(FALSE);
        
        // read analog recorder input, which should be the waveform of the beep 
        // for TINY_BEEP_LENGTH, e.g. 100ms
        isOK = TRUE;            
        
        // calibrated to match current recorder trimmer resistor
        // with some allowances
        // too low a magnitude value will indicate possible audio system issues
        unsigned int fMag = goertzel_mag();        
        if (fMag > AUDIO_SELF_TEST_DB_THRESHOLD)
        {
            isOK = TRUE;
            sprintf(strResBuf, "SND: OK (%u)", fMag);
        }
        else 
        {
            isOK = FALSE;
            sprintf(strResBuf, "SND: Err (%u)", fMag);
            
            sysResultCode = sysResultCode + (1 << testCount);
        }        
    }
    else {
        // operating in slow mode 
        // this never happens in current code as POST is always performed in FAST mode
        tinyBeep(TRUE);                
        isOK = TRUE;            
        strcpy(strResBuf, "SND: OK");       
    }    
    
    // display result of audio test
    SendUARTStr(strResBuf);
    LCD_showBootInfo(0, 0, strResBuf);
    
    // beep finish, disable otherwise radio test will cause overflow in output voltage, and oscillation
    enableAudio(FALSE);
    
    showPOSTProgress(10);
    
   // always beep at max volume, regardless of volume level
   // we will restore correct volume level later
   unsigned char curVolume = volumeLevel;
   volumeLevel = MAX_VOL_LVL - 1;
   
   // raw battery voltage, terminate if too low
   unsigned int rawBattVal = 0;
   float vin = getBattVoltAvg(&rawBattVal);   
   formatBatteryVoltage(rawBattVal, vin, 0, TRUE, FALSE);   
   
   // battery voltage
   LCD_showBootInfo(0, 1, strResBuf);       
   SendUARTStr(strResBuf);
   
    // battery too low, refuse to start   
   if (vin > INVALID_BATT_VOLTAGE && vin < LOW_BATT_VOLTAGE)
   {
       SendUARTStr("LwBt");
       
       drawFullScreenBitmap(noBatteryIcon);       
       longBeep(FALSE);
       
       return FALSE;
   }       
   
   showPOSTProgress(20);
           
    // TFMS infrared decoder module status
    // consider working if high (default state) until key is pressed
    BOOL isTFMSOK = TFMS_INP;
    
    // keyboard status
    // keyboard fed via analog input. Default should be 5V.
    // If less than 5V, means keyb stuck. If 0V means not present
    unsigned int keybAdc = getKeyboardADC();
    isKbAdcOK = (keybAdc > ANALOG_KEY_MAX_ADC);
    if (isTFMSOK && isKbAdcOK)
    {
       strcpy(strResBuf,   "KBD: OK");      
    }
    else if (!isTFMSOK && !isKbAdcOK)
    {
       strcpy(strResBuf,   "KBD: IR/AD Err");      
    }
    else if (!isTFMSOK) 
    {
       strcpy(strResBuf,   "KBD: IR Err");      
    }
    else if (!isKbAdcOK)
    {
        strcpy(strResBuf,  "KBD: AD Err");
    }    
    if (!isKbAdcOK || !isTFMSOK)
    {
        sysResultCode = sysResultCode + (1 << testCount);
    }    
    LCD_showBootInfo(1, 0, strResBuf);    
    SendUARTStr(strResBuf);    
    testCount++;      
   
    showPOSTProgress(30);
    
   // check if user pressed RESET
   // only if keyboard is OK 
   BOOL isReset = FALSE;
   unsigned char mappedKey = 0;
   
    // if user presses the RESET key now, config will be cleared
    // only show prompt if keyboard is OK
   // to prevent random input causing reset, key must be held for the duration of the message to trigger reset
    if (isKbAdcOK)
    {
        LCD_showBootInfo(9, 0, "PWR = Load Default"); 
        
        // delay a bit for user to press key
        delay_ms(1000);        
        mappedKey = getKeyboardInput();
        isReset = (mappedKey == KEY_RESET_POWER);        
    }
    else {
        LCD_showBootInfo(9, 0, "Booting ..."); 
    }
    
   showPOSTProgress(40);
   
   // RTC show at last line
   // should be last test, as oscillator takes a while to start!
   isRTCOK = rtcSelfTest();
   if (!isRTCOK)
   {
       sysResultCode = sysResultCode + (1 << testCount);   
       
       // RTC not running
       sprintf(strResBuf,     "Clock Error          ");  // cover previously written text
       LCD_showBootInfo(9, 0, strResBuf); 
       SendUARTStr(strResBuf);
   }
   testCount++;  
        
   // check if RESET is still held, only then will we reset
   if (isKbAdcOK && isReset)
   {       
        mappedKey = getKeyboardInput();
        isReset = (mappedKey == KEY_RESET_POWER);
   }   
   showPOSTProgress(50);
   
   // EEPROM self test
   unsigned int selfTestRes = eeprom_selfTest();
   isOK = FALSE;
   if (selfTestRes == EEPROM_SELF_TEST_OK)
   {
        // if self test OK and not resetting, load app config
        BOOL existConfig = FALSE;
        if (!isReset)
        {
            isOK = readAppConfig();            
            if (isOK)
            {
                forceMono = appConfigObj.config.forceMono;                                    
                curPowerType = appConfigObj.config.powerType;                    
                curIdleOffSetting = appConfigObj.config.idleOffMode;
                curAlwaysOffSetting = appConfigObj.config.alwaysOffMode;               
                disabledBeep = appConfigObj.config.disabledBeep;       
                alarmEnabled = appConfigObj.config.alarmEnabled;       
                alarmHour = appConfigObj.config.alarmHour;
                alarmMin = appConfigObj.config.alarmMinute;
                
                curMyLat1 = appConfigObj.config.myLat1;
                curMyLat2 = appConfigObj.config.myLat2;
                curMyLng1 = appConfigObj.config.myLng1;
                curMyLng2 = appConfigObj.config.myLng2;
                
                curTimeZone = appConfigObj.config.timeZone;
                            
                // app config read OK
                isPendingConfig = TRUE;

                if (
                        isValidDate(appConfigObj.config.curYear2k, appConfigObj.config.curMonth,appConfigObj.config.curDay) &&
                        isValidTime(appConfigObj.config.curHour, appConfigObj.config.curMinute, appConfigObj.config.curSecond) 
                   )
                {            
                     setRTC(appConfigObj.config.curHour, appConfigObj.config.curMinute, appConfigObj.config.curSecond, appConfigObj.config.curDay, appConfigObj.config.curMonth, appConfigObj.config.curYear2k);

                     // assuming PIC reboots, add some extra seconds to make up for RTC lost time
                     unsigned char c;
                     for (c = 0; c < 3; c++)
                     {
                         rtcAddSec();
                     }
                }

                // show checksum
                sprintf(strResBuf, "NVM: OK (%04X)", appConfigObj.config.crc16Checksum);   
                existConfig = TRUE;
            }
            else {
                // no existing config, use default
                writeAppConfig(SOURCE_SD);
                strcpy(strResBuf, "NVM: Err (CSUM)");       // must be after writeAppConfig, because it uses debugPrint which also uses strResBuf!
            }                    
        }
        else
        {
            // config reset by user
            writeAppConfig(SOURCE_SD);
            strcpy(strResBuf, "NVM: RESET");
        }
   }
   else
   {
       // app config read fails, use default
       writeAppConfig(SOURCE_SD);              
       sprintf(strResBuf, "NVM: Err (%04X)", selfTestRes);                 
   }   
   
   LCD_showBootInfo(1, 1, strResBuf);    
   SendUARTStr(strResBuf);
   if (!isOK)
   {
        sysResultCode = sysResultCode + (1 << testCount);   
   }
   testCount++;
   updatePowerSaveConfig();
      
   // RTC OK    
   if (isRTCOK) {
        unsigned char dow = getDayOfWeek(2000 + curYear2k, curMonth, curDay);    
        sprintf(strResBuf, "%s %02d-%02d-%04d %02d:%02d:%02d %c%02d:00", daysOfWeek[dow], curDay, curMonth, 2000 + curYear2k, curHour, curMinute, curSecond, 
                                                                         curTimeZone >= 0 ? '+' : '-', curTimeZone >= 0 ? curTimeZone : -curTimeZone);
        LCD_showBootInfo(9, 0, strResBuf); 
        SendUARTStr(strResBuf);
   }

   // Input Source
   inputSource = appConfigObj.config.inputSrc;              
   getInputSourceStr(inputSource, buf);
   sprintf(strResBuf, "SRC: %s", buf);
   LCD_showBootInfo(2, 0, strResBuf);    
   SendUARTStr(strResBuf);
   
   // SI4735 chipset test   
    radioCurFreq = appConfigObj.config.radioCurFreq;
    radioCurBand = appConfigObj.config.radioCurBand;
    radioFMRange = appConfigObj.config.radioFMRange;
    radioFMDeemp =  appConfigObj.config.radioFMDeemp;
    radioFMSpacing = appConfigObj.config.radioFMSpacing;
    radioAMSpacing = appConfigObj.config.radioAMSpacing;
    radioAMBW = appConfigObj.config.radioAMBW;
    radioSSBBW = appConfigObj.config.radioSSBBW;
    radioSSBMode = appConfigObj.config.radioSSBMode;
    radioSSBSync = appConfigObj.config.radioSSBSync;
    radioRecordingMode = RECORD_MODE_NONE;    
    
   isSI473XOk = FALSE;
   si473x_firmware_information fi = si473x_setup(radioCurBand == RADIO_CUR_BAND_FM ? POWER_UP_FM : POWER_UP_AM, &isSI473XOk);
   if (isSI473XOk)
   {       
       // I2C bus OK, set a frequency and read back
       // This will fail if crystal source for SI4735 is not present
       // Must delay long enough, otherwise the crystal has not yet started and frequency set will fail!
       delay_ms(2000);
       
       if (radioCurBand == RADIO_CUR_BAND_FM)
       {
           isSI473XOk = si473x_setFM(RADIO_FM_MIN_64MHz, RADIO_FM_MAX_MHz, RADIO_TEST_FM_FREQ, RADIO_FM_SPACING_100kHZ_VAL);
       }
       else {           
           si473x_setSSBMode(SSB_NONE); 
           isSI473XOk = si473x_setAM(RADIO_AM_MIN_9kHz, RADIO_AM_MAX_kHz, RADIO_TEST_AM_FREQ, RADIO_MW_SPACING_9kHZ_VAL);
       }
       
       if (isSI473XOk)
       {
            sprintf(strResBuf, "RDO: OK (%u.%u)", fi.resp.CMPMAJOR, fi.resp.CMPMINOR);   
       }
       else {
            sprintf(strResBuf, "RDO: Err (%u.%u)", fi.resp.CMPMAJOR, fi.resp.CMPMINOR);   
       }
       
       // wait for radio to settle after previous tuning request
       // otherwise it may fail when we tune to our real frequency
       delay_ms(500);
   }
   else {
       // I2C bus fails, cannot read radio version
       strcpy(strResBuf,  "RDO: Err (I2C)");   
   }
   LCD_showBootInfo(2, 1, strResBuf);    
   SendUARTStr(strResBuf);
   if (!isSI473XOk)
   {
        sysResultCode = sysResultCode + (1 << testCount);   
   }
   testCount++;
   
   showPOSTProgress(60);
        
    // App Info
   sprintf(strResBuf, "VER: "APP_VERSION_STRING);
   LCD_showBootInfo(3, 0, strResBuf);    
   SendUARTStr(strResBuf);   
   
   showPOSTProgress(70);
   
    // temperature sensor info    
    isTempSensorOK = readTempHumid(&curTemperature, &curHumidity, TRUE);
    if (isTempSensorOK)
    {
        sprintf(strResBuf, "DHT: %d%cC (%d%%)", curTemperature, 127, curHumidity);
    }
    else {
        strcpy(strResBuf, "DHT: Err");
    }
    if (!isTempSensorOK)
    {       
        sysResultCode = sysResultCode + (1 << testCount);       
    }
    SendUARTStr(strResBuf);
    LCD_showBootInfo(3, 1, strResBuf);
    testCount++;   
   
   showPOSTProgress(80);
   
   // enable SPI2 for CH376 USB controller
   SPI2STATbits.SPIEN = 1;                      
   
   // default input src is SD card if invalid
   if (inputSource != SOURCE_USB && inputSource != SOURCE_SD && inputSource < SOURCE_RADIO)
       inputSource = SOURCE_SD;
   // debugPrint("InputSrc: %u", inputSource);    
   
    unsigned int  _sdSelectedFolder = appConfigObj.config.sdSelectedFolder;
    unsigned int  _sdSelectedFile = appConfigObj.config.sdSelectedFile;
    unsigned int  _sdFileDuration = appConfigObj.config.sdFileDuration;
    unsigned int  _usbSelectedFolder = appConfigObj.config.usbSelectedFolder;
    unsigned int  _usbSelectedFile = appConfigObj.config.usbSelectedFile;
    unsigned int  _usbFileDuration = appConfigObj.config.usbFileDuration;
        
   // Test both USB and SD cards
   // If input source is USB, we'll test SD card first.
   // If input source is SD, we'll test USB first.
   // This way, we will end up with the active input source
   // If FM, we'll still test both sources
   INPUT_SOURCE testSrc = (inputSource == SOURCE_USB ? SOURCE_SD : SOURCE_USB);    
   BOOL isTestingUSB = (testSrc == SOURCE_USB); 
   unsigned char c;
   for (c = 0; c < 2; c++)
   {
        // debugPrint("Test: %s", isTestingUSB ? "USB" : "SD");
        setFatFsIsUsb(isTestingUSB);

        unsigned char count = 0;
        BOOL isFatFSOk = FALSE;
        do
        {
         isFatFSOk = initFatFS();
         count++;
        }
        while (!isFatFSOk && count < 5);

        if (diskSizeMB > 0)
        {        
             // Disk OK
             sprintf(strResBuf,       isTestingUSB ? "USB: %uMB" : "SD : %uMB", diskSizeMB);        
             LCD_showBootInfo(4, c, strResBuf); 
             SendUARTStr(strResBuf);
             
             isOK = TRUE;
        }
        else 
        {
            // Disk Error
            if (isTestingUSB)
            {
                // USB Error
                if (!ch376_getChipStatus())
                {
                     // CH376 Error
                     LCD_showBootInfo(4, c, "USB: 376 Err");    
                     isOK = FALSE;
                }
                 else if (!ch376_getUSBConnectedStatus())
                {
                     // USB not connected, not an error
                     LCD_showBootInfo(4, c, "USB: -");    
                     isOK = TRUE;
                }           
                else if (!ch376_getUSBMountStatus())
                {
                     // USB connected but can't be mounted
                     LCD_showBootInfo(4, c, "USB: FMT Err");    
                     isOK = FALSE;
                }           
                else 
                {
                     // USB mounted but FATFS fails
                     LCD_showBootInfo(4, c, "USB: FS Err");       
                     isOK = FALSE;
                }
            }
            else {               
                // consider error if SD init OK but cannot be mounted
                // otherwise SD is not present, not an error
                isOK = !is_sd_init_ok();
                
                sprintf(strResBuf, isOK ? "SD : -" : "SD : FS Err");           
                LCD_showBootInfo(4, c, strResBuf); 
                SendUARTStr(strResBuf);
            }
        }
        if (!isOK)
        {
             sysResultCode = sysResultCode + (1 << testCount);   
        }
        testCount++;  
        
        // if at first input, prepare FatFS to switch to the other input
        if (c == 0)
        {
            isTestingUSB = !isTestingUSB;
                    
            ch376_clearvars();                    
            resetFatFS();   
            
            showPOSTProgress(80);
        }
   }
   showPOSTProgress(90);
   
   // current long/lat as configured by user
   sprintf(strResBuf, "LAT: %d.%02d", curMyLat1, curMyLat2);                  
   SendUARTStr(strResBuf);
   LCD_showBootInfo(5, 0, strResBuf); 
   sprintf(strResBuf, "LNG: %d.%02d", curMyLng1, curMyLng2);                  
   SendUARTStr(strResBuf);
   LCD_showBootInfo(5, 1, strResBuf); 
   
   // CPU clock speed
   sprintf(strResBuf, "CLK: %u MHz", (unsigned int)((inputSource == SOURCE_RADIO ? FOSC_LOW : FOSC_HIGH) / 1000000L));                  
   SendUARTStr(strResBuf);
   LCD_showBootInfo(6, 0, strResBuf); 
   
   // UART speed
   sprintf(strResBuf, "SER: %ld", UART1_SPEED);                  
   SendUARTStr(strResBuf);
   LCD_showBootInfo(6, 1, strResBuf); 
   
   // file positions in radio mode
    if (inputSource >= SOURCE_RADIO)
    {        
        curFileIndex = _sdSelectedFile;
        curFolderIndex = _sdSelectedFolder;
        prevDuration = _sdFileDuration;

        bkFileIndex = _usbSelectedFile;
        bkFolderIndex = _usbSelectedFolder;
        bkPrevDuration = _usbFileDuration;
        
        // debugPrint("ID: %u %u %u, USB: %u %u %u", curFileIndex, curFolderIndex, prevDuration, bkFileIndex, bkFolderIndex, bkPrevDuration);                
    }
      
   // disable CH375 and unused SPIs if not needed to save powers
   if (inputSource != SOURCE_USB)
   {        
        ch376_reset();                      // Must reset first, otherwise sleep has no effect
        delay_ms(1);                        // Wait a bit before sleeping
        ch376_sleep();
        delay_ms(1);                        // Wait until all SPI transmission (hardware) is done before disabling SPI
        SPI2STATbits.SPIEN = 0;
   }      
   if (inputSource != SOURCE_SD)
   {
        delay_ms(1);
        SD_CS_HIGH;
        SPI1STATbits.SPIEN = 0;
   }
   
   // unless on radio, beep if disk has no supported files
   BOOL isDiskEmpty = FALSE;   
   if (inputSource < SOURCE_RADIO)
   {         
        isDiskEmpty = (numOfFilesInFolder == 0 && numOfFoldersInRoot == 0);
        if (isDiskEmpty)
        {
             sysResultCode = sysResultCode + (1 << testCount);
        }
        testCount++;           
   }
   
   // POST done
   showPOSTProgress(100);   
   if (sysResultCode != 0) {
       sprintf(strResBuf, "ERROR %04X                    ", sysResultCode);
   }
   else {
       strcpy(strResBuf, "Starting ...                   ");
   }
   LCD_showBootInfo(9, 0, strResBuf);    
   SendUARTStr(strResBuf);
       
   // singe BEEP if all POST tests passed
   // long beep to indicate error   
   enableAudio(TRUE);
   unsigned int ind = 0;
   if (sysResultCode == 0)
   {
       shortBeep();
   }
   else {      
       // for each test, short BEEP if OK, long beep if failed.
       for (ind = 0; ind < testCount; ind++)
       {
           if (sysResultCode & (1 << ind))
           {
               longBeep();
           }
           else 
           {
               shortBeep();
           }           
       }
   }    
   volumeLevel = curVolume;
   LCD_setAutoUpdate(FALSE);
   
   // long beep to indicate CFG reset
   if (isReset)
   {
       LCD_showBootInfo(9, 0, "CONFIG RESET        "); 
        LCD_updateDisplay();
           
       longBeep();
       delay_ms(1000);
   }
   
   enableAudio(FALSE);
   
   if (inputSource >= SOURCE_RADIO)
   {       
       // if choosing radio, hangs at boot if SI issue1s
       if (isSI473XOk)
       {
            updateRadioConfig();
            
            if (radioSSBMode != SSB_NONE)
            {
                radioSetAmOrSsbMode(TRUE);
            }
            
            // go to previous selected frequency
            delay_ms(100);
            hasRadioBandChanged = TRUE;
            ignorePartialRefreshFreq = TRUE;
            isSI473XOk = gotoRadioFreq(radioCurFreq);
            if (isSI473XOk)
            {
                 // disable PWM, we won't need it except during beeping
                 enableAudio(FALSE);                    

                 samplingRate = RECORDING_TIMER_FREQ;
                 setTmr4Freq(samplingRate);
                 
                 // no PAUSE in radio mode, mute only
                 isPaused = FALSE;
                 isFilePlaying = TRUE;
                                                 
                 if (isPaused)
                 {
                     stopRadio();
                 }
                 else 
                 {
                    debugPrint("vl%u", appConfigObj.config.curVolumeLevel);
                     
                    setRadioVolume(appConfigObj.config.curVolumeLevel);
                 }
                 
                 // debugPrint("%u %u", isFilePlaying, isPaused);

                  // On radio, run at 8MHz to save power unless recording
                 setOperatingMode(inputSource == SOURCE_RADIO ? OP_MODE_LOW_SPEED : OP_MODE_FASTEST);  
                 
                 lastUpdateRadioSignal = 0;
                 radioSSBBfoOffset = 0;
                 isRadioSeeking = FALSE;
                 isRadioFineTune = FALSE;
                 justChangedBfoOffset = FALSE;
            }
       }
       
       // fail if radio is not OK
       if (!isSI473XOk)
       {
           LCD_showBootInfo(9, 0, "RADIO ERROR         "); 
           LCD_updateDisplay();
           
           longBeep();
           delay_ms(100);
           longBeep();
           
           drawFullScreenBitmap(radioErrorIcon); 
           return FALSE;
       }       
   }
   else
   {
       // non radio mode, turn off RADIO to save power
       if (isSI473XOk)
       {
           si473x_radioPowerDown();
       }
       
        BOOL isDskErr = (diskSizeMB == 0 || isDiskEmpty);;
        if (isDskErr)
        {
         LCD_showBootInfo(9, 0, "DISK ERROR          ");   
         LCD_updateDisplay();
         
           longBeep();
           delay_ms(100);
           longBeep();
        }
        
        // if choosing SD/USB, hangs at boot if disk issues
        if (diskSizeMB == 0)
        {      
            drawFullScreenBitmap( (inputSource == SOURCE_USB) ? usbErrorIcon : sdErrorIcon); 
            return FALSE;
        }
        else if (isDiskEmpty)
        {   
            drawFullScreenBitmap(noFilesIcon); 
            return FALSE;
        }
   }
   
   // timer 3 for TFMS decoder
   initTmr3();    
    
   hasSysPosted = TRUE;
   
   return TRUE;
}

// update screen display, only one thing at a time to save precious CPU cycles!
void updateMediaScreen()
{
    char infoBuf[32];
   
    BOOL isScrUpdated = FALSE;
    BOOL hasUpdatedInfo = FALSE;
   
    BOOL isTotalDuration = (curDurationMode == DURATION_MODE_TOTAL);
    unsigned int curDuration = samplingRate > 0 ? outPcmSamples / samplingRate : 0;
        
    // update status only if there are changes
    BOOL isThisPlaying = isPlayingMP3 ? (mp3PlaybackState != MP3_IDLE_STATE) : (wavPlaybackState != WAV_IDLE_STATE);
    if (
        (isThisPlaying != isFilePlaying) || 
        (shouldUpdateStatus && curDuration % 15 == 0)
       )
    {        
        // SendUARTStr("status");
        
        isFilePlaying = isThisPlaying;        
        drawStatusBar();
        
        shouldUpdateStatus = FALSE;
        isScrUpdated = TRUE;
    }
    
    // update clock every minute
    if (shouldDrawClock)
    {
        drawTopClock();      
        LCD_updateTopMiddleInfo();
        
        shouldDrawClock = FALSE;        
    }
    
    if (curDuration != prevDuration)
    {                            
        // update only the portion being modified, not entire screen, to save CPU time
        if (!isTotalDuration || shouldUpdateDuration)
        {
            drawSongTime();
            shouldUpdateDuration = FALSE;
            
            LCD_updateRightStatusText();
        }
                        
        prevDuration = curDuration;
                             
        // 03 Feb 2021 - no need to toggle between displayed info, waste CPU power
        // BOOL shouldDrawInfo = FALSE;
        // unsigned char updateKey = (curDuration / 7) % 5;
        // if (isTotalDuration)
        // {
            // if duration display is total, show file info only once
            // if (curDuration == 3)
            // {
            //    updateKey = 3;
            //     shouldDrawInfo = TRUE;
            // }            
        // }
        // else {
            // update artist and other information display (if available) every once in a while
            // if duration display is not total
            // shouldDrawInfo = (curDuration == 1 || curDuration % 7 == 0);            
        // }
        
        if (shouldDrawMediaInfo)        
        {            
            shouldDrawMediaInfo = FALSE;
            
            strcpy(infoBuf, "");
            if (shouldSetRTC)
            {
                // prompt user to set clock once if needed
                strcpy(infoBuf, CLOCK_SET_STR);
                shouldSetRTC = FALSE;
            }
            else
            {
                // else MP3 file info
                switch (mediaInfoKeyIndex % MEDIA_INFO_KEY_COUNT)
                {
                    case 0:
                        if (isPlayingMP3)
                        {
                            if (isVBRFile)
                            {
                                strcpy(strResBuf, "VBR");
                            }
                            else {
                                sprintf(strResBuf, "%ukbps", (unsigned int)(mp3BitRate / 1000));
                            }

                            sprintf(infoBuf, "%s %ukHz %s", strResBuf, (unsigned int)(samplingRate / 1000), isPlayStereo ? "Stereo" : "Mono");
                        }
                        else {
                            sprintf(infoBuf, "%ukHz %s-bit %s", (unsigned int)(samplingRate / 1000), is16bitPCM ? "16" : "8", isPlayStereo ? "Stereo" : "Mono");
                        }
                        break;
                        
                    case 1:
                        if (strlen(mp3info.artist) > 0)
                        {
                            strcpy(infoBuf, mp3info.artist);
                        }
                        else {
                            strcpy(infoBuf, "Unknown Artist");
                        }
                        break;

                    case 2:
                        if (strlen(mp3info.album) > 0)
                        {
                            strcpy(infoBuf, mp3info.album);
                        }
                        else {
                            strcpy(infoBuf, "Untitled Album");
                        }
                        break;

                    case 3:                    
                        // special processing for year, which has exactly 4 digits and is always full (e.g. 2004) with no terminating character
                        // we can't use our trimStringToLastSpace which risks losing the last character
                        memcpy(strResBuf, mp3info.year, MAX_ID3_YEAR_LEN);
                        strResBuf[MAX_ID3_YEAR_LEN] = 0;                    
                        trimStringToLastSpace(strResBuf, MAX_ID3_YEAR_LEN + 1);        
                        if (strlen(strResBuf) > 0)
                        {
                            strcpy(infoBuf, strResBuf);
                        }
                        else {
                            // no year, show file size
                            sprintf(infoBuf, "%u KB", (unsigned int)(mediaFileSize / 1024));
                        }
                        break;

                    case 4:
                        if (strlen(mp3info.comment) > 0)
                        {
                            strcpy(infoBuf, mp3info.comment);                             
                        }
                        else {
                            // no comment, show file format
                            sprintf(infoBuf, isPlayingMP3 ? "MP3 File" : "WAV File");
                        }
                        break;
                }
            }

            unsigned char infoBufLen = strlen(infoBuf);

            if (infoBufLen > 0)
            {
                unsigned char len = MIN(infoBufLen, MAX_TITLE_CHAR_SMALL);

                if (len < lastInfoLen)
                {
                    // clear previous line only if shorter length
                    LCD_drawInfoText(EMPTY_INFO_LINE);  
                }

                LCD_drawInfoText2(infoBuf, 0, len); 

                lastInfoLen = len;

                hasUpdatedInfo = TRUE;
                isScrUpdated = TRUE;
            }
        }
        
        // 03 Feb 20201 - no need to scroll title, waste battery
        /*
        if (shouldUpdateTitle && !hasUpdatedInfo && !isTotalDuration)
        {
            drawTitle();
            isScrUpdated = TRUE;
        }
        */
        
        // update weather and battery info 30-min into the playback for long tracks
        // do not use exact multiple otherwise this will be called at 0 second!
        if (curDuration % 1801 == 0)
        {
            drawBattery(TRUE);
            
            // only do so if there is no remote digits entered
            // otherwise refreshing weather will clear remote 
            if (remoteDigitsPressedCount == 0)    
                displayWeatherInfo(TRUE, TRUE);
            else 
                drawRemoteInput();
            
            isScrUpdated = TRUE;
        }
    }
        
    if (isScrUpdated)
    {
        LCD_updateDisplay();
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="File Format">
BOOL isMP3File(char filename[])
{
    char* ext = strrchr(filename, '.');
    if (ext != NULL)
    {
        if (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".MP3") == 0)
        {            
            return TRUE;
        }
    }
    return FALSE;
}

BOOL isWavFile(char filename[])
{
    char* ext = strrchr(filename, '.');
    if (ext != NULL)
    {
        if (strcmp(ext, ".wav") == 0 || strcmp(ext, ".WAV") == 0)
        {            
            return TRUE;
        }
    }
    return FALSE;
}

BOOL isSupportedFile(char filename[])
{
    return isMP3File(filename) || isWavFile(filename);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Media Playback">
void playMP3file(void)
{                         
    isPlayingMP3 = TRUE;
    switch(mp3PlaybackState)
    {
        case(MP3_OPENING):
        {
            SendUARTStr(fullAudioFilename);
            
            ID3v2size = getID3v2Size();
            // debugPrint("ID3v2 size: %lu", ID3v2size);
            
            reaID3v1Tags();
            
            LCD_cls();
            
            // printCurrentDate();
                    
            displayWeatherInfo(FALSE, TRUE);
            drawTitle();                        
            drawBattery(TRUE);
            drawTopClock(); 
            drawInputSource();
            
            // drawVolume();
            drawFolderName();    
        
            remoteDigitsPressedCount = 0;
            samplingRate = 0;
            mediaInfoKeyIndex = 0;
            mp3BitRate = 0;
            prevDuration = 0;
            curTitlePos = 0;
            lastInfoLen = 0;
            totalDuration = 0;
            isFilePlaying = FALSE;
            isPaused = FALSE;
            isFilePlayedOK = FALSE;
            shouldDrawMediaInfo = TRUE;
            
            hasShowInfo = FALSE;
            shouldUpdateTitle = TRUE;
            outPcmSamples = 0;             
            totalDecErrors = 0;
            shouldUpdateDuration = TRUE;
            isVBRFile = FALSE;
            
            enableAudio(TRUE);
            
            FRESULT res = f_open(&fMediaFile, fullAudioFilename, FA_READ);
            if (res == FR_OK)
            {
                if (ID3v2size > 0)
                {
                    res = f_lseek(&fMediaFile, ID3v2size);
                    if (res == FR_OK)
                    {
                        SendUARTStr("ID2Sk");       
                    }
                    else {
                        SendUARTStr("ID2SEr");       
                    }
                }
                
                // SendUARTStr("Reading MP3");
                buffer_desc.start  = (U08*)&dataBuf.mp3_buf[0];
                
                asyncStatus = ASYNC_DEFAULT;

                mad_decoder_init(&decoder_desc,
                    &buffer_desc,
                    mp3_get,           /* input mp3 data */
                    NULL,              /* header */
                    NULL,              /* filter */
                    pcm_put,           /* output pcm data */
                    user_error,        /* error manage function */
                    NULL               /* message - <<not used ???>> */
                   ); 
                
                MAD_RETVAL ret = mad_run_async_begin(&decoder_desc);
                if (ret != RET_OK)
                {
                    SendUARTStr("MADDcEr");
                    mp3PlaybackState = MP3_FILE_CLOSE;
                }
                else {
                    is16bitPCM = TRUE;
                    
                    mp3PlaybackState = MP3_DECODE;
                }
            }
            else {
                SendUARTStr("MP3OpEr");
                mp3PlaybackState = MP3_IDLE_STATE;
            }
            
            LCD_updateDisplay();
        }
        break;
                     
        case (MP3_DECODE):
        {
            BOOL needData = (asyncStatus == ASYNC_CONTINUE_NEED_DATA || asyncStatus == ASYNC_DEFAULT);
            asyncStatus =  mad_run_async_step(&decoder_desc, needData);
            if ( (asyncStatus == ASYNC_CONTINUE_BUFFER || asyncStatus == ASYNC_CONTINUE_NEED_DATA) && (totalDecErrors < MAX_TOTAL_DEC_ERRORS) )
            {
                totalDecErrors = 0;
                mp3PlaybackState = MP3_WAIT_PLAYBACK;            
            }
            else
                mp3PlaybackState = MP3_FILE_CLOSE;                
        }
        break;        
                
        case(MP3_WAIT_PLAYBACK):
        {            
            if (samplingRate > MAX_SUPPORTED_FREQ || mp3BitRate > MAX_SUPPORTED_BITRATE)
            {
                SendUARTStr("RtEr");
                mp3PlaybackState = MP3_FILE_CLOSE;
                isFilePlayedOK = FALSE;                
            }
            else {                               
                isFilePlayedOK = TRUE;                                
                if (isPaused)
                {
                    mp3PlaybackState = MP3_WAIT_PLAYBACK;
                }
                else if ( (queue_get_len_left() < BUFFER_TOPUP_THRESHOLD) || (queue_get_len_right() < BUFFER_TOPUP_THRESHOLD) )
                {
                    mp3PlaybackState = MP3_DECODE;
                }
                else {
                    mp3PlaybackState = MP3_WAIT_PLAYBACK;
                }
            }
        }
        break;
        
        case(MP3_FILE_CLOSE):
        {            
            MAD_RETVAL ret = mad_run_async_end(&decoder_desc);
            if (ret != RET_OK)
            {
                SendUARTStr("MADClE1");
            }
            
            FRESULT res = f_close(&fMediaFile);
            if (res == FR_OK)
            {
                // SendUARTStr("MP3 File Closed");
            }
            else {
                SendUARTStr("MP3ClE2");
            }
            
            // debugPrint("pcmLength=%lu", pcmLength);            
            mp3PlaybackState = MP3_IDLE_STATE;
            
            shouldUpdateTitle = FALSE;            
            
            isFilePlaying = FALSE;
            drawStatusBar();
            
            if (!isUsingRemote)
            {            
                processPlayMode();
            }
            LCD_updateDisplay();            
        }
        break;
        
        case(MP3_IDLE_STATE):
        {
           // SendUARTStr("MP3_IDLE_STATE");
        }
        break;
                        
        default:
        {
            // SendUARTStr("MP3_INVALID_STATE");
        }
        break;
    }
}

void playWavFile()
{        
    resetMP3Tag();
    
    isPlayingMP3 = FALSE;
    switch (wavPlaybackState)
    {
        case WAV_OPENING:
        {
            SendUARTStr(fullAudioFilename);
            
            shouldUpdateTitle = TRUE;
            
            enableAudio(TRUE);
            
            LCD_cls();
            
            displayWeatherInfo(FALSE, TRUE);
            drawTitle();     
            drawBattery(TRUE);
            drawTopClock(); 
            drawInputSource();
            
            // printCurrentDate();
            
            // drawVolume();
            drawFolderName();
            
            remoteDigitsPressedCount = 0;
            samplingRate = 0;
            prevDuration = 0;
            mediaInfoKeyIndex = 0;
            curTitlePos = 0;
            lastInfoLen = 0;
            totalDuration = 0;
            isFilePlaying = FALSE;
            isPaused = FALSE;
            outPcmSamples = 0;                                     
            isFilePlayedOK = FALSE;
            shouldUpdateDuration = TRUE;
            isVBRFile = FALSE;
            shouldDrawMediaInfo = TRUE;
            
            FRESULT res = f_open(&fMediaFile, fullAudioFilename, FA_READ);
            if (res != FR_OK)
            {
                SendUARTStr("WvOpEr");
                wavPlaybackState = WAV_IDLE_STATE;                
            }
            else {                            
                mediaFileSize = f_size(&fMediaFile);
                // debugPrint("Wav Size: %ld", mediaFileSize);
        
                int headerSize = sizeof(wavHeader);
                
                unsigned int bytesRead = 0;
                FRESULT res = f_read(&fMediaFile, &wavHeader, headerSize, &bytesRead);

                if (res != FR_OK || bytesRead == 0)
                {
                    SendUARTStr("WvHdRdEr");
                    wavPlaybackState = WAV_FILE_CLOSE;   
                }
                else {
                     if (wavHeader.riff[0] != 'R' || wavHeader.riff[1] != 'I' || wavHeader.riff[2] != 'F' || wavHeader.riff[3] != 'F')
                     {
                        SendUARTStr("WvHdEr");    
                        wavPlaybackState = WAV_FILE_CLOSE;   
                     }
                     else if (wavHeader.format_type == 3)  {
                        SendUARTStr("IEWvEr");   
                        wavPlaybackState = WAV_FILE_CLOSE;   
                     }
                     else if (wavHeader.sample_rate > MAX_SUPPORTED_FREQ)
                     {
                        SendUARTStr("SRtEr");
                        wavPlaybackState = WAV_FILE_CLOSE;
                     }
                     else {	                    	                                                                                                
                        totalDuration = mediaFileSize / wavHeader.sample_rate / wavHeader.channels / (wavHeader.bits_per_sample / 8);
                        
                        debugPrint("%d %d %u %d %u", wavHeader.format_type, wavHeader.channels, (unsigned int)wavHeader.sample_rate, wavHeader.bits_per_sample, totalDuration);
                        
                        wavPlaybackState = WAV_PLAYBACK;   
                        
                        wavBytesPerSample = (wavHeader.bits_per_sample / 8);
                        is16bitPCM = (wavBytesPerSample > 1);
                        isPlayStereo = (wavHeader.channels > 1);

                        samplingRate = wavHeader.sample_rate;
                        setTmr4Freq(samplingRate);
                     }
                }
            }
            
            LCD_updateDisplay();
        }
            break;
            
        case WAV_PLAYBACK:
        {	                   
            int i;
            unsigned int bytesRead = 0;
            unsigned int elementsRead = 0;
            FRESULT res = FR_OK;
                                
            if (is16bitPCM)
            {
                res = f_read(&fMediaFile, dataBuf.dataBuffer16, WAV_FILE_READ_BUFFER_SIZE*2, &bytesRead);
            }
            else {
                res = f_read(&fMediaFile, dataBuf.dataBuffer8, WAV_FILE_READ_BUFFER_SIZE, &bytesRead);
            }
                 
            // debugPrint("WAV read: %d bytes", bytesRead);
			if (bytesRead > 0) 
			{				
                elementsRead = is16bitPCM ? bytesRead / 2 : bytesRead;
                
                i = 0;
				while (i<elementsRead)
				{		
                    if (is16bitPCM)
                    {
                        if (isPlayStereo)
                        {
                            queue_put_left(dataBuf.dataBuffer16[i]);
                            queue_put_right(dataBuf.dataBuffer16[i+1]);
                        }
                        else {
                            queue_put_left(dataBuf.dataBuffer16[i]);
                            queue_put_right(dataBuf.dataBuffer16[i]);
                        }
                    }
                    else {
                        if (isPlayStereo)
                        {
                            queue_put_left(dataBuf.dataBuffer8[i]);
                            queue_put_right(dataBuf.dataBuffer8[i+1]);
                        }
                        else
                        {
                            queue_put_left(dataBuf.dataBuffer8[i]);
                            queue_put_right(dataBuf.dataBuffer8[i]);
                        }        
                    }
                                        
                    if (isPlayStereo) 
                        i += 2;
                    else 
                        i++;
				}
                
                wavPlaybackState = WAV_WAIT_PLAYBACK;
			}
            else {
                // SendUARTStr("Wav Ends");                                                     
                wavPlaybackState = WAV_FILE_CLOSE;
            }            
        }
            break;
        
        case WAV_WAIT_PLAYBACK:
        {
            isFilePlayedOK = TRUE;
            
            // SendUARTStr("WAV_WAIT_PLAYBACK");               
            if (isPaused)
            {
                wavPlaybackState = WAV_WAIT_PLAYBACK;
            }
            else if ( (queue_get_len_left() < BUFFER_TOPUP_THRESHOLD) || (queue_get_len_right() < BUFFER_TOPUP_THRESHOLD) )
            {		      	      
                wavPlaybackState = WAV_PLAYBACK;
            }
            else {
                wavPlaybackState = WAV_WAIT_PLAYBACK;
            }                        
        }
            
            break;
        
        case WAV_FILE_CLOSE:
        {            
           FRESULT res = f_close(&fMediaFile);
           if (res != FR_OK)
           {
              SendUARTStr("WvClEr");
           }

           shouldUpdateTitle = FALSE;            
           
           isFilePlaying = FALSE;
           drawStatusBar();    
           
           wavPlaybackState = WAV_IDLE_STATE;
                      
           if (!isUsingRemote)
           {
                processPlayMode();
           }
           
           LCD_updateDisplay();
        }
        
            break;
            
        case WAV_IDLE_STATE:
            // SendUARTStr("WAV_IDLE_STATE");
            break;
            
        default:
            // SendUARTStr("WAV_INVALID_STATE");
            break;
    }
}		


void processPlayMode()
{    
    switch(curPlayMode)
    {
        case PLAY_MODE_REPEAT_TRACK:
        {
            if (isFilePlayedOK)
            {
                // play same track again, only if track was OK            
                openFile(curFileIndex);    
            }
            else {
                // if track could not be played, continue processing subsequent files
                if (isPlayGoingNext)
                {
                    gotoNextFile();
                }
                else {
                    gotoPreviousFile();
                }
            }                                        
        }
        break;
        
        case PLAY_MODE_REPEAT_FOLDER:
        {
            if (isFilePlayedOK && curFileIndex == numOfFilesInFolder - 1)
            {
                // if the last track was ok, play first track within current folder if reach end of folder                            
                gotoFirstFileInFolder();
            }
            else {
                // if track could not be played, continue processing subsequent files
                if (isPlayGoingNext)
                {
                    gotoNextFile();
                }
                else {
                    gotoPreviousFile();
                }
            }
        }
        break;
        
        case PLAY_MODE_SHUFFLE_FOLDER:
        {
            if (numOfFilesInFolder > 1)
            {
                // if folder has more than 1 files, go to random file within current folder
                if (!hasInitRand)
                {
                    initRand();
                    hasInitRand = TRUE;
                }
                curFileIndex = getRandom(numOfFilesInFolder);
                openFile(curFileIndex);
            }
            else {
                // if track could not be played, continue processing subsequent files
                if (isPlayGoingNext)
                {
                    gotoNextFile();
                }
                else {
                    gotoPreviousFile();
                }
            }  
        }
        break;
        
        case PLAY_MODE_SHUFFLE_ALL:
        {
            if (!hasInitRand)
            {
                initRand();
                hasInitRand = TRUE;
            }
            
            // randomize new folder and open it if different from current folder
            unsigned char newFolderInd = getRandom(numOfFoldersInRoot);
            if (newFolderInd != curFolderIndex)
            {
                curFolderIndex = newFolderInd;
                openFolder(curFolderIndex);
            }
            
            // with the opened folder (current or new)
            if (numOfFilesInFolder > 1)
            {
                // if folder has more than 1 files, go to random file within current folder
                curFileIndex = getRandom(numOfFilesInFolder);
                openFile(curFileIndex);
            }
            else {
                // if track could not be played, continue processing subsequent files
                if (isPlayGoingNext)
                {
                    gotoNextFile();
                }
                else {
                    gotoPreviousFile();
                }
            }  
        }
        break;
                
        case PLAY_MODE_ALL_TRACKS:
        {
            // if track could not be played, continue processing subsequent files
            if (isPlayGoingNext)
            {
                gotoNextFile();
            }
            else {
                gotoPreviousFile();
            }
        }      
        break;
            
        default:
        {   
            // single track only
            // no thing to do, wait until user select a track
        }
        break;  
    }
}

void processMediaFile()
{
    BOOL isMP3 = FALSE;
    if (isFilePlaying)
    {
        isMP3 = isPlayingMP3;
    }
    else {       
        isMP3 = isMP3File(fullAudioFilename);
    }
    
    if (isMP3)
    {
        playMP3file();
    }
    else
    {   
        playWavFile();
    }
}        
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="File Folder Navigation">
void getFileTime(WORD fdate, WORD ftime, unsigned int *year, unsigned char *month, unsigned char *day, unsigned char *hour, unsigned char *minute, unsigned char *second)
{
    *year   = (fdate >> 9) + 1980;
    *month  = (fdate & 0b0000000111100000) >> 5;
    *day    = (fdate & 0b0000000000011111);
    *hour   = (ftime >> 11);
    *minute = (ftime & 0b0000011111100000) >> 5;
    *second = (ftime & 0b0000000000011111) * 2;
}
void openFolderName(char folderName[], BOOL isRoot)
{    
    if (isRoot)
    {
        numOfFoldersInRoot = 0;
    }
    numOfFilesInFolder = 0;
        
    debugPrint("D:%s.Rt:%d", folderName, isRoot);
    
    memset(&dir, 0, sizeof(dir));
    
    FILINFO fno;
    FRESULT res = f_opendir(&dir, folderName);
    if (res == FR_OK) {
        // SendUARTStr("Dir Open OK");
        
        while (1) {
            res = f_readdir(&dir, &fno);                   
            // TODO: might need to check altname as well if fname is not present (e.g. no long filename?)
            if (res != FR_OK || fno.fname[0] == 0) 
                break;  

            unsigned int  year   = 0;
            unsigned char month  = 0;
            unsigned char day    = 0;
            unsigned char hour   = 0;
            unsigned char minute = 0;
            unsigned char second = 0;
            
            getFileTime(fno.fdate, fno.ftime, &year, &month, &day, &hour, &minute, &second);

            // SendUARTStr(fno.fname);

            BOOL isDir = (fno.fattrib & AM_DIR);
            // debugPrint("%s %02d/%02d/%04d %02d:%02d:%02d %ld", isDir ? "D" : "F", day, month, year, hour, minute, second, fno.fsize);   

            if (isDir)
            {
                if (isRoot)
                {
                    numOfFoldersInRoot += 1;
                }
            }
            else if (isSupportedFile(fno.fname)) {
                numOfFilesInFolder += 1;
            }
        }
        f_closedir(&dir);
        
        // debugPrint("Files: %d. Root Folders: %d", numOfFilesInFolder, numOfFoldersInRoot);
    }
    else {
        debugPrint("DrEr:%s", folderName);
    } 
    
    // SendUARTStr(folderName);
    strcpy(curFolderName, folderName);
    
    if (!isInitFatFS)
    {
        drawFolderName();
    }
}

void openFolder(unsigned int folderInd)
{            
    if (folderInd == 0)
    {
        // opening root folder
        openFolderName(ROOT_FOLDER_NAME, TRUE);
    }
    else {
        unsigned int folderCount = 0;
        BOOL foundFolder = FALSE;
        FILINFO fno;
        
        memset(&dir, 0, sizeof(dir));
        
        // first navigate the root folder again to find the correct subfolder index
        // debugPrint("Reading %s", ROOT_FOLDER_NAME);
        FRESULT res = f_opendir(&dir, ROOT_FOLDER_NAME);
        if (res == FR_OK) {
          while (1) {
            res = f_readdir(&dir, &fno); 
            // TODO: might need to check altname as well if fname is not present (e.g. no long filename?)
            if (res != FR_OK || fno.fname[0] == 0) 
                break;
            
            // count starts from 1 (0 for root), so we increment first before comparing
            BOOL isDir = (fno.fattrib & AM_DIR);
            if (isDir)
            {
                folderCount++;
            }

            if (folderCount == folderInd)
            {
                foundFolder = TRUE;
                
                // index starts from 1, 0 is root folder
                debugPrint("Fn:D#%d", folderInd);
                break;
            }                                    
          }
          f_closedir(&dir);
        }
                                
        // then opening that sub-folder index        
        if (foundFolder)
        {
            openFolderName(fno.fname, FALSE);
        }
        else {
            debugPrint("NFn:D#%d", folderInd);
        }
    }
}

void openFile(unsigned int fileInd)
{
    // debugPrint("of%u %s", fileInd, curFolderName);
    
    memset(&fMediaFile, 0, sizeof(fMediaFile));
    
    prevDuration = 0;
    totalDuration = 0;
    curTitlePos = 0;
    lastInfoLen = 0;

    // enumerate the folder file list, looking for matching file index
    unsigned int fileCount = 0;
    BOOL foundFile = FALSE;
    FILINFO fno;

    FRESULT res = f_opendir(&dir, curFolderName);
    if (res == FR_OK) {
      while (1) {
        res = f_readdir(&dir, &fno);                   
        if (res != FR_OK || fno.fname[0] == 0) 
            break;

        BOOL isDir = (fno.fattrib & AM_DIR);
        BOOL isSupported = !isDir && isSupportedFile(fno.fname);
        
        if (isSupported && fileCount == fileInd)
        {
            foundFile = TRUE;

            // index starts from 1, 0 is root folder
            debugPrint("Fn:F#%d", fileInd);
            
            curFileDate = fno.fdate;
            curFileTime = fno.ftime;
            
            break;
        }
                
        if (isSupported)
        {
            fileCount++;
        }
      }
      f_closedir(&dir);
    }

    // then opening that sub-folder index        
    if (foundFile)
    {
        strcpy(shortAudioFilename, fno.fname);
        if (curFolderIndex == 0)
        {
            strcpy(fullAudioFilename, fno.fname);
        }
        else
        {
            sprintf(fullAudioFilename, "%s/%s", curFolderName, fno.fname);
        }
        
        if (isMP3File(fullAudioFilename))
            mp3PlaybackState = MP3_OPENING;
        else 
            wavPlaybackState = WAV_OPENING;
    }
    else {
        // no files found in this folder, we exit here and simulate a remote key press to go to next folder (which will be processed in main loop)
        // this is to avoid stack overflow if too many empty folders result in too many nested function calls
        // if the code is to switch to the next folder automatically
        
        // empty spaces to overwrite previous characters
        LCD_drawItemTitle("Empty folder  ");
        LCD_drawInfoText("                     ");
        LCD_drawRightStatusText("      ");         
        LCD_drawTopMiddleInfo("     ");
        LCD_updateDisplay();
        
        lastRemoteChar = KEY_UP;
    }
}

void gotoNextFolder()
{
    isPlayGoingNext = TRUE;
    if (numOfFoldersInRoot > 0) {
        // curFolderIndex starts from 0 (for root folder)
        // 1 is first sub-folder
        // numOfFoldersInRoot counts sub-folders only. Be careful!
        if (curFolderIndex < numOfFoldersInRoot) {
            // first file, next folder
            curFileIndex = 0;
            curFolderIndex++;
            openFolder(curFolderIndex);
            openFile(curFileIndex);
        }
        else {
            // reach end of list, go to first file, first folder
            curFileIndex = 0;
            curFolderIndex = 0;
            openFolder(curFolderIndex);
            openFile(curFileIndex);
        }
    }
    else {
        // default, go to first file, first folder
        curFileIndex = 0;
        curFolderIndex = 0;
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
}

void gotoPreviousFolder()
{
    isPlayGoingNext = FALSE;
    if (curFolderIndex > 0) {
        // first file, previous folder
        curFileIndex = 0;
        curFolderIndex--;
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
    else if (numOfFoldersInRoot > 0) {
        // reach beginning of list, go to first file, last folder
        curFileIndex = 0;
        curFolderIndex = numOfFoldersInRoot;    // index starts from 1 for first sub-folder, 0 is root folder
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
    else {
        // default, go to first file, first folder
        curFileIndex = 0;
        curFolderIndex = 0;
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
}

void gotoFirstFileInFolder()
{
    isPlayGoingNext = TRUE;
    curFileIndex = 0;                        
    openFile(curFileIndex);                    
}

void gotoNextFile()
{
    isPlayGoingNext = TRUE;
    if (numOfFilesInFolder > 0 && curFileIndex < numOfFilesInFolder - 1)
    {
        // next file, same folder
        curFileIndex++;
        openFile(curFileIndex);
    }
    else if (numOfFoldersInRoot > 0) {
        if (curFolderIndex < numOfFoldersInRoot) {
            // curFolderIndex starts from 0 (for root folder)
            // 1 is first sub-folder
            // numOfFoldersInRoot counts sub-folders only. Be careful!
            curFileIndex = 0;
            curFolderIndex++;
            openFolder(curFolderIndex);
            openFile(curFileIndex);
        }
        else {
            // reach end of list, go to first file, first folder
            curFileIndex = 0;
            curFolderIndex = 0;
            openFolder(curFolderIndex);
            openFile(curFileIndex);
        }
    }
    else {
        // default, go to first file, same folder
        curFileIndex = 0;
        openFile(curFileIndex);
    }
}

void gotoPreviousFile()
{
    isPlayGoingNext = FALSE;
    if (curFileIndex > 0)
    {
        // previous file, same folder
        curFileIndex--;
        openFile(curFileIndex);
    }
    else if (curFolderIndex > 0) {
        // first file, previous folder
        curFileIndex = 0;
        curFolderIndex--;
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
    else if (numOfFoldersInRoot > 0) {
        // reach beginning of list, go to first file, last folder
        curFileIndex = 0;
        curFolderIndex = numOfFoldersInRoot;    // index starts from 1 for first sub folder, 0 is root folder
        openFolder(curFolderIndex);
        openFile(curFileIndex);
    }
    else {
        // default, go to first file, same folder
        curFileIndex = 0;
        openFile(curFileIndex);
    }
}

void adjustToDuration(unsigned int newDuration)
{
    unsigned long int newOffset = 0;
    if (isPlayingMP3)
    {
        // approximate, will be highly incorrect for VBR files
        // which we will just have to accept as limitations
        newOffset = newDuration * mp3BitRate / 8 + ID3v2size;
    }        
    else {
        // exact value for WAV files
        newOffset = newDuration * wavHeader.sample_rate * wavHeader.channels * wavHeader.bits_per_sample / 8 + sizeof(wavHeader);
    }

    if (newOffset < mediaFileSize)
    {
        debugPrint("SK:%us %lub", newDuration, newOffset);

        FRESULT res = f_lseek(&fMediaFile, newOffset);
        if (res == FR_OK)
        {
            outPcmSamples = (UINT32)newDuration * (UINT32)samplingRate;
        }
        else {
            SendUARTStr("SKEr");
        }
    }
    else {
        SendUARTStr("EOFEr");
    }
}

void adjustPlayPosition(unsigned char durationOffset, BOOL isFwd)
{
    BOOL shouldAdjust = FALSE;
    unsigned int curDuration = samplingRate > 0 ? outPcmSamples / samplingRate : 0;
    unsigned int newDuration = 0;
    if (isFwd)
    {
        newDuration = curDuration + durationOffset;
        if (newDuration < totalDuration)
        {
            shouldAdjust = TRUE;
        }
    }
    else {
        shouldAdjust = TRUE;
        
        if (curDuration > durationOffset)
        {
            newDuration = curDuration - durationOffset;
        }
        else 
        {
            newDuration = 0;
        }
    }
    
    if (shouldAdjust)
    {
        // for VBR, exact duration cannot be calculated, we will not display seek position
        if (!isVBRFile)
        {
            sprintf(strResBuf, "Seek: %us", newDuration);
            drawUIMessage(strResBuf);                                    
        }        
        adjustToDuration(newDuration);
    }
    else {
        SendUARTStr("FFEr");
    }
}

void stopCurrentPlayback()
{
    if (isPlayingMP3 && mp3PlaybackState != MP3_IDLE_STATE)
    {
        // SendUARTStr("MP3 Stop");
        mp3PlaybackState = MP3_FILE_CLOSE;
        processMediaFile();
    }
    else if (!isPlayingMP3 && wavPlaybackState != WAV_IDLE_STATE)
    {
        // SendUARTStr("WAV Stop");        
        wavPlaybackState = WAV_FILE_CLOSE;
        processMediaFile();
    }                                
}
// </editor-fold>
 
// <editor-fold defaultstate="collapsed" desc="Remote Control">
INT32 getRemoteNumericValue()
{
    unsigned char c = 0;
    
    strcpy(strResBuf, "");
    for (c = 0; c<remoteDigitsPressedCount; c++)
    {
        sprintf(strResBuf, "%s%c", strResBuf, remoteDigitsPressed[c]);
    }
        
    return atol(strResBuf);
}

void resetMenuOption()
{
    curMenuOption = MENU_ITEM_INVALID;      
    
    if (inputSource < SOURCE_RADIO)
    {
        drawFolderName();
    }
    else {
        resetMenuLine(); 
        LCD_clearRect(CHECKMARK_LOC_X, CHECKMARK_LOC_Y, CHECKMARK_WIDTH, CHECKMARK_HEIGHT);

        if (radioRecordingMode != RECORD_MODE_NONE)
        {
            // show recording file name, remove extensions
            drawMenuInfo2(shortAudioFilename, strlen(shortAudioFilename) - 4); 
        }
        
        if (radioCurBand == RADIO_CUR_BAND_SW && hasStationInfo)
        {
            unsigned char curIndex = stnInfoEntryCurIndex - stnInfoEntryFirstIndex + 1;
            unsigned char totalIndex = stnInfoEntryLastIndex - stnInfoEntryFirstIndex + 1;

            if (radioRecordingMode == RECORD_MODE_NONE)
            {
                // show station info if not recording
                if (stnDistanceKm != STN_DISTANCE_INVALID && stnDistanceKm > 5)
                {
                    // distance and direction inaccurate over short distance, do not show
                    sprintf(strResBuf, "%ukm %s", stnDistanceKm, stnDirection);
                    drawMenuInfo(strResBuf);
                }

                if (stnIsBroadcasting)
                {
                    LCD_drawBitmap(checkMarkIcon, CHECKMARK_LOC_X, CHECKMARK_LOC_Y);
                }
            }
            
            // show station info match count
            if (shouldShowMatchCount)
            {
                LCD_drawTopLeftInfo2("   ");
                sprintf(strResBuf, "%02d/%02d ", curIndex, totalIndex);
                LCD_drawTopLeftInfo(strResBuf);

                shouldShowMatchCount = FALSE;
            }
        }
   }
}

void drawMenuInfo2(char menu[], unsigned char maxLen)
{
    // should be max 18 chars for icon on the right
    resetMenuLine();    
    LCD_drawMenuText2(menu, 0, MIN(strlen(menu), maxLen));    
}

void drawMenuInfo(char menu[])
{
    drawMenuInfo2(menu, MAX_TITLE_CHAR_SMALL - 1);
}

unsigned char getNextMenuItem(unsigned char curEntry)
{
    unsigned entryId = curEntry;
    
    // advance current entry
    if (entryId == MENU_ITEM_INVALID)
    {
        entryId = 0;
    }
    else {
        entryId++;
    }
    
    // ignore options not applicable for current configuration
    if (entryId == MENU_ITEM_PLAY_MODE && inputSource >= SOURCE_RADIO)
    {
        entryId = MENU_ITEM_RADIO_BAND;
    }
    if (entryId == MENU_ITEM_RADIO_BAND && inputSource < SOURCE_RADIO)
    {
        entryId = MENU_ITEM_FORCE_MONO;
    }
    if (entryId == MENU_ITEM_RADIO_FIND_STN_INFO && (inputSource < SOURCE_RADIO || radioCurBand != RADIO_CUR_BAND_SW || !hasStationInfo) )
    {
        entryId = MENU_ITEM_RADIO_SCAN_FAST;
    }
    if (entryId == MENU_ITEM_RADIO_SCAN_FAST && inputSource >= SOURCE_RADIO && radioSSBMode != SSB_NONE)
    {
        entryId = MENU_ITEM_RADIO_FINE_TUNE;
    }
    if (entryId == MENU_ITEM_RADIO_SCAN_FAST && inputSource >= SOURCE_RADIO && radioCurBand != RADIO_CUR_BAND_SW)
    {
        entryId = MENU_ITEM_RADIO_SCAN_ALL;
    }    
    if (entryId == MENU_ITEM_RADIO_REC_USB && inputSource == SOURCE_RADIO)
    {
        entryId = MENU_ITEM_FM_RANGE;
    }    
    if (entryId == MENU_ITEM_RADIO_REC_USB && inputSource > SOURCE_RADIO && radioRecordingMode != RECORD_MODE_NONE)
    {
        entryId = MENU_ITEM_RADIO_STOP_REC;
    }    
    if (entryId == MENU_ITEM_RADIO_STOP_REC && inputSource > SOURCE_RADIO && radioRecordingMode == RECORD_MODE_NONE)
    {
        entryId = MENU_ITEM_FM_RANGE;
    }    
    if ( 
          (entryId == MENU_ITEM_FM_RANGE || entryId == MENU_ITEM_FM_DEEMP || entryId == MENU_ITEM_FM_SPACING)
           && inputSource >= SOURCE_RADIO && radioCurBand != RADIO_CUR_BAND_FM
       )
    {
        entryId = MENU_ITEM_AM_SPACING;
    }
    if (entryId == MENU_ITEM_AM_SPACING && inputSource >= SOURCE_RADIO && radioCurBand != RADIO_CUR_BAND_MW)
    {
        entryId = MENU_ITEM_SSB_BANDWIDTH;
    }
    if (entryId == MENU_ITEM_AM_BANDWIDTH && inputSource >= SOURCE_RADIO && radioSSBMode != SSB_NONE)
    {
        entryId = MENU_ITEM_SSB_BANDWIDTH;
    }
    if ( 
          (entryId == MENU_ITEM_SSB_BANDWIDTH || entryId == MENU_ITEM_SSB_SYNC)
          && inputSource >= SOURCE_RADIO && (radioCurBand == RADIO_CUR_BAND_FM || radioSSBMode == SSB_NONE)
       )
    {
        entryId = MENU_ITEM_SSB_MODE;
    }
    if (entryId == MENU_ITEM_SSB_MODE && inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_FM)
    {
        entryId = MENU_ITEM_FORCE_MONO;
    }
    if (entryId == MENU_ITEM_FORCE_MONO && inputSource >= SOURCE_RADIO && radioCurBand != RADIO_CUR_BAND_FM)
    {
        entryId = MENU_ITEM_POWER_TYPE;
    }
    
    return entryId;
}

unsigned char getPreviousMenuItem(unsigned char curEntry)
{
    // BACK key from first index will go to last index
    // assuming last menu item (Help) is always available regardless of settings
    if (curEntry == 0)
    {
        return MAX_VALID_MENU_ITEM;
    }
    
    // otherwise locate the previous menu entry to go to
    // menu will skip certain entries depending on current settings, we can't just do curEntry--, 
    // we instead loop from 0 and find out which menu item will advances to the current entry
    unsigned char ind = 0;
    unsigned char retVal = 0;
    for (ind = 0; ind <= MAX_VALID_MENU_ITEM; ind++)
    {
        unsigned char nextEntry = getNextMenuItem(ind);
        if (nextEntry < curEntry)
        {
            retVal = nextEntry;
        }
        else {
            break;
        }
    }
    return retVal;
}

void processRemoteKey(char lastRemoteKey)
{
    BOOL isValid = FALSE;
    BOOL shouldBeep = TRUE;
    char buf[32];
                    
    // reset auto power off count
    // SendUARTStr("A");
    totalRunningInactiveSecs = 0;    
    
    BOOL isDigitInput = (lastRemoteKey >= KEY_0 && lastRemoteKey <= KEY_9);
    BOOL isVolumeUp = (lastRemoteKey == KEY_COLOR_VOLP);
    BOOL isVolumeDown = (lastRemoteKey == KEY_COLOR_VOLM);
    BOOL isMute = (lastRemoteKey == KEY_MUTE);
    BOOL isRewind = (lastRemoteKey == KEY_LEFT_OPT);
    BOOL isFastForward = (lastRemoteKey == KEY_RIGHT_OPT);
    BOOL isPlayPause = (lastRemoteKey == KEY_OK);
         
    if (
         remoteDigitsPressedCount == 0 
            && 
         (lastRemoteKey == KEY_SETUP || lastRemoteKey == KEY_PREVIOUS)
        ) // no remote digits pressed, enter menu
    {
        isValid = TRUE;           
        
        if (lastRemoteKey == KEY_PREVIOUS)
        {
            // back to previous menu
            curMenuOption = getPreviousMenuItem(curMenuOption);
        }
        else
        {
            // one of the keys to process menu options
            curMenuOption = getNextMenuItem(curMenuOption);
        }

        // Menu Options
        resetMenuLine();
        if (curMenuOption == MENU_ITEM_SOURCE)
        {
            getInputSourceStr(inputSource, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"SRC: %s", buf);
            drawMenuInfo(strResBuf);                                        
        }                        
        else if (curMenuOption == MENU_ITEM_PLAY_MODE)
        {
            getPlayModeStr(curPlayMode, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", buf);
            drawMenuInfo(strResBuf);
        }                        
        else if (curMenuOption == MENU_ITEM_DURATION_MODE)
        {
            getDurationModeStr(curDurationMode, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", buf);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_RADIO_BAND)
        {
            getRadioBandStr(radioCurBand, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"Band: %s", buf);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_RADIO_FIND_STN_INFO)
        {
            getRadioBandStr(radioCurBand, buf);
            strcpy(strResBuf, MENU_CURSOR_ARROW"Find Stn Info");
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_RADIO_VIEW_STN_INFO)
        {
            getRadioBandStr(radioCurBand, buf);
            strcpy(strResBuf, MENU_CURSOR_ARROW"View Stn Info");
            drawMenuInfo(strResBuf);
        }        
        else if (curMenuOption == MENU_ITEM_RADIO_SCAN_FAST)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Quick Scan");
        }
        else if (curMenuOption == MENU_ITEM_RADIO_SCAN_ALL)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Full Scan");
        }
        else if (curMenuOption == MENU_ITEM_RADIO_FINE_TUNE)
        {
            sprintf(strResBuf, MENU_CURSOR_ARROW"Fine Tune: %s", isRadioFineTune ? "On" : "Off");
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_RADIO_REC_USB)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Rec (USB)");
        }
        else if (curMenuOption == MENU_ITEM_RADIO_REC_SD)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Rec (SD)");
        }
        else if (curMenuOption == MENU_ITEM_RADIO_STOP_REC)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Stop Rec");
        }
        else if (curMenuOption == MENU_ITEM_FM_RANGE)
        {                
            getFmRangeMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }   
        else if (curMenuOption == MENU_ITEM_FM_DEEMP)
        {                
            getFmDeempMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_FM_SPACING)
        {                
            getFmSpacingMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_AM_SPACING)
        {                         
            getAmSpacingMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_AM_BANDWIDTH)
        {                
            getAmBwMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_SSB_BANDWIDTH)
        {                
            getSsbBwMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_SSB_MODE)
        {
            getSsbModeMenuStr(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_SSB_SYNC)
        {
            getSsbSyncMenu(strResBuf, TRUE);
            drawMenuInfo(strResBuf);
        }            
        else if (curMenuOption == MENU_ITEM_FORCE_MONO)
        {
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", forceMono ? FORCE_MONO_STR : ALLOW_STEREO_STR);
            drawMenuInfo(strResBuf);
        }            
        else if (curMenuOption == MENU_ITEM_POWER_TYPE)
        {
            getPowerTypeStr(curPowerType, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s: %s", POWER_MENU_STR, buf);
            drawMenuInfo(strResBuf);
        }                                    
        else if (curMenuOption == MENU_ITEM_POWER_OFF_IDLE)
        {
            getIdleOffStr(curIdleOffSetting, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s: %s", IDLEOFF_MENU_STR, buf);
            drawMenuInfo(strResBuf);
        }                        
        else if (curMenuOption == MENU_ITEM_POWER_OFF_ALWAYS)
        {
            getAlwaysOffStr(curAlwaysOffSetting, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s: %s", AUTOOFF_MENU_STR, buf);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_BEEPER)
        {
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", disabledBeep ? BEEPER_OFF_STR : BEEPER_ON_STR);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_EDIT_CONFIG)
        {
            getEditConfigStr(isEditingConfig, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", buf);
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_ALARM)
        {
            getAlarmMenuStr(alarmEnabled, alarmHour, alarmMin, buf);
            sprintf(strResBuf, MENU_CURSOR_ARROW"%s", buf);            
            drawMenuInfo(strResBuf);
        }
        else if (curMenuOption == MENU_ITEM_SHUTDOWN_KEEP_PWR)
        {
            // power off the CPU, but peripheral power remains on
            // for analog functionality only (e.g. mixer)
            drawMenuInfo(MENU_CURSOR_ARROW"Standby");
        }
        else if (curMenuOption == MENU_ITEM_SHUTDOWN_FULL)
        {
            // full power off, including peripheral power
            drawMenuInfo(MENU_CURSOR_ARROW"Power Off");
        }            
        /*
        else if (curMenuOption == MENU_ITEM_SLEEP)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Sleep");
        } 
        */            
        else if (curMenuOption == MENU_ITEM_HELP)
        {
            drawMenuInfo(MENU_CURSOR_ARROW"Help");
        }            
        else {
            resetMenuOption();
        }            
        
        remoteDigitsPressedCount = 0;     
        drawRemoteInput();
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_SETUP)                                       
    {
        if (remoteDigitsPressedCount == 4 && isEditingConfig)                  // update time in hhmm format
        {
            drawUIMessage("");    
            unsigned int num = getRemoteNumericValue();     
            unsigned char hour = num / 100;
            unsigned char min = num % 100;
            
            if (isValidTime(hour, min, 0))
            {
                if (isRTCOK)
                {
                    setRTC(hour, min, 0, curDay, curMonth, curYear2k);
                    drawUIMessage("Time set");   
                    
                    if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_SW)
                        fetchRadioInfoEntry();
                    
                    drawTopClock();
                    LCD_drawInfoText(EMPTY_INFO_LINE);
                    
                    tinyBeep(FALSE);

                    curHour = hour;
                    curMinute = min;
                    curSecond = 0;
                    isValid = TRUE;
                    
                    shouldUpdateRadioScreen = TRUE;
                }
                else 
                {
                    drawUIMessage("RTC Error");                           
                    shouldBeep = FALSE;
                }
            }
            else {
                drawUIMessage("Invalid time");                    
                shouldBeep = FALSE;
            }
        }
        else if (remoteDigitsPressedCount == 6 && isEditingConfig)             // update date in ddmmyy format
        {
            drawUIMessage("");    
            INT32 num = getRemoteNumericValue();     
            unsigned int day = num / 10000;
            unsigned int month = (num / 100) % 100;
            unsigned int year = num % 100;
            
            if ( isValidDate(year, month, day) )
            {
                if (isRTCOK)
                {
                    setRTC(curHour, curMinute, curSecond, day, month, year);
                    drawUIMessage("Date set");    
                    
                    if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_SW)
                        fetchRadioInfoEntry();
                    
                    drawTopClock();
                    LCD_drawInfoText(EMPTY_INFO_LINE);
                    tinyBeep(FALSE);

                    curDay = day;
                    curMonth = month;
                    curYear2k = year;

                    isValid = TRUE;      
                    shouldUpdateRadioScreen = TRUE;
                }
                else 
                {
                    drawUIMessage("RTC Error");                         
                    shouldBeep = FALSE;
                }
            }                                  
            else {
                drawUIMessage("Invalid date");                  
                shouldBeep = FALSE;
            }
        }
                
        remoteDigitsPressedCount = 0;     
        drawRemoteInput();
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_SET)                  //  Set Button
    {
        if (remoteDigitsPressedCount == 4 && isEditingConfig)              // Set alarm clock
        {
            drawUIMessage("");    
            unsigned int num = getRemoteNumericValue();     
            unsigned char hour = num / 100;
            unsigned char min = num % 100;

            if (isValidTime(hour, min, 0))
            {
                alarmHour = hour;
                alarmMin = min;
                alarmEnabled = TRUE;
            }
            else {
                drawUIMessage("Invalid time");                    
                shouldBeep = FALSE;
            }
        }
        else {
            alarmEnabled = !alarmEnabled;
        }
                                
        getAlarmMenuStr(alarmEnabled, alarmHour, alarmMin, strResBuf);
        drawMenuInfo(strResBuf);   
        drawTopClock();
                
        remoteDigitsPressedCount = 0;     
        drawRemoteInput();
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_TV_IN)                  // Change Source Button
    {
        resetMenuLine();
        
        if (inputSource >= SOURCE_RADIO)
        {
            stopAnyCurRadioRecording();
        }

        // source change handled in main loop
        shouldChangeSource = TRUE;
        // SendUARTStr("c1");
        isValid = TRUE;
        curMenuOption = MENU_ITEM_INVALID;
    }
    else if (lastRemoteKey == KEY_APP)                  
    {
        if (curMenuOption != MENU_ITEM_INVALID)
        {
            resetMenuLine();
            
            // select a particular menu option
            if (curMenuOption == MENU_ITEM_SOURCE)
            {
                if (inputSource >= SOURCE_RADIO)
                {
                    stopAnyCurRadioRecording();
                }
                
                // source change handled in main loop
                // SendUARTStr("c4");
                shouldChangeSource = TRUE;
            }
            else if (curMenuOption == MENU_ITEM_PLAY_MODE)
            {               
                curPlayMode = (curPlayMode + 1) % TOTAL_PLAY_MODES;
                getPlayModeStr(curPlayMode, strResBuf);
                drawMenuInfo(strResBuf);
            }           
            else if (curMenuOption == MENU_ITEM_DURATION_MODE)
            {
                curDurationMode = (curDurationMode + 1) % TOTAL_DURATION_MODES;
                getDurationModeStr(curDurationMode, strResBuf);
                drawMenuInfo(strResBuf);                
            }            
            /*
            else if (curMenuOption == MENU_ITEM_DISK_TEST)
            {
                SendUARTStr("DSKTST");
            }
            */ 
            else if (curMenuOption == MENU_ITEM_RADIO_BAND)
            {
                stopAnyCurRadioRecording();
                gotoNextRadioBand();
            }
            else if (curMenuOption == MENU_ITEM_RADIO_FIND_STN_INFO)
            {
                drawUIMessage("Loading ...");
                LCD_updateMenuText();
                
                // find the best match station info entry
                stopAnyCurRadioRecording();                    
                findStnInfo();
                fetchRadioInfoEntry();

                // refresh the display
                shouldShowMatchCount = TRUE;
                remoteDigitsPressedCount = 0;
                drawRemoteInput();
                shouldUpdateRadioScreen = TRUE;
            }            
            else if (curMenuOption == MENU_ITEM_RADIO_VIEW_STN_INFO)
            {
                // view the current station information
                stopAnyCurRadioRecording();                    
                drawStnInfo();
            }            
            else if (curMenuOption == MENU_ITEM_RADIO_SCAN_FAST)
            {
                stopAnyCurRadioRecording();
                updateRadioConfig();
                resetMenuLine();
                shouldUpdateRadioScreen = TRUE;
                
                scanEntireBand(FALSE);                
            }
            else if (curMenuOption == MENU_ITEM_RADIO_SCAN_ALL)
            {
                stopAnyCurRadioRecording();                
                updateRadioConfig();
                resetMenuLine();
                shouldUpdateRadioScreen = TRUE;
                
                scanEntireBand(TRUE);                                
            }
            else if (curMenuOption == MENU_ITEM_RADIO_FINE_TUNE)
            {
                isRadioFineTune = !isRadioFineTune;
            }            
            else if (curMenuOption == MENU_ITEM_RADIO_REC_USB)
            {
                startRadioRecording(RECORD_MODE_USB);
                shouldUpdateRadioScreen = TRUE;
            }
            else if (curMenuOption == MENU_ITEM_RADIO_REC_SD)
            {
                startRadioRecording(RECORD_MODE_SD);
                shouldUpdateRadioScreen = TRUE;
            }
            else if (curMenuOption == MENU_ITEM_RADIO_STOP_REC)
            {
                stopAnyCurRadioRecording();
                shouldUpdateRadioScreen = TRUE;
            }            
            else if (curMenuOption == MENU_ITEM_FM_RANGE)
            {
                stopAnyCurRadioRecording();                
                radioFMRange = (radioFMRange + 1) % MAX_RADIO_FM_RANGES;
                radioCurFreq = 0;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getFmRangeMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);                
                
                gotoRadioFreq(radioCurFreq);
            }
            else if (curMenuOption == MENU_ITEM_FM_DEEMP)
            {
                radioFMDeemp = !radioFMDeemp;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getFmDeempMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);
                
                // SI values 1/2, our values 0/1
                if (isPaused) {
                    startRadio();
                }
                si473x_setFMDeEmphasis(radioFMDeemp + 1);
            }
            else if (curMenuOption == MENU_ITEM_FM_SPACING)
            {
                stopAnyCurRadioRecording();
                
                radioFMSpacing = (radioFMSpacing + 1) % MAX_FM_SPACING;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getFmSpacingMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);                
                
                gotoRadioFreq(radioCurFreq);
            }
            else if (curMenuOption == MENU_ITEM_AM_SPACING)
            {
                stopAnyCurRadioRecording();
                
                radioAMSpacing = !radioAMSpacing;
                radioCurFreq = 0;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getAmSpacingMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);           
                
                gotoRadioFreq(radioCurFreq);
            }
            else if (curMenuOption == MENU_ITEM_AM_BANDWIDTH)
            {                
                radioAMBW = (radioAMBW + 1) % MAX_RADIO_AM_BW;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getAmBwMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);
                
                if (isPaused) {
                    startRadio();
                }
                si473x_setBandwidth(radioAMBW, 0);
            }
            else if (curMenuOption == MENU_ITEM_SSB_BANDWIDTH)
            {                
                radioSSBBW = (radioSSBBW + 1) % MAX_RADIO_SSB_BW;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getSsbBwMenuStr(strResBuf, FALSE);
                drawMenuInfo(strResBuf);               
                
                if (isPaused) {
                    startRadio();
                }
                si473x_setSSBAudioBandwidth(radioSSBBW);
            }
            else if (curMenuOption == MENU_ITEM_SSB_MODE)
            {
                unsigned char oldSSbMode = radioSSBMode;
                
                if (isPaused) {
                    startRadio();
                }                                
                
                radioSSBMode = (radioSSBMode + 1) % MAX_SSB_MODES;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                drawMenuInfo(LOADING_MENU_LINE);
                LCD_updateDisplay();
                                
                if (oldSSbMode == SSB_NONE || radioSSBMode == SSB_NONE)
                {
                    // change from no-SSB to SSB
                    radioSetAmOrSsbMode(TRUE);
                }
                else {
                    // change between SSB mode (USB/LSB)
                    si473x_setSSBMode(radioSSBMode);
                }
                
                // new SSB mode on screen                
                getSsbModeMenuStr(strResBuf, FALSE);
                SendUARTStr(strResBuf);
                drawMenuInfo(strResBuf);
                
                // reset current frequency
                hasRadioBandChanged = TRUE;
                gotoRadioFreq(radioCurFreq);
            }
            else if (curMenuOption == MENU_ITEM_SSB_SYNC)
            {
                radioSSBSync = !radioSSBSync;
                updateRadioConfig();
                shouldUpdateRadioScreen = TRUE;
                
                getSsbSyncMenu(strResBuf, FALSE);
                SendUARTStr(strResBuf);
                drawMenuInfo(strResBuf);           
                
                if (isPaused) {
                    startRadio();
                }
                
                si473x_setSSBSync(radioSSBSync);
            } 
            else if (curMenuOption == MENU_ITEM_FORCE_MONO)
            {
                forceMono = !forceMono;
                drawMenuInfo(forceMono ? FORCE_MONO_STR : ALLOW_STEREO_STR);                
                
                if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_FM)
                {
                    if (isPaused) {
                        startRadio();
                    }
                    
                    setFmStereoMode();
                }
            }
            else if (curMenuOption == MENU_ITEM_POWER_TYPE)
            {
                curPowerType = (curPowerType + 1) % POWER_TYPE_COUNT;
                
                getPowerTypeStr(curPowerType, buf);
                sprintf(strResBuf, POWER_MENU_STR": %s", buf);
                drawMenuInfo(strResBuf);                                
                
                drawBattery(TRUE);
            }                        
            else if (curMenuOption == MENU_ITEM_POWER_OFF_IDLE)
            {
                curIdleOffSetting = (curIdleOffSetting + 1) % IDLE_OFF_COUNT;
                
                getIdleOffStr(curIdleOffSetting, buf);
                sprintf(strResBuf, IDLEOFF_MENU_STR": %s", buf);
                drawMenuInfo(strResBuf);
                
                totalNotPlayingSecs = 0;                
                updatePowerSaveConfig();                
            }
            else if (curMenuOption == MENU_ITEM_POWER_OFF_ALWAYS)
            {
                curAlwaysOffSetting = (curAlwaysOffSetting + 1) % ALWAYS_OFF_COUNT;
                getAlwaysOffStr(curAlwaysOffSetting, buf);
                sprintf(strResBuf, AUTOOFF_MENU_STR": %s", buf);
                drawMenuInfo(strResBuf);
                
                totalRunningInactiveSecs = 0;                
                updatePowerSaveConfig();                
            }        
            else if (curMenuOption == MENU_ITEM_BEEPER)
            {
                disabledBeep = !disabledBeep;
                drawMenuInfo(disabledBeep ? BEEPER_OFF_STR : BEEPER_ON_STR);
            }                       
            else if (curMenuOption == MENU_ITEM_EDIT_CONFIG)
            {
                isEditingConfig = !isEditingConfig;
                getEditConfigStr(isEditingConfig, buf);
                drawMenuInfo(buf);
            }
            else if (curMenuOption == MENU_ITEM_ALARM)
            {
                alarmEnabled = !alarmEnabled;                
                getAlarmMenuStr(alarmEnabled, alarmHour, alarmMin, strResBuf);
                drawMenuInfo(strResBuf);   
                drawTopClock();
            }
            else if (curMenuOption == MENU_ITEM_SHUTDOWN_KEEP_PWR)
            {
                //  Mixer mode, digital part off
                //  but keep peripheral voltage on so that analog function can still work
                shouldEnterStandby = TRUE;
                keepPwrOnInStandby = TRUE;
            }
            else if (curMenuOption == MENU_ITEM_SHUTDOWN_FULL)
            {
                //  Power off completely
                shouldEnterStandby = TRUE;
                keepPwrOnInStandby = FALSE;
            }            
            /*
            else if (curMenuOption == MENU_ITEM_SLEEP)
            {
                SendUARTStr("SL");

                // put PIC into sleep mode to reduce disturbance
                if (inputSource >= SOURCE_RADIO)
                {
                    stopAnyCurRadioRecording();
                }
                
                // disable timer interrupt
                IEC1bits.T4IE = 0;
                IEC0bits.T1IE = 0;

                // sleep will end and execution will continue upon interrupt from remote or from keypad
                setKeypadAnalog(FALSE); // keypad in digital mode
                delay_ms(5);
                enterSleepMode();
                setKeypadAnalog(TRUE);  // keypad back to analog mode
                delay_ms(5);
                lastRemoteChar = KEY_INVALID;
                isPwrBtnPressed = FALSE;
                                        
                SendUARTStr("WK");
                
                // enable timer interrupt
                IEC1bits.T4IE = 1;
                IEC0bits.T1IE = 1;
            }
            */ 
            else if (curMenuOption == MENU_ITEM_HELP)
            {
                if (inputSource >= SOURCE_RADIO)
                {
                    stopAnyCurRadioRecording();
                }
                
                //  Help, blocking until remote key is pressed
                drawHelp();
            }                        
                                        
            curMenuOption = MENU_ITEM_INVALID;
            isValid = TRUE;
        }
        else if (remoteDigitsPressedCount > 0 && remoteDigitsPressedCount <= 4)
        {
            if (inputSource < SOURCE_RADIO)
            {
                // in MP3 mode, go to folder #XXXX, starts from 1
                unsigned int num = getRemoteNumericValue();

                // index starts from 1
                // 1 is root folder
                // 2 is first sub-folder
                // numOfFoldersInRoot counts sub-folders only, hence we need to +1
                if (num > 0 && num <= numOfFoldersInRoot + 1)
                {
                    isUsingRemote = TRUE;
                    stopCurrentPlayback();
                    isUsingRemote = FALSE;

                    curFolderIndex = num - 1;
                    openFolder(curFolderIndex);
                    curFileIndex = 0;
                    openFile(curFileIndex);

                    isValid = TRUE;
                }
            }
            else {
                // in radio mode, do nothing
            }
        }
        else {
            if (inputSource >= SOURCE_RADIO)
            {
                // radio mode
                if (radioCurBand == RADIO_CUR_BAND_SW)
                {
                    // SW mode, go to next shortwave band
                    gotoNextSwBand();
                    isValid = TRUE;
                }
                else {
                    // non SW mode, go to next band
                    gotoNextRadioBand();   
                    isValid = TRUE;
                }
            }   
            else {
                if (inputSource >= SOURCE_RADIO)
                {
                    stopAnyCurRadioRecording();
                }
                
                // SendUARTStr("c10");
                
                // normal mode, source change handled in main loop
                shouldChangeSource = TRUE;   
                isValid = TRUE;
            }
        }
        
        if (!isValid)
        {
            // drawUIMessage("Invalid folder");  
            // LCD_updateDisplay();  
            resetMenuOption();            
            remoteDigitsPressedCount = 0;     
            drawRemoteInput();            
            shouldBeep = FALSE;
        }
        else {
            drawStatusBar();
        }
        
        remoteDigitsPressedCount = 0;                
        LCD_updateDisplay();
    }
    else if (isPlayPause)                  
    {
        BOOL shouldRefreshRemoteInput = TRUE;
        
        if (remoteDigitsPressedCount > 0 && remoteDigitsPressedCount <= 4)
        {
            // if remote keys are pressed
            unsigned int track = getRemoteNumericValue();        
                                                
            if (inputSource < SOURCE_RADIO)
            {
                // in MP3 mode, go to track #XXXX within folder, starts from 1
                if (track > 0 && track <= numOfFilesInFolder)
                {
                    isUsingRemote = TRUE;
                    stopCurrentPlayback();
                    isUsingRemote = FALSE;

                    curFileIndex = track - 1;
                    openFile(track - 1);                
                }
                else {
                    drawUIMessage("Invalid track");                        
                    shortBeep();
                    shouldBeep = FALSE;
                }
            }
            else {                   
                // in radio mode, go to preset
                if (track >= 0 && track < EEPROM_MAX_BAND_PRESETS)
                {                    
                    radioCurPresetInd = track;
                    
                    unsigned int presetOffset = getEepromPresetOffset();        
                    unsigned int presetFreq = readRadioPresetFrequency(presetOffset, track);
                    gotoRadioFreq(presetFreq);                                                           
                    showLoadedPreset(track);                    
                    
                    isValid = TRUE;
                    shouldUpdateRadioScreen = TRUE;
                    shouldRefreshRemoteInput = FALSE;
                }
                else {                    
                    drawUIMessage("Invalid preset");                                           
                                                                     
                    shortBeep();
                    shouldBeep = FALSE;                    
                }
            }
        }
        else {
            // if no remote key has been pressed
            if (inputSource < SOURCE_RADIO)
            {
                // MP3 Mode
                if (isFilePlaying)
                {
                    isPaused = !isPaused;            
                    debugPrint("PS:%d", isPaused);                               

                    if (!isPaused)
                    {
                        enableAudio(TRUE);
                    }

                    drawTopClock();   
                    LCD_updateDisplay();

                    // do not update battery here, otherwise battery level will increase if updated when paused
                    // drawBattery(TRUE);
                }
                else if (!isFilePlaying) {
                    isPaused = FALSE;            
                    enableAudio(TRUE);

                    // if file has stopped, simply go to next file
                    gotoNextFile();
                }
            }
            else {
                // in radio mode
                BOOL wasRecording = (radioRecordingMode != RECORD_MODE_NONE);
                if (wasRecording)
                {
                    // if is recording, stop 
                    stopAnyCurRadioRecording();
                    delay_ms(1000);
                }                                                
                else {          
                    if (radioSSBMode != SSB_NONE)
                    {
                        updateRadioConfig();
                        shouldUpdateRadioScreen = TRUE;
                        
                        // if in SSB mode, toggle USB/LSB
                        radioSSBMode = (radioSSBMode == USB_MODE ? LSB_MODE : USB_MODE);
                        si473x_setSSBMode(radioSSBMode);
                        
                        // show new SSB mode on screen                
                        getSsbModeMenuStr(strResBuf, FALSE);
                        SendUARTStr(strResBuf);
                        drawMenuInfo(strResBuf);

                        // reset current frequency
                        hasRadioBandChanged = TRUE;
                        gotoRadioFreq(radioCurFreq);
                    }   
                    else {
                        // else do nothing
                    }
                }
            }
        }
        
        drawStatusBar();
        remoteDigitsPressedCount = 0;           
        if (shouldRefreshRemoteInput) {
            drawRemoteInput();
        }
        LCD_updateDisplay();
    }
    else if (isVolumeDown || isMute)              // Volume Down or Mute
    {    
        if (isMute)
        {
            if (volumeLevel > 0)
                volumeLevel = 0;
            else 
                volumeLevel = MAX_VOL_LVL - 1;
        }
        else if (volumeLevel > 0)
            volumeLevel--;

        remoteDigitsPressedCount = 0;
        drawVolumeTitle();
        
        if (inputSource >= SOURCE_RADIO)
        {
            if (isPaused) {
                startRadio();
            }
            
            // setRadioVolume is actually adjusted, hence we call setVolume directly
            if (volumeLevel == 0)
            {
                si473x_setVolume(0);
            }
            else {
                setRadioVolume(volumeLevel);
            }
        }
    }
    else if (isVolumeUp)              // Volume Up 
    {
        BOOL isRadio = (inputSource >= SOURCE_RADIO);
        unsigned char overAmplify = isRadio ? 1 : OVER_AMPLIFY_RATIO;
        if (volumeLevel < MAX_VOL_LVL * overAmplify)
            volumeLevel++;

        remoteDigitsPressedCount = 0;
        drawVolumeTitle();
        
        if (isRadio)
        {
            if (isPaused) {
                startRadio();
            }
            
            setRadioVolume(volumeLevel);
        }
    }
    else if (lastRemoteKey == KEY_LEFT)              // Back
    {                                
        if (inputSource < SOURCE_RADIO)
        {
            // In MP3 mode, go to previous track, regardless of play mode
            isUsingRemote = TRUE;
            stopCurrentPlayback();
            gotoPreviousFile();                

            isUsingRemote = FALSE;
            remoteDigitsPressedCount = 0;

            resetMenuOption();
            LCD_updateDisplay();
        }
        else {
            // In radio mode, go to the previous preset
            unsigned int presetInd = 0;
            if (radioCurPresetInd == RADIO_INVALID_PRESET_IND)
            {
                // radioCurPresetInd = findNearestRadioPresetFrequency(radioCurFreq);
                radioCurPresetInd = findRadioPresetFrequency(radioCurFreq);
            }
            if (radioCurPresetInd > 0 && radioCurPresetInd != RADIO_INVALID_PRESET_IND)
            {
                presetInd = radioCurPresetInd - 1;
            }
            unsigned int presetOffset = getEepromPresetOffset();        
            unsigned int newPrFr = readRadioPresetFrequency(presetOffset, presetInd);
            
            radioCurPresetInd = presetInd;

            gotoRadioFreq(newPrFr);            
            showLoadedPreset(presetInd);
            isValid = TRUE;        
            shouldUpdateRadioScreen = TRUE;
        }
    }
    else if (lastRemoteKey == KEY_BLACK_VOLM || lastRemoteKey == KEY_BLACK_VOLP)         // Black volume down
    {
        BOOL isVDown = (lastRemoteKey == KEY_BLACK_VOLM);        
        if (inputSource >= SOURCE_RADIO)
        {
            // in radio mode
            if (remoteDigitsPressedCount == 6  && isEditingConfig)
            {
                // update current location
                BOOL isCoordOK = swUpdateMyLoc(isVDown);
                if (isCoordOK)
                {
                    if (isVDown)
                    {
                        sprintf(strResBuf, "Lat: %d.%02d%c", curMyLat1, curMyLat2, 127);                  
                    }
                    else {
                        sprintf(strResBuf, "Lng: %d.%02d%c", curMyLng1, curMyLng2, 127);                  
                    }   
                    drawUIMessage(strResBuf);
                    fetchRadioInfoEntry();
                }
                else {
                    drawUIMessage("Long/lat error");   
                }
            }
            else if (remoteDigitsPressedCount == 2  && isEditingConfig)
            {
                // update timezone
                unsigned char rawInp = getRemoteNumericValue();
                if (rawInp < 15)
                {
                    if (isVDown)
                    {
                        curTimeZone = -rawInp;
                    }
                    else {
                        curTimeZone = +rawInp;
                    }

                    sprintf(strResBuf, "Timezone: %c%02d:00", curTimeZone >= 0 ? '+' : '-', rawInp);
                    drawUIMessage(strResBuf);
                    
                    if (inputSource >= SOURCE_RADIO && radioCurBand == RADIO_CUR_BAND_SW)
                        fetchRadioInfoEntry();
                }
                else {
                    drawUIMessage("Timezone error");
                }
            }
            else {
                // switch between matches in radio station information
                if (radioCurBand == RADIO_CUR_BAND_SW)
                {
                    if (hasStationInfo)
                    {
                        if (isVDown)
                        {
                            if (stnInfoEntryCurIndex > stnInfoEntryFirstIndex)
                            {
                                stnInfoEntryCurIndex--;
                            }
                            else {
                                stnInfoEntryCurIndex = stnInfoEntryLastIndex;
                            }
                        }
                        else {
                            if (stnInfoEntryCurIndex < stnInfoEntryLastIndex)
                            {
                                stnInfoEntryCurIndex++;
                            }
                            else {
                                stnInfoEntryCurIndex = stnInfoEntryFirstIndex;
                            }
                        }
                        fetchRadioInfoEntry();
                        shouldShowMatchCount = TRUE;
                    }
                }
            }
           
            remoteDigitsPressedCount = 0;
            drawRemoteInput();
            resetMenuOption();
            
            isValid = TRUE;        
            shouldUpdateRadioScreen = TRUE;
        }
        else {
            // in media mode, toggle between media information to be displayed
            if (isVDown)
            {
                if (mediaInfoKeyIndex > 0)
                    mediaInfoKeyIndex--;
                else 
                    mediaInfoKeyIndex = MEDIA_INFO_KEY_COUNT - 1;
            }
            else {
                if (mediaInfoKeyIndex < MEDIA_INFO_KEY_COUNT - 1)
                    mediaInfoKeyIndex++;
                else 
                    mediaInfoKeyIndex = 0;
            }
            
            isValid = TRUE;        
            shouldDrawMediaInfo = TRUE;
        }
    }
    else if (lastRemoteKey == KEY_RIGHT)              // Next
    {       
        if (inputSource < SOURCE_RADIO)
        {
            // In MP3 mode, go to next track, regardless of play mode
            isUsingRemote = TRUE;
            stopCurrentPlayback();
            gotoNextFile();      

            isUsingRemote = FALSE;
            remoteDigitsPressedCount = 0;

            resetMenuOption();
            LCD_updateDisplay();
        }
        else {
            // In radio mode, 
            if (remoteDigitsPressedCount > 0 && remoteDigitsPressedCount <= 4)
            {
                // store current frequency to preset memory if digits are entered
                resetMenuOption();
                
                unsigned int track = getRemoteNumericValue();    
                if (track >= 0 && track < EEPROM_MAX_BAND_PRESETS)
                {   
                    unsigned int presetOffset = getEepromPresetOffset(); 
                    writeRadioPresetFrequency(presetOffset, track, radioCurFreq);                                        
                    
                    remoteDigitsPressedCount = 0;    
                                        
                    shouldUpdateRadioScreen = TRUE;
                    isValid = TRUE;
                    
                    // will automatically overwrite current remote input display
                    showLoadedPreset(track);                 
                }
                else {                    
                    remoteDigitsPressedCount = 0;    
                    drawRemoteInput();
                    
                    drawUIMessage("Invalid preset");                        
                                                        
                    shortBeep();                    
                    shouldBeep = FALSE;                        
                }
            }
            else {
                // go to next preset
                unsigned int presetInd = 0;                   
                if (radioCurPresetInd == RADIO_INVALID_PRESET_IND)
                {
                    // radioCurPresetInd = findNearestRadioPresetFrequency(radioCurFreq);
                    radioCurPresetInd = findRadioPresetFrequency(radioCurFreq);
                }
                if (radioCurPresetInd < EEPROM_MAX_BAND_PRESETS - 1)
                {
                    presetInd = radioCurPresetInd + 1;
                }

                radioCurPresetInd = presetInd;
                
                unsigned int presetOffset = getEepromPresetOffset();                     
                unsigned int newPrFr = readRadioPresetFrequency(presetOffset, presetInd);
                
                gotoRadioFreq(newPrFr);
                showLoadedPreset(presetInd);
                
                isValid = TRUE;   
                shouldUpdateRadioScreen = TRUE;
            }
        }
    }
    else if (lastRemoteKey == KEY_DOWN)              
    {
        if (inputSource >= SOURCE_RADIO)
        {
            if (isPaused) {
                startRadio();
            }
                            
            if (radioSSBMode == SSB_NONE)
            {                
                stopAnyCurRadioRecording();
                
                // goto previous station, non-SSB only
                tinyBeep(FALSE);                
                scanPreviousStation();
                shouldBeep = FALSE;                
            }
            else {
                // SSB Mode, decrease BFO
                if (radioSSBBfoOffset > -MAX_SSB_BFO_OFFSET_HZ)
                {
                    radioSSBBfoOffset -= SSB_BFO_OFFSET_STEP_HZ;
                }
                si473x_setSSBBfoOffset(radioSSBBfoOffset);
                justChangedBfoOffset = TRUE;
                shouldUpdateRadioScreen = TRUE;
            }
        }
        else
        {
            // previous folder
            isUsingRemote = TRUE;
            stopCurrentPlayback();
            gotoPreviousFolder();
            isUsingRemote = FALSE;
        }
        remoteDigitsPressedCount = 0;
        resetMenuOption();
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_UP)              
    {           
        if (inputSource >= SOURCE_RADIO)
        {
            if (isPaused) {
                startRadio();
            }
            
            if (radioSSBMode == SSB_NONE)
            {
                stopAnyCurRadioRecording();
                
                // goto next station, non-SSB only
                tinyBeep(FALSE);                
                scanNextStation();
                shouldBeep = FALSE;                
            }
            else {
                // SSB Mode, increase BFO
                if (radioSSBBfoOffset < MAX_SSB_BFO_OFFSET_HZ)
                {
                    radioSSBBfoOffset += SSB_BFO_OFFSET_STEP_HZ;
                }
                si473x_setSSBBfoOffset(radioSSBBfoOffset);
                justChangedBfoOffset = TRUE;
                shouldUpdateRadioScreen = TRUE;
            }
        }
        else
        {
            // next folder   
            isUsingRemote = TRUE;
            stopCurrentPlayback();
            gotoNextFolder();     
            isUsingRemote = FALSE;
        }
        remoteDigitsPressedCount = 0;
        resetMenuOption();
        LCD_updateDisplay();
    }
    else if (isRewind)              // Rewind
    {        
        remoteDigitsPressedCount = 0;                
        resetMenuOption();
        LCD_updateDisplay();
        
        if (inputSource >= SOURCE_RADIO)
        {
            // scan previous frequency
            gotoRadioPrevFreq();
        }
        else {
            // rewind
            adjustPlayPosition(REW_FW_DURATION_SEC, FALSE);
        }            
    }
    else if (isFastForward)              
    {
        BOOL isAdjust = FALSE;
        
        unsigned int num = (remoteDigitsPressedCount > 0) ? getRemoteNumericValue() : 0;           
        if (inputSource >= SOURCE_RADIO)
        {
            // radio mode
            if (num > 0)                  
            {
                // goto freq entered
                if (num >= radioMinFreq && num <= radioMaxFreq)
                {
                    gotoRadioFreq(num);
                    isValid = TRUE;
                    shouldBeep = FALSE;
                }
            }
            else {
                // no freq entered, scan next frequency
                gotoRadioNextFreq();   
                isValid = TRUE;
                shouldBeep = FALSE;
            }
        }
        else {
            // MP3 mode
            if (!isVBRFile && (remoteDigitsPressedCount == 4 || remoteDigitsPressedCount == 6) )
            {                
                // seek to location in mmss or hhmmss format
                // not applicable to VBR file
                unsigned char hour = 0;
                unsigned char min = 0;
                unsigned char sec = 0; 

                hour = num / 10000;
                min = (num / 100) % 100;
                sec = num % 100;

                if (hour >= 0 && hour <= 23 && min >=0 && min <= 59 && sec >= 0 && sec <= 59)
                {                
                    unsigned int seekToDuration = hour * 3600 + min * 60 + sec;
                    if (seekToDuration >= 0 && seekToDuration < totalDuration)
                    {
                        sprintf(strResBuf, "Seek: %u sec", seekToDuration);                        
                        drawUIMessage(strResBuf);

                        adjustToDuration(seekToDuration);
                        isValid = TRUE;
                    }
                }
            }
            else {
                // no location entered, fast Forward     
                adjustPlayPosition(REW_FW_DURATION_SEC, TRUE);   
                isValid = TRUE;
                isAdjust = TRUE;
            }
        }

        if (!isValid)
        {
            drawUIMessage("Seek error");    
            shortBeep();

            shouldBeep = FALSE;
        }
        
        remoteDigitsPressedCount = 0;
        drawRemoteInput();
        if (!isAdjust)
        {
            resetMenuOption();
        }
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_BACKSPACE)
    {
        // delete 1 remote char
        if (remoteDigitsPressedCount > 0)
            remoteDigitsPressedCount--;
        
        drawRemoteInput();
        resetMenuOption();
        LCD_updateDisplay();
    }    
    else if (isDigitInput)
    {
        remoteDigitsPressed[remoteDigitsPressedCount] = lastRemoteKey;
        remoteDigitsPressedCount++;
        
        if (remoteDigitsPressedCount > MAX_REMOTE_DIGITS_PRESSED)
            remoteDigitsPressedCount = 0;
        
        drawRemoteInput();
        resetMenuOption();
        LCD_updateDisplay();
    }
    else if (lastRemoteKey == KEY_HOME)
    {                
        resetMenuOption();
        LCD_updateDisplay();
    }
    else {
        shouldBeep = FALSE;
    }
        
    // play beep for most remote control keys
    if (shouldBeep && !disabledBeep && !isVolumeUp && !isVolumeDown && !isRewind && !isFastForward)
    {
        tinyBeep(FALSE);
    }        
}
// </editor-fold>

// Main app entry point
int main()
{               
    char buf[32];
    
    initIO();  
    
    // wait for clock to stabilize
    delay_ms(10);    
    SendUARTStr("INIT");
    
    // turn on power, this might cause surge on the power supply lines (amplifier starting) and cause PIC to reset (might be solved by adding capacitor etc.)
    // if it's done almost immediately after main, it's still ok and does not cause surge. Make sure the delay above is short.
    OUTPUT_PWR_ON = TRUE;        
    togglePeripherals(TRUE);
    setKeypadAnalog(TRUE);    
    
    // timer 1 for RTC, must be inited first
    initTmr1();
    
    // default RTC value
    curSecond = 0;
    curMinute = 0; 
    curHour = 0; 
    curDay = 1;         
    curMonth = 1;
    curYear2k = 0;
            
    initADC();     
            
    totalRunningInactiveSecs = 0;
    totalRunningSecs = 0;
                    
    initPWM_native(PWM_VALUE);
               
    // SPI 2 for CH376S, 8-bit, @ 12MHz, CH376 supports up to 24MHz
    spi2Init(0b100111, 1, 0, 1, 1);   
       
    // LCD init, after a short delay for RST capacitor to charge up. Refer to st7565r_init() method for details.
    delay_ms(500);     
    SetUARTPin(FALSE);              // Not sure why we need to disable UART pin again here before init'ing LCD
                                    // By right UART should have already been disabled after the last write
                                    // But calling this method again here makes things work!    
                             
    // Entry point every time user presses power button to turn device on
beginProgram:             
    st7565r_soft_reset();    
    st7565r_init();       
    st7565r_set_all_pixels(FALSE);
        
    shouldSetRTC = !hasSetRTC;
    isEditingConfig = FALSE;

    delay_ms(500);                          // delay for crystal to startup and peripherals to be ready after powered is applied
               
    SendUARTStr("POST");
            
    // perform boot up POST
    BOOL canSystemRun = performPOSTTests();
    if (canSystemRun)
    {
        // normal processing after POST, only if system is not halted due to error
        LCD_cls();

        if (isPendingConfig)
        {        
            // volume level restored here, otherwise the POST beep will be muted if volume is mutes
            if (appConfigObj.config.curVolumeLevel >= 0 && appConfigObj.config.curVolumeLevel < MAX_VOL_LVL * 2)
            {
                volumeLevel = appConfigObj.config.curVolumeLevel;
                debugPrint("VL:%d", volumeLevel);            
            }
        }
        
        if (inputSource < SOURCE_RADIO)
        {
            mp3PlaybackState = MP3_IDLE_STATE;
            wavPlaybackState = WAV_IDLE_STATE;

            // default root folder, first file
            // if a valid config entry is found, navigate to the previously stored file/folder if it's not already the first file
            // as well as restoring the previous volume level
            curFileIndex = 0;
            curFolderIndex = 0;
            if (isPendingConfig)
            {        
                if (appConfigObj.config.curPlayMode >= 0 && appConfigObj.config.curPlayMode < TOTAL_PLAY_MODES)
                {
                    curPlayMode = appConfigObj.config.curPlayMode;
                }

                // check for folders count only, do not check for file count
                // as file count won't be known until we opened the folder
                // if the file count is invalid, the code will simply go to next file and do no harm
                // for folder count, index 1 is first sub-folder, index 0 is root folder, hence the '<=' sign        
                if  ( 
                       (inputSource == SOURCE_SD && (appConfigObj.config.sdSelectedFile > 0 || appConfigObj.config.sdSelectedFolder > 0) && (appConfigObj.config.sdSelectedFolder <= numOfFoldersInRoot) ) ||
                        (inputSource == SOURCE_USB && (appConfigObj.config.usbSelectedFile > 0 || appConfigObj.config.usbSelectedFolder > 0) && (appConfigObj.config.usbSelectedFolder <= numOfFoldersInRoot) )
                     )

                {
                    // debugPrint("Reload T#%u F#%u", curFileIndex, curFolderIndex);

                    curFileIndex = (inputSource == SOURCE_SD ? appConfigObj.config.sdSelectedFile : appConfigObj.config.usbSelectedFile);
                    curFolderIndex = (inputSource == SOURCE_SD ? appConfigObj.config.sdSelectedFolder : appConfigObj.config.usbSelectedFolder);
                }    
                else {
                    // debugPrint("Not loading previous file");
                }

                // only restore previous duration if significant
                if (
                     (inputSource == SOURCE_SD && appConfigObj.config.sdFileDuration > REW_FW_DURATION_SEC) ||
                     (inputSource == SOURCE_USB && appConfigObj.config.usbFileDuration > REW_FW_DURATION_SEC)
                   )
                {
                    isPendingSeek = TRUE;
                }

                // backup params belongs to the other input mode
                bkFileIndex = (inputSource != SOURCE_SD ? appConfigObj.config.sdSelectedFile : appConfigObj.config.usbSelectedFile);
                bkFolderIndex = (inputSource != SOURCE_SD ? appConfigObj.config.sdSelectedFolder : appConfigObj.config.usbSelectedFolder);
                bkPrevDuration = (inputSource != SOURCE_SD ? appConfigObj.config.sdFileDuration : appConfigObj.config.usbFileDuration);
                // debugPrint("BK T#%u F#%u D %u", bkFileIndex, bkFolderIndex, bkPrevDuration);

                if (appConfigObj.config.isPaused)
                {
                    isPendingPause = TRUE;
                }

                isPendingConfig = FALSE;
            }

            openFolder(curFolderIndex);
            openFile(curFileIndex);
        }
    }
 
    // main loop begins
    SendUARTStr("MAIN");
    
    // debugPrint("%u,%u,%u", EEPROM_CFG_DATA_PAGE, EEPROM_MW_PRESETS_OFFSET, EEPROM_FM_PRESETS_OFFSET);
    // debugPrint("%u,%u,%u,%u", EEPROM_SW_PRESETS_OFFSET, EEPROM_LW_PRESETS_OFFSET, EEPROM_SSB_PRESETS_OFFSET,EEPROM_SELF_TEST_PAGE);
                
    isPwrBtnPressed = FALSE;
    lastPwrBtnPressed = totalRunningSecs;    
    isTFMSInt = FALSE;
    shouldUpdateRadioScreen = TRUE;
    lastRemoteChar = KEY_INVALID;
    shouldChangeSource = FALSE;
    while(1)
    {
        if (isInStandby)
        {
            if (shouldDrawClock)
            {
                // 8MHz clock, good enough to operate LCD while saving battery
                setOperatingMode(OP_MODE_LOW_SPEED);
                                
                if (alarmEnabled && isPlayingAlarm)
                {   
                    unsigned char oldPwrOn = OUTPUT_PWR_ON;
                                        
                    // turn on peripherals power for alarm speaker 
                    // delay a while for stability
                    OUTPUT_PWR_ON = TRUE;
                    delay_ms(100);
            
                    // default device unselected
                    toggleCsLines(TRUE);
                            
                    // enable pull-up while power is applied
                    // otherwise bus will be unstable
                    setPullUpDownStatus(TRUE);
                                                            
                    // if power was not on previously, we reset the USB controller chip
                    // otherwise powering on without resetting USB chip will cause excessive current consumption
                    if (!oldPwrOn)
                    {
                        delay_ms(100);
                        SPI2STATbits.SPIEN = 1;                    
                        
                        setFatFsIsUsb(TRUE);
                        ch376_clearvars();  
                        initFatFS();
                        delay_ms(100);
                        resetFatFS();                        
                        
                        // Must reset first, otherwise sleep has no effect
                        // Reset also put the USB drive to rest and reduce power consumption
                        ch376_reset();                      
                        delay_ms(5);                        // Wait a bit before sleeping
                        
                        // TODO: Normally sleeping the CH376 after resetting would reduce the power consumption further
                        // We observe this to be working (reduce 18mA) in high speed mode
                        // In Low speed (8MHz mode), sleeping has no effect, most likely to do with CS line timing issues
                        // We will have to accept this for now
                        ch376_sleep();
                        delay_ms(5);                        // Wait until all SPI transmission (hardware) is done before disabling SPI   
                        
                        SPI2STATbits.SPIEN = 0;                        
                        delay_ms(100);
                    }
                                                                                                                   
                    // update alarm clock                     
                    SendUARTStr("ALS");                       
                    LCD_cls();
                    drawFullScreenClock(FALSE);
                    LCD_updateDisplay();
                    
                    // long beep for a number of times
                    isPwrBtnPressed = FALSE;
                    unsigned char alarmCnt = 0;
                    BOOL isShortBeep = TRUE;
                    for (alarmCnt = 0; alarmCnt < 250; alarmCnt++)
                    {
                        if (alarmCnt % 3 == 0 && alarmCnt > 0)
                        {
                            isShortBeep = !isShortBeep;
                            delay_ms(200);
                        }                        
                        if (isShortBeep)
                        {
                            tinyBeep(FALSE);
                            delay_ms(100);
                        }
                        else {
                            shortBeep();
                            delay_ms(50);
                        }

                        // flashing alarm display
                        if (alarmCnt%2 == 0)
                        {
                            LCD_drawString(15, STATUS_LINE_Y, ALARM_PLAYING_STR,  1);                            
                        }
                        else {
                            LCD_drawString(15, STATUS_LINE_Y, EMPTY_PLAYING_STR,  1);
                        }    
                        LCD_updateStatusText();
                        
                        // update clock time periodically also, otherwise clock will appear stuck while alarm is running
                        if (alarmCnt % 40 == 0 && alarmCnt > 0)
                        {
                            drawFullScreenClock(FALSE);
                            LCD_updateDisplay();
                        }

                        // PWR or remote key to cancel alarm
                        if (isPwrBtnPressed)
                        {
                            SendUARTStr("ALK");
                            isPwrBtnPressed = FALSE;
                            break;
                        }                   
                    }                    
                    isPlayingAlarm = FALSE;
                    SendUARTStr("ALE");      
                    
                    // alarm done, restore display with full clock
                    LCD_cls();
                    drawFullScreenClock(TRUE);
                    
                    // set CS line to low again, we are done and power will be removed shortly
                    toggleCsLines(oldPwrOn);
                    
                    // disable pull-up/down, now that power is removed
                    setPullUpDownStatus(oldPwrOn);
                    
                    // disable peripheral power, alarm done
                    // delay a while for stability
                    OUTPUT_PWR_ON = oldPwrOn;
                    delay_ms(1000);
                    isPwrBtnPressed = FALSE;
                    
                }
                else {
                    // update normal clock
                    drawFullScreenClock(TRUE);                                
                }
                shouldDrawClock = FALSE;
                
                // revert to low power clock
                setOperatingMode(OP_MODE_STANDBY);
            }
            
            // if mixer mode (off but analog remains on) has been on for too long, we turn off peripheral powers
            if (OUTPUT_PWR_ON && totalRunningInactiveSecs > MAX_MIXER_MODE_DURATION_SECS)
            {
                // we didn't do this when CPU was shutdown (otherwise there will be excessive current consumption)
                // we will do it now
                toggleCsLines(FALSE);
                OUTPUT_PWR_ON = FALSE;
            }
            
            if (shouldExitStandby)
            {
                // turn on power before writing to EEPROM, as it's using 3V3SD power source
                OUTPUT_PWR_ON = TRUE;
                
                // switch to low clock speed enough to update EEPROM
                setOperatingMode(OP_MODE_LOW_SPEED);
                
                // wait a bit for power to become stable, otherwise EEPROM write will fail!
                // must do this after changing clock, otherwise delay_ms in low clock mode will take forever!
                delay_ms(200);                          
                        
                // update EEPROM
                writeAppConfig(inputSource);                    
                
                // exiting standby mode simply by resetting PIC
                // we tried to change clock from LP or non-PLL to PLL which still crashes
                // we might as well reset
                resetPIC();
                
                // the following code is no longer relevant
                shouldExitStandby = FALSE;
                isInStandby = FALSE;
                
                // reset states
                ch376_clearvars();                    
                resetFatFS();
                
                // start from beginning, as if rebooting
                goto beginProgram;
            }
            else {
                // in 32kHz standby mode, draw clock only   
                // SendUARTStr("in standby");
                // delay_ms(100);
            }
        }
        else {
            if (canSystemRun)
            {
                // system not halted, in normal mode, main progress                
                if (inputSource < SOURCE_RADIO)
                {
                    processMediaFile();        
                    updateMediaScreen();
                }
                else {
                    updateRadioScreen();
                }
                
                // handle alarm while not sleeping
                if (alarmEnabled && isPlayingAlarm)
                {
                    SendUARTStr("ALS");
                    setKeypadAnalog(FALSE);     // so that pressing PWR will trigger interrupt to stop alarm
                    
                    // stop any current recording
                    BOOL wasRecording = (radioRecordingMode != RECORD_MODE_NONE);
                    if (wasRecording)
                    {
                        stopAnyCurRadioRecording();
                        delay_ms(1000);
                    }
                    
                    // mute the radio if in radio mode
                    // if in media player mode, no need to mute as delaying here for beeping will stop all playback activities
                    remoteDigitsPressedCount = 0;
                    drawRemoteInput();
                    if (inputSource >= SOURCE_RADIO && !isPaused)
                    {
                        si473x_setVolume(0);
                    }
                                        
                    isPwrBtnPressed = FALSE;
                    isTFMSInt = FALSE;
                    lastRemoteChar = KEY_INVALID;
                    
                    // long beep for a number of times
                    unsigned char alarmCnt = 0;
                    for (alarmCnt = 0; alarmCnt < 250; alarmCnt++)
                    {
                        tinyBeep(TRUE);
                        
                        // flashing alarm display
                        if (alarmCnt%2 == 0)
                        {
                            drawMenuInfo(ALARM_PLAYING_STR);                            
                        }
                        else {
                            drawMenuInfo(EMPTY_MENU_LINE);
                        }
                        drawTopClock();
                        LCD_updateMenuText();
                        LCD_updateTopMiddleInfo();                      
                                                
                        // PWR or remote key to cancel alarm
                        if (isPwrBtnPressed || (lastRemoteChar != KEY_INVALID) )
                        {
                            SendUARTStr("ALK");
                            isPwrBtnPressed = FALSE;
                            isTFMSInt = FALSE;
                            lastRemoteChar = KEY_INVALID;
                            break;
                        }                   
                    }
                                                
                    // restore volume if in radio mode
                    if (inputSource >= SOURCE_RADIO && !isPaused)
                    {
                        setRadioVolume(volumeLevel);
                    }
                                                                   
                    isPlayingAlarm = FALSE;
                    setKeypadAnalog(TRUE);     
                    isPwrBtnPressed = FALSE;        // otherwise random interrupt might trigger
                    isTFMSInt = FALSE;
                    lastRemoteChar = KEY_INVALID;                    
                    SendUARTStr("ALE");                    
                    
                    drawMenuInfo(EMPTY_MENU_LINE);
                    LCD_updateDisplay();             
                }
                
                // must be before isPendingConfig check, so that file can be opened first before seeking
                if (isPendingSeek && totalDuration > 0)
                {                                                                              
                    // seek to location
                    unsigned int seekDur = (inputSource == SOURCE_SD ? appConfigObj.config.sdFileDuration : appConfigObj.config.usbFileDuration);
                    if (seekDur < totalDuration)
                    {
                        adjustToDuration(seekDur);
                    }
                    isPendingSeek = FALSE;
                }

                // if previous config says file was paused, we paused the file while it's playing
                if (isPendingPause && totalDuration > 0 && !isPaused)
                {
                    processRemoteKey(KEY_OK);
                    isPendingPause = FALSE;            
                }

                // remote button pressed, debouncing by limiting duration between button press
                if (lastRemoteChar != KEY_INVALID && TMR3_LAPSED_x10MS() > REMOTE_DEBOUNCE_MS)
                {
                    // debugPrint("pr: %d", lastRemoteChar);

                    processRemoteKey(lastRemoteChar);

                    lastRemoteChar = KEY_INVALID;            
                    TMR3_DELAY_CNT = 0;            
                }

                // remote decode timeout, reset state
                if (TMR3_LAPSED_x10MS() > REMOTE_TIMEOUT_MS && remoteState != REMOTE_IDLE)
                {
                    // SendUART('&');

                    resetTFMSVars();            
                }                             
            }
                                 
            // standby (switch to 31kHz) if required
            if (shouldEnterStandby)
            {
                if (inputSource >= SOURCE_RADIO)
                {
                    stopAnyCurRadioRecording();
                }
                
                // update app config first
                ch376_clearvars();                    
                resetFatFS();
                
                // never default to radio recording mode (waste power)
                if (inputSource == SOURCE_RADIO_REC)
                {
                    inputSource = SOURCE_RADIO;
                }
                
                writeAppConfig(inputSource);

                shouldEnterStandby = FALSE;
                totalRunningInactiveSecs = 0;

                // goto standby
                // SendUARTStr(isMixerMode ? "M" : "U");
                enterStandbyMode(keepPwrOnInStandby);                                
                keepPwrOnInStandby = FALSE;                
                
                // ignore the last section of the while to avoid float processing of power button
                continue;
            }
            
            // analog keyboard, process even if system is halted, only if not recording
            // if recording, analog keyboard acts as digital, any key to stop recording
            // delay for debouncing so as not to process stuck key repeatedly
            if (radioRecordingMode != RECORD_MODE_NONE)
            {
                // queue almost full, update recording file
                if (queue_get_len_left() > BUFFER_TOPUP_THRESHOLD)
                {
#define WAV_WRITE_BUF_LEN   128
                    unsigned int qcnt = 0;
                    BOOL isQueueAvail = FALSE;
                    for (qcnt = 0; qcnt < BUFFER_TOPUP_THRESHOLD / WAV_WRITE_BUF_LEN; qcnt++)
                    {
                        unsigned int ind = 0;
                        unsigned int count = 0;
                        int temp = 0;
                        int recBuf[WAV_WRITE_BUF_LEN];
                        for (ind = 0; ind < WAV_WRITE_BUF_LEN; ind++)
                        {
                            isQueueAvail = queue_get_left(&temp);
                            if (isQueueAvail)
                            {
                                // adjust DC offset (around 2V)
                                // For 12-bit signed ADC, e.g. -2048 .. 2047
                                // e.g. 2 / 3.3 * 2^11 = 1240;
                                // adjusted from 12 to 16 bit, e.g. x16
                                // divided by 2 to center around mid-point
                                recBuf[count] = (temp - WAV_DC_OFFSET_ADJUST);
                                count++;
                            }
                            else 
                            {
                                break;                        
                            }
                        }
                        
                        if (count > 0)
                        {
                            UINT bytesWritten = 0;
                            FRESULT fWRes = f_write(&fMediaFile, recBuf, count*2, &bytesWritten);
                            if (fWRes != FR_OK || bytesWritten < (count*2) )
                            {
                                debugPrint("wrEr:%d", fWRes);
                                                                                    
                                // in recording mode, pwr btn will stop recording
                                // we mark as pressed so that recording will stop upon an error
                                isPwrBtnPressed = TRUE;
                            }
                            else {                                                                                            
                                // debugPrint("wrOk:%ub",bytesWritten);
                                totalRecWords += count;
                            }
                            
                            // stop recording if duration exceeds
                            unsigned int totalSec = totalRecWords / RECORDING_FREQ;
                            if (totalSec > RECORDING_MAX_LEN_SEC && !isPwrBtnPressed)
                            {
                                SendUARTStr("RcMx");
                                isPwrBtnPressed = TRUE;
                            }
                        }
                        
                        if (!isQueueAvail)
                            break;
                    }
                }
            }
            else {
                if (isKbAdcOK)
                {
                    unsigned char retKey = getKeyboardInput();           
                    BOOL debouncePassed = (TMR3_LAPSED_x10MS() > REMOTE_DEBOUNCE_MS);
                    if (debouncePassed)
                    {    
                        if (retKey != 0)
                        {
                            TMR3_DELAY_CNT = 0;       
                        }                    
                        if (retKey == KEY_SOURCE)
                        {                                
                            // SendUARTStr("c00");   
                            shouldChangeSource = TRUE;                 
                        }
                        else if (retKey == KEY_RESET_POWER)
                        {                                  
                            isPwrBtnPressed = TRUE;    
                        }
                        else if (canSystemRun && retKey != 0) {
                            // SendUARTStr("c");

                            // other keys, only available if system is not halted                        
                            processRemoteKey(retKey);   
                        }
                    }
                }
            }
            
            // if system hangs because of an error, any remote control key is considered as changing source
            if (!canSystemRun && (lastRemoteChar != KEY_INVALID) )
            {
                lastRemoteChar = KEY_INVALID;
                shouldChangeSource = TRUE;
                
                // SendUARTStr("c2");
            }

            if (shouldChangeSource)
            {                
                lastUpdateRadioWeather = 0;
                enableAudio(FALSE);
                INPUT_SOURCE oldSrc = inputSource;

                // next is radio with recording, don't power down yet
                BOOL isCurRadio = (inputSource == SOURCE_RADIO);

                BOOL isCurRadioRec = (inputSource == SOURCE_RADIO_REC);
                if (isCurRadioRec)
                {
                    // next is SD, power off radio
                    si473x_radioPowerDown();
                }

                // switching source, available when system is halted
                inputSource = (inputSource + 1) % MAX_INPUT_SOURCES;                                

                getInputSourceStr(inputSource, buf);
                sprintf(strResBuf, "Loading %s ...", buf);

                if (!canSystemRun)
                {
                    LCD_cls();
                }

                drawMenuInfo(strResBuf);
                LCD_updateDisplay();

                // update config before switching
                writeAppConfig(oldSrc);                

                // if we are in low clock radio mode (not recording), might as well reset
                // because switching clock to higher clock will also cause reset!
                if (isCurRadio)
                {
                    // need to toggle power as well, otherwise SD card will somehow hang!
                    togglePeripherals(FALSE);
                    setPullUpDownStatus(FALSE);
                    OUTPUT_PWR_ON = FALSE;
                    delay_ms(500);
                    resetPIC();
                }
                else {
                    // delay just in case
                    delay_ms(500);
                }

                // src changed, reset vars
                ch376_clearvars();                    
                resetFatFS();

                // reset remote state, else source might change several times!
                isTFMSInt = FALSE;
                lastRemoteChar = KEY_INVALID;
                shouldChangeSource = FALSE;

                // start from beginning, as if rebooting
                goto beginProgram;     
            }
        }
                       
        // power button pressed, switch between 31kHz and normal mode
        // pwr btn also used to cancel radio seek, do not handle that case
        if ( (isPwrBtnPressed && !isRadioSeeking) && (totalRunningSecs - lastPwrBtnPressed > 2) )
        {                   
            if (radioRecordingMode != RECORD_MODE_NONE)
            {
                // SendUARTStr("Pwr");
                            
                // any hardware key during recording to stop recording
                stopAnyCurRadioRecording();
            }
            else {
                // ON/OFF button
                lastPwrBtnPressed = totalRunningSecs;            

                if (isInStandby)
                {
                    // don't delay here, delay count is wrong is 32kHz mode

                    // power on
                    shouldExitStandby = TRUE;
                    shouldEnterStandby = FALSE;
                }
                else {
                    // button debouncing
                    delay_ms(500);
                    isPwrBtnPressed = FALSE;

                    // power off
                    shouldExitStandby = FALSE;
                    shouldEnterStandby = TRUE;
                }
            }
        }
        else {
            isPwrBtnPressed = FALSE;
        }
    }
    
    return 0;
}