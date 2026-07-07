EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "dsPIC LW/MW/FM/SW/SSB Radio and Media Player/Recorder"
Date "2021-03-15"
Rev "1.2.1"
Comp "ToughDev"
Comment1 "Bluetooth playback via external module through TDA1308 mixer/amplifier"
Comment2 "Supports audio recording into WAV files "
Comment3 "Uses dsPIC DSP module for MP3 playback and SI4735 for radio"
Comment4 "SD card and USB media player using CH376 & FATFS"
$EndDescr
$Comp
L DSP_Microchip_DSPIC33:DSPIC33FJ64MC802-xSP U3
U 1 1 5F98EEF7
P 3700 3300
F 0 "U3" H 2350 4500 50  0000 C CNN
F 1 "dsPIC33EP512MC502" H 4700 4500 50  0000 C CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket" H 3700 3300 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/70291G.pdf" H 3700 3300 50  0001 C CNN
	1    3700 3300
	1    0    0    -1  
$EndComp
$Comp
L symbols_STRN_Lab:LT1763-S8 U1
U 1 1 5F991B1A
P 1700 1200
F 0 "U1" H 1700 1617 50  0000 C CNN
F 1 "LT1763-3.3" H 1700 1526 50  0000 C CNN
F 2 "LT1763IS8-5PBF:SO-8_S" H 1700 400 50  0001 C CNN
F 3 "" H 2500 1350 50  0001 C CNN
	1    1700 1200
	1    0    0    -1  
$EndComp
$Comp
L symbols_STRN_Lab:LT1763-S8 U5
U 1 1 5F993C64
P 5300 1250
F 0 "U5" H 5300 1667 50  0000 C CNN
F 1 "LT1763-3.3" H 5300 1576 50  0000 C CNN
F 2 "LT1763IS8-5PBF:SO-8_S" H 5300 450 50  0001 C CNN
F 3 "" H 6100 1400 50  0001 C CNN
	1    5300 1250
	1    0    0    -1  
$EndComp
Connection ~ 1700 1550
Wire Wire Line
	1700 1550 1800 1550
Wire Wire Line
	2200 1050 2200 1150
$Comp
L Device:C C6
U 1 1 5F9A1590
P 2350 1300
F 0 "C6" H 2465 1346 50  0000 L CNN
F 1 "10nF" H 2465 1255 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 2388 1150 50  0001 C CNN
F 3 "~" H 2350 1300 50  0001 C CNN
	1    2350 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 1150 2350 1150
Connection ~ 2200 1150
Wire Wire Line
	2200 1350 2200 1450
Wire Wire Line
	2200 1450 2350 1450
Wire Wire Line
	3600 1050 3600 2000
$Comp
L Device:C C8
U 1 1 5F9A581E
P 4200 1200
F 0 "C8" H 4315 1246 50  0000 L CNN
F 1 "1uF" H 4315 1155 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 4238 1050 50  0001 C CNN
F 3 "~" H 4200 1200 50  0001 C CNN
	1    4200 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5F9A60E0
P 4200 1350
F 0 "#PWR015" H 4200 1100 50  0001 C CNN
F 1 "GND" H 4205 1177 50  0000 C CNN
F 2 "" H 4200 1350 50  0001 C CNN
F 3 "" H 4200 1350 50  0001 C CNN
	1    4200 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 5F9A8560
P 2100 2200
F 0 "R10" V 2200 2150 50  0000 L CNN
F 1 "47k" V 2100 2150 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2030 2200 50  0001 C CNN
F 3 "~" H 2100 2200 50  0001 C CNN
	1    2100 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 2000 3600 2000
Connection ~ 3600 2000
Wire Wire Line
	3600 1050 3800 1050
$Comp
L Device:R R18
U 1 1 5F9A9A08
P 3800 1550
F 0 "R18" H 3870 1596 50  0000 L CNN
F 1 "10" H 3870 1505 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 3730 1550 50  0001 C CNN
F 3 "~" H 3800 1550 50  0001 C CNN
	1    3800 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 1400 3800 1050
Connection ~ 3800 1050
Wire Wire Line
	3800 1050 4200 1050
$Comp
L power:GND #PWR07
U 1 1 5F9AC660
P 1600 3300
F 0 "#PWR07" H 1600 3050 50  0001 C CNN
F 1 "GND" H 1605 3127 50  0000 C CNN
F 2 "" H 1600 3300 50  0001 C CNN
F 3 "" H 1600 3300 50  0001 C CNN
	1    1600 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 3000 2100 3000
$Comp
L power:GND #PWR011
U 1 1 5F9B0AB6
P 2950 1700
F 0 "#PWR011" H 2950 1450 50  0001 C CNN
F 1 "GND" H 2955 1527 50  0000 C CNN
F 2 "" H 2950 1700 50  0001 C CNN
F 3 "" H 2950 1700 50  0001 C CNN
	1    2950 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5F9B559A
P 800 3750
F 0 "R1" H 870 3796 50  0000 L CNN
F 1 "200K" H 870 3705 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 730 3750 50  0001 C CNN
F 3 "~" H 800 3750 50  0001 C CNN
	1    800  3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  4000 800  3900
$Comp
L Device:R R2
U 1 1 5F9B66E5
P 800 4150
F 0 "R2" H 870 4196 50  0000 L CNN
F 1 "68K" H 870 4105 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 730 4150 50  0001 C CNN
F 3 "~" H 800 4150 50  0001 C CNN
	1    800  4150
	1    0    0    -1  
$EndComp
Connection ~ 800  4000
$Comp
L power:GND #PWR04
U 1 1 5F9B6F16
P 800 4350
F 0 "#PWR04" H 800 4100 50  0001 C CNN
F 1 "GND" H 805 4177 50  0000 C CNN
F 2 "" H 800 4350 50  0001 C CNN
F 3 "" H 800 4350 50  0001 C CNN
	1    800  4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  4300 800  4350
Text Label 1650 4100 2    50   ~ 0
T1CK
Wire Wire Line
	1550 4000 1550 3800
Wire Wire Line
	1550 3800 2100 3800
Text Label 2950 800  0    50   ~ 0
3V3PIC
Connection ~ 2200 1050
$Comp
L Device:R R3
U 1 1 5FA0F37E
P 1100 5850
F 0 "R3" H 1170 5896 50  0000 L CNN
F 1 "4k7" H 1170 5805 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1030 5850 50  0001 C CNN
F 3 "~" H 1100 5850 50  0001 C CNN
	1    1100 5850
	0    1    1    0   
$EndComp
Text Label 600  5850 0    50   ~ 0
5VUSB
$Comp
L Device:R R15
U 1 1 5FA2491E
P 3000 6750
F 0 "R15" V 2793 6750 50  0000 C CNN
F 1 "100k" V 2884 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2930 6750 50  0001 C CNN
F 3 "~" H 3000 6750 50  0001 C CNN
	1    3000 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 5FA4A513
P 1500 6750
F 0 "R6" H 1570 6796 50  0000 L CNN
F 1 "100k" H 1570 6705 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1430 6750 50  0001 C CNN
F 3 "~" H 1500 6750 50  0001 C CNN
	1    1500 6750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R17
U 1 1 5FA761F2
P 3600 6750
F 0 "R17" V 3393 6750 50  0000 C CNN
F 1 "100k" V 3484 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 3530 6750 50  0001 C CNN
F 3 "~" H 3600 6750 50  0001 C CNN
	1    3600 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R19
U 1 1 5FA788A9
P 3900 6750
F 0 "R19" V 3693 6750 50  0000 C CNN
F 1 "100k" V 3784 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 3830 6750 50  0001 C CNN
F 3 "~" H 3900 6750 50  0001 C CNN
	1    3900 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R20
U 1 1 5FA7B356
P 4200 6750
F 0 "R20" H 4270 6796 50  0000 L CNN
F 1 "100k" H 4270 6705 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 4130 6750 50  0001 C CNN
F 3 "~" H 4200 6750 50  0001 C CNN
	1    4200 6750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R23
U 1 1 5FA7DE94
P 4500 6750
F 0 "R23" H 4570 6796 50  0000 L CNN
F 1 "100k" H 4570 6705 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 4430 6750 50  0001 C CNN
F 3 "~" H 4500 6750 50  0001 C CNN
	1    4500 6750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 5700 1550 6200
$Comp
L power:GND #PWR06
U 1 1 5F9F62F5
P 1550 6200
F 0 "#PWR06" H 1550 5950 50  0001 C CNN
F 1 "GND" H 1555 6027 50  0000 C CNN
F 2 "" H 1550 6200 50  0001 C CNN
F 3 "" H 1550 6200 50  0001 C CNN
	1    1550 6200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5FA1652E
P 1800 6750
F 0 "R8" V 1593 6750 50  0000 C CNN
F 1 "100k" V 1684 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1730 6750 50  0001 C CNN
F 3 "~" H 1800 6750 50  0001 C CNN
	1    1800 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R16
U 1 1 5FAEE31A
P 3300 6750
F 0 "R16" V 3093 6750 50  0000 C CNN
F 1 "100k" V 3184 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 3230 6750 50  0001 C CNN
F 3 "~" H 3300 6750 50  0001 C CNN
	1    3300 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R14
U 1 1 5FA202D3
P 2700 6750
F 0 "R14" V 2493 6750 50  0000 C CNN
F 1 "100k" V 2584 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2630 6750 50  0001 C CNN
F 3 "~" H 2700 6750 50  0001 C CNN
	1    2700 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R12
U 1 1 5FA1CEE2
P 2400 6750
F 0 "R12" V 2193 6750 50  0000 C CNN
F 1 "100k" V 2284 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2330 6750 50  0001 C CNN
F 3 "~" H 2400 6750 50  0001 C CNN
	1    2400 6750
	0    1    1    0   
$EndComp
Wire Wire Line
	1650 5700 1650 6750
Wire Wire Line
	1950 6500 1950 6750
$Comp
L Device:R R11
U 1 1 5FA1B086
P 2100 6750
F 0 "R11" V 1893 6750 50  0000 C CNN
F 1 "100k" V 1984 6750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2030 6750 50  0001 C CNN
F 3 "~" H 2100 6750 50  0001 C CNN
	1    2100 6750
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 6500 1750 6500
Wire Wire Line
	1750 6500 1750 5700
Connection ~ 1950 6750
Wire Wire Line
	1850 5700 1850 6400
Wire Wire Line
	1850 6400 2250 6400
Wire Wire Line
	2250 6400 2250 6750
Connection ~ 2250 6750
Wire Wire Line
	1950 5700 1950 6250
Wire Wire Line
	2550 6250 2550 6750
Wire Wire Line
	1950 6250 2550 6250
Connection ~ 2550 6750
Wire Wire Line
	2050 5700 2050 6150
Wire Wire Line
	2050 6150 2850 6150
Wire Wire Line
	2850 6150 2850 6750
Connection ~ 2850 6750
Wire Wire Line
	2150 5700 2150 6050
Wire Wire Line
	2150 6050 3150 6050
Wire Wire Line
	3150 6050 3150 6750
Connection ~ 3150 6750
Wire Wire Line
	3450 5700 3450 6750
Wire Wire Line
	2250 5700 2950 5700
$Comp
L power:GND #PWR012
U 1 1 5FBD87CF
P 2950 5800
F 0 "#PWR012" H 2950 5550 50  0001 C CNN
F 1 "GND" H 2955 5627 50  0000 C CNN
F 2 "" H 2950 5800 50  0001 C CNN
F 3 "" H 2950 5800 50  0001 C CNN
	1    2950 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5800 2950 5700
Connection ~ 2950 5700
Wire Wire Line
	2950 5700 3350 5700
Connection ~ 3450 6750
Wire Wire Line
	3550 5700 3550 6500
Wire Wire Line
	3550 6500 3750 6500
Wire Wire Line
	3750 6500 3750 6750
Connection ~ 3750 6750
Wire Wire Line
	3650 6400 4050 6400
Wire Wire Line
	4050 6400 4050 6750
Connection ~ 4050 6750
Wire Wire Line
	3650 6400 3650 5700
Wire Wire Line
	4350 6750 4350 6300
Wire Wire Line
	4350 6300 3750 6300
Wire Wire Line
	3750 6300 3750 5700
Connection ~ 4350 6750
Wire Wire Line
	4650 6750 4650 6200
Wire Wire Line
	4650 6200 3850 6200
Wire Wire Line
	3850 6200 3850 5700
Connection ~ 1650 6750
Text Label 900  6750 0    50   ~ 0
3V3PIC
Wire Wire Line
	900  6750 1350 6750
Text Label 1650 7050 0    50   ~ 0
KB_ADC_INP
Wire Wire Line
	1650 6750 1650 7050
$Comp
L Diode:1N4148 D1
U 1 1 5FC12951
P 1350 4000
F 0 "D1" H 1350 3783 50  0000 C CNN
F 1 "1N4148" H 1350 3874 50  0000 C CNN
F 2 "Diodes_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 1350 3825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 1350 4000 50  0001 C CNN
	1    1350 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	800  4000 1200 4000
Wire Wire Line
	1500 4000 1550 4000
$Comp
L power:VCC #PWR017
U 1 1 5FC1DB73
P 4700 950
F 0 "#PWR017" H 4700 800 50  0001 C CNN
F 1 "VCC" H 4715 1123 50  0000 C CNN
F 2 "" H 4700 950 50  0001 C CNN
F 3 "" H 4700 950 50  0001 C CNN
	1    4700 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 1100 4700 1100
Wire Wire Line
	850  1050 1050 1050
Wire Wire Line
	850  1550 1600 1550
Connection ~ 1600 1550
Wire Wire Line
	1600 1550 1700 1550
$Comp
L power:GND #PWR018
U 1 1 5FC4C3FD
P 4700 1750
F 0 "#PWR018" H 4700 1500 50  0001 C CNN
F 1 "GND" H 4705 1577 50  0000 C CNN
F 2 "" H 4700 1750 50  0001 C CNN
F 3 "" H 4700 1750 50  0001 C CNN
	1    4700 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 1600 5400 1600
Wire Wire Line
	5200 1600 5300 1600
Connection ~ 5300 1600
$Comp
L Device:C C11
U 1 1 5FC7581C
P 6050 1350
F 0 "C11" H 6165 1396 50  0000 L CNN
F 1 "10nF" H 6165 1305 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 6088 1200 50  0001 C CNN
F 3 "~" H 6050 1350 50  0001 C CNN
	1    6050 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 1200 6050 1200
Wire Wire Line
	5800 1400 5800 1500
Wire Wire Line
	5800 1500 6050 1500
Wire Wire Line
	5800 1200 5800 1100
Connection ~ 5800 1200
$Comp
L Device:CP C14
U 1 1 5FC8262D
P 6500 1450
F 0 "C14" H 6550 1250 50  0000 L CNN
F 1 "47uF" H 6300 1250 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 6538 1300 50  0001 C CNN
F 3 "~" H 6500 1450 50  0001 C CNN
	1    6500 1450
	1    0    0    -1  
$EndComp
Text Label 6500 800  0    50   ~ 0
3V3SD
Wire Wire Line
	2200 1050 2950 1050
$Comp
L Device:CP C7
U 1 1 5FC90280
P 2950 1300
F 0 "C7" H 3068 1346 50  0000 L CNN
F 1 "47uF" H 3068 1255 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 2988 1150 50  0001 C CNN
F 3 "~" H 2950 1300 50  0001 C CNN
	1    2950 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1700 2950 1550
Wire Wire Line
	2950 1150 2950 1050
Connection ~ 2950 1050
Wire Wire Line
	2950 1050 2950 800 
$Comp
L power:GND #PWR023
U 1 1 5FCB71A0
P 6500 1750
F 0 "#PWR023" H 6500 1500 50  0001 C CNN
F 1 "GND" H 6505 1577 50  0000 C CNN
F 2 "" H 6500 1750 50  0001 C CNN
F 3 "" H 6500 1750 50  0001 C CNN
	1    6500 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 1750 6500 1600
Text Label 2500 7350 0    50   ~ 0
3V3PIC
Text Label 2650 8450 0    50   ~ 0
T1CK
$Comp
L power:GND #PWR05
U 1 1 5F9A9EB9
P 800 8750
F 0 "#PWR05" H 800 8500 50  0001 C CNN
F 1 "GND" H 805 8577 50  0000 C CNN
F 2 "" H 800 8750 50  0001 C CNN
F 3 "" H 800 8750 50  0001 C CNN
	1    800  8750
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  8450 1100 8450
$Comp
L Device:C C1
U 1 1 5F9A7E44
P 800 8600
F 0 "C1" H 915 8646 50  0000 L CNN
F 1 "47pF" H 915 8555 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 838 8450 50  0001 C CNN
F 3 "~" H 800 8600 50  0001 C CNN
	1    800  8600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5F9A7627
