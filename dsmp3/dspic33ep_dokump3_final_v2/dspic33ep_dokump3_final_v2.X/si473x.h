/**
 * @brief SI4735 ARDUINO LIBRARY  
 * 
 * @details This is an Arduino library for the SI47XX, BROADCAST AM/FM/SW RADIO RECEIVER IC family from Silicon Labs. 
 * @details This library is intended to provide an easier interface for controlling the SI47XX by using Arduino platform. 
 * @details The communication used by this library is I2C.
 * @details This file contains: const (#define), Defined Data type and Methods declarations
 * @details You can see a complete documentation on <https://github.com/pu2clr/SI4735>
 * @details The are more than 20 examples on <https://github.com/pu2clr/SI4735/tree/master/examples>
 *   
 * @see https://pu2clr.github.io/SI4735/
 * @see Si47XX PROGRAMMING GUIDE AN332 (Rev 1.0): https://www.silabs.com/documents/public/application-notes/AN332.pdf
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES
 * 
 * This library was built based on ?Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0)?. 
 * It also can be used on all members of the SI473X family respecting, of course, the features available 
 * for each IC version.  These functionalities can be seen in the comparison matrix shown in 
 * table 1 (Product Family Function); pages 2 and 3 of the programming guide.
 * 
 * @author PU2CLR - Ricardo Lima Caratti 
 * 
 * @author PU2CLR - Ricardo Lima Caratti 
 * @date  2019-2020
 */

#define POWER_UP_FM 0  // FM
#define POWER_UP_AM 1  // AM and SSB (if patch applyed)
#define POWER_UP_WB 3  // Weather Band Receiver
#define POWER_PATCH 15 

// Active 8-bit SI473X I2C bus address
#define SI473X_ADDR_W SI473X_ADDR_SEN_LOW_W
#define SI473X_ADDR_R SI473X_ADDR_SEN_LOW_R

// 8-bit SI473X I2C bus address when the SEN pin (16) is set to low 0V.
#define SI473X_ADDR_SEN_LOW_W 0x22
#define SI473X_ADDR_SEN_LOW_R 0x23

// 8-bit SI473X I2C bus address when the SEN pin (16) is set to high +3.3V
#define SI473X_ADDR_SEN_HIGH_W 0xC6
#define SI473X_ADDR_SEN_HIGH_R 0xC7

// SI473X commands (general)
#define SI473X_POWER_UP 0x01       // Power up device and mode selection.
#define SI473X_GET_REV 0x10        // Returns revision information on the device.
#define POWER_DOWN 0x11     // Power down device.
#define SET_PROPERTY 0x12   // Sets the value of a property.
#define GET_PROPERTY 0x13   // Retrieves a property's value.
#define GET_INT_STATUS 0x14 // Read interrupt status bits.

// AM Max Gain in dB
#define SI473X_AVC_AM_MAX_GAIN_NORMAL      96
#define SI473X_AVC_AM_MAX_GAIN_SSB         96

// FM Band
#define FM_TUNE_FREQ 0x20
#define FM_SEEK_START 0x21 // Begins searching for a valid FM frequency.
#define FM_TUNE_STATUS 0x22
#define FM_AGC_STATUS 0x27
#define FM_AGC_OVERRIDE 0x28
#define FM_RSQ_STATUS 0x23
#define FM_RDS_STATUS 0x24 // Returns RDS information for current channel and reads an entry from the RDS FIFO.

// FM RDS properties
#define FM_RDS_INT_SOURCE 0x1500
#define FM_RDS_INT_FIFO_COUNT 0x1501
#define FM_RDS_CONFIG 0x1502
#define FM_RDS_CONFIDENCE 0x1503
#define FM_DEEMPHASIS 0x1100
#define FM_BLEND_STEREO_THRESHOLD 0x1105
#define FM_BLEND_MONO_THRESHOLD 0x1106
#define FM_BLEND_RSSI_STEREO_THRESHOLD 0x1800
#define FM_BLEND_RSSI_MONO_THRESHOLD 0x1801
#define FM_BLEND_SNR_STEREO_THRESHOLD 0x1804
#define FM_BLEND_SNR_MONO_THRESHOLD 0x1805
#define FM_BLEND_MULTIPATH_STEREO_THRESHOLD 0x1808
#define FM_BLEND_MULTIPATH_MONO_THRESHOLD 0x1809

// FM Seek Properties
#define FM_SEEK_BAND_BOTTOM 0x1400         // Sets the bottom of the FM band for seek
#define FM_SEEK_BAND_TOP 0x1401            // Sets the top of the FM band for seek
#define FM_SEEK_FREQ_SPACING 0x1402        // Selects frequency spacing for FM seek
#define FM_SEEK_TUNE_SNR_THRESHOLD 0x1403  // Sets the SNR threshold for a valid FM Seek/Tune
#define FM_SEEK_TUNE_RSSI_THRESHOLD 0x1404 // Sets the RSSI threshold for a valid FM Seek/Tune

// AM command
#define AM_TUNE_FREQ 0x40    // Tunes to a given AM frequency.
#define AM_SEEK_START 0x41   // Begins searching for a valid AM frequency.
#define AM_TUNE_STATUS 0x42  // Queries the status of the already issued AM_TUNE_FREQ or AM_SEEK_START command.
#define AM_RSQ_STATUS 0x43   // Queries the status of the Received Signal Quality (RSQ) for the current channel.
#define AM_AGC_STATUS 0x47   // Queries the current AGC settings.
#define AM_AGC_OVERRIDE 0x48 // Overrides AGC settings by disabling and forcing it to a fixed value.
#define GPIO_CTL 0x80        // Configures GPO1, 2, and 3 as output or Hi-Z.
#define GPIO_SET 0x81        // Sets GPO1, 2, and 3 output level (low or high).

// SSB command (SAME AM CMD VALUES)
// See AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 4 and 5
#define SSB_TUNE_FREQ 0x40    // Tunes to a given SSB frequency.
#define SSB_TUNE_STATUS 0x42  // Queries the status of the already issued SSB_TUNE_FREQ or AM_SEEK_START command.
#define SSB_RSQ_STATUS 0x43   // Queries the status of the Received Signal Quality (RSQ) for the current channel.
#define SSB_AGC_STATUS 0x47   // Queries the current AGC settings.
#define SSB_AGC_OVERRIDE 0x48 // Overrides AGC settings by disabling and forcing it to a fixed value.

