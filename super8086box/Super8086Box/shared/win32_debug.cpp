// =============================================================================
// File: win32_debug.cpp
//
// Description:
// Win32 debug dialog.
// Shows register state, disassembly and memory view.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>
#include <time.h>

#include "win32_debug.h"
#include "file_dialog.h"
#include "resource.h"

#include "debug_disasm.h"

// These must match 8086tiny.

#define RAM_SIZE 0x10FFF0

#define REGS_BASE 0xF0000

// index into regs16
#define REG_AX 0
#define REG_CX 1
#define REG_DX 2
#define REG_BX 3
#define REG_SP 4
#define REG_BP 5
#define REG_SI 6
#define REG_DI 7

#define REG_ES 8
#define REG_CS 9
#define REG_SS 10
#define REG_DS 11

#define REG_IP 14

// index into regs8
#define FLAG_CF 40
#define FLAG_PF 41
#define FLAG_AF 42
#define FLAG_ZF 43
#define FLAG_SF 44
#define FLAG_TF 45
#define FLAG_IF 46
#define FLAG_DF 47
#define FLAG_OF 48

// =============================================================================
// Local variables
//
static HFONT hFont;
static HWND DbgHwnd = 0;
static DebugState_t DbgState = DEBUG_NONE;

unsigned short newIpVal = 0;
bool isPendingIpChange;

static int DisassTabStops[] =
  { 12 };

static int MemoryTabStops[] =
  { 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168, 180, 192, 204, 216, 228 };

static unsigned long port_in_size;

static unsigned char *mem;
static unsigned char *ports;
static unsigned char *regs8;
static unsigned short *regs16;

static int disass_seg = 0;
static int disass_off = 0;

static int showmem_seg = 0;
static int showmem_off = 0;

static HWND ConfigParent = 0;
static bool LastPosSet = false;
static int LastDialogX = 0;
static int LastDialogY = 0;

// Breakpoints
#define DISASS_NUM_LINES 256

static bool BreakPointSet[RAM_SIZE];
static int DisassLineAddr[DISASS_NUM_LINES];

// Instruction trace
#define TRACE_BUFFER_LENGTH 65536

static bool TraceEnabled = false;
static int TraceLength = 0;
static int TracePos = 0;
static int TraceCS[TRACE_BUFFER_LENGTH];
static int TraceIP[TRACE_BUFFER_LENGTH];

// =============================================================================
// Local Functions
//
static void Breakpoint_ClearAll(void)
{
  for (int i = 0 ; i < RAM_SIZE ; i++)
  {
    BreakPointSet[i] = false;
  }

  if (TraceEnabled)
  {
    BreakpointCount = 1;
  }
  else
  {
    BreakpointCount = 0;
  }
}

static void DEBUG_SaveTrace(const char *filename)
{
  FILE *fp;
  int TraceIdx;
  char DASMBuffer[64];
  int dasm_addr;

  fp = fopen(filename, "wt");

  if (fp == NULL) return;

  if (TraceLength < TRACE_BUFFER_LENGTH)
  {
    TraceIdx = 0;
  }
  else
  {
    TraceIdx = TracePos;
  }

  for (int i = 0 ; i < TraceLength ; i++)
  {
    dasm_addr = 16 * TraceCS[TraceIdx] + TraceIP[TraceIdx];

    DasmI386(DASMBuffer, mem, dasm_addr, TraceIP[TraceIdx], false);

    fprintf(fp, "%c\t%04X:%04X   %s\n",
            BreakPointSet[dasm_addr] ? 'B' : ' ',
            TraceCS[TraceIdx], TraceIP[TraceIdx], DASMBuffer);

    TraceIdx = (TraceIdx + 1) % TRACE_BUFFER_LENGTH;
  }

  fclose(fp);
}

#define readable(X) ((X) > 0x20 && (X) < 127 ? (X) : 46)

