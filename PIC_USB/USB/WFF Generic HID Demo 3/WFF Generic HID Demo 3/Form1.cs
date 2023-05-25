//-----------------------------------------------------------------------------
//
//  Form1.cs
//
//  USB Generic HID Demonstration 3_0_0_0
//
//  A reference test application for the usbGenericHidCommunications library
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

//
//  Current reference class library version:
//  usbGenericHidCommunications class library version 3.0.0.0
//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WFF_Generic_HID_Demo_3
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            // Create the USB reference device object (passing VID and PID)
            theUsbDemoDevice = new usbDemoDevice(0x04D8, 0x0042);

            // Add a listener for usb events
            theUsbDemoDevice.usbEvent += new usbDemoDevice.usbEventsHandler(usbEvent_receiver);

            // Perform an initial search for the target device
            theUsbDemoDevice.findTargetDevice();
        }

        // Create an instance of the USB reference device
        private usbDemoDevice theUsbDemoDevice;

        // Listener for USB events
        private void usbEvent_receiver(object o, EventArgs e)
        {
            // Check the status of the USB device and update the form accordingly
            if (theUsbDemoDevice.isDeviceAttached)
            {
                // Device is currently attached

                // Update the status label
                this.toolStripStatusLabel1.Text = "USB Device is attached";

                // Update the form
                this.setRTCButton.Enabled = true;
                this.captureImageButton.Enabled = true;
            }
            else
            {
                // Device is currently unattached

                // Update the status label
                this.toolStripStatusLabel1.Text = "USB Device is detached";

                // Update the form
                this.captureImageButton.Enabled = false;
                this.setRTCButton.Enabled = false;
            }
        }

        // Timer 1 has ticked, poll the USB device for status
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (theUsbDemoDevice.isDeviceAttached)
            {
                // read the RTC state
                DateTime rtcTime = DateTime.MinValue;
                RTCState rtcState = theUsbDemoDevice.readRTCState(ref rtcTime);

                if (rtcState == RTCState.RTC_OK) this.RTCStateLabel.Text = rtcTime.ToString();
                else this.RTCStateLabel.Text = rtcState.ToString();
                this.setRTCButton.Enabled = (rtcState != RTCState.RTC_ERROR);

                // read the temperature sensor state
                double temp = 0;
                bool tempState = theUsbDemoDevice.readTempState(ref temp);
                if (tempState == true) this.DS1621StateLabel.Text = temp.ToString() + "C";
                else this.DS1621StateLabel.Text = "Error";

                // read the buzzer state
                bool buzzerState = theUsbDemoDevice.readBuzzerState();
                if (buzzerState) this.labelBuzzerState.Text = "On";
                else this.labelBuzzerState.Text = "Off";

                // Read the camera state
                bool camState = theUsbDemoDevice.readCameraState();
                if (camState == true) this.cameraStateLabel.Text = "OK";
                else this.cameraStateLabel.Text = "Error";
                this.captureImageButton.Enabled = camState;

                // read characters from the keyboard buffer
                byte[] kbchar = theUsbDemoDevice.getKeyboardChar();
                if (kbchar != null)
                {
                    for (int i = 0; i < kbchar.Length; i++)
                    {
                        this.txtKbInput.Text += Convert.ToChar(kbchar[i]);
                    }
                }
                this.txtKbInput.SelectionStart = this.txtKbInput.Text.Length;

                // read the DHT11 temp. sensor values
                byte dht11temp = 0xFF, dht11humid = 0xFF;
                bool isDHT11OK = theUsbDemoDevice.readDHT11Values(ref dht11temp, ref dht11humid);
                if (isDHT11OK)
                {
                    this.labelDHT11Temp.Text = dht11temp.ToString() + "C";
                    this.labelHumidity.Text = dht11humid.ToString() + "%";
                }
                else
                {
                    this.labelDHT11Temp.Text = "Error";
                    this.labelHumidity.Text = "Error";
                }

                // read the LM35 temperature value
                byte lm35temp = theUsbDemoDevice.readLM35Value();
                this.LM35ValueLabel.Text = lm35temp.ToString() + "C";

                // hall effect sensor output
                bool magState = theUsbDemoDevice.readMagFieldState();
                if (magState) this.labelMagField.Text = "Present";
                else this.labelMagField.Text = "Not Found";

                // PIR sensor output
                bool PIRState = theUsbDemoDevice.readPIRState();
                if (PIRState) this.labelMotionState.Text = "Detected";
                else this.labelMotionState.Text = "Not Found";

                // distance finder module output
                int distance = 0;
                HCSR04State dstState = theUsbDemoDevice.readDistance(ref distance);
                if (dstState == HCSR04State.MODULE_OK) this.labelDistance.Text = distance.ToString() + " cm";
                else this.labelDistance.Text = dstState.ToString();
            }
        }

        // Collect debug timer has ticked
        private void debugCollectionTimer_Tick(object sender, EventArgs e)
        {
            String debugString;

            // Only collect debug if the device is attached
            if (theUsbDemoDevice.isDeviceAttached)
            {
                // Collect the debug information from the device
                debugString = theUsbDemoDevice.collectDebug();

                // Display the debug information
                if (debugString != String.Empty)
                {
                    this.debugTextBox.AppendText(debugString);
                }
            }
            else
            {
                // Clear the debug window
                this.debugTextBox.Clear();
            }
        }

        private void setRTCButton_Click(object sender, EventArgs e)
        {
            if (theUsbDemoDevice.isDeviceAttached)
            {
                theUsbDemoDevice.setRTCTime();
            }
        }

        private void captureImageButton_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            Bitmap bmp = new Bitmap(128, 128);

            for (int packet=0;packet<512;packet++)
            {                           
                if (packet % 128 == 0 || packet == 511)
                {
                    int pctDone = (packet * 100 / 511);
                    this.labelDeviceCurrentAction.Text = String.Format("Capturing image data ({0}%)", pctDone);
                    this.Refresh();
                }

                // 65 bytes with first byte ignored and 32 byte of image data
                // each pixel consume 2 bytes
                byte[] cameraData = theUsbDemoDevice.getNextCameraFrame(packet);

                for (int i = 0; i < 32; i++)
                {
                    int pixelInd = packet * 32 + i;
                    int col = pixelInd % 128;
                    int row = pixelInd / 128;

                    // extract the pixel data
                    int pixelData = ( (cameraData[i * 2 + 1] << 8) + cameraData[i * 2 + 2]) / 4;

                    Color thisColor = Color.FromArgb(pixelData, pixelData, pixelData);
                    bmp.SetPixel(col, row, thisColor);
                }
            }            

            this.pictureBoxCamera.Image = bmp;

            this.labelDeviceCurrentAction.Text = "";

            Cursor.Current = Cursors.Default;
        }

        private void saveImageButton_Click(object sender, EventArgs e)
        {
            Image bmp = this.pictureBoxCamera.Image;
            string fn = "IMG" + (DateTime.Now.Ticks % 100000).ToString() + ".bmp";
            this.saveFileDialog1.FileName = fn;
            if (this.saveFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                bmp.Save(this.saveFileDialog1.FileName, System.Drawing.Imaging.ImageFormat.Bmp);
            }
        }

        private void clearDebugButton_Click(object sender, EventArgs e)
        {
            this.debugTextBox.Text = "";
        }

        private void btnClearKBInput_Click(object sender, EventArgs e)
        {
            this.txtKbInput.Text = "";
        }

        private void clearImageButton_Click(object sender, EventArgs e)
        {
            this.pictureBoxCamera.Image = new Bitmap(128, 128);
        }

        private void readtimeButton_Click(object sender, EventArgs e)
        {
            bool isok = this.theUsbDemoDevice.readCurrentTime();
            if (isok)
            {
                this.labelDeviceCurrentAction.Text = "Read Time Done";
            }
            else
            {
                this.labelDeviceCurrentAction.Text = "Read Time Failed.";
            }
        }

        private void buttonPronounceTime_Click(object sender, EventArgs e)
        {
            theUsbDemoDevice.readCurrentTime();
        }
    }
}