// AM/SW/LW Receiver Property Summary
// See  Si47XX PROGRAMMING GUIDE AN332 (REV 1.0); page 125
#define DIGITAL_OUTPUT_FORMAT 0x0102                // Configure digital audio outputs.
#define DIGITAL_OUTPUT_SAMPLE_RATE 0x0104           // Configure digital audio output sample rate
#define REFCLK_FREQ 0x0201                          //Sets frequency of reference clock in Hz. The range is 31130 to 34406 Hz, or 0 to disable the AFC. Default is 32768 Hz.
#define REFCLK_PRESCALE 0x0202                      // Sets the prescaler value for RCLK input.
#define AM_DEEMPHASIS 0x3100                        // Sets deemphasis time constant. Can be set to 50 ?s. Deemphasis is disabled by default.
#define AM_CHANNEL_FILTER 0x3102                    // Selects the bandwidth of the channel filter for AM reception. The choices are 6, 4, 3, 2, 2.5, 1.8, or 1 (kHz). The default bandwidth is 2 kHz.
#define AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN 0x3103 // Sets the maximum gain for automatic volume control.
#define AM_MODE_AFC_SW_PULL_IN_RANGE 0x3104         // Sets the SW AFC pull-in range.
#define AM_MODE_AFC_SW_LOCK_IN_RANGE 0x3105         // Sets the SW AFC lock-in.
#define AM_RSQ_INTERRUPTS 0x3200                    // Same SSB - Configures interrupt related to Received Signal Quality metrics. All interrupts are disabled by default.
#define AM_RSQ_SNR_HIGH_THRESHOLD 0x3201            //Sets high threshold for SNR interrupt.
#define AM_RSQ_SNR_LOW_THRESHOLD 0x3202             // Sets low threshold for SNR interrupt.
#define AM_RSQ_RSSI_HIGH_THRESHOLD 0x3203           // Sets high threshold for RSSI interrupt.
#define AM_RSQ_RSSI_LOW_THRESHOLD 0x3204            // Sets low threshold for RSSI interrupt.
#define AM_SOFT_MUTE_RATE 0x3300                    // Sets the attack and decay rates when entering or leaving soft mute. The default is 278 dB/s.
#define AM_SOFT_MUTE_SLOPE 0x3301                   // Sets the AM soft mute slope. Default value is a slope of 1.
#define AM_SOFT_MUTE_MAX_ATTENUATION 0x3302         // Sets maximum attenuation during soft mute (dB). Set to 0 to disable soft mute. Default is 8 dB.
#define AM_SOFT_MUTE_SNR_THRESHOLD 0x3303           // Sets SNR threshold to engage soft mute. Default is 8 dB.
#define AM_SOFT_MUTE_RELEASE_RATE 0x3304            // Sets softmute release rate. Smaller values provide slower release, and larger values provide faster release.
#define AM_SOFT_MUTE_ATTACK_RATE 0x3305             // Sets software attack rate. Smaller values provide slower attack, and larger values provide faster attack.
#define AM_SEEK_BAND_BOTTOM 0x3400                  // Sets the bottom of the AM band for seek. Default is 520.
#define AM_SEEK_BAND_TOP 0x3401                     // Sets the top of the AM band for seek. Default is 1710.
#define AM_SEEK_FREQ_SPACING 0x3402                 // Selects frequency spacing for AM seek. Default is 10 kHz spacing.
#define AM_SEEK_SNR_THRESHOLD 0x3403                // Sets the SNR threshold for a valid AM Seek/Tune.
#define AM_SEEK_RSSI_THRESHOLD 0x3404               // Sets the RSSI threshold for a valid AM Seek/Tune.
#define AM_AGC_ATTACK_RATE 0x3702                   // Sets the number of milliseconds the high peak detector must be exceeded before decreasing gain.
#define AM_AGC_RELEASE_RATE 0x3703                  // Sets the number of milliseconds the low peak detector must not be exceeded before increasing the gain.
#define AM_FRONTEND_AGC_CONTROL 0x3705              // Adjusts AM AGC for frontend (external) attenuator and LNA.
#define AM_NB_DETECT_THRESHOLD 0x3900               // Sets the threshold for detecting impulses in dB above the noise floor
#define AM_NB_INTERVAL 0x3901                       // Interval in micro-seconds that original samples are replaced by interpolated clean samples
#define AM_NB_RATE 0x3902                           // Noise blanking rate in 100 Hz units. Default value is 64.
#define AM_NB_IIR_FILTER 0x3903                     // Sets the bandwidth of the noise floor estimator. Default value is 300.
#define AM_NB_DELAY 0x3904                          // Delay in micro-seconds before applying impulse blanking to the original samples

// Volume
#define RX_VOLUME 0x4000
#define RX_HARD_MUTE 0x4001

// SSB properties
// See AN332 REV 0.8 Universal Programming Guide (Amendment for SI4735-D60 SSN and NBFM Patches)
#define GPO_IEN 0x0001                       // AM and SSB - Enable interrupt source
#define SSB_BFO 0x0100                       // Sets the Beat Frequency Offset (BFO) under SSB mode.
#define SSB_MODE 0x0101                      // Sets number of properties of the SSB mode.
#define SSB_RSQ_INTERRUPTS 0x3200            // Configure Interrupts related to RSQ
#define SSB_RSQ_SNR_HI_THRESHOLD 0x3201      // Sets high threshold for SNR interrupt
#define SSB_RSQ_SNR_LO_THRESHOLD 0x3202      // Sets low threshold for SNR interrupt
#define SSB_RSQ_RSSI_HI_THRESHOLD 0x3203     // Sets high threshold for RSSI interrupt
#define SSB_RSQ_RSSI_LO_THRESHOLD 0x3204     // Sets low threshold for RSSI interrupt
#define SSB_SOFT_MUTE_RATE 0x3300            // Sets the attack and decay rates when entering or leaving soft mute
#define SSB_SOFT_MUTE_MAX_ATTENUATION 0x3302 // Sets the maximum attenuation during soft mute (db); 0dB to disable soft mute; defaul 8dB;
#define SSB_SOFT_MUTE_SNR_THRESHOLD 0x3303   // Sets SNR threshould to engage soft mute. Defaul 8dB
#define SSB_RF_AGC_ATTACK_RATE 0x3700        // Sets the number of milliseconds the high RF peak detector must be exceeded before decreasing the gain. Defaul 4.
#define SSB_RF_AGC_RELEASE_RATE 0x3701       // Sets the number of milliseconds the low RF peak detector must be exceeded before increasing the gain. Defaul 24.

// SSB
#define SSB_RF_IF_AGC_ATTACK_RATE 0x3702  // Sets the number of milliseconds the high IF peak detector must be exceeded before decreasing gain. Defaul 4.
#define SSB_RF_IF_AGC_RELEASE_RATE 0x3703 // Sets the number of milliseconds the low IF peak detector must be exceeded before increasing the gain. Defaul 140.

// See AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 12 and 13
#define SSB_NONE        0
#define LSB_MODE        1 // 01
#define USB_MODE        2 // 10
#define MAX_SSB_MODES   3

// Parameters
#define SI473X_RDS_OUTPUT_ONLY 0b00000000      // RDS output only (no audio outputs) Si4749 only
#define SI473X_ANALOG_AUDIO 0b00000101         // Analog Audio Inputs
#define SI473X_DIGITAL_AUDIO1 0b00001011       // Digital audio output (DCLK, LOUT/DFS, ROUT/DIO)
#define SI473X_DIGITAL_AUDIO2 0b10110000       // Digital audio outputs (DCLK, DFS, DIO)
#define SI473X_ANALOG_DIGITAL_AUDIO 0b10110101 // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS,DIO)

