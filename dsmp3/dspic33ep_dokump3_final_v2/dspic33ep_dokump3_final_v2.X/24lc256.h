#ifndef _24LC256_H_
#define	_24LC256_H_

// 8-bit address, A2=A1=A0=0
#define EEPROM_ADDRESS_W  0b10100000
#define EEPROM_ADDRESS_R  0b10100001

// size of page in bytes
#define EEPROM_PAGE_SIZE            (64)

// address used for storing config data
#define EEPROM_CFG_DATA_PAGE        (1 * EEPROM_PAGE_SIZE)

// address for storing radio band presets
// each preset is 2 bytes, mmore than enough for some bands, which are ok
// each page is 64 bytes, can store 32 preset
// we make sure that preset offset for each band always begins on page boundary (divisible by 64)
#define EEPROM_MAX_BAND_PRESETS        (512)
#define RADIO_INVALID_PRESET_IND       (0xFFFF)
#define EEPROM_MW_PRESETS_OFFSET       (2 * EEPROM_PAGE_SIZE)
#define EEPROM_FM_PRESETS_OFFSET       (EEPROM_MW_PRESETS_OFFSET + EEPROM_MAX_BAND_PRESETS * 2)
#define EEPROM_SW_PRESETS_OFFSET       (EEPROM_FM_PRESETS_OFFSET + EEPROM_MAX_BAND_PRESETS * 2)
#define EEPROM_LW_PRESETS_OFFSET       (EEPROM_SW_PRESETS_OFFSET + EEPROM_MAX_BAND_PRESETS * 2)

// address used for testing EEPROM I/O function. Do not store data here, else it will be lost
// EEPROM size 32768 bytes. This is the address of the last page
#define EEPROM_SELF_TEST_PAGE          (511 * EEPROM_PAGE_SIZE)

// Write cycle time to wait for write to finish
// Add some buffer for safety
// Datasheet page 4
#define EEPROM_WRITE_CYCLE_TIME_MS  6

void eeprom_writeByte(unsigned int address, unsigned char data);
unsigned char eeprom_readByte(unsigned int address);

void eeprom_writePage(unsigned int address, unsigned char size, unsigned char *buffer);
void eeprom_readPage(unsigned int address, unsigned char size, unsigned char *buffer);
    
#endif	

