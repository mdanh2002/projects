#include "picojpeg.h"

#define STATUS_OK 				0x00
#define STATUS_INVALIDDATA 		0xFD
#define STATUS_FILEREADERROR 	0xFE
#define STATUS_UNKNOWN 			0xFF

typedef struct _JPEG_Info
{
	unsigned char loadStatus;
	int loadedWidth;
	int loadedHeight;
	int actualWidth;
	int actualHeight;
	pjpeg_scan_type_t scanType;
	int colorComps;

} JPEG_Info;

JPEG_Info pjpeg_load_from_file(const char *pFilename, int reduce, unsigned char showHorizontal);
