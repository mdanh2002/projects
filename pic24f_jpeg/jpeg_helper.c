//------------------------------------------------------------------------------
// jpg2tga.c
// JPEG to TGA file conversion example program.
// Public domain, Rich Geldreich <richgel99@gmail.com>
// Last updated Nov. 26, 2010
//------------------------------------------------------------------------------
#include "picojpeg.h"
#include "jpeg_helper.h"
#include "MDD File System/FSIO.h"
#include "MDD File System/SD-SPI.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "ili9341.h"
#include "uart.h"

//------------------------------------------------------------------------------
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned long uint;
//------------------------------------------------------------------------------
static FSFILE *g_pInFile;
static uint g_nInFileSize;
static uint g_nInFileOfs;
//------------------------------------------------------------------------------
unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
	uint n;
	// pCallback_data;

	n = min(g_nInFileSize - g_nInFileOfs, buf_size);
	if (n && (FSfread(pBuf, 1, n, g_pInFile) != n))
	return PJPG_STREAM_READ_ERROR;
	*pBytes_actually_read = (unsigned char)(n);
	g_nInFileOfs += n;
	return 0;
}
//------------------------------------------------------------------------------
// Loads JPEG image from specified file. Returns NULL on failure.
// If reduce is non-zero, the image will be more quickly decoded at approximately
// 1/8 resolution (the actual returned resolution will depend on the JPEG 
// subsampling factor).
JPEG_Info pjpeg_load_from_file(const char *pFilename, int reduce, unsigned char showHorizontal)
{
	JPEG_Info retVal;
	retVal.loadStatus = STATUS_UNKNOWN;
	retVal.loadedWidth = 0;
	retVal.loadedHeight = 0;
	retVal.actualWidth = 0;
	retVal.actualHeight = 0;
	retVal.colorComps = 0;
	retVal.scanType = PJPG_GRAYSCALE;

	pjpeg_image_info_t image_info;
	int mcu_x = 0;
	int mcu_y = 0;
	uint row_pitch;

	uint8 status;
	uint decoded_width, decoded_height;
	uint row_blocks_per_mcu, col_blocks_per_mcu;

	g_pInFile = FSfopen (pFilename, FS_READ);
	if (!g_pInFile)
	{
		retVal.loadStatus = STATUS_FILEREADERROR;
		return retVal;
	}

	g_nInFileOfs = 0;

	FSfseek(g_pInFile, 0, SEEK_END);
	g_nInFileSize = FSftell(g_pInFile);
	FSfseek(g_pInFile, 0, SEEK_SET);
	
	status = pjpeg_decode_init(&image_info, pjpeg_need_bytes_callback, NULL, (unsigned char)reduce);
	
	if (status)
	{
		retVal.loadStatus = status;
		FSfclose(g_pInFile);
		return retVal;
	}

	retVal.scanType = image_info.m_scanType;

	// In reduce mode output 1 pixel per 8x8 block.
	decoded_width = reduce ? (image_info.m_MCUSPerRow * image_info.m_MCUWidth) / 8 : image_info.m_width;
	decoded_height = reduce ? (image_info.m_MCUSPerCol * image_info.m_MCUHeight) / 8 : image_info.m_height;

	row_pitch = decoded_width * image_info.m_comps;

	row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
	col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

	// assigned image information
	retVal.actualWidth = image_info.m_width;
	retVal.actualHeight = image_info.m_height;
	retVal.loadedWidth = decoded_width;
	retVal.loadedHeight = decoded_height;
	retVal.colorComps = image_info.m_comps;

	for ( ; ; )
	{
		status = pjpeg_decode_mcu();
		
		if (status)
		{
			if (status != PJPG_NO_MORE_BLOCKS)
			{
				retVal.loadStatus = status;
				FSfclose(g_pInFile);
				return retVal;
			}

			break;
		}

		if (mcu_y >= image_info.m_MCUSPerCol)
		{
			retVal.loadStatus = STATUS_INVALIDDATA;
			FSfclose(g_pInFile);
			return retVal;
		}

		if (reduce)
		{
			// In reduce mode, only the first pixel of each 8x8 block is valid.
			// m_comps: color component (1 for grayscale, 3 for RGB) 
			uint y, x;
			for (y = 0; y < col_blocks_per_mcu; y++)
			{
				uint src_ofs = (y * 128U);

				for (x = 0; x < row_blocks_per_mcu; x++)
				{
					uint lcd_x = decoded_width - mcu_x * row_blocks_per_mcu + x - 1;
					uint lcd_y = mcu_y * col_blocks_per_mcu + y;
					TFT_setPixel(showHorizontal ? lcd_y : lcd_x, 
								 showHorizontal ? lcd_x : lcd_y,
								 RGB565(image_info.m_pMCUBufR[src_ofs], 
								 image_info.m_pMCUBufG[src_ofs], 
								 image_info.m_pMCUBufB[src_ofs]));

					src_ofs += 64;
				}
			}
		}
		else
		{
			// Copy MCU's pixel blocks into the destination bitmap.
			uint y, x;

			for (y = 0; y < image_info.m_MCUHeight; y += 8)
			{
				const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

				for (x = 0; x < image_info.m_MCUWidth; x += 8)
				{
					// Compute source byte offset of the block in the decoder's MCU buffer.
					uint src_ofs = (x * 8U) + (y * 16U);
					const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
					const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
					const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

					const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));
					
					int bx, by;
					for (by = 0; by < by_limit; by++)
					{
						for (bx = 0; bx < bx_limit; bx++)
						{
							uint lcd_x = image_info.m_width - (mcu_x * image_info.m_MCUWidth + x + bx) - 1;
							uint lcd_y = mcu_y * image_info.m_MCUHeight + y + by;

							TFT_setPixel(showHorizontal ? lcd_y : lcd_x,
										 showHorizontal ? lcd_x : lcd_y, 
									 	 RGB565(*pSrcR++, *pSrcG++, *pSrcB++));
						}

						pSrcR += (8 - bx_limit);
						pSrcG += (8 - bx_limit);
						pSrcB += (8 - bx_limit);
					}
				}
			}
		}
		
		mcu_x++;
		if (mcu_x == image_info.m_MCUSPerRow)
		{
			mcu_x = 0;
			mcu_y++;
		}
	}

	FSfclose(g_pInFile);

	retVal.loadStatus = STATUS_OK;
	return retVal;
}
