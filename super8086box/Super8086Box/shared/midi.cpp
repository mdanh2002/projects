#include "midi.h"
#include <stdio.h>     /* for printf() function */

// MIDI device interface for sending MIDI output
HMIDIOUT device;

// whether joystick 1 and 2 is attached
bool bDev1Attached = false;
bool bDev2Attached = false;

// select which MIDI output port to open. 0 to use default
int midiport = 0;
int getMidiPort()
{
	return midiport;
}
void setMidiPort(int port)
{
	midiport = port;
}

unsigned char maxJoyStickCount = 0;
unsigned char getMaxJoystickCount()
{
	return maxJoyStickCount;
}
void setMaxJoystickCount(unsigned char count) {
	maxJoyStickCount = 0;
}

void initJoystick(HWND hwnd) {
	JOYINFO joyinfo;
	UINT wNumDevs;

	if (maxJoyStickCount > 0)
	{
		wNumDevs = joyGetNumDevs();
		if (wNumDevs > 0) {
			MMRESULT joy1res = joyGetPos(JOYSTICKID1, &joyinfo);
			if (joy1res != MMSYSERR_NOERROR)
			{
				printf("Error accessing joystick 1: %d\n", joy1res);
				setMaxJoystickCount(0);
			}
			else {
				joy1res = joySetCapture(hwnd, JOYSTICKID1, NULL, FALSE);
				if (joy1res != MMSYSERR_NOERROR)
				{
					printf("Error capturing joystick 1: %d\n", joy1res);
					setMaxJoystickCount(0);
				}
				else {
					bDev1Attached = true;

					printf("Joystick 1 enabled\n");
				}
			}

			if (wNumDevs > 1 && maxJoyStickCount > 1)
			{
				MMRESULT joy2res = joyGetPos(JOYSTICKID2, &joyinfo);
				if (joy2res != MMSYSERR_NOERROR)
				{
					printf("Error accessing joystick 2: %d\n", joy2res);
					setMaxJoystickCount(0);
				}
				else {
					joy2res = joySetCapture(hwnd, JOYSTICKID2, NULL, FALSE);
					if (joy2res != MMSYSERR_NOERROR)
					{
						printf("Error capturing joystick 2: %d\n", joy2res);
						setMaxJoystickCount(0);
					}
					else {
						bDev2Attached = true;

						printf("Joystick 2 enabled\n");
					}
				}
			}
		}
		else {
			printf("No joysticks available for emulation\n");
			setMaxJoystickCount(0);
		}
	}
	else {		
		printf("Joystick emulation disabled\n");
	}
}


void closeJoystick(HWND hwnd) {
	if (bDev1Attached)
	{
		MMRESULT joy1res = joyReleaseCapture(JOYSTICKID1);
		if (joy1res != MMSYSERR_NOERROR)
		{
			printf("Error closing joystick 1: %d\n", joy1res);
		}
		else {
			bDev1Attached = false;
		}
	}

	if (bDev2Attached)
	{
		MMRESULT joy2res = joyReleaseCapture(JOYSTICKID2);
		if (joy2res != MMSYSERR_NOERROR)
		{
			printf("Error closing joystick 2: %d\n", joy2res);
		}
		else {
			bDev2Attached = false;
		}
	}
}

bool isJoystick1Installed() {
	return bDev1Attached;
}

bool isJoystick2Installed() {
	return bDev2Attached;
}

void initMidi() {
	if (midiport < 0)
	{
		printf("MIDI is disabled\r\n");
		return;
	}

	int flag = midiOutOpen(&device, midiport, 0, 0, CALLBACK_NULL);
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutOpen error: %d for device: %d\r\n", flag, midiport);
	}
	else {
		printf("Successfully opened MIDI device: %d\r\n", midiport);
	}
}

void sendMidiMsg(unsigned long word) {
	if (midiport < 0)
	{
		printf("MIDI is disabled\r\n");
		return;
	}

	MMRESULT flag = midiOutShortMsg(device, word);
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutShortMsg error: %d\n", flag);
	}
}

void sendMidiLongMsg(unsigned char bytes[], int length) {
	if (midiport < 0)
	{
		printf("MIDI is disabled\r\n");
		return;
	}

	MIDIHDR     midiHdr;

	midiHdr.lpData = (LPSTR)&bytes[0];
	midiHdr.dwBufferLength = length;
	midiHdr.dwFlags = 0;

	MMRESULT flag = midiOutPrepareHeader((HMIDIOUT)device, &midiHdr, sizeof(MIDIHDR));
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutPrepareHeader error: %d\n", flag);
		return;
	}

	flag = midiOutLongMsg((HMIDIOUT)device, &midiHdr, sizeof(MIDIHDR));
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutLongMsg error: %d\n", flag);
		return;
	}

	int count = 0;
	int countMax = 5000;
	while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader((HMIDIOUT)device, &midiHdr, sizeof(MIDIHDR)) && count < countMax)
	{
		Sleep(1);
		count++;
	}
	if (count >= countMax)
	{
		printf("Timeout waiting for midiOutUnprepareHeader\r\n");
	}
}

void closeMidi() {
	if (midiport < 0)
	{
		printf("MIDI is disabled\r\n");
		return;
	}

	// turn any MIDI notes currently playing:
	MMRESULT flag = midiOutReset(device);
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutReset error: %d\r\n", flag);
	}
	else {
		printf("midiOutReset OK");
	}


	// Remove any data in MIDI device and close the MIDI Output port
	flag = midiOutClose(device);
	if (flag != MMSYSERR_NOERROR) {
		printf("midiOutClose error: %d\r\n", flag);
	}
	else {
		printf("midiOutClose OK");
	}
}

void printMidiDevices()
{
	UINT nMidiDeviceNum;
	MIDIOUTCAPS caps;

	nMidiDeviceNum = midiOutGetNumDevs();
	if (nMidiDeviceNum == 0) {
		printf("midiInGetNumDevs() return 0 devices\r\n");
		return;
	}

	printf("MIDI output devices:\r\n");
	for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
		MMRESULT flag = midiOutGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
		if (flag != MMSYSERR_NOERROR) {
			printf("%d - midiOutGetDevCaps error %d\n", i, flag);
		}
		else
			printf("%d - %s\n", i, caps.szPname);
	}
	printf("\r\n");
}

bool midiCfgRead(FILE *fp) {
	char Line[256];
	int tmp;

	fgets(Line, 256, fp);
	if (strncmp(Line, "[MIDI_DEVICE]", 13) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);

	fgets(Line, 256, fp);
	if (strncmp(Line, "[JOYSTICK_DEVICES]", 18) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);

	maxJoyStickCount = tmp;
	

	return true;
}

unsigned char getExpectedParamCount(unsigned char midiCmd) {
	// get the expected parameter count for a MIDI command byte
	// return -1 if midiCmd is not a valid command byte
	// see http://fmslogo.sourceforge.net/manual/midi-table.html for details
	if (midiCmd < 0b10000000)
	{
		return 0; // MIDI data byte, invalid param
	}
	else if (midiCmd < 192) {
		return 2;
	}
	else if (midiCmd < 224) {
		return 1;
	}
	else if (midiCmd < 240) {
		return 2;
	}
	else if (midiCmd == 241) {
		return 0; 	// SysEx message handled separately, not included here
	}
	else if (midiCmd == 242) {
		return 2;
	}
	else if (midiCmd == 243) {
		return 1;
	}
	else { // >=244
		return 0;
	}
}