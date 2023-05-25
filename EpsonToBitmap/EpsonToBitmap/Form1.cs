using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace EpsonToBitmap
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            if (this.openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                this.txtInputFileName.Text = this.openFileDialog1.FileName;
        }

        private void ConvertEpson(string inputFile, string outputfile)
        {
            byte[] inputData = File.ReadAllBytes(inputFile);

            Bitmap outputBmp = new Bitmap(450, 250);

            // background to Black unless Optimized for Printing is checked 
            if (!this.chkOptimizePrinting.Checked)
            {
                for (int x = 0; x < outputBmp.Width; x++)
                    for (int y = 0; y < outputBmp.Height; y++)
                        outputBmp.SetPixel(x, y, Color.FromArgb(0, 0, 0));
            }

            int currentX = 0;
            int currentY = 0;

            // check for escape codes
            int i = 0;
            while (i < inputData.Length)
            {
                byte thisByte = inputData[i];
                if (thisByte == 27)
                {
                    i++;
                    byte escCommand = inputData[i];

                    /*
                     Recognized escape code from a sample output file
                      
                    27 65 (ESC A) - Set n/60-inch line spacing for ESC/P or n/72-inch line spacing for 9-pin ESC/P
                    27 108 (ESC l) - Set left margin
                    27 42 (ESC *) - Select bit image
                    27 50 - Select 1/6-inch line spacing
                      
                     Other escape code in use but not in docs
                      
                     27 3 3 - Not in Doc 
                     27 - Not in Doc
                     27 10 - Not in Doc                       
                     */

                    Debug.WriteLine(String.Format("Byte #{0} - Escape code found: {1} {2}", i, thisByte, escCommand));

                    if (escCommand == 42) // we only interested in ESC * - select bit image
                    {
                        /*
Format of ESC * Select bit image - extracted from manual Page C-177 (185 out of 231)
ASCII ESC * m nL nH d1 . . . dk
                          
Range:                         
0 <= nL <= 255
0 <= nH <= 31
m = 0, 1, 2, 3, 4, 6, 32, 33, 38, 39, 40, 71, 72, 73
                          
Function:                         
Prints dot-graphics in 8, 24, or 48-dot columns, depending on the following parameters:
m Specifies the dot density (see table below)
nL, nH Specifies the total number of columns of graphics data that follow:
                          
(number of dot columns) = ((nH * 256) + nL) where
nH= number of dot columns / 256
nL= number of dot columns MOD 256
                         * 
d1. . . dk Bytes of graphics data; k is determined by multiplying the total number of columns times the number of bytes required for each column (see the table below)
                          
                         */

                        i++;
                        byte dotDensity = inputData[i];  // m

                        i++;
                        byte imageLength = inputData[i]; // nL

                        i++;
                        byte imageHeight = inputData[i]; // nH

                        int expectedBytes = imageHeight * 256 + imageLength;

                        Debug.WriteLine(String.Format("Density {0} nH {1} nL {2} Expected bytes {3}", dotDensity, imageHeight, imageLength, expectedBytes));

                        // For simplicity, assuming density 4 (80 dots horizontal, 72 dots vertical, Adjacent printing, 8 dots per column, 1 bytes per column)
                        // Refer to Page 186 for details
                        int readIndex = 0;
                        while (readIndex < expectedBytes)
                        {
                            i++;
                            byte line = inputData[i];

                            for (int j = 0; j < 8; j++)
                            {
                                if ((line & 0x1) > 0)
                                {
                                    // green or black pixel, depending on whether Optimize for Printing is checked
                                    outputBmp.SetPixel(currentX, currentY + 8 - j, this.chkOptimizePrinting.Checked ? Color.FromArgb(0, 0, 0) : Color.FromArgb(0, 255, 0));
                                }

                                line >>= 1;
                            }

                            currentX += 1;
                            readIndex += 1;
                        }
                    }
                }

                // processing new line character (\r\n, \n\r or \n or \r alone)
                int lineSpacing = 8;
                if (thisByte == 10) // new line char
                {
                    currentY += lineSpacing; // line spacing
                    currentX = 0;

                    Debug.WriteLine(String.Format("Byte #{0} - New line", i));

                    // ignore next byte if it's also new line
                    byte nextByte = inputData[i+1];
                    if (nextByte == 13) i += 1;
                }

                if (thisByte == 13) // new line char
                {
                    currentY += lineSpacing;
                    currentX = 0;

                    Debug.WriteLine(String.Format("Byte #{0} - New line", i));

                    // ignore next byte if it's also new line
                    byte nextByte = inputData[i + 1];
                    if (nextByte == 10) i += 1;
                }

                i++;
            }

            outputBmp.Save(outputfile);
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            if (!File.Exists(this.txtInputFileName.Text))
                MessageBox.Show("Unable to open input file");
            else
            {
                this.saveFileDialog1.FileName = Path.GetFileNameWithoutExtension(this.txtInputFileName.Text) + ".bmp";
                if (this.saveFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    try
                    {
                        Debug.WriteLine("Converting " + this.txtInputFileName.Text + " to " + this.saveFileDialog1.FileName);
                            
                        Cursor.Current = Cursors.WaitCursor;
                        ConvertEpson(this.txtInputFileName.Text, this.saveFileDialog1.FileName);
                        Cursor.Current = Cursors.Default;

                        Debug.WriteLine("Converting done.");

                        MessageBox.Show("Image saved to: " + this.saveFileDialog1.FileName);
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine(ex.Message);

                        Cursor.Current = Cursors.Default;
                        MessageBox.Show(ex.Message);
                    }
                }
            }
        }
    }
}