static void DEBUG_UpdateMemDump(HWND hwnd)
{
	char RegText[16];
	char Buffer[512];

	sprintf(RegText, "%04X:%04X", showmem_seg, showmem_off);
	SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, WM_SETTEXT, 0, (LPARAM)RegText);

	sprintf(RegText, "AX=%04X", regs16[REG_AX]);
	SendDlgItemMessage(hwnd, IDC_SET_REGISTER_EXPR, WM_SETTEXT, 0, (LPARAM)RegText);

	SendDlgItemMessage(hwnd, IDC_LIST_MEMORY, WM_SETFONT, (WPARAM)hFont, TRUE);
	// SendMessage(window, WM_SETFONT, hFont, TRUE);

  unsigned int memaddr = showmem_seg * 16 + showmem_off;

  sprintf(RegText, "%02X", mem[memaddr]);
  SendDlgItemMessage(hwnd, IDC_MEMORY_VALUES, WM_SETTEXT, 0, (LPARAM)RegText);

  for (int i = 0 ; i < 2048 ; i++)
  {
    SendDlgItemMessage(hwnd, IDC_LIST_MEMORY, LB_DELETESTRING, 0, 0);
  }
  // SendDlgItemMessage(hwnd, IDC_LIST_MEMORY, LB_SETTABSTOPS , 16, (LPARAM) MemoryTabStops);
  for (int i = 0 ; i < 2048 ; i+= 16)
  {
	  /*
	  if ( (showmem_off + i) % 0xFFFF == 0xf0)
	  {
		  printf("here");
	  }
	  */

	  strcpy(Buffer, "");

	  sprintf(
		  Buffer,
		  "%04X:%04X   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X   ",
		  showmem_seg,
		  (showmem_off + i) % 0xFFFF,
		  mem[memaddr + i], mem[memaddr + i + 1], mem[memaddr + i + 2], mem[memaddr + i + 3],
		  mem[memaddr + i + 4], mem[memaddr + i + 5], mem[memaddr + i + 6], mem[memaddr + i + 7],
		  mem[memaddr + i + 8], mem[memaddr + i + 9], mem[memaddr + i + 10], mem[memaddr + i + 11],
		  mem[memaddr + i + 12], mem[memaddr + i + 13], mem[memaddr + i + 14], mem[memaddr + i + 15]);

	  // readable will printf space and chars < 32 as '.' so that it can be seen on screen
	  int start = strlen(Buffer);
	  for (int k = 0; k < 16; k++)
	  {
		  sprintf(Buffer, "%s%c", Buffer, readable(mem[memaddr + i + k]));
	  }

    SendDlgItemMessage(hwnd, IDC_LIST_MEMORY, LB_ADDSTRING, 0, (LPARAM) Buffer);
  }
}
static void DEBUG_SaveDisassembly(HWND hwnd)
{
	char DASMBuffer[128];
	char ASMText[128];
	char OpCodes[128];
	unsigned short reg_ip;
	unsigned int instr_size;
	int dasm_addr;
	char fdump[256];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(fdump, "Super8086Box_DisAsm_%04d%02d%02d%02d%02d%02d.asm", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	FILE *pFile = fopen(fdump, "a");
	if (pFile == NULL) {
		MessageBox(hwnd, "Error opening disassembly file", "Error", MB_OK);
		printf("Error opening %s\n", fdump);
		return;
	}

	reg_ip = 0;
	fprintf(pFile, "; CS:IP = %04X:%04X\n", regs16[REG_CS], regs16[REG_IP]);
	fprintf(pFile, "; SS:SP = %04X:%04X\n", regs16[REG_SS], regs16[REG_SP]);

	// unsigned int stackAddr = regs16[REG_SS] * 16 + regs16[REG_SP];
	// unsigned int memVal = mem[stackAddr];
	
	unsigned int disAsmBytes = 0;
	while (disAsmBytes < 32768)
	{
		dasm_addr = 16 * disass_seg + reg_ip;
		
		instr_size = DasmI386(DASMBuffer, mem, dasm_addr, reg_ip, false);
		disAsmBytes += instr_size;

		strcpy(OpCodes, "");
		for (int k = 0; k < instr_size; k++)
		{
			sprintf(OpCodes, "%s%02X", OpCodes, mem[dasm_addr+k]);
		}

		sprintf(ASMText, "%04X:%04X %-16s   %s",
			disass_seg, reg_ip, OpCodes, DASMBuffer);
		reg_ip += instr_size;

		fprintf(pFile, "%s\n", ASMText);

		// procedure separator
		if (strstr(ASMText, "ret"))
			fprintf(pFile, ";;;;;;;;;;;;;;;;;;;\n");
	}

	int res = fclose(pFile);
	if (res != 0)
	{
		printf("Error closing %s\n", fdump);
		MessageBox(hwnd, "Error closing disassembly file", "Error", MB_OK);
	}
	else {
		printf("Diassembly saved to %s. Bytes=%u\n", fdump, disAsmBytes);
		MessageBox(hwnd, "Disassembly file created in executable directory", "Success", MB_OK);
	}
}

unsigned int curInstrLen = 0;
static void DEBUG_UpdateDisassembly(HWND hwnd)
{
	char RegText[16];
	char curIpBytes[16];
	char DASMBuffer[64];
	char ASMText[128];
	unsigned short reg_ip;
	unsigned int instr_size;
	int dasm_addr;

	sprintf(RegText, "%04X:%04X", disass_seg, disass_off);
	SendDlgItemMessage(hwnd, IDC_DISASS_ADDRESS, WM_SETTEXT, 0, (LPARAM)RegText);


	strcpy(curIpBytes, "");

	reg_ip = disass_off;

	for (int i = 0; i < DISASS_NUM_LINES; i++)
	{
		SendDlgItemMessage(hwnd, IDC_LIST_ASM, LB_DELETESTRING, 0, 0);
	}
	SendDlgItemMessage(hwnd, IDC_LIST_ASM, LB_SETTABSTOPS, 1, (LPARAM)DisassTabStops);

	for (int i = 0; i < DISASS_NUM_LINES; i++)
	{
		dasm_addr = 16 * disass_seg + reg_ip;
		DisassLineAddr[i] = dasm_addr;

		instr_size = DasmI386(DASMBuffer, mem, dasm_addr, reg_ip, false);
		if (i == 0)
		{
			curInstrLen = instr_size;

			// bytes for current instructions
			for (int k = 0; k < instr_size; k++)
			{
				sprintf(curIpBytes, "%s%02X", curIpBytes, mem[dasm_addr + k]);
			}
		}

		sprintf(ASMText, "%c\t%04X:%04X [%X]   %s",
			BreakPointSet[dasm_addr] ? 'B' : ' ',
			disass_seg, reg_ip, instr_size, DASMBuffer);
		SendDlgItemMessage(hwnd, IDC_LIST_ASM, LB_ADDSTRING, 0, (LPARAM)ASMText);
		reg_ip += instr_size;
	}

	// SendDlgItemMessage(hwnd, IDC_PATCH_BYTES_ADDR, WM_SETTEXT, 0, (LPARAM)curIpBytes);
}

static unsigned int lastPortAddr = 0x378;
static void DEBUG_UpdateControls(HWND hwnd)
{
  char RegText[32];

  // Update register states
  sprintf(RegText, "%04X", regs16[REG_CS]);
  SendDlgItemMessage(hwnd, IDC_EDIT_CS, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_SS]);
  SendDlgItemMessage(hwnd, IDC_EDIT_SS, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_DS]);
  SendDlgItemMessage(hwnd, IDC_EDIT_DS, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_ES]);
  SendDlgItemMessage(hwnd, IDC_EDIT_ES, WM_SETTEXT, 0, (LPARAM) RegText);

  sprintf(RegText, "%04X", regs16[REG_AX]);
  SendDlgItemMessage(hwnd, IDC_EDIT_AX, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_BX]);
  SendDlgItemMessage(hwnd, IDC_EDIT_BX, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_CX]);
  SendDlgItemMessage(hwnd, IDC_EDIT_CX, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_DX]);
  SendDlgItemMessage(hwnd, IDC_EDIT_DX, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_IP]);
  SendDlgItemMessage(hwnd, IDC_EDIT_IP, WM_SETTEXT, 0, (LPARAM)RegText);
  sprintf(RegText, "%04X", regs16[REG_SP]);
  SendDlgItemMessage(hwnd, IDC_EDIT_SP, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_BP]);
  SendDlgItemMessage(hwnd, IDC_EDIT_BP, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_SI]);
  SendDlgItemMessage(hwnd, IDC_EDIT_SI, WM_SETTEXT, 0, (LPARAM) RegText);
  sprintf(RegText, "%04X", regs16[REG_DI]);
  SendDlgItemMessage(hwnd, IDC_EDIT_DI, WM_SETTEXT, 0, (LPARAM) RegText);

  // flags value
  RegText[0] = 0;
  strcat(RegText, (regs8[FLAG_CF]) ? " C" : " -");
  strcat(RegText, (regs8[FLAG_ZF]) ? " Z" : " -");
  strcat(RegText, (regs8[FLAG_SF]) ? " S" : " -");
  strcat(RegText, (regs8[FLAG_OF]) ? " O" : " -");
  strcat(RegText, (regs8[FLAG_PF]) ? " P" : " -");
  strcat(RegText, (regs8[FLAG_AF]) ? " A" : " -");
  strcat(RegText, (regs8[FLAG_TF]) ? " T" : " -");
  strcat(RegText, (regs8[FLAG_IF]) ? " I" : " -");
  strcat(RegText, (regs8[FLAG_DF]) ? " D" : " -");
  SendDlgItemMessage(hwnd, IDC_STATIC_FLAGS, WM_SETTEXT, 0, (LPARAM) RegText);

  // PORTS
  sprintf(RegText, "%04X", lastPortAddr);
  SendDlgItemMessage(hwnd, IDC_SET_PORT_ADDR, WM_SETTEXT, 0, (LPARAM)RegText);

  sprintf(RegText, "%02X", ports[lastPortAddr]);
  SendDlgItemMessage(hwnd, IDC_SET_PORT_VALUE, WM_SETTEXT, 0, (LPARAM)RegText);

  // read only
  SendDlgItemMessage(hwnd, IDC_EDIT_CS, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_SS, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_DS, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_ES, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_AX, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_BX, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_CX, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_DX, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_SP, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_BP, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_SI, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_DI, EM_SETREADONLY, TRUE, 0);
  SendDlgItemMessage(hwnd, IDC_EDIT_IP, EM_SETREADONLY, TRUE, 0);
  // SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, EM_SETREADONLY, TRUE, 0);
  // SendDlgItemMessage(hwnd, IDC_DISASS_ADDRESS, EM_SETREADONLY, TRUE, 0);

  SendDlgItemMessage(hwnd,
                     IDC_ENABLE_TRACE,
                     BM_SETCHECK,
                     (TraceEnabled) ? BST_CHECKED : BST_UNCHECKED,
                     (LPARAM) 0);


  // Update disassembly
  disass_seg = regs16[REG_CS];
  disass_off = regs16[REG_IP];

  DEBUG_UpdateDisassembly(hwnd);

  // update RAM display
  DEBUG_UpdateMemDump(hwnd);
}

