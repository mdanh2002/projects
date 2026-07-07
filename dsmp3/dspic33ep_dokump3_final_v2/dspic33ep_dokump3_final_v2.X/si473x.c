/**
 * @mainpage SI47XX Arduino Library implementation 
 * This is a library for the SI4735, BROADCAST AM/FM/SW RADIO RECEIVER, IC from Silicon Labs for the 
 * Arduino development environment.  It works with I2C protocol and can provide an easier interface for controlling the SI47XX CI family.<br>
 * This library was built based on [Si47XX PROGRAMMING GUIDE-AN332 (REV 1.0)](https://www.silabs.com/documents/public/application-notes/AN332.pdf) document from Silicon Labs. 
 * It also can be used on **all members of the SI473X family** respecting, of course, the features available for each IC version. 
 * These functionalities can be seen in the comparison matrix shown in table 1 (Product Family Function); pages 2 and 3 of the programming guide.
 * If you need to build a prototype based on SI47XX device, see <https://pu2clr.github.io/SI4735/><br>
 * This library has more than 20 examples and it can be freely distributed using the MIT Free Software model. [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/SI4735/#mit-license).  
 * Contact: pu2clr@gmail.com
 * @details This class has a set of functions that can help you to build your receiver based on Si47XX IC family.
 * @details This library uses the I²C communication protocol and implements most of the functions offered by Si47XX (BROADCAST AM / FM / SW / LW RADIO RECEIVER) IC family from Silicon Labs. 
 * @details Currently you have more than 120 functions implemented to control the Si47XX devices. These functions are listed and documented here.
 * @details Some methods were implemented using  resource.  methods are implemented in SI4735.h
 * IMPORTANT: According to Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 207, "For write operations, the system controller next 
 * sends a data unsigned char on SDIO, which is captured by the device on rising edges of SCLK. The device acknowledges 
 * each data unsigned char by driving SDIO low for one cycle on the next falling edge of SCLK. 
 * The system controller may write up to 8 data unsigned chars in a single 2-wire transaction. 
 * The first unsigned char is a command, and the next seven unsigned chars are arguments. Writing more than 8 unsigned chars results 
 * in unpredictable device behavior". So, If you are extending this library, consider that restriction presented earlier.

 * @details This library uses the I²C communication protocol and implements most of the functions offered by Si47XX (BROADCAST AM / FM / SW / LW RADIO RECEIVER) IC family from Silicon Labs. 
 * @details The main features of this library are listed below. 
 * @details 1. Open Source. It is free. You can use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software. See [MIT License](https://pu2clr.github.io/SI4735/#mit-license) to know more.   
 * @details 2. Built Based on [Si47XX PROGRAMMING GUIDE](https://www.silabs.com/documents/public/application-notes/AN332.pdf).
 * @details 3. C++ Language and Object-oriented programming. You can easily extend the SI4735 class by adding more functionalities.
 * @details 4. Available on Arduino IDE (Manage Libraries).
 * @details 5. Cross-platform. You can compile and run this library on most of board available on Arduino IDE (Examples: ATtiny85, boards based on ATmega328 and ATmega-32u4, ATmega2560, 32 ARM Cortex, Arduino DUE, ESP32 and more). See [Boards where this library has been successfully tested](https://pu2clr.github.io/SI4735/#boards-where-this-library-has-been-successfully-tested).
 * @details 6. Simplifies projects based on SS4735-D60, SI4732-A10, SI4730-D60, and other SI473X devices;
 * @details 7. I²C communication and Automatic I²C bus address detection. 
 * @details 8. More than 120 functions implemented. You can customize almost every feature available on Si47XX family. 
 * @details 9. RDS support.
 * @details 10. SSB (Single Side Band) patch support (SI4735-D60 and SI4732-A10). 
 * @details 11. Digital Audio (__Attention__: Crystal and digital audio mode cannot be used at the same time).
 * @details 12. More than 20 example available.  See <https://github.com/pu2clr/SI4735/tree/master/examples><br>
 *  Some texts were extracted directly from the Silicon Labs documentation. The name of the Silicon Labs document and pages are described in the source code comments.
 * @see https://pu2clr.github.io/SI4735/
 * @see Si47XX PROGRAMMING GUIDE AN332 (REV 1.0): https://www.silabs.com/documents/public/application-notes/AN332.pdf
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES
 * @author PU2CLR - Ricardo Lima Caratti 
 * @date  2019-2020
 * @copyright MIT Free Software model. See [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/SI4735/#mit-license). 
 */

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "i2c_lib.h"
#include "si473x.h"

unsigned int si473x_currentMinimumFrequency; //  Minimum frequency of the current band
unsigned int si473x_currentMaximumFrequency; //  Maximum frequency of the current band
unsigned int si473x_currentWorkFrequency; //  Current frequency
unsigned int si473x_currentStep; //  Stores the current step used to increment or decrement the frequency.
unsigned char si473x_currentBandTuneCmd; //  Tell the current tune (FM, AM or SSB)
unsigned char si473x_lastMode = -1; //  Stores the last mode used.
unsigned char si473x_currentAvcAmMaxGain = 48; //  Stores the current Automatic Volume Control Gain for AM. Default value is 48.
unsigned char si473x_currentSsbStatus = 0; //  Whether SSB is active
unsigned char si473x_volume = 32; //  Stores the current vlume setup (0-63).

si473x_set_frequency currentFrequencyParams; //  Frequency Parameters
si473x_response_status currentStatus; //  Current device status
si473x_rds_status currentRdsStatus; //  Current RDS status
si473x_ssb_mode currentSSBMode; //  Indicates if USB or LSB

/** @defgroup group05 Deal with Interrupt and I2C bus */

unsigned char si473x_getCurrentBandTuneCmd()
{
    return si473x_currentBandTuneCmd;
}

unsigned char si473x_getCurrentSsbStatus()
{
    return si473x_currentSsbStatus;
}

/**
 * @brief Enables output for GPO1, 2, and 3. 
 * @details GPO1, 2, and 3 can be configured for output (Hi-Z or active drive) by setting the GPO1OEN, GPO2OEN, and GPO3OEN bit. 
 * @details The state (high or low) of GPO1, 2, and 3 is set with the GPIO_SET command. 
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 * | GPIO Output Enable  | value 0 | value 1 |
 * | ---- ---------------| ------- | ------- | 
 * | GPO1OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 * | GPO2OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 * | GPO3OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 82 and 144
 * @param GPO1OEN 
 * @param GPO2OEN 
 * @param GPO3OEN 
 */
void si473x_setGpioCtl(unsigned char GPO1OEN, unsigned char GPO2OEN, unsigned char GPO3OEN) {
  si473x_gpio gpio;

  gpio.arg.GPO1OEN = GPO1OEN;
  gpio.arg.GPO2OEN = GPO2OEN;
  gpio.arg.GPO3OEN = GPO3OEN;
  gpio.arg.DUMMY1 = 0;
  gpio.arg.DUMMY2 = 0;

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(GPIO_CTL);
  i2c_write(gpio.raw);
  i2c_stop();
}

/**
 * @brief Sets the output level (high or low) for GPO1, 2, and 3.  
 * @details GPO1, 2, and 3 can be configured for output by setting the GPO1OEN, GPO2OEN, and GPO3OEN bit in the GPIO_CTL command. 
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 * | GPIO Output Enable  | value 0 | value 1 |
 * | ---- ---------------| ------- | ------- | 
 * | GPO1LEVEL            |  Output low (default) | Output high |
 * | GPO2LEVEL            |  Output low (default) | Output high |
 * | GPO3LEVEL            |  Output low (default) | Output high |
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 83 and 145
 * @param GPO1LEVEL
 * @param GPO2LEVEL
 * @param GPO3LEVEL
 */
void si473x_setGpio(unsigned char GPO1LEVEL, unsigned char GPO2LEVEL, unsigned char GPO3LEVEL) {
  si473x_gpio gpio;

  gpio.arg.GPO1OEN = GPO1LEVEL;
  gpio.arg.GPO2OEN = GPO2LEVEL;
  gpio.arg.GPO3OEN = GPO3LEVEL;
  gpio.arg.DUMMY1 = 0;
  gpio.arg.DUMMY2 = 0;

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(GPIO_SET);
  i2c_write(gpio.raw);
  i2c_stop();
}

/**
 * @brief Configures the sources for the GPO2/INT interrupt pin.
 * @details Valid sources are the lower 8 bits of the STATUS unsigned char, including CTS, ERR, RSQINT, and STCINT bits. 
 * @details The corresponding bit is set before the interrupt occurs. The CTS bit (and optional interrupt) is set when it is safe to send the next command. 
 * @details The CTS interrupt enable (CTSIEN) can be set with this property and the POWER_UP command.
 * @details The state of the CTSIEN bit set during the POWER_UP command can be read by reading this property and modified by writing this property. 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 146 
 *  
 * @param STCIEN Seek/Tune Complete Interrupt Enable (0 or 1).
 * @param RSQIEN RSQ Interrupt Enable (0 or 1).
 * @param ERRIEN ERR Interrupt Enable (0 or 1).
 * @param CTSIEN CTS Interrupt Enable (0 or 1).
 * @param STCREP STC Interrupt Repeat (0 or 1).
 * @param RSQREP RSQ Interrupt Repeat(0 or 1).
 */
void si473x_setGpioIen(unsigned char STCIEN, unsigned char RSQIEN, unsigned char ERRIEN, unsigned char CTSIEN, unsigned char STCREP, unsigned char RSQREP) {
  si473x_gpio_ien gpio;

  gpio.arg.DUMMY1 = gpio.arg.DUMMY2 = gpio.arg.DUMMY3 = gpio.arg.DUMMY4 = 0;
  gpio.arg.STCIEN = STCIEN;
  gpio.arg.RSQIEN = RSQIEN;
  gpio.arg.ERRIEN = ERRIEN;
  gpio.arg.CTSIEN = CTSIEN;
  gpio.arg.STCREP = STCREP;
  gpio.arg.RSQREP = RSQREP;

  si473x_sendProperty(GPO_IEN, gpio.raw);
}

void si473x_rcv(unsigned char buf[], unsigned char count) {
  unsigned char i;
  // char resBuf[64];
  
  i2c_start();
  i2c_write(SI473X_ADDR_R);
  
  // strcpy(resBuf, "");
  
  // i2c_start();
  for (i = 0; i < count; i++) {
    // do not ack last char
    buf[i] = i2c_rx(i == (count - 1) ? 0 : 1);
    
    // sprintf(resBuf, "%s %02X", resBuf, buf[i]);  
  }
  i2c_stop();
  
  // SendUARTStr(resBuf);
}

/**
 * @brief  Wait for the si473x is ready (Clear to Send (CTS) status bit have to be 1).  
 * @details This function should be used before sending any command to a SI47XX device.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 63, 128
 */
unsigned char si473x_waitToSend() {
  unsigned char buf[1];
   
  unsigned int c = 0;
  do {
      if (c > SI473X_WAIT_TIMEOUT)
      {
          SendUARTStr("siWtSndEr");            
          return 0;
      }
      
    si473x_rcv(buf, 1);
    delay_us(MIN_DELAY_WAIT_SEND_LOOP); // Need to check the minimum value.
        
    c++;
  } while (!(buf[0] & 0b10000000));
  
  return 1;
}

/** @defgroup group07 Device Setup and Start up */

/**
 * @brief Powerup the Si47XX
 * @details Before call this function call the setPowerUp to set up the parameters.
 * @details Parameters you have to set up with setPowerUp
 * | Parameter | Description |
 * | --------- | ----------- |
 * | CTSIEN    | Interrupt anabled or disabled |
 * | GPO2OEN   | GPO2 Output Enable or disabled |
 * | PATCH     | Boot normally or patch |
 * | XOSCEN    | Use external crystal oscillator. 1 = Use crystal oscillator;  (crystal oscillator disabled) |
 * | FUNC      | defaultFunction = 0 = FM Receive; 1 = AM (LW/MW/SW) Receiver |
 * | OPMODE    | SI473X_ANALOG_AUDIO (B00000101) or SI473X_DIGITAL_AUDIO (B00001011) |
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 *see setMaxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_POWERUP 
 * @see  si473x_setPowerUp()
 * @see  Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64, 129
 */
