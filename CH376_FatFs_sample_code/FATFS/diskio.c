/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module                          */
/*-----------------------------------------------------------------------*/
/*
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "diskio.h"
#include <xc.h>
#include <stdio.h>
#include "../config.h"
#include "../uart.h"
#include "../spi_lib.h"
#include "mmc_pic.h"
#include "../CH376/ch376.h"


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */


static
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;			/* Card type flags */


static 
unsigned char isUsingCH376  = 0;     /* Set to TRUE to connect to CH376 module, instead of SD card */


void setFatFsIsUsb(unsigned char isUSB)
{
    isUsingCH376 = isUSB;
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)	/* 1:Ready, 0:Timeout */
{
	UINT tmr;

    // SendUARTStr("wait_ready");
    
	for (tmr = 500; tmr > 0; tmr--) {	/* Wait for ready in timeout of 500ms */
		if (sd_rx() == 0xFF) 
        {
            // SendUARTStr("ready");
            break;
        }
        
		delay_ms(1);
	}

    // SendUARTStr("wait_ready ends");
    
	return tmr ? 1 : 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
    // SendUARTStr("deselect");
    
	SD_CS_HIGH;	/* Set CS# high */
	sd_rx();	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
BYTE select (void)	/* 1:Successful, 0:Timeout */
{
    // SendUARTStr("select starts");
    
	SD_CS_LOW;	/* Set CS# low */
	sd_rx();	/* Dummy clock (force DO enabled) */
	if (wait_ready()) 
    {
        // SendUARTStr("select ok");
        
        return 1;	/* Wait for card ready */
    }

    // SendUARTStr("select fails");
	deselect();
	return 0;	/* Timeout */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;
	UINT tmr;


	for (tmr = 2000; tmr; tmr--) {	/* Wait for data packet in timeout of 200ms */
		token = sd_rx();
		if (token != 0xFF) break;
		delay_us(100);
	}
	if (token != 0xFE) return 0;	/* If not valid data token, retutn with error */

	do
		*buff++ = sd_rx();		/* Receive the data block into buffer */
	while (--btr);
	sd_rx();					/* Discard CRC */
	sd_rx();

	return 1;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if	_USE_WRITE
static
BYTE xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE resp;
	WORD i;


	if (!wait_ready()) return 0;

	sd_tx(token);			/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		i = 512;
		do
			sd_tx(*buff++);				/* Xmit the data block to the MMC */
		while (--i);
		sd_rx();						/* CRC (Dummy) */
		sd_rx();
		resp = sd_rx();					/* Reveive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return 0;
	}

	return 1;
}
#endif



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

/* NOTE: XC8 compiler is unable to allow recursion,
/  so the send_cmd function had to be divided */

#ifdef __XC8
static
BYTE __send_cmd (		/* Returns R1 resp (bit7==1:Send failed) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect();
		if (!select()) return 0xFF;
	}

	/* Send command packet */
	sd_tx(0x40 | cmd);				/* Start + Command index */
	sd_tx((BYTE)(arg >> 24));		/* Argument[31..24] */
	sd_tx((BYTE)(arg >> 16));		/* Argument[23..16] */
	sd_tx((BYTE)(arg >> 8));		/* Argument[15..8] */
	sd_tx((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) + Stop */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) Stop */
	sd_tx(n);

	/* Receive command response */
	if (cmd == CMD12) sd_rx();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = sd_rx();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}
#endif

static
BYTE send_cmd (		/* Returns R1 resp (bit7==1:Send failed) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
)
{
#ifndef __XC8
	BYTE n;
#endif
	BYTE res;
    // char resBuf[32];


    // sprintf(resBuf, "sendCmd: %d", cmd);
    // SendUARTStr(resBuf);
    
	if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
        //SendUARTStr("CMD55-CMD<n>");
        
		cmd &= 0x7F;
#ifdef __XC8
		res = __send_cmd(CMD55, 0);
#else
		res = send_cmd(CMD55, 0);
#endif
		if (res > 1) 
        {
            // SendUARTStr("r1");
            return res;
        }
        
	}

#ifdef __XC8
	return __send_cmd(cmd, arg);	/* Return with the response value */
#else
	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
        // SendUARTStr("NOT CMD12");
        
		deselect();
		if (!select()) 
        {
            // SendUARTStr("SELECT ERROR");
            return 0xFF;
        }
	}

    // SendUARTStr("main cmd");
    
	/* Send command packet */
	sd_tx(0x40 | cmd);				/* Start + Command index */
	sd_tx((BYTE)(arg >> 24));		/* Argument[31..24] */
	sd_tx((BYTE)(arg >> 16));		/* Argument[23..16] */
	sd_tx((BYTE)(arg >> 8));		/* Argument[15..8] */
	sd_tx((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) + Stop */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) Stop */
	sd_tx(n);

    // SendUARTStr("end main cmd");
    
	/* Receive command response */
	if (cmd == CMD12) 
        sd_rx();		/* Skip a stuff byte when stop reading */
    
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
    {    
		res = sd_rx();
        delay_us(1);
        
        // sprintf(resBuf, "a4a: %d", res);
        // SendUARTStr(resBuf);        
    }
	while ((res & 0x80) && --n);

    // SendUARTStr("send cmd ends");
    
	return res;			/* Return with the response value */
#endif
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

unsigned char _last_sd_init_ok = 0;
        
DSTATUS disk_initialize_sd (
	BYTE pdrv		/* Physical drive nmuber (0) */
)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;
    // char resBuf[32];

    _last_sd_init_ok = 0;
    
    // SendUARTStr("disk_init");

	if (pdrv) return STA_NOINIT;		/* Supports only single drive */

	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	FCLK_SLOW;
    SD_CS_HIGH;
	for (n = 10; n; n--) 
    {
        sd_rx();	/* 80 dummy clocks */
    }
	ty = 0;
    
    delay_ms(10);
       
    unsigned char attempt = 5;
    BYTE res = 0;
    while (attempt > 0 && res != 1)
    {
        SD_CS_HIGH;
        sd_rx();
        SD_CS_LOW;

        res = send_cmd(CMD0, 0);    
        
        // sprintf(resBuf, "cmd0 res: %02X", res);
        // SendUARTStr(resBuf);
        
        attempt--;
    }
    
	if (res == 1) {			/* Enter Idle state */
        // SendUARTStr("idle state");        
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
            // SendUARTStr("cmd8");
            
			for (n = 0; n < 4; n++) ocr[n] = sd_rx();	/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd(ACMD41, 1UL << 30) == 0) break;
					delay_ms(1);
				}
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = sd_rx();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
            // SendUARTStr("acmd41");
            
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
				if (send_cmd(cmd, 0) == 0) break;
				delay_ms(1);
			}
			if (!tmr || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
    
    // SendUARTStr("init_cont");    
    
	CardType = ty;
	deselect();

	if (ty) {			/* Initialization succeded */
        // SendUARTStr("init_ok");
        
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
		FCLK_FAST;
	}

    // char resbuf[32];
    // sprintf(resBuf, "diskinit: %d", Stat);
    // SendUARTStr(resBuf); 
    
    _last_sd_init_ok = (Stat == RES_OK);
    
	return Stat;
}

