namespace WFF_Generic_HID_Demo_3
    {
    partial class Form1
        {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
            {
            if (disposing && (components != null))
                {
                components.Dispose();
                }
            base.Dispose(disposing);
            }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
            {
                this.components = new System.ComponentModel.Container();
                this.statusStrip1 = new System.Windows.Forms.StatusStrip();
                this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
                this.timer1 = new System.Windows.Forms.Timer(this.components);
                this.debugTextBox = new System.Windows.Forms.TextBox();
                this.debugCollectionTimer = new System.Windows.Forms.Timer(this.components);
                this.label3 = new System.Windows.Forms.Label();
                this.DS1621StateLabel = new System.Windows.Forms.Label();
                this.label5 = new System.Windows.Forms.Label();
                this.RTCStateLabel = new System.Windows.Forms.Label();
                this.label6 = new System.Windows.Forms.Label();
                this.setRTCButton = new System.Windows.Forms.Button();
                this.cameraStateLabel = new System.Windows.Forms.Label();
                this.label4 = new System.Windows.Forms.Label();
                this.captureImageButton = new System.Windows.Forms.Button();
                this.pictureBoxCamera = new System.Windows.Forms.PictureBox();
                this.saveImageButton = new System.Windows.Forms.Button();
                this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
                this.clearDebugButton = new System.Windows.Forms.Button();
                this.btnClearKBInput = new System.Windows.Forms.Button();
                this.label7 = new System.Windows.Forms.Label();
                this.txtKbInput = new System.Windows.Forms.TextBox();
                this.labelDHT11Temp = new System.Windows.Forms.Label();
                this.label8 = new System.Windows.Forms.Label();
                this.label9 = new System.Windows.Forms.Label();
                this.labelHumidity = new System.Windows.Forms.Label();
                this.clearImageButton = new System.Windows.Forms.Button();
                this.labelDeviceCurrentAction = new System.Windows.Forms.Label();
                this.label2 = new System.Windows.Forms.Label();
                this.labelMagField = new System.Windows.Forms.Label();
                this.labelDistance = new System.Windows.Forms.Label();
                this.label11 = new System.Windows.Forms.Label();
                this.labelMotionState = new System.Windows.Forms.Label();
                this.label12 = new System.Windows.Forms.Label();
                this.labelBuzzerState = new System.Windows.Forms.Label();
                this.label13 = new System.Windows.Forms.Label();
                this.LM35ValueLabel = new System.Windows.Forms.Label();
                this.label10 = new System.Windows.Forms.Label();
                this.statusStrip1.SuspendLayout();
                ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCamera)).BeginInit();
                this.SuspendLayout();
                // 
                // statusStrip1
                // 
                this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
                this.statusStrip1.Location = new System.Drawing.Point(0, 557);
                this.statusStrip1.Name = "statusStrip1";
                this.statusStrip1.Size = new System.Drawing.Size(394, 22);
                this.statusStrip1.SizingGrip = false;
                this.statusStrip1.TabIndex = 0;
                this.statusStrip1.Text = "statusStrip1";
                // 
                // toolStripStatusLabel1
                // 
                this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
                this.toolStripStatusLabel1.Size = new System.Drawing.Size(116, 17);
                this.toolStripStatusLabel1.Text = "USB Status unknown";
                // 
                // timer1
                // 
                this.timer1.Enabled = true;
                this.timer1.Interval = 50;
                this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
                // 
                // debugTextBox
                // 
                this.debugTextBox.Location = new System.Drawing.Point(12, 451);
                this.debugTextBox.Multiline = true;
                this.debugTextBox.Name = "debugTextBox";
                this.debugTextBox.ReadOnly = true;
                this.debugTextBox.Size = new System.Drawing.Size(346, 97);
                this.debugTextBox.TabIndex = 8;
                // 
                // debugCollectionTimer
                // 
                this.debugCollectionTimer.Enabled = true;
                this.debugCollectionTimer.Interval = 50;
                this.debugCollectionTimer.Tick += new System.EventHandler(this.debugCollectionTimer_Tick);
                // 
                // label3
                // 
                this.label3.AutoSize = true;
                this.label3.Location = new System.Drawing.Point(12, 427);
                this.label3.Name = "label3";
                this.label3.Size = new System.Drawing.Size(75, 13);
                this.label3.TabIndex = 9;
                this.label3.Text = "Debug output:";
                // 
                // DS1621StateLabel
                // 
                this.DS1621StateLabel.AutoSize = true;
                this.DS1621StateLabel.Location = new System.Drawing.Point(258, 46);
                this.DS1621StateLabel.Name = "DS1621StateLabel";
                this.DS1621StateLabel.Size = new System.Drawing.Size(53, 13);
                this.DS1621StateLabel.TabIndex = 11;
                this.DS1621StateLabel.Text = "Unknown";
                // 
                // label5
                // 
                this.label5.Location = new System.Drawing.Point(201, 46);
                this.label5.Name = "label5";
                this.label5.Size = new System.Drawing.Size(52, 13);
                this.label5.TabIndex = 10;
                this.label5.Text = "DS1621:";
                // 
                // RTCStateLabel
                // 
                this.RTCStateLabel.AutoSize = true;
                this.RTCStateLabel.Location = new System.Drawing.Point(259, 61);
                this.RTCStateLabel.Name = "RTCStateLabel";
                this.RTCStateLabel.Size = new System.Drawing.Size(53, 13);
                this.RTCStateLabel.TabIndex = 13;
                this.RTCStateLabel.Text = "Unknown";
                // 
                // label6
                // 
                this.label6.AutoSize = true;
                this.label6.Location = new System.Drawing.Point(201, 61);
                this.label6.Name = "label6";
                this.label6.Size = new System.Drawing.Size(32, 13);
                this.label6.TabIndex = 12;
                this.label6.Text = "RTC:";
                // 
                // setRTCButton
                // 
                this.setRTCButton.Location = new System.Drawing.Point(17, 12);
                this.setRTCButton.Name = "setRTCButton";
                this.setRTCButton.Size = new System.Drawing.Size(75, 23);
                this.setRTCButton.TabIndex = 14;
                this.setRTCButton.Text = "Set RTC";
                this.setRTCButton.UseVisualStyleBackColor = true;
                this.setRTCButton.Click += new System.EventHandler(this.setRTCButton_Click);
                // 
                // cameraStateLabel
                // 
                this.cameraStateLabel.AutoSize = true;
                this.cameraStateLabel.Location = new System.Drawing.Point(115, 62);
                this.cameraStateLabel.Name = "cameraStateLabel";
                this.cameraStateLabel.Size = new System.Drawing.Size(53, 13);
                this.cameraStateLabel.TabIndex = 16;
                this.cameraStateLabel.Text = "Unknown";
                // 
                // label4
                // 
                this.label4.AutoSize = true;
                this.label4.Location = new System.Drawing.Point(60, 62);
                this.label4.Name = "label4";
                this.label4.Size = new System.Drawing.Size(46, 13);
                this.label4.TabIndex = 15;
                this.label4.Text = "Camera:";
                // 
                // captureImageButton
                // 
                this.captureImageButton.Location = new System.Drawing.Point(162, 162);
                this.captureImageButton.Name = "captureImageButton";
                this.captureImageButton.Size = new System.Drawing.Size(104, 23);
                this.captureImageButton.TabIndex = 17;
                this.captureImageButton.Text = "Capture Image";
                this.captureImageButton.UseVisualStyleBackColor = true;
                this.captureImageButton.Click += new System.EventHandler(this.captureImageButton_Click);
                // 
                // pictureBoxCamera
                // 
                this.pictureBoxCamera.Location = new System.Drawing.Point(17, 162);
                this.pictureBoxCamera.Name = "pictureBoxCamera";
                this.pictureBoxCamera.Size = new System.Drawing.Size(128, 128);
                this.pictureBoxCamera.TabIndex = 18;
                this.pictureBoxCamera.TabStop = false;
                // 
                // saveImageButton
                // 
                this.saveImageButton.Location = new System.Drawing.Point(162, 202);
                this.saveImageButton.Name = "saveImageButton";
                this.saveImageButton.Size = new System.Drawing.Size(104, 23);
                this.saveImageButton.TabIndex = 19;
                this.saveImageButton.Text = "Save Image";
                this.saveImageButton.UseVisualStyleBackColor = true;
                this.saveImageButton.Click += new System.EventHandler(this.saveImageButton_Click);
                // 
                // saveFileDialog1
                // 
                this.saveFileDialog1.Filter = "Bitmap files|*.bmp";
                // 
                // clearDebugButton
                // 
                this.clearDebugButton.Location = new System.Drawing.Point(283, 422);
                this.clearDebugButton.Name = "clearDebugButton";
                this.clearDebugButton.Size = new System.Drawing.Size(75, 23);
                this.clearDebugButton.TabIndex = 20;
                this.clearDebugButton.Text = "Clear";
                this.clearDebugButton.UseVisualStyleBackColor = true;
                this.clearDebugButton.Click += new System.EventHandler(this.clearDebugButton_Click);
                // 
                // btnClearKBInput
                // 
                this.btnClearKBInput.Location = new System.Drawing.Point(285, 290);
                this.btnClearKBInput.Name = "btnClearKBInput";
                this.btnClearKBInput.Size = new System.Drawing.Size(75, 23);
                this.btnClearKBInput.TabIndex = 26;
                this.btnClearKBInput.Text = "Clear";
                this.btnClearKBInput.UseVisualStyleBackColor = true;
                this.btnClearKBInput.Click += new System.EventHandler(this.btnClearKBInput_Click);
                // 
                // label7
                // 
                this.label7.AutoSize = true;
                this.label7.Location = new System.Drawing.Point(14, 295);
                this.label7.Name = "label7";
                this.label7.Size = new System.Drawing.Size(81, 13);
                this.label7.TabIndex = 25;
                this.label7.Text = "Keyboard input:";
                // 
                // txtKbInput
                // 
                this.txtKbInput.Location = new System.Drawing.Point(14, 319);
                this.txtKbInput.Multiline = true;
                this.txtKbInput.Name = "txtKbInput";
                this.txtKbInput.ReadOnly = true;
                this.txtKbInput.Size = new System.Drawing.Size(346, 97);
                this.txtKbInput.TabIndex = 24;
                // 
                // labelDHT11Temp
                // 
                this.labelDHT11Temp.AutoSize = true;
                this.labelDHT11Temp.Location = new System.Drawing.Point(115, 78);
                this.labelDHT11Temp.Name = "labelDHT11Temp";
                this.labelDHT11Temp.Size = new System.Drawing.Size(53, 13);
                this.labelDHT11Temp.TabIndex = 28;
                this.labelDHT11Temp.Text = "Unknown";
                // 
                // label8
                // 
                this.label8.Location = new System.Drawing.Point(32, 78);
                this.label8.Name = "label8";
                this.label8.Size = new System.Drawing.Size(78, 13);
                this.label8.TabIndex = 27;
                this.label8.Text = "DHT11 Temp:";
                // 
                // label9
                // 
                this.label9.Location = new System.Drawing.Point(201, 79);
                this.label9.Name = "label9";
                this.label9.Size = new System.Drawing.Size(52, 13);
                this.label9.TabIndex = 29;
                this.label9.Text = "Humidity:";
                // 
                // labelHumidity
                // 
                this.labelHumidity.AutoSize = true;
                this.labelHumidity.Location = new System.Drawing.Point(259, 78);
                this.labelHumidity.Name = "labelHumidity";
                this.labelHumidity.Size = new System.Drawing.Size(53, 13);
                this.labelHumidity.TabIndex = 30;
                this.labelHumidity.Text = "Unknown";
                // 
                // clearImageButton
                // 
                this.clearImageButton.Location = new System.Drawing.Point(162, 248);
                this.clearImageButton.Name = "clearImageButton";
                this.clearImageButton.Size = new System.Drawing.Size(104, 23);
                this.clearImageButton.TabIndex = 31;
                this.clearImageButton.Text = "Clear Image";
                this.clearImageButton.UseVisualStyleBackColor = true;
                this.clearImageButton.Click += new System.EventHandler(this.clearImageButton_Click);
                // 
                // labelDeviceCurrentAction
                // 
                this.labelDeviceCurrentAction.Location = new System.Drawing.Point(17, 132);
                this.labelDeviceCurrentAction.Name = "labelDeviceCurrentAction";
                this.labelDeviceCurrentAction.Size = new System.Drawing.Size(343, 23);
                this.labelDeviceCurrentAction.TabIndex = 32;
                // 
                // label2
                // 
                this.label2.Location = new System.Drawing.Point(48, 97);
                this.label2.Name = "label2";
                this.label2.Size = new System.Drawing.Size(62, 13);
                this.label2.TabIndex = 34;
                this.label2.Text = "Mag. Field:";
                // 
                // labelMagField
                // 
                this.labelMagField.AutoSize = true;
                this.labelMagField.Location = new System.Drawing.Point(115, 98);
                this.labelMagField.Name = "labelMagField";
                this.labelMagField.Size = new System.Drawing.Size(53, 13);
                this.labelMagField.TabIndex = 35;
                this.labelMagField.Text = "Unknown";
                // 
                // labelDistance
                // 
                this.labelDistance.AutoSize = true;
                this.labelDistance.Location = new System.Drawing.Point(260, 96);
                this.labelDistance.Name = "labelDistance";
                this.labelDistance.Size = new System.Drawing.Size(53, 13);
                this.labelDistance.TabIndex = 37;
                this.labelDistance.Text = "Unknown";
                // 
                // label11
                // 
                this.label11.Location = new System.Drawing.Point(202, 97);
                this.label11.Name = "label11";
                this.label11.Size = new System.Drawing.Size(52, 13);
                this.label11.TabIndex = 36;
                this.label11.Text = "Distance:";
                // 
                // labelMotionState
                // 
                this.labelMotionState.AutoSize = true;
                this.labelMotionState.Location = new System.Drawing.Point(115, 116);
                this.labelMotionState.Name = "labelMotionState";
                this.labelMotionState.Size = new System.Drawing.Size(53, 13);
                this.labelMotionState.TabIndex = 39;
                this.labelMotionState.Text = "Unknown";
                // 
                // label12
                // 
                this.label12.Location = new System.Drawing.Point(65, 115);
                this.label12.Name = "label12";
                this.label12.Size = new System.Drawing.Size(47, 13);
                this.label12.TabIndex = 38;
                this.label12.Text = "Motion:";
                // 
                // labelBuzzerState
                // 
                this.labelBuzzerState.AutoSize = true;
                this.labelBuzzerState.Location = new System.Drawing.Point(260, 114);
                this.labelBuzzerState.Name = "labelBuzzerState";
                this.labelBuzzerState.Size = new System.Drawing.Size(53, 13);
                this.labelBuzzerState.TabIndex = 41;
                this.labelBuzzerState.Text = "Unknown";
                // 
                // label13
                // 
                this.label13.Location = new System.Drawing.Point(202, 115);
                this.label13.Name = "label13";
                this.label13.Size = new System.Drawing.Size(52, 13);
                this.label13.TabIndex = 40;
                this.label13.Text = "Buzzer:";
                // 
                // LM35ValueLabel
                // 
                this.LM35ValueLabel.AutoSize = true;
                this.LM35ValueLabel.Location = new System.Drawing.Point(114, 46);
                this.LM35ValueLabel.Name = "LM35ValueLabel";
                this.LM35ValueLabel.Size = new System.Drawing.Size(53, 13);
                this.LM35ValueLabel.TabIndex = 43;
                this.LM35ValueLabel.Text = "Unknown";
                // 
                // label10
                // 
                this.label10.Location = new System.Drawing.Point(68, 46);
                this.label10.Name = "label10";
                this.label10.Size = new System.Drawing.Size(38, 13);
                this.label10.TabIndex = 42;
                this.label10.Text = "LM35:";
                // 
                // Form1
                // 
                this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
                this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
                this.ClientSize = new System.Drawing.Size(394, 579);
                this.Controls.Add(this.LM35ValueLabel);
                this.Controls.Add(this.label10);
                this.Controls.Add(this.labelBuzzerState);
                this.Controls.Add(this.label13);
                this.Controls.Add(this.labelMotionState);
                this.Controls.Add(this.label12);
                this.Controls.Add(this.labelDistance);
                this.Controls.Add(this.label11);
                this.Controls.Add(this.labelMagField);
                this.Controls.Add(this.label2);
                this.Controls.Add(this.labelDeviceCurrentAction);
                this.Controls.Add(this.clearImageButton);
                this.Controls.Add(this.labelHumidity);
                this.Controls.Add(this.label9);
                this.Controls.Add(this.labelDHT11Temp);
                this.Controls.Add(this.label8);
                this.Controls.Add(this.btnClearKBInput);
                this.Controls.Add(this.label7);
                this.Controls.Add(this.txtKbInput);
                this.Controls.Add(this.clearDebugButton);
                this.Controls.Add(this.saveImageButton);
                this.Controls.Add(this.pictureBoxCamera);
                this.Controls.Add(this.captureImageButton);
                this.Controls.Add(this.cameraStateLabel);
                this.Controls.Add(this.label4);
                this.Controls.Add(this.setRTCButton);
                this.Controls.Add(this.RTCStateLabel);
                this.Controls.Add(this.label6);
                this.Controls.Add(this.DS1621StateLabel);
                this.Controls.Add(this.label5);
                this.Controls.Add(this.label3);
                this.Controls.Add(this.debugTextBox);
                this.Controls.Add(this.statusStrip1);
                this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
                this.MaximizeBox = false;
                this.MinimizeBox = false;
                this.Name = "Form1";
                this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
                this.Text = "WFF Generic HID Demo 3";
                this.statusStrip1.ResumeLayout(false);
                this.statusStrip1.PerformLayout();
                ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCamera)).EndInit();
                this.ResumeLayout(false);
                this.PerformLayout();

            }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.TextBox debugTextBox;
        private System.Windows.Forms.Timer debugCollectionTimer;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label DS1621StateLabel;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label RTCStateLabel;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button setRTCButton;
        private System.Windows.Forms.Label cameraStateLabel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button captureImageButton;
        private System.Windows.Forms.PictureBox pictureBoxCamera;
        private System.Windows.Forms.Button saveImageButton;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Button clearDebugButton;
        private System.Windows.Forms.Button btnClearKBInput;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtKbInput;
        private System.Windows.Forms.Label labelDHT11Temp;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label labelHumidity;
        private System.Windows.Forms.Button clearImageButton;
        private System.Windows.Forms.Label labelDeviceCurrentAction;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label labelMagField;
        private System.Windows.Forms.Label labelDistance;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label labelMotionState;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label labelBuzzerState;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label LM35ValueLabel;
        private System.Windows.Forms.Label label10;
        }
    }