// Other parameters
#define FM_CURRENT_MODE 0
#define AM_CURRENT_MODE 1
#define SSB_CURRENT_MODE 2

// Frequency Seeking
#define SEEK_UP 1
#define SEEK_DOWN 0
#define MAX_DELAY_AFTER_SET_FREQUENCY 30 // In ms - This value helps to improve the precision during of getting frequency value
#define MAX_DELAY_AFTER_POWERUP 10       // In ms - Max delay you have to setup after a power up command.
#define MIN_DELAY_WAIT_SEND_LOOP 300     // In uS (Microsecond) - each loop of waitToSend should wait this value in microsecond
#define MAX_SEEK_TIME 8000               // defines the maximum seeking time 8s is default.

// Crystal
#define XOSCEN_CRYSTAL 1 // Use crystal oscillator
#define XOSCEN_RCLK 0    // Use external RCLK (crystal oscillator disabled).

/** @defgroup group01 Union, Struct and Defined Data Types 
 * @section group01 Data Types 
 *  
 * @brief SI473X data representation
 * 
 * @details The goal of this approach is separating data from code. 
 * The Si47XX family works with many internal data that can be represented by data structure 
 * or defined data type in C/C++. These C/C++ resources have been used widely here.  
 * This aproach made the library easier to build and maintain.  Each data structure created 
 * here has its reference (name of the document and page on which it was based). 
 * In other words, to make the SI47XX device easier to deal, some defined data types were 
 * created to handle byte and bits to process  commands, properties and responses.
 * These data types will be usefull to deal with SI473X 
 */

/**
 * 
 * @brief Power Up arguments data type 
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 65
 */
typedef union
{
    struct
    {
        // ARG1
        unsigned char FUNC : 4;    //!<  Function (0 = FM Receive; 1?14 = Reserved; 15 = Query Library ID)
        unsigned char XOSCEN : 1;  //!<  Crystal Oscillator Enable (0 = crystal oscillator disabled; 1 = Use crystal oscillator and and OPMODE=ANALOG AUDIO) .
        unsigned char PATCH : 1;   //!<  Patch Enable (0 = Boot normally; 1 = Copy non-volatile memory to RAM).
        unsigned char GPO2OEN : 1; //!<  GPO2 Output Enable (0 = GPO2 output disabled; 1 = GPO2 output enabled).
        unsigned char CTSIEN : 1;  //!<  CTS Interrupt Enable (0 = CTS interrupt disabled; 1 = CTS interrupt enabled).
        // ARG2
        unsigned char OPMODE; //!<  Application Setting. See page 65
    } arg;              //!<  Refined powerup parameters
    unsigned char raw[2];     //!<  Raw powerup parameters data. Same arg memory position. So, same content.
} si473x_powerup;
/**
 * 
 * @brief Data type for Enables output for GPO1, GPO2 and GPO3
 * 
 * @details GPO1, 2, and 3 can be configured for output (Hi-Z or active drive) by setting the GPO1OEN, GPO2OEN, and GPO3OEN bit. 
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 82 and 144
 */
typedef union
{
    struct
    {
        unsigned char DUMMY1 : 1;  //!< Always write 0.
        unsigned char GPO1OEN : 1; //!< GPO1 Output Enable.
        unsigned char GPO2OEN : 1; //!< GPO2 Output Enable.
        unsigned char GPO3OEN : 1; //!< GPO3 Output Enable.
        unsigned char DUMMY2 : 4;  //!< Always write 0.
    } arg;                   //!<  Refined powerup parameters
    unsigned char raw;
} si473x_gpio;

/**
 * 
 * @brief Data type for Configuring the sources for the GPO2/INT interrupt pin
 * 
 * @details Valid sources are the lower 8 bits of the STATUS byte, including CTS, ERR, RSQINT, and STCINT bits.
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 146
 */
typedef union
{
    struct
    {
        unsigned char STCIEN : 1; //!< Seek/Tune Complete Interrupt Enable (0 or 1).
        unsigned char DUMMY1 : 2; //!< Always write 0.
        unsigned char RSQIEN : 1; //!< RSQ Interrupt Enable (0 or 1).
        unsigned char DUMMY2 : 2; //!< Always write 0.
        unsigned char ERRIEN : 1; //!< ERR Interrupt Enable (0 or 1).
        unsigned char CTSIEN : 1; //!< CTS Interrupt Enable (0 or 1).
        unsigned char STCREP : 1; //!< STC Interrupt Repeat (0 or 1).
        unsigned char DUMMY3 : 2; //!< Always write 0.
        unsigned char RSQREP : 1; //!< RSQ Interrupt Repeat (0 or 1).
        unsigned char DUMMY4 : 4; //!< Always write 0.
    } arg;
    unsigned int raw;
} si473x_gpio_ien;

/**
 * 
 * @brief Represents how the  frequency is stored in the si4735.
 * @details It helps to convert frequency in unsigned int to two bytes (unsigned char) (FREQL and FREQH)  
 */
typedef union
{
    struct
    {
        unsigned char FREQL; //!<  Tune Frequency Low byte.
        unsigned char FREQH; //!<  Tune Frequency High byte.
    } raw;             //!<  Raw data that represents the frequency stored in the Si47XX device.
    unsigned int value;    //!<  frequency (integer value)
} si473x_frequency;

/**
 * @brief Antenna Tuning Capacitor data type manupulation 
 */
typedef union
{
    struct
    {
        unsigned char ANTCAPL; //!<  Antenna Tuning Capacitor High byte
        unsigned char ANTCAPH; //!<  Antenna Tuning Capacitor Low byte
    } raw;
    unsigned int value;
} si473x_antenna_capacitor;

/**
 * 
 * @brief AM Tune frequency data type command (AM_TUNE_FREQ command)
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 135
 */
typedef union
{
    struct
    {
        unsigned char FAST : 1;   //!<  ARG1 - FAST Tuning. If set, executes fast and invalidated tune. The tune status will not be accurate.
        unsigned char FREEZE : 1; //!<  Valid only for FM (Must be 0 to AM)
        unsigned char DUMMY1 : 4; //!<  Always set 0
        unsigned char USBLSB : 2; //!<  SSB Upper Side Band (USB) and Lower Side Band (LSB) Selection. 10 = USB is selected; 01 = LSB is selected.
        unsigned char FREQH;      //!<  ARG2 - Tune Frequency High byte.
        unsigned char FREQL;      //!<  ARG3 - Tune Frequency Low byte.
        unsigned char ANTCAPH;    //!<  ARG4 - Antenna Tuning Capacitor High byte.
        unsigned char ANTCAPL;    //!<  ARG5 - Antenna Tuning Capacitor Low byte. Note used for FM.
    } arg;
    unsigned char raw[5];
} si473x_set_frequency;

