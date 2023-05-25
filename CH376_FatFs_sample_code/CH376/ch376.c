#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ch376.h"
#include "../config.h"
#include "../spi_lib.h"

BOOL _ch376OK = FALSE;                      // Whether the CH376 module is itself OK (not counting USB)
BOOL _deviceConnected = FALSE;               
BOOL _deviceMounted = FALSE;               // FALSE - USB detached, TRUE - mounted
uint8_t _driveSource = 0;                   // 0 = USB, 1 = SD

// <editor-fold defaultstate="collapsed" desc="SPI Helper">
void ch376_sendSpiByte(unsigned char X)        
{
    spi2Write(X);
}

uint8_t ch376_readSpiByte() {
    return spi2Write(0x00);
}
void ch376_spiBeginTransfer() {
    CH376_CS = 0;
    delay_us(1);
}

void ch376_spiEndTransfer() { 
    delay_us(1);
    CH376_CS = 1;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Initialization">
uint8_t ch376_setMode(uint8_t mode){
	uint8_t tmpReturn = 0;
	
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_SET_USB_MODE);
    delay_us(2);
    ch376_sendSpiByte(mode);
    delay_us(2);
    tmpReturn = ch376_readSpiByte();
    ch376_spiEndTransfer();
	
	return tmpReturn; // success or fail
}

BOOL  ch376_pingDevice(){
	BOOL tmpReturn = 0;
    
    unsigned char testByte = 0x35;
    
    // SendUARTStr("CHTst");

    // cannot put >=0 with c--, it will wrap and hang!
    for (testByte = 255; testByte > 0; testByte--)
    {
        unsigned char expected = (~testByte & 0xFF);        
        ch376_spiBeginTransfer();        
        ch376_sendSpiByte(CMD_CHECK_EXIST);    
        delay_us(2);
        ch376_sendSpiByte(testByte); 
        delay_us(2);
        unsigned char res = ch376_readSpiByte();

        // char buf[32];
        // sprintf(buf, "V:%u, R:%u, C:%u", testByte, res, expected);
        // SendUARTStr(buf);

        if(res == expected){
            // SendUARTStr("Valid Resp");
            tmpReturn = TRUE; //true
        }
        else {
            tmpReturn = FALSE;            
            break;
        }
        ch376_spiEndTransfer();            
        delay_ms(1);
    }
            
    if (tmpReturn)
    {
        SendUARTStr("CHOk");   
    }
    else {
        SendUARTStr("CHEr");   
    }
    
	return tmpReturn;
}

uint8_t ch376_getInterrupt(){
	uint8_t _tmpReturn = 0;
    
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_GET_STATUS);
    delay_us(2);
    _tmpReturn = ch376_readSpiByte();

    ch376_spiEndTransfer();
    
	return _tmpReturn;
}

BOOL ch376_checkIntMessage(){ 
	BOOL intRequest = FALSE;
    
    uint8_t tmpReturn = ch376_getInterrupt(); // get int message    
    
    // char buf[16];
    // sprintf(buf, "IntVal: %d", tmpReturn);
    // SendUARTStr(buf);
    
    switch(tmpReturn){ // 0x15 device attached, 0x16 device disconnect
        case ANSW_USB_INT_CONNECT:
            SendUARTStr("USBFnd");
            delay_ms(1);
            
            intRequest = TRUE;
            ch376_driveAttach();//device attached
            
            _deviceConnected = TRUE;
            
            break;
        case ANSW_USB_INT_DISCONNECT:
            SendUARTStr("USBRmv");
            
            intRequest = TRUE;
            ch376_driveDetach();//device detached
            
            _deviceConnected = FALSE;
            
            break;
    }
        
	return intRequest;
}

void ch376_clearvars()
{
    _ch376OK = FALSE;                      
    _deviceConnected = FALSE;               
    _deviceMounted = FALSE;               
    _driveSource = 0;
}

void ch376_reset() {
    CH376_CS = 1;
    delay_us(100);
    
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_RESET_ALL);
    ch376_spiEndTransfer();    
    
    delay_ms(100);// wait after reset command
}

