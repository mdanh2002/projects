// =============================================================================
// File: win32_8086tiny_interface.cpp
//
// Description:
// Win32 implementation of the 8086tiny interface class.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include "8086tiny_interface.h"
#include "resource.h"

#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include <ctype.h>

#include <cmath>
#include <time.h>

#include "serial_emulation.h"
#include "file_dialog.h"

#include "win32_cga.h"
#include "win32_serial_cfg.h"
#include "win32_debug.h"
#include "win32_cpu_speed_dialog.h"
#include "win32_sound_cfg.h"
#include "win32_snd_drv.h"
#include "midi.h"
#include "adlib.h"

#include "ne2000.h"
#include "pcap_helper.h"

#define CONFIG_FILE_NAME "default.cfg"
#define APP_VERSION	"1.0.1"

/*  Make the class name into a global variable  */
char szClassName[ ] = "8086TinyWindowsApp";

/* The app instance */
static HINSTANCE MyInstance;

/* This is the handle for our window */
static HWND hwndMain;
#define WIN_FLAGS (WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU)
static int CurrentDispW = 0;
static int CurrentDispH = 0;
static int WindowScaling = 1;
static TextDisplay_t WindowTextDisplay = TD_VGA_8x16;
static bool FullScreen = false;
static bool FullScreenFit = false;
static int fullScreenXAdjust = 0;
static int fullScreenYAdjust = 0;
static int fullScreenMethod = 0;
static bool needRefreshWindow = false;
static HMENU appMenu = NULL;

// emulation state control flags
static bool EmulationExitFlag = false;
static bool ResetPending = false;
static bool FDImageChanged = false;

// disk and bios image file names
static char BiosFilename[1024];
static char HDFilename[1024];
static char FDFilename[1024];
static char ZIPFilename[1024];

int CPU_Clock_Hz = 4770000;
const int PIT_Clock_Hz = 1193181;

int CPU_Counter = 0;
int CPU_Frame = 0;
int PIT_Counter = 0;

// timing control variables
DWORD INT8_PERIOD_MS = 55;
static DWORD NextInt8Time = 0;

#define MPU401_INT_NO 0x0A		 // for IRQ 2, cascaded to IRQ 9
static int NE2000IRQPending = 0; // for NE2000
static int MPU401IRQPending = 0; // for MT32


static int Int8Pending = 0; // for timer software interrupt

static DWORD NextSlowdownTime = 0;

// Mouse state variables

static bool HaveCapture = false;
static bool MouseLButtonDown = false;
static bool MouseRButtonDown = false;

static bool lastPosSet = false;
static int lx = 0;
static int ly = 0;

// Sound emulation data
int CurrentSampleRate;
CWaveOut *WaveOut;
CWaveOut *WaveOutDisney;
CWaveOut *WaveOutAdlib;

static bool SpkrData = false;
static bool SpkrT2Gate = false;
static bool SpkrT2Out = false;
static bool SpkrT2US = false;  // Is T2 rate ultrasonic? Some games use this
                               // instead of silence!

static short SndBuffer[2048];
static int SndBufferLen = 0;
static int SND_Counter = 0;

// joystick emulation variables
UINT joy1xPos = 0;
UINT joy1yPos = 0;
bool joy1but1Pressed = 0;
bool joy1but2Pressed = 0;

UINT joy2xPos = 0;
UINT joy2yPos = 0;
bool joy2but1Pressed = 0;
bool joy2but2Pressed = 0;

unsigned long long joystickLastTickReset = 0;
unsigned long long joystickTicksCount = 0;

// NE2000 Emulation
#define MAC_ADDRESS_LEN 6
uint8_t mac_addr[MAC_ADDRESS_LEN];
bool NE2000Enabled = false;
bool isMacProvided = false;
NE2000State* NE2000Adapter;

unsigned char SelectedNetworkDeviceIndex = 0;

HANDLE hThreadReceiveNetworkBytes = INVALID_HANDLE_VALUE;
DWORD threadIDReceiveNetworkBytes = 0;

// in 16-bit mode, in ax, dx / out dx, ax will translate to 2 successive calls to ReadPort/WritePort
// we need to emulate the behaviour appropriately
uint16_t lastNE2000WordRead = 0;
unsigned char lastNE2000ByteWritten = 0;

// =============================================================================
// File Handling
//
int file_exists(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
}

void showConfigError() {
	MessageBox(hwndMain, "Invalid configuration detected. Check console log for any error messages. App will now quit.", "Configuration Error", MB_OK);
	exit(1);
}


// =============================================================================
// PIC 8259 stuff
//

int PIC_OCW_Idx = 0;
unsigned char PIC_OCW[3] = { 0, 0, 0 };
int PIC_ICW_Idx = 0;
unsigned char PIC_ICW[4] = { 0, 0, 0, 0 };

// =============================================================================
// PIT 8253 stuff
//

struct TimerData_t
{
  bool BCD;             // BCD mode
  int Mode;             // Timer mode
  int RLMode;           // Read/Load mode
  int ResetHolding;     // Holding area for timer reset count
  int ResetCount;       // Reload value when count = 0
  int Count;            // Current timer counter
  int Latch;            // Latched timer count: -1 = not latched
  bool LSBToggle;       // Read load LSB (true) /MSB(false) next?
};

const TimerData_t PIT_Channel0Default = { false, 2, 3, 0, 0, 0, -1 , true};
const TimerData_t PIT_Channel1Default = { false, 2, 3, 1024, 1024, 1024, -1, true };
const TimerData_t PIT_Channel2Default = { false, 3, 3, 1024, 1024, 1024, -1, true };


TimerData_t PIT_Channel0 = PIT_Channel0Default;
TimerData_t PIT_Channel1 = PIT_Channel1Default;
TimerData_t PIT_Channel2 = PIT_Channel2Default;

void ResetPIT(void)
{
  PIT_Channel0 = PIT_Channel0Default;
  PIT_Channel1 = PIT_Channel1Default;
  PIT_Channel2 = PIT_Channel2Default;

  INT8_PERIOD_MS = 55;
  NextInt8Time = 0;
}

void PIT_UpdateTimers(int Ticks)
{
  PIT_Channel0.Count -= Ticks;
  while (PIT_Channel0.Count <= 0)
  {
    if (PIT_Channel0.ResetCount == 0)
    {
      PIT_Channel0.Count += 65536;
    }
    else
    {
      PIT_Channel0.Count += PIT_Channel0.ResetCount;
    }

    Int8Pending++;
  }

  // PIT Channel 1 is only used for DRAM refresh.
  // Don't bother update this timer.
  //PIT_Channel1.Count -= Ticks;
  //while (PIT_Channel1.Count <= 0)
  //{
  //  if (PIT_Channel1.ResetCount == 0)
  //  {
  //    PIT_Channel1.Count += 65536;
  //  }
  //  else
  //  {
  //    PIT_Channel1.Count += PIT_Channel1.ResetCount;
  //  }
  //}

  PIT_Channel2.Count -= Ticks;
  if (PIT_Channel2.Mode == 2)
  {
    SpkrT2Out = false;

    if (PIT_Channel2.Count <= 0)
    {
      if (PIT_Channel2.ResetCount == 0)
      {
        PIT_Channel2.Count += 65536;
      }
      else
      {
        PIT_Channel2.Count += PIT_Channel2.ResetCount;
      }
      SpkrT2Out = true;
    }
  }
  else if (PIT_Channel2.Mode == 3)
  {
    if (PIT_Channel2.Count <= 0)
    {
      if (PIT_Channel2.ResetCount == 0)
      {
        PIT_Channel2.Count += 65536;
      }
      else
      {
        PIT_Channel2.Count += PIT_Channel2.ResetCount;
      }
    }
    SpkrT2Out = (PIT_Channel2.Count >= (PIT_Channel2.ResetCount / 2));
  }
}

void PIT_WriteTimer(int T, unsigned char Val)
{
  TimerData_t *Timer;
  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;

  bool WriteLSB = false;

  if (Timer->RLMode == 1)
  {
    WriteLSB = true;
  }
  else if (Timer->RLMode == 3)
  {
    WriteLSB = Timer->LSBToggle;
    Timer->LSBToggle = !Timer->LSBToggle;
  }

  if (WriteLSB)
  {
    Timer->ResetHolding = (Timer->ResetHolding & 0xFF00) | Val;
  }
  else
  {
    Timer->ResetHolding = (Timer->ResetHolding & 0x00FF) | (((int) Val) << 8);
    Timer->ResetCount = Timer->ResetHolding;

    if (Timer->Mode == 0)
    {
      Timer->Count = Timer->ResetCount;
    }

    if (T == 0)
    {
      INT8_PERIOD_MS = (Timer->ResetCount * 1000) / PIT_Clock_Hz;
      if (INT8_PERIOD_MS == 0) INT8_PERIOD_MS = 1;
    }
    else if (T == 2)
    {
      // Is T2 frequency ultrasonic (> 15 kHz)
      SpkrT2US = (Timer->ResetCount < 80);
    }
  }
}

unsigned char PIT_ReadTimer(int T)
{
  TimerData_t *Timer;
  int ReadValue;
  bool ReadLSB = false;
  unsigned char Val;

  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;


  if (Timer->Latch != -1)
  {
    ReadValue = Timer->Latch;
  }
  else
  {
    ReadValue = Timer->Count;
  }

  if (Timer->RLMode == 1)
  {
    ReadLSB = true;
  }
  else if (Timer->RLMode == 3)
  {
    ReadLSB = Timer->LSBToggle;
    Timer->LSBToggle = !Timer->LSBToggle;
  }

  if (ReadLSB)
  {
    Val = (unsigned char)(ReadValue & 0xFF);
  }
  else
  {
    Val = (unsigned char)((ReadValue >> 8) & 0xFF);
    Timer->Latch = -1;
  }

  return Val;
}

void PIT_WriteControl(unsigned char Val)
{
  int T = (Val >> 6) & 0x03;
  TimerData_t *Timer;

  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;

  int RLMode = (Val >> 4) & 0x03;
  if (RLMode == 0)
  {
    Timer->Latch = Timer->Count;
    Timer->LSBToggle = true;
  }
  else
  {
    Timer->RLMode = RLMode;
    if (RLMode == 3) Timer->LSBToggle = true;
  }

  int Mode = (Val >> 1) & 0x07;
  Timer->Mode = Mode;

  Timer->BCD = (Val & 1) == 1;
}