P 2500 9300
F 0 "#PWR010" H 2500 9050 50  0001 C CNN
F 1 "GND" H 2505 9127 50  0000 C CNN
F 2 "" H 2500 9300 50  0001 C CNN
F 3 "" H 2500 9300 50  0001 C CNN
	1    2500 9300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5F9A620A
P 1700 8750
F 0 "#PWR08" H 1700 8500 50  0001 C CNN
F 1 "GND" H 1705 8577 50  0000 C CNN
F 2 "" H 1700 8750 50  0001 C CNN
F 3 "" H 1700 8750 50  0001 C CNN
	1    1700 8750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 8450 1900 8450
Connection ~ 1700 8450
$Comp
L Device:C C4
U 1 1 5F9A55E8
P 1700 8600
F 0 "C4" H 1815 8646 50  0000 L CNN
F 1 "47pF" H 1815 8555 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 1738 8450 50  0001 C CNN
F 3 "~" H 1700 8600 50  0001 C CNN
	1    1700 8600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 8450 1100 9100
Wire Wire Line
	1100 9100 2200 9100
Connection ~ 1100 8450
Connection ~ 2500 8450
Wire Wire Line
	2500 8050 2500 8450
Wire Wire Line
	2500 8450 2500 8900
Wire Wire Line
	2200 8450 2500 8450
$Comp
L Transistor_BJT:2N3904 Q1
U 1 1 5F99E2F5
P 2400 9100
F 0 "Q1" H 2590 9146 50  0000 L CNN
F 1 "2N3904" H 2590 9055 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 2600 9025 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 2400 9100 50  0001 L CNN
	1    2400 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 7600 1100 7800
Wire Wire Line
	2500 7600 1100 7600
$Comp
L Device:R R13
U 1 1 5F9BB4C0
P 2500 7900
F 0 "R13" H 2570 7946 50  0000 L CNN
F 1 "15K" H 2570 7855 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 2430 7900 50  0001 C CNN
F 3 "~" H 2500 7900 50  0001 C CNN
	1    2500 7900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 7750 2500 7600
Wire Wire Line
	1550 8450 1700 8450
$Comp
L Device:R R9
U 1 1 5F9919E9
P 2050 8450
F 0 "R9" V 1843 8450 50  0000 C CNN
F 1 "330k" V 1934 8450 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1980 8450 50  0001 C CNN
F 3 "~" H 2050 8450 50  0001 C CNN
	1    2050 8450
	0    1    1    0   
$EndComp
Wire Wire Line
	1100 8450 1250 8450
Wire Wire Line
	1100 8100 1100 8450
$Comp
L Device:Crystal Y1
U 1 1 5F9C0DB9
P 1400 8450
F 0 "Y1" H 1400 8718 50  0000 C CNN
F 1 "32768" H 1400 8627 50  0000 C CNN
F 2 "Crystals:Crystal_Round_d2.0mm_Vertical" H 1400 8450 50  0001 C CNN
F 3 "~" H 1400 8450 50  0001 C CNN
	1    1400 8450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5F9BD592
P 1100 7950
F 0 "R4" H 1170 7996 50  0000 L CNN
F 1 "10M" H 1170 7905 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1030 7950 50  0001 C CNN
F 3 "~" H 1100 7950 50  0001 C CNN
	1    1100 7950
	1    0    0    -1  
$EndComp
Text Label 5600 2700 0    50   ~ 0
KB_ADC_INP
Wire Wire Line
	1250 5850 1450 5850
Wire Wire Line
	1450 5850 1450 5700
Wire Wire Line
	600  5850 950  5850
$Comp
L Device:R R7
U 1 1 5FA3D2C4
P 1700 10850
F 0 "R7" H 1770 10896 50  0000 L CNN
F 1 "330" H 1770 10805 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1630 10850 50  0001 C CNN
F 3 "~" H 1700 10850 50  0001 C CNN
	1    1700 10850
	1    0    0    -1  
$EndComp
Text Label 1700 11150 0    50   ~ 0
5VUSB
Wire Wire Line
	1700 10050 1700 10600
Wire Wire Line
	1700 11000 1700 11150
$Comp
L power:GND #PWR09
U 1 1 5FA4A393
P 2350 10950
F 0 "#PWR09" H 2350 10700 50  0001 C CNN
F 1 "GND" H 2355 10777 50  0000 C CNN
F 2 "" H 2350 10950 50  0001 C CNN
F 3 "" H 2350 10950 50  0001 C CNN
	1    2350 10950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 10050 1800 10300
Wire Wire Line
	1800 10300 2350 10300
Wire Wire Line
	2350 10300 2350 10600
$Comp
L Device:CP C5
U 1 1 5FA5756D
P 2000 10600
F 0 "C5" V 2255 10600 50  0000 C CNN
F 1 "4u7" V 2164 10600 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 2038 10450 50  0001 C CNN
F 3 "~" H 2000 10600 50  0001 C CNN
	1    2000 10600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1850 10600 1700 10600
Connection ~ 1700 10600
Wire Wire Line
	1700 10600 1700 10700
Wire Wire Line
	2150 10600 2350 10600
Connection ~ 2350 10600
Wire Wire Line
	2350 10600 2350 10950
Text Notes 500  3050 0    50   ~ 0
Consider MCLR RC circuit to prevent \npremature bootup  (might cause\nLVP/ISP issues). Or just delay in \nmain() before starting.\nIncrease MCLR & voltage divider\nresistor values to reduce standby\ncurrent.
Text Label 650  10300 0    50   ~ 0
TFMS_INP
Wire Wire Line
	1500 10300 1500 10050
$Comp
L Device:R R5
U 1 1 5FAECF76
P 1450 10600
F 0 "R5" V 1243 10600 50  0000 C CNN
F 1 "10k" V 1334 10600 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1380 10600 50  0001 C CNN
F 3 "~" H 1450 10600 50  0001 C CNN
	1    1450 10600
	0    1    1    0   
$EndComp
Wire Wire Line
	1600 10600 1700 10600
Wire Wire Line
	650  10300 1300 10300
Wire Wire Line
	1300 10600 1300 10300
Connection ~ 1300 10300
Wire Wire Line
	1300 10300 1500 10300
Text Label 5600 4100 0    50   ~ 0
TFMS_INP
Wire Wire Line
	5300 4100 5600 4100
$Comp
L Connector:LCD_CONN J3
U 1 1 5FB416FC
P 3450 7150
F 0 "J3" H 2900 7150 50  0000 L CNN
F 1 "LCD" H 3200 7150 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical" H 3450 7150 50  0001 C CNN
F 3 "" H 3450 7150 50  0001 C CNN
	1    3450 7150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5FB62F4D
P 4650 7550
F 0 "C9" H 4535 7504 50  0000 R CNN
F 1 "2u2" H 4535 7595 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4688 7400 50  0001 C CNN
F 3 "~" H 4650 7550 50  0001 C CNN
	1    4650 7550
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR019
U 1 1 5FBA05EE
P 5000 8100
F 0 "#PWR019" H 5000 7850 50  0001 C CNN
F 1 "GND" H 5005 7927 50  0000 C CNN
F 2 "" H 5000 8100 50  0001 C CNN
F 3 "" H 5000 8100 50  0001 C CNN
	1    5000 8100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5FBD5BAB
P 3600 4900
F 0 "#PWR013" H 3600 4650 50  0001 C CNN
F 1 "GND" H 3605 4727 50  0000 C CNN
F 2 "" H 3600 4900 50  0001 C CNN
F 3 "" H 3600 4900 50  0001 C CNN
	1    3600 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 4600 3600 4900
Wire Wire Line
	3800 4600 3600 4600
Connection ~ 3600 4600
Text Label 3800 7500 0    50   ~ 0
LCDRS_U1TX_DHT
Wire Wire Line
	3450 7500 3800 7500
Text Label 3800 7600 0    50   ~ 0
LCDSCK_PGED
Wire Wire Line
	3450 7600 3800 7600
Text Label 3800 7700 0    50   ~ 0
LCDSDA_PGEC
Wire Wire Line
	3450 7700 3800 7700
Wire Wire Line
	4650 7900 4650 7700
Text Label 5600 3600 0    50   ~ 0
LCDRS_U1TX_DHT
Wire Wire Line
	5300 3600 5600 3600
Wire Wire Line
	5300 2700 5600 2700
Text Label 5600 2800 0    50   ~ 0
LCDSDA_PGEC
Wire Wire Line
	5300 2800 5600 2800
Text Label 5600 2900 0    50   ~ 0
LCDSCK_PGED
Wire Wire Line
	5300 2900 5600 2900
Text Label 1750 3900 0    50   ~ 0
SD_CS
Wire Wire Line
	1750 3900 2100 3900
Text Label 5600 3500 0    50   ~ 0
SD_MISO
Wire Wire Line
	5300 3500 5600 3500
Text Label 5600 3400 0    50   ~ 0
SD_MOSI
Wire Wire Line
	5300 3400 5600 3400
Text Label 5600 3300 0    50   ~ 0
SD_SCK
Wire Wire Line
	5300 3300 5600 3300
Text Label 4250 10650 0    50   ~ 0
LCDRS_U1TX_DHT
Wire Wire Line
	3700 10650 4250 10650
Text Label 4250 10850 0    50   ~ 0
LCDSDA_PGEC
Text Label 4250 10750 0    50   ~ 0
LCDSCK_PGED
$Comp
L power:GND #PWR016
U 1 1 5FF0A1D6
P 4300 10950
F 0 "#PWR016" H 4300 10700 50  0001 C CNN
F 1 "GND" H 4305 10777 50  0000 C CNN
F 2 "" H 4300 10950 50  0001 C CNN
F 3 "" H 4300 10950 50  0001 C CNN
	1    4300 10950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 10950 4300 10950
Text Label 8600 3350 0    50   ~ 0
LINE_IN_L
Wire Wire Line
	8600 3350 9100 3350
Text Label 8600 3750 0    50   ~ 0
BT_AUD_L
Wire Wire Line
	8600 3750 9100 3750
$Comp
L Device:R R32
U 1 1 5FFD73B1
P 9450 2900
F 0 "R32" V 9243 2900 50  0000 C CNN
F 1 "680" V 9334 2900 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 2900 50  0001 C CNN
F 3 "~" H 9450 2900 50  0001 C CNN
	1    9450 2900
	0    1    1    0   
$EndComp
$Comp
L Device:R R33
U 1 1 5FFD7F75
P 9450 3350
F 0 "R33" V 9243 3350 50  0000 C CNN
F 1 "680" V 9334 3350 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 3350 50  0001 C CNN
F 3 "~" H 9450 3350 50  0001 C CNN
	1    9450 3350
	0    1    1    0   
$EndComp
$Comp
L Device:R R34
U 1 1 5FFD8AD4
P 9450 3750
F 0 "R34" V 9243 3750 50  0000 C CNN
F 1 "680" V 9334 3750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 3750 50  0001 C CNN
F 3 "~" H 9450 3750 50  0001 C CNN
	1    9450 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	9600 2500 9600 2900
Wire Wire Line
	9600 2900 9600 3350
Connection ~ 9600 2900
Wire Wire Line
	9600 3350 9600 3750
Connection ~ 9600 3350
Text Label 8600 4900 0    50   ~ 0
RADIO_AUD_R
Wire Wire Line
	8600 4900 9100 4900
Text Label 8600 5350 0    50   ~ 0
LINE_IN_R
Text Label 8600 5750 0    50   ~ 0
BT_AUD_R
Wire Wire Line
	8600 5750 9100 5750
$Comp
L Device:R R35
U 1 1 60001064
P 9450 4500
F 0 "R35" V 9243 4500 50  0000 C CNN
F 1 "4k7" V 9334 4500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 4500 50  0001 C CNN
F 3 "~" H 9450 4500 50  0001 C CNN
	1    9450 4500
	0    1    1    0   
$EndComp
$Comp
L Device:R R36
U 1 1 6000106A
P 9450 4900
F 0 "R36" V 9243 4900 50  0000 C CNN
F 1 "680" V 9334 4900 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 4900 50  0001 C CNN
F 3 "~" H 9450 4900 50  0001 C CNN
	1    9450 4900
	0    1    1    0   
$EndComp
$Comp
L Device:R R37
U 1 1 60001070
P 9450 5350
F 0 "R37" V 9243 5350 50  0000 C CNN
F 1 "680" V 9334 5350 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 5350 50  0001 C CNN
F 3 "~" H 9450 5350 50  0001 C CNN
	1    9450 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	9600 4500 9600 4900
Wire Wire Line
	9600 4900 9600 5350
Connection ~ 9600 4900
Wire Wire Line
	9600 5350 9600 5750
Connection ~ 9600 5350
$Comp
L Amplifier_Operational:NE5532 U10
U 1 1 60021178
P 10600 2800
F 0 "U10" H 10600 3167 50  0000 C CNN
F 1 "TDA1308" H 10600 3076 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 10600 2800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/ne5532.pdf" H 10600 2800 50  0001 C CNN
	1    10600 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 2900 10200 2900
$Comp
L Amplifier_Operational:NE5532 U10
U 2 1 60087A3C
P 10650 4800
F 0 "U10" H 10650 5167 50  0000 C CNN
F 1 "TDA1308" H 10650 5076 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 10650 4800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/ne5532.pdf" H 10650 4800 50  0001 C CNN
	2    10650 4800
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:NE5532 U10
U 3 1 600B0181
P 10900 3550
F 0 "U10" H 10400 2850 50  0000 L CNN
F 1 "TDA1308" H 10350 2950 50  0000 L CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 10900 3550 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/ne5532.pdf" H 10900 3550 50  0001 C CNN
	3    10900 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR036
U 1 1 600B39C7
P 10050 4250
F 0 "#PWR036" H 10050 4000 50  0001 C CNN
F 1 "GND" H 10055 4077 50  0000 C CNN
F 2 "" H 10050 4250 50  0001 C CNN
F 3 "" H 10050 4250 50  0001 C CNN
	1    10050 4250
	1    0    0    -1  
$EndComp
Text Label 10450 3250 0    50   ~ 0
5VSPK
$Comp
L Device:R R41
U 1 1 600F50E9
P 10050 3900
F 0 "R41" H 10120 3946 50  0000 L CNN
F 1 "10k" H 10120 3855 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9980 3900 50  0001 C CNN
F 3 "~" H 10050 3900 50  0001 C CNN
	1    10050 3900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R40
U 1 1 60103F2E
P 10050 3400
F 0 "R40" H 10120 3446 50  0000 L CNN
F 1 "10k" H 10120 3355 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9980 3400 50  0001 C CNN
F 3 "~" H 10050 3400 50  0001 C CNN
	1    10050 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	10050 3250 10350 3250
Wire Wire Line
	10050 3550 10050 3650
Wire Wire Line
	10050 4250 10050 4100
Wire Wire Line
	10050 4100 10350 4100
Wire Wire Line
	10800 4100 10800 3850
Connection ~ 10050 4100
Wire Wire Line
	10050 4100 10050 4050
Wire Wire Line
	10050 3650 9850 3650
Wire Wire Line
	9850 3650 9850 2700
Connection ~ 10050 3650
Wire Wire Line
	10050 3650 10050 3750
Wire Wire Line
	9850 3650 9850 4100
Wire Wire Line
	9850 4700 10350 4700
Connection ~ 9850 3650
$Comp
L Device:CP C35
U 1 1 6018DE11
P 9850 5250
F 0 "C35" H 9968 5296 50  0000 L CNN
F 1 "22uF" H 9968 5205 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9888 5100 50  0001 C CNN
F 3 "~" H 9850 5250 50  0001 C CNN
	1    9850 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 4700 9850 5100
Connection ~ 9850 4700
$Comp
L power:GND #PWR035
U 1 1 6019CC6B
P 9850 5650
F 0 "#PWR035" H 9850 5400 50  0001 C CNN
F 1 "GND" H 9855 5477 50  0000 C CNN
F 2 "" H 9850 5650 50  0001 C CNN
F 3 "" H 9850 5650 50  0001 C CNN
	1    9850 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 5650 9850 5400
Wire Wire Line
	10200 5450 10200 4900
Connection ~ 10200 4900
Wire Wire Line
	10200 4900 10350 4900
Wire Wire Line
	10950 5450 10950 4800
$Comp
L Device:R R44
U 1 1 6022D118
P 11400 3300
F 0 "R44" H 11470 3346 50  0000 L CNN
F 1 "2k2" H 11470 3255 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 11330 3300 50  0001 C CNN
F 3 "~" H 11400 3300 50  0001 C CNN
	1    11400 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R45