/** 
 * 
 * @brief Seek frequency (automatic tuning). ARG1
 * 
 * @details Represents searching for a valid frequency data type AM and FM.
 * @details When AM, the searching data have to be complemented by si47x_seek_am_complement.
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 72 and 137
 * @see si47x_seek_am_complement
 */
typedef union
{
    struct
    {
        unsigned char RESERVED1 : 2;
        unsigned char WRAP : 1;   //!<  Determines whether the seek should Wrap = 1, or Halt = 0 when it hits the band limit.
        unsigned char SEEKUP : 1; //!<  Determines the direction of the search, either UP = 1, or DOWN = 0.
        unsigned char RESERVED2 : 4;
    } arg;
    unsigned char raw;
} si473x_seek;

/** 
 * 
 * @brief Seek frequency (automatic tuning) AM complement (ARG2, ARG3, ARG4 and ARG5)
 * 
 * @details Represents AM complement searching information for a valid frequency data type.
 * 
 * @see  @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 72 and 137
 */
typedef struct
{
    unsigned char ARG2; // Always 0.
    unsigned char ARG3; // Always 0.
    unsigned char ANTCAPH;
    unsigned char ANTCAPL;
} si473x_seek_am_complement;

/** 
 * @ingroup group01 status response structure
 * 
 * @brief Status response data representation
 * 
 * @details Represents searching for a valid frequency data type.
 */
typedef union
{
    struct
    {
        unsigned char STCINT : 1; //!< 1 = Tune complete has been triggered.
        unsigned char DUMMY1 : 1; //!< Reserved (Values may vary).
        unsigned char RDSINT : 1; //!< 1 = Radio data system interrupt has been triggered.
        unsigned char RSQINT : 1; //!< 1 = Received Signal Quality measurement has been triggered.
        unsigned char DUMMY2 : 2; //!< Reserved (Values may vary).
        unsigned char ERR : 1;    //!< 1 = Error.
        unsigned char CTS : 1;    //!< 0 = Wait before sending next command; 1 = Clear to send next command.
    } refined;
    unsigned char raw;
} si473x_status;

/**  
 * 
 * @brief Response status command 
 * 
 * @details Response data from a query status command
 * 
 * @see Si47XX PROGRAMMING GUIDE; pages 73 and 
 */
typedef union
{
    struct
    {
        // Status
        unsigned char STCINT : 1; //!<  Seek/Tune Complete Interrupt; 1 = Tune complete has been triggered.
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1; //!<  Radio Data System (RDS) Interrup; 0 = interrupt has not been triggered.
        unsigned char RSQINT : 1; //!<  Received Signal Quality Interrupt; 0 = interrupt has not been triggered.
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1; //!<  Error. 0 = No error 1 = Error
        unsigned char CTS : 1; //!<  Clear to Send.
        // RESP1
        unsigned char VALID : 1; //!<  Valid Channel
        unsigned char AFCRL : 1; //!<  AFC Rail Indicator
        unsigned char DUMMY3 : 5;
        unsigned char BLTF : 1; //!<  Reports if a seek hit the band limit
        // RESP2
        unsigned char READFREQH; //!<  Read Frequency High byte.
        // RESP3
        unsigned char READFREQL; //!<  Read Frequency Low byte.
        // RESP4
        unsigned char RSSI; //!<  Received Signal Strength Indicator (dB?V)
        // RESP5
        unsigned char SNR; //!<  This byte contains the SNR metric when tune is complete (dB).
        // RESP6
        unsigned char MULT; //!<  If FM, contains the multipath metric when tune is complete; IF AM READANTCAPH (tuning capacitor value high byte)
        // RESP7
        unsigned char READANTCAP; //!<  If FM, contains the current antenna tuning capacitor value; IF AM READANTCAPL (tuning capacitor value low byte)
    } resp;
    unsigned char raw[8]; //!<  Check it
} si473x_response_status;

/**
 * 
 * @brief Data representation for  Firmware Information (GET_REV)
 * 
 * @details The part number, chip revision, firmware revision, patch revision and component revision numbers. 
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 66 and 131 
 */
typedef union
{
    struct
    {
        // status ("RESP0")
        unsigned char STCINT : 1;
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1;
        unsigned char RSQINT : 1;
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1;
        unsigned char CTS : 1;
        unsigned char PN;       //!<  RESP1 - Final 2 digits of Part Number (HEX).
        unsigned char FWMAJOR;  //!<  RESP2 - Firmware Major Revision (ASCII).
        unsigned char FWMINOR;  //!<  RESP3 - Firmware Minor Revision (ASCII).
        unsigned char PATCHH;   //!<  RESP4 - Patch ID High byte (HEX).
        unsigned char PATCHL;   //!<  RESP5 - Patch ID Low byte (HEX).
        unsigned char CMPMAJOR; //!<  RESP6 - Component Major Revision (ASCII).
        unsigned char CMPMINOR; //!<  RESP7 - Component Minor Revision (ASCII).
        unsigned char CHIPREV;  //!<  RESP8 - Chip Revision (ASCII).
        // RESP9 to RESP15 not used
    } resp;
    unsigned char raw[9];
} si473x_firmware_information;

/**
 * 
 * @brief Firmware Query Library ID response. 
 * 
 * @details Used to represent the response of a power up command with FUNC = 15 (patch)
 * 
 * To confirm that the patch is compatible with the internal device library revision, the library 
 * revision should be confirmed by issuing the POWER_UP command with Function = 15 (query library ID)
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 12 
 */
typedef union
{
    struct
    {
        // status ("RESP0")
        unsigned char STCINT : 1;
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1;
        unsigned char RSQINT : 1;
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1;
        unsigned char CTS : 1;
        unsigned char PN;        //!<  RESP1 - Final 2 digits of Part Number (HEX).
        unsigned char FWMAJOR;   //!<  RESP2 - Firmware Major Revision (ASCII).
        unsigned char FWMINOR;   //!<  RESP3 - Firmware Minor Revision (ASCII).
        unsigned char RESERVED1; //!<  RESP4 - Reserved, various values.
        unsigned char RESERVED2; //!<  RESP5 - Reserved, various values.
        unsigned char CHIPREV;   //!<  RESP6 - Chip Revision (ASCII).
        unsigned char LIBRARYID; //!<  RESP7 - Library Revision (HEX).
        // RESP9 to RESP15 not used
    } resp;
    unsigned char raw[8];
} si473x_firmware_query_library;

/**
 * 
 * @brief Seek station status
 * 
 * @details Status of FM_TUNE_FREQ or FM_SEEK_START commands or Status of AM_TUNE_FREQ or AM_SEEK_START commands.
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 73 and 139
 */
typedef union
{
    struct
    {
        unsigned char INTACK : 1; //!<  If set, clears the seek/tune complete interrupt status indicator.
        unsigned char CANCEL : 1; //!<  If set, aborts a seek currently in progress.
        unsigned char RESERVED2 : 6;
    } arg;
    unsigned char raw;
} si473x_tune_status;

/**
 * 
 * @brief Data type to deal with SET_PROPERTY command
 * 
 * @details Property Data type (help to deal with SET_PROPERTY command on si473X)
 */