unsigned char si473x_radioPowerUp(unsigned char defaultFunction) {
  si473x_powerup si473x_powerUp; //  Powerup settings    
    
  SendUARTStr("siOn");
  
  si473x_powerUp.arg.CTSIEN = SI473X_INT_ENABLE; // 1 -> Interrupt anabled;
  si473x_powerUp.arg.GPO2OEN = 0; // 1 -> GPO2 Output Enable;
  si473x_powerUp.arg.PATCH = 0; // 0 -> Boot normally;
  si473x_powerUp.arg.XOSCEN = SI473X_CUR_CLOCK_TYPE; // 1 -> Use external crystal oscillator;
  si473x_powerUp.arg.FUNC = defaultFunction; // 0 = FM Receive; 1 = AM/SSB (LW/MW/SW) Receiver.
  si473x_powerUp.arg.OPMODE = SI473X_ANALOG_AUDIO; // 0x5 = 00000101 = Analog audio outputs (LOUT/ROUT).

  // Set the current tuning frequency mode 0X20 = FM and 0x40 = AM (LW/MW/SW)
  // See See Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55 and 124
  if (defaultFunction == 0) {
    si473x_currentBandTuneCmd = FM_TUNE_FREQ;
    currentFrequencyParams.arg.FREEZE = 1;
  } else {
    si473x_currentBandTuneCmd = AM_TUNE_FREQ;
    currentFrequencyParams.arg.FREEZE = 0;
  }
  currentFrequencyParams.arg.FAST = 1;
  currentFrequencyParams.arg.DUMMY1 = 0;
  currentFrequencyParams.arg.ANTCAPH = 0;
  
  // MD: Why set to 1, not 0?
  currentFrequencyParams.arg.ANTCAPL = 1;
  
  // delay_us(1000);
  if (!si473x_waitToSend())
      return 0;
  
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SI473X_POWER_UP);
  i2c_write(si473x_powerUp.raw[0]); // Content of ARG1
  i2c_write(si473x_powerUp.raw[1]); // COntent of ARG2
  i2c_stop();
  // Delay at least 500 ms between powerup command and first tune command to wait for
  // the oscillator to stabilize if XOSCEN is set and crystal is used as the RCLK.
  
  if (!si473x_waitToSend())
      return 0;
  
  delay_ms(MAX_DELAY_AFTER_POWERUP);

  if (SI473X_CUR_CLOCK_TYPE == 0) {
    si473x_setRefClock(SI473X_REF_CLK);
    si473x_setRefClockPrescaler(SI473X_REF_CLK_PRESCALE, SI473X_REF_CLK_SRC_PIN);
  }
  
  // disable all interrupts as we are not going to use them
  si473x_setGpioIen(0, 0, 0, 0, 0, 0);
  
  return 1;
}

/** 
 * @brief Moves the device from powerup to powerdown mode.
 * @details After Power Down command, only the Power Up command is accepted.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 67, 132
 * @see radioPowerUp()
 */
void si473x_radioPowerDown(void) {
  SendUARTStr("siOff");
    
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(POWER_DOWN);
  i2c_stop();
  delay_us(2500);
}

/**
 * @brief Gets firmware information 
 * @details The firmware information will be stored in firmwareInfo member variable 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 66, 131
 * @see firmwareInfo
 */
si473x_firmware_information si473x_getFirmware(unsigned char *isOK) { 
  si473x_waitToSend();

  // SendUARTStr("si473x_getFirmware");
  
  si473x_firmware_information firmwareInfo; //  Firmware information
  
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SI473X_GET_REV);
  i2c_stop();

  unsigned int c = 0;
  do {          
    if (c > 5)
    {
       *isOK = 0;
        
       SendUARTStr("siFiEr");   
       return firmwareInfo;
    }
    
    // Request for 9 unsigned chars response
    si473x_waitToSend();
    si473x_rcv(firmwareInfo.raw, 9);
    c++;
  } while (firmwareInfo.resp.ERR);
  
  *isOK = 1;
  return firmwareInfo;
}

/**
 * @brief Sets the frequency of the REFCLK from the output of the prescaler
 * @details The REFCLK range is 31130 to 34406 Hz (32768 ±5% Hz) in 1 Hz steps, or 0 (to disable AFC). For example, an RCLK of 13 MHz would require a prescaler value of 400 to divide it to 32500 Hz REFCLK.
 * @details The reference clock frequency property would then need to be set to 32500 Hz. 
 * @details RCLK frequencies between 31130 Hz and 40 MHz are supported, however, there are gaps in frequency coverage for prescaler values ranging from 1 to 10, or frequencies up to 311300 Hz. See table below.
 * Table REFCLK Prescaler
 * | Prescaler  | RCLK Low (Hz) | RCLK High (Hz)   |
 * | ---------- | ------------- | ---------------- | 
 * |    1       |   31130       |   34406          |
 * |    2       |   62260       |   68812          |
 * |    3       |   93390       |  103218          |
 * |    4       |  124520       |  137624          |
 * |    5       |  155650       |  172030          | 
 * |    6       |  186780       |  206436          |
 * |    7       |  217910       |  240842          | 
 * |    8       |  249040       |  275248          | 
 * |    9       |  280170       |  309654          | 
 * |   10       |  311300       |  344060          |       
 *  
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 34 and 35
 * @param refclk The allowed REFCLK frequency range is between 31130 and 34406 Hz (32768 ±5%), or 0 (to disable AFC).
 */
void si473x_setRefClock(unsigned int refclk) {
  si473x_sendProperty(REFCLK_FREQ, refclk);
}

/**
 * @brief Sets the number used by the prescaler to divide the external RCLK down to the internal REFCLK. 
 * @details The range may be between 1 and 4095 in 1 unit steps. 
 * @details For example, an RCLK of 13 MHz would require a prescaler value of 400 to divide it to 32500 Hz. The reference clock frequency property would then need to be set to 32500 Hz. 
 * @details ATTENTION by default, this function considers you are using the RCLK pin as clock source. 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 34 and 35
 * @param prescale  Prescaler for Reference Clock value; Between 1 and 4095 in 1 unit steps. Default is 1. 
 * @param rclk_sel  0 = RCLK pin is clock source (default); 1 = DCLK pin is clock source
 */
void si473x_setRefClockPrescaler(unsigned int prescale, unsigned char rclk_sel) {
  si473x_sendProperty(REFCLK_PRESCALE, prescale); //| (rclk_sel << 13)); // Sets the D12 to rclk_sel
}

/** 
 *  
 * @brief Starts the Si473X device. 
 * @details Use this function to start the device up with the parameters shown below.
 * @details If the audio mode parameter is not entered, analog mode will be considered.
 * @details You can use any Arduino digital pin. Be sure you are using less than 3.6V on Si47XX RST pin.   
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 * @param resetPin Digital Arduino Pin used to RESET de Si47XX device. 
 * @param interruptPin interrupt Arduino Pin (see your Arduino pinout). If less than 0, iterrupt disabled.
 * @param defaultFunction is the mode you want the receiver starts.
 * @param audioMode default SI473X_ANALOG_AUDIO (Analog Audio). Use SI473X_ANALOG_AUDIO or SI473X_DIGITAL_AUDIO.
 * @param clockType 0 = Use external RCLK (crystal oscillator disabled); 1 = Use crystal oscillator 
 */
si473x_firmware_information si473x_setup(unsigned char defaultFunction, unsigned char *isOK) {
  // Set the initial SI473X behavior
  // CTSIEN   interruptEnable -> Interrupt anabled or disable;
  // GPO2OEN  1 -> GPO2 Output Enable;
  // PATCH    0 -> Boot normally;
  // XOSCEN   clockType -> Use external crystal oscillator (XOSCEN_CRYSTAL) or reference clock (XOSCEN_RCLK);
  // FUNC     defaultFunction = 0 = FM Receive; 1 = AM (LW/MW/SW) Receiver.
  // OPMODE   SI473X_ANALOG_AUDIO or SI473X_DIGITAL_AUDIO.
  *isOK = si473x_radioPowerUp(defaultFunction);
  
  if (*isOK)
  {
      // ok, set default volume level and return
      si473x_setVolume(30); 
      return si473x_getFirmware(isOK);
  }
  else {
      // error, return dummy valud
      si473x_firmware_information fi;
      return fi;
  }
}

/** @defgroup group08 Tune, Device Mode and Filter setup */

/**
 * @brief Selects the tuning capacitor value.
 * @details On FM mode, The Antenna Tuning Capacitor is valid only when using TXO/LPI pin as the antenna input.
 * This selects the value of the antenna tuning capacitor manually, or automatically if set to zero. 
 * The valid range is 0 to 191. Automatic capacitor tuning is recommended. When tuned manually,the varactor is offset by fourcodes. 
 * For example,if the varactor is set to a value of 5 manually, when read back the value will be 1. The four codes (1pf) delta accounts for the capacitance at the chip.
 * @details on AM mode, If the value is set to anything other than 0, the tuning capacitance is manually set as 95 fF x ANTCAP + 7 pF. 
 * ANTCAP manual range is 1?6143. Automatic capacitor tuning is recommended. In SW mode, ANTCAPH[15:8] (high unsigned char) needs to be set to 0 and ANTCAPL[7:0] (low unsigned char) needs to be set to 1. 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 71 and 136
 * @param capacitor If zero, the tuning capacitor value is selected automatically. 
 *                  If the value is set to anything other than 0:
 *                  AM - the tuning capacitance is manually set as 95 fF x ANTCAP + 7 pF. 
 *                       ANTCAP manual range is 1?6143;
 *                  FM - the valid range is 0 to 191.    
 *                  According to Silicon Labs, automatic capacitor tuning is recommended (value 0). 
 */
void si473x_setTuneFrequencyAntennaCapacitor(unsigned int capacitor) {
  si473x_antenna_capacitor cap;

  cap.value = capacitor;

  currentFrequencyParams.arg.DUMMY1 = 0;

  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) {
    // For FM, the capacitor value has just one unsigned char
    currentFrequencyParams.arg.ANTCAPH = (capacitor <= 191) ? cap.raw.ANTCAPL : 0;
  } else {
    if (capacitor <= 6143) {
      currentFrequencyParams.arg.FREEZE = 0; // This parameter is not used for AM
      currentFrequencyParams.arg.ANTCAPH = cap.raw.ANTCAPH;
      currentFrequencyParams.arg.ANTCAPL = cap.raw.ANTCAPL;
    }
  }

  // Tune the device again with the current frequency.
  si473x_setFrequency(si473x_currentWorkFrequency);
}

/**
 * @brief Set the frequency to the corrent function of the Si4735 (FM, AM or SSB)
 * @details You have to call setup or setPowerUp before call setFrequency.
 * @see MAX_DELAY_AFTER_SET_FREQUENCY()
 * @see MAX_DELAY_AFTER_SET_FREQUENCY
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 70, 135
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 13
 * @param unsigned int  freq is the frequency to change. For example, FM => 10390 = 103.9 MHz; AM => 810 = 810 KHz.
 */
unsigned char si473x_setFrequency(unsigned int freq) {
   si473x_frequency currentFrequency; //  data structure to get current frequency
    
  si473x_waitToSend(); // Wait for the si473x is ready.
  currentFrequency.value = freq;
  currentFrequencyParams.arg.FREQH = currentFrequency.raw.FREQH;
  currentFrequencyParams.arg.FREQL = currentFrequency.raw.FREQL;

  if (si473x_currentSsbStatus != 0) {
    currentFrequencyParams.arg.DUMMY1 = 0;
    currentFrequencyParams.arg.USBLSB = si473x_currentSsbStatus; // Set to LSB or USB
    currentFrequencyParams.arg.FAST = 1; // Used just on AM and FM
    currentFrequencyParams.arg.FREEZE = 0; // Used just on FM
  }

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(si473x_currentBandTuneCmd);
  i2c_write(currentFrequencyParams.raw[0]); // Send a unsigned char with FAST and  FREEZE information; if not FM must be 0;
  i2c_write(currentFrequencyParams.arg.FREQH);
  i2c_write(currentFrequencyParams.arg.FREQL);
  i2c_write(currentFrequencyParams.arg.ANTCAPH);
  // If current tune is not FM sent one more unsigned char
  if (si473x_currentBandTuneCmd != FM_TUNE_FREQ)
    i2c_write(currentFrequencyParams.arg.ANTCAPL);

  i2c_stop();
  si473x_waitToSend(); // Wait for the si473x is ready.
  si473x_currentWorkFrequency = freq; // check it
  delay_ms(MAX_DELAY_AFTER_SET_FREQUENCY); // For some reason I need to delay here.

  unsigned char isValid = 0;
  unsigned int newFreq = si473x_getFrequency(&isValid);  
  if (newFreq != freq)
  {
      SendUARTStr("siFrEr");
      return 0;
  }
  else {
      // SendUARTStr("freqOk");
      return 1;
  }
}