U 1 1 6022EEB1
P 11400 4200
F 0 "R45" H 11470 4246 50  0000 L CNN
F 1 "2k2" H 11470 4155 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 11330 4200 50  0001 C CNN
F 3 "~" H 11400 4200 50  0001 C CNN
	1    11400 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	11400 2800 11400 3150
Wire Wire Line
	11400 4800 11400 4350
Connection ~ 10950 4800
Wire Wire Line
	10950 4800 11400 4800
Wire Wire Line
	11400 4050 11400 3650
$Comp
L Device:R_POT_Small RV1
U 1 1 602D04DA
P 11800 3750
F 0 "RV1" H 11740 3796 50  0000 R CNN
F 1 "10k" H 11740 3705 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 11800 3750 50  0001 C CNN
F 3 "~" H 11800 3750 50  0001 C CNN
	1    11800 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	11400 3650 11800 3650
Connection ~ 11400 3650
Wire Wire Line
	11400 3650 11400 3450
$Comp
L power:GND #PWR040
U 1 1 602F00E4
P 11800 4400
F 0 "#PWR040" H 11800 4150 50  0001 C CNN
F 1 "GND" H 11805 4227 50  0000 C CNN
F 2 "" H 11800 4400 50  0001 C CNN
F 3 "" H 11800 4400 50  0001 C CNN
	1    11800 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	11800 4400 11800 4350
$Comp
L Device:R R47
U 1 1 6030184B
P 12100 3900
F 0 "R47" H 12170 3946 50  0000 L CNN
F 1 "680" H 12170 3855 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 12030 3900 50  0001 C CNN
F 3 "~" H 12100 3900 50  0001 C CNN
	1    12100 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	11900 3750 12100 3750
$Comp
L Device:C_Small C42
U 1 1 60322468
P 12100 4250
F 0 "C42" H 12192 4296 50  0000 L CNN
F 1 "10nF" H 12192 4205 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12100 4250 50  0001 C CNN
F 3 "~" H 12100 4250 50  0001 C CNN
	1    12100 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	12100 4350 11800 4350
Text Label 12200 4100 0    50   ~ 0
REC_INP
Text Label 1750 3700 0    50   ~ 0
REC_INP
Wire Wire Line
	1750 3700 2100 3700
$Comp
L Device:R R25
U 1 1 603AD989
P 7600 2500
F 0 "R25" V 7393 2500 50  0000 C CNN
F 1 "68" V 7484 2500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 7530 2500 50  0001 C CNN
F 3 "~" H 7600 2500 50  0001 C CNN
	1    7600 2500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C16
U 1 1 603AF1A6
P 7850 2700
F 0 "C16" H 7758 2654 50  0000 R CNN
F 1 "100nF" H 7758 2745 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 7850 2700 50  0001 C CNN
F 3 "~" H 7850 2700 50  0001 C CNN
	1    7850 2700
	-1   0    0    1   
$EndComp
Wire Wire Line
	7750 2500 7850 2500
Wire Wire Line
	7850 2500 7850 2600
$Comp
L power:GND #PWR028
U 1 1 603C17B9
P 8000 3200
F 0 "#PWR028" H 8000 2950 50  0001 C CNN
F 1 "GND" H 8005 3027 50  0000 C CNN
F 2 "" H 8000 3200 50  0001 C CNN
F 3 "" H 8000 3200 50  0001 C CNN
	1    8000 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3200 8000 3050
$Comp
L Device:R R27
U 1 1 603D33AA
P 8100 2500
F 0 "R27" V 7893 2500 50  0000 C CNN
F 1 "680" V 7984 2500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 8030 2500 50  0001 C CNN
F 3 "~" H 8100 2500 50  0001 C CNN
	1    8100 2500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C18
U 1 1 603D33B0
P 8300 2700
F 0 "C18" H 8208 2654 50  0000 R CNN
F 1 "10nF" H 8208 2745 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8300 2700 50  0001 C CNN
F 3 "~" H 8300 2700 50  0001 C CNN
	1    8300 2700
	-1   0    0    1   
$EndComp
Wire Wire Line
	8300 2500 8300 2600
Wire Wire Line
	8000 3050 8300 3050
Wire Wire Line
	8300 3050 8300 2800
Wire Wire Line
	7850 2500 7950 2500
Connection ~ 7850 2500
Text Label 7100 2500 0    50   ~ 0
PWM1_H
Wire Wire Line
	7100 2500 7450 2500
Wire Wire Line
	8250 2500 8300 2500
Text Label 5600 4000 0    50   ~ 0
PWM2_H
Wire Wire Line
	5300 4000 5600 4000
Text Label 5600 3800 0    50   ~ 0
PWM1_H
Wire Wire Line
	5300 3800 5600 3800
$Comp
L Device:R R50
U 1 1 606F7D67
P 14350 1900
F 0 "R50" H 14420 1946 50  0000 L CNN
F 1 "18k" H 14420 1855 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 14280 1900 50  0001 C CNN
F 3 "~" H 14350 1900 50  0001 C CNN
	1    14350 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR046
U 1 1 60985DED
P 14100 2150
F 0 "#PWR046" H 14100 1900 50  0001 C CNN
F 1 "GND" H 14105 1977 50  0000 C CNN
F 2 "" H 14100 2150 50  0001 C CNN
F 3 "" H 14100 2150 50  0001 C CNN
	1    14100 2150
	1    0    0    -1  
$EndComp
Text Label 16000 2000 3    50   ~ 0
AMP_SHDN
Text Label 14700 650  0    50   ~ 0
5VSPK
$Comp
L Device:R R49
U 1 1 60B6B786
P 14200 1050
F 0 "R49" H 14000 900 50  0000 L CNN
F 1 "100k" V 14100 950 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 14130 1050 50  0001 C CNN
F 3 "~" H 14200 1050 50  0001 C CNN
	1    14200 1050
	1    0    0    -1  
$EndComp
Text Label 13750 1750 0    50   ~ 0
LEFT_CH
Text Label 13250 1300 2    50   ~ 0
RIGHT_CH
Text Label 9000 4100 0    50   ~ 0
2V5
$Comp
L Device:CP_Small C24
U 1 1 610F6EA2
P 9200 2500
F 0 "C24" V 9425 2500 50  0000 C CNN
F 1 "1uF" V 9334 2500 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 2500 50  0001 C CNN
F 3 "~" H 9200 2500 50  0001 C CNN
	1    9200 2500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8600 2900 9100 2900
$Comp
L Device:CP_Small C25
U 1 1 611436F9
P 9200 2900
F 0 "C25" V 9425 2900 50  0000 C CNN
F 1 "1uF" V 9334 2900 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 2900 50  0001 C CNN
F 3 "~" H 9200 2900 50  0001 C CNN
	1    9200 2900
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP_Small C26
U 1 1 6114461F
P 9200 3350
F 0 "C26" V 9425 3350 50  0000 C CNN
F 1 "1uF" V 9334 3350 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 3350 50  0001 C CNN
F 3 "~" H 9200 3350 50  0001 C CNN
	1    9200 3350
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP_Small C27
U 1 1 6115DE49
P 9200 3750
F 0 "C27" V 9425 3750 50  0000 C CNN
F 1 "1uF" V 9334 3750 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 3750 50  0001 C CNN
F 3 "~" H 9200 3750 50  0001 C CNN
	1    9200 3750
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP_Small C28
U 1 1 61160B2D
P 9200 4500
F 0 "C28" V 9425 4500 50  0000 C CNN
F 1 "1uF" V 9334 4500 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 4500 50  0001 C CNN
F 3 "~" H 9200 4500 50  0001 C CNN
	1    9200 4500
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP_Small C29
U 1 1 61178FF6
P 9200 4900
F 0 "C29" V 9425 4900 50  0000 C CNN
F 1 "1uF" V 9334 4900 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 4900 50  0001 C CNN
F 3 "~" H 9200 4900 50  0001 C CNN
	1    9200 4900
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP_Small C30
U 1 1 611914E2
P 9200 5350
F 0 "C30" V 9425 5350 50  0000 C CNN
F 1 "1uF" V 9334 5350 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 5350 50  0001 C CNN
F 3 "~" H 9200 5350 50  0001 C CNN
	1    9200 5350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8600 5350 9100 5350
$Comp
L Device:CP_Small C31
U 1 1 611A9979
P 9200 5750
F 0 "C31" V 9425 5750 50  0000 C CNN
F 1 "1uF" V 9334 5750 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 9200 5750 50  0001 C CNN
F 3 "~" H 9200 5750 50  0001 C CNN
	1    9200 5750
	0    -1   -1   0   
$EndComp
Text Notes 11000 2550 0    50   ~ 0
Amplifier output uses 2.5V as reference,\nnot circuit GND due to usage of voltage\ndivider for op amp in single supply mode. \n1uF series capacitor to remove headphone offset\nInput sources GND should be connected to 2.5V.
Text Label 12600 4550 0    50   ~ 0
RIGHT_CH
Text Label 12600 3050 0    50   ~ 0
LEFT_CH
Text Notes 1150 2000 0    50   ~ 0
MAIN POWER RAIL (ALWAYS ON)\nSoft power (default on when /SHDN = 1)\nRC circuit at PIC PWR_ON to prevent\npower disruption due to noises.\n2nd decoupling cap should be close to PIC VDD pin
Text Notes 600  7300 0    50   ~ 0
32768Hz OSCILLATOR FOR SOFTWARE RTC\n70uA current, 3Vpp, 5s powerup time.
Text Notes 3000 10500 0    50   ~ 0
Shared pins: UART TX/LCD_RS/DHT11\nPICKit/LCD_SDA/LCD_SCK
Text Notes 550  9600 0    50   ~ 0
INFRARED REMOTE DECODER MODULE (TFMS5400)
Text Notes 2400 5200 0    50   ~ 0
ANALOG KEYBOARD INPUT
Text Notes 11000 5900 0    50   ~ 0
Summing amplifier configuration.\nSupports headphone output. Resistor \nratios/gains chosen to fit typical\nvoltage levels of input sources. \nRefer to datasheet for typical values.\nExcessive values cause background white \nnoises and oscillations. \nConsider using double log potentionmeter\nfor op-amp feedback resistors and for \namplifier trimmer resistors to adjust \nheadphone and speaker volume.
Text Notes 7100 3950 0    50   ~ 0
Double RC low-pass filter (22kHz)  to remove \nPWM harmonics\n\nCapacitors before amplifier inputs to remove \nDC offset (especially for radio output)
Connection ~ 9850 4100
Wire Wire Line
	9850 4100 9850 4700
Text Notes 8650 6500 0    50   ~ 0
Low-pass filter to remove high freq \nnoises causing recording audio issues \ndue to aliasing. Adjust trimmer to \nmaximize output @ 3.3V. DC offset \nremoved in software (cannot use \nseries capacitor which causes \nnegative voltage)
Text Notes 7300 2150 0    50   ~ 0
SPEAKER POWER RAIL\n\nLarge decoupling capacitor to\nreduce noises due to speaker\nhigh current consumption\nplaced near amplifier
$Comp
L Device:C_Small C2
U 1 1 6171BBBD
P 850 1250
F 0 "C2" H 942 1296 50  0000 L CNN
F 1 "10uF" H 942 1205 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 850 1250 50  0001 C CNN
F 3 "~" H 850 1250 50  0001 C CNN
	1    850  1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  1050 850  1150
Text Notes 600  650  0    50   ~ 0
Separate power supply rails to reduce noises. 3V3PIC always on, the rest will only be on when PWR_ON pin is high.\nReverse polarity will not cause damage due to LT1763 support, take note to use non-polarized capacitor for VCC.
Text Notes 13200 4250 0    50   ~ 0
556 TIMER NON-INVERTING BUFFER\n\nHeadphone removed, pin 3 & 5 connected, HP_STATE \n= audio level max around 1.4V (PWM) <1/3 VCC -> OUT = 0V\nHeadphone connected, pin 5 floating, HP_STATE = 5V (>2/3 VCC)\n-> OUT = 5V.\nLEFT_CH centered above GND,  diodes prevent NEG voltage when \nheadphone is not connected and drop voltage below 1/3 VCC.\nNested inverting buffer = non-inverting for 8002 amplifier \n(SHDN is active high, >1.2V = shutdown, < 0.9V = normal)\nSingle non-inverting buffer config of 555 cannot be used \n(threshold is 0.7V). Input impedance at SHDN pin is low - \ncannot simply use trimmer.
Text Notes 4950 1750 0    50   ~ 0
SD CARD POWER RAIL
Text Notes 750  4900 0    50   ~ 0
Voltage divider to measure battery voltage.\nDiode prevents PIC damage due to reverse polarity.\nDiode voltage drop compensated in software.\nThis limits max. VCC to around 12V.
$Comp
L Device:CP_Small C40
U 1 1 5F9C29A2
P 11700 2800
F 0 "C40" V 11850 2800 50  0000 C CNN
F 1 "1uF" V 11550 2800 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 11700 2800 50  0001 C CNN
F 3 "~" H 11700 2800 50  0001 C CNN
	1    11700 2800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11400 2800 11600 2800
Connection ~ 11400 2800
$Comp
L Device:CP_Small C41
U 1 1 5FA91A4D
P 11700 4800
F 0 "C41" V 11850 4800 50  0000 C CNN
F 1 "1uF" V 11550 4800 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 11700 4800 50  0001 C CNN
F 3 "~" H 11700 4800 50  0001 C CNN
	1    11700 4800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11400 4800 11600 4800
Text Notes 13200 4650 0    50   ~ 0
For line-in, audio input GND is 2.5V and not circuit GND\nFor bluetooth, audio input GND is also circuit GND\n(this assumes line-in does not share common ground\nwhereas bluetoth does)
Connection ~ 11800 4350
Wire Wire Line
	11800 4350 11800 3850
Wire Wire Line
	2950 1050 3400 1050
Connection ~ 3600 1050
Wire Wire Line
	9000 4100 9850 4100
Text Notes 7100 4200 0    50   ~ 0
AUDIO MIXER & PRE-AMPLIFIER
Connection ~ 5200 1600
Wire Wire Line
	4700 1600 4700 1750
Wire Wire Line
	4700 1600 5200 1600
Wire Wire Line
	4700 1100 4700 950 
Wire Wire Line
	6050 1200 6500 1200
Wire Wire Line
	6500 1200 6500 1300
Connection ~ 6050 1200
Wire Wire Line
	6500 1200 6500 800 
Connection ~ 6500 1200
$Comp
L symbols_STRN_Lab:LT1763-S8 U8
U 1 1 5FE8C2BC
P 7650 1250
F 0 "U8" H 7650 1667 50  0000 C CNN
F 1 "LT1763-5.0" H 7650 1576 50  0000 C CNN
F 2 "LT1763IS8-5PBF:SO-8_S" H 7650 450 50  0001 C CNN
F 3 "" H 8450 1400 50  0001 C CNN
	1    7650 1250
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR025
U 1 1 5FE8C2C2
P 7050 950
F 0 "#PWR025" H 7050 800 50  0001 C CNN
F 1 "VCC" H 7065 1123 50  0000 C CNN
F 2 "" H 7050 950 50  0001 C CNN
F 3 "" H 7050 950 50  0001 C CNN
	1    7050 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 1100 7050 1100
$Comp
L power:GND #PWR026
U 1 1 5FE8C2CB
P 7050 1750
F 0 "#PWR026" H 7050 1500 50  0001 C CNN
F 1 "GND" H 7055 1577 50  0000 C CNN
F 2 "" H 7050 1750 50  0001 C CNN
F 3 "" H 7050 1750 50  0001 C CNN
	1    7050 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 1600 7750 1600
Wire Wire Line
	7550 1600 7650 1600
Connection ~ 7650 1600
$Comp
L Device:C C20
U 1 1 5FE8C2D4
P 8400 1350
F 0 "C20" H 8515 1396 50  0000 L CNN
F 1 "10nF" H 8515 1305 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8438 1200 50  0001 C CNN
F 3 "~" H 8400 1350 50  0001 C CNN
	1    8400 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1200 8400 1200
Wire Wire Line
	8150 1400 8150 1500
Wire Wire Line
	8150 1500 8400 1500
Wire Wire Line
	8150 1200 8150 1100
Connection ~ 8150 1200
Text Label 8850 800  0    50   ~ 0
5VSPK
Connection ~ 7550 1600
Wire Wire Line
	7050 1600 7050 1750
Wire Wire Line
	7050 1600 7550 1600
Wire Wire Line
	7050 1100 7050 950 
Wire Wire Line
	8400 1200 8850 1200
Connection ~ 8400 1200
Wire Wire Line
	8850 1200 8850 800 