typedef union
{
    struct
    {
        unsigned char byteLow;
        unsigned char byteHigh;
    } raw;
    unsigned int value;
} si473x_property;

/** 
 * 
 * @brief  Radio Signal Quality data representation 
 * 
 * @details Data type for status information about the received signal quality (FM_RSQ_STATUS and AM_RSQ_STATUS)
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 75 and 
 */
typedef union
{
    struct
    {
        // status ("RESP0")
        unsigned char STCINT : 1;
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1;
        unsigned char RSQINT : 1;
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1;
        unsigned char CTS : 1;
        // RESP1
        unsigned char RSSIILINT : 1; //!<  RSSI Detect Low.
        unsigned char RSSIHINT : 1;  //!<  RSSI Detect High.
        unsigned char SNRLINT : 1;   //!<  SNR Detect Low.
        unsigned char SNRHINT : 1;   //!<  SNR Detect High.
        unsigned char MULTLINT : 1;  //!<  Multipath Detect Low
        unsigned char MULTHINT : 1;  //!<  Multipath Detect High
        unsigned char DUMMY3 : 1;
        unsigned char BLENDINT : 1; //!<  Blend Detect Interrupt.
        // RESP2
        unsigned char VALID : 1; //!<  Valid Channel.
        unsigned char AFCRL : 1; //!<  AFC Rail Indicator.
        unsigned char DUMMY4 : 1;
        unsigned char SMUTE : 1; //!<  Soft Mute Indicator. Indicates soft mute is engaged.
        unsigned char DUMMY5 : 4;
        // RESP3
        unsigned char STBLEND : 7; //!<  Indicates amount of stereo blend in% (100 = full stereo, 0 = full mono).
        unsigned char PILOT : 1;   //!<  Indicates stereo pilot presence.
        // RESP4 to RESP7
        unsigned char RSSI;    //!<  RESP4 - Contains the current receive signal strength (0-127 dBV).
        unsigned char SNR;     //!<  RESP5 - Contains the current SNR metric (0-127 dB).
        unsigned char MULT;    //!<  RESP6 - Contains the current multipath metric. (0 = no multipath; 100 = full multipath)
        unsigned char FREQOFF; //!<  RESP7 - Signed frequency offset (kHz).
    } resp;
    unsigned char raw[8];
} si473x_rqs_status;

/**
 * 
 * @brief Data type for RDS Status command and response information 
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 77 and 78 
 * @see Also https://en.wikipedia.org/wiki/Radio_Data_System
 */
typedef union
{
    struct
    {
        unsigned char INTACK : 1;     // Interrupt Acknowledge; 0 = RDSINT status preserved; 1 = Clears RDSINT.
        unsigned char MTFIFO : 1;     // Empty FIFO; 0 = If FIFO not empty; 1 = Clear RDS Receive FIFO.
        unsigned char STATUSONLY : 1; // Determines if data should be removed from the RDS FIFO.
        unsigned char dummy : 5;
    } arg;
    unsigned char raw;
} si473x_rds_command;

/**
 * 
 * @brief Response data type for current channel and reads an entry from the RDS FIFO.
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 77 and 78
 */
typedef union
{
    struct
    {
        // status ("RESP0")
        unsigned char STCINT : 1;
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1;
        unsigned char RSQINT : 1;
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1;
        unsigned char CTS : 1;
        // RESP1
        unsigned char RDSRECV : 1;      //!<  RDS Received; 1 = FIFO filled to minimum number of groups set by RDSFIFOCNT.
        unsigned char RDSSYNCLOST : 1;  //!<  RDS Sync Lost; 1 = Lost RDS synchronization.
        unsigned char RDSSYNCFOUND : 1; //!<  RDS Sync Found; 1 = Found RDS synchronization.
        unsigned char DUMMY3 : 1;
        unsigned char RDSNEWBLOCKA : 1; //!<  RDS New Block A; 1 = Valid Block A data has been received.
        unsigned char RDSNEWBLOCKB : 1; //!<  RDS New Block B; 1 = Valid Block B data has been received.
        unsigned char DUMMY4 : 2;
        // RESP2
        unsigned char RDSSYNC : 1; //!<  RDS Sync; 1 = RDS currently synchronized.
        unsigned char DUMMY5 : 1;
        unsigned char GRPLOST : 1; //!<  Group Lost; 1 = One or more RDS groups discarded due to FIFO overrun.
        unsigned char DUMMY6 : 5;
        // RESP3 to RESP11
        unsigned char RDSFIFOUSED; //!<  RESP3 - RDS FIFO Used; Number of groups remaining in the RDS FIFO (0 if empty).
        unsigned char BLOCKAH;     //!<  RESP4 - RDS Block A; HIGH byte
        unsigned char BLOCKAL;     //!<  RESP5 - RDS Block A; LOW byte
        unsigned char BLOCKBH;     //!<  RESP6 - RDS Block B; HIGH byte
        unsigned char BLOCKBL;     //!<  RESP7 - RDS Block B; LOW byte
        unsigned char BLOCKCH;     //!<  RESP8 - RDS Block C; HIGH byte
        unsigned char BLOCKCL;     //!<  RESP9 - RDS Block C; LOW byte
        unsigned char BLOCKDH;     //!<  RESP10 - RDS Block D; HIGH byte
        unsigned char BLOCKDL;     //!<  RESP11 - RDS Block D; LOW byte
        // RESP12 - Blocks A to D Corrected Errors.
        // 0 = No errors;
        // 1 = 1?2 bit errors detected and corrected;
        // 2 = 3?5 bit errors detected and corrected.
        // 3 = Uncorrectable.
        unsigned char BLED : 2;
        unsigned char BLEC : 2;
        unsigned char BLEB : 2;
        unsigned char BLEA : 2;
    } resp;
    unsigned char raw[13];
} si473x_rds_status;

/**
 * 
 * @brief FM_RDS_INT_SOURCE property data type
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 103
 * @see also https://en.wikipedia.org/wiki/Radio_Data_System
 */
typedef union
{
    struct
    {
        unsigned char RDSRECV : 1;      //!<  If set, generate RDSINT when RDS FIFO has at least FM_RDS_INT_FIFO_COUNT entries.
        unsigned char RDSSYNCLOST : 1;  //!<  If set, generate RDSINT when RDS loses synchronization.
        unsigned char RDSSYNCFOUND : 1; //!<  f set, generate RDSINT when RDS gains synchronization.
        unsigned char DUMMY1 : 1;       //!<  Always write to 0.
        unsigned char RDSNEWBLOCKA : 1; //!<  If set, generate an interrupt when Block A data is found or subsequently changed
        unsigned char RDSNEWBLOCKB : 1; //!<  If set, generate an interrupt when Block B data is found or subsequently changed
        unsigned char DUMMY2 : 5;       //!<  Reserved - Always write to 0.
        unsigned char DUMMY3 : 5;       //!<  Reserved - Always write to 0.
    } refined;
    unsigned char raw[2];
} si473x_rds_int_source;

