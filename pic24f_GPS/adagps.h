/*
Adapted from https://github.com/adafruit/Adafruit-GPS-Library/
*/
#ifndef _ADAFRUIT_GPS_H
#define _ADAFRUIT_GPS_H

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG "$PMTK185,0*22"
#define PMTK_LOCUS_LOGSTARTED "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"
#define LOCUS_OVERLAP 0
#define LOCUS_FULLSTOP 1

// standby command & boot successful message
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36" // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31"

// request for updates on antenna status
#define PGCMD_ANTENNA "$PGCMD,33,1*6C"
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D"

// how long to wait when we're looking for a response
#define MAXWAITSENTENCE 5

// boolean data type
#define boolean unsigned char
#define true 1
#define false 0

// other data types
#define uint8_t unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long

// structures
typedef struct _GPS_DATE_INFO
{
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint16_t milliseconds;
} GPS_DATE_INFO;

typedef struct _GPS_SIGNAL_INFO
{
	boolean fix;
	uint8_t fixquality;
	uint8_t satellites;
} GPS_SIGNAL_INFO;

typedef struct _GPS_LOCATION_INFO
{
	float latitude;
	float longitude;
	float geoidheight;
	float altitude;
	float speed;
	float angle;
	float magvariation;
	float HDOP;
} GPS_LOCATION_INFO;

// internal functions
void GPS_common_init(void);
char *GPS_lastNMEA(void);
void GPS_pause(boolean b);
boolean is_GPS_paused();
boolean GPS_parse(char *);
char GPS_read(void);
boolean GPS_newNMEAreceived(void);

// get GPS information
GPS_DATE_INFO GPS_getDateInfo();
GPS_SIGNAL_INFO GPS_getSignalInfo();
GPS_LOCATION_INFO GPS_getLocationInfo();

#endif