Text Notes 9850 1750 0    50   ~ 0
USB POWER RAIL
$Comp
L symbols_STRN_Lab:LT1763-S8 U9
U 1 1 5FF43708
P 10100 1250
F 0 "U9" H 10100 1667 50  0000 C CNN
F 1 "LT1763-5.0" H 10100 1576 50  0000 C CNN
F 2 "LT1763IS8-5PBF:SO-8_S" H 10100 450 50  0001 C CNN
F 3 "" H 10900 1400 50  0001 C CNN
	1    10100 1250
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR032
U 1 1 5FF4370E
P 9500 950
F 0 "#PWR032" H 9500 800 50  0001 C CNN
F 1 "VCC" H 9515 1123 50  0000 C CNN
F 2 "" H 9500 950 50  0001 C CNN
F 3 "" H 9500 950 50  0001 C CNN
	1    9500 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 1100 9500 1100
$Comp
L power:GND #PWR033
U 1 1 5FF43717
P 9500 1750
F 0 "#PWR033" H 9500 1500 50  0001 C CNN
F 1 "GND" H 9505 1577 50  0000 C CNN
F 2 "" H 9500 1750 50  0001 C CNN
F 3 "" H 9500 1750 50  0001 C CNN
	1    9500 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 1600 10200 1600
Wire Wire Line
	10000 1600 10100 1600
Connection ~ 10100 1600
$Comp
L Device:C C36
U 1 1 5FF43720
P 10850 1350
F 0 "C36" H 10965 1396 50  0000 L CNN
F 1 "10nF" H 10965 1305 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10888 1200 50  0001 C CNN
F 3 "~" H 10850 1350 50  0001 C CNN
	1    10850 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 1200 10850 1200
Wire Wire Line
	10600 1400 10600 1500
Wire Wire Line
	10600 1500 10850 1500
Wire Wire Line
	10600 1200 10600 1100
Connection ~ 10600 1200
$Comp
L Device:CP C37
U 1 1 5FF4372B
P 11300 1450
F 0 "C37" H 11418 1496 50  0000 L CNN
F 1 "100uF" H 11418 1405 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 11338 1300 50  0001 C CNN
F 3 "~" H 11300 1450 50  0001 C CNN
	1    11300 1450
	1    0    0    -1  
$EndComp
Text Label 11300 800  0    50   ~ 0
5VUSB
$Comp
L power:GND #PWR038
U 1 1 5FF43732
P 11300 1750
F 0 "#PWR038" H 11300 1500 50  0001 C CNN
F 1 "GND" H 11305 1577 50  0000 C CNN
F 2 "" H 11300 1750 50  0001 C CNN
F 3 "" H 11300 1750 50  0001 C CNN
	1    11300 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	11300 1750 11300 1600
Connection ~ 10000 1600
Wire Wire Line
	9500 1600 9500 1750
Wire Wire Line
	9500 1600 10000 1600
Wire Wire Line
	9500 1100 9500 950 
Wire Wire Line
	10850 1200 11300 1200
Wire Wire Line
	11300 1200 11300 1300
Connection ~ 10850 1200
Wire Wire Line
	11300 1200 11300 800 
Connection ~ 11300 1200
Wire Wire Line
	800  3200 800  3600
$Comp
L power:VCC #PWR03
U 1 1 5F9B7BC6
P 800 3200
F 0 "#PWR03" H 800 3050 50  0001 C CNN
F 1 "VCC" H 815 3373 50  0000 C CNN
F 2 "" H 800 3200 50  0001 C CNN
F 3 "" H 800 3200 50  0001 C CNN
	1    800  3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	13700 1750 13700 1600
Connection ~ 13700 1750
Wire Wire Line
	13700 1750 13750 1750
Wire Wire Line
	14200 1400 14200 1250
Wire Wire Line
	13650 1850 13900 1850
Wire Wire Line
	14100 1850 14100 1400
Wire Wire Line
	14100 1400 14200 1400
Text Label 12600 3650 2    50   ~ 0
MONO_AUDIO
Text Notes 14150 8650 0    50   ~ 0
Gain = 20*log [2*(Rf/Ri)] \nLog scale potentiometer to adjust volume\nDelay 200ms when audio is resumed after \nheadset removed due to SHDN startup time.\nSmall decoupling cap close to amplifier\nBig decoupling cap close to power supply
$Comp
L power:GND #PWR041
U 1 1 613312EA
P 15450 6250
F 0 "#PWR041" H 15450 6000 50  0001 C CNN
F 1 "GND" H 15455 6077 50  0000 C CNN
F 2 "" H 15450 6250 50  0001 C CNN
F 3 "" H 15450 6250 50  0001 C CNN
	1    15450 6250
	1    0    0    -1  
$EndComp
Text Label 15700 5950 0    50   ~ 0
5VSPK
Wire Wire Line
	15450 5950 15700 5950
$Comp
L power:GND #PWR022
U 1 1 61533F14
P 5100 10400
F 0 "#PWR022" H 5100 10150 50  0001 C CNN
F 1 "GND" H 5105 10227 50  0000 C CNN
F 2 "" H 5100 10400 50  0001 C CNN
F 3 "" H 5100 10400 50  0001 C CNN
	1    5100 10400
	1    0    0    -1  
$EndComp
Text Label 5900 7400 0    50   ~ 0
5VUSB
$Comp
L Device:C_Small C12
U 1 1 616176FF
P 6350 8050
F 0 "C12" V 6121 8050 50  0000 C CNN
F 1 "100nF" V 6212 8050 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 6350 8050 50  0001 C CNN
F 3 "~" H 6350 8050 50  0001 C CNN
	1    6350 8050
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C10
U 1 1 616E195C
P 5250 8400
F 0 "C10" H 5342 8446 50  0000 L CNN
F 1 "100nF" H 5342 8355 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 5250 8400 50  0001 C CNN
F 3 "~" H 5250 8400 50  0001 C CNN
	1    5250 8400
	1    0    0    -1  
$EndComp
Text Label 8600 10450 0    50   ~ 0
SD_MISO
Text Label 8600 10350 0    50   ~ 0
SD_SCK
Text Label 8600 10250 0    50   ~ 0
SD_MOSI
Text Label 9100 10150 3    50   ~ 0
SD_CS
Wire Wire Line
	8450 10050 8600 10050
Text Label 8600 10050 0    50   ~ 0
3V3SD
$Comp
L Connector:SD_CONN J2
U 1 1 5FDED04F
P 8450 9950
F 0 "J2" H 8200 9200 50  0000 L CNN
F 1 "SD1" H 8050 9950 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 8450 9950 50  0001 C CNN
F 3 "" H 8450 9950 50  0001 C CNN
	1    8450 9950
	1    0    0    -1  
$EndComp
Text Label 5000 9450 2    50   ~ 0
CH376_SDO
Text Label 5000 9550 2    50   ~ 0
CH376_SDI
Text Label 5000 9350 2    50   ~ 0
CH376_SCK
Text Label 5000 9150 2    50   ~ 0
CH376_SCS
$Comp
L power:GND #PWR027
U 1 1 61D65297
P 7150 8650
F 0 "#PWR027" H 7150 8400 50  0001 C CNN
F 1 "GND" H 7155 8477 50  0000 C CNN
F 2 "" H 7150 8650 50  0001 C CNN
F 3 "" H 7150 8650 50  0001 C CNN
	1    7150 8650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 8650 6900 8450
Text Notes 8050 8950 0    50   ~ 0
CH376 USB PROCESSOR\n\nCH376 also supports SD card. But better to use \nPIC to read SD cards (faster, save power). The \nSD card slot here is provided for experiments only.\nIf CH376 SD is used, PIC SD pins are available for\nother uses (e.g. access SI4735 interrupt or to toggle \nexternal bluetooth module power, demonstrated below)\nCH376S has wider pin pitch than CH376T \n(easier to solder) but only CH376T is supported by \nKiCad. We need to design CH376S manually.
Text Notes 8050 9200 0    50   ~ 0
Simplified socket for SD card in SPI mode\n(assuming a breakout board is used)
Text Label 8600 2900 0    50   ~ 0
RADIO_AUD_L
Wire Wire Line
	7850 2800 7850 3050
Wire Wire Line
	7850 3050 8000 3050
Connection ~ 8000 3050
Wire Wire Line
	8300 2500 9100 2500
Connection ~ 8300 2500
$Comp
L Device:R R26
U 1 1 623D3F7F
P 7600 4500
F 0 "R26" V 7393 4500 50  0000 C CNN
F 1 "68" V 7484 4500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 7530 4500 50  0001 C CNN
F 3 "~" H 7600 4500 50  0001 C CNN
	1    7600 4500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C17
U 1 1 623D3F85
P 7850 4700
F 0 "C17" H 7758 4654 50  0000 R CNN
F 1 "100nF" H 7758 4745 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 7850 4700 50  0001 C CNN
F 3 "~" H 7850 4700 50  0001 C CNN
	1    7850 4700
	-1   0    0    1   
$EndComp
Wire Wire Line
	7750 4500 7850 4500
Wire Wire Line
	7850 4500 7850 4600
$Comp
L Device:R R28
U 1 1 623D3F94
P 8100 4500
F 0 "R28" V 7893 4500 50  0000 C CNN
F 1 "680" V 7984 4500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 8030 4500 50  0001 C CNN
F 3 "~" H 8100 4500 50  0001 C CNN
	1    8100 4500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C19
U 1 1 623D3F9A
P 8300 4700
F 0 "C19" H 8208 4654 50  0000 R CNN
F 1 "10nF" H 8208 4745 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8300 4700 50  0001 C CNN
F 3 "~" H 8300 4700 50  0001 C CNN
	1    8300 4700
	-1   0    0    1   
$EndComp
Wire Wire Line
	8300 4500 8300 4600
Wire Wire Line
	8300 5050 8300 4800
Wire Wire Line
	7850 4500 7950 4500
Connection ~ 7850 4500
Text Label 7100 4500 0    50   ~ 0
PWM2_H
Wire Wire Line
	7100 4500 7450 4500
Wire Wire Line
	8250 4500 8300 4500
Wire Wire Line
	7850 4800 7850 5050
Wire Wire Line
	8300 4500 9100 4500
Connection ~ 8300 4500
Text Label 5600 3100 0    50   ~ 0
CH376_SCS
Wire Wire Line
	5300 3100 5600 3100
Text Label 5600 3700 0    50   ~ 0
CH376_SDO
Wire Wire Line
	5300 3700 5600 3700
Text Label 5600 2600 0    50   ~ 0
CH376_SDI
Wire Wire Line
	5300 2600 5600 2600
Text Label 5600 3000 0    50   ~ 0
CH376_SCK
Wire Wire Line
	5300 3000 5600 3000
Wire Wire Line
	5300 3200 6100 3200
$Comp
L Device:R R24
U 1 1 5F9B9878
P 6250 3200
F 0 "R24" V 6350 3200 50  0000 C CNN
F 1 "1k" V 6250 3200 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 6180 3200 50  0001 C CNN
F 3 "~" H 6250 3200 50  0001 C CNN
	1    6250 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 3600 6550 3500
$Comp
L power:GND #PWR024
U 1 1 5F9B8555
P 6550 3600
F 0 "#PWR024" H 6550 3350 50  0001 C CNN
F 1 "GND" H 6555 3427 50  0000 C CNN
F 2 "" H 6550 3600 50  0001 C CNN
F 3 "" H 6550 3600 50  0001 C CNN
	1    6550 3600
	1    0    0    -1  
$EndComp
Text Label 6700 3200 0    50   ~ 0
PWR_ON
Text Label 5600 3900 0    50   ~ 0
I2C_SDA
Wire Wire Line
	5300 3900 5600 3900
Text Label 1750 4000 0    50   ~ 0
I2C_SCL
Wire Wire Line
	1750 4000 2100 4000
$Comp
L power:PWR_FLAG #FLG01
U 1 1 64ECDCF9
P 700 900
F 0 "#FLG01" H 700 975 50  0001 C CNN
F 1 "PWR_FLAG" H 700 1073 50  0000 C CNN
F 2 "" H 700 900 50  0001 C CNN
F 3 "~" H 700 900 50  0001 C CNN
	1    700  900 
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5F994FBD
P 700 1050
F 0 "#PWR01" H 700 900 50  0001 C CNN
F 1 "VCC" H 715 1223 50  0000 C CNN
F 2 "" H 700 1050 50  0001 C CNN
F 3 "" H 700 1050 50  0001 C CNN
	1    700  1050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	700  1050 700  900 
Wire Wire Line
	700  1050 850  1050
Connection ~ 700  1050
Connection ~ 850  1050
$Comp
L power:PWR_FLAG #FLG02
U 1 1 65217E2B
P 850 1800
F 0 "#FLG02" H 850 1875 50  0001 C CNN
F 1 "PWR_FLAG" H 850 2000 50  0000 C CNN
F 2 "" H 850 1800 50  0001 C CNN
F 3 "~" H 850 1800 50  0001 C CNN
	1    850  1800
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 7900 3650 7900
Wire Wire Line
	3450 7300 3650 7300
Wire Wire Line
	3650 7300 3650 7900
Connection ~ 3650 7900
Wire Wire Line
	3650 7900 4650 7900
Connection ~ 850  1550
$Comp
L power:GND #PWR0101
U 1 1 65F9C40A
P 850 2050
F 0 "#PWR0101" H 850 1800 50  0001 C CNN
F 1 "GND" H 855 1877 50  0000 C CNN
F 2 "" H 850 2050 50  0001 C CNN
F 3 "" H 850 2050 50  0001 C CNN
	1    850  2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  1800 850  2050
$Comp
L Device:C_Small C13
U 1 1 61595682
P 6350 8450
F 0 "C13" V 6121 8450 50  0000 C CNN
F 1 "10nF" V 6212 8450 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 6350 8450 50  0001 C CNN
F 3 "~" H 6350 8450 50  0001 C CNN
	1    6350 8450
	0    1    1    0   
$EndComp
Connection ~ 6900 8450
Wire Wire Line
	6900 8050 6900 8450
Wire Wire Line
	6050 8600 6050 8450
Wire Wire Line
	6050 8450 6250 8450
Wire Wire Line
	3800 2000 3800 1700
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5FA874BF
P 1400 10850
F 0 "#FLG0102" H 1400 10925 50  0001 C CNN
F 1 "PWR_FLAG" H 1400 11023 50  0000 C CNN
F 2 "" H 1400 10850 50  0001 C CNN
F 3 "~" H 1400 10850 50  0001 C CNN
	1    1400 10850
	-1   0    0    1   
$EndComp
Wire Wire Line
	1400 10850 1400 10700
Wire Wire Line
	1400 10700 1700 10700
Connection ~ 1700 10700
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 5FACD4FA
P 6050 8350
F 0 "#FLG0103" H 6050 8425 50  0001 C CNN
F 1 "PWR_FLAG" H 6150 8550 50  0000 C CNN
F 2 "" H 6050 8350 50  0001 C CNN
F 3 "~" H 6050 8350 50  0001 C CNN
	1    6050 8350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 8450 6050 8350
Connection ~ 6050 8450
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 5FB13601
P 4050 2000
F 0 "#FLG0104" H 4050 2075 50  0001 C CNN
F 1 "PWR_FLAG" H 4050 2173 50  0000 C CNN
F 2 "" H 4050 2000 50  0001 C CNN
F 3 "~" H 4050 2000 50  0001 C CNN
	1    4050 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2000 3800 2000
Connection ~ 3800 2000
Text Notes 11000 2100 0    50   ~ 0
Note: upside down op-amp icon in KiCad
Text Notes 5450 10900 0    50   ~ 0
SPI pull-up resistors via SD card \nbreakout board, internal pull-up (CH376),\nor weak pull-up enabled in PIC
$Comp
L Connector:TFMS5400 U2
U 1 1 5FD70A05
P 1900 10050
F 0 "U2" H 1650 10100 50  0000 C CNN
F 1 "TFMS" H 1650 9500 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 1900 10050 50  0001 C CNN
F 3 "" H 1900 10050 50  0001 C CNN
	1    1900 10050
	0    1    1    0   
$EndComp
Wire Wire Line
	7850 5050 8300 5050
$Comp
L power:GND #PWR029
U 1 1 623D3F8D
P 8300 5200
F 0 "#PWR029" H 8300 4950 50  0001 C CNN
F 1 "GND" H 8305 5027 50  0000 C CNN
F 2 "" H 8300 5200 50  0001 C CNN
F 3 "" H 8300 5200 50  0001 C CNN
	1    8300 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 5200 8300 5050
Connection ~ 8300 5050
Wire Wire Line
	5000 9550 5350 9550
Wire Wire Line
	5000 9150 5350 9150
Wire Wire Line
	5000 9450 5350 9450