BOOL ch376_init(){    
    // CH376 flag only set once for info only, no need to reset upon init
    // _ch376OK = FALSE;
    
    // we do not support hot swap for now, and FATFS may retry init multiple times
    // hence there is no need to reset deviceConnect
    // _deviceConnected = FALSE;
    
    _deviceMounted = FALSE;
    
    ch376_reset();
    
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_SET_SD0_INT);
    delay_us(2);
    ch376_sendSpiByte(0x16);
    
    // ch376_sendSpiByte(0x10);        // external interrupt pin (INT)
    ch376_sendSpiByte(0x90);           // SDO as interrupt
    
    ch376_spiEndTransfer();
    delay_us(50);// wait after reset command
    
    // SendUARTStr("ping test");
	BOOL _controllerReady = ch376_pingDevice();// check the communication
	if(_controllerReady) 
    {
        // SendUARTStr("CHRdy");
        
        _ch376OK = TRUE;
    }
    else {
        SendUARTStr("CHEr");   
    }

    ch376_setMode(MODE_HOST_0);
	ch376_checkIntMessage();
    
    return _ch376OK;
}

void ch376_sleep()
{
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_ENTER_SLEEP);   
    ch376_spiEndTransfer();
}

uint8_t ch376_spiWaitInterrupt(){
    uint32_t count = 0;
    
	while (CH376_INT){
        // SendUARTStr("spWaitInterrupt");
        
        count++;
        
        if (count > 4000000ULL)
        {
            SendUARTStr("spWtInEr");   
            break;
        }        
	}
    
	return ch376_getInterrupt();
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Mount and Unmount">
uint8_t ch376_mount(){ // return ANSWSUCCESS or ANSW DISK DISCON
	uint8_t tmpReturn = 0;
	
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_DISK_MOUNT);    
    ch376_spiEndTransfer();
    delay_us(2);
    tmpReturn = ch376_spiWaitInterrupt();
	                
	return tmpReturn;
}

void ch376_driveAttach(){
    uint8_t tmpReturn = 0;
    uint8_t a = 0;
    
    SendUARTStr("USBAtt");
    if (_driveSource == 0){//if USB
        // SendUARTStr("Change USB mode");
        
        ch376_setMode(MODE_HOST_1); //TODO:if 5F failure
        delay_us(10);
        ch376_setMode(MODE_HOST_2);        
        delay_us(10);
        
        tmpReturn = ch376_spiWaitInterrupt();
    }
                    
    // char buf[16];
    // sprintf(buf, "mode res: %d", tmpReturn);
    // SendUARTStr(buf);
    
    delay_ms(50);
    if ((tmpReturn == ANSW_USB_INT_CONNECT) || (!tmpReturn)){
        for(a = 0; a < 16; a++) {
            SendUARTStr("USBMnt");
            
            tmpReturn = ch376_mount();
            
            // sprintf(buf, "mnt res: %d", tmpReturn);
            // SendUARTStr(buf);
            
            if (tmpReturn == ANSW_USB_INT_SUCCESS){
                SendUARTStr("USBMntd");                
                _deviceMounted = TRUE;
                
                break;
            }
            else {
                SendUARTStr("USBMntEr");
                _deviceMounted = FALSE;
            }
            
            delay_ms(50);
        }
    } 
    else 
    {
        SendUARTStr("USBAtEr");
        
        ch376_driveDetach();
    }    
}

void ch376_driveDetach(){
	if(_driveSource == 0){//if USB
		ch376_setMode(MODE_HOST_0);
	}
    
    _deviceMounted = FALSE;    
    _deviceConnected = FALSE;
}

BOOL ch376_getUSBMountStatus(){
	return _deviceMounted;
}

BOOL ch376_getUSBConnectedStatus()
{
    return _deviceConnected;
}

BOOL ch376_getChipStatus()
{
    return _ch376OK;
}

void ch376_setSource(uint8_t inpSource){
	if(_driveSource != inpSource){
		_driveSource = inpSource;
		if(_driveSource == 1){// SD mode
			ch376_driveDetach();
			ch376_setMode(MODE_DEFAULT);
			ch376_setMode(MODE_HOST_SD);
			ch376_driveAttach();
		} else { // USB mode
			ch376_driveDetach();
			ch376_driveAttach();
		}
	}
}

