#pragma once

#include <windows.h>   /* required before including mmsystem.h */
#include <mmsystem.h>  /* multimedia functions (such as MIDI) for Windows */

#include <stdio.h>

unsigned char getMaxJoystickCount();
void setMaxJoystickCount(unsigned char count);

void initJoystick(HWND hwnd);
void closeJoystick(HWND hwnd);

bool isJoystick1Installed();
bool isJoystick2Installed();

int getMidiPort();
void setMidiPort(int port);
void initMidi();
void sendMidiMsg(unsigned long word);
void sendMidiLongMsg(unsigned char bytes[], int length);
void closeMidi();
void printMidiDevices();
bool midiCfgRead(FILE *fp);
unsigned char getExpectedParamCount(unsigned char midiCmd);