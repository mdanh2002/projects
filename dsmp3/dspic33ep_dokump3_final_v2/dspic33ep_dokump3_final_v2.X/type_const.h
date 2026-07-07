// boolean
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

// common types
#define INT32  signed long int
#define UINT32 unsigned long int
#define UINT16 unsigned int
#define INT16  signed int
#define UINT8  unsigned char
#define INT8  unsigned char

// MP3 ID3v1
#define MAX_ID3_LEN 30
#define MAX_ID3_YEAR_LEN 4
typedef struct _MP3Info {
	char tag[3];				/* = "TAG" */
	char title[MAX_ID3_LEN];
	char artist[MAX_ID3_LEN];
	char album[MAX_ID3_LEN];
	char year[MAX_ID3_YEAR_LEN];
	char comment[MAX_ID3_LEN];
	unsigned char genre;
} MP3Info;

// http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
// WAVE file header format
typedef struct _WAV_HEADER {
	unsigned char riff[4];						// RIFF string
	UINT32  overall_size;				// overall size of file in bytes
	unsigned char wave[4];						// WAVE string
	unsigned char fmt_chunk_marker[4];			// fmt string with trailing null char
	UINT32 length_of_fmt;					// length of the format data
	UINT16 format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	UINT16 channels;						// no.of channels
	UINT32 sample_rate;					// sampling rate (blocks per second)
	UINT32 byterate;						// SampleRate * NumChannels * BitsPerSample/8
	UINT16 block_align;					// NumChannels * BitsPerSample/8
	UINT16 bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_chunk_header [4];		// DATA string or FLLR string
	UINT32 data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} WAV_HEADER;                
WAV_HEADER wavHeader;

// WAV state machine
typedef enum 
{
    WAV_IDLE_STATE,
    WAV_OPENING,   
    WAV_PLAYBACK,
    WAV_WAIT_PLAYBACK,
    WAV_FILE_CLOSE
} WAV_PLAYBACK_STATE;
WAV_PLAYBACK_STATE wavPlaybackState;

// MP3 state machine
typedef enum 
{
    MP3_IDLE_STATE,
    MP3_OPENING,
    MP3_DECODE,            
    MP3_WAIT_PLAYBACK,
    MP3_FILE_CLOSE    
} MP3_PLAYBACK_STATE;
MP3_PLAYBACK_STATE mp3PlaybackState;

// For use with replacement Remote Control for Original MXQ Pro 4k M8S Android Smart TV Box which runs on AA battery and has strong signal
// remote key enum
// except for digit keys, for which we use the enum values to concatenate the digits entered
// the remaining values are not critical. Take note not to use 0xFF, reserved for invalid keys
typedef enum 
{
    KEY_0 = '0',            
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',    
    KEY_DOWN = 'A',
    KEY_APP = 'B',
    KEY_UP = 'C',
    KEY_LEFT = 'D',
    KEY_RIGHT = 'E',
    KEY_OK = 'F',
    KEY_COLOR_VOLM = 'G',            
    KEY_COLOR_VOLP = 'H',            
    KEY_SETUP = 'I',
    KEY_LEFT_OPT = 'J',
    KEY_RIGHT_OPT = 'K',
    KEY_SOURCE = 'L',
    KEY_RESET_POWER = 'M',            
    KEY_SET = 'N',
    KEY_RED_OFF = 'O',
    KEY_GRAY_OFF = 'P',
    KEY_TV_IN = 'Q',
    KEY_BLACK_VOLM = 'R',
    KEY_BLACK_VOLP = 'S',
    KEY_HOME = 'T',
    KEY_PREVIOUS = 'U',
    KEY_MUTE = 'V',
    KEY_BACKSPACE = 'W',
            
    KEY_INVALID = 255
} KEYCODE;

// remote status
typedef enum
{
    REMOTE_IDLE,
    REMOTE_INIT_LOW,
    REMOTE_INIT_HIGH,
    REMOTE_SIGN1,        
    REMOTE_SIGN2,        
    REMOTE_DATABYTE1,                
    REMOTE_DATABYTE2,
    REMOTE_EXTRADATA
} REMOTE_STATE;