/**
 * 
 * @brief Data type for FM_RDS_CONFIG Property
 * 
 * IMPORTANT: all block errors must be less than or equal the associated block error threshold for the group 
 * to be stored in the RDS FIFO. 
 * 0 = No errors; 1 = 1?2 bit errors detected and corrected; 2 = 3?5 bit errors detected and corrected; 3 = Uncorrectable.
 * Recommended Block Error Threshold options:
 *  2,2,2,2 = No group stored if any errors are uncorrected.
 *  3,3,3,3 = Group stored regardless of errors.
 *  0,0,0,0 = No group stored containing corrected or uncorrected errors.
 *  3,2,3,3 = Group stored with corrected errors on B, regardless of errors on A, C, or D.
 *  
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 58 and 104 
 */
typedef union
{
    struct
    {
        unsigned char RDSEN : 1; //!<  1 = RDS Processing Enable.
        unsigned char DUMMY1 : 7;
        unsigned char BLETHD : 2; //!<  Block Error Threshold BLOCKD
        unsigned char BLETHC : 2; //!<  Block Error Threshold BLOCKC.
        unsigned char BLETHB : 2; //!<  Block Error Threshold BLOCKB.
        unsigned char BLETHA : 2; //!<  Block Error Threshold BLOCKA.
    } arg;
    unsigned char raw[2];
} si473x_rds_config;

/**
 * 
 * @brief Block A data type
 */
typedef union
{
    struct
    {
        unsigned int pi;
    } refined;
    struct
    {
        unsigned char highValue; // Most Significant unsigned char first
        unsigned char lowValue;
    } raw;
} si473x_rds_blocka;

/**
 * 
 * @brief Block B data type
 * 
 * @details For GCC on System-V ABI on 386-compatible (32-bit processors), the following stands:
 * 
 * 1) Bit-fields are allocated from right to left (least to most significant).
 * 2) A bit-field must entirely reside in a storage unit appropriate for its declared type.
 *    Thus a bit-field never crosses its unit boundary.
 * 3) Bit-fields may share a storage unit with other struct/union members, including members that are not bit-fields.
 *    Of course, struct members occupy different parts of the storage unit.
 * 4) Unnamed bit-fields' types do not affect the alignment of a structure or union, although individual 
 *    bit-fields' member offsets obey the alignment constraints.   
 * 
 * @see also Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 78 and 79
 * @see also https://en.wikipedia.org/wiki/Radio_Data_System
 */
typedef union
{
    struct
    {
        unsigned int address : 2;            // Depends on Group Type and Version codes. If 0A or 0B it is the Text Segment Address.
        unsigned int DI : 1;                 // Decoder Controll bit
        unsigned int MS : 1;                 // Music/Speech
        unsigned int TA : 1;                 // Traffic Announcement
        unsigned int programType : 5;        // PTY (Program Type) code
        unsigned int trafficProgramCode : 1; // (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        unsigned int versionCode : 1;        // (B0) => 0=A; 1=B
        unsigned int groupType : 4;          // Group Type code.
    } group0;
    struct
    {
        unsigned int address : 4;            // Depends on Group Type and Version codes. If 2A or 2B it is the Text Segment Address.
        unsigned int textABFlag : 1;         // Do something if it chanhes from binary "0" to binary "1" or vice-versa
        unsigned int programType : 5;        // PTY (Program Type) code
        unsigned int trafficProgramCode : 1; // (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        unsigned int versionCode : 1;        // (B0) => 0=A; 1=B
        unsigned int groupType : 4;          // Group Type code.
    } group2;
    struct
    {
        unsigned int content : 4;            // Depends on Group Type and Version codes.
        unsigned int textABFlag : 1;         // Do something if it chanhes from binary "0" to binary "1" or vice-versa
        unsigned int programType : 5;        // PTY (Program Type) code
        unsigned int trafficProgramCode : 1; // (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        unsigned int versionCode : 1;        // (B0) => 0=A; 1=B
        unsigned int groupType : 4;          // Group Type code.
    } refined;
    struct
    {
        unsigned char lowValue;
        unsigned char highValue; // Most Significant byte first
    } raw;
} si473x_rds_blockb;

/*
 * 
 * 
 * Group type 4A ( RDS Date and Time)
 * When group type 4A is used by the station, it shall be transmitted every minute according to EN 50067.
 * This Structure uses blocks 2,3 and 5 (B,C,D)
 * 
 * Commented due to ?Crosses boundary? on GCC 32-bit plataform.
 */
/*
typedef union {
    struct
    {
        uint32_t offset : 5;       // Local Time Offset
        uint32_t offset_sense : 1; // Local Offset Sign ( 0 = + , 1 = - )
        uint32_t minute : 6;       // UTC Minutes
        uint32_t hour : 5;         // UTC Hours
        uint32_t mjd : 17;        // Modified Julian Day Code
    } refined;
    unsigned char raw[6];
} si47x_rds_date_time;
*/

/**
 * 
 * Group type 4A ( RDS Date and Time)
 * When group type 4A is used by the station, it shall be transmitted every minute according to EN 50067.
 * This Structure uses blocks 2,3 and 5 (B,C,D)
 * 
 * ATTENTION: 
 * To make it compatible with 8, 16 and 32 bits platforms and avoid Crosses boundary, it was necessary to
 * split minute and hour representation. 
 */
typedef union
{
    struct
    {
        unsigned char offset : 5;       // Local Time Offset
        unsigned char offset_sense : 1; // Local Offset Sign ( 0 = + , 1 = - )
        unsigned char minute1 : 2;      // UTC Minutes - 2 bits less significant (void ?Crosses boundary?).
        unsigned char minute2 : 4;      // UTC Minutes - 4 bits  more significant  (void ?Crosses boundary?)
        unsigned char hour1 : 4;        // UTC Hours - 4 bits less significant (void ?Crosses boundary?)
        unsigned char hour2 : 1;        // UTC Hours - 4 bits more significant (void ?Crosses boundary?)
        uint32_t mjd : 17;        // Modified Julian Day Code
    } refined;
    unsigned char raw[6];
} si473x_rds_date_time;

/** 
 * 
 * AGC data types
 * FM / AM and SSB structure to AGC
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 80; for AM page 142
 * @see AN332 REV 0.8 Universal Programming Guide Amendment for SI4735-D60 SSB and NBFM patches; page 18. 
 */
typedef union
{
    struct
    {
        // status ("RESP0")
        unsigned char STCINT : 1;
        unsigned char DUMMY1 : 1;
        unsigned char RDSINT : 1; // Not used for AM/SSB
        unsigned char RSQINT : 1;
        unsigned char DUMMY2 : 2;
        unsigned char ERR : 1;
        unsigned char CTS : 1;
        // RESP1
        unsigned char AGCDIS : 1; // This bit indicates if the AGC is enabled or disabled. 0 = AGC enabled; 1 = AGC disabled.
        unsigned char DUMMY : 7;
        // RESP2
        unsigned char AGCIDX; // For FM (5 bits - READ_LNA_GAIN_INDEX - 0 = Minimum attenuation (max gain)). For AM (8 bits). This byte reports the current AGC gain index.
    } refined;
    unsigned char raw[3];
} si473x_agc_status;

