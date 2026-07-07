#include "stdio.h" 
#include "string.h" 
#include "adlib.h" 

#define YM3812_CLOCK 3580000
#define YM3812_NUM_CHIPS 1
#define YM3812_CHIP_ID 0

bool AdlibEnabled = false;
int getAdlibEnabled()
{
	return AdlibEnabled;
}
void setAdlibEnabled(bool enabled)
{
	AdlibEnabled = enabled;
}

int LptSndType = 0;
int getLptPortType()
{
	return LptSndType;
}
void setLptPortType(int sndType)
{
	LptSndType = sndType;
}

void initAdlib(UINT32 samplingRate) {
	YM3812Init(YM3812_NUM_CHIPS, YM3812_CLOCK, samplingRate);
}

void closeAdlib() {
	YM3812Shutdown();
}

void resetAdlib() {
	YM3812ResetChip(YM3812_CHIP_ID);
}

void writeAdlib(UINT8 address, UINT8 val) {
	YM3812Write(YM3812_CHIP_ID, address, val);
}

UINT8 readAdlibReg(UINT8 address) {
	return YM3812Read(YM3812_CHIP_ID, address);
}

void updateAdlibFrame(UINT32 length, INT16 *buffer) {
	YM3812UpdateOne(YM3812_CHIP_ID, buffer, length);
}

UINT8 adlibTimerOverflow(UINT8 timerId) {
	return YM3812TimerOver(YM3812_CHIP_ID, timerId);
}

UINT8 adlibGetStatus() {
	return YM3812GetStatus(YM3812_CHIP_ID);
}

void setAdlibTimerHandler(OPL_TIMERHANDLER TimerHandler) {
	YM3812SetTimerHandler(YM3812_CHIP_ID, TimerHandler, 0);
}

void setAdlibIRQHandler(OPL_IRQHANDLER IRQHandler) {
	YM3812SetIRQHandler(YM3812_CHIP_ID, IRQHandler, 0);
}

void setAdlibUpdateHandler(OPL_UPDATEHANDLER UpdateHandler) {
	YM3812SetUpdateHandler(YM3812_CHIP_ID, UpdateHandler, 0);
}

bool adlibCfgRead(FILE *fp)
{
	char Line[256];
	int tmp;

	fgets(Line, 256, fp);
	if (strncmp(Line, "[ADLIB_ENABLE]", 14) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);

	if (tmp)
	{
		printf("Adlib emulation enabled\n");
	}
	else
	{
		printf("Adlib emulation disabled\n");
	}
	setAdlibEnabled(tmp);

	fgets(Line, 256, fp);
	if (strncmp(Line, "[LPT1_MODE]", 11) != 0) return 0;

	fgets(Line, 256, fp);
	sscanf(Line, "%d\n", &tmp);

	if (tmp >= LPT_SOUND_TYPE_DISNEY)
	{
		printf("Disney Sound emulation enabled\n");
	}
	else if (tmp == LPT_SOUND_TYPE_COVOX)
	{
		printf("Covox Speech Thing Sound emulation enabled\n");
	}
	else {
		printf("Parallel port support disabled\n");
	}
	setLptPortType(tmp);

	return true;
}