// raw remote codes and associated keys
// do not use 0xFF (255), which is the marked for invalid key or no key pressed
__prog__ static const unsigned char __attribute__((section("rawRemoteCodes"), space(prog))) rawRemoteCodes[] = {
    1, KEY_0,    
    78, KEY_1,
    13, KEY_2,
    12, KEY_3,
    74, KEY_4,
    9, KEY_5,
    8, KEY_6,
    70, KEY_7,
    5, KEY_8,
    4, KEY_9,    
    26, KEY_DOWN,
    15, KEY_APP,
    22, KEY_UP,
    81, KEY_LEFT,
    80, KEY_RIGHT,
    19, KEY_OK,
    16, KEY_COLOR_VOLM,
    24, KEY_COLOR_VOLP,
    67, KEY_SETUP,
    76, KEY_LEFT_OPT,
    0, KEY_RIGHT_OPT,
    242, KEY_SET,
    64, KEY_RED_OFF,
    241, KEY_GRAY_OFF,
    243, KEY_TV_IN,
    244, KEY_BLACK_VOLM,
    245, KEY_BLACK_VOLP,
    17, KEY_HOME,
    25, KEY_PREVIOUS,    
    65, KEY_MUTE,
    66, KEY_BACKSPACE
};

// both the array and the individual strings are stored constants, hence both "const" keywords are used
const char const *daysOfWeek[7] = {
    "Sun",    
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

// Each line should have exactly the same length due to the way we are reading the lines
// We cannot store the lines as array of strings, because the arrays are in program memory which requires _strncpy_p2d16
// to copy the data to RAM. This macro however has issues with working with string arrays.
#define MAX_HELP_MENU_LINES     10
#define HELP_LINE_LEN           32       
__prog__ static const char const __attribute__((section("helpMenuMedia"), space(prog))) helpMenuMedia[] =  
    "RedPwr: PwrOff, GrayPwr: Standby"\
    "XXX+APP(OK): Open Folder/File No"\
    "SETUP: Menu (APP: Select Menu)  "\
    "ddmmyy/hhmm+SETUP: Set Date/Time"\
    "hhmm+SET: Alarm Time(SET=Toggle)"\
    "<,>:Track ^,v:Folder OK:Pause   "\
    "CT1/CT2: Rewind/Fast Forward    "\
    "mmss+CT2: Seek (Pause = SavePos)"\
    "M-/M+: File Details & ID3 tags  "\
    "Supports: MP3/WAV, FAT16/FAT32  "
;    

__prog__ static const char const __attribute__((section("helpMenuRadio"), space(prog))) helpMenuRadio[] = 
    "RedPwr: PwrOff, GrayPwr: Standby"\
    "<,>:Prev/Next, xxxx+OK:Preset No"\
    "ddmmyy/hhmm+SETUP: Set Date/Time"\
    "hhmm+SET: Alarm Time(SET=Toggle)"\
    "APP:NextBand, xxxx+>:Save Preset"\
    "^,v:AutoScan/BFO, XXX+C2:Go Freq"\
    "SETUP:Menu(APP:Enter),C1/C2:Seek"\
    "Timezone: ZZ,M+:+ZZ / ZZ,M-:-ZZ "\
    "OK:SSB/StopRec, M+/M-: FreqInfo "\
    "ZXXXYY,M+/M-: Lng/Lat Z=0/1:-/+ "
;

// menu items
enum {
    MENU_ITEM_SOURCE,
    MENU_ITEM_PLAY_MODE,
    MENU_ITEM_DURATION_MODE,
    MENU_ITEM_RADIO_BAND,
    MENU_ITEM_RADIO_FIND_STN_INFO,
    MENU_ITEM_RADIO_VIEW_STN_INFO,
    MENU_ITEM_RADIO_SCAN_FAST,
    MENU_ITEM_RADIO_SCAN_ALL,
    MENU_ITEM_RADIO_FINE_TUNE,
    MENU_ITEM_RADIO_REC_USB,
    MENU_ITEM_RADIO_REC_SD,
    MENU_ITEM_RADIO_STOP_REC,
    MENU_ITEM_FM_RANGE,
    MENU_ITEM_FM_DEEMP,
    MENU_ITEM_FM_SPACING,
    MENU_ITEM_AM_SPACING,
    MENU_ITEM_AM_BANDWIDTH,
    MENU_ITEM_SSB_BANDWIDTH,
    MENU_ITEM_SSB_SYNC,
    MENU_ITEM_SSB_MODE,    
    MENU_ITEM_FORCE_MONO,
    MENU_ITEM_POWER_TYPE,
    MENU_ITEM_POWER_OFF_IDLE,
    MENU_ITEM_POWER_OFF_ALWAYS,
    MENU_ITEM_ALARM,
    MENU_ITEM_BEEPER,
    MENU_ITEM_EDIT_CONFIG,
    MENU_ITEM_SHUTDOWN_KEEP_PWR,    
    MENU_ITEM_SHUTDOWN_FULL,
    // MENU_ITEM_SLEEP,
    MENU_ITEM_HELP,
    
    MENU_ITEM_INVALID
} curMenuOption = MENU_ITEM_INVALID;
#define MAX_VALID_MENU_ITEM     MENU_ITEM_HELP

// AN channel for battery analog input through voltage divider
#define BATTERY_INPUT_AN_PIN    1

// AN channel for keyboard analog input
#define KEYBOARD_INPUT_AN_PIN   3

// refer to performPostTests() and goertzel_mag() function before changing these values

// analog pin for use with recorder input 
#define RECORDER_INPUT_AN_PIN   0

// frequency of generated beep tone in almost except AUX (where the beep is the PWM itself)
#define BEEP_FREQ               (2000)

// timer frequency used for generating beeps
#define BEEP_TMR_FREQ       8000

// if the magnitude of the test tone is below this, consider audio failure
// must be calibrated against trimmer values and test frequency
// it also depends on available current. the magnitude will reduce as the battery level wears down and supplied current reduces 
// bench PSU with current limit set to 500mA might response immediately during boot (e.g. power surge) and reduce available current
// causing the beep amplitude and resulting magnitude to reduce compared with running battery
// values must be set carefully so as not to cause false alarms
#define AUDIO_SELF_TEST_DB_THRESHOLD    70

// type of input
typedef enum
{
    SOURCE_SD = 0,
    SOURCE_USB = 1,
    SOURCE_RADIO = 2,
    SOURCE_RADIO_REC = 3
} INPUT_SOURCE;
#define MAX_INPUT_SOURCES  4

// frequency & bit rate limits
#define MAX_SUPPORTED_FREQ      44100
#define MAX_SUPPORTED_BITRATE   128000
#define HIGH_MP3_FREQ           44100

// recording settings
// oversampling with window average to reduce noise (may not be needed on PCB with low noises)
// Set to MOVING_AVG_COUNT to 1 (and update associated values) to disable
#define RECORDING_DIR_NAME         "REC"
#define MOVING_AVG_COUNT           (4)
#define RECORDING_FREQ             (8000)
#define RECORDING_TIMER_FREQ       (32000)          // cannot put (RECORDING_TIMER_FREQ * MOVING_AVG_COUNT), it will say "integer overflow in expression"
#define RECORDING_MAX_LEN_SEC      (14400)          // max rec duration in                    
#define WAV_DC_OFFSET_ADJUST       (5000)              // DC offset adjustment, calibrated to match ADC trimmer, otherwise certain tools (Media Player Classic) might not like it, but VLC works fine.

// play mode
#define PLAY_MODE_ALL_TRACKS         0
#define PLAY_MODE_REPEAT_TRACK       1
#define PLAY_MODE_REPEAT_FOLDER      2
#define PLAY_MODE_SHUFFLE_FOLDER     3
#define PLAY_MODE_SHUFFLE_ALL        4
#define PLAY_MODE_SINGLE             5
#define TOTAL_PLAY_MODES             6

// duration display mode
#define DURATION_MODE_TOTAL         0
#define DURATION_MODE_ELAPSED       1
#define DURATION_MODE_REMAINING     2
#define TOTAL_DURATION_MODES        3

// power type
#define POWER_TYPE_6_ALKALINE       0
#define POWER_TYPE_6_NIMH           1
#define POWER_TYPE_9V_NIMH          2
#define POWER_TYPE_9V_ALKALINE      3
#define POWER_TYPE_2_18650          4
#define POWER_TYPE_DC               5
#define POWER_TYPE_COUNT            6

// always off settings
#define ALWAYS_OFF_60MIN          0
#define ALWAYS_OFF_120MIN         1
#define ALWAYS_OFF_180MIN         2
#define ALWAYS_OFF_240MIN         3
#define ALWAYS_OFF_NEVER          4
#define ALWAYS_OFF_COUNT          5

// idle off settings
#define IDLE_OFF_5MIN           0
#define IDLE_OFF_15MIN          1
#define IDLE_OFF_30MIN          2
#define IDLE_OFF_NEVER          3
#define IDLE_OFF_COUNT          4

// record mode
#define RECORD_MODE_NONE         0
#define RECORD_MODE_USB          1
#define RECORD_MODE_SD           2
       
// default value of analog input when key is not pressed (connected to 5V, almost near 10-bit max)
#define     ANALOG_KEY_MAX_ADC              3762          // assuming 12-bit ADC
#define     MAX_10B_ADC_STEPS               1023.0        // 10-bit unsigned ADC
#define     MAX_12B_ADC_STEPS               4095.0        // 12-bit unsigned ADC

// max num of chars per line
#define MAX_TITLE_CHAR_SCROLL       14
#define MAX_TITLE_CHAR_SMALL        21
#define MAX_TITLE_CHAR_TINY         32

// click speed mode
#define CLOCK_MODE_LP           0b101           // LPRC, 31.25kHz
#define CLOCK_MODE_FASTRC       0b000           // Fast RC Oscillator (7.37MHz) with no PLL, not used in this project
#define CLOCK_MODE_FASTRCPLL    0b001           // Fast RC Oscillator (7.37MHz) with PLL
#define OP_MODE_FASTEST             0           
#define OP_MODE_LOW_SPEED           1
#define OP_MODE_STANDBY             2

// volume level
#define OVER_AMPLIFY_RATIO  2       // set to 2 to allow magnification, with distortion risks, set to 1 to disable
#define MAX_VOL_LVL         30      // refer to app_config, 5-bits only

// remote
#define MAX_REMOTE_DIGITS_PRESSED 6

// placeholder for invalid time
#define TIME_INVALID 0xFF

// SSB
#define MAX_SSB_BFO_OFFSET_HZ      3000
#define SSB_BFO_OFFSET_STEP_HZ     100

// Define when to topup the PCM queue buffer
#define BUFFER_TOPUP_THRESHOLD   ( (QUEUE_CAPACITY / 2) )

// how many MP3 bytes to read at a time
#define MP3_INPUT_BUF_SIZE          970

// how many WAV bytes to read at a time
#define WAV_FILE_READ_BUFFER_SIZE   1024

// fast forward & rewind duration in seconds
#define REW_FW_DURATION_SEC    15

// how many MP3 decoder erros before giving up
#define MAX_TOTAL_DEC_ERRORS   10

// name of root folder
#define ROOT_FOLDER_NAME "/"

// menu line
#define LOADING_MENU_LINE     "Loading ..."
#define EMPTY_TITLE_LINE      "              "
#define EMPTY_MENU_LINE       "                     "
#define EMPTY_STATUS_LINE     "               "
#define EMPTY_INFO_LINE       "                     "
#define EMPTY_SMALL_INFO_LINE       "                                "
#define MENU_CURSOR_ARROW     ">> "

// menu strings
#define CLOCK_SET_STR           "Clock Not Set"
#define POWER_MENU_STR          "Pwr"
#define IDLEOFF_MENU_STR        "IdleOff"
#define AUTOOFF_MENU_STR        "AutoOff"
#define BEEPER_ON_STR           "Beeper On"
#define BEEPER_OFF_STR          "Beeper Off"
#define FORCE_MONO_STR          "Always Mono"
#define ALLOW_STEREO_STR        "Allow Stereo"
#define ALARM_PLAYING_STR       "PWR = Stop Alarm"
#define EMPTY_PLAYING_STR       "                "

// maximum duration in seconds of the mixer mode (e.g. off with analog on)
#define MAX_MIXER_MODE_DURATION_SECS     7200