uint8_t ch376_getSource(){
	return _driveSource;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Raw Sector Read Write">
unsigned int ch376_sectorRead(uint32_t lba, uint8_t count, unsigned char* rawSector) {
    uint16_t totalRead = 0;
    
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_DISK_READ);
    delay_us(2);
    ch376_sendSpiByte( lba & 0xFF );
    ch376_sendSpiByte( (lba >> 8) & 0xFF);
    ch376_sendSpiByte( (lba >> 16) & 0xFF);
    ch376_sendSpiByte( (lba >> 24) & 0xFF);
    ch376_sendSpiByte(count);
    ch376_spiEndTransfer();
        
    unsigned char* rawSectorPt = rawSector;
    
    // char buf[32];
    // sprintf(buf, "rd: %lu %u", lba, count);
    // SendUARTStr(buf);
    
    // Each sector returned as 8 data blocks with 64 bytes each
    uint8_t intCount = 0;
    uint8_t attempt = 0;
    uint8_t dataLen = 0;
    uint8_t s = 0;
    uint8_t tmpReturn = 0;
    while (intCount < 9)        // 9 Interrupts for 8 sectors, last one to signal complete
    {        
        if (attempt > 3)
        {
            SendUARTStr("DskRdEr");
            break;
        }
        
        tmpReturn = ch376_spiWaitInterrupt();
        
        // sprintf(buf, "wi: %u", tmpReturn);
        // SendUARTStr(buf);
        
        if (tmpReturn == ANSW_USB_INT_DISK_READ)
        {                        
            // sector info available, retrieving
            ch376_spiBeginTransfer();
            ch376_sendSpiByte(CMD_RD_USB_DATA0);
            delay_us(2);
            dataLen = ch376_readSpiByte();
            totalRead += dataLen;            
        
            for (s = 0; s < dataLen; s++){
                *rawSectorPt++ = ch376_readSpiByte(); 
            }
            ch376_spiEndTransfer();
            
            // reading next 
            ch376_spiBeginTransfer();
            ch376_sendSpiByte(CMD_DISK_READ_GO);
            ch376_spiEndTransfer();
            
            intCount++;
            attempt = 0;
        }
        else if (tmpReturn == ANSW_USB_INT_SUCCESS) {
            // no more sector
            // SendUARTStr("Read Done");
            
            break;
        }
        else if (tmpReturn == ANSW_USB_INT_DISK_ERR)
        {
            SendUARTStr("RdEr");
            break;
        }
        
        attempt++;
    }
    
    // sprintf(buf, "Read: %u bytes", totalRead);
    // SendUARTStr(buf);
            
    return totalRead;
}

unsigned int ch376_sectorWrite(uint32_t lba, uint8_t count, unsigned char* rawSector) {
    uint16_t totalWrite = 0;
    
    ch376_spiBeginTransfer();
    ch376_sendSpiByte(CMD_DISK_WRITE);
    delay_us(2);
    ch376_sendSpiByte( lba & 0xFF );
    ch376_sendSpiByte( (lba >> 8) & 0xFF);
    ch376_sendSpiByte( (lba >> 16) & 0xFF);
    ch376_sendSpiByte( (lba >> 24) & 0xFF);
    ch376_sendSpiByte(count);
    ch376_spiEndTransfer();
    
    // char buf[32];
    // sprintf(buf, "wr: %lu %u", lba, count);
    // SendUARTStr(buf);
    
    unsigned char* rawSectorPt = rawSector;
    
    // Each sector returned as 8 data blocks with 64 bytes each
    uint8_t intCount = 0;
    uint8_t attempt = 0;

    uint8_t s = 0;
    uint8_t tmpReturn = 0;
    while (intCount < 9)        // 9 Interrupts for 8 sectors, last one to signal complete
    {        
        if (attempt > 3)
        {
            SendUARTStr("WrEr");
            break;
        }
        
        tmpReturn = ch376_spiWaitInterrupt();
        
        // sprintf(buf, "wi: %u", tmpReturn);
        // SendUARTStr(buf);
        
#define CH376_SECTOR_BLOCK_SIZE     64
        
        if (tmpReturn == ANSW_USB_INT_DISK_WRITE)
        {                        
            // write sector block
            ch376_spiBeginTransfer();
            ch376_sendSpiByte(CMD_WR_HOST_DATA);
            delay_us(2);
            ch376_sendSpiByte(CH376_SECTOR_BLOCK_SIZE);
        
            for (s = 0; s < CH376_SECTOR_BLOCK_SIZE; s++){
                ch376_sendSpiByte(*rawSectorPt++);
            }
            ch376_spiEndTransfer();
            totalWrite += CH376_SECTOR_BLOCK_SIZE;            
                        
            // writing next 
            ch376_spiBeginTransfer();
            ch376_sendSpiByte(CMD_DISK_WRITE_GO);
            ch376_spiEndTransfer();
            
            intCount++;
            attempt = 0;
        }
        else if (tmpReturn == ANSW_USB_INT_SUCCESS) {
            // SendUARTStr("Write Done");
            
            // no more sector
            break;
        }
        else if (tmpReturn == ANSW_USB_INT_DISK_ERR)
        {
            SendUARTStr("WrEr2");
            break;
        }
        
        attempt++;
    }
    
    // sprintf(buf, "Written: %u bytes", totalWrite);
    // SendUARTStr(buf);
            
    return totalWrite;    
}
// </editor-fold>