void setFullScreen(bool isFullScreen, bool isFit) {
	CGA_SetFullScreen(isFullScreen);
	CGA_SetFullScreenFit(isFit);
	if (isFullScreen)
	{
		CGA_SetNeedClearDisplaySurface();
		CGA_UpdateFullScreenScaleRatio();

		LONG lStyle = GetWindowLong(hwndMain, GWL_STYLE);
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		SetWindowLong(hwndMain, GWL_STYLE, lStyle);

		appMenu = GetMenu(hwndMain);
		SetMenu(hwndMain, NULL);
	}
	else {
		printf("Back from full screen\n");

		WindowScaling = 1;
		CGA_SetScale(1.0);

		if (appMenu != NULL)
			SetMenu(hwndMain, appMenu);

		LONG lStyle = GetWindowLong(hwndMain, GWL_STYLE);
		lStyle |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		SetWindowLong(hwndMain, GWL_STYLE, lStyle);
	}

	needRefreshWindow = true;
	FullScreen = isFullScreen;
	FullScreenFit = isFit;
}

// =============================================================================
// Keyboard stuff
//

#define KEYBUFFER_LEN 64
static int KeyBufferHead = 0;
static int KeyBufferTail = 0;
static int KeyBufferCount = 0;
static unsigned char KeyBuffer[KEYBUFFER_LEN];

static unsigned char KeyInputBuffer = 0;
static bool KeyInputFull = false;

// Convert windows virtual key to set 1 scan code for alpha keys
static unsigned char VKAlphaToSet1[26] =
{
  0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, // 'A' .. 'J'
  0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, // 'K' .. 'T'
  0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C                          // 'U' .. 'Z'
};

// Convert windows virtual key to set 1 scan code for digit keys
static unsigned char VKDigitToSet1[10] =
{
  0x0B, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A // '0' .. '9'
};

static unsigned int VKtoSet1Code(int vk, bool IsExtended)
{
  if ((vk >= 'A') && (vk <= 'Z'))
  {
    return VKAlphaToSet1[vk-'A'];
  }
  else if ((vk >= '0') && (vk <= '9'))
  {
    return VKDigitToSet1[vk - '0'];
  }
  else if ((vk >= VK_F1) && (vk <= VK_F10))
  {
    return 0x3B + (vk - VK_F1);
  }

  switch (vk)
  {
    case VK_RETURN:
      return 0x1C;

    case VK_ESCAPE:
      return 0x01;

    case VK_SPACE:
      return 0x39;

    case VK_SHIFT:
      if (IsExtended)
        return 0x36;
      else
        return 0x2A;
    case VK_RSHIFT:
      return 0x36;
    case VK_CONTROL:
      return 0x1D;
    case VK_RCONTROL:
      return 0x1D;

    case VK_MENU: // Alt
      return 0x38;

    case VK_LEFT:
      return 0x4B;
    case VK_UP:
      return 0x48;
    case VK_RIGHT:
      return 0x4D;
    case VK_DOWN:
      return 0x50;

    case VK_BACK:
      return 0x0E;

    case VK_TAB:
      return 0x0F;

    case VK_CAPITAL:
      return 0x3A;
    case VK_NUMLOCK:
      return 0x45;
    case VK_SCROLL:
      return 0x46;

    case VK_NUMPAD0:
    case VK_INSERT:
      return 0x52;
    case VK_NUMPAD1:
    case VK_END:
      return 0x4F;
    case VK_NUMPAD2:
      return 0x50;
    case VK_NUMPAD3:
    case VK_NEXT:
      return 0x51;
    case VK_NUMPAD4:
      return 0x4B;
    case VK_NUMPAD5:
    case VK_CLEAR:
      return 0x4C;
    case VK_NUMPAD6:
      return 0x4D;
    case VK_NUMPAD7:
    case VK_HOME:
      return 0x47;
    case VK_NUMPAD8:
      return 0x48;
    case VK_NUMPAD9:
    case VK_PRIOR:
      return 0x49;
    case VK_MULTIPLY:
      return 0x37;
    case VK_SUBTRACT:
      return 0x4A;
    case VK_ADD:
      return 0x4E;
    case VK_DIVIDE:
      return 0x35;
    case VK_DECIMAL:
    case VK_DELETE:
      return 0x53;

    case VK_OEM_1: // ';'
      return 0x27;

    case VK_OEM_PLUS:
      return 0x0D;

    case VK_OEM_COMMA:
      return 0x33;

    case VK_OEM_MINUS:
      return 0x0C;

    case VK_OEM_PERIOD:
      return 0x34;

    case VK_OEM_2: // '/'
      return 0x35;

    case VK_OEM_3: // '~'
      return 0x29;

    case VK_OEM_4: // '['
      return 0x1A;

    case VK_OEM_5: // '\'
      return 0x2B;

    case VK_OEM_6: // ']'
      return 0x1B;

    case VK_OEM_7: // '''
      return 0x28;

    case VK_F11:

      // F11 is not available on the XT keyboard so use this to test
      // instruction execution (trace on and off)
      return 0x7e;

    case VK_F12:
      // F12 is not available on the XT keyboard
      if (HaveCapture)
      {
        HaveCapture = false;
        ReleaseCapture();
        ShowCursor(TRUE);

		if (FullScreen)
		{
			setFullScreen(false, false);
		}
      }
      return 0xfF;

    default:
      printf("Unhandled key code 0x%02x\n", vk);
      break;

  }

  return 0xFF;
}

static inline void AddKeyEvent(unsigned char code)
{
  if (KeyBufferCount < KEYBUFFER_LEN)
  {
    KeyBuffer[KeyBufferTail] = code;
    KeyBufferTail = (KeyBufferTail + 1) % KEYBUFFER_LEN;
    KeyBufferCount++;
  }
}

static inline  bool IsKeyEventAvailable(void)
{
  return (KeyBufferCount > 0);
}

static inline unsigned char GetKeyEvent(void)
{
  unsigned char code = 0xff;

  if (KeyBufferCount > 0)
  {
    code = KeyBuffer[KeyBufferHead];
  }

  return code;
}

static inline unsigned char NextKeyEvent(void)
{
  unsigned char code = 0xff;

  if (KeyBufferCount > 0)
  {
    code = KeyBuffer[KeyBufferHead];
    KeyBufferHead = (KeyBufferHead + 1) % KEYBUFFER_LEN;
    KeyBufferCount--;
  }

  return code;
}

bool fullScreenCfgRead(FILE *fp)
{
	char Line[256];
	int tmp;

	fgets(Line, 256, fp);
	if (strncmp(Line, "[FULL_SCREEN_ADJUST_X_PIXELS]", 29) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);
	fullScreenXAdjust = tmp;

	fgets(Line, 256, fp);
	if (strncmp(Line, "[FULL_SCREEN_ADJUST_Y_PIXELS]", 29) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);
	fullScreenYAdjust = tmp;

	fgets(Line, 256, fp);
	if (strncmp(Line, "[FULL_SCREEN_METHOD]", 20) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);
	fullScreenMethod = tmp;

	printf("fullScreenXAdjust=%d fullScreenYAdjust=%d fullScreenMethod=%d\n", fullScreenXAdjust, fullScreenYAdjust, fullScreenMethod);

	return true;
}

int ReadConfig(const char *Filename)
{
  FILE *fp;
  char Line[1024];
  int len;

  fp = fopen(Filename, "r");
  if (fp == NULL)
  {
    /*
     * .cfg file doesn't exist yet
     */
    return 0;
  }

  // Read BIOS image file
  fgets(Line, 256, fp);
  if (strncmp(Line, "[BIOS]", 6) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    BiosFilename[0] = 0;
  }
  else
  {
    strncpy(BiosFilename, Line, 1024);
  }

  // Read floppy image name
  fgets(Line, 256, fp);
  if (strncmp(Line, "[FD]", 4) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    FDFilename[0] = 0;
  }
  else
  {
    strncpy(FDFilename, Line, 1024);
  }

  // Read HD image name
  fgets(Line, 256, fp);
  if (strncmp(Line, "[HD]", 4) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    HDFilename[0] = 0;
  }
  else
  {
    strncpy(HDFilename, Line, 1024);
  }

  // Read CPU speed
  fgets(Line, 256, fp);
  if (strncmp(Line, "[CPU_SPEED]", 11) != 0) return 0;

  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &CPU_Clock_Hz);

  // Read NE2000 Mac config
  fgets(Line, 256, fp);
  if (strncmp(Line, "[NE2000_MAC]", 12) != 0) return 0;

  fgets(Line, 256, fp);
  if (strlen(Line) > 0)
  {
	  if (strlen(Line) >= 16)
	  {
		  // specify a valid mac address e.g. "00:0d:3f:cd:02:5f" to enable NE2000 emulation
		  if (sscanf(Line, "%x:%x:%x:%x:%x:%x",
			  &mac_addr[0],
			  &mac_addr[1],
			  &mac_addr[2],
			  &mac_addr[3],
			  &mac_addr[4],
			  &mac_addr[5]) >= MAC_ADDRESS_LEN)
		  {
			  isMacProvided = true;
		  }
		  else {
			  printf("Could not parse provided MAC address.\n");
			  isMacProvided = false;
		  }

		  NE2000Enabled = true;
	  }
	  else if (Line[0] == '1') {
		  // specified 1 for random Mac address
		  NE2000Enabled = true;
	  }
	  else {
		  // specified other values to disable NE2000 emulation
		  NE2000Enabled = false;
	  }
  }
  else {
	  NE2000Enabled = false;
  }

  // Read Network Device Device Index 
  fgets(Line, 256, fp);
  if (strncmp(Line, "[NETWORK_DEVICE_INDEX]", 22) != 0) return 0;

  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &SelectedNetworkDeviceIndex);

  // Read serial port configuration.
  SERIAL_ReadConfig(fp);

  // Read sound configuration
  SNDCFG_Read(fp);

  // Read midi config
  midiCfgRead(fp);

  // Read adlib config
  adlibCfgRead(fp);


  // Pixels adjust for full screen mode
  fullScreenCfgRead(fp);  
  CGA_SetFullScreenAdjust(fullScreenXAdjust, fullScreenYAdjust);
  CGA_SetFullScreenMethod(fullScreenMethod);

  fclose(fp);

  return 1;
}

