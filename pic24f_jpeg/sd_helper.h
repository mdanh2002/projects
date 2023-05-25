#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"

typedef struct _SDCARD_INFO
{
	char fsType;
	char fsTypeStr[10];
	unsigned long totalSize;
	unsigned long freeSize;
	char volumeLabel[12];
} SDCARD_INFO;

// SD Card Helper functions using Microchip Memory Disk Drive source code
BOOL initSDCard();
SDCARD_INFO getCardInfo();