Wire Wire Line
	5000 9350 5350 9350
NoConn ~ 5350 9250
Wire Wire Line
	5250 8300 5250 8050
Wire Wire Line
	5250 8500 5250 9000
Wire Wire Line
	5250 9000 5350 9000
NoConn ~ 5350 8900
Wire Wire Line
	5950 10400 5200 10400
$Comp
L project-rescue:CH376S-Memory_Controller U6
U 1 1 6000AD86
P 5950 9500
F 0 "U6" H 5550 10300 50  0000 C CNN
F 1 "CH376S" H 5950 8420 50  0000 C CNN
F 2 "SOP-28:SOP-28_300MIL" H 5950 8500 50  0001 C CIN
F 3 "http://iteadstudio.com/store/images/produce/Platform/FPGA/BlackGold/CH376DS1.PDF" H 5950 9600 50  0001 C CNN
	1    5950 9500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 9750 5200 9750
Wire Wire Line
	5200 9750 5200 9850
Connection ~ 5200 10400
Wire Wire Line
	5200 10400 5100 10400
Wire Wire Line
	5350 9850 5200 9850
Connection ~ 5200 9850
Wire Wire Line
	5200 9850 5200 10400
$Comp
L Device:Crystal Y3
U 1 1 6167A57B
P 6900 9600
F 0 "Y3" V 6700 9550 50  0000 L CNN
F 1 "12M" V 6900 9500 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 6900 9600 50  0001 C CNN
F 3 "~" H 6900 9600 50  0001 C CNN
	1    6900 9600
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 9450 6900 9450
Wire Wire Line
	6550 9550 6600 9550
Wire Wire Line
	6600 9750 6900 9750
NoConn ~ 6550 9150
NoConn ~ 6550 9250
NoConn ~ 6550 9350
Wire Wire Line
	5250 8050 5900 8050
Connection ~ 5900 8050
Wire Wire Line
	5900 8050 6250 8050
Wire Wire Line
	5900 8050 5900 7700
$Comp
L Device:LED D3
U 1 1 61968EF2
P 6700 9200
F 0 "D3" V 6739 9082 50  0000 R CNN
F 1 "LED" V 6648 9082 50  0000 R CNN
F 2 "LEDs:LED_D3.0mm" H 6700 9200 50  0001 C CNN
F 3 "~" H 6700 9200 50  0001 C CNN
	1    6700 9200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6600 9550 6600 9750
Wire Wire Line
	6550 9650 6700 9650
Wire Wire Line
	6700 9650 6700 9350
$Comp
L Device:R R46
U 1 1 61CF239D
P 6700 8700
F 0 "R46" H 6500 8700 50  0000 L CNN
F 1 "4k7" H 6800 8700 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 6630 8700 50  0001 C CNN
F 3 "~" H 6700 8700 50  0001 C CNN
	1    6700 8700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 9050 6700 8850
Wire Wire Line
	6600 5050 6450 5050
Wire Wire Line
	6450 4950 6600 4950
Connection ~ 6600 4950
Wire Wire Line
	6600 4950 6600 5050
Wire Wire Line
	6450 4850 6600 4850
Wire Wire Line
	6600 4850 6600 4950
$Comp
L power:GND #PWR0102
U 1 1 638B4FDF
P 6600 5250
F 0 "#PWR0102" H 6600 5000 50  0001 C CNN
F 1 "GND" H 6605 5077 50  0000 C CNN
F 2 "" H 6600 5250 50  0001 C CNN
F 3 "" H 6600 5250 50  0001 C CNN
	1    6600 5250
	1    0    0    -1  
$EndComp
Text Label 6050 5750 0    50   ~ 0
3V3SD
NoConn ~ 5650 4850
$Comp
L Connector:KEYB_CONN1 J1
U 1 1 5FB1C66E
P 2350 5850
F 0 "J1" V 1600 4900 50  0000 L CNN
F 1 "KEYB1" V 1600 5550 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical" H 2400 5850 50  0001 C CNN
F 3 "" H 2400 5850 50  0001 C CNN
	1    2350 5850
	0    1    1    0   
$EndComp
$Comp
L Connector:KEYB_CONN2 J5
U 1 1 5FB698AE
P 4000 5700
F 0 "J5" V 3600 5000 50  0000 L CNN
F 1 "KEYB2" V 3600 5450 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4000 5700 50  0001 C CNN
F 3 "" H 4000 5700 50  0001 C CNN
	1    4000 5700
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 8450 6900 8450
Wire Wire Line
	6450 8050 6900 8050
Wire Wire Line
	6700 8550 6700 7700
Wire Wire Line
	6700 7700 5900 7700
Connection ~ 5900 7700
Wire Wire Line
	5900 7700 5900 7400
$Comp
L Device:CP C3
U 1 1 5F9AD289
P 1600 3150
F 0 "C3" H 1718 3196 50  0000 L CNN
F 1 "10uF" H 1718 3105 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 1638 3000 50  0001 C CNN
F 3 "~" H 1600 3150 50  0001 C CNN
	1    1600 3150
	1    0    0    -1  
$EndComp
$Comp
L Connector:CONN_PWR J13
U 1 1 60004161
P 500 1450
F 0 "J13" V 300 1350 50  0000 R CNN
F 1 "PWR" V 0   1400 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 500 1450 50  0001 C CNN
F 3 "" H 500 1450 50  0001 C CNN
	1    500  1450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	700  1050 600  1050
Wire Wire Line
	600  1050 600  1450
Wire Wire Line
	700  1450 850  1450
Wire Wire Line
	850  1350 850  1450
Connection ~ 850  1450
Wire Wire Line
	850  1450 850  1550
Wire Wire Line
	850  1550 850  1800
Connection ~ 850  1800
$Comp
L Connector:Conn_01x02_Male J16
U 1 1 5FADB5DB
P 15250 5950
F 0 "J16" H 15358 6131 50  0000 C CNN
F 1 "SPK_PWR" H 15358 6040 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 15250 5950 50  0001 C CNN
F 3 "~" H 15250 5950 50  0001 C CNN
	1    15250 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	15450 6050 15450 6250
Text Notes 4800 6700 0    50   ~ 0
7-bit I2C addresses for 24LC256: 0b1010000 (A0=A1=A2=0)\nUsed for storing app config and station presets\n\nDHT11 pins shared with LCD RS / U1TX. \nWorks well with 3.3V. Pull-up enabled \nin software when reading temperature. \nStandby current is low, which is acceptable.\n\nCapacitor needed on SCL only (values should be reduced),\nSDA probably does not need capacitor.
Wire Wire Line
	3700 10850 4250 10850
Wire Wire Line
	3700 10750 4250 10750
$Comp
L Connector:CONN_DEBUG J4
U 1 1 60DF9A74
P 3700 10100
F 0 "J4" H 3050 9650 50  0000 C CNN
F 1 "DEBUG" H 3250 9150 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3700 10100 50  0001 C CNN
F 3 "" H 3700 10100 50  0001 C CNN
	1    3700 10100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 8050 5900 8600
NoConn ~ 5750 8600
NoConn ~ 5350 9650
NoConn ~ 5350 9950
NoConn ~ 5350 10050
Wire Wire Line
	1200 1350 1050 1350
Wire Wire Line
	1050 1350 1050 1050
Connection ~ 1050 1050
Wire Wire Line
	1050 1050 1200 1050
Wire Wire Line
	4650 1400 4800 1400
Text Label 7050 1400 2    50   ~ 0
PWR_ON
Wire Wire Line
	7050 1400 7150 1400
Text Label 4650 1400 2    50   ~ 0
PWR_ON
Text Label 9450 1400 2    50   ~ 0
PWR_ON
Wire Wire Line
	9450 1400 9600 1400
$Comp
L Memory_EEPROM:24LC256 U7
U 1 1 64C88029
P 6050 4950
F 0 "U7" H 5800 4650 50  0000 C CNN
F 1 "24LC256" H 6250 4650 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 6050 4950 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/devicedoc/21203m.pdf" H 6050 4950 50  0001 C CNN
	1    6050 4950
	-1   0    0    1   
$EndComp
Text Label 4600 5050 2    50   ~ 0
I2C_SDA
Text Label 4600 4950 2    50   ~ 0
I2C_SCL
Wire Wire Line
	6050 5250 6050 5550
Wire Wire Line
	6600 5050 6600 5250
Connection ~ 6600 5050
Wire Wire Line
	6050 4650 6050 4550
Wire Wire Line
	6050 4550 6600 4550
Wire Wire Line
	6600 4550 6600 4850
Connection ~ 6600 4850
Text Label 4250 6950 2    50   ~ 0
3V3PIC
Wire Wire Line
	3550 8200 3550 8000
Wire Wire Line
	3550 8000 3450 8000
Text Label 3550 8600 2    50   ~ 0
3V3SD
Wire Wire Line
	4650 7400 4650 7300
Wire Wire Line
	3450 7400 4650 7400
Connection ~ 4650 7400
$Comp
L Device:R R22
U 1 1 5FB62F53
P 4650 7150
F 0 "R22" V 4650 7150 50  0000 C CNN
F 1 "100k" V 4750 7150 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 4580 7150 50  0001 C CNN
F 3 "~" H 4650 7150 50  0001 C CNN
	1    4650 7150
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 7800 4500 7800
Wire Wire Line
	4500 7800 4500 6950
Wire Wire Line
	4250 6950 4500 6950
Wire Wire Line
	4500 6950 4650 6950
Connection ~ 4500 6950
Wire Wire Line
	4850 7000 4850 6950
Wire Wire Line
	4850 6950 4650 6950
Connection ~ 4650 6950
Wire Wire Line
	4650 7400 4850 7400
Wire Wire Line
	4850 7400 4850 7300
Wire Wire Line
	4650 7000 4650 6950
$Comp
L Diode:1N4148 D4
U 1 1 612E8ABB
P 4850 7150
F 0 "D4" V 4804 7230 50  0000 L CNN
F 1 "1N4148" V 4895 7230 50  0000 L CNN
F 2 "Diodes_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4850 6975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 4850 7150 50  0001 C CNN
	1    4850 7150
	0    1    1    0   
$EndComp
Text Notes 3600 8700 0    50   ~ 0
RC circuit at LCD RESET pin\n(to emulate software reset)\nRemove LCD EEPROM to \nreduce current consumption\nDiode to acts as short when \n5V is removed for faster discharge\nAdjustable LCD backlight
$Comp
L Diode:1N4148 D2
U 1 1 5FB320B5
P 14400 1250
F 0 "D2" H 14250 1200 50  0000 C CNN
F 1 "1N4148" H 14350 1100 50  0000 C CNN
F 2 "Diodes_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 14400 1075 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 14400 1250 50  0001 C CNN
	1    14400 1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	4650 7900 4650 8100
Connection ~ 4650 7900
Wire Wire Line
	3450 8100 4650 8100
Wire Wire Line
	4650 8100 5000 8100
Connection ~ 4650 8100
Wire Wire Line
	2500 8450 2650 8450
Wire Wire Line
	2500 7600 2500 7350
Connection ~ 2500 7600
$Comp
L Device:R_POT_Small RV3
U 1 1 61C0C8CC
P 3400 8300
F 0 "RV3" H 3340 8346 50  0000 R CNN
F 1 "4k7" H 3340 8255 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3400 8300 50  0001 C CNN
F 3 "~" H 3400 8300 50  0001 C CNN
	1    3400 8300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 8200 3550 8200
Wire Wire Line
	3500 8300 3550 8300
Wire Wire Line
	3550 8300 3550 8600
NoConn ~ 3400 8400
Wire Wire Line
	2100 2050 2100 2000
$Comp
L Device:CP C22
U 1 1 620F0D6A
P 3400 1300
F 0 "C22" H 3250 1450 50  0000 L CNN
F 1 "220uF" V 3450 1000 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 3438 1150 50  0001 C CNN
F 3 "~" H 3400 1300 50  0001 C CNN
	1    3400 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 1550 3400 1450
Wire Wire Line
	3400 1150 3400 1050
Connection ~ 3400 1050
Wire Wire Line
	3400 1050 3600 1050
Connection ~ 2950 1550
Wire Wire Line
	2950 1550 2950 1450
Wire Wire Line
	2950 1550 3400 1550
Wire Wire Line
	10950 2800 11400 2800
Wire Wire Line
	10900 2800 10950 2800
Connection ~ 10950 2800
Wire Wire Line
	10950 2200 10950 2800
Wire Wire Line
	10650 2200 10950 2200
Wire Wire Line
	9850 2700 10300 2700
Wire Wire Line
	10200 2200 10200 2900
Connection ~ 10200 2900
Wire Wire Line
	10200 2900 10300 2900
Wire Wire Line
	9600 4900 10200 4900
$Comp
L Device:R R31
U 1 1 5FFD66EF
P 9450 2500
F 0 "R31" V 9243 2500 50  0000 C CNN
F 1 "4k7" V 9334 2500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 2500 50  0001 C CNN
F 3 "~" H 9450 2500 50  0001 C CNN
	1    9450 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	1650 4100 2100 4100
Text Notes 3550 9450 0    50   ~ 0
Be careful with SDO/SDI \nnaming convention!
$Comp
L power:GND #PWR014
U 1 1 5FE1FE34
P 8450 11000
F 0 "#PWR014" H 8450 10750 50  0001 C CNN
F 1 "GND" H 8455 10827 50  0000 C CNN
F 2 "" H 8450 11000 50  0001 C CNN
F 3 "" H 8450 11000 50  0001 C CNN
	1    8450 11000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J8
U 1 1 5FE62B9C
P 1350 2250
F 0 "J8" H 1450 2100 50  0000 C CNN
F 1 "CONN_MCLR" V 1300 2250 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1350 2250 50  0001 C CNN
F 3 "~" H 1350 2250 50  0001 C CNN
	1    1350 2250
	1    0    0    -1  
$EndComp
Text Label 1550 2250 0    50   ~ 0
3V3PIC
$Comp
L power:GND #PWR02
U 1 1 5FF0B413
P 1600 2350
F 0 "#PWR02" H 1600 2100 50  0001 C CNN
F 1 "GND" H 1605 2177 50  0000 C CNN
F 2 "" H 1600 2350 50  0001 C CNN
F 3 "" H 1600 2350 50  0001 C CNN
	1    1600 2350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2100 2350 2100 2400
Wire Wire Line
	1950 2150 1950 2400
Wire Wire Line
	1950 2400 2100 2400
Wire Wire Line
	1550 2150 1950 2150
Connection ~ 2100 2400
Wire Wire Line
	2100 2400 2100 2600
Wire Wire Line
	1550 2350 1600 2350
$Comp
L Device:R_POT RV2
U 1 1 60BDFF3A
P 13100 5500
F 0 "RV2" V 12893 5500 50  0000 C CNN
F 1 "10K(LOG)" V 12984 5500 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 13100 5500 50  0001 C CNN
F 3 "~" H 13100 5500 50  0001 C CNN
	1    13100 5500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR044
U 1 1 60E1F7F1
P 13350 5700
F 0 "#PWR044" H 13350 5450 50  0001 C CNN
F 1 "GND" V 13355 5572 50  0000 R CNN
F 2 "" H 13350 5700 50  0001 C CNN
F 3 "" H 13350 5700 50  0001 C CNN
	1    13350 5700
	1    0    0    -1  
$EndComp
Text Label 12800 5050 2    50   ~ 0
MONO_AUDIO
Wire Wire Line
	13250 5500 13350 5500
$Comp
L Connector:NS8002 U13
U 1 1 60648482
P 14250 5550
F 0 "U13" H 14400 5965 50  0000 C CNN
F 1 "NS8002" H 14400 5874 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 14250 5550 50  0001 C CNN
F 3 "" H 14250 5550 50  0001 C CNN
	1    14250 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	13350 5500 13350 5700
Wire Wire Line
	14050 5600 13900 5600
Wire Wire Line
	13900 5600 13900 5500
Wire Wire Line
	13900 5500 14050 5500
$Comp
L Device:C_Small C46
U 1 1 6077EC3F
P 13650 5500
F 0 "C46" V 13421 5500 50  0000 C CNN
F 1 "1uF" V 13512 5500 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 13650 5500 50  0001 C CNN
F 3 "~" H 13650 5500 50  0001 C CNN
	1    13650 5500
	0    1    1    0   
$EndComp
Wire Wire Line
	13350 5500 13550 5500
Connection ~ 13350 5500
Wire Wire Line
	13750 5500 13900 5500
Connection ~ 13900 5500
Wire Wire Line
	13100 5650 13100 6150