unsigned char is_sd_init_ok()
{
    return _last_sd_init_ok;
}

DSTATUS disk_initialize_usb ()
{    
    ch376_sleep();
    delay_ms(500);

    unsigned char is376OK = ch376_init();    
    if (is376OK)
    {
        ch376_checkIntMessage();            
        
        BOOL connected = ch376_getUSBMountStatus();
        if (connected)
        {
            return RES_OK;
        }
        else {
            return STA_NODISK;
        }
    }
    else {
        return STA_NOINIT;
    }
}

DSTATUS disk_initialize(BYTE pdrv) {
    // SendUARTStr("disk_initialize");
    
    if (isUsingCH376)
    {
        // SendUARTStr("Init USB");
        return disk_initialize_usb();
    }
    else {
        // SendUARTStr("Init SD");
        return disk_initialize_sd(pdrv);
    }
}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status_sd (
	BYTE pdrv		/* Physical drive nmuber (0) */
)
{
	if (pdrv) return STA_NOINIT;	/* Supports only single drive */
	return Stat;
}

DSTATUS disk_status_usb() {
    BOOL connected = ch376_getUSBMountStatus();
    if (connected)
    {
        return 0;
    }
    else {
        return STA_NODISK;
    }
}

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0) */
)
{
    // SendUARTStr("disk_status");
    
    if (isUsingCH376)
    {
        return disk_status_usb();
    }
    else {
        return disk_status_sd(pdrv);
    }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read_sd (
	BYTE pdrv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	BYTE cmd;


	if (pdrv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	cmd = count > 1 ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	if (send_cmd(cmd, sector) == 0) {
		do {
			if (!rcvr_datablock(buff, 512)) break;
			buff += 512;
		} while (--count);
		if (cmd == CMD18) send_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_read_usb (
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
    // CH376 only supports reading 1 or 4 sectors at the same time
    // But FATFS may read 3, we therefore read only 1 sector at a time for cases of 3 sectors.
    BYTE *buffPT = buff;
    UINT c = 0;
    UINT readCnt = 0;
    UINT cnt = 0;
    if (count == 1 || count == 4)
    {
        cnt = ch376_sectorRead(sector, count, buffPT);        
        readCnt += cnt;
        buffPT += cnt;
        
        if (cnt != SECTOR_SIZE)
        {            
            // read 0 bytes indicating error            
            SendUARTStr("sctRdE1");
            return RES_ERROR;
        }
    }
    else {
        for (c = 0; c < count; c++)
        {        
            cnt = ch376_sectorRead(sector + c, 1, buffPT);        
            readCnt += cnt;
            buffPT += cnt;

            if (cnt != SECTOR_SIZE)
            {            
                // read 0 bytes indicating error            
                SendUARTStr("sctRdE2");
                return RES_ERROR;
            }
        }
    }
    
    return RES_OK;
}

DRESULT disk_read (
	BYTE pdrv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{    
    if (isUsingCH376)
    {
        return disk_read_usb(buff, sector, count);
    }
    else 
    {
        return disk_read_sd(pdrv, buff, sector, count);
    }
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write_sd (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	if (pdrv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_write_usb (
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
    // char buf[32];
            
    // CH376 only supports writing 1 or 4 sectors at the same time
    // But FATFS may writing 3, we therefore read only 1 sector at a time!
    BYTE *buffW = (BYTE *)buff;
    UINT c = 0;
    UINT writeCnt = 0;
    UINT cnt = 0;
    for (c = 0; c < count; c++)
    {
        // sprintf(buf, "R: %lu SECT: #%u/%u", sector, c, count);
        // SendUARTStr(buf);
        cnt = ch376_sectorWrite(sector + c, 1, buffW);
        writeCnt += cnt;
        buffW += SECTOR_SIZE;
            
        if (cnt == SECTOR_SIZE)
        {
            // write ok
        }
        else {            
            // write 0 bytes, indicating failure            
            SendUARTStr("DsWrEr");
            return RES_ERROR;
        }
    }
    
    // sprintf(buf, "ROK: %lu %u %u", sector, count, readCnt);
    // SendUARTStr(buf);
    
    return RES_OK;
}

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
    // char buf[32];
    // sprintf(buf, "disk_write: %lu %u", sector, count);
    // SendUARTStr(buf);
    
    
    // FATFS called disk_read again right after disk_write to verify. This reduces speed by half
    // We can further improve the speed by replacing the following in ff.c line 3689 with memcpy
    // InterestingLY, setting FS_TINY to 1 remove the disk_read but does not help with the speed much, 
    // probably because the buffer is no longer in use
    // On dsPIC33EP @ 90MIPS (180MHz overclocked), write speed is 70-256KB/s, read speed is 200-256KB/s @ 12MHz SPI speed    
    /*
            if (fp->sect != sect && 		// Fill sector cache with file data
            fp->fptr < fp->obj.objsize &&
            disk_read(fs->drv, fp->buf, sect, 1) != RES_OK) {
                ABORT(fs, FR_DISK_ERR);
     */ 
    if (isUsingCH376)
    {
        return disk_write_usb(buff, sector, count);
    }
    else {
        return disk_write_sd(pdrv, buff, sector, count);
    }
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl_sd (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csize;


	if (pdrv) return RES_PARERR;

	res = RES_ERROR;

	if (Stat & STA_NOINIT) return RES_NOTRDY;

	switch (cmd) {
	case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
		if (select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csize << 10;
			} else {					/* SDC ver 1.XX or MMC*/
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (CardType & CT_SD2) {	/* SDv2? */
			if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
				sd_rx();
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) sd_rx();	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDv1 or MMCv3 */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDv1 */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMCv3 */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	/* Following commands are never used by FatFs module */

	case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
		if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
			&& rcvr_datablock(ptr, 16))
			res = RES_OK;
		break;

	case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
		if (send_cmd(CMD10, 0) == 0		/* READ_CID */
			&& rcvr_datablock(ptr, 16))
			res = RES_OK;
		break;

	case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 4; n; n--) *ptr++ = sd_rx();
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
		if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			sd_rx();
			if (rcvr_datablock(ptr, 64))
				res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	deselect();

	return res;
}

DRESULT disk_ioctl_usb (
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{    
    // Ignore IOCTL for USB. Almost of them are for trimming of deleted data. Just return OK for all.
    // GET_SECTOR_COUNT is for getting total sectors during partition creation, which we don't support via the CH375 anyway.
    return RES_OK;
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    // char buf[32];
    // sprintf(buf, "disk_ioctl: %u", cmd);
    // SendUARTStr(buf);
    
    if (isUsingCH376)
    {
        return disk_ioctl_usb(cmd, buff);
    }
    else {
        return disk_ioctl_sd(pdrv, cmd, buff);
    }
    
}        
#endif
