//-----------------------------------------------------------------------------
//
//  usbDemoDevice.cs
//
//  USB Generic Demonstration 3_0_0_0
//
//  A demonstration application for the usbGenericHidCommunications library
//  Copyright (C) 2011 Simon Inns
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Web:    http://www.waitingforfriday.com
//  Email:  simon.inns@gmail.com
//
//-----------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// The following namespace allows debugging output (when compiled in debug mode)
using System.Diagnostics;

public enum RTCState
{
    RTC_OK,
    RTC_NOTSET,
    RTC_ERROR
}

public enum HCSR04State
{
    MODULE_ERROR = 0,
    OUT_OF_RANGE = 1,
    MODULE_OK = 2
}

namespace WFF_Generic_HID_Demo_3
{
    using usbGenericHidCommunications;

    /// <summary>
    /// This class performs several different tests against the 
    /// reference hardware/firmware to confirm that the USB
    /// communication library is functioning correctly.
    /// 
    /// It also serves as a demonstration of how to use the class
    /// library to perform different types of read and write
    /// operations.
    /// </summary>
    class usbDemoDevice : usbGenericHidCommunication
    {
        /// <summary>
        /// Class constructor - place any initialisation here
        /// </summary>
        /// <param name="vid"></param>
        /// <param name="pid"></param>
        public usbDemoDevice(int vid, int pid)
            : base(vid, pid)
        {
        }

        public bool setRTCTime()
        {
            // Command 0x85 - Set the RTC Current Time

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x85;

            DateTime now = DateTime.Now;

            //year, month, day, dayofweek, hour, minute, and second.
            outputBuffer[2] = (byte)(now.Year % 100);
            outputBuffer[3] = (byte)(now.Month);
            outputBuffer[4] = (byte)(now.Day);
            outputBuffer[5] = (byte)(now.DayOfWeek);
            outputBuffer[6] = (byte)(now.Hour);
            outputBuffer[7] = (byte)(now.Minute);
            outputBuffer[8] = (byte)(now.Second);

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // We can't tell if the device receieved the data ok, we are
            // only indicating that the write was error free.
            return success;
        }

        public RTCState readRTCState(ref DateTime RTCTime)
        {
            // Command 0x84 - Read the DS1307 RTC

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x84;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 0xFF)
            {
                //RTC OK, retrieves the measured value
                try
                {
                    RTCTime = new DateTime(2000 + inputBuffer[2], inputBuffer[3], inputBuffer[4], inputBuffer[5], inputBuffer[6], inputBuffer[7]);
                }
                catch { }

                return RTCState.RTC_OK;
            }
            else if (inputBuffer[1] == 0xF0)
            {
                //RTC OK but time not set
                return RTCState.RTC_NOTSET;
            }
            else
            {
                //RTC Error
                return RTCState.RTC_ERROR;
            }
        }

        public bool readTempState(ref double temperature)
        {
            // Command 0x83 - Read the DS1621 temperature sensor state

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x83;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 0xFF)
            {
                //temperature sensor OK, retrieves the measured value
                temperature = inputBuffer[2] + inputBuffer[3] / 256.0;

                return true;
            }
            else return false;
        }

        public HCSR04State  readDistance(ref int distanceIncm)
        {
            // Command 0x96 - Read the HCSR04 distance finder module

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x96;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            HCSR04State result = (HCSR04State) inputBuffer[1];

            if (result == HCSR04State.MODULE_OK)
            {
                //temperature sensor OK, retrieves the measured value
                distanceIncm = (inputBuffer[2] << 8) + inputBuffer[3];                
            }

            return result;
        }

        public bool readDHT11Values(ref byte temperature, ref byte humidity)
        {
            // Command 0x93 - Read the DHT11 temperature and humidity sensor values

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x93;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 0xFF)
            {
                //DHT11 sensor OK, retrieves the measured values
                temperature = inputBuffer[2];
                humidity = inputBuffer[3];

                return true;
            }
            else return false;
        }

        public byte[] getKeyboardChar()
        {
            // Command 0x92 - get a character from the keyboard buffer. return 0 if buffer is empty

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x92;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] > 0)
            {
                byte[]  retVal;
                retVal = new byte[inputBuffer[1]];

                for (int i=0;i<inputBuffer[1];i++)
                {
                    retVal[i] = inputBuffer[i+2];
                }

                return retVal;
            }

            return null;
        }

        public bool readBuzzerState()
        {
            // Command 0x89 - Read the LED state

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x89;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 1) return true; else return false;
        }

        public bool readMagFieldState()
        {
            // Command 0x95 - Read the Magnetic Field Presence from the hall effect sensor

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x95;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 1) return true; else return false;
        }


        public bool readPIRState()
        {
            // Command 0x97 - Read the PIR sensor status

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x97;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 1) return true; else return false;
        }

        public byte readLM35Value()
        {
            // Command 0x98 - Read the LM35 temperature sensor in C

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x98;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            return inputBuffer[1];
        }

        public bool readCameraState()
        {
            // Command 0x86 - Read the LED state

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x86;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            if (inputBuffer[1] == 1) return true; else return false;
        }

        public bool readCurrentTime()
        {
            // Command 0x94 - pronounce current time

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x94;

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            return success;
        }

        public byte[] getNextCameraFrame(int frameNo)
        {
            // Command 0x87 - get next camera frame, return array of 65 bytes, 2nd byte onwards contain 64 of pixel data

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x87;

            outputBuffer[2] = (byte)(frameNo >> 8); // first 8 bit
            outputBuffer[3] = (byte)(frameNo & 0xFF); // last 8 bit

            // Perform the write command
            bool success;
            success = writeRawReportToDevice(outputBuffer);

            // Only proceed if the write was successful
            if (success)
            {
                // Perform the read
                success = readSingleReportFromDevice(ref inputBuffer);
            }

            return inputBuffer;
        }

        // Collect debug information from the device
        public String collectDebug()
        {
            // Collect debug information from USB device
            Debug.WriteLine("Reference Application -> Collecting debug information from device");

            // Declare our output buffer
            Byte[] outputBuffer = new Byte[65];

            // Declare our input buffer
            Byte[] inputBuffer = new Byte[65];

            // Byte 0 must be set to 0
            outputBuffer[0] = 0;

            // Byte 1 must be set to our command
            outputBuffer[1] = 0x10;

            // Send the collect debug command
            writeRawReportToDevice(outputBuffer);

            // Read the response from the device
            readSingleReportFromDevice(ref inputBuffer);

            // Byte 1 contains the number of characters transfered
            if (inputBuffer[1] == 0) return String.Empty;

            // Convert the Byte array into a string of the correct length
            string s = System.Text.ASCIIEncoding.ASCII.GetString(inputBuffer, 2, inputBuffer[1]);

            return s;
        }
    }
}