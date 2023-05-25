#include "sd_helper.h"
#include "uart.h"

BOOL initSDCard()
{
	BOOL isOK = FALSE;
	int attempt = 0;

	SendUARTStr("Waiting for media card...");
	while (attempt < 100 && !isOK)
	{
		isOK = MDD_MediaDetect();
		attempt++;
	}
	if (isOK)
	{
		SendUARTStr("Initializing media card...");
		
		isOK = FALSE;
		attempt = 0;
		while (attempt < 10 && !isOK)
		{
			isOK = FSInit();
			attempt++;
		}
		if (isOK)
		SendUARTStr("Media card initialized...");
		else
		SendUARTStr("Media card initialization failed...");
	}
	else
	{
		SendUARTStr("Media card not detected...");
	}

	return isOK;
}

SDCARD_INFO getCardInfo()
{
	SDCARD_INFO retVal;

	FS_DISK_PROPERTIES disk_properties;
	disk_properties.new_request = TRUE;
	do
	{
		FSGetDiskProperties(&disk_properties);
	} while (disk_properties.properties_status == FS_GET_PROPERTIES_STILL_WORKING);

	retVal.totalSize = disk_properties.results.total_clusters * disk_properties.results.sector_size * disk_properties.results.sectors_per_cluster;
	retVal.freeSize = disk_properties.results.free_clusters * disk_properties.results.sector_size * disk_properties.results.sectors_per_cluster;
	retVal.fsType = disk_properties.results.disk_format;

	char* fsTypeStr;
	switch (disk_properties.results.disk_format)
	{
	case FAT12:
		fsTypeStr = "FAT12";
		break;
	case FAT16:
		fsTypeStr = "FAT16";
		break;
	case FAT32:
		fsTypeStr = "FAT32";
		break;
	default:
		fsTypeStr = "Unknown";
		break;
	}
	strcpy(retVal.fsTypeStr, fsTypeStr);		

	// the volume label is the first File Record in the root directory
	SearchRec rec;
	int res = 0;
	res = FindFirst("*.*", ATTR_MASK, &rec); //8.3 filename
	// res = wFindFirst("*.*", ATTR_MASK, &rec); //Long filename
	if (res == 0 && rec.attributes & ATTR_VOLUME)
	{
		strcpy(retVal.volumeLabel, rec.filename);		
	}

	return retVal;
}