/**
 * @brief Sets the radio to AM (LW/MW/SW) function. 
 * @details The example below sets the band from 550KHz to 1750KHz on AM mode. The band will start on 810KHz and step is 10KHz. 
 * @code
 * si4735.setAM(520, 1750, 810, 10); 
 * @endcode
 *  
 * @see setFM()
 * @see setSSB()
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency 
 * @param step step used to go to the next channel
 */
unsigned char si473x_setAM(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step) {    
  si473x_currentMinimumFrequency = fromFreq;
  si473x_currentMaximumFrequency = toFreq;
  si473x_currentStep = step;

  if (initialFreq < fromFreq || initialFreq > toFreq)
    initialFreq = fromFreq;

  // If you?re already using AM mode, it is not necessary to call powerDown and radioPowerUp.
  // The other properties also should have the same value as the previous status.
  if (si473x_lastMode != AM_CURRENT_MODE) {
    si473x_radioPowerDown();
    si473x_radioPowerUp(POWER_UP_AM);
    si473x_setAvcAmMaxGain(SI473X_AVC_AM_MAX_GAIN_NORMAL); // Set AM Automatic Volume Gain 
    si473x_setVolume(si473x_volume); // Set to previous configured volume
  }
  
  si473x_currentSsbStatus = 0;
  si473x_lastMode = AM_CURRENT_MODE;

  si473x_currentWorkFrequency = initialFreq;
  return si473x_setFrequency(si473x_currentWorkFrequency);  
}

/**
 * @brief Sets the radio to FM function. 
 * @details Defines the band range you want to use for the FM mode. 
 * @details The example below sets the band from 64MHz to 108MHzKHz on FM mode. The band will start on 103.9MHz and step is 100KHz.
 * On FM mode, the step 10 means 100KHz. If you want a 1MHz step, use 100.     
 * @code
 * si4735.setFM(6400, 10800, 10390, 10);
 * @endcode
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 70
 * @see setFM()
 * @see setFrequencyStep()
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency (default frequency)
 * @param step step used to go to the next channel   
 */
unsigned char si473x_setFM(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step) {    
  si473x_currentMinimumFrequency = fromFreq;
  si473x_currentMaximumFrequency = toFreq;
  si473x_currentStep = step;

  if (initialFreq < fromFreq || initialFreq > toFreq)
    initialFreq = fromFreq;

  si473x_radioPowerDown();
  si473x_radioPowerUp(POWER_UP_FM);
  si473x_setVolume(si473x_volume); // Set to previous configured volume
  si473x_currentSsbStatus = 0;
  si473x_disableFmDebug();
  si473x_lastMode = FM_CURRENT_MODE;

  si473x_currentWorkFrequency = initialFreq;
  return si473x_setFrequency(si473x_currentWorkFrequency);  
}

/** @defgroup group08 Tune */

/**
 * @brief Selects the bandwidth of the channel filter for AM reception. 
 * @details The choices are 6, 4, 3, 2, 2.5, 1.8, or 1 (kHz). The default bandwidth is 2 kHz. It works only in AM / SSB (LW/MW/SW) 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 125, 151, 277, 181.
 * @param AMCHFLT the choices are:   0 = 6 kHz Bandwidth                    
 *                                   1 = 4 kHz Bandwidth
 *                                   2 = 3 kHz Bandwidth
 *                                   3 = 2 kHz Bandwidth
 *                                   4 = 1 kHz Bandwidth
 *                                   5 = 1.8 kHz Bandwidth
 *                                   6 = 2.5 kHz Bandwidth, gradual roll off
 *                                   7-15 = Reserved (Do not use).
 * @param AMPLFLT Enables the AM Power Line Noise Rejection Filter.
 */
void si473x_setBandwidth(unsigned char AMCHFLT, unsigned char AMPLFLT) {
  si473x_bandwidth_config filter;
  si473x_property property;

  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) // Only for AM/SSB mode
    return;

  if (AMCHFLT > 6)
    return;

  filter.raw[0] = filter.raw[1] = 0;

  property.value = AM_CHANNEL_FILTER;

  filter.param.AMCHFLT = AMCHFLT;
  filter.param.AMPLFLT = AMPLFLT;

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00); // Always 0x00
  i2c_write(property.raw.byteHigh); // High unsigned char first
  i2c_write(property.raw.byteLow); // Low unsigned char after
  i2c_write(filter.raw[1]); // Raw data for AMCHFLT and
  i2c_write(filter.raw[0]); // AMPLFLT
  i2c_stop();

  si473x_waitToSend();
}

/**
 * @brief Gets the current frequency of the Si4735 (AM or FM)
 * @details The method status do it an more. See getStatus below. 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 73 (FM) and 139 (AM)
 */
unsigned int si473x_getFrequency(unsigned char *isValid) {
  si473x_frequency freq;
  si473x_updateCurrentStatus(0, 1);

  freq.raw.FREQL = currentStatus.resp.READFREQL;
  freq.raw.FREQH = currentStatus.resp.READFREQH;

  *isValid = currentStatus.resp.VALID;
  
  si473x_currentWorkFrequency = freq.value;
  return freq.value;
}

/**
 * @brief Gets the current status  of the Si4735 (AM or FM)
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 73 (FM) and 139 (AM)
 * @param unsigned char INTACK Seek/Tune Interrupt Clear. If set, clears the seek/tune complete interrupt status indicator;
 * @param unsigned char CANCEL Cancel seek. If set, aborts a seek currently in progress;
 */
unsigned char si473x_updateCurrentStatus(unsigned char INTACK, unsigned char CANCEL) {
  si473x_tune_status status;
  unsigned int c = 0;
  unsigned char cmd = (si473x_currentBandTuneCmd == FM_TUNE_FREQ) ? FM_TUNE_STATUS : AM_TUNE_STATUS;

  si473x_waitToSend();

  status.arg.INTACK = INTACK;
  status.arg.CANCEL = CANCEL;
  status.arg.RESERVED2 = 0;

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(cmd);
  i2c_write(status.raw);
  i2c_stop();
  // Reads the current status (including current frequency).
  do {
      if (c > SI473X_WAIT_TIMEOUT)
      {
          SendUARTStr("siStEr");
          return 0;
      }
          
    si473x_waitToSend();
    si473x_rcv(currentStatus.raw, 8);
    c++;
  } while (currentStatus.resp.ERR); // If error, try it again
  si473x_waitToSend();
  
  return 1;
}

/**
 * @brief Queries Automatic Gain Control STATUS
 * @details After call this method, you can call isAgcEnabled to know the AGC status and getAgcGainIndex to know the gain index value.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 80; for AM page 142.
 * @see AN332 REV 0.8 Universal Programming Guide Amendment for SI4735-D60 SSB and NBFM patches; page 18. 
 */
si473x_agc_status si473x_getAutomaticGainControl() {
  unsigned char cmd;
  unsigned int c = 0;

  si473x_agc_status currentAgcStatus; //  Current AGC status

  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) { // FM TUNE
    cmd = FM_AGC_STATUS;
  } else { // AM TUNE - SAME COMMAND used on SSB mode
    cmd = AM_AGC_STATUS;
  }

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(cmd);
  i2c_stop();

  do {
      if (c > SI473X_WAIT_TIMEOUT)
      {
          SendUARTStr("siAgEr");
          break;
      }
      
    si473x_waitToSend();
    si473x_rcv(currentAgcStatus.raw, 3);
    c++;
  } while (currentAgcStatus.refined.ERR); // If error, try get AGC status again.
  
  return currentAgcStatus;
}

/** 
 * @brief Automatic Gain Control setup 
 * @details If FM, overrides AGC setting by disabling the AGC and forcing the LNA to have a certain gain that ranges between 0 
 * (minimum attenuation) and 26 (maximum attenuation).
 * @details If AM/SSB, Overrides the AGC setting by disabling the AGC and forcing the gain index that ranges between 0 
 * (minimum attenuation) and 37+ATTN_BACKUP (maximum attenuation).
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 81; for AM page 143 
 * @param unsigned char AGCDIS This param selects whether the AGC is enabled or disabled (0 = AGC enabled; 1 = AGC disabled);
 * @param unsigned char AGCIDX AGC Index (0 = Minimum attenuation (max gain); 1 ? 36 = Intermediate attenuation); 
 *                if >greater than 36 - Maximum attenuation (min gain) ).
 */
void si473x_setAutomaticGainControl(unsigned char AGCDIS, unsigned char AGCIDX) {
  si473x_agc_overrride agc;

  unsigned char cmd;

  cmd = (si473x_currentBandTuneCmd == FM_TUNE_FREQ) ? FM_AGC_OVERRIDE : AM_AGC_OVERRIDE; // AM_AGC_OVERRIDE = SSB_AGC_OVERRIDE = 0x48

  agc.arg.AGCDIS = AGCDIS;
  agc.arg.AGCIDX = AGCIDX;

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(cmd);
  i2c_write(agc.raw[0]);
  i2c_write(agc.raw[1]);
  i2c_stop();

  si473x_waitToSend();
}

/**
 * @brief Sets the maximum gain for automatic volume control.
 * Datasheet only allows 15-bit for the gain, max raw value is 32767 or 96dB, min is 12dB
 *  
 * @details If no parameter is sent, it will be consider 48dB.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 152
 * @see setAvcAmMaxGain()
 * @param unsigned char gain  Select a value between 12 and 192.  Default value 48dB.
 */
void si473x_setAvcAmMaxGain(unsigned char gain) {
  unsigned int aux;
  aux = (gain > 12 && gain < 97) ? (gain * 340) : (48 * 340);
  si473x_currentAvcAmMaxGain = gain;
  si473x_sendProperty(AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN, aux);
}

/**
 * @brief Queries the status of the Received Signal Quality (RSQ) of the current channel.
 * @details This method sould be called berore call getCurrentRSSI(), getCurrentSNR() etc.
 * Command FM_RSQ_STATUS
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 75 and 141
 * @param INTACK Interrupt Acknowledge. 
 *        0 = Interrupt status preserved; 
 *        1 = Clears RSQINT, BLENDINT, SNRHINT, SNRLINT, RSSIHINT, RSSILINT, MULTHINT, MULTLINT.
 */
si473x_rqs_status si473x_getCurrentReceivedSignalQuality(unsigned char INTACK) {
  unsigned char arg;
  unsigned char cmd;
  int sizeResponse;

  si473x_rqs_status currentRqsStatus; //  Current Radio SIgnal Quality status
  
  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) { // FM TUNE
    cmd = FM_RSQ_STATUS;
    sizeResponse = 8; // Check it
  } else { // AM TUNE
    cmd = AM_RSQ_STATUS;
    sizeResponse = 6; // Check it
  }

  si473x_waitToSend();

  arg = INTACK;
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(cmd);
  i2c_write(arg); // send B00000001
  i2c_stop();

  // Check it
  si473x_waitToSend();
  si473x_rcv(currentRqsStatus.raw, sizeResponse);
  
  return currentRqsStatus;
}

/**
 * @brief Look for a station (Automatic tune)
 * @details Starts a seek process for a channel that meets the RSSI and SNR criteria for AM.  
 * @details __This function does not work on SSB mode__.  
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55, 72, 125 and 137
 * @param SEEKUP Seek Up/Down. Determines the direction of the search, either UP = 1, or DOWN = 0. 
 * @param Wrap/Halt. Determines whether the seek should Wrap = 1, or Halt = 0 when it hits the band limit.
 */