$Comp
L Device:C_Small C38
U 1 1 60B0513D
P 13450 6150
F 0 "C38" V 13300 6150 50  0000 C CNN
F 1 "100nF" V 13600 6150 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 13450 6150 50  0001 C CNN
F 3 "~" H 13450 6150 50  0001 C CNN
	1    13450 6150
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R21
U 1 1 60BEB694
P 13750 6150
F 0 "R21" V 13650 6150 50  0000 C CNN
F 1 "10k" V 13850 6150 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 13750 6150 50  0001 C CNN
F 3 "~" H 13750 6150 50  0001 C CNN
	1    13750 6150
	0    1    1    0   
$EndComp
Wire Wire Line
	13550 6150 13650 6150
Wire Wire Line
	13850 6150 13950 6150
Wire Wire Line
	13950 6150 13950 6000
Wire Wire Line
	13950 5700 14050 5700
Wire Wire Line
	14750 5700 14950 5700
Wire Wire Line
	15050 5700 15050 5600
Wire Wire Line
	15050 5600 15150 5600
Wire Wire Line
	15150 5500 14950 5500
Wire Wire Line
	14950 5500 14950 5400
Wire Wire Line
	14950 5400 14750 5400
$Comp
L power:GND #PWR042
U 1 1 60E165CA
P 14850 5850
F 0 "#PWR042" H 14850 5600 50  0001 C CNN
F 1 "GND" H 14750 5800 50  0000 R CNN
F 2 "" H 14850 5850 50  0001 C CNN
F 3 "" H 14850 5850 50  0001 C CNN
	1    14850 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 5500 14850 5500
Wire Wire Line
	14850 5500 14850 5850
$Comp
L Device:R_Small R48
U 1 1 5FE1111A
P 14400 6000
F 0 "R48" V 14300 6000 50  0000 C CNN
F 1 "47k" V 14500 6000 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 14400 6000 50  0001 C CNN
F 3 "~" H 14400 6000 50  0001 C CNN
	1    14400 6000
	0    1    1    0   
$EndComp
Wire Wire Line
	13950 6000 14300 6000
Connection ~ 13950 6000
Wire Wire Line
	13950 6000 13950 5700
Wire Wire Line
	14950 6000 14950 5700
Wire Wire Line
	14500 6000 14950 6000
Connection ~ 14950 5700
Wire Wire Line
	14950 5700 15050 5700
Text Label 14900 4800 2    50   ~ 0
5VSPK
Wire Wire Line
	14750 5600 14900 5600
Wire Wire Line
	14900 5600 14900 4900
$Comp
L Device:CP_Small C47
U 1 1 6003D3C8
P 15050 5000
F 0 "C47" H 15050 5200 50  0000 C CNN
F 1 "1uF" V 14950 5000 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 15050 5000 50  0001 C CNN
F 3 "~" H 15050 5000 50  0001 C CNN
	1    15050 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15050 4900 14900 4900
Connection ~ 14900 4900
Wire Wire Line
	14900 4900 14900 4800
$Comp
L power:GND #PWR045
U 1 1 601CC183
P 15050 5200
F 0 "#PWR045" H 15050 4950 50  0001 C CNN
F 1 "GND" H 15150 5050 50  0000 R CNN
F 2 "" H 15050 5200 50  0001 C CNN
F 3 "" H 15050 5200 50  0001 C CNN
	1    15050 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	15050 5100 15050 5200
$Comp
L Connector:Conn_01x02_Male J9
U 1 1 610E9D79
P 15350 5600
F 0 "J9" H 15300 5550 50  0000 C CNN
F 1 "SPK_AUD1" H 15300 5400 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 15350 5600 50  0001 C CNN
F 3 "~" H 15350 5600 50  0001 C CNN
	1    15350 5600
	-1   0    0    1   
$EndComp
$Comp
L Device:CP_Small C48
U 1 1 60454707
P 15250 5000
F 0 "C48" H 15250 5200 50  0000 C CNN
F 1 "470uF" H 15250 4850 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 15250 5000 50  0001 C CNN
F 3 "~" H 15250 5000 50  0001 C CNN
	1    15250 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15050 4900 15250 4900
Connection ~ 15050 4900
Connection ~ 15050 5100
Wire Wire Line
	15050 5100 15250 5100
Wire Wire Line
	13100 6150 13350 6150
Text Label 13900 1250 2    50   ~ 0
HP_STATE
Wire Wire Line
	13900 1850 13900 1250
Connection ~ 13900 1850
Wire Wire Line
	13900 1850 14100 1850
Text Label 13800 5050 0    50   ~ 0
AMP_SHDN
Wire Wire Line
	13800 5400 13800 5050
Wire Wire Line
	13800 5400 14050 5400
$Comp
L Device:CP_Small C50
U 1 1 6002A6E0
P 15450 5000
F 0 "C50" H 15450 5200 50  0000 C CNN
F 1 "470uF" H 15500 4850 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 15450 5000 50  0001 C CNN
F 3 "~" H 15450 5000 50  0001 C CNN
	1    15450 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15250 4900 15450 4900
Connection ~ 15250 4900
Wire Wire Line
	15450 5100 15250 5100
Connection ~ 15250 5100
$Comp
L Device:CP_Small C51
U 1 1 6027C500
P 15650 5000
F 0 "C51" H 15650 5200 50  0000 C CNN
F 1 "1uF" V 15550 5000 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 15650 5000 50  0001 C CNN
F 3 "~" H 15650 5000 50  0001 C CNN
	1    15650 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15450 4900 15650 4900
Connection ~ 15450 4900
Wire Wire Line
	15650 5100 15450 5100
Connection ~ 15450 5100
Wire Wire Line
	8600 10450 8450 10450
Wire Wire Line
	15700 1500 15900 1500
Wire Wire Line
	15900 1500 15900 900 
Wire Wire Line
	15900 900  14700 900 
Wire Wire Line
	16000 2000 15700 2000
Wire Wire Line
	14850 2000 14750 2000
Wire Wire Line
	14750 2000 14750 1600
Wire Wire Line
	14750 1600 14850 1600
Wire Wire Line
	14750 1600 14550 1600
Connection ~ 14750 1600
Wire Wire Line
	15700 2100 15850 2100
Wire Wire Line
	15850 2100 15850 1700
Wire Wire Line
	15850 1700 15700 1700
Wire Wire Line
	14600 1900 14600 2250
Wire Wire Line
	14600 2250 15850 2250
Wire Wire Line
	15850 2250 15850 2100
Wire Wire Line
	14600 1900 14850 1900
Connection ~ 15850 2100
NoConn ~ 14850 1500
NoConn ~ 14850 1700
Wire Wire Line
	15700 1900 15900 1900
Wire Wire Line
	15900 1900 15900 1500
Connection ~ 15900 1500
NoConn ~ 15700 1600
NoConn ~ 15700 1800
$Comp
L Connector:NE556 U12
U 1 1 613427A0
P 15050 1600
F 0 "U12" H 15275 1965 50  0000 C CNN
F 1 "NE556" H 15275 1874 50  0000 C CNN
F 2 "Package_SO:SO-14_3.9x8.65mm_P1.27mm" H 15050 1600 50  0001 C CNN
F 3 "" H 15050 1600 50  0001 C CNN
	1    15050 1600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J17
U 1 1 60AF6CAD
P 12900 4750
F 0 "J17" V 12962 4894 50  0000 L CNN
F 1 "ST_EN1" V 13053 4894 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 12900 4750 50  0001 C CNN
F 3 "~" H 12900 4750 50  0001 C CNN
	1    12900 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	12800 5050 12800 4950
Wire Wire Line
	12950 5500 12900 5500
Wire Wire Line
	12900 5500 12900 4950
Text Label 13000 5050 0    50   ~ 0
LEFT_CH
Wire Wire Line
	13000 5050 13000 4950
$Comp
L Device:R_POT RV4
U 1 1 60D3F7C4
P 13250 7300
F 0 "RV4" V 13043 7300 50  0000 C CNN
F 1 "10K(LOG)" V 13134 7300 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 13250 7300 50  0001 C CNN
F 3 "~" H 13250 7300 50  0001 C CNN
	1    13250 7300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR048
U 1 1 60D3F7CA
P 13500 7500
F 0 "#PWR048" H 13500 7250 50  0001 C CNN
F 1 "GND" V 13505 7372 50  0000 R CNN
F 2 "" H 13500 7500 50  0001 C CNN
F 3 "" H 13500 7500 50  0001 C CNN
	1    13500 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	13400 7300 13500 7300
$Comp
L Connector:NS8002 U14
U 1 1 60D3F7D2
P 14400 7350
F 0 "U14" H 14550 7765 50  0000 C CNN
F 1 "NS8002" H 14550 7674 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 14400 7350 50  0001 C CNN
F 3 "" H 14400 7350 50  0001 C CNN
	1    14400 7350
	1    0    0    -1  
$EndComp
Wire Wire Line
	13500 7300 13500 7500
Wire Wire Line
	14200 7400 14050 7400
Wire Wire Line
	14050 7400 14050 7300
Wire Wire Line
	14050 7300 14200 7300
$Comp
L Device:C_Small C53
U 1 1 60D3F7DC
P 13800 7300
F 0 "C53" V 13571 7300 50  0000 C CNN
F 1 "1uF" V 13662 7300 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 13800 7300 50  0001 C CNN
F 3 "~" H 13800 7300 50  0001 C CNN
	1    13800 7300
	0    1    1    0   
$EndComp
Wire Wire Line
	13500 7300 13700 7300
Connection ~ 13500 7300
Wire Wire Line
	13900 7300 14050 7300
Connection ~ 14050 7300
Wire Wire Line
	13250 7450 13250 7950
$Comp
L Device:C_Small C52
U 1 1 60D3F7E7
P 13600 7950
F 0 "C52" V 13450 7950 50  0000 C CNN
F 1 "100nF" V 13750 7950 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 13600 7950 50  0001 C CNN
F 3 "~" H 13600 7950 50  0001 C CNN
	1    13600 7950
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R52
U 1 1 60D3F7ED
P 13900 7950
F 0 "R52" V 13800 7950 50  0000 C CNN
F 1 "10k" V 14000 7950 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 13900 7950 50  0001 C CNN
F 3 "~" H 13900 7950 50  0001 C CNN
	1    13900 7950
	0    1    1    0   
$EndComp
Wire Wire Line
	13700 7950 13800 7950
Wire Wire Line
	14000 7950 14100 7950
Wire Wire Line
	14100 7950 14100 7800
Wire Wire Line
	14100 7500 14200 7500
Wire Wire Line
	14900 7500 15100 7500
Wire Wire Line
	15200 7500 15200 7400
Wire Wire Line
	15200 7400 15300 7400
Wire Wire Line
	15300 7300 15100 7300
Wire Wire Line
	15100 7300 15100 7200
Wire Wire Line
	15100 7200 14900 7200
$Comp
L power:GND #PWR049
U 1 1 60D3F7FD
P 15000 7650
F 0 "#PWR049" H 15000 7400 50  0001 C CNN
F 1 "GND" H 14900 7600 50  0000 R CNN
F 2 "" H 15000 7650 50  0001 C CNN
F 3 "" H 15000 7650 50  0001 C CNN
	1    15000 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	14900 7300 15000 7300
Wire Wire Line
	15000 7300 15000 7650
$Comp
L Device:R_Small R53
U 1 1 60D3F805
P 14550 7800
F 0 "R53" V 14450 7800 50  0000 C CNN
F 1 "47k" V 14650 7800 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 14550 7800 50  0001 C CNN
F 3 "~" H 14550 7800 50  0001 C CNN
	1    14550 7800
	0    1    1    0   
$EndComp
Wire Wire Line
	14100 7800 14450 7800
Connection ~ 14100 7800
Wire Wire Line
	14100 7800 14100 7500
Wire Wire Line
	15100 7800 15100 7500
Wire Wire Line
	14650 7800 15100 7800
Connection ~ 15100 7500
Wire Wire Line
	15100 7500 15200 7500
Wire Wire Line
	14900 7400 15050 7400
Wire Wire Line
	15050 7400 15050 6900
$Comp
L Device:CP_Small C54
U 1 1 60D3F814
P 15200 6800
F 0 "C54" H 15200 7000 50  0000 C CNN
F 1 "1uF" V 15100 6800 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 15200 6800 50  0001 C CNN
F 3 "~" H 15200 6800 50  0001 C CNN
	1    15200 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	15200 6700 15050 6700
Connection ~ 15050 6700
Wire Wire Line
	15050 6700 15050 6600
$Comp
L power:GND #PWR050
U 1 1 60D3F81D
P 15200 7000
F 0 "#PWR050" H 15200 6750 50  0001 C CNN
F 1 "GND" H 15300 6850 50  0000 R CNN
F 2 "" H 15200 7000 50  0001 C CNN
F 3 "" H 15200 7000 50  0001 C CNN
	1    15200 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15200 6900 15200 7000
$Comp
L Connector:Conn_01x02_Male J21
U 1 1 60D3F824
P 15500 7400
F 0 "J21" H 15450 7350 50  0000 C CNN
F 1 "SPK_AUD2" H 15450 7200 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 15500 7400 50  0001 C CNN
F 3 "~" H 15500 7400 50  0001 C CNN
	1    15500 7400
	-1   0    0    1   
$EndComp
$Comp
L Device:CP_Small C55
U 1 1 60D3F82A
P 15400 6800
F 0 "C55" H 15400 7000 50  0000 C CNN
F 1 "470uF" H 15400 6650 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 15400 6800 50  0001 C CNN
F 3 "~" H 15400 6800 50  0001 C CNN
	1    15400 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	15200 6700 15400 6700
Connection ~ 15200 6700
Connection ~ 15200 6900
Wire Wire Line
	15200 6900 15400 6900
Wire Wire Line
	13250 7950 13500 7950
Text Label 13950 6550 0    50   ~ 0
AMP_SHDN
$Comp
L Device:CP_Small C56
U 1 1 60D3F838
P 15600 6800
F 0 "C56" H 15600 7000 50  0000 C CNN
F 1 "470uF" H 15650 6650 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 15600 6800 50  0001 C CNN
F 3 "~" H 15600 6800 50  0001 C CNN
	1    15600 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	15400 6700 15600 6700
Connection ~ 15400 6700
Wire Wire Line
	15600 6900 15400 6900
Connection ~ 15400 6900
$Comp
L Device:CP_Small C57
U 1 1 60D3F842
P 15800 6800
F 0 "C57" H 15800 7000 50  0000 C CNN
F 1 "1uF" V 15700 6800 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 15800 6800 50  0001 C CNN
F 3 "~" H 15800 6800 50  0001 C CNN
	1    15800 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	15600 6700 15800 6700
Connection ~ 15600 6700
Wire Wire Line
	15800 6900 15600 6900
Connection ~ 15600 6900
Wire Wire Line
	13100 7300 13050 7300
Text Label 15050 6600 2    50   ~ 0
5VSPK
Text Label 13050 6950 2    50   ~ 0
RIGHT_CH
$Comp
L Connector:Conn_01x03_Male J20
U 1 1 615FAB39
P 13750 6800
F 0 "J20" H 13858 7081 50  0000 C CNN
F 1 "ST_EN2" V 13650 6800 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 13750 6800 50  0001 C CNN
F 3 "~" H 13750 6800 50  0001 C CNN
	1    13750 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	14200 7200 14100 7200
Wire Wire Line
	14100 7200 14100 6800
Wire Wire Line
	14100 6800 13950 6800
Wire Wire Line
	13950 6900 15050 6900
Connection ~ 15050 6900
Wire Wire Line
	15050 6900 15050 6700
Wire Wire Line
	13950 6700 13950 6550
Wire Wire Line
	13050 7300 13050 6950
Text Notes 14150 9350 0    50   ~ 0
Jumpers to switch between mono & stereo\nIn mono mode, mixed output is fed \nto amp 1 and amp 2 is disabled  (SHDN \nof amp 2 shorted to 5V). In stereo mode, left\nchannel is fed to amp 1, right channel\nto amp 2 and SHDN of both amps are \nconnected to AMP_SHDN.
Text Notes 14100 9800 0    50   ~ 0
NOTE:\nKiCad does not decrease component indices\nafter removal from schematics, only increases \nthem upon addition, otherwise PCB will be wrong.
$Comp
L Device:C_Small C58
U 1 1 5FEDA5BD
P 10350 3650
F 0 "C58" H 10350 3500 50  0000 R CNN
F 1 "100nF" V 10300 3950 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10350 3650 50  0001 C CNN
F 3 "~" H 10350 3650 50  0001 C CNN
	1    10350 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	10350 3750 10350 4100
Connection ~ 10350 4100
Wire Wire Line
	10350 3550 10350 3250
Connection ~ 10350 3250
Wire Wire Line
	10350 3250 10550 3250