/** 
 * 
 * If FM, Overrides AGC setting by disabling the AGC and forcing the LNA to have a certain gain that ranges between 0 
 * (minimum attenuation) and 26 (maximum attenuation).
 * If AM, overrides the AGC setting by disabling the AGC and forcing the gain index that ranges between 0
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 81; for AM page 143
 */
typedef union
{
    struct
    {
        // ARG1
        unsigned char AGCDIS : 1; // if set to 1 indicates if the AGC is disabled. 0 = AGC enabled; 1 = AGC disabled.
        unsigned char DUMMY : 7;
        // ARG2
        unsigned char AGCIDX; // AGC Index; If AMAGCDIS = 1, this byte forces the AGC gain index; 0 = Minimum attenuation (max gain)
    } arg;
    unsigned char raw[2];
} si473x_agc_overrride;

/** 
 * 
 * The bandwidth of the AM channel filter data type
 * AMCHFLT values: 0 = 6 kHz Bandwidth                    
 *                 1 = 4 kHz Bandwidth
 *                 2 = 3 kHz Bandwidth
 *                 3 = 2 kHz Bandwidth
 *                 4 = 1 kHz Bandwidth
 *                 5 = 1.8 kHz Bandwidth
 *                 6 = 2.5 kHz Bandwidth, gradual roll off
 *                 7?15 = Reserved (Do not use)
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 125 and 151
 */
typedef union
{
    struct
    {
        unsigned char AMCHFLT : 4; //!<  Selects the bandwidth of the AM channel filter.
        unsigned char DUMMY1 : 4;
        unsigned char AMPLFLT : 1; //!<  Enables the AM Power Line Noise Rejection Filter.
        unsigned char DUMMY2 : 7;
    } param;
    unsigned char raw[2];
} si473x_bandwidth_config; // AM_CHANNEL_FILTER

/** 
 * 
 * SSB - datatype for SSB_MODE (property 0x0101)
 * 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 */
typedef union
{
    struct
    {
        unsigned char AUDIOBW : 4;     //!<  0 = 1.2KHz (default); 1=2.2KHz; 2=3KHz; 3=4KHz; 4=500Hz; 5=1KHz
        unsigned char SBCUTFLT : 4;    //!<  SSB side band cutoff filter for band passand low pass filter
        unsigned char AVC_DIVIDER : 4; //!<  set 0 for SSB mode; set 3 for SYNC mode;
        unsigned char AVCEN : 1;       //!<  SSB Automatic Volume Control (AVC) enable; 0=disable; 1=enable (default);
        unsigned char SMUTESEL : 1;    //!<  SSB Soft-mute Based on RSSI or SNR
        unsigned char DUMMY1 : 1;      //!<  Always write 0;
        unsigned char DSP_AFCDIS : 1;  //!<  0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable.
    } param;
    unsigned char raw[2];
} si473x_ssb_mode;

/**
 * 
 * @brief Digital audio output format data structure (Property 0x0102. DIGITAL_OUTPUT_FORMAT).
 * 
 * @details Used to configure: DCLK edge, data format, force mono, and sample precision.
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 195. 
 */
typedef union
{
    struct
    {
        unsigned char OSIZE : 2; //!<  Digital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
        unsigned char OMONO : 1; //!<  Digital Output Mono Mode (0=Use mono/stereo blend ).
        unsigned char OMODE : 4; //!<  Digital Output Mode (0000=I2S, 0110 = Left-justified, 1000 = MSB at second DCLK after DFS pulse, 1100 = MSB at first DCLK after DFS pulse).
        unsigned char OFALL : 1; //!<  Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
        unsigned char dummy : 8; //!<  Always 0.
    } refined;
    unsigned int raw;
} si473x_digital_output_format;

/**

 * @brief patch header stored in a eeprom 
 * @details This data type represents o header of a eeprom with a patch content 
 * @details This structure will be used to read an eeprom generated by leo sketch SI47XX_09_SAVE_SSB_PATCH_EEPROM.ino.
 * @details The sketch SI47XX_09_SAVE_SSB_PATCH_EEPROM can be found on Examples/SI47XX_TOOLS folder   
 */
typedef union
{
    struct
    {
        unsigned char reserved[8];  // Not used
        unsigned char status[8];    // Note used
        unsigned char patch_id[14]; // Patch name
        unsigned int patch_size;  // Patch size (in bytes)
    } refined;
    unsigned char raw[32];
} si473x_eeprom_patch_header;

/**
 * @ingroup group01
 * 
 * @brief Digital audio output sample structure (Property 0x0104. DIGITAL_OUTPUT_SAMPLE_RATE).
 * 
 * @details Used to enable digital audio output and to configure the digital audio output sample rate in samples per second (sps).
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 196. 
 */
typedef struct
{
    unsigned int DOSR;                   // Digital Output Sample Rate(32?48 ksps .0 to disable digital audio output).
} si473x_digital_output_sample_rate; // Maybe not necessary

// Connection Configuration
#define SI473X_INT_ENABLE               0                   // 1 = Interrupt Enabled
#define SI473X_CUR_CLOCK_TYPE           XOSCEN_CRYSTAL      // Crystal Type
#define SI473X_REF_CLK                  32768               // Crystal Frequency, Originally 31768 why?
#define SI473X_REF_CLK_PRESCALE         1                   // Prescaler
#define SI473X_REF_CLK_SRC_PIN          0                   // 0 = RCLK pin is clock source; 1 = DCLK pin is clock source.
#define SI473X_WAIT_TIMEOUT             10000               // Arbitrary constant for operation timeout