void si473x_seekStation(unsigned char SEEKUP, unsigned char WRAP) {
  si473x_seek seek;
  si473x_seek_am_complement seek_am_complement;

  // Check which FUNCTION (AM or FM) is working now
  unsigned char seek_start_cmd = (si473x_currentBandTuneCmd == FM_TUNE_FREQ) ? FM_SEEK_START : AM_SEEK_START;

  si473x_waitToSend();

  seek.arg.SEEKUP = SEEKUP;
  seek.arg.WRAP = WRAP;
  seek.arg.RESERVED1 = 0;
  seek.arg.RESERVED2 = 0;

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(seek_start_cmd);
  i2c_write(seek.raw); // ARG1

  if (seek_start_cmd == AM_SEEK_START) // Sets additional configuration for AM mode
  {
    seek_am_complement.ARG2 = seek_am_complement.ARG3 = 0;
    seek_am_complement.ANTCAPH = 0;
    seek_am_complement.ANTCAPL = (si473x_currentWorkFrequency > 1800) ? 1 : 0; // if SW = 1
    i2c_write(seek_am_complement.ARG2); // ARG2 - Always 0
    i2c_write(seek_am_complement.ARG3); // ARG3 - Always 0
    i2c_write(seek_am_complement.ANTCAPH); // ARG4 - Tuning Capacitor: The tuning capacitor value
    i2c_write(seek_am_complement.ANTCAPL); // ARG5 - will be selected automatically.
  }

  i2c_stop();

}

/**
 * @brief Sets the bottom frequency and top frequency of the AM band for seek. Default is 520 to 1710.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 127, 161, and 162
 * @param unsigned int bottom - the bottom of the AM (MW/SW) mode for seek
 * @param unsigned int    top - the top of the AM (MW/SW) mode for seek
 */
void si473x_setSeekAmLimits(unsigned int bottom, unsigned int top) {
  si473x_sendProperty(AM_SEEK_BAND_BOTTOM, bottom);
  si473x_sendProperty(AM_SEEK_BAND_TOP, top);
}

/**
 * @brief Sets the bottom frequency and top frequency of the FM band for seek. Default is 8750 to 10790.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 100 and  101
 * @param unsigned int bottom - the bottom of the FM(VHF) mode for seek
 * @param unsigned int    top - the top of the FM(VHF) mode for seek
 */
void si473x_setSeekFmLimits(unsigned int bottom, unsigned int top) {
  si473x_sendProperty(FM_SEEK_BAND_BOTTOM, bottom);
  si473x_sendProperty(FM_SEEK_BAND_TOP, top);
}

/**
 * @brief Selects frequency spacing for AM seek. Default is 10 kHz spacing.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 163, 229 and 283
 * @param unsigned int spacing - step in KHz
 */
void si473x_setSeekAmSpacing(unsigned int spacing) {
  si473x_sendProperty(AM_SEEK_FREQ_SPACING, spacing);
}

/**
 * @brief Selects frequency spacing for FM seek. Default is 100 kHz spacing. There are only 3 valid values: 5, 10, and 20.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 101
 * @param unsigned int spacing - step in KHz
 */
void si473x_setSeekFmSpacing(unsigned int spacing) {
  si473x_sendProperty(FM_SEEK_FREQ_SPACING, spacing);
}

/**
 * @brief Sets the SNR threshold for a valid AM Seek/Tune. 
 * @details If the value is zero then SNR threshold is not considered when doing a seek. Default value is 5 dB.
 * @see Si47XX PROGRAMMING GUIDE;  (REV 1.0); page 127
 */
void si473x_setSeekAmSrnThreshold(unsigned int value) {
  si473x_sendProperty(AM_SEEK_SNR_THRESHOLD, value);
}

/**
 * @brief Sets the SNR threshold for a valid FM Seek/Tune. 
 * @details SNR Threshold which determines if a valid channel has been found during Seek/Tune. Specified in units of dB in 1 dB steps (0?127). Default is 3 dB
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 102
 * @param value between 0 and 127.
 */
void si473x_setSeekFmSrnThreshold(unsigned int value) {
  si473x_sendProperty(FM_SEEK_TUNE_SNR_THRESHOLD, value);
}

/**
 * @brief Sets the RSSI threshold for a valid AM Seek/Tune. 
 * @details If the value is zero then RSSI threshold is not considered when doing a seek. Default value is 25 dB?V.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 127
 */
void si473x_setSeekAmRssiThreshold(unsigned int value) {
  si473x_sendProperty(AM_SEEK_RSSI_THRESHOLD, value);
}

/**
 * @brief Sets the RSSI threshold for a valid FM Seek/Tune.
 * @details RSSI threshold which determines if a valid channel has been found during seek/tune. Specified in units of dB?V in 1 dB?V steps (0?127). Default is 20 dB?V.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 102
 */
void si473x_setSeekFmRssiThreshold(unsigned int value) {
  si473x_sendProperty(FM_SEEK_TUNE_RSSI_THRESHOLD, value);
}

/** @defgroup group10 Tools method 
 * @details A set of functions used to support other functions
 */

/**
 * @brief Sends (sets) property to the SI47XX
 * @details This method is used for others to send generic properties and params to SI47XX
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 68, 124 and  133.
 * @see setProperty, sendCommand, getProperty, getCommandResponse
 * @param propertyNumber property number (example: RX_VOLUME)
 * @param parameter   property value that will be seted
 */
void si473x_sendProperty(unsigned int propertyNumber, unsigned int parameter) {
  si473x_property property;
  si473x_property param;

  property.value = propertyNumber;
  param.value = parameter;
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00);
  i2c_write(property.raw.byteHigh); // Send property - High unsigned char - most significant first
  i2c_write(property.raw.byteLow); // Send property - Low unsigned char - less significant after
  i2c_write(param.raw.byteHigh); // Send the argments. High Byte - Most significant first
  i2c_write(param.raw.byteLow); // Send the argments. Low Byte - Less significant after
  i2c_stop();
  delay_us(550);
}

/**
 * @brief Sends a given command to the SI47XX devices. 
 * @details This function can be useful when you want to execute a SI47XX device command and it was not implemented by this library.
 * @details In this case you have to check the  AN332-Si47XX PROGRAMMING GUIDE to know how the command works. 
 * @details Also, you need to work with bit operators to compose the parameters of the command [ &(and), ?(xor), |(or) etc ].   
 * @see getCommandResponse, setProperty
 *  
 * @param cmd command number (see AN332-Si47XX PROGRAMMING GUIDE)
 * @param parameter_size Parameter size in unsigned chars. Tell the number of argument used by the command.
 * @param parameter unsigned unsigned char array with the arguments of the command  
 */
void si473x_sendCommand(unsigned char cmd, int parameter_size,
  const unsigned char * parameter) {
  unsigned char i;

  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  // Sends the command to the device
  i2c_write(cmd);
  // Sends the argments (parameters) of the command
  for (i = 0; i < parameter_size; i++)
    i2c_write(parameter[i]);
  i2c_stop();
}

/**
 * @brief   Returns with the command response.  
 * @details After a command is executed by the device, you can get the result (response) of the command by calling this method.
 * @see sendCommand, setProperty
 * @param response_size  num of unsigned chars returned by the command.
 * @param response  unsigned char array where the response will be stored.     
 */
void si473x_getCommandResponse(int response_size, unsigned char * response) {
  si473x_rcv(response, response_size);
}

/**
 * @brief Gets the first unsigned char response. 
 * @details In this context status is the first response unsigned char for any SI47XX command. See si47x_status structure. 
 * @details This function can be useful to check, for example, the success or failure of a command. 
 * @see si47x_status
 */
si473x_status si473x_getStatusResponse() {
  si473x_status status;

   si473x_waitToSend();
   i2c_start();
   i2c_write(SI473X_ADDR_W);
   i2c_write(GET_INT_STATUS);
   i2c_stop();
    
  unsigned char buf[1];
  si473x_rcv(buf, 1);
  status.raw = buf[0];

  return status;
}

/**
 * @brief Gets a given property from the SI47XX
 * @details This method is used to get a given property from SI47XX
 * @details You might need to extract set of bits information from the returned value to know the real value
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55, 69, 124 and  134.
 * @see sendProperty, setProperty, sendCommand, getCommandResponse
 * @param propertyNumber property number (example: RX_VOLUME)
 */
int32_t si473x_getProperty(unsigned int propertyNumber) {
  si473x_property property;
  si473x_status status;

  property.value = propertyNumber;
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(GET_PROPERTY);
  i2c_write(0x00);
  i2c_write(property.raw.byteHigh); // Send property - High unsigned char - most significant first
  i2c_write(property.raw.byteLow); // Send property - Low unsigned char - less significant after
  i2c_stop();

  si473x_waitToSend();
  unsigned char buf[4];
  si473x_rcv(buf, 4);

  status.raw = buf[0];

  // if error, return 0;
  if (status.refined.ERR == 1)
    return -1;

  // gets the property value
  property.raw.byteHigh = buf[2];
  property.raw.byteLow = buf[3];

  return property.value;
}

/** @defgroup group12 FM Mono Stereo audio setup */

/**
 *  
 * @brief Sets RSSI threshold for stereo blend (Full stereo above threshold, blend below threshold). 
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default is 49
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 90. 
 * @param parameter  valid values: 0 to 127
 */
void si473x_setFmBlendStereoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_STEREO_THRESHOLD, parameter);
}

/**
 * @brief Sets RSSI threshold for mono blend (Full mono below threshold, blend above threshold). 
 * @details To force stereo set this to 0. To force mono set this to 127. Default value is 30 dB?V.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 56.
 * @param parameter valid values: 0 to 127
 */
void si473x_setFmBlendMonoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_MONO_THRESHOLD, parameter);
}

/** 
 * @brief Sets RSSI threshold for stereo blend. (Full stereo above threshold, blend below threshold.) 
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 49 dB?V.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59. 
 * @param parameter valid values: 0 to 127
 */
void si473x_setFmBlendRssiStereoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_RSSI_STEREO_THRESHOLD, parameter);
}

/** 
 * @brief Sets RSSI threshold for mono blend (Full mono below threshold, blend above threshold). 
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 30 dB?V.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.  
 * @param parameter valid values: 0 to 127
 */
void si473x_setFmBLendRssiMonoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_RSSI_MONO_THRESHOLD, parameter);
}

/**
 * @brief Sets SNR threshold for stereo blend (Full stereo above threshold, blend below threshold). 
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 27 dB.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.  
 * @param parameter valid values: 0 to 127
 */
void si473x_setFmBlendSnrStereoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_SNR_STEREO_THRESHOLD, parameter);
}

/**
 * @brief Sets SNR threshold for mono blend (Full mono below threshold, blend above threshold). 
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 14 dB.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59. 
 * @param parameter valid values: 0 to 127 
 */
void si473x_setFmBLendSnrMonoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_SNR_MONO_THRESHOLD, parameter);
}

/** 
 * @brief Sets multipath threshold for stereo blend (Full stereo below threshold, blend above threshold). 
 * @details To force stereo, set this to 100. To force mono, set this to 0. Default value is 20.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 60.
 * @param parameter valid values: 0 to 100 
 */
void si473x_setFmBlendMultiPathStereoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_MULTIPATH_STEREO_THRESHOLD, parameter);
}

/**
 * @brief Sets Multipath threshold for mono blend (Full mono above threshold, blend below threshold). 
 * @details To force stereo, set to 100. To force mono, set to 0. The default is 60.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 60.
 * @param parameter valid values: 0 to 100 
 */
void si473x_setFmBlendMultiPathMonoThreshold(unsigned char parameter) {
  si473x_sendProperty(FM_BLEND_MULTIPATH_MONO_THRESHOLD, parameter);
}

/** 
 * @todo 
 * @brief Turn Off Stereo operation.
 */
void si473x_setFmStereoOff() {
  si473x_setFmBlendStereoThreshold(127);
  si473x_setFmBlendMonoThreshold(127);
  si473x_setFmBlendRssiStereoThreshold(127);
  si473x_setFmBLendRssiMonoThreshold(127);
  si473x_setFmBlendSnrStereoThreshold(127);
  si473x_setFmBLendSnrMonoThreshold(127);
  si473x_setFmBlendMultiPathStereoThreshold(0);
  si473x_setFmBlendMultiPathMonoThreshold(0);
}

/** 
 * @todo 
 * @brief Turn On Stereo operation.
 */
void si473x_setFmStereoOn() {
  si473x_setFmBlendStereoThreshold(49);
  si473x_setFmBlendMonoThreshold(30);
  si473x_setFmBlendRssiStereoThreshold(49);
  si473x_setFmBLendRssiMonoThreshold(30);
  si473x_setFmBlendSnrStereoThreshold(27);
  si473x_setFmBLendSnrMonoThreshold(14);
  si473x_setFmBlendMultiPathStereoThreshold(20);
  si473x_setFmBlendMultiPathMonoThreshold(60);
}