Wire Wire Line
	10350 4100 10550 4100
Wire Wire Line
	10550 3750 10550 4100
Connection ~ 10550 4100
Wire Wire Line
	10550 4100 10800 4100
Wire Wire Line
	10550 3550 10550 3250
Connection ~ 10550 3250
Wire Wire Line
	10550 3250 10800 3250
$Comp
L Device:CP_Small C59
U 1 1 5FFFD922
P 10550 3650
F 0 "C59" H 10600 3800 50  0000 C CNN
F 1 "100uF" V 10600 3450 50  0000 C CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 10550 3650 50  0001 C CNN
F 3 "~" H 10550 3650 50  0001 C CNN
	1    10550 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	14350 1600 14350 1750
Wire Wire Line
	13700 1600 14350 1600
Wire Wire Line
	14350 2100 14350 2050
Wire Wire Line
	8600 10350 8450 10350
$Comp
L Device:R R38
U 1 1 60001076
P 9450 5750
F 0 "R38" V 9243 5750 50  0000 C CNN
F 1 "680" V 9334 5750 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9380 5750 50  0001 C CNN
F 3 "~" H 9450 5750 50  0001 C CNN
	1    9450 5750
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 8100 5100 8100
Wire Wire Line
	5100 7700 5100 8100
Connection ~ 5000 8100
Wire Wire Line
	5100 7400 5100 6950
Wire Wire Line
	5100 6950 4850 6950
Connection ~ 4850 6950
Wire Wire Line
	12100 4050 12100 4100
Wire Wire Line
	12200 4100 12100 4100
Connection ~ 12100 4100
Wire Wire Line
	12100 4100 12100 4150
$Comp
L Device:R R54
U 1 1 60DEA2D0
P 12600 3300
F 0 "R54" H 12670 3346 50  0000 L CNN
F 1 "2k2" H 12670 3255 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 12530 3300 50  0001 C CNN
F 3 "~" H 12600 3300 50  0001 C CNN
	1    12600 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	12600 2800 12600 3150
$Comp
L Device:R R55
U 1 1 60E49AA3
P 12600 4150
F 0 "R55" H 12670 4196 50  0000 L CNN
F 1 "2k2" H 12670 4105 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 12530 4150 50  0001 C CNN
F 3 "~" H 12600 4150 50  0001 C CNN
	1    12600 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	12600 3450 12600 4000
Wire Wire Line
	12600 4300 12600 4800
Wire Wire Line
	11800 2800 12600 2800
Wire Wire Line
	11800 4800 12600 4800
Wire Wire Line
	14850 1800 14700 1800
Wire Wire Line
	14700 1800 14700 900 
Connection ~ 14700 900 
$Comp
L Diode:1N4148 D6
U 1 1 617595EC
P 14550 1400
F 0 "D6" V 14400 1350 50  0000 C CNN
F 1 "1N4148" H 14450 1500 50  0000 C CNN
F 2 "Diodes_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 14550 1225 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 14550 1400 50  0001 C CNN
	1    14550 1400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	14550 1600 14550 1550
Wire Wire Line
	14250 1250 14200 1250
Connection ~ 14200 1250
Wire Wire Line
	14200 1250 14200 1200
Wire Wire Line
	14200 900  14700 900 
Wire Wire Line
	14700 900  14700 650 
Wire Wire Line
	6900 8650 7150 8650
Wire Wire Line
	6400 3200 6550 3200
$Comp
L Device:CP C60
U 1 1 623A8D54
P 6550 3350
F 0 "C60" H 6600 3250 50  0000 L CNN
F 1 "100u" H 6350 3250 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 6588 3200 50  0001 C CNN
F 3 "~" H 6550 3350 50  0001 C CNN
	1    6550 3350
	1    0    0    -1  
$EndComp
Connection ~ 6550 3200
Wire Wire Line
	6550 3200 6700 3200
$Comp
L Device:CP C15
U 1 1 62461125
P 5100 7550
F 0 "C15" H 5150 7400 50  0000 L CNN
F 1 "100u" H 4850 7400 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.50mm" H 5138 7400 50  0001 C CNN
F 3 "~" H 5100 7550 50  0001 C CNN
	1    5100 7550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R30
U 1 1 635D3FE6
P 5500 5300
F 0 "R30" H 5570 5346 50  0000 L CNN
F 1 "4k7" H 5570 5255 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 5430 5300 50  0001 C CNN
F 3 "~" H 5500 5300 50  0001 C CNN
	1    5500 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R29
U 1 1 6359CD62
P 5350 5300
F 0 "R29" H 5150 5350 50  0000 L CNN
F 1 "4k7" H 5150 5250 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 5280 5300 50  0001 C CNN
F 3 "~" H 5350 5300 50  0001 C CNN
	1    5350 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 5150 5350 4950
Connection ~ 5350 4950
Wire Wire Line
	5350 4950 5650 4950
Wire Wire Line
	5500 5150 5500 5050
Connection ~ 5500 5050
Wire Wire Line
	5500 5050 5650 5050
Wire Wire Line
	5500 5450 5500 5550
Wire Wire Line
	5500 5550 6050 5550
Connection ~ 6050 5550
Wire Wire Line
	6050 5550 6050 5750
Wire Wire Line
	5350 5450 5350 5550
Wire Wire Line
	5350 5550 5500 5550
Connection ~ 5500 5550
$Comp
L Connector:Conn_01x05_Male J12
U 1 1 603437E3
P 12900 1900
F 0 "J12" H 12700 1900 50  0000 C CNN
F 1 "AUDIO_OUT" V 12800 1900 50  0000 C CNN
F 2 "3.5mm Audio:Tayda_3.5mm_stereo_TRS_jack_A-853" H 12900 1900 50  0001 C CNN
F 3 "~" H 12900 1900 50  0001 C CNN
	1    12900 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 6034624D
P 12800 1450
F 0 "#PWR030" H 12800 1200 50  0001 C CNN
F 1 "GND" H 12805 1277 50  0000 C CNN
F 2 "" H 12800 1450 50  0001 C CNN
F 3 "" H 12800 1450 50  0001 C CNN
	1    12800 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	13100 1700 13100 1450
Wire Wire Line
	13100 1450 12800 1450
Wire Wire Line
	13100 1800 13250 1800
Wire Wire Line
	13250 1800 13250 1300
NoConn ~ 13100 2000
Wire Wire Line
	13100 1900 13350 1900
Wire Wire Line
	13350 1900 13350 1750
Wire Wire Line
	13350 1750 13700 1750
Wire Wire Line
	13100 2100 13650 2100
Wire Wire Line
	13650 2100 13650 1850
$Comp
L Connector:Conn_01x05_Male J11
U 1 1 603CE05C
P 13200 2850
F 0 "J11" V 13100 3100 50  0000 C CNN
F 1 "LINE_IN" V 13100 2850 50  0000 C CNN
F 2 "3.5mm Audio:Tayda_3.5mm_stereo_TRS_jack_A-853" H 13200 2850 50  0001 C CNN
F 3 "~" H 13200 2850 50  0001 C CNN
	1    13200 2850
	1    0    0    -1  
$EndComp
NoConn ~ 13400 2950
Text Label 13600 2750 0    50   ~ 0
LINE_IN_R
Wire Wire Line
	13600 2750 13400 2750
Text Label 13600 2850 0    50   ~ 0
LINE_IN_L
Wire Wire Line
	13600 2850 13400 2850
Text Label 13400 2550 0    50   ~ 0
2V5
Wire Wire Line
	13400 2650 13400 2550
NoConn ~ 13400 3050
$Comp
L power:GND #PWR031
U 1 1 60B1AD2D
P 15450 3000
F 0 "#PWR031" H 15450 2750 50  0001 C CNN
F 1 "GND" H 15455 2827 50  0000 C CNN
F 2 "" H 15450 3000 50  0001 C CNN
F 3 "" H 15450 3000 50  0001 C CNN
	1    15450 3000
	1    0    0    -1  
$EndComp
Text Label 15700 2700 0    50   ~ 0
5VSPK
Wire Wire Line
	15450 2700 15700 2700
$Comp
L Connector:Conn_01x02_Male J14
U 1 1 60B1AD35
P 15250 2700
F 0 "J14" H 15358 2881 50  0000 C CNN
F 1 "BT_PWR" H 15358 2790 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 15250 2700 50  0001 C CNN
F 3 "~" H 15250 2700 50  0001 C CNN
	1    15250 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	15450 2800 15450 3000
Wire Wire Line
	14350 2100 14850 2100
Wire Wire Line
	14350 2100 14100 2100
Wire Wire Line
	14100 2100 14100 2150
Connection ~ 14350 2100
$Comp
L Connector:Conn_01x05_Male J10
U 1 1 60BF755E
P 14200 2850
F 0 "J10" V 14100 3100 50  0000 C CNN
F 1 "BT_IN" V 14100 2850 50  0000 C CNN
F 2 "3.5mm Audio:Tayda_3.5mm_stereo_TRS_jack_A-853" H 14200 2850 50  0001 C CNN
F 3 "~" H 14200 2850 50  0001 C CNN
	1    14200 2850
	1    0    0    -1  
$EndComp
NoConn ~ 14400 2950
Text Label 14600 2750 0    50   ~ 0
BT_AUD_R
Wire Wire Line
	14600 2750 14400 2750
Text Label 14600 2850 0    50   ~ 0
BT_AUD_L
Wire Wire Line
	14600 2850 14400 2850
NoConn ~ 14400 3050
$Comp
L power:GND #PWR021
U 1 1 60CCF8DD
P 14400 2500
F 0 "#PWR021" H 14400 2250 50  0001 C CNN
F 1 "GND" H 14405 2327 50  0000 C CNN
F 2 "" H 14400 2500 50  0001 C CNN
F 3 "" H 14400 2500 50  0001 C CNN
	1    14400 2500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	14400 2650 14400 2500
$Comp
L Connector:Conn_01x05_Male J7
U 1 1 618ADB97
P 7800 8800
F 0 "J7" H 7772 8732 50  0000 R CNN
F 1 "USB" H 7772 8823 50  0000 R CNN
F 2 "Connector_USB_new:USB_A_Molex_105057_Vertical" H 7800 8800 50  0001 C CNN
F 3 "~" H 7800 8800 50  0001 C CNN
	1    7800 8800
	-1   0    0    1   
$EndComp
Text Label 7600 9200 3    50   ~ 0
5VUSB
Wire Wire Line
	7600 9200 7600 9000
Wire Wire Line
	7600 8700 7450 8700
Wire Wire Line
	7450 8700 7450 8650
Wire Wire Line
	7450 8650 7150 8650
Connection ~ 7150 8650
Wire Wire Line
	6550 8900 7450 8900
Wire Wire Line
	7450 8900 7450 8800
Wire Wire Line
	7450 8800 7600 8800
Wire Wire Line
	6550 9000 7500 9000
Wire Wire Line
	7500 9000 7500 8900
Wire Wire Line
	7500 8900 7600 8900
NoConn ~ 7600 8600
Text Label 7450 10250 3    50   ~ 0
3V3SD
$Comp
L Connector:SD_CONN J15
U 1 1 6037495D
P 7550 10350
F 0 "J15" H 7300 9600 50  0000 L CNN
F 1 "SD2" H 7150 10350 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 7550 10350 50  0001 C CNN
F 3 "" H 7550 10350 50  0001 C CNN
	1    7550 10350
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR034
U 1 1 60374963
P 7400 9750
F 0 "#PWR034" H 7400 9500 50  0001 C CNN
F 1 "GND" H 7405 9577 50  0000 C CNN
F 2 "" H 7400 9750 50  0001 C CNN
F 3 "" H 7400 9750 50  0001 C CNN
	1    7400 9750
	0    1    1    0   
$EndComp
Wire Wire Line
	7550 9750 7400 9750
Wire Wire Line
	7550 10250 7450 10250
Wire Wire Line
	6550 9850 7550 9850
Wire Wire Line
	6550 9950 7550 9950
Wire Wire Line
	6550 10050 7550 10050
Wire Wire Line
	6550 10150 7550 10150
$Comp
L power:GND #PWR037
U 1 1 6043DB44
P 9900 10350
F 0 "#PWR037" H 9900 10100 50  0001 C CNN
F 1 "GND" H 9905 10177 50  0000 C CNN
F 2 "" H 9900 10350 50  0001 C CNN
F 3 "" H 9900 10350 50  0001 C CNN
	1    9900 10350
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q2
U 1 1 6043DB4A
P 9800 10150
F 0 "Q2" H 9990 10196 50  0000 L CNN
F 1 "2N3904" H 9990 10105 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10000 10075 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 9800 10150 50  0001 L CNN
	1    9800 10150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R39
U 1 1 604D58CA
P 9350 10150
F 0 "R39" V 9250 10150 50  0000 L CNN
F 1 "1k" V 9500 10100 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 9280 10150 50  0001 C CNN
F 3 "~" H 9350 10150 50  0001 C CNN
	1    9350 10150
	0    1    1    0   
$EndComp
Wire Wire Line
	9600 10150 9500 10150
Wire Wire Line
	8450 10150 9200 10150
Text Label 10150 9750 0    50   ~ 0
5VUSB
$Comp
L Connector:Conn_01x02_Male J18
U 1 1 6065A3E2
P 9700 9750
F 0 "J18" H 9808 9931 50  0000 C CNN
F 1 "BT_PWR" H 9808 9840 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 9700 9750 50  0001 C CNN
F 3 "~" H 9700 9750 50  0001 C CNN
	1    9700 9750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 9950 9900 9850
Wire Wire Line
	9900 9750 10150 9750
Text Notes 9350 11100 0    50   ~ 0
Transistor as a switch to togtgle bluetooth module power.\nShared pins with SD card, Input = 0V (OFF), 5V (ON)\nGND reference when OFF will be 5V, which is OK due to\n1uF capacitor between bluetooth audio output & amplifier.\nSI4735 INT can also be fed to MISO with option for buzzer\nif CH376 handles SD card.
Wire Wire Line
	7800 6000 7500 6000
Text Label 7800 6000 0    50   ~ 0
3V3PIC
Text Label 7800 6300 0    50   ~ 0
LCDRS_U1TX_DHT
Wire Wire Line
	7500 6600 7500 6700
$Comp
L power:GND #PWR020
U 1 1 5FCB2214
P 7500 6700
F 0 "#PWR020" H 7500 6450 50  0001 C CNN
F 1 "GND" H 7505 6527 50  0000 C CNN
F 2 "" H 7500 6700 50  0001 C CNN
F 3 "" H 7500 6700 50  0001 C CNN
	1    7500 6700
	1    0    0    -1  
$EndComp
$Comp
L Sensor:DHT11 U15
U 1 1 5FBB99D1
P 7500 6300
F 0 "U15" H 7256 6346 50  0000 R CNN
F 1 "DHT11" H 7256 6255 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7500 5900 50  0001 C CNN
F 3 "http://akizukidenshi.com/download/ds/aosong/DHT11.pdf" H 7650 6550 50  0001 C CNN
	1    7500 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 4950 4750 4950
Wire Wire Line
	4600 5050 4950 5050
$Comp
L Device:C_Small C21
U 1 1 60449E7D
P 4750 5300
F 0 "C21" H 5050 5250 50  0000 R CNN
F 1 "120pF" H 5100 5350 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4750 5300 50  0001 C CNN
F 3 "~" H 4750 5300 50  0001 C CNN
	1    4750 5300
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C23
U 1 1 604EA077
P 4950 5300
F 0 "C23" H 4900 5200 50  0000 R CNN
F 1 "120pF" V 4850 5600 50  0000 R CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4950 5300 50  0001 C CNN
F 3 "~" H 4950 5300 50  0001 C CNN
	1    4950 5300
	-1   0    0    1   
$EndComp
Wire Wire Line
	4750 5200 4750 4950
Connection ~ 4750 4950
Wire Wire Line
	4750 4950 5350 4950
Wire Wire Line
	4950 5050 4950 5200
Connection ~ 4950 5050
Wire Wire Line
	4950 5050 5500 5050
$Comp
L power:GND #PWR039
U 1 1 6066F9C8
P 4850 5550
F 0 "#PWR039" H 4850 5300 50  0001 C CNN
F 1 "GND" H 4855 5377 50  0000 C CNN
F 2 "" H 4850 5550 50  0001 C CNN
F 3 "" H 4850 5550 50  0001 C CNN
	1    4850 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 5400 4850 5400
Wire Wire Line
	4850 5550 4850 5400
Connection ~ 4850 5400
Wire Wire Line
	4850 5400 4950 5400