// Method Headers
void si473x_setGpioCtl(unsigned char GPO1OEN, unsigned char GPO2OEN, unsigned char GPO3OEN);
void si473x_setGpio(unsigned char GPO1LEVEL, unsigned char GPO2LEVEL, unsigned char GPO3LEVEL);
void si473x_setGpioIen(unsigned char STCIEN, unsigned char RSQIEN, unsigned char ERRIEN, unsigned char CTSIEN, unsigned char STCREP, unsigned char RSQREP);
void si473x_rcv(unsigned char buf[], unsigned char count);
unsigned char si473x_waitToSend();
unsigned char si473x_radioPowerUp(unsigned char defaultFunction);
void si473x_radioPowerDown(void);
si473x_firmware_information si473x_getFirmware(unsigned char *isOK);
void si473x_setRefClock(unsigned int refclk);
void si473x_setRefClockPrescaler(unsigned int prescale, unsigned char rclk_sel);
si473x_firmware_information si473x_setup(unsigned char defaultFunction, unsigned char *isOK);
void si473x_setTuneFrequencyAntennaCapacitor(unsigned int capacitor);
unsigned char si473x_setFrequency(unsigned int freq);
unsigned char si473x_setAM(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step);
unsigned char si473x_setFM(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step);
void si473x_setBandwidth(unsigned char AMCHFLT, unsigned char AMPLFLT);
unsigned int si473x_getFrequency(unsigned char *isValid);
unsigned char si473x_updateCurrentStatus(unsigned char INTACK, unsigned char CANCEL);
si473x_agc_status si473x_getAutomaticGainControl();
void si473x_setAutomaticGainControl(unsigned char AGCDIS, unsigned char AGCIDX);
void si473x_setAvcAmMaxGain(unsigned char gain);
si473x_rqs_status si473x_getCurrentReceivedSignalQuality(unsigned char INTACK);
void si473x_seekStation(unsigned char SEEKUP, unsigned char WRAP);
void si473x_setSeekAmLimits(unsigned int bottom, unsigned int top);
void si473x_setSeekFmLimits(unsigned int bottom, unsigned int top);
void si473x_setSeekAmSpacing(unsigned int spacing);
void si473x_setSeekFmSpacing(unsigned int spacing);
void si473x_setSeekAmSrnThreshold(unsigned int value);
void si473x_setSeekFmSrnThreshold(unsigned int value);
void si473x_setSeekAmRssiThreshold(unsigned int value);
void si473x_setSeekFmRssiThreshold(unsigned int value);
void si473x_sendProperty(unsigned int propertyNumber, unsigned int parameter);
void si473x_getCommandResponse(int response_size, unsigned char * response);
si473x_status si473x_getStatusResponse();
int32_t si473x_getProperty(unsigned int propertyNumber);
void si473x_setFmBlendStereoThreshold(unsigned char parameter);
void si473x_setFmBlendMonoThreshold(unsigned char parameter);
void si473x_setFmBlendRssiStereoThreshold(unsigned char parameter);
void si473x_setFmBLendRssiMonoThreshold(unsigned char parameter);
void si473x_setFmBlendSnrStereoThreshold(unsigned char parameter);
void si473x_setFmBLendSnrMonoThreshold(unsigned char parameter);
void si473x_setFmBlendMultiPathStereoThreshold(unsigned char parameter);
void si473x_setFmBlendMultiPathMonoThreshold(unsigned char parameter);
void si473x_setFmStereoOff();
void si473x_setFmStereoOn();
void si473x_disableFmDebug();
void si473x_digitalOutputFormat(unsigned char OSIZE, unsigned char OMONO, unsigned char OMODE, unsigned char OFALL);
void si473x_digitalOutputSampleRate(unsigned int DOSR);
void si473x_setVolume(unsigned char volume);
void si473x_setAudioMute(unsigned char off);
unsigned char si473x_getVolume();
void si473x_setRdsConfig(unsigned char RDSEN, unsigned char BLETHA, unsigned char BLETHB, unsigned char BLETHC, unsigned char BLETHD);
void si473x_setRdsIntSource(unsigned char RDSNEWBLOCKB, unsigned char RDSNEWBLOCKA, unsigned char RDSSYNCFOUND, unsigned char RDSSYNCLOST, unsigned char RDSRECV);
unsigned char si473x_updateRdsStatus(unsigned char INTACK, unsigned char MTFIFO, unsigned char STATUSONLY);
unsigned int si473x_getRdsPI(void);
unsigned char si473x_getRdsGroupType(void);
unsigned char si473x_getRdsFlagAB(void);
unsigned char si473x_getRdsTextSegmentAddress(void);
unsigned char si473x_getRdsVersionCode(void);
unsigned char si473x_getRdsProgramType(void);
void si473x_getNext2Block(char c[]);
void si473x_getNext4Block(char c[]);
void si473x_getRdsText(char rds_buffer2A[]);
unsigned char si473x_getRdsText0A(char rds_buffer0A[]);
unsigned char si473x_getRdsText2A(char rds_buffer2A[]);
unsigned char si473x_getRdsText2B(char rds_buffer2B[]);
unsigned char si473x_getRdsTime(char rds_time[]);
void si473x_setSSBBfoOffset(int offset);
void si473x_setSSBConfig(unsigned char AUDIOBW, unsigned char SBCUTFLT, unsigned char AVC_DIVIDER, unsigned char AVCEN, unsigned char SMUTESEL, unsigned char DSP_AFCDIS);
void si473x_setSSBDspAfc(unsigned char DSP_AFCDIS);
void si473x_setSSBAvcDivider(unsigned char AVC_DIVIDER);
void si473x_setSSBSync(unsigned char enabled);
void si473x_setSSBSoftMute(unsigned char SMUTESEL);
void si473x_setSSBAutomaticVolumeControl(unsigned char AVCEN);
void si473x_setSBBSidebandCutoffFilter(unsigned char SBCUTFLT);
void si473x_setSSBAudioBandwidth(unsigned char AUDIOBW);
void si473x_setSSBMode(unsigned char usblsb);
unsigned char si473x_setSSB(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step, unsigned char usblsb);
void si473x_sendSSBModeProperty();
si473x_firmware_query_library si473x_queryLibraryId(unsigned char *isOK);
void si473x_patchPowerUp();
void si473x_ssbPowerUp();
void si473x_setProperty(unsigned int propertyNumber, unsigned int param);
unsigned int si473x_getAntennaTuningCapacitor();
void si473x_setAmSoftMuteMaxAttenuation(unsigned char smattn);
void si473x_setSsbSoftMuteMaxAttenuation(unsigned char smattn);
unsigned char si473x_getCurrentAvcAmMaxGain();
unsigned char si473x_getCurrentVolume();
void si473x_setAmDelayNB(unsigned int value);
void si473x_setFMDeEmphasis(unsigned char parameter);
void si473x_setAMDeEmphasis(unsigned char parameter);
void si473x_setAMSoftMuteSlop(unsigned char parameter);
void si473x_setAMSoftMuteRate(unsigned char parameter);
void si473x_setAMSoftMuteSnrThreshold(unsigned char parameter);
void si473x_setAMSoftMuteReleaseRate(unsigned char parameter);
void si473x_setAMSoftMuteAttackRate(unsigned int parameter);
unsigned char si473x_isCurrentTuneFM();
unsigned char si473x_isCurrentTuneAM();
unsigned char si473x_isCurrentTuneSSB();
unsigned char si473x_getTuneFrequecyFast();
void si473x_setTuneFrequencyFast(unsigned char FAST);
unsigned char si473x_getTuneFrequecyFreeze();
void si473x_setTuneFrequencyFreze(unsigned char FREEZE);
void si473x_setFrequencyStep(unsigned int step);
unsigned int si473x_getCurrentFrequency();
unsigned char si473x_getCurrentBandTuneCmd();
unsigned char si473x_getCurrentSsbStatus();
unsigned char si473x_downloadPatch(__prog__ const unsigned char ssb_patch_content[], const unsigned int ssb_patch_content_size);
void si473x_loadPatch(__prog__ const unsigned char ssb_patch_content[], const unsigned int ssb_patch_content_size, unsigned char ssb_audiobw);