/**
 * @brief There is a debug feature that remains active in Si4704/05/3x-D60 firmware which can create periodic noise in audio.
 * @details Silicon Labs recommends you disable this feature by sending the following unsigned chars (shown here in hexadecimal form):
 * 0x12 0x00 0xFF 0x00 0x00 0x00.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 299. 
 */
void si473x_disableFmDebug() {
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(0x12);
  i2c_write(0x00);
  i2c_write(0xFF);
  i2c_write(0x00);
  i2c_write(0x00);
  i2c_write(0x00);
  i2c_stop();
  delay_us(2500);
}

/** @defgroup group13 Audio setup */

/**
 * @brief Configures the digital audio output format. 
 * @details Options: DCLK edge, data format, force mono, and sample precision.
 *  
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 * @see Si47XX PROGRAMINGGUIDE; AN332 (REV 1.0); page 195.
 * @see Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; AN383; rev 0.8; page 6; 

 * @param unsigned char OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
 * @param unsigned char OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
 * @param unsigned char OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
 * @param unsigned char OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
 */
void si473x_digitalOutputFormat(unsigned char OSIZE, unsigned char OMONO, unsigned char OMODE, unsigned char OFALL) {
  si473x_digital_output_format df;
  df.refined.OSIZE = OSIZE;
  df.refined.OMONO = OMONO;
  df.refined.OMODE = OMODE;
  df.refined.OFALL = OFALL;
  df.refined.dummy = 0;
  si473x_sendProperty(DIGITAL_OUTPUT_FORMAT, df.raw);
}

/**
 * @brief Enables digital audio output and configures digital audio output sample rate in samples per second (sps).
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 * @see Si47XX PROGRAMINGGUIDE; AN332 (REV 1.0); page 196. 
 * @see Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES; AN383; rev 0.8; page 6
 * @param unsigned int DOSR Diital Output Sample Rate(32?48 ksps .0 to disable digital audio output).
 */
void si473x_digitalOutputSampleRate(unsigned int DOSR) {
  si473x_sendProperty(DIGITAL_OUTPUT_SAMPLE_RATE, DOSR);
}

/** 
 * @brief Sets volume level (0  to 63)
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 62, 123, 170, 173 and 204
 * @param unsigned char volume (domain: 0 - 63) 
 */
void si473x_setVolume(unsigned char volume) {
  si473x_sendProperty(RX_VOLUME, volume);
  si473x_volume = volume;
}

/**
 * @brief Sets the audio on or off. 
 * @details Useful to mute the audio output of the SI47XX device. This function does not work to reduce the pop in the speaker at start the system up.
 * @details If you want to remove the loud click or pop in the speaker at start, power down and power up commands, use setHardwareAudioMute with a external mute circuit. 
 * @see See Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 62, 123, 171 
 * @see setHardwareAudioMute
 * @param value if 1, mute the audio; if 0 unmute the audio.
 */
void si473x_setAudioMute(unsigned char off) {
  unsigned int value = (off) ? 3 : 0; // 3 means mute; 0 means unmute
  si473x_sendProperty(RX_HARD_MUTE, value);
}

/**
 * @brief Gets the current volume level.
 * @see setVolume()
 */
unsigned char si473x_getVolume() {
  return si473x_volume;
}

/**
 * @defgroup group16 FM RDS/RBDS
 * @todo RDS Dynamic PS or Scrolling PS support  
 */

/*******************************************************************************
 * RDS implementation 
 ******************************************************************************/

/**
 * @brief Sets RDS property
 * @details Configures RDS settings to enable RDS processing (RDSEN) and set RDS block error thresholds. 
 * @details When a RDS Group is received, all block errors must be less than or equal the associated block 
 * error threshold for the group to be stored in the RDS FIFO. 
 * @details IMPORTANT: 
 * All block errors must be less than or equal the associated block error threshold 
 * for the group to be stored in the RDS FIFO. 
 * |Value | Description |
 * |------| ----------- | 
 * | 0    | No errors |
 * | 1    | 1?2 bit errors detected and corrected |
 * | 2    | 3?5 bit errors detected and corrected |
 * | 3    | Uncorrectable |
 * @details Recommended Block Error Threshold options:
 * | Examples | Description |
 * | -------- | ----------- |
 * | 2,2,2,2  | No group stored if any errors are uncorrected |
 * | 3,3,3,3  | Group stored regardless of errors |
 * | 0,0,0,0  | No group stored containing corrected or uncorrected errors |
 * | 3,2,3,3  | Group stored with corrected errors on B, regardless of errors on A, C, or D | 
 *  
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 104
 * @param unsigned char RDSEN RDS Processing Enable; 1 = RDS processing enabled.
 * @param unsigned char BLETHA Block Error Threshold BLOCKA.   
 * @param unsigned char BLETHB Block Error Threshold BLOCKB.  
 * @param unsigned char BLETHC Block Error Threshold BLOCKC.  
 * @param unsigned char BLETHD Block Error Threshold BLOCKD. 
 */
void si473x_setRdsConfig(unsigned char RDSEN, unsigned char BLETHA, unsigned char BLETHB, unsigned char BLETHC, unsigned char BLETHD) {
  si473x_property property;
  si473x_rds_config config;

  si473x_waitToSend();

  // Set property value
  property.value = FM_RDS_CONFIG;

  // Arguments
  config.arg.RDSEN = RDSEN;
  config.arg.BLETHA = BLETHA;
  config.arg.BLETHB = BLETHB;
  config.arg.BLETHC = BLETHC;
  config.arg.BLETHD = BLETHD;
  config.arg.DUMMY1 = 0;

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00); // Always 0x00 (I need to check it)
  i2c_write(property.raw.byteHigh); // Send property - High unsigned char - most significant first
  i2c_write(property.raw.byteLow); // Low unsigned char
  i2c_write(config.raw[1]); // Send the argments. Most significant first
  i2c_write(config.raw[0]);
  i2c_stop();
  delay_us(550);
}

/** 
 * @brief Configures interrupt related to RDS
 * @details Use this method if want to use interrupt
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 103
 * @param RDSRECV If set, generate RDSINT when RDS FIFO has at least FM_RDS_INT_FIFO_COUNT entries.
 * @param RDSSYNCLOST If set, generate RDSINT when RDS loses synchronization.
 * @param RDSSYNCFOUND set, generate RDSINT when RDS gains synchronization.
 * @param RDSNEWBLOCKA If set, generate an interrupt when Block A data is found or subsequently changed
 * @param RDSNEWBLOCKB If set, generate an interrupt when Block B data is found or subsequently changed
 */
void si473x_setRdsIntSource(unsigned char RDSNEWBLOCKB, unsigned char RDSNEWBLOCKA, unsigned char RDSSYNCFOUND, unsigned char RDSSYNCLOST, unsigned char RDSRECV) {
  si473x_property property;
  si473x_rds_int_source rds_int_source;

  if (si473x_currentBandTuneCmd != FM_TUNE_FREQ)
    return;

  rds_int_source.refined.RDSNEWBLOCKB = RDSNEWBLOCKB;
  rds_int_source.refined.RDSNEWBLOCKA = RDSNEWBLOCKA;
  rds_int_source.refined.RDSSYNCFOUND = RDSSYNCFOUND;
  rds_int_source.refined.RDSSYNCLOST = RDSSYNCLOST;
  rds_int_source.refined.RDSRECV = RDSRECV;
  rds_int_source.refined.DUMMY1 = 0;
  rds_int_source.refined.DUMMY2 = 0;

  property.value = FM_RDS_INT_SOURCE;

  si473x_waitToSend();

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00); // Always 0x00 (I need to check it)
  i2c_write(property.raw.byteHigh); // Send property - High unsigned char - most significant first
  i2c_write(property.raw.byteLow); // Low unsigned char
  i2c_write(rds_int_source.raw[1]); // Send the argments. Most significant first
  i2c_write(rds_int_source.raw[0]);
  i2c_stop();
  si473x_waitToSend();
}

/**
 * @brief Gets the RDS status. Store the status in currentRdsStatus member. RDS COMMAND FM_RDS_STATUS
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55 and 77
 * @param INTACK Interrupt Acknowledge; 0 = RDSINT status preserved. 1 = Clears RDSINT.
 * @param MTFIFO 0 = If FIFO not empty, read and remove oldest FIFO entry; 1 = Clear RDS Receive FIFO.
 * @param STATUSONLY Determines if data should be removed from the RDS FIFO.
 */
unsigned char si473x_updateRdsStatus(unsigned char INTACK, unsigned char MTFIFO, unsigned char STATUSONLY) {
  si473x_rds_command rds_cmd;
  unsigned int c = 0;
  
  // checking current FUNC (Am or FM)
  if (si473x_currentBandTuneCmd != FM_TUNE_FREQ)
    return 0;

  si473x_waitToSend();

  rds_cmd.arg.INTACK = INTACK;
  rds_cmd.arg.MTFIFO = MTFIFO;
  rds_cmd.arg.STATUSONLY = STATUSONLY;

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(FM_RDS_STATUS);
  i2c_write(rds_cmd.raw);
  i2c_stop();

  do {
      if (c > SI473X_WAIT_TIMEOUT)
      {
          SendUARTStr("siRdEr");
          return 0;
      }
          
    si473x_waitToSend();
    si473x_rcv(currentRdsStatus.raw, 13);
    c++;
  } while (currentRdsStatus.resp.ERR);
  delay_us(550);
  
  return 1;
}

// See s methods / functions on SI4735.h

/**  
 * @brief Returns the programa type. 
 * @details Read the Block A content
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 77 and 78
 */
unsigned int si473x_getRdsPI(void) {
  if (currentRdsStatus.resp.RDSRECV && currentRdsStatus.resp.RDSNEWBLOCKA) {
    return currentRdsStatus.resp.BLOCKAL;
  }
  return 0;
}

/**
 * @brief Returns the Group Type (extracted from the Block B)
 */
unsigned char si473x_getRdsGroupType(void) {
  si473x_rds_blockb blkb;

  blkb.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
  blkb.raw.highValue = currentRdsStatus.resp.BLOCKBH;

  return blkb.refined.groupType;
}

/**
 * @brief Returns the current Text Flag A/B  
 */
unsigned char si473x_getRdsFlagAB(void) {
  si473x_rds_blockb blkb;

  blkb.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
  blkb.raw.highValue = currentRdsStatus.resp.BLOCKBH;

  return blkb.refined.textABFlag;
}

/**
 * @brief Returns the address of the text segment.
 * @details 2A - Each text segment in version 2A groups consists of four characters. A messages of this group can be 
 * have up to 64 characters. 
 * @details 2B - In version 2B groups, each text segment consists of only two characters. When the current RDS status is
 *      using this version, the maximum message length will be 32 characters.
 */
unsigned char si473x_getRdsTextSegmentAddress(void) {
  si473x_rds_blockb blkb;
  blkb.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
  blkb.raw.highValue = currentRdsStatus.resp.BLOCKBH;

  return blkb.refined.content;
}

/**
 * @brief Gets the version code (extracted from the Block B)
 */
unsigned char si473x_getRdsVersionCode(void) {
  si473x_rds_blockb blkb;

  blkb.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
  blkb.raw.highValue = currentRdsStatus.resp.BLOCKBH;

  return blkb.refined.versionCode;
}

/**  
 * @brief Returns the Program Type (extracted from the Block B)
 * @see https://en.wikipedia.org/wiki/Radio_Data_System
 */
unsigned char si473x_getRdsProgramType(void) {
  si473x_rds_blockb blkb;

  blkb.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
  blkb.raw.highValue = currentRdsStatus.resp.BLOCKBH;

  return blkb.refined.programType;
}

/**
 * @brief Process data received from group 2B
 * @param c  char array reference to the "group 2B" text 
 */
void si473x_getNext2Block(char c[]) {
  char raw[2];
  int i, j;

  raw[1] = currentRdsStatus.resp.BLOCKDL;
  raw[0] = currentRdsStatus.resp.BLOCKDH;

  for (i = j = 0; i < 2; i++) {
    if (raw[i] == 0xD || raw[i] == 0xA) {
      c[j] = '\0';
      return;
    }
    if (raw[i] >= 32) {
      c[j] = raw[i];
      j++;
    } else {
      c[i] = ' ';
    }
  }
}

/**
 * @brief Process data received from group 2A
 * @param c  char array reference to the "group  2A" text 
 */
void si473x_getNext4Block(char c[]) {
  char raw[4];
  int i, j;

  raw[0] = currentRdsStatus.resp.BLOCKCH;
  raw[1] = currentRdsStatus.resp.BLOCKCL;
  raw[2] = currentRdsStatus.resp.BLOCKDH;
  raw[3] = currentRdsStatus.resp.BLOCKDL;
  for (i = j = 0; i < 4; i++) {
    if (raw[i] == 0xD || raw[i] == 0xA) {
      c[j] = '\0';
      return;
    }
    if (raw[i] >= 32) {
      c[j] = raw[i];
      j++;
    } else {
        c[i] = ' ';
    }
  }
}

/**
 * @brief Gets the RDS Text when the message is of the Group Type 2 version A
 */
void si473x_getRdsText(char rds_buffer2A[]) {

  // Needs to get the "Text segment address code".
  // Each message should be ended by the code 0D (Hex)
    int rdsTextAdress2A = 0;
    
  if (rdsTextAdress2A >= 16)
    rdsTextAdress2A = 0;

  si473x_getNext4Block( & rds_buffer2A[rdsTextAdress2A * 4]);

  rdsTextAdress2A += 4;
}

/**
 * @todo RDS Dynamic PS or Scrolling PS 
 * @brief Gets the station name  
 * However, some stations send other kind of messages in this field
 */
unsigned char si473x_getRdsText0A(char rds_buffer0A[]) {
  si473x_rds_blockb blkB;

  // getRdsStatus();

  unsigned int rdsTextAdress0A = 0;
  
  if (currentRdsStatus.resp.RDSRECV) {
    if (si473x_getRdsGroupType() == 0) {
      // Process group type 0
      blkB.raw.highValue = currentRdsStatus.resp.BLOCKBH;
      blkB.raw.lowValue = currentRdsStatus.resp.BLOCKBL;

      rdsTextAdress0A = blkB.group0.address;
      if (rdsTextAdress0A >= 0 && rdsTextAdress0A < 4) {
        si473x_getNext2Block( & rds_buffer0A[rdsTextAdress0A * 2]);
        rds_buffer0A[8] = '\0';
        
        return rdsTextAdress0A;
      }
      else {
        return 0xFF;   
      }
    }
    else {
       // SendUARTStr("Rds0ANA");
       return 0xFF;
    }
  }
  else {
      // SendUARTStr("RdsNA");
      return 0xFF;
  }
}

/**
 * @brief Gets the Text processed for the 2A group
 */
unsigned char si473x_getRdsText2A(char rds_buffer2A[]) {
  si473x_rds_blockb blkB;

  unsigned int  rdsTextAdress2A = 0;
  
  // getRdsStatus();
  if (currentRdsStatus.resp.RDSRECV) {
    if (si473x_getRdsGroupType() == 2 /* && getRdsVersionCode() == 0 */ ) {
      // Process group 2A
      // Decode B block information
      blkB.raw.highValue = currentRdsStatus.resp.BLOCKBH;
      blkB.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
      rdsTextAdress2A = blkB.group2.address;

      if (rdsTextAdress2A >= 0 && rdsTextAdress2A < 16) {
        si473x_getNext4Block( & rds_buffer2A[rdsTextAdress2A * 4]);
        rds_buffer2A[63] = '\0';
      }
      return 1;
    }
    else {
        // SendUARTStr("Rds2ANA");
        return 0;
    }
  }
  else {
      // SendUARTStr("RdsNA");
      return 0;
  }
}

/**
 * @brief Gets the Text processed for the 2B group
 */
unsigned char si473x_getRdsText2B(char rds_buffer2B[]) {
  si473x_rds_blockb blkB;

  unsigned int rdsTextAdress2B = 0;
  
  // getRdsStatus();
  // if (getRdsReceived())
  // {
  // if (getRdsNewBlockB())
  // {
  if (si473x_getRdsGroupType() == 2 /* && getRdsVersionCode() == 1 */ ) {
    // Process group 2B
    blkB.raw.highValue = currentRdsStatus.resp.BLOCKBH;
    blkB.raw.lowValue = currentRdsStatus.resp.BLOCKBL;
    rdsTextAdress2B = blkB.group2.address;
    if (rdsTextAdress2B >= 0 && rdsTextAdress2B < 16) {
      si473x_getNext2Block( & rds_buffer2B[rdsTextAdress2B * 2]);
      rds_buffer2B[32] = '\0';
    }
    return 1;
  }
  else {
      // SendUARTStr("Rds2BNA");
      return 0;
  }
  //  }
  // }  
}

/**
 * @brief Gets the RDS time and date when the Group type is 4 
 */
unsigned char si473x_getRdsTime(char rds_time[]) {
  // Under Test and construction
  // Need to check the Group Type before.
  si473x_rds_date_time dt;

  unsigned int minute;
  unsigned int hour;

  if (si473x_getRdsGroupType() == 4) {
    char offset_sign;
    int offset_h;
    int offset_m;

    // unsigned int y, m, d;

    dt.raw[4] = currentRdsStatus.resp.BLOCKBL;
    dt.raw[5] = currentRdsStatus.resp.BLOCKBH;
    dt.raw[2] = currentRdsStatus.resp.BLOCKCL;
    dt.raw[3] = currentRdsStatus.resp.BLOCKCH;
    dt.raw[0] = currentRdsStatus.resp.BLOCKDL;
    dt.raw[1] = currentRdsStatus.resp.BLOCKDH;

    // Unfortunately it was necessary to wotk well on the GCC compiler on 32-bit
    // platforms. See si47x_rds_date_time (typedef union) and CGG ?Crosses boundary? issue/features.
    // Now it is working on Atmega328, STM32, Arduino DUE, ESP32 and more.
    minute = (dt.refined.minute2 << 2) | dt.refined.minute1;
    hour = (dt.refined.hour2 << 4) | dt.refined.hour1;

    offset_sign = (dt.refined.offset_sense == 1) ? '+' : '-';
    offset_h = (dt.refined.offset * 30) / 60;
    offset_m = (dt.refined.offset * 30) - (offset_h * 60);
    // sprintf(rds_time, "%02u:%02u %c%02u:%02u", dt.refined.hour, dt.refined.minute, offset_sign, offset_h, offset_m);
    sprintf(rds_time, "%02u:%02u %c%02u:%02u", hour, minute, offset_sign, offset_h, offset_m);
    
    return 1;
  }
  else {
    SendUARTStr("RdsHRNA");
    return 0;
  }
}

/**
 * @defgroup group17 Si4735-D60 Single Side Band (SSB) support
 * @brief Single Side Band (SSB) implementation.<br>  
 * First of all, it is important to say that the SSB patch content **is not part of this library**. 
 * The paches used here were made available by Mr. Vadim Afonkin on his [Dropbox repository](https://www.dropbox.com/sh/xzofrl8rfaaqh59/AAA5au2_CVdi50NBtt0IivyIa?dl=0). 
 * It is important to note that the author of this library does not encourage anyone to use the SSB patches content for commercial purposes.
 * In other words, this library only supports SSB patches, the patches themselves are not part of this library.  
 * @details This implementation was tested only on Si4735-D60  and SI4732-A10 devices. 
 * @details SSB modulation is a refinement of amplitude modulation that one of the side band and the carrier are suppressed.
 * @details What does SSB patch means?
 * In this context, a patch is a piece of software used to change the behavior of the SI4735-D60/SI4732-A10 device.
 * There is little information available about patching the SI4735-D60/SI4732-A10.
 *  
 * The following information is the understanding of the author of this project and 
 * it is not necessarily correct. 
 * A patch is executed internally (run by internal MCU) of the device. Usually, 
 * patches are used to fixes bugs or add improvements and new features of the firmware
 * installed in the internal ROM of the device. Patches to the SI4735 are distributed
 * in binary form and have to be transferred to the internal RAM of the device by the 
 * host MCU (in this case Arduino boards). 
 * Since the RAM is volatile memory, the patch stored into the device gets lost when 
 * you turn off the system. Consequently, the content of the patch has to be transferred
 * again to the device each time after turn on the system or reset the device.
 * I would like to thank Mr Vadim Afonkin for making available the SSBRX patches for 
 * SI4735-D60/SI4732-A10 on his Dropbox repository. On this repository you have two files, 
 * amrx_6_0_1_ssbrx_patch_full_0x9D29.csg and amrx_6_0_1_ssbrx_patch_init_0xA902.csg. 
 * It is important to know that the patch content of the original files is constant 
 * hexadecimal representation used by the language C/C++. Actally, the original files 
 * are in ASCII format (not in binary format). 
 * If you are not using C/C++ or if you want to load the files directly to the SI4735, 
 * you must convert the values to numeric value of the hexadecimal constants. 
 * For example: 0x15 = 21 (00010101); 0x16 = 22 (00010110); 0x01 = 1 (00000001); 
 * 0xFF = 255 (11111111);
 * @details ATTENTION: The author of this project does not guarantee that procedures shown 
 * here will work in your development environment. Given this, it is at your own risk 
 * to continue with the procedures suggested here. This library works with the I²C 
 * communication protocol and it is designed to apply a SSB extension PATCH to  
 * SI4735-D60 and SI4732-A10 devices. Once again, the author disclaims any liability for any damage this 
 * procedure may cause to your SI4735-D60 or SI4732-A10 or other devices that you are using.
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 3 and 5 
 */

/**
 *  
 * @brief Sets the SSB Beat Frequency Offset (BFO). 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 5 and 23
 * @param offset 16-bit signed value (unit in Hz). The valid range is -16383 to +16383 Hz. 
 */
void si473x_setSSBBfoOffset(int offset) {

  si473x_property property;
  si473x_frequency bfo_offset;

  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) // Only for AM/SSB mode
    return;

  si473x_waitToSend();

  property.value = SSB_BFO;
  bfo_offset.value = offset;

  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00); // Always 0x00
  i2c_write(property.raw.byteHigh); // High unsigned char first
  i2c_write(property.raw.byteLow); // Low unsigned char after
  i2c_write(bfo_offset.raw.FREQH); // Offset freq. high unsigned char first
  i2c_write(bfo_offset.raw.FREQL); // Offset freq. low unsigned char first

  i2c_stop();
  delay_us(550);
}

/**
 * @brief Sets the SSB receiver mode.
 * @details You can use this method for:  
 * @details 1) Enable or disable AFC track to carrier function for receiving normal AM signals;
 * @details 2) Set the audio bandwidth;
 * @details 3) Set the side band cutoff filter;
 * @details 4) Set soft-mute based on RSSI or SNR;
 * @details 5) Enable or disbable automatic volume control (AVC) function. 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 * @param AUDIOBW SSB Audio bandwidth; 0 = 1.2KHz (default); 1=2.2KHz; 2=3KHz; 3=4KHz; 4=500Hz; 5=1KHz.
 * @param SBCUTFLT SSB side band cutoff filter for band passand low pass filter
 *                 if 0, the band pass filter to cutoff both the unwanted side band and high frequency 
 *                  component > 2KHz of the wanted side band (default).
 * @param AVC_DIVIDER set 0 for SSB mode; set 3 for SYNC mode.
 * @param AVCEN SSB Automatic Volume Control (AVC) enable; 0=disable; 1=enable (default).
 * @param SMUTESEL SSB Soft-mute Based on RSSI or SNR.
 * @param DSP_AFCDIS DSP AFC Disable or enable; 0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable. 
 */
void si473x_setSSBConfig(unsigned char AUDIOBW, unsigned char SBCUTFLT, unsigned char AVC_DIVIDER, unsigned char AVCEN, unsigned char SMUTESEL, unsigned char DSP_AFCDIS) {
  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ) // Only AM/SSB mode
    return;

  currentSSBMode.param.AUDIOBW = AUDIOBW;
  currentSSBMode.param.SBCUTFLT = SBCUTFLT;
  currentSSBMode.param.AVC_DIVIDER = AVC_DIVIDER;
  currentSSBMode.param.AVCEN = AVCEN;
  currentSSBMode.param.SMUTESEL = SMUTESEL;
  currentSSBMode.param.DUMMY1 = 0;
  currentSSBMode.param.DSP_AFCDIS = DSP_AFCDIS;

  si473x_sendSSBModeProperty();
}

/**
 * @brief Sets DSP AFC disable or enable
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 * @param DSP_AFCDIS 0 = SYNC mode, AFC enable; 1 = SSB mode, AFC disable
 */