// global buffer, else MessageBox might be show after string buffer is destroyed
char genericBuffer[512];

long strSearch(unsigned char* str, unsigned char* substr, unsigned long sourceLen, unsigned long substrLen)
{
	long index = -1;
	for (int i = 0; i < sourceLen; i++) {
		index = -1;
		for (int j = 0; j < substrLen; j++) {
			if (str[i + j] != substr[j]) {
				index = -1;
				break;
			}
			index = i;
		}
		if (index != -1) {
			break;
		}
	}
	return index;
}


// =============================================================================
// FUNCTION: DebugDialogProc
//
// DESCRIPTION:
// Windows callback for the debug dialog.
//
// PARAMETERS:
//
//   hwnd   : The window handle for the message
//
//   msg    : The message received
//
//   wparam : The wparam for the message
//
//   lparam : The lparam for the message.
//
// RETURN VALUE:
//
//   0 if the message was handled by this procedure.
//
static BOOL CALLBACK DebugDialogProc(
  HWND hwnd,
  UINT msg,
  WPARAM wparam,
  LPARAM /* lparam */)
{
  RECT WRect;
  RECT DRect;
  int WindowWidth;
  int WindowHeight;
  int DialogWidth;
  int DialogHeight;
  int DialogX;
  int DialogY;
  int DasmLine;

  //
  // What is the message
  //
  switch (msg)
  {
    case WM_INITDIALOG:

      // hwndFocus = (HWND) wparam; // handle of control to receive focus
      // lInitParam = lparam;
      //
      // Do initialization stuff here
      //
      if (!LastPosSet)
      {
        GetWindowRect(ConfigParent, &WRect);
        WindowWidth = (WRect.right - WRect.left) + 1;
        WindowHeight = (WRect.bottom - WRect.top) + 1;

        GetWindowRect(hwnd, &DRect);
        DialogWidth = (DRect.right - DRect.left) + 1;
        DialogHeight = (DRect.bottom - DRect.top) + 1;

        DialogX = WRect.left + (WindowWidth - DialogWidth) / 2;
        DialogY = WRect.top + (WindowHeight - DialogHeight) / 2;

        if (DialogX < 0) DialogX = 0;
        if (DialogY < 0) DialogY = 0;
      }
      else
      {
        DialogX = LastDialogX;
        DialogY = LastDialogY;
      }

      SetWindowPos(hwnd, NULL, DialogX, DialogY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	  SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SYSMENU);	// hide close button as we don't handle it

      // Update dialog controls.
      DEBUG_UpdateControls(hwnd);

      return(FALSE);

    case WM_COMMAND:
    {
      int wNotifyCode = HIWORD(wparam); // notification code
      int wID = LOWORD(wparam);         // item, control, or accelerator identifier
      //HWND hwndCtl = (HWND) lparam;     // handle of control

      switch (wNotifyCode)
      {
        case BN_CLICKED:
        {
			switch (wID)
			{
			case IDC_ENABLE_TRACE:
			{
				bool NewTraceEnabled =
					(SendDlgItemMessage(hwnd, IDC_ENABLE_TRACE, BM_GETCHECK, 0, 0) == BST_CHECKED);

				if (NewTraceEnabled != TraceEnabled)
				{
					TraceEnabled = NewTraceEnabled;
					if (TraceEnabled)
					{
						BreakpointCount++;
					}
					else
					{
						BreakpointCount--;
					}
				}
				break;
			}

			case IDC_DUMP_TRACE:
			{
				char ofnBuffer[1024];
				if (SaveFileDialog("Save trace file...", ofnBuffer, 1024, "Text files\0*.txt\0\0"))
				{
					DEBUG_SaveTrace(ofnBuffer);
				}
				break;
			}

			case IDC_MEM_WRITE:
			{
				unsigned int memSeg = 0;
				unsigned int memOff = 0;
				unsigned int memVal = 0;
				SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				int cnt = sscanf(genericBuffer, "%04X:%04X", &memSeg, &memOff);
				if (cnt != 2)
				{
					MessageBox(hwnd, "Memory address must be of format XXXX:YYYY where XXXX is the segment in hex and YYYY is the offset", "Error", MB_OK);
				}
				else {
#define PATCH_BYTES_MAX_HEX	32
					char hexBuffer[PATCH_BYTES_MAX_HEX * 2];
					SendDlgItemMessage(hwnd, IDC_MEMORY_VALUES, WM_GETTEXT, 256, (LPARAM)hexBuffer);
					unsigned char bytearray[PATCH_BYTES_MAX_HEX];
					int hexlen = strlen(hexBuffer);
					if (hexlen == 0 || hexlen % 2 != 0)
					{
						MessageBox(hwnd, "Invalid hexadecimal format for memory values", "Error", MB_OK);
					}
					else
					{
						printf("Patching memory at at %04X:%04X to ", memSeg, memOff);
						for (size_t i = 0, j = 0; i < hexlen / 2; i++, j += 2)
						{
							bytearray[i] = (hexBuffer[j] % 32 + 9) % 25 * 16 + (hexBuffer[j + 1] % 32 + 9) % 25;
							printf("%02X ", bytearray[i]);
							mem[memSeg * 16 + memOff + i] = bytearray[i];
						}
						printf("\n");

						MessageBox(hwnd, "Patching completed", "Success", MB_OK);

						DEBUG_UpdateDisassembly(hwnd);
						DEBUG_UpdateMemDump(hwnd);
						DEBUG_UpdateControls(hwnd);
					}
				}
			}
			break;

			case IDC_BRK_SET:
				DasmLine = SendDlgItemMessage(hwnd, IDC_LIST_ASM, LB_GETCURSEL, 0, (LPARAM)0);
				if (DasmLine != LB_ERR)
				{
					if (!BreakPointSet[DisassLineAddr[DasmLine]])
					{
						BreakpointCount++;
						BreakPointSet[DisassLineAddr[DasmLine]] = true;
						DEBUG_UpdateDisassembly(hwnd);
					}
				}
				break;

			case IDC_BRK_CLR:
				DasmLine = SendDlgItemMessage(hwnd, IDC_LIST_ASM, LB_GETCURSEL, 0, (LPARAM)0);
				if (DasmLine != LB_ERR)
				{
					if (BreakPointSet[DisassLineAddr[DasmLine]])
					{
						BreakpointCount--;
						BreakPointSet[DisassLineAddr[DasmLine]] = false;
						DEBUG_UpdateDisassembly(hwnd);
					}
				}
				break;

			case IDC_BRK_CLR_ALL:
				Breakpoint_ClearAll();
				DEBUG_UpdateDisassembly(hwnd);
				break;

			case IDC_DEBUG_CONTINUE:
			{
				RECT wr;
				GetWindowRect(hwnd, &wr);

				LastPosSet = true;
				LastDialogX = wr.left;
				LastDialogY = wr.top;

				DbgState = DEBUG_NONE;
				EndDialog(hwnd, IDOK);
				DbgHwnd = 0;
				break;
			}

			case IDC_DEBUG_STEP:
				DbgState = DEBUG_STEP;
				break;

			case IDC_DEBUG_DUMP_MEM_PORTS:
			{
				char fdump[256];
				time_t t = time(NULL);

				struct tm tm = *localtime(&t);
				sprintf(fdump, "Super8086Box_MemoryDump_%04d%02d%02d%02d%02d%02d.bin", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

				bool isOk = false;
				FILE *f = fopen(fdump, "wb");
				if (f)
				{
					size_t cnt = fwrite(mem, sizeof(char), RAM_SIZE, f);
					printf("%u bytes written into %s\n", cnt, fdump);
					int res = fclose(f);
					printf("Memory dump file close code: %d\n", res);
					isOk = (res == 0);
				}
				else {
					printf("Failed to created memory dump: %s\n", fdump);
				}
				if (!isOk)
				{
					MessageBox(hwnd, "Memory dump failed", "Error", MB_OK);
				}
				else {
					sprintf(fdump, "Super8086Box_PortDump_%04d%02d%02d%02d%02d%02d.bin", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

					FILE *f2 = fopen(fdump, "wb");
					if (f2)
					{
						size_t cnt = fwrite(ports, sizeof(char), port_in_size, f2);
						printf("%u bytes written into %s\n", cnt, fdump);
						int res = fclose(f2);
						printf("Port dump file close code: %d\n", res);
						isOk = (res == 0);
					}
					else {
						printf("Failed to created port dump: %s\n", fdump);
					}
					if (isOk)
					{
						MessageBox(hwnd, "Dump files written to executable directory", "Success", MB_OK);
					}
					else {
						MessageBox(hwnd, "Port dump failed", "Error", MB_OK);
					}
				}
			}
			break;

			case IDC_DEBUG_SHOW_IP_BYTES:
			{
				showmem_seg = disass_seg;
				showmem_off = disass_off;
				printf("MemAddr changed to %02X:%02X\n", showmem_seg, showmem_off);
				DEBUG_UpdateMemDump(hwnd);
			}
			break;

			case IDC_MEM_SHOW:
				// user has edited memory address
				SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				printf("MemAddr changed to %s\n", genericBuffer);
				sscanf(genericBuffer, "%X:%X", (unsigned int *)&showmem_seg, (unsigned int *)&showmem_off);


				DEBUG_UpdateMemDump(hwnd);
				break;

			case IDC_MEM_SEARCH:
			{
				SendDlgItemMessage(hwnd, IDC_MEMORY_SEARCH_TEXT, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				
				int inputlen = strlen(genericBuffer);
				if (inputlen == 0)
				{
					MessageBox(hwnd, "Please enter values to search for", "Error", MB_OK);
					break;
				}
				
				unsigned int startAddr = showmem_seg * 16 + showmem_off + 1; // workaround here so that Search can be used to resume search
																			// downside is that if string is at 0000:0000 it won't be found (but will be obvious anyway)
				unsigned long maxSearchLen = RAM_SIZE - startAddr;
				long index = -1;

				// TODO: Need to set max length for text field, will crash if user enter too many chars
#define SEARCH_BYTES_MAX	256
				unsigned char searchBytes[SEARCH_BYTES_MAX];

				if (MessageBox(hwnd, "Do you want to search for hexadecimal values?\nClick Yes to search for hex, No to search for text.", "Question", MB_YESNO) == IDYES)
				{
					if (inputlen == 0 || inputlen % 2 != 0)
					{
						MessageBox(hwnd, "Invalid hexadecimal format for memory values", "Error", MB_OK);
						break;
					}
					else
					{
						for (size_t i = 0, j = 0; i < inputlen / 2; i++, j += 2)
						{
							searchBytes[i] = (genericBuffer[j] % 32 + 9) % 25 * 16 + (genericBuffer[j + 1] % 32 + 9) % 25;
						}
						index = strSearch(mem + startAddr, searchBytes, maxSearchLen, inputlen/2);
					}
				}
				else {
					for (int i = 0; i < strlen(genericBuffer); i++)
						searchBytes[i] = genericBuffer[i];

					index = strSearch(mem + startAddr, searchBytes, maxSearchLen, strlen(genericBuffer));
				}

				if (index >= 0)
				{
					index = index + startAddr;

					// try to remain in the same segment if possible
					// multiple segment:offset pairs could point to the same address
					unsigned int newOff = index - (showmem_seg * 16);	
					if (showmem_seg * 16 + newOff == index) // == will not match if overflow above
					{
						showmem_off = newOff;
					}
					else {
						showmem_seg = index / 16;
						showmem_off = index - showmem_seg * 16;
					}

					sprintf(genericBuffer, "%04X:%04X", showmem_seg, showmem_off);
					SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, WM_SETTEXT, 0, (LPARAM)genericBuffer);
					DEBUG_UpdateMemDump(hwnd);
				}
				else {
					MessageBox(hwnd, "No more occurrences found", "Error", MB_OK);
				}

			}
			break;

			case IDC_WRITE_PORT:
			case IDC_READ_PORT:
			{
				SendDlgItemMessage(hwnd, IDC_SET_PORT_ADDR, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				if (strlen(genericBuffer) != 4)
				{
					MessageBox(hwnd, "Port address must have 4 hex digits, e.g. 0378", "Error", MB_OK);
				}
				else {
					int res = sscanf(genericBuffer, "%04X", &lastPortAddr);
					if (res != 1)
					{
						MessageBox(hwnd, "Invalid hexadecimal value for port address.", "Error", MB_OK);
					}
					else {
						if (wID == IDC_READ_PORT)
						{
							unsigned char portVal = ports[lastPortAddr];

							sprintf(genericBuffer, "%02X", portVal);
							SendDlgItemMessage(hwnd, IDC_SET_PORT_VALUE, WM_SETTEXT, 0, (LPARAM)genericBuffer);

							sprintf(genericBuffer, "Current value for port 0x%04X is 0x%02X", lastPortAddr, portVal);
							MessageBox(hwnd, genericBuffer, "Message", MB_OK);
						}
						else {
							SendDlgItemMessage(hwnd, IDC_SET_PORT_VALUE, WM_GETTEXT, 256, (LPARAM)genericBuffer);
							if (strlen(genericBuffer) != 2)
							{
								MessageBox(hwnd, "Port value must have 2 hex digits, e.g. FF", "Error", MB_OK);
							}
							else {
								unsigned int portVal;
								int res = sscanf(genericBuffer, "%02X", &portVal);
								if (res != 1)
								{
									MessageBox(hwnd, "Invalid hexadecimal value for port value.", "Error", MB_OK);
								}
								else {
									ports[lastPortAddr] = portVal;

									sprintf(genericBuffer, "Value for port 0x%04X has been set to 0x%02X", lastPortAddr, portVal);
									MessageBox(hwnd, genericBuffer, "Message", MB_OK);
								}
							}
						}
					}
				}
			}
				break;

			case IDC_SET_REGISTER:
			{
				SendDlgItemMessage(hwnd, IDC_SET_REGISTER_EXPR, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				bool isValidSyntax = true;
				if (strlen(genericBuffer) == 3 || strlen(genericBuffer) == 7)
				{
					if (strlen(genericBuffer) == 3)
					{
						// e.g. Z=1 or A=0
						bool isSet = (genericBuffer[2] == '1');
						if (genericBuffer[1] == '=')
						{
							switch (genericBuffer[0])
							{
								case 'C':
									regs8[FLAG_CF] = isSet;
									break;
								case 'Z':
									regs8[FLAG_ZF] = isSet;
									break;
								case 'S':
									regs8[FLAG_SF] = isSet;
									break;
								case 'O':
									regs8[FLAG_OF] = isSet;
									break;
								case 'P':
									regs8[FLAG_PF] = isSet;
									break;
								case 'A':
									regs8[FLAG_AF] = isSet;
									break;
								case 'T':
									regs8[FLAG_TF] = isSet;
									break;
								case 'I':
									regs8[FLAG_IF] = isSet;
									break;
								case 'D':
									regs8[FLAG_IF] = isSet;
									break;
								default:
									isValidSyntax = false;
							}

							if (isValidSyntax)
							{
								MessageBox(hwnd, "Flag state has been updated", "Success", MB_OK);
							}
						}
						else {
							isValidSyntax = false;
						}
					}
					else {
						// e.g. AX = FFFF. AL=FF is not supported
						if (genericBuffer[2] == '=')
						{
							char regName[16];
							memcpy(regName, genericBuffer, 2);
							regName[2] = 0;

							char regVal[16];
							memcpy(regVal, genericBuffer + 3, 4);
							regVal[4] = 0;

							unsigned int newVal;
							int res = sscanf(regVal, "%04X", &newVal);
							if (res != 1)
							{
								MessageBox(hwnd, "Invalid hexadecimal value for register value.", "Error", MB_OK);
							}
							else {
								if (strcmp(regName, "AX") == 0)
								{
									regs16[REG_AX] = newVal;
								}
								else if (strcmp(regName, "BX") == 0)
								{
									regs16[REG_BX] = newVal;
								}
								else if (strcmp(regName, "CX") == 0)
								{
									regs16[REG_CX] = newVal;
								}
								else if (strcmp(regName, "DX") == 0)
								{
									regs16[REG_DX] = newVal;
								}
								else if (strcmp(regName, "SP") == 0)
								{
									regs16[REG_SP] = newVal;
								}
								else if (strcmp(regName, "BP") == 0)
								{
									regs16[REG_BP] = newVal;
								}
								else if (strcmp(regName, "SI") == 0)
								{
									regs16[REG_SI] = newVal;
								}
								else if (strcmp(regName, "DI") == 0)
								{
									regs16[REG_DI] = newVal;
								}
								else if (strcmp(regName, "IP") == 0)
								{
									isPendingIpChange = true;
									// unsigned short regcs = regs16[REG_CS];
									// newIpVal = regcs * 16 + newVal;
									newIpVal = newVal;
								}
								else if (strcmp(regName, "SS") == 0)
								{
									regs16[REG_SS] = newVal;
								}
								else if (strcmp(regName, "DS") == 0)
								{
									regs16[REG_DS] = newVal;
								}
								else if (strcmp(regName, "ES") == 0)
								{
									regs16[REG_ES] = newVal;
								}
								else if (strcmp(regName, "CS") == 0)
								{
									regs16[REG_CS] = newVal;
								}
								else {
									isValidSyntax = false;
								}

								if (isValidSyntax)
								{
									if (!isPendingIpChange)
									{
										sprintf(genericBuffer, "Value of register %s has been set to 0x%04X", regName, newVal);
										MessageBox(hwnd, genericBuffer, "Success", MB_OK);
									}
									else {
										sprintf(genericBuffer, "Do you want to change CS:IP to %04X:%04X?", regs16[REG_CS], newVal);
										if (MessageBox(hwnd, genericBuffer, "Question", MB_YESNO) == IDYES)
										{
											sprintf(genericBuffer, "CS:IP is now %04X:%04X", regs16[REG_CS], newVal);
											MessageBox(hwnd, genericBuffer, "Success", MB_OK);

											DbgState = DEBUG_STEP;
										}
									}
								}
							}
						}
						else {
							isValidSyntax = false;
						}
					}
				}
				else {
					isValidSyntax = false;
				}

				if (!isValidSyntax)
				{
					MessageBox(hwnd, "Syntax error.\nExample: AL=FFFF or Z=1 to set register or flag respectively.\nNames are case sensitive.\n8-bit register names (e.g. AL=FF) is not supported. Use 16-bit names (e.g. AX) instead.", "Error", MB_OK);
				}
				else {
					DEBUG_UpdateDisassembly(hwnd);
					DEBUG_UpdateMemDump(hwnd);
					DEBUG_UpdateControls(hwnd);
				}				
			}
				break;

			case IDC_DISASS_GOTO:
			{
				// user has edited disassembly memory address
				SendDlgItemMessage(hwnd, IDC_DISASS_ADDRESS, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				printf("Disassembly Addr changed to %s\n", genericBuffer);
				sscanf(genericBuffer, "%X:%X", (unsigned int *)&disass_seg, (unsigned int *)&disass_off);

				// update disassembly display
				DEBUG_UpdateDisassembly(hwnd);

				break;
			}

			case IDC_DEBUG_SAVE_DISASM:
			{
				DEBUG_SaveDisassembly(hwnd);
			}
				break;

            default:
              break;
          }
          break;
        }

        case EN_KILLFOCUS:
        {
          switch (wID)
          {         
            case IDC_MEMORY_ADDRESS:
				// user has edited memory address
				SendDlgItemMessage(hwnd, IDC_MEMORY_ADDRESS, WM_GETTEXT, 256, (LPARAM)genericBuffer);
				printf("MemAddr changed to %s\n", genericBuffer);
				sscanf(genericBuffer, "%X:%X", (unsigned int *)&showmem_seg, (unsigned int *)&showmem_off);

				// update RAM display
				DEBUG_UpdateMemDump(hwnd);

              break;
          }
        }

        default:
          break;
      }
      break;
    }


    default:
      break;

  }

  return (FALSE);

}

// =============================================================================
// Exported variables
//

int BreakpointCount = 0;


// =============================================================================
// Exported Functions
//

void DEBUG_Initialise(unsigned char *mem_in, unsigned char *ports_in, unsigned long _port_in_size)
{
	isPendingIpChange = false;
  mem = mem_in;
  ports = ports_in;
  port_in_size = _port_in_size;
  regs8 = mem + REGS_BASE;
  regs16 = (unsigned short *)(mem + REGS_BASE);

  hFont = CreateFont(10, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
	  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	  DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier"));

  Breakpoint_ClearAll();
}

void DEBUG_CreateDialog(HINSTANCE hInstance, HWND hwndParent)
{
  ConfigParent = hwndParent;

  if (DbgHwnd == 0)
  {
    DbgHwnd = CreateDialog(
        hInstance,
        MAKEINTRESOURCE(IDD_DIALOG_DEBUG),
        hwndParent,
        DebugDialogProc);
    DbgState = DEBUG_STOPPED;
  }
}

void DEBUG_Update(void)
{
  DEBUG_UpdateControls(DbgHwnd);
}

DebugState_t DEBUG_GetState(void)
{
  if (DbgState == DEBUG_STEP)
  {
    DbgState = DEBUG_STOPPED;
    return DEBUG_STEP;
  }
  return DbgState;
}

unsigned short DEBUG_GetNewIPVal()
{
	return newIpVal;
}
bool DEBUG_IsPendingIpChange()
{
	return isPendingIpChange;
}
void DEBUG_ClearPendingIpChange()
{
	isPendingIpChange = false;
	newIpVal = 0x00;
}

void DEBUG_CheckBreak(void)
{
  if (BreakpointCount == 0)
  {
    return;
  }

  unsigned short reg_cs = regs16[REG_CS];
  unsigned short reg_ip = regs16[REG_IP];

  if (TraceEnabled)
  {
    TraceCS[TracePos] = reg_cs;
    TraceIP[TracePos] = reg_ip;

    if (TraceLength < TRACE_BUFFER_LENGTH) TraceLength++;
    TracePos = (TracePos + 1) % TRACE_BUFFER_LENGTH;
  }

  int Address = reg_cs * 16 + reg_ip;

  if (BreakPointSet[Address])
  {
    DbgState = DEBUG_STOPPED;
  }

}