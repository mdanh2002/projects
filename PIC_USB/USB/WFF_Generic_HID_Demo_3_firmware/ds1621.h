// DS1621 Interface Functions
#define DS1621_W 0x90
#define DS1621_R 0x91

char 	ds1621_init(void);					//init the DS1621 module, should be called once only. Return 1 if OK, 0 if failed.
float 	ds1621_read_temp(void); 			//return the temperature to to 0.5C accuracy
float 	ds1621_read_temp_precise(void);		//return the temperature to much higher accuracy but takes slightly longer