void si473x_setSSBDspAfc(unsigned char DSP_AFCDIS) {
  currentSSBMode.param.DSP_AFCDIS = DSP_AFCDIS;
  si473x_sendSSBModeProperty();
}

/**
 * @brief Sets SSB Soft-mute Based on RSSI or SNR Selection: 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 * @param SMUTESEL  0 = Soft-mute based on RSSI (default); 1 = Soft-mute based on SNR.
 */
void si473x_setSSBSoftMute(unsigned char SMUTESEL) {
  currentSSBMode.param.SMUTESEL = SMUTESEL;
  si473x_sendSSBModeProperty();
}

/**
 * @brief Sets SSB Automatic Volume Control (AVC) for SSB mode
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 * @param AVCEN 0 = Disable AVC; 1 = Enable AVC (default).
 */
void si473x_setSSBAutomaticVolumeControl(unsigned char AVCEN) {
  currentSSBMode.param.AVCEN = AVCEN;
  si473x_sendSSBModeProperty();
}

/**
 * @brief Sets AVC Divider
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24  
 * @param AVC_DIVIDER  SSB mode, set divider = 0; SYNC mode, set divider = 3; Other values = not allowed.
 */
void si473x_setSSBAvcDivider(unsigned char AVC_DIVIDER) {
  currentSSBMode.param.AVC_DIVIDER = AVC_DIVIDER;
  si473x_sendSSBModeProperty();
}

void si473x_setSSBSync(unsigned char enabled)
{
    // AVC_DIVIDER  - set 0 for SSB mode; set 3 for SYNC mode.
    si473x_setSSBAvcDivider(enabled ? 3 : 0);
    
    // DSP_AFCDIS - DSP AFC Disable or enable; 0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable.
    // si473x_setSSBAutomaticVolumeControl(enabled);
}

/**  
 * @brief Sets SBB Sideband Cutoff Filter for band pass and low pass filters.
 * @details 0 = Band pass filter to cutoff both the unwanted side band and high frequency components > 2.0 kHz of the wanted side band. (default)
 * @details 1 = Low pass filter to cutoff the unwanted side band. 
 * Other values = not allowed.
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 *  
 * @param SBCUTFLT 0 or 1; see above
 */
void si473x_setSBBSidebandCutoffFilter(unsigned char SBCUTFLT) {
  currentSSBMode.param.SBCUTFLT = SBCUTFLT;
  si473x_sendSSBModeProperty();
}

/**
 * @brief SSB Audio Bandwidth for SSB mode
 * @details 0 = 1.2 kHz low-pass filter  (default).
 * @details 1 = 2.2 kHz low-pass filter.
 * @details 2 = 3.0 kHz low-pass filter.
 * @details 3 = 4.0 kHz low-pass filter.
 * @details 4 = 500 Hz band-pass filter for receiving CW signal, i.e. [250 Hz, 750 Hz] with center 
 * frequency at 500 Hz when USB is selected or [-250 Hz, -750 1Hz] with center frequency at -500Hz 
 * when LSB is selected* .
 * @details 5 = 1 kHz band-pass filter for receiving CW signal, i.e. [500 Hz, 1500 Hz] with center 
 * frequency at 1 kHz when USB is selected or [-500 Hz, -1500 1 Hz] with center frequency 
 *     at -1kHz when LSB is selected.
 * @details Other values = reserved.
 * @details If audio bandwidth selected is about 2 kHz or below, it is recommended to set SBCUTFLT[3:0] to 0 
 * to enable the band pass filter for better high- cut performance on the wanted side band. Otherwise, set it to 1.
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24 
 * @param AUDIOBW the valid values are 0, 1, 2, 3, 4 or 5; see description above
 */
void si473x_setSSBAudioBandwidth(unsigned char AUDIOBW) {
  // Sets the audio filter property parameter
  currentSSBMode.param.AUDIOBW = AUDIOBW;
  si473x_sendSSBModeProperty();
}

/**
 * @brief Set the radio to AM function. 
 * @todo Adjust the power up parameters
 * @details Set the radio to SSB (LW/MW/SW) function. 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 13 and 14 
 * @see setAM()
 * @see setFrequencyStep()
 * @see void si473x_setFrequency(unsigned int freq)
 * @param usblsb upper or lower side band;  1 = LSB; 2 = USB
 */
void si473x_setSSBMode(unsigned char usblsb) {
  // Is it needed to load patch when switch to SSB?
  // powerDown();
  // It starts with the same AM parameters.
  // setPowerUp(1, 1, 0, 1, 1, currentAudioMode);
  // si473x_radioPowerUp(POWER_UP_AM);
  // ssbPowerUp(); // Not used for regular operation
  // si473x_setVolume(si473x_volume); // Set to previous configured volume
  
    si473x_currentSsbStatus = usblsb;
  
  // si473x_lastMode = SSB_CURRENT_MODE;
}

/**
 *  
 * @details Tunes the SSB (LSB or USB) receiver to a frequency between 520 and 30 MHz in 1 kHz steps. 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 13 and 14
 * @see setAM()
 * @see setFrequencyStep()
 * @see void si473x_setFrequency(unsigned int freq)
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency 
 * @param step step used to go to the next channel  
 * @param usblsb SSB Upper Side Band (USB) and Lower Side Band (LSB) Selection; 
 *               value 2 (banary 10) = USB; 
 *               value 1 (banary 01) = LSB.   
 */
unsigned char si473x_setSSB(unsigned int fromFreq, unsigned int toFreq, unsigned int initialFreq, unsigned int step, unsigned char usblsb) {
  si473x_currentMinimumFrequency = fromFreq;
  si473x_currentMaximumFrequency = toFreq;
  si473x_currentStep = step;
  
  si473x_lastMode = SSB_CURRENT_MODE;
          
  if (initialFreq < fromFreq || initialFreq > toFreq)
    initialFreq = fromFreq;

  si473x_setSSBMode(usblsb);
              
  // TODO: Does not seem to have effects in SSB mode
  si473x_setAvcAmMaxGain(SI473X_AVC_AM_MAX_GAIN_SSB); // Set AM Automatic Volume Gain to 48
  si473x_setVolume(si473x_volume); // Set to previous configured volume
    
  si473x_currentWorkFrequency = initialFreq;
  si473x_setFrequency(si473x_currentWorkFrequency);
  delay_us(550);
  
  unsigned char isValid = 0;
  unsigned int newFreq = si473x_getFrequency(&isValid);  
  if (newFreq != initialFreq)
  {
      SendUARTStr("ssbEr");
      return 0;
  }
  else {
      SendUARTStr("ssbOk");
      return 1;
  }
  
}

/**  
 * @brief Just send the property SSB_MOD to the device.  Internal use (private method). 
 */
void si473x_sendSSBModeProperty() {
  si473x_property property;
  property.value = SSB_MODE;
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SET_PROPERTY);
  i2c_write(0x00); // Always 0x00
  i2c_write(property.raw.byteHigh); // High unsigned char first
  i2c_write(property.raw.byteLow); // Low unsigned char after
  i2c_write(currentSSBMode.raw[1]); // SSB MODE params; freq. high unsigned char first
  i2c_write(currentSSBMode.raw[0]); // SSB MODE params; freq. low unsigned char after

  i2c_stop();
  delay_us(550);
}

/***************************************************************************************
 * SI47XX PATCH RESOURCES
 **************************************************************************************/

/** 
 * @brief Query the library information of the Si47XX device 
 * @details Used to confirm if the patch is compatible with the internal device library revision.
 * @details You have to call this function if you are applying a patch on SI47XX (SI4735-D60/SI4732-A10).
 * @details The first command that is sent to the device is the POWER_UP command to confirm 
 * that the patch is compatible with the internal device library revision. 
 * @details The device moves into the powerup mode, returns the reply, and moves into the 
 * powerdown mode. 
 * @details The POWER_UP command is sent to the device again to configure 
 * the mode of the device and additionally is used to start the patching process.
 * @details When applying the patch, the PATCH bit in ARG1 of the POWER_UP command must be 
 * set to 1 to begin the patching process. [AN332 (REV 1.0) page 219].
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 215-220.
 * @see struct si47x_firmware_query_library
 */
si473x_firmware_query_library si473x_queryLibraryId(unsigned char *isOK) {
  si473x_firmware_query_library libraryID;
  unsigned int c = 0;

  si473x_radioPowerDown(); // Is it necessary

  // delay_ms(500);

  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SI473X_POWER_UP);
  i2c_write(0b00011111); // Set to Read Library ID, disable interrupt; disable GPO2OEN; boot normaly; enable External Crystal Oscillator  .
  i2c_write(SI473X_ANALOG_AUDIO); // Set to Analog Line Input.
  i2c_stop();

  do {
      if (c > SI473X_WAIT_TIMEOUT)
      {
          SendUARTStr("siLibEr");
          *isOK = 0;
          break;
      }
          
    si473x_waitToSend();
    si473x_rcv(libraryID.raw, 8);
    c++;
  } while (libraryID.resp.ERR); // If error found, try it again.

  delay_us(2500);

  *isOK = 1;
  return libraryID;
}

/**  
 *  
 * @brief This method can be used to prepare the device to apply SSBRX patch
 *  
 * @details Call queryLibraryId before call this method. Powerup the device by issuing the POWER_UP 
 * command with FUNC = 1 (AM/SW/LW Receive).
 * @see setMaxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_POWERUP 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 215-220 and
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES; page 7.
 */
void si473x_patchPowerUp() {
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(SI473X_POWER_UP);
  i2c_write(0b00110001); // Set to AM, Enable External Crystal Oscillator; Set patch enable; GPO2 output disabled; CTS interrupt disabled.
  i2c_write(SI473X_ANALOG_AUDIO); // Set to Analog Output
  i2c_stop();
  delay_ms(MAX_DELAY_AFTER_POWERUP);
}

/**
 * @brief This function can be useful for debug and test. 
 */
/*
void si473x_ssbPowerUp() {
  si473x_waitToSend();
  i2c_start();
  i2c_write(SI473X_ADDR_W);
  i2c_write(POWER_UP);
  i2c_write(0b00010001); // Set to AM/SSB, disable interrupt; disable GPO2OEN; boot normaly; enable External Crystal Oscillator  .
  i2c_write(0b00000101); // Set to Analog Line Input.
  i2c_stop();
  delay_us(2500);

  si473x_powerUp.arg.CTSIEN = SI473X_INT_ENABLE; // 1 -> Interrupt anabled;
  si473x_powerUp.arg.GPO2OEN = 0; // 1 -> GPO2 Output Enable;
  si473x_powerUp.arg.PATCH = 0; // 0 -> Boot normally;
  si473x_powerUp.arg.XOSCEN = SI473X_CUR_CLOCK_TYPE; // 1 -> Use external crystal oscillator;
  si473x_powerUp.arg.FUNC = 1; // 0 = FM Receive; 1 = AM/SSB (LW/MW/SW) Receiver.
  si473x_powerUp.arg.OPMODE = 0b00000101; // 0x5 = 00000101 = Analog audio outputs (LOUT/ROUT).
}
*/

/**
 *  
 * @brief Transfers the content of a patch stored in a array of unsigned chars to the SI4735 device. 
 *  
 * @details You must mount an array as shown below and know the size of that array as well.
 *  @details It is importante to say  that patches to the SI4735 are distributed in binary form and 
 *  have to be transferred to the internal RAM of the device by the host MCU (in this case Arduino).
 *  Since the RAM is volatile memory, the patch stored into the device gets lost when you turn off 
 *  the system. Consequently, the content of the patch has to be transferred again to the device 
 *  each time after turn on the system or reset the device.
 *  @details The disadvantage of this approach is the amount of memory used by the patch content. 
 *  This may limit the use of other radio functions you want implemented in Arduino.
 *  @details Example of content:
 *  @code
 *  const PROGMEM unsigned char ssb_patch_content_full[] =
 *   { // SSB patch for whole SSBRX full download
 *       0x15, 0x00, 0x0F, 0xE0, 0xF2, 0x73, 0x76, 0x2F,
 *       0x16, 0x6F, 0x26, 0x1E, 0x00, 0x4B, 0x2C, 0x58,
 *       0x16, 0xA3, 0x74, 0x0F, 0xE0, 0x4C, 0x36, 0xE4,
 *          .
 *          .
 *          .
 *       0x16, 0x3B, 0x1D, 0x4A, 0xEC, 0x36, 0x28, 0xB7,
 *       0x16, 0x00, 0x3A, 0x47, 0x37, 0x00, 0x00, 0x00,
 *       0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D, 0x29};   
 *  const int size_content_full = sizeof ssb_patch_content_full;
 *  @endcode
 * @see Si47XX PROGRAMMING GUIDE; ;AN332 (REV 1.0) pages 64 and 215-220.  
 *  @param ssb_patch_content point to array of unsigned chars content patch.
 *  @param ssb_patch_content_size array size (number of unsigned chars). The maximum size allowed for a patch is 15856 unsigned chars
 */