$Comp
L Device:R_POT RV5
U 1 1 604FB33A
P 10450 2050
F 0 "RV5" V 10243 2050 50  0000 C CNN
F 1 "4k7" V 10334 2050 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10450 2050 50  0001 C CNN
F 3 "~" H 10450 2050 50  0001 C CNN
	1    10450 2050
	0    1    1    0   
$EndComp
Wire Wire Line
	10200 2200 10450 2200
Wire Wire Line
	10650 2200 10650 2050
Wire Wire Line
	10650 2050 10600 2050
NoConn ~ 10300 2050
$Comp
L Device:R_POT RV6
U 1 1 6068BAF8
P 10550 5650
F 0 "RV6" V 10435 5650 50  0000 C CNN
F 1 "4k7" V 10344 5650 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10550 5650 50  0001 C CNN
F 3 "~" H 10550 5650 50  0001 C CNN
	1    10550 5650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10400 5450 10400 5650
Wire Wire Line
	10200 5450 10400 5450
Wire Wire Line
	10550 5500 10550 5450
Wire Wire Line
	10550 5450 10950 5450
NoConn ~ 10700 5650
$Comp
L Connector:CM1213A-04SO U4
U 1 1 6064CD01
P 7400 7300
F 0 "U4" H 7375 7565 50  0000 C CNN
F 1 "CM1213A" H 7375 7474 50  0000 C CNN
F 2 "SOT-23-6:SOT-23-6" H 7350 7450 50  0001 C CNN
F 3 "" H 7350 7450 50  0001 C CNN
	1    7400 7300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR043
U 1 1 6064F258
P 7350 7800
F 0 "#PWR043" H 7350 7550 50  0001 C CNN
F 1 "GND" H 7355 7627 50  0000 C CNN
F 2 "" H 7350 7800 50  0001 C CNN
F 3 "" H 7350 7800 50  0001 C CNN
	1    7350 7800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 7400 6900 7400
Wire Wire Line
	6900 7400 6900 7750
Wire Wire Line
	6900 7750 7350 7750
Wire Wire Line
	7350 7750 7350 7800
Wire Wire Line
	7350 7750 7800 7750
Wire Wire Line
	7800 7750 7800 7400
Wire Wire Line
	7800 7400 7700 7400
Connection ~ 7350 7750
Text Label 6900 7300 2    50   ~ 0
WHIP_CONN
Wire Wire Line
	6900 7300 7050 7300
Text Notes 7950 7600 0    50   ~ 0
SI4735 has ESD protection on certain \npins (datasheet never says which pins). \nWe use C1213 for better protection.\nSupply pins VP and VN are both grounded \nbased on AN383. Unused channels should \nbe left floating.
$Comp
L RF_AM_FM:Si4735-D60-GU U11
U 1 1 60A2F0F1
P 11350 7250
F 0 "U11" H 11750 8000 50  0000 C CNN
F 1 "Si4735" H 11000 7950 50  0000 C CNN
F 2 "Package_SO:SSOP-24_3.9x8.7mm_P0.635mm" H 11600 6550 50  0001 L CNN
F 3 "http://www.silabs.com/Support%20Documents/TechnicalDocs/Si4730-31-34-35-D60.pdf" H 11400 6250 50  0001 C CNN
	1    11350 7250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR051
U 1 1 60A32655
P 11400 8650
F 0 "#PWR051" H 11400 8400 50  0001 C CNN
F 1 "GND" H 11405 8477 50  0000 C CNN
F 2 "" H 11400 8650 50  0001 C CNN
F 3 "" H 11400 8650 50  0001 C CNN
	1    11400 8650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11150 8050 11250 8050
Wire Wire Line
	11250 8050 11450 8050
Connection ~ 11250 8050
Wire Wire Line
	11450 8050 11550 8050
Connection ~ 11450 8050
Wire Wire Line
	11250 8050 11250 8150
Wire Wire Line
	11250 8150 12050 8150
Wire Wire Line
	12050 8150 12050 7650
Wire Wire Line
	12050 7650 11950 7650
Connection ~ 11250 8150
Text Label 12050 7050 0    50   ~ 0
RADIO_AUD_R
Wire Wire Line
	12050 7050 11950 7050
Text Label 12050 7150 0    50   ~ 0
RADIO_AUD_L
Wire Wire Line
	12050 7150 11950 7150
Text Label 11350 6100 0    50   ~ 0
3V3SD
Wire Wire Line
	11250 6450 11350 6450
Wire Wire Line
	11350 6100 11350 6300
Connection ~ 11350 6450
Wire Wire Line
	11350 6450 11450 6450
$Comp
L Device:C_Small C39
U 1 1 60D6C209
P 12050 6650
F 0 "C39" H 12150 6550 50  0000 C CNN
F 1 "100nF" V 11900 6650 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12050 6650 50  0001 C CNN
F 3 "~" H 12050 6650 50  0001 C CNN
	1    12050 6650
	-1   0    0    1   
$EndComp
Wire Wire Line
	11950 6750 12050 6750
Wire Wire Line
	12050 6300 11350 6300
Connection ~ 11350 6300
Wire Wire Line
	11350 6300 11350 6450
Wire Wire Line
	12050 6300 12050 6550
Wire Wire Line
	12200 6300 12050 6300
Connection ~ 12050 6300
$Comp
L power:GND #PWR052
U 1 1 610C47E6
P 12600 6350
F 0 "#PWR052" H 12600 6100 50  0001 C CNN
F 1 "GND" H 12605 6177 50  0000 C CNN
F 2 "" H 12600 6350 50  0001 C CNN
F 3 "" H 12600 6350 50  0001 C CNN
	1    12600 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	12400 6300 12600 6300
Wire Wire Line
	12600 6300 12600 6350
Wire Wire Line
	12050 6950 11950 6950
Wire Wire Line
	12050 7250 11950 7250
Text Label 10600 7550 2    50   ~ 0
SI_GPO2_INT
Wire Wire Line
	10750 7550 10600 7550
Wire Wire Line
	10750 7650 10600 7650
$Comp
L power:GND #PWR047
U 1 1 612D3644
P 9700 6950
F 0 "#PWR047" H 9700 6700 50  0001 C CNN
F 1 "GND" H 9705 6777 50  0000 C CNN
F 2 "" H 9700 6950 50  0001 C CNN
F 3 "" H 9700 6950 50  0001 C CNN
	1    9700 6950
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y2
U 1 1 61382902
P 10550 8150
F 0 "Y2" H 10550 8350 50  0000 C CNN
F 1 "32768" H 10550 8000 50  0000 C CNN
F 2 "Crystals:Crystal_Round_d2.0mm_Vertical" H 10550 8150 50  0001 C CNN
F 3 "~" H 10550 8150 50  0001 C CNN
	1    10550 8150
	-1   0    0    1   
$EndComp
Wire Wire Line
	10750 6950 10450 6950
$Comp
L Device:C C32
U 1 1 6153918F
P 10400 8500
F 0 "C32" H 10515 8546 50  0000 L CNN
F 1 "15pF" H 10515 8455 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 10438 8350 50  0001 C CNN
F 3 "~" H 10400 8500 50  0001 C CNN
	1    10400 8500
	-1   0    0    1   
$EndComp
$Comp
L Device:C C34
U 1 1 615909A5
P 10700 8500
F 0 "C34" H 10450 8550 50  0000 L CNN
F 1 "15pF" H 10400 8450 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 10738 8350 50  0001 C CNN
F 3 "~" H 10700 8500 50  0001 C CNN
	1    10700 8500
	-1   0    0    1   
$EndComp
Wire Wire Line
	10700 8350 10700 8150
Wire Wire Line
	10400 8350 10400 8150
Wire Wire Line
	10400 8650 10700 8650
Wire Wire Line
	11250 8150 11250 8650
Wire Wire Line
	10700 8650 11250 8650
Connection ~ 10700 8650
Wire Wire Line
	9950 8150 9950 7250
Wire Wire Line
	9950 7250 10750 7250
Wire Wire Line
	9950 8150 10400 8150
Connection ~ 10400 8150
Wire Wire Line
	10700 8150 10700 7900
Wire Wire Line
	10700 7900 10000 7900
Wire Wire Line
	10000 7900 10000 7450
Wire Wire Line
	10000 7450 10750 7450
Connection ~ 10700 8150
$Comp
L Device:R_Small R42
U 1 1 6196A35E
P 10450 6500
F 0 "R42" V 10350 6500 50  0000 C CNN
F 1 "100k" V 10550 6500 50  0000 C CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 10450 6500 50  0001 C CNN
F 3 "~" H 10450 6500 50  0001 C CNN
	1    10450 6500
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C33
U 1 1 61B35230
P 10450 6800
F 0 "C33" V 10350 6800 50  0000 C CNN
F 1 "2u2" V 10550 6800 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10450 6800 50  0001 C CNN
F 3 "~" H 10450 6800 50  0001 C CNN
	1    10450 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 6400 10450 6300
Wire Wire Line
	10450 6300 11350 6300
Wire Wire Line
	10450 6600 10450 6650
Wire Wire Line
	10450 6950 10450 6900
Connection ~ 10450 6950
Wire Wire Line
	10450 6950 9700 6950
Wire Wire Line
	10450 6650 10750 6650
Wire Wire Line
	10750 6650 10750 6750
Connection ~ 10450 6650
Wire Wire Line
	10450 6650 10450 6700
Text Label 10650 7050 2    50   ~ 0
I2C_SDA
Wire Wire Line
	10750 7050 10650 7050
Text Label 10650 7150 2    50   ~ 0
I2C_SCL
Wire Wire Line
	10750 7150 10650 7150
$Comp
L Device:C_Small C45
U 1 1 61FAFFA6
P 12300 7450
F 0 "C45" V 12200 7550 50  0000 C CNN
F 1 "470nF" V 12200 7300 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12300 7450 50  0001 C CNN
F 3 "~" H 12300 7450 50  0001 C CNN
	1    12300 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	11950 7450 12200 7450
$Comp
L Device:C_Small C43
U 1 1 620CCD05
P 12200 7750
F 0 "C43" H 12050 7900 50  0000 C CNN
F 1 "1nF" V 12100 7750 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12200 7750 50  0001 C CNN
F 3 "~" H 12200 7750 50  0001 C CNN
	1    12200 7750
	-1   0    0    1   
$EndComp
Wire Wire Line
	11950 7550 12200 7550
$Comp
L Connector:Conn_01x03_Male J6
U 1 1 62190A1F
P 12500 8450
F 0 "J6" V 12550 8700 50  0000 R CNN
F 1 "ANT_SWITCH" V 12450 8700 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 12500 8450 50  0001 C CNN
F 3 "~" H 12500 8450 50  0001 C CNN
	1    12500 8450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	12200 7650 12200 7550
Wire Wire Line
	12200 7950 12200 7900
$Comp
L Device:L L1
U 1 1 621F6C8F
P 12200 8100
F 0 "L1" H 12253 8146 50  0000 L CNN
F 1 "4u7" H 12253 8055 50  0000 L CNN
F 2 "Resistors_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" H 12200 8100 50  0001 C CNN
F 3 "~" H 12200 8100 50  0001 C CNN
	1    12200 8100
	1    0    0    -1  
$EndComp
Wire Wire Line
	12400 8250 12200 8250
Wire Wire Line
	12500 8250 12500 7700
Wire Wire Line
	12400 7450 12500 7450
$Comp
L Device:L_Core_Ferrite L2
U 1 1 624A260E
P 12850 8250
F 0 "L2" V 12669 8250 50  0000 C CNN
F 1 "AM_ANT" V 12760 8250 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12850 8250 50  0001 C CNN
F 3 "~" H 12850 8250 50  0001 C CNN
	1    12850 8250
	0    1    1    0   
$EndComp
Wire Wire Line
	12700 8250 12600 8250
$Comp
L power:GND #PWR053
U 1 1 62506CD0
P 13150 8250
F 0 "#PWR053" H 13150 8000 50  0001 C CNN
F 1 "GND" H 13155 8077 50  0000 C CNN
F 2 "" H 13150 8250 50  0001 C CNN
F 3 "" H 13150 8250 50  0001 C CNN
	1    13150 8250
	0    -1   -1   0   
$EndComp
$Comp
L Device:CTRIM_DIF C49
U 1 1 6262AFFA
P 12800 7800
F 0 "C49" V 13132 7800 50  0000 C CNN
F 1 "50pF" V 13041 7800 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 12800 7800 50  0001 C CNN
F 3 "~" H 12800 7800 50  0001 C CNN
	1    12800 7800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	12650 7700 12500 7700
Connection ~ 12500 7700
Wire Wire Line
	12500 7700 12500 7450
Wire Wire Line
	12600 8250 12600 7900
Wire Wire Line
	12600 7900 12650 7900
Connection ~ 12600 8250
NoConn ~ 12950 7800
Wire Wire Line
	11400 8650 11250 8650
Connection ~ 11250 8650
Wire Wire Line
	13150 8250 13000 8250
Text Notes 11750 9800 0    50   ~ 0
In AM mode (switch 2-3), whip antenna is disconnected \nand series capacitor is bypassed so ferrite rod acts as AM \nantenna. In SW/FM mode (switch 1-2), ferrite rod and \ncapacitor filters out AM frequenies to avoid interferences.\nThe 4.7uH inductor, used with the SI4735 varactor, acts as \nlow-pass filter with peaking in the SW band. \n\nPins 10,11 of SI4735 marked as NC \nbut must be connected to GND according\nto datasheet. Fixed by running KiCad as\nadmin to modify these pins to PASSIVE\nwhich will accept connections.\n\nWhen ordering with JLCPCB,put notes \nabout solder masks between SI4735 pins.
Text Notes 10250 9400 0    50   ~ 0
Defaults I2C, GPO2 & GPO1 unused:\n\n7-bit I2C address\n/SEN grounded: 0b0010001\n/SEN connected to VCC: 0b1100011\n\nReset done using RC circuit.
NoConn ~ 7700 7300
NoConn ~ 7700 7500
NoConn ~ 7050 7500
Text Label 11900 8300 2    50   ~ 0
WHIP_CONN
Wire Wire Line
	12100 8300 12100 7900
Wire Wire Line
	12100 7900 12200 7900
Connection ~ 12200 7900
Wire Wire Line
	12200 7900 12200 7850
Wire Wire Line
	11900 8300 12100 8300
$Comp
L Connector:Conn_01x01_Male J19
U 1 1 62BFA994
P 11900 8500
F 0 "J19" H 11850 8600 50  0000 C CNN
F 1 "WHIP_ANT" H 11700 8500 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 11900 8500 50  0001 C CNN
F 3 "~" H 11900 8500 50  0001 C CNN
	1    11900 8500
	1    0    0    -1  
$EndComp
Wire Wire Line
	12100 8500 12100 8300
Connection ~ 12100 8300
NoConn ~ 12050 6950
NoConn ~ 12050 7250
Text Label 10600 7650 2    50   ~ 0
SI_GPO1
$Comp
L Connector:Conn_01x02_Male J22
U 1 1 6076DB7C
P 10650 9800
F 0 "J22" H 10758 9981 50  0000 C CNN
F 1 "SI4735_EXT" H 10758 9890 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10650 9800 50  0001 C CNN
F 3 "~" H 10650 9800 50  0001 C CNN
	1    10650 9800
	1    0    0    -1  
$EndComp
Text Label 11050 9800 0    50   ~ 0
SI_GPO1
Wire Wire Line
	11050 9800 10850 9800
Text Label 11050 9900 0    50   ~ 0
SI_GPO2_INT
Wire Wire Line
	10850 9900 10950 9900
$Comp
L Connector:Conn_01x02_Male J23
U 1 1 608A6BA2
P 10950 10350
F 0 "J23" H 11100 10450 50  0000 C CNN
F 1 "SI_INT" V 10850 10350 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10950 10350 50  0001 C CNN
F 3 "~" H 10950 10350 50  0001 C CNN
	1    10950 10350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10950 10150 10950 9900
Connection ~ 10950 9900
Wire Wire Line
	10950 9900 11050 9900
Text Label 11150 10150 0    50   ~ 0
SD_MISO
Wire Wire Line
	11150 10150 11050 10150
$Comp
L Device:C_Small C44
U 1 1 60F6E5D2
P 12300 6300
F 0 "C44" V 12350 6200 50  0000 C CNN
F 1 "100nF" V 12350 6450 50  0000 C CNN
F 2 "Capacitors_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 12300 6300 50  0001 C CNN
F 3 "~" H 12300 6300 50  0001 C CNN
	1    12300 6300
	0    1    1    0   
$EndComp
Text Notes 10300 6150 0    50   ~ 0
SI4735 RADIO MODULE
Wire Wire Line
	8450 10550 8450 11000
Wire Wire Line
	8600 10250 8450 10250
$EndSCHEMATC