// No longer used - new to add new config sections
/*
int WriteConfigFile(const char *Filename)
{
  FILE *fp = fopen(Filename, "wt");
  if (fp == NULL) return 0;

  fprintf(fp, "[BIOS]\n");
  fprintf(fp, "%s\n", BiosFilename[0] ? BiosFilename : "NIL");
  fprintf(fp, "[FD]\n");
  fprintf(fp, "%s\n", FDFilename[0] ? FDFilename : "NIL");
  fprintf(fp, "[HD]\n");
  fprintf(fp, "%s\n", HDFilename[0] ? HDFilename : "NIL");
  fprintf(fp, "[CPU_SPEED]\n");
  fprintf(fp, "%d\n", CPU_Clock_Hz);

  SERIAL_WriteConfig(fp);
  SNDCFG_Write(fp);

  fclose(fp);
  return 1;
}
*/

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  unsigned int KeyCode;
  char Filename[1024];

  switch (message)                  /* handle the messages */
  {
	  // Joystick code added by ToughDev
	case MM_JOY1MOVE:
	case MM_JOY1BUTTONDOWN:
	case MM_JOY1BUTTONUP:
	case MM_JOY2MOVE:
	case MM_JOY2BUTTONDOWN:
	case MM_JOY2BUTTONUP:
	{
		UINT fwButtons = wParam; 
		UINT xPos = LOWORD(lParam);
		UINT yPos = HIWORD(lParam);
		DWORD timeNow = timeGetTime();

		if (message == MM_JOY1MOVE)
		{
			joy1xPos = xPos;
			joy1yPos = yPos;
		}
		else if (message == MM_JOY2MOVE)
		{
			joy2xPos = xPos;
			joy2yPos = yPos;
		}
		else if (message == MM_JOY1BUTTONDOWN)
		{
			/*
			Bit-wise

			JOY_BUTTON1CHG & JOY_BUTTON1 - button 1 changes states
			JOY_BUTTON2CHG & JOY_BUTTON2 - button 2 changes states etc.
			*/
			if (fwButtons & JOY_BUTTON1)
				joy1but1Pressed = true;

			if (fwButtons & JOY_BUTTON2)
				joy1but2Pressed = true;
		}
		else if (message == MM_JOY2BUTTONDOWN)
		{
			if (fwButtons & JOY_BUTTON1)
				joy2but1Pressed = true;

			if (fwButtons & JOY_BUTTON2)
				joy2but2Pressed = true;
		}
		else if (message == MM_JOY1BUTTONUP)
		{
			if (fwButtons & JOY_BUTTON1CHG)
				joy1but1Pressed = false;

			if (fwButtons & JOY_BUTTON2CHG)
				joy1but2Pressed = false;
		}
		else if (message == MM_JOY2BUTTONUP)
		{
			if (fwButtons & JOY_BUTTON1CHG)
				joy2but1Pressed = false;

			if (fwButtons & JOY_BUTTON2CHG)
				joy2but2Pressed = false;
		}

		/*
		printf("message=%d, fwButtons=%d, xPos=%d, yPos=%d, joy1xPos=%d, joy1yPos=%d, joy1but1Pressed=%d, joy1but2Pressed=%d, joy2xPos=%d, joy2yPos=%d, joy2but1Pressed=%d, joy2but2Pressed=%d\n", 
				message, fwButtons, xPos, yPos, joy1xPos, joy1yPos, joy1but1Pressed, joy1but2Pressed, joy2xPos, joy2yPos, joy2but1Pressed, joy2but2Pressed);
		*/

		break;
	}	

  case WM_DESTROY:
	  EmulationExitFlag = true;
	  if (NE2000Enabled)
	  {
		  if (hThreadReceiveNetworkBytes != INVALID_HANDLE_VALUE)
		  {
			  TerminateThread(hThreadReceiveNetworkBytes, 1);
		  }
	  }

	  PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
	  break;

  case WM_INITMENU:
	  if (FullScreen && FullScreenFit)
	  {
		  CheckMenuItem((HMENU) wParam, IDM_SCALE_X1, MF_BYCOMMAND | MF_UNCHECKED);
		  CheckMenuItem((HMENU) wParam, IDM_SCALE_X2, MF_BYCOMMAND | MF_UNCHECKED);
		  CheckMenuItem((HMENU) wParam, IDM_SCALE_FULLSCREEN_ASPECT, MF_BYCOMMAND | MF_CHECKED);
		  CheckMenuItem((HMENU)wParam, IDM_SCALE_FULLSCREEN_STRETCH, MF_BYCOMMAND | MF_UNCHECKED);
	  }
	  else if (FullScreen)
	  {
		  CheckMenuItem((HMENU)wParam, IDM_SCALE_X1, MF_BYCOMMAND | MF_UNCHECKED);
		  CheckMenuItem((HMENU)wParam, IDM_SCALE_X2, MF_BYCOMMAND | MF_UNCHECKED);
		  CheckMenuItem((HMENU)wParam, IDM_SCALE_FULLSCREEN_ASPECT, MF_BYCOMMAND | MF_UNCHECKED);
		  CheckMenuItem((HMENU)wParam, IDM_SCALE_FULLSCREEN_STRETCH, MF_BYCOMMAND | MF_CHECKED);
	  }
      else if (WindowScaling == 1)
      {
        CheckMenuItem((HMENU) wParam, IDM_SCALE_X1, MF_BYCOMMAND | MF_CHECKED);
        CheckMenuItem((HMENU) wParam, IDM_SCALE_X2, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem((HMENU) wParam, IDM_SCALE_FULLSCREEN_ASPECT, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem((HMENU)wParam, IDM_SCALE_FULLSCREEN_STRETCH, MF_BYCOMMAND | MF_UNCHECKED);
      }
      else
      {
        CheckMenuItem((HMENU) wParam, IDM_SCALE_X1, MF_BYCOMMAND | MF_UNCHECKED);
        CheckMenuItem((HMENU) wParam, IDM_SCALE_X2, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem((HMENU) wParam, IDM_SCALE_FULLSCREEN_ASPECT, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem((HMENU)wParam, IDM_SCALE_FULLSCREEN_STRETCH, MF_BYCOMMAND | MF_UNCHECKED);
      }
      if (WindowTextDisplay == TD_CGA)
      {
        CheckMenuItem((HMENU) wParam, IDM_TEXT_CGA, MF_BYCOMMAND | MF_CHECKED);
        CheckMenuItem((HMENU) wParam, IDM_TEXT_VGA_8x16, MF_BYCOMMAND | MF_UNCHECKED);
      }
      else
      {
        CheckMenuItem((HMENU) wParam, IDM_TEXT_CGA, MF_BYCOMMAND | MF_UNCHECKED);
        CheckMenuItem((HMENU) wParam, IDM_TEXT_VGA_8x16, MF_BYCOMMAND | MF_CHECKED);
      }
      break;

  case WM_SYSCOMMAND:
	  // https://stackoverflow.com/questions/9627379/how-to-disable-normal-behaviour-of-alt-key
	  // prevent Alt key from accessing the window menu, as our emulated app will need it
	  // 8088 Emulator menu must be accessed by mouse only
	  if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;
	  return DefWindowProc(hwnd, message, wParam, lParam);

	  break;

    case WM_KEYDOWN:
      KeyCode = VKtoSet1Code(wParam, ((lParam & 0x01000000) != 0));
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent(KeyCode & 0xFF);
      break;

    case WM_KEYUP:
      KeyCode = VKtoSet1Code(wParam, ((lParam & 0x01000000) != 0));
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent((KeyCode & 0xFF) | 0x80);
      break;

    case WM_SYSKEYDOWN:
      KeyCode = VKtoSet1Code(wParam, ((lParam & 0x01000000) != 0));
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent(KeyCode & 0xFF);
      break;

    case WM_SYSKEYUP:
      KeyCode = VKtoSet1Code(wParam, ((lParam & 0x01000000) != 0));
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent((KeyCode & 0xFF) | 0x80);
      break;

    case WM_CAPTURECHANGED:
      if (HaveCapture)
      {
        ShowCursor(TRUE);
        HaveCapture = false;
      }
      break;

    case WM_LBUTTONDOWN:
      if (!HaveCapture)
      {
        SetCapture(hwndMain);
        ShowCursor(FALSE);
        HaveCapture = true;
      }
      MouseLButtonDown = true;
      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;

    case WM_LBUTTONUP:
      MouseLButtonDown = false;
      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;

    case WM_RBUTTONDOWN:
      MouseRButtonDown = true;
      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;

    case WM_RBUTTONUP:
      MouseRButtonDown = false;
      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;

    case WM_COMMAND:
    {
      //  int wNotifyCode = HIWORD(wParam); // notification code
      int wID = LOWORD(wParam);         // item, control, or accelerator identifier
      //HWND hwndCtl = (HWND) lparam;     // handle of control

      switch (wID)
      {
        //
        // File menu
        //
        case IDM_RESET:
			printf("Resetting emulator\n");
          ResetPending = true;
          break;

		case IDM_ABOUT:
		{
			char buf[128];
			sprintf(buf, "Super 8086 Box v%s. Copyright 2023", APP_VERSION);
			MessageBox(hwnd, buf, "About", MB_OK);
		}
			break;

        case IDM_QUIT:
          DestroyWindow(hwnd);
          break;

        case IDM_SET__FD_IMAGE:
          FDImageChanged = OpenFileDialog("Select floppy image...", FDFilename, 1024, "all files\0*.*\0\0");
          break;

        case IDM_SET__HD_IMAGE:
		{
			bool diskChanged = OpenFileDialog("Select hard drive image...", HDFilename, 1024, "all files\0*.*\0\0");
			if (diskChanged)
			{
				MessageBox(hwnd, "Please reboot emulator for changes to take effect", "Error", MB_OK);
			}
		}
          break;


        case IDM_SET__BIOS:
          OpenFileDialog("Select BIOS image...", BiosFilename, 1024, "all files\0*.*\0\0");
          break;

        case IDM_SET_SPEED:
          CPU_SPEED_Dialog(MyInstance, hwnd, CPU_Clock_Hz);
          break;

        case IDM_SCALE_X1:
        {
			if (FullScreen)
				setFullScreen(false, false);
			
          WindowScaling = 1;
          CGA_SetScale(1.0);
          break;
        }

        case IDM_SCALE_X2:
        {
			if (FullScreen)
				setFullScreen(false, false);

          WindowScaling = 2;
          CGA_SetScale(2.0);
          break;
        }

		case IDM_SCALE_FULLSCREEN_ASPECT:
		case IDM_SCALE_FULLSCREEN_STRETCH:
		{
			// https://stackoverflow.com/questions/2382464/win32-full-screen-and-hiding-taskbar
			setFullScreen(true, wID == IDM_SCALE_FULLSCREEN_ASPECT);
		}
		break;

        case IDM_TEXT_CGA:
        {
          WindowTextDisplay = TD_CGA;
          CGA_SetTextDisplay(TD_CGA);
          break;
        }

        case IDM_TEXT_VGA_8x16:
        {
          WindowTextDisplay = TD_VGA_8x16;
          CGA_SetTextDisplay(TD_VGA_8x16);
          break;
        }

        case IDM_SET_SERIAL_PORTS:
          SERIAL_ConfigDialog(MyInstance, hwnd);
          break;

        case IDM_CONFIGURE_SOUND:
          if (SNDCFG_Dialog(MyInstance, hwnd))
          {
            if (CurrentSampleRate != AudioSampleRate)
            {
              delete WaveOut;

              WAVEFORMATEX wfx;
              wfx.cbSize = 0;
              wfx.wFormatTag = WAVE_FORMAT_PCM;
              wfx.nAvgBytesPerSec = AudioSampleRate;
              wfx.nChannels = 1;
              wfx.nSamplesPerSec = AudioSampleRate;
              wfx.wBitsPerSample = 16;
              wfx.nBlockAlign = 2;

              WaveOut = new CWaveOut(&wfx, 64, 1024);

              CurrentSampleRate = AudioSampleRate;
            }
          }
          break;

        case IDM_LOAD_CONFIGURATION:
          if (OpenFileDialog("Load configuration...", Filename, 1024, "configuration file\0*.cfg\0\0"))
          {
            ReadConfig(Filename);
          }
          break;

		  /*
        case IDM_SAVE_CONFIGURATION:
          if (SaveFileDialog("Save configuration...", Filename, 1024, "configuration file\0*.cfg\0\0"))
          {
            WriteConfigFile(Filename);
          }
          break;

        case IDM_SAVE_DEFAULT_CONFIGURTION:
          WriteConfigFile(CONFIG_FILE_NAME);
          break;
		  */

        case IDM_START_DEBUGGER:
          DEBUG_CreateDialog(MyInstance, hwnd);
          break;

		case IDM_SHOW_DATE_TIME:
		{
			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			printf("; ---------------------------\n");
			printf("; %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			printf("; ---------------------------\n");
			break;
		}

		case IDM_CLEAR_CONSOLE:
			// https://mathbits.com/MathBits/CompSci/Introduction/clear.htm
			// Clear the debug output console
			system("CLS");
			break;
      }
    }

    default:                      /* for messages that we don't deal with */
      return DefWindowProc (hwnd, message, wParam, lParam);
  }

  return 0;
}

// =============================================================================
// Interface class.
//

T8086TinyInterface_t::T8086TinyInterface_t()
{
}

T8086TinyInterface_t::~T8086TinyInterface_t()
{
}

void T8086TinyInterface_t::SetInstance(HINSTANCE hInst)
{
  hInstance = hInst;
  MyInstance = hInst;
}


// in milliseconds since Windows starts
// 0 means timer not running
DWORD timer1expiry = 0;
DWORD timer2expiry = 0;
double eps = 1e-05;
bool shouldSendIRQ = false;
unsigned char lastAdlibReadVal = 0;
DWORD lastAdlibAudioOut = 0;

void updateAdlibTimer()
{
	DWORD currentTime = timeGetTime(); // in ms since Windows starts

	if (timer1expiry > 0 && timer1expiry <= currentTime)
	{
		// printf("Current time is %d - Timer 1 overflow\n", currentTime);
		adlibTimerOverflow(OPL_TIMER_1);

		if (shouldSendIRQ)
			Int8Pending++;
	}

	if (timer2expiry > 0 && timer2expiry <= currentTime)
	{
		// printf("Current time is %d - Timer 2 overflow\n", currentTime);
		adlibTimerOverflow(OPL_TIMER_2);

		if (shouldSendIRQ)
			Int8Pending++;
	}
}

// Disney Audio Buffer Queue
// https://codeforwin.org/2018/08/queue-implementation-using-array-in-c.html
#define DISNEY_BUF_MAX 4096
unsigned char disneySndBuf[DISNEY_BUF_MAX];
unsigned int curDisneyBufSize = 0;
unsigned int disneyBufRear = DISNEY_BUF_MAX - 1;   // Initally assumed that rear is at end
unsigned int disneyBufFront = 0;
DWORD lastDisneyAudioOut = 0;
unsigned int unreadStatusByteCount = 0;


#define DISNEY_AUD_BUF_INT 5

bool isDisneyBufFull()
{
	return (curDisneyBufSize == DISNEY_BUF_MAX);
}

void emptyDisneyBuffer()
{
	curDisneyBufSize = 0;
}

bool isDisneyBufEmpty()
{
	return (curDisneyBufSize == 0);
}

bool addByteToDisneyBuf(unsigned char data)
{
	// Queue is full throw Queue out of capacity error.
	if (isDisneyBufFull())
	{
		printf("Disney buffer is full\n");
		return false;
	}

	// Ensure rear never crosses array bounds
	disneyBufRear = (disneyBufRear + 1) % DISNEY_BUF_MAX;

	// Increment queue size
	curDisneyBufSize++;

	// Enqueue new element to queue
	disneySndBuf[disneyBufRear] = data;

	// Successfully enqueued element to queue
	return true;
}

unsigned char getDisneyBufByte()
{
	unsigned char data = 0;

	// Queue is empty, throw Queue underflow error
	if (isDisneyBufEmpty())
	{
		printf("Disney buffer is empty\n");
	}
	else {
		// Dequeue element from queue
		data = disneySndBuf[disneyBufFront];

		// Ensure front never crosses array bounds
		disneyBufFront = (disneyBufFront + 1) % DISNEY_BUF_MAX;

		// Decrease queue size
		curDisneyBufSize--;
	}

	return data;
}

void outputDisneyFrame() {
	DWORD currentTime = timeGetTime(); // in ms since Windows starts
	unsigned int timeLapsed = currentTime - lastDisneyAudioOut;

	if (timeLapsed > DISNEY_AUD_BUF_INT && curDisneyBufSize > 0)
	{
		if (curDisneyBufSize > 2 * DISNEY_BUF_MAX / 3)
		{
			printf("Skipping previous audio frames due to huge buffer size: %d\n", curDisneyBufSize);
			emptyDisneyBuffer();
		}
		else {
			unsigned int audBufSize = 0;
			double samplingRate = 0;
			double multiplier = 0;
			unsigned int bytesToPlay = 0;

			if (getLptPortType() == LPT_SOUND_TYPE_DISNEY)
			{
				// Disney sound source has an internal buffer
				// and always plays audio at a fixed rate of 7kHz
				samplingRate = 7000;
				bytesToPlay = min(timeLapsed * samplingRate / 1000, curDisneyBufSize);
			}
			else {
				// For Covox Speech Thing, we output whatever in audio buffer 
				// Try to calculate the raw sampling rate of the original audio (e.g. 8kHz) and upscale the samples
				// to match the sound card sampling rate (e.g. 44.1kHz)
				samplingRate = (double)curDisneyBufSize * 1000 / (double)timeLapsed;

				// for small set of samples, estimated sampling rate is not accurate. We assume lowest
				samplingRate = max(samplingRate, 8000);				

				bytesToPlay = curDisneyBufSize;
			}

			// samplingRate = samplingRate * 1.1;

			multiplier = (double)AudioSampleRate / (double)samplingRate;			
			audBufSize = multiplier * bytesToPlay;

			// crude proportional up-sample algorithm
			// not accurate but it's ok since the original Covox does not have very good audio quality
			// better option: http://www.mega-nerd.com/SRC/ 
			INT8* orgAudioBuf = (INT8*)malloc(curDisneyBufSize);
			INT8* newAudioBuf = (INT8*)malloc(audBufSize);
			unsigned int start = 0;
			while (!isDisneyBufEmpty())
			{
				unsigned char bufOut = getDisneyBufByte();
				orgAudioBuf[start] = bufOut;
				start++;
			}

			for (unsigned int i = 0; i < audBufSize; i++)
			{
				unsigned int oldInd = (unsigned int)(i / multiplier);
				newAudioBuf[i] = orgAudioBuf[oldInd];
			}

			// printf("lapsed: %d. audBufSize: %d, rawRate: %.0fHz, multiplier: %.4f, remainBytes: %d\n", timeLapsed, audBufSize, samplingRate, multiplier, curDisneyBufSize);

			WaveOutDisney->Write((PBYTE)newAudioBuf, audBufSize);
			free(orgAudioBuf);
			free(newAudioBuf);
		}
		lastDisneyAudioOut = currentTime;
	}
}

void outputAdlibFrame() {
	DWORD currentTime = timeGetTime(); // in ms since Windows starts
	unsigned int timeLapsed = currentTime - lastAdlibAudioOut;
	if (timeLapsed > 0)
	{
		// generate only the most recent samples, otherwise if 8088 execution pauses, for example because of an opened menu, 
		// the time lapsed will be huge causing the number of sample to grow very large and cWaveOut->Write to take a long time to write
		// the effect is cascading due to the way the adlib timer is implemented (based on system clock, not CPU clock)
		// the best way to avoid this is to implement the Adlib timer based on CPU clock, which will cause static in the audio output
		// since there are gaps in the emulated CPU running sequence (for the code to process other stuff).
		// This can be observed in the existing PC speaker emulation code by playing a continuous sound - there will be gaps in the tone
		// However, because PC speaker quality is already poor, the result is still acceptable, which is definitely not the case with Adlib emulation.
		// The value chosen should be much larger than the number of actual samples generated between two successive 8088 timer ticks
		// otherwise the audio will still be choppy		
		timeLapsed = min(timeLapsed, 50); // play only the last few ms to prevent buffer overflow

		unsigned int numOfSamples = timeLapsed * AudioSampleRate / 1000;
		lastAdlibAudioOut = currentTime;

		// printf("Samples needed: %d.\n", numOfSamples);
		
		INT16* buf = (INT16*)malloc(numOfSamples * 2);
		updateAdlibFrame(numOfSamples, buf);
		WaveOutAdlib->Write((PBYTE)buf, numOfSamples * 2);
		free(buf);
		
	}
}

void TimerHandler_3812(int channel, double interval_Sec) {
	DWORD currentTime = timeGetTime();
	// printf("TimerHandler_3812 channel=%d interval_Sec=%.20f currentTime=%d\n", channel, interval_Sec, currentTime);

	if (channel == 0)
	{
		if (abs(interval_Sec) > eps)
		{
			timer1expiry = currentTime + interval_Sec * 1000;
		}
		else
		{
			timer1expiry = 0;
		}
	}
	else
	{
		if (abs(interval_Sec) > eps)
		{
			timer2expiry = currentTime + interval_Sec * 1000;
		}
		else
		{
			timer2expiry = 0;
		}
	}
}

void IRQHandler_3812(int param, int irq) {
	// printf("IRQHandler_3812 param=%d irq=%d\n", param, irq);
	shouldSendIRQ = irq; // 0 to disable IRQ, 1 to enable IRQ
}

INT16* buffer;

void UpdateHandler_3812(int param, int min_interval_us) {
	printf("UpdateHandler_3812 param=%d min_interval_us=%d\n", param, min_interval_us);
	// updateAdlibFrame(0, buffer);
}

void NE2000ModeChangeHandler(bool isIn16Bit) {
}

void NE2000IRQHandler()
{
	NE2000IRQPending++;
}

void NE2000SendHandler(unsigned char* data, uint16_t count)
{
	/*
	for (int i = 0; i < count; i++)
		printf("0x%02x ", data[i]);

	printf("\n");
	*/
	
	sendPcapPacket(data, count);
}

void PCapDataReceive(const unsigned char* data, unsigned int count)
{
	/*
	for (int i = 0; i < count; i++)
		printf("0x%02x ", data[i]);

	printf("\n");
	*/

	ne2000_receive(NE2000Adapter, data, count);
}

DWORD WINAPI ThreadReceiveNetworkBytes(LPVOID lpParam)
{
	PCapReceiveThread();
	return 0;
}

void setupNetworkReceiveThread() {
	hThreadReceiveNetworkBytes = CreateThread(NULL, // security attributes ( default if NULL )
		0, // stack SIZE default if 0
		ThreadReceiveNetworkBytes, // Start Address
		NULL, // input data
		0, // creational flag ( start if  0 )
		&threadIDReceiveNetworkBytes); // thread ID

	if (hThreadReceiveNetworkBytes)
	{
		printf("Receive network traffic Thread ID: %d\n", threadIDReceiveNetworkBytes);
	}
	else {
		printf("Failed to create thread to receive network traffic\n");
	}
}

void prepareNetworkSetup() {

	NE2000Adapter = new NE2000State();

	if (!isMacProvided)
	{
		printf("Using randomized MAC address\n");
		NE2000Adapter->macaddr[0] = 0x00;
		srand(timeGetTime());
		for (int i = 0; i < 6; i++)
		{
			NE2000Adapter->macaddr[i] = rand() % 256; // beteen 00 and FF
		}

		// https://www.geeksforgeeks.org/introduction-of-mac-address-in-computer-network/
		// for byte 0 of Mac address
		// bit 0: 0 = unicast, 1 = nulticast
		// bit 1: 0 = globally unique, 1 = locally administrator
		// We set to 00 (unicast, locally admin)
		NE2000Adapter->macaddr[0] = NE2000Adapter->macaddr[0] & 0b11111100;
	}
	else {
		memcpy(NE2000Adapter->macaddr, mac_addr, MAC_ADDRESS_LEN);
	}

	printf("NE2000 MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		NE2000Adapter->macaddr[0],
		NE2000Adapter->macaddr[1],
		NE2000Adapter->macaddr[2],
		NE2000Adapter->macaddr[3],
		NE2000Adapter->macaddr[4],
		NE2000Adapter->macaddr[5]);

	ne2000_reset(NE2000Adapter);
	NE2000SetIRQHandler(NE2000IRQHandler);
	NE2000SetSendHandler(NE2000SendHandler);
	NE2000SetModeChangeHandler(NE2000ModeChangeHandler);

	bool isOk = setupPcapDevice(SelectedNetworkDeviceIndex, mac_addr);
	if (isOk)
	{
		setupNetworkReceiveThread();
		PCapSetReceiveHandler(PCapDataReceive);
	}
}

bool T8086TinyInterface_t::Initialise(unsigned char *mem_in, unsigned char *ports_in)
{
  AllocConsole();
  freopen("CONOUT$", "wb", stdout);

  printf("Super 8086 Box v%s started\n", APP_VERSION);

  COORD coord;
  coord.X = 80;
  coord.Y = 30000;

  HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleScreenBufferSize(Handle, coord);

  // Store a pointer to system memory
  mem = mem_in;
  ports = ports_in;

  // Initialise ports
  for (int i = 0 ; i < 65536 ; i++)
  {
    Port[i] = 0xff;
  }

  WNDCLASSEX wincl;        /* Data structure for the windowclass */

  /* The Window structure */
  wincl.hInstance = hInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
  wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
  wincl.cbSize = sizeof (WNDCLASSEX);

  /* Use default icon and mouse-pointer */
  wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;                 /* No menu */
  wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
  wincl.cbWndExtra = 0;                      /* structure or the window instance */
  /* Use Windows's default colour as the background of the window */
  // wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

  // black color as window background
  HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
  wincl.hbrBackground = (HBRUSH)brush;

  /* Register the window class, and if it fails quit the program */
  if (!RegisterClassEx (&wincl)) return false;

  CurrentDispW = 640;
  CurrentDispH = 400;
  RECT wrect = { 0, 0, CurrentDispW, CurrentDispH };
  AdjustWindowRect(&wrect, WIN_FLAGS, TRUE);
  int width = wrect.right - wrect.left;
  int height = wrect.bottom - wrect.top;

  /* The class is registered, let's create the program*/
  hwndMain = CreateWindowEx (
          0,                   /* Extended possibilities for variation */
          szClassName,         /* Classname */
          "Super 8086 Box",         /* Title Text */
          WIN_FLAGS,           /* window flags */
          CW_USEDEFAULT,       /* Windows decides the position */
          CW_USEDEFAULT,       /* where the window ends up on the screen */
          width,               /* The programs width */
          height,              /* and height in pixels */
          HWND_DESKTOP,        /* The window is a child-window to desktop */
          (HMENU) LoadMenu(hInstance, (LPCSTR) IDR_MENU1),           /* Main menu */
          hInstance,           /* Program Instance handler */
          NULL                 /* No Window Creation data */
          );

  /* Make the window visible on the screen */
  ShowWindow (hwndMain, SW_SHOW);

  timeBeginPeriod(1);

  CGA_Initialise();
  SERIAL_Initialise();
  DEBUG_Initialise(mem_in, ports, sizeof(Port)); // ports is a pointer, Port is an array. Port stores output only, ports stores both output/input

  if (!file_exists(CONFIG_FILE_NAME))
  {
	  printf("Configuration file not found: " CONFIG_FILE_NAME "\n");
	  showConfigError();
  }
  else {
	  ReadConfig(CONFIG_FILE_NAME);
  }

  // original sound init code
  WAVEFORMATEX wfx;
  wfx.cbSize = 0;
  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.nAvgBytesPerSec = AudioSampleRate;
  wfx.nChannels = 1;
  wfx.nSamplesPerSec = AudioSampleRate;
  wfx.wBitsPerSample = 16;
  wfx.nBlockAlign = 2;

  WaveOut = new CWaveOut(&wfx, 64, 1024);
  CurrentSampleRate = AudioSampleRate;

  // Adlib init code, added by MD
  WAVEFORMATEX wfx2;
  wfx2.cbSize = 0;
  wfx2.wFormatTag = WAVE_FORMAT_PCM;
  wfx2.nAvgBytesPerSec = AudioSampleRate;
  wfx2.nChannels = 1;
  wfx2.nSamplesPerSec = AudioSampleRate;
  wfx2.wBitsPerSample = 16;
  wfx2.nBlockAlign = 2;
  WaveOutAdlib = new CWaveOut(&wfx2, 64, 1024);

  // Disney init code, added by MD
  WAVEFORMATEX wfx3;
  wfx3.cbSize = 0;
  wfx3.wFormatTag = WAVE_FORMAT_PCM;
  wfx3.nAvgBytesPerSec = AudioSampleRate;
  wfx3.nChannels = 1;
  wfx3.nSamplesPerSec = AudioSampleRate;
  wfx3.wBitsPerSample = 8;
  wfx3.nBlockAlign = 1;
  WaveOutDisney = new CWaveOut(&wfx3, 64, 1024);

  if (getMaxJoystickCount() > 0)
  {
	  initJoystick(hwndMain);
  }
  else {
	  printf("Joystick emulation is disabled\n");
  }

  if (NE2000Enabled)
  {	  
	  prepareNetworkSetup();
  }
  else {
	  printf("NE2000 emulation disabled\n");
  }

  initAdlib(AudioSampleRate);
  setAdlibTimerHandler(TimerHandler_3812);
  setAdlibIRQHandler(IRQHandler_3812);
  setAdlibUpdateHandler(UpdateHandler_3812);
  lastAdlibAudioOut = timeGetTime();

  return true;
}

void T8086TinyInterface_t::Cleanup(void)
{
  delete WaveOut;
  delete WaveOutAdlib;
  delete WaveOutDisney;

  if (NE2000Enabled)
  {
	  delete NE2000Adapter;
	  closePcapDevice();
  }

  closeAdlib();

  if (getMaxJoystickCount() > 0)
  {
	  closeJoystick(hwndMain);
  }

  timeEndPeriod(1);

  CGA_Cleanup();
  SERIAL_Cleanup();
}

bool T8086TinyInterface_t::ExitEmulation(void)
{
  return EmulationExitFlag;
}

bool T8086TinyInterface_t::Reset(void)
{
  if (ResetPending)
  {
    CPU_Counter = 0;
    CPU_Frame = 0;
    PIT_Counter = 0;

    // Reset keyboard
    KeyBufferHead = 0;
    KeyBufferTail = 0;
    KeyBufferCount = 0;
    KeyInputBuffer = 0;
    KeyInputFull = false;

    // Reset sound emulation
    SpkrData = false;
    SpkrT2Gate = false;
    SpkrT2Out = false;
    SpkrT2US = false;
    SndBufferLen = 0;
    SND_Counter = 0;

    CGA_Reset();
    SERIAL_Reset();
    ResetPIT();
    Int8Pending = 0;
    ResetPending = false;

    for (int i = 0 ; i < 4 ; i++) PIC_ICW[i] = 0;
    for (int i = 0 ; i < 3 ; i++) PIC_OCW[i] = 0;
    PIC_ICW_Idx = 0;
    PIC_OCW_Idx = 0;

    return true;
  }
  return false;
}

char *T8086TinyInterface_t::GetBIOSFilename(void)
{
  if (BiosFilename[0] == 0)
  {
	  printf("BiosFilename is empty\n");
      return NULL;
  }

  if (!file_exists(BiosFilename))
  {
	  printf("BiosFilename not found: %s\n", BiosFilename);
	  showConfigError();
  }

  printf("BiosFilename=%s\n", BiosFilename);


  return BiosFilename;
}

char *T8086TinyInterface_t::GetFDImageFilename(void)
{
  FDImageChanged = false;
  if (FDFilename[0] == 0)
  {
	printf("FDFilename is empty\n");
    return NULL;
  }

  if (!file_exists(FDFilename))
  {
	  printf("FDFilename not found: %s\n", FDFilename);
	  showConfigError();
  }

  printf("FDFilename=%s\n", FDFilename);
  return FDFilename;
}

char *T8086TinyInterface_t::GetHDImageFilename(void)
{
  if (HDFilename[0] == 0)
  {
	printf("HDFilename is empty\n");
    return NULL;
  }

  if (!file_exists(HDFilename))
  {
	  printf("HDFilename not found: %s\n", HDFilename);
	  showConfigError();
  }

  printf("HDFilename=%s\n", HDFilename);
  return HDFilename;
}

bool T8086TinyInterface_t::FDChanged(void)
{
  return FDImageChanged;
}

bool T8086TinyInterface_t::TimerTick(int nTicks)
{
	int PIT_Ticks;
	MSG messages;

	bool NextVideoFrame = false;

	joystickTicksCount += nTicks;

  if (getAdlibEnabled())
  {
	  updateAdlibTimer();
	  outputAdlibFrame();
  }

  int lptPortType = getLptPortType();
  if (lptPortType > 0) {
	  outputDisneyFrame();
  }

  // TODO:
  // This works OK for small values of nTicks, however, the following
  // processing will break down horribly if nTicks is ever larger ( > about 10)
  // If I ever get nTicks per instruction then this will need to change to
  // a loop. Probably safe to process 4 ticks per loop.

  // Update PIT

  PIT_Counter = PIT_Counter + PIT_Clock_Hz * nTicks;
  PIT_Ticks = PIT_Counter / CPU_Clock_Hz;
  PIT_Counter = PIT_Counter % CPU_Clock_Hz;

  PIT_UpdateTimers(PIT_Ticks);

  // Update sound output
  if (SoundEnabled)
  {
    int SoundTicks;
    SND_Counter = SND_Counter + AudioSampleRate * nTicks;
    SoundTicks = SND_Counter / CPU_Clock_Hz;
    SND_Counter = SND_Counter % CPU_Clock_Hz;
    for (int i = 0 ; i < SoundTicks ; i++)
    {
      if (SpkrT2Gate)
      {
        if (SpkrT2US)
        {
          SndBuffer[SndBufferLen] = 0;
        }
        else
        {
          SndBuffer[SndBufferLen] = (SpkrT2Out) ? VolumeSample : -VolumeSample;
        }
      }
      else
      {
        SndBuffer[SndBufferLen] = (SpkrData) ? VolumeSample : 0;
      }
      SndBufferLen+=1;
    }
  }

  // Debug processing

  DebugState_t Dbg = DEBUG_GetState();
  if ((BreakpointCount != 0) && (Dbg == DEBUG_NONE))
  {
    DEBUG_CheckBreak();
    Dbg = DEBUG_GetState();
    if (Dbg != DEBUG_NONE)
    {
      DEBUG_CreateDialog(MyInstance, hwndMain);
    }
  }

  if (Dbg == DEBUG_STOPPED)
  {
    /* Update the debug dialog state */
    DEBUG_Update();

    /* Run the message loop until the debug state is no longer stopped */
    while (GetMessage (&messages, NULL, 0, 0) &&
           (Dbg == DEBUG_STOPPED))
    {
      /* Translate virtual-key messages into character messages */
      TranslateMessage(&messages);
      /* Send message to WindowProcedure */
      DispatchMessage(&messages);

      Dbg = DEBUG_GetState();
    }

    return false;
  }



  // main update processing is every 4 ms of CPU time.

  CPU_Counter += nTicks;
  if (CPU_Counter > (CPU_Clock_Hz / 250))
  {

    CPU_Counter = 0;
    CPU_Frame++;

    if (CPU_Frame == 4)
    {
      if (SoundEnabled)
      {
        WaveOut->Write((PBYTE) SndBuffer, SndBufferLen*2);
        SndBufferLen = 0;
      }

      int w, h;
      CGA_GetDisplaySize(w, h);
      if ((w != CurrentDispW) || (h != CurrentDispH) || needRefreshWindow)
      {
        CurrentDispW = w;
        CurrentDispH = h;

		if (FullScreen)
		{
			SetWindowPos(hwndMain, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
		}
		else {
			RECT wrect = { 0, 0, CurrentDispW, CurrentDispH };
			AdjustWindowRect(&wrect, WIN_FLAGS, TRUE);
			w = wrect.right - wrect.left;
			h = wrect.bottom - wrect.top;
			SetWindowPos(hwndMain, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
		}

		needRefreshWindow = false;
      }

      CGA_DrawScreen(hwndMain, mem);
      NextVideoFrame = true;
      CPU_Frame = 0;

      // Get the mouse position using GetCursorPos.

      POINT cp;
      GetCursorPos(&cp);
      int xPos = cp.x;
      int yPos = cp.y;

      if (lastPosSet)
      {
        int dx = xPos - lx;
        int dy = yPos - ly;
        if ((dx != 0) || (dy != 0))
        {
          SERIAL_MouseMove(dx, dy, MouseLButtonDown, MouseRButtonDown);
        }
      }

      if (HaveCapture)
      {
        static double scale = 1.0;
        RECT wrect;
        GetWindowRect(hwndMain, &wrect);
        lx = (wrect.left + wrect.right) / 2;
        ly = (wrect.top + wrect.bottom) / 2;
        SetCursorPos(lx * scale + 0.5, ly * scale + 0.5);
        GetCursorPos(&cp);
        if ((cp.x != lx) || (cp.y != ly))
        {
          // If the cursor position we get is not what we set then
          // display scaling has occurred.
          // We need to calculate the scaling factor so we can account
          // for it.
          scale = 1.0;
          SetCursorPos(lx * scale + 0.5, ly * scale + 0.5);
          GetCursorPos(&cp);

          scale = ((double) lx) / ((double) cp.x);
          SetCursorPos(lx * scale + 0.5, ly * scale + 0.5);
        }
      }
      else
      {
        lx = xPos;
        ly = yPos;
        lastPosSet = true;
      }

      /* Run the message loop. It will run until PeekMessage() returns 0 */
      while (PeekMessage (&messages, NULL, 0, 0, PM_REMOVE))
      {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
      }
    }

    SERIAL_HandleSerial();

    DWORD CurrentTime = timeGetTime();
    if (CurrentTime >= NextSlowdownTime)
    {
      // No slowdown required
      NextSlowdownTime = CurrentTime + 4;
    }
    else
    {
      Sleep(NextSlowdownTime - CurrentTime);
      NextSlowdownTime += 4;
    }

    if (NextVideoFrame)
    {
      CGA_VBlankStart();
    }
  }

  return NextVideoFrame;
}

void T8086TinyInterface_t::CheckBreakPoints(void)
{
  MSG messages;

  DebugState_t Dbg = DEBUG_GetState();
  if ((BreakpointCount != 0) && (Dbg == DEBUG_NONE))
  {
    DEBUG_CheckBreak();
    Dbg = DEBUG_GetState();
    if (Dbg != DEBUG_NONE)
    {
      DEBUG_CreateDialog(MyInstance, hwndMain);
    }
  }

  if (Dbg == DEBUG_STOPPED)
  {
    /* Update the debug dialog state */
    DEBUG_Update();

    /* Run the message loop until the debug state is no longer stopped */
    while (GetMessage (&messages, NULL, 0, 0) &&
           (Dbg == DEBUG_STOPPED))
    {
      /* Translate virtual-key messages into character messages */
      TranslateMessage(&messages);
      /* Send message to WindowProcedure */
      DispatchMessage(&messages);

      Dbg = DEBUG_GetState();
    }

  }
}

#define mpuCommandReset 0xFF
#define mpuCommandAck  0xFE
#define mpuCommandUART  0x3F
unsigned char lastMpuCommand = 0;
bool shouldSentAck = false;

#define MIDI_BUF_LEN 512
unsigned char midiBuffer[MIDI_BUF_LEN];
int bytesRecv = 0;
bool isInSysEx = false;
unsigned char lastMidiParamCount;
bool isPendingMidiCmd = false;

MIDIHDR     midiHdr;
union { unsigned long word; unsigned char data[4]; } midiMessage;

bool hasReadPortVal = false;
unsigned char lastAdlibAddrPortVal = 0;

void T8086TinyInterface_t::WritePort(int Address, unsigned char Value, bool isWord, bool is2ndByte)
{
	Port[Address] = Value;

	if (CGA_WritePort(Address, Value))
	{
		return;
	}

	if (SERIAL_WritePort(Address, Value))
	{
		return;
	}

	if (NE2000Enabled && Address >= NE2000_ADDR && Address <= NE2000_ADDR + 0x1F)
	{
		int baseAddress = Address - NE2000_ADDR;

		/*
		if (isWord && is2ndByte)
			baseAddress = baseAddress - 1;
		*/

		if (baseAddress >= NE_NOVELL_ASIC_OFFSET)
		{
			if (isWord)
			{
				if (is2ndByte)
				{
					// second (high) byte of 16-bit operation, merge high byte and low byte into word before writing
					uint16_t wordToWrite = lastNE2000ByteWritten | (Value << 8);
					ne2000_asic_ioport_write(NE2000Adapter, baseAddress, wordToWrite);

					// printf("16-bit ne2000_asic_ioport_write second byte. baseAddress=0x%04x. Value=0x%02x. wordToWrite=0x%04x\n", baseAddress, Value, wordToWrite);
				}
				else {
					// first (low) byte of 16-bit operation, store first
					lastNE2000ByteWritten = Value;

					// printf("16-bit ne2000_asic_ioport_write first byte. baseAddress=0x%04x. Value=0x%02x\n", baseAddress, Value);
				}
			}
			else {
				ne2000_asic_ioport_write(NE2000Adapter, baseAddress, Value);

				// printf("8-bit ne2000_asic_ioport_write. baseAddress=0x%04x. Value=0x%02x\n", baseAddress, Value);
			}
		}
		else {
			ne2000_ioport_write(NE2000Adapter, baseAddress, Value);

			// printf("ne2000_ioport_write. baseAddress=0x%04x. Value=0x%02x\n", baseAddress, Value);
		}
	}
	else {
		switch (Address)
		{
		case 0x201: // Write Joystick Port (only to reset timing, actual value not relevant)
		{
			joystickTicksCount = 0;
			joystickLastTickReset = 0;
			// printf("Joystick timing reset for reading\n");
			break;
		}
		case 0x378: // Data Register (Write only for standard parallel port)
		{
			int lptPortType;
			lptPortType = getLptPortType();
			if (lptPortType > 0)
			{
				// Disney Sound Source / Coovox emulation
				addByteToDisneyBuf(Value);
				// printf("Write Data Port, Value=%d, time=%d, bufSize=%d\n", Value, timeGetTime(), curDisneyBufSize);

				unreadStatusByteCount++;
			}

			break;
		}
		case 0x379: // Status Register (Read only)
		{
			break;
		}		
		case 0x37a: // Control Register (Write only)
		{
			int lptPortType;
			lptPortType = getLptPortType();
			// printf("Write Control Port Value=%d\n", Value);
			if (lptPortType > 0 && Value == 0x04 && lastDisneyAudioOut == 0)
			{
				// printf("Disney sound source turned on\n");
				lastDisneyAudioOut = timeGetTime();
			}

			break;
		}
		case 0x37c: // EPP Register
		{
			printf("LPT1 EPP Write: 0x%02X\n", Value);
			break;
		}
		case 0x388:
		{
			if (getAdlibEnabled())
			{
				// printf("Preparing to write to Adlib register %02xh\r\n", Value);
				lastAdlibAddrPortVal = Value;
				hasReadPortVal = false;
			}
		}
		break;

		case 0x389:
		{
			if (getAdlibEnabled())
			{
				// printf("Write Adlib Register %02xh Value %02xh\r\n", lastAdlibAddrPortVal, Value);
				writeAdlib(lastAdlibAddrPortVal, Value);
				hasReadPortVal = false;
			}
		}
		break;


		// PIC Registers
		case 0x330:
		{
			if (getMidiPort() >= 0)
			{
				midiBuffer[bytesRecv] = Value;
				bytesRecv++;

				// printf("Value=%02x bytesRecv=%d\r\n", Value, bytesRecv);

				// http://www.indiana.edu/~emusic/etext/MIDI/chapter3_MIDI9.shtml
				if (Value == 0xF0)
				{
					isInSysEx = true;
				}

				if (isInSysEx)
				{
					if (Value == 0xF7 || Value >= 0xF8 || bytesRecv >= MIDI_BUF_LEN)
					{
						// end of sys ex or MIDI real-time msgs detected
						isInSysEx = false;
						// printf("Finished SysEx. Length = %d\r\n", bytesRecv);

						// if terminated by real-time msg, count this byte as beginning of next buffer
						// else reset buffer
						int sendUntil = 0;
						if (Value == 0xF7)
						{
							sendUntil = bytesRecv;
							bytesRecv = 0;
						}
						else {
							sendUntil = bytesRecv - 1;
							bytesRecv = 1;
						}

						sendMidiLongMsg(midiBuffer, sendUntil);
					}
				}
				else {
					bool shouldSend = false;
					if (Value >= 0b10000000)
					{
						// MIDI command byte, check to see if we should send now (command with no param)
						// or should wait until the correct parameter arrives
						lastMidiParamCount = getExpectedParamCount(Value);
						if (lastMidiParamCount == 0)
						{
							// printf("Sending cmd with no param\r\n");
							midiMessage.data[0] = midiBuffer[0]; // midi Command with no parameters
							midiMessage.data[1] = 0;
							midiMessage.data[2] = 0;
							midiMessage.data[3] = 0;

							shouldSend = true;
						}
						else
						{
							// printf("Not sent yet. Expected param count=%d\r\n", lastMidiParamCount);

							isPendingMidiCmd = true;
							shouldSend = false;
						}
					}
					else {
						if (isPendingMidiCmd)
						{
							if (lastMidiParamCount == 2 && bytesRecv == 3) // 2 parameters
							{
								// printf("Sending 1 cmd byte + 2 param\r\n");
								midiMessage.data[0] = midiBuffer[0];
								midiMessage.data[1] = midiBuffer[1];
								midiMessage.data[2] = midiBuffer[2];
								midiMessage.data[3] = 0;

								shouldSend = true;
								isPendingMidiCmd = false;
							}
							else if (lastMidiParamCount == 1 && bytesRecv == 2) // 1 parameter
							{
								// printf("Sending 1 cmd byte + 1 param\r\n");
								midiMessage.data[0] = midiBuffer[0];
								midiMessage.data[1] = midiBuffer[1];
								midiMessage.data[2] = 0;
								midiMessage.data[3] = 0;

								shouldSend = true;
								isPendingMidiCmd = false;
							}
							else {
								// printf("Not sent yet. Wait for correct param combination\r\n");
							}
						}
						else {
							if (bytesRecv >= 2) // 2 parameters
							{
								// printf("Sending 2 param, no cmd byte\r\n");
								midiMessage.data[0] = midiBuffer[0];
								midiMessage.data[1] = midiBuffer[1];
								midiMessage.data[2] = 0;
								midiMessage.data[3] = 0;

								shouldSend = true;
							}
							else {
								// printf("Not sent yet. Waiting for next param\r\n");

								shouldSend = false;
							}
						}
					}

					if (shouldSend)
					{
						// printf("midiMessage: %02x %02x %02x %02x\r\n", midiMessage.data[0], midiMessage.data[1], midiMessage.data[2], midiMessage.data[3]);
						sendMidiMsg(midiMessage.word);

						for (int i = 0; i < MIDI_BUF_LEN; i++)
							midiBuffer[i] = 0;
						bytesRecv = 0; // reset MIDI buffer
					}
				}
			}

			break;
		}
		case 0x331:
		{
			if (getMidiPort() >= 0)
			{
				// printf("Write MPU401 Command Value: %d\r\n", Value);
				lastMpuCommand = Value;
			}

			break;
		}
		case 0x20:
			if (PIC_OCW_Idx == 0)
			{
				if ((Value & 0x10) != 0)
				{
					PIC_ICW[0] = Value;
					PIC_ICW_Idx = 1;
				}
			}
			else
			{
				PIC_OCW[PIC_OCW_Idx] = Value;
				PIC_OCW_Idx++;
				if (PIC_OCW_Idx > 2) PIC_OCW_Idx = 0;
			}
			break;
		case 0x21:
			if (PIC_ICW_Idx == 0)
			{
				PIC_OCW[0] = Value;
				PIC_OCW_Idx = 1;
			}
			else
			{
				PIC_ICW[PIC_ICW_Idx] = Value;
				PIC_ICW_Idx++;
				if ((PIC_ICW[0] & 0x02) != 0)
				{
					// No ICW3 needed
					if (PIC_ICW_Idx > 1) PIC_ICW_Idx = 0;
				}
				if ((PIC_ICW[0] & 0x01) == 0)
				{
					// No ICW 4 needed
					if (PIC_ICW_Idx > 2) PIC_ICW_Idx = 0;
				}
				if (PIC_ICW_Idx > 3) PIC_ICW_Idx = 0;
			}
			break;

			// PIT Registers
		case 0x40:
			PIT_WriteTimer(0, Value);
			break;
		case 0x41:
			PIT_WriteTimer(1, Value);
			break;
		case 0x42:
			PIT_WriteTimer(2, Value);
			break;
		case 0x43:
			PIT_WriteControl(Value);
			break;

		case 0x61:
			SpkrData = ((Value & 0x02) == 0x02);
			SpkrT2Gate = ((Value & 0x01) == 0x01);
			break;

		default:
			//printf("OUT %04x=%02x\n", Address, Value);
			break;
		}
	}
}

int count = 0;
unsigned char T8086TinyInterface_t::ReadPort(int Address, bool isWord, bool is2ndByte)
{
  // By default return the last value written to the port.
  unsigned char retval = Port[Address];

  if (CGA_ReadPort(Address, retval))
  {
    return retval;
  }

  if (SERIAL_ReadPort(Address, retval))
  {
    return retval;
  }

  if (Address == 0xABCD)
  {
	  count++;
	  return retval;
  }

  if (NE2000Enabled && Address >= NE2000_ADDR && Address <= NE2000_ADDR + 0x1F)
  {
	  int baseAddress = Address - NE2000_ADDR;

	  /*
	  if (isWord && is2ndByte)
		  baseAddress = baseAddress - 1;
	  */

	  if (baseAddress >= NE_NOVELL_ASIC_OFFSET)
	  {
		  if (isWord)
		  {
			  if (is2ndByte)
			  {				  
				  // second byte of 16-bit operation, transfer high byte
				  retval = (lastNE2000WordRead >> 8) & 0xFF;

				  // printf("16-bit ne2000_asic_ioport_read. baseAddress=0x%04x. lastNE2000WordRead=0x%04x. 2ndByte = 0x%02x\n", baseAddress, lastNE2000WordRead, retval);
			  }
			  else {
				  // first byte of 16-bit operation, transfer low byte 
				  lastNE2000WordRead = ne2000_asic_ioport_read(NE2000Adapter, baseAddress);
				  retval = lastNE2000WordRead & 0xFF;

				  // printf("16-bit ne2000_asic_ioport_read. baseAddress=0x%04x. lastNE2000WordRead=0x%04x. 1stByte = 0x%02x\n", baseAddress, lastNE2000WordRead, retval);
			  }
		  }
		  else {
			  retval = ne2000_asic_ioport_read(NE2000Adapter, baseAddress);

			  // printf("8-bit ne2000_asic_ioport_read. baseAddress=0x%04x. retval=0x%02x\n", baseAddress, retval);
		  }
	  }
	  else {
		  retval = ne2000_ioport_read(NE2000Adapter, baseAddress);

		  // printf("ne2000_ioport_read. baseAddress=0x%04x. retval=0x%02x\n", baseAddress, retval);
	  }
  }
  else {
	  // Handle specific processing for ports that do something different.
	  switch (Address)
	  {
	  case 0x201:
	  {
		  if (getMaxJoystickCount() > 0)
		  {
			  /*
			  http://www.fysnet.net/joystick.htm

			  Bit 0 - joystick 1, x coord (0 - timing is active)
			  Bit 1 - joystick 1, y coord (0 - timing is active)
			  Bit 2 - joystick 2, x coord (0 - timing is active)
			  Bit 3 - joystick 2, y coord (0 - timing is active)
			  Bit 4 - joystick 1, button 1 (0 - pressed)
			  Bit 5 - joystick 1, button 2 (0 - pressed)
			  Bit 6 - joystick 2, button 1 (0 - pressed)
			  Bit 7 - joystick 2, button 2 (0 - pressed)

			  Bits 3-0 are resistive inputs with the length of the pulse determined by 0-100k ohm resistive load.
			  How long the bit should remain on depends on the time calculated based on joystick resistance value
			  For Windows API, xPOS from 0 - 65535 maps to 0 - 100K

			  Use the formula
			  time = 24.2µs + (0.011µs * resistance) or resistance = (time - 24.2) / 0.011 [resistance from 0 to 100K]
			  */

			  retval = 0x00;

			  // joystick analog X Y value
			  double timeLapsedSinceLastReset = (double)joystickTicksCount * 1000000 / (double)CPU_Clock_Hz; // in micro sec

			  double joy1XposRVal = (double)joy1xPos * 100000 / 65535;
			  double joy1YposRVal = (double)joy1yPos * 100000 / 65535;
			  double joy1XPulseWidthUs = 24.2 + (0.011 * joy1XposRVal);
			  double joy1YPulseWidthUs = 24.2 + (0.011 * joy1YposRVal);

			  double joy2XposRVal = (double)joy2xPos * 100000 / 65535;
			  double joy2YposRVal = (double)joy2yPos * 100000 / 65535;
			  double joy2XPulseWidthUs = 24.2 + (0.011 * joy2XposRVal);
			  double joy2YPulseWidthUs = 24.2 + (0.011 * joy2YposRVal);

			  if (timeLapsedSinceLastReset < joy1XPulseWidthUs)
			  {
				  retval = retval | 0b00000001;
			  }
			  if (timeLapsedSinceLastReset < joy1YPulseWidthUs)
			  {
				  retval = retval | 0b00000010;
			  }

			  if (timeLapsedSinceLastReset < joy2XPulseWidthUs)
			  {
				  retval = retval | 0b00000100;
			  }
			  if (timeLapsedSinceLastReset < joy2YPulseWidthUs)
			  {
				  retval = retval | 0b00001000;
			  }


			  // joystick button status
			  if (joy1but1Pressed)
				  retval = retval & 0b11101111;
			  else
				  retval = retval | 0b00010000;

			  if (joy1but2Pressed)
				  retval = retval & 0b11011111;
			  else
				  retval = retval | 0b00100000;

			  if (joy2but1Pressed)
				  retval = retval & 0b10111111;
			  else
				  retval = retval | 0b01000000;

			  if (joy2but2Pressed)
				  retval = retval & 0b01111111;
			  else
				  retval = retval | 0b10000000;

			  /*
			  printf("Joystick read. joy1xPos=%d, joy1yPos=%d, joy1PulseWidthMicroSec=%.4f, joy2PulseWidthMicroSec=%.4f, timeLapsedSinceLastReset=%.4f, Value=%d\n",
				  joy1xPos, joy1yPos, joy1XPulseWidthUs, joy1YPulseWidthUs, timeLapsedSinceLastReset, retval);
				*/
		  }

		  break;
	  }

	  case 0x378: // Data Register (Write-only)
	  {
		  printf("Incorrect LPT1 data port read\n");

		  break;
	  }
	  case 0x379: // Status Register (Read-only)
	  {
		  int lptPortType;
		  lptPortType = getLptPortType();

		  /*
		  Some games (Keen Dreams) attempt to detect the Disney Sound Source by doing the following (BASE_PORT = 0x378)

		  1. Send the value 0x04 to BASE_PORT+2 to turn the device on
		  2. Read parallel port ACK status, make sure it's not on
		  3. Send a lot of bytes (~32) to the device (raw value to BASE_PORT followed by control bytes 0x0C to BASE_PORT+2 and 0x04 to BASE_PORT+2)
		  4. Read parallel port ACK status, make sure it's on to indicate buffer overflow
		  5. Send the value 0x0C to BASE_PORT+2 to turn the device off

		  The device is considered detected if ACK is off in step (2) and on in step (4).
		  Our dummy implementation attempts to pass the above detection logic. Control bytes are ignored.
		  */
		  if (lptPortType > 0)
		  {
			  if (unreadStatusByteCount > 16)
			  {
				  retval = 0b01000000;	// ACK ON
			  }
			  else
			  {
				  retval = 0b00000000;	// ACK OFF
			  }

			  // printf("Read Status Port, retVal=%d\n", retval);
			  unreadStatusByteCount = 0;
		  }

		  break;
	  }
	  case 0x37a: // Control Register (Write-only)
	  {
		  break;
	  }

	  case 0x37c: // EPP Register
	  {
		  printf("LPT1 EPP Read\n");
		  break;
	  }

	  case 0x388:
	  {
		  if (getAdlibEnabled())
		  {
			  /*
			  Bit 7 - set if either timer has expired.
			  6 - set if timer 1 has expired.
			  5 - set if timer 2 has expired
			  */
			  retval = adlibGetStatus();

			  if (!hasReadPortVal)
			  {
				  // printf("Read Adlib Status Port. RetVal = %02xh\r\n", retval);
				  hasReadPortVal = true;
			  }
		  }
	  }
	  break;

	  case 0x389:
	  {
		  if (getAdlibEnabled())
		  {
			  // printf("Invalid read from write-only Adlib Data Port!\n");
		  }
	  }
	  break;

	  case 0x330:
	  {
		  if (getMidiPort() >= 0)
		  {
			  /*
			  In Uart mode, the MPU recognizes only 1 command; the command to put it back into Intelligent mode, the Reset command.
			  After you write a command byte to the COMMAND port, the MPU acknowledges this command.
			  It sends an FE (hex) byte to its DATA port
			  */
			  if (shouldSentAck)
			  {
				  retval = mpuCommandAck;
				  shouldSentAck = false;
				  MPU401IRQPending++;
			  }
			  else {
				  retval = 0;
			  }

			  // printf("Read MPU401 Data Port. lastMpuCommand = %d. retval = %d\r\n", lastMpuCommand, retval);
			  lastMpuCommand = 0;
		  }
	  }
	  break;

	  case 0x331:
	  {
		  if (getMidiPort() >= 0)
		  {
			  /*
			  The highest bit (7) of this byte is the state of the DATA SET READY (DSR) line.
			  This bit will be clear (0) if the MPU has some incoming MIDI byte (or Ack byte or Operation byte) waiting for you to read.
			  The DSR line will remain low until you've read all of the bytes that the MPU has waiting in its hardware input buffer.
			  You should continue reading bytes from the DATA port until the DSR bit of the STATUS port toggles to a 1 (ie, sets).
			  When that happens, the MPU has no more bytes waiting to be read. The next highest bit (6) is the state of the DATA READ READY (DRR) line.
			  This bit will be clear whenever it's OK for you to write a byte to the MPU's DATA or COMMAND ports.
			  The MPU sets bit 6 of the STATUS port whenever it is NOT ready for you to write a byte to the DATA or COMMAND ports.
			  */
			  if (lastMpuCommand != 0 && !shouldSentAck)
			  {
				  retval = 0b10000000;
				  shouldSentAck = true;
				  MPU401IRQPending++;
			  }
			  else if (lastMpuCommand != 0 && shouldSentAck)
			  {
				  retval = 0b00000000;
			  }
			  else {
				  retval = 0b10000000;
			  }

			  // printf("Read MPU401 Status Port. lastMpuCommand = %d. RetVal = %d\r\n", lastMpuCommand, retval);
		  }
	  }
	  break;

	  case 0x0020:
		  retval = 0;
		  break;
	  case 0x0021:
		  retval = PIC_OCW[0];
		  break;
	  case 0x0040:
		  retval = PIT_ReadTimer(0);
		  break;
	  case 0x0041:
		  retval = PIT_ReadTimer(1);
		  break;
	  case 0x0042:
		  retval = PIT_ReadTimer(2);
		  break;
	  case 0x0043:
		  break;

	  case 0x0060:
		  retval = KeyInputBuffer;
		  KeyInputFull = false;
		  break;

	  case 0x0064:
		  retval = 0x14;
		  if (KeyInputFull) retval |= 0x01;
		  break;

	  default:
		  //printf("IN %04X\n", Address);
		  break;
	  }
  }

  return retval;
}

unsigned int T8086TinyInterface_t::VMemRead(int i_w, int addr)
{
  return CGA_VMemRead(mem, i_w, addr);
}

unsigned int T8086TinyInterface_t::VMemWrite(int i_w, int addr, unsigned int val)
{
  return CGA_VMemWrite(mem, i_w, addr, val);
}

bool T8086TinyInterface_t::IntPending(int &IntNumber)
{
  if (Int8Pending > 0)
  {
    IntNumber = 8;
    Int8Pending--;
    return true;
  }
  if (NE2000IRQPending > 0)
  {
	  // printf("Raising Hardware Interrupt %d\n", NE2000_IRQ);
	  IntNumber = NE2000_INT_NO;
	  NE2000IRQPending--;
	  return true;
  }
  if (MPU401IRQPending > 0)
  {
	  IntNumber = MPU401_INT_NO;
	  MPU401IRQPending--;
	  return true;
  }

  if (IsKeyEventAvailable() && !KeyInputFull)
  {
    KeyInputBuffer = NextKeyEvent();
    KeyInputFull = true;
    IntNumber = 9;
    return true;
  }

  return SERIAL_IntPending(IntNumber);
}