unsigned char si473x_downloadPatch(__prog__ const unsigned char ssb_patch_content[], const unsigned int ssb_patch_content_size) {
  unsigned char content;
  register int i, offset;
  // Send patch to the SI4735 device
  for (offset = 0; offset < (int) ssb_patch_content_size; offset += 8) {
    i2c_start();
    i2c_write(SI473X_ADDR_W);
    for (i = 0; i < 8; i++) {
      content = ssb_patch_content[i + offset];
      i2c_write(content);
    }
    i2c_stop();

    // Testing download performance
    // approach 1 - Faster - less secure (it might crash in some architectures)
    delay_us(MIN_DELAY_WAIT_SEND_LOOP); // Need check the minimum value
  
    // approach 3 - same approach 2
    // si473x_waitToSend();

    // approach 4 - safer
    /*
    si473x_waitToSend();
    unsigned char cmd_status;
    // Uncomment the lines below if you want to check erro.
    Wire.requestFrom(deviceAddress, 1);
    cmd_status = Wire.read();
    // The SI4735 issues a status after each 8 unsigned char transfered.
    // Just the bit 7 (CTS) should be seted. if bit 6 (ERR) is seted, the system halts.
    if (cmd_status != 0x80)
       return 0;
    */
  }
  delay_us(250);
  return 1;
}

/**
 * @brief Loads a given SSB patch content
 * @details Configures the Si4735-D60/SI4732-A10 device to work with SSB. 
 * @param ssb_patch_content        point to patch content array 
 * @param ssb_patch_content_size   size of patch content 
 * @param ssb_audiobw              SSB Audio bandwidth; 0 = 1.2KHz (default); 1=2.2KHz; 2=3KHz; 3=4KHz; 4=500Hz; 5=1KHz.
 */
void si473x_loadPatch(__prog__ const unsigned char ssb_patch_content[], const unsigned int ssb_patch_content_size, unsigned char ssb_audiobw) {
  // reset();

  unsigned char isOK = 0;
  si473x_queryLibraryId(&isOK);
  si473x_patchPowerUp();
  delay_ms(50);
  si473x_downloadPatch(ssb_patch_content, ssb_patch_content_size);
  // Parameters
  // AUDIOBW - SSB Audio bandwidth; 0 = 1.2KHz (default); 1=2.2KHz; 2=3KHz; 3=4KHz; 4=500Hz; 5=1KHz;
  // SBCUTFLT SSB - side band cutoff filter for band passand low pass filter ( 0 or 1)
  // AVC_DIVIDER  - set 0 for SSB mode; set 3 for SYNC mode.
  // AVCEN - SSB Automatic Volume Control (AVC) enable; 0=disable; 1=enable (default).
  // SMUTESEL - SSB Soft-mute Based on RSSI or SNR (0 or 1).
  // DSP_AFCDIS - DSP AFC Disable or enable; 0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable.
  si473x_setSSBConfig(ssb_audiobw, 1, 0, 0, 0, 1);
  delay_ms(25);
}

/**
 * @brief Sets a given SI47XX device property 
 * @details Sets the Si47XX device with a given attribute. 
 * @details You might need to use the bit operations or some bit field structure to set right the values.
 * @details Used this function instead of the sendProperty.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55, 69, 124 and  134.
 * @see getProperty, sendProperty
 * @param propertyNumber
 * @param param  pamameter value 
 */
void si473x_setProperty(unsigned int propertyNumber, unsigned int param) {
  si473x_sendProperty(propertyNumber, param);
};

/**
 * @brief Get the Antenna Tuning Capacitor value
 * @details Returns the current antenna tuning capacitor value. The tuning capacitance is 95 fF x READANTCAP + 7 pF.
 * @details ON AM or SSB mode, the MULT attribute sotores the high unsigned char of READANTCAP and the attribute READANTCAP by itself stores the low unsigned char.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 74,74, 140 and 141.
 */
unsigned int si473x_getAntennaTuningCapacitor() {
  si473x_antenna_capacitor cap;

  if (si473x_currentBandTuneCmd == FM_TUNE_FREQ)
    return currentStatus.resp.READANTCAP;
  else {
    cap.raw.ANTCAPL = currentStatus.resp.READANTCAP; // On AM it is the low unsigned char the READANTCAP value
    cap.raw.ANTCAPH = currentStatus.resp.MULT; // On AM it is the high unsigned char the READANTCAP value
    return cap.value;
  }
};

/**
 * @brief Sets the Am Soft Mute Max Attenuation 
 * @details This function can be useful to disable Soft Mute. The value 0 disable soft mute.
 * @details Specified in units of dB. Default maximum attenuation is 8 dB. It works for AM and SSB.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 158. 
 * @param smattn Maximum attenuation to apply when in soft mute
 */
void si473x_setAmSoftMuteMaxAttenuation(unsigned char smattn) {
  si473x_sendProperty(AM_SOFT_MUTE_MAX_ATTENUATION, smattn);
};

/**
 * @brief Sets the SSB Soft Mute Max Attenuation object
 * @details Sets maximum attenuation during soft mute (dB). Set to 0 to disable soft mute. 
 * @details Specified in units of dB. Default maximum attenuation is 8 dB.
 * @details You can use setAmSoftMuteMaxAttenuation instead. Same AM property values.  
 * @param smattn Maximum attenuation to apply when in soft mute.
 */
void si473x_setSsbSoftMuteMaxAttenuation(unsigned char smattn) {
  si473x_sendProperty(SSB_SOFT_MUTE_MAX_ATTENUATION, smattn);
};

/**
 * @brief Get the current Avc Am Max Gain 
 */
unsigned char si473x_getCurrentAvcAmMaxGain() {
  return si473x_currentAvcAmMaxGain;
};

// AM and FM

/**
 * @brief Get the Current Volume 
 * @details Returns the current volume level.
 */
unsigned char si473x_getCurrentVolume() {
  return si473x_volume;
};

/**
 * @brief Sets the delay before applying impulse blanking 
 * @details Delay in micro-sonds before applying impulse blanking to the original samples. Default value is 172.
 * @param value Delay in micro-seconds
 */
void si473x_setAmDelayNB(unsigned int value) {
  si473x_sendProperty(AM_NB_DELAY, value);
}

/**
 * @brief Sets the FM Receive de-emphasis to 50 or 75us. 
 * @details valid parameters are 1 = 50us. Used in Europe, Australia, Japan; 2 = 75us. Used in USA (default)
 * @param parameter 1 or 2 (default 1 - USA)
 */
void si473x_setFMDeEmphasis(unsigned char parameter) {
  si473x_sendProperty(FM_DEEMPHASIS, parameter);
};

/**
 * @brief Sets the AM Receive de-emphasis to 50 or disable. 
 * @details valid parameters are 1 = 50 ?s. Usedin urope, Australia, Japan; 2 = 75 ?s. Used in USA (default)
 * @param parameter 1 = enable or 0 = disable
 */
void si473x_setAMDeEmphasis(unsigned char parameter) {
  si473x_sendProperty(AM_DEEMPHASIS, parameter);
};

/**
 * @brief Sets the AM attenuation slope during soft mute
 * @details Configures attenuation slope during soft mute in dB attenuation per dB SNR below the soft mute SNR threshold.
 * @details Soft mute attenuation is the minimum of SMSLOPEx(SMTHR?SNR) and SMATTN.
 * @details The default slope is 1 dB/dB for AMRX component 5.0 or later and 2 dB/dB for AMRX component 3.0 or earlier.
 * @see setAmSoftMuteMaxAttenuation
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); 
 * @param parameter  the valid values are 1?5 (default 1).
 */
void si473x_setAMSoftMuteSlop(unsigned char parameter) {
  si473x_sendProperty(AM_SOFT_MUTE_SLOPE, parameter);
};

/**
 * @brief Sets the attack and decay rates when entering or leaving soft mute.
 * @details The value specified is multiplied by 4.35 dB/s to come up with the actual attack rate
 * @details The default rate is 278 dB/s.
 * @see setAmSoftMuteMaxAttenuation
 * @see Si47XX PRORAMMING GUIDE; AN332 (REV 1.0); 
 * @param parameter  The valid values are 1-255  ( Default is ~64 - [64 x 4.35 = 278] )
 */
void si473x_setAMSoftMuteRate(unsigned char parameter) {
  si473x_sendProperty(AM_SOFT_MUTE_RATE, parameter);
};

/**
 * @brief Sets the SNR threshold to engage soft mute
 * @details Whenever the SNR for a tuned frequency drops below this threshold the AM reception will go in soft mute,
 * @details provided soft mute max attenuation property is non-zero. The default value is 8dB
 * @see setAmSoftMuteMxAttenuation
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.0); 
 * @param parameter  0-63 (default is 8)
 */
void si473x_setAMSoftMuteSnrThreshold(unsigned char parameter) {
  si473x_sendProperty(AM_SOFT_MUTE_SNR_THRESHOLD, parameter);
};

/**
 * @brief Sets the soft mute release rate.
 * @details Smaller values provide slower release and larger values provide faster release. The default is 8192 (approximately 8000 dB/s).
 * @see setAmSoftMuteMxAttenuation
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.0); 
 * @param parameter  1?32767 
 */
void si473x_setAMSoftMuteReleaseRate(unsigned char parameter) {
  si473x_sendProperty(AM_SOFT_MUTE_RELEASE_RATE, parameter);
};

/**
 * @brief Sets the soft mute attack rate.
 * @details Smaller values provide slower attack and larger values provide faster attack. 
 * @see setAmSoftMuteMxAttenuation
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.0); 
 * @param parameter  1?32767 (The default is 8192 (approximately 8000 dB/s)
 */
void si473x_setAMSoftMuteAttackRate(unsigned int parameter) {
  si473x_sendProperty(AM_SOFT_MUTE_ATTACK_RATE, parameter);
};

/**
 * @brief Sets the FAST Tuning.  
 * @details If set, excutes fast and invalidated tune. Theune status will not be accurate
 * @param FAST 
 */
void si473x_setTuneFrequencyFast(unsigned char FAST) {
  currentFrequencyParams.arg.FAST = FAST;
};

/**
 * @brief Sets Freeze Metrics During Alternate Frequency Jum.
 * @details Only on FM mode
 * @param FREEZE 
 */
void si473x_setTuneFrequencyFreze(unsigned char FREEZE) {
  currentFrequencyParams.arg.FREEZE = FREEZE;
};

/** 
 * @brief Sets the current step value. 
 * @details This function does not check the limits of the current band. Please, don't take a step bigger than your legs.
 * @details Example:
 * @code
 * setFM(6400,10800,10390,10);
 * setFrequencyStep(100); // the step will be 1MHz (you are using FM mode)
 * .
 * .
 * .
 * setAM(7000,7600,7100,5); 
 * setFrequencyStep(1); // the step will be 1KHz (you are using AM or SSB mode)  
 * @endcode 
 * @see setFM()
 * @see setAM()
 * @see setSSB()
 * @param step if you are using FM, 10 means 100KHz. If you are using AM 10 means 10KHz
 *             For AM, 1 (1KHz) to 1000 (1MHz) are valid values.
 *             For FM 5 (50KHz), 10 (100KHz) and 100 (1MHz) are valid values.  
 */
void si473x_setFrequencyStep(unsigned int step) {
  si473x_currentStep = step;
}

/**
 * @brief Gets the current frequency saved in memory. 
 * @details Unlike getFrequency, this method gets the current frequency recorded after the last setFrequency command. 
 * @details This method avoids bus traffic and CI processing.
 * @details However, you can not get others status information like RSSI.
 * @see getFrequency()
 */
unsigned int si473x_getCurrentFrequency() {
  return si473x_currentWorkFrequency;
}