// DS1307 RTC Functions
// Adapted from http://www.ccsinfo.com/forum/viewtopic.php?t=23255

// init the DS1307. 
// After init, Return 1 if the RTC has been started previously and is running (e.g. backup battery is installed).
// Return 0 if the RTC was not running, in which case calling this function will start it
char ds1307_init(void);

// start the RTC with the number of seconds specified. Call this after the time was set for the first time
void ds1307_startRTC(unsigned short seconds);

void ds1307_set_date_time(unsigned short day, unsigned short mth, unsigned short year, unsigned short dow, unsigned short hr, unsigned short min, unsigned short sec);
void ds1307_get_date(unsigned short *day, unsigned short *mth, unsigned short *year, unsigned short *dow);
void ds1307_get_time(unsigned short *hr, unsigned short *min, unsigned short *sec);
