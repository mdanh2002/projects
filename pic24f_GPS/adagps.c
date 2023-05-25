#include "adagps.h"
#include <stdlib.h>
#include "string.h"
#include "math.h"
#include <p24fj64ga002.h>
#include "uart.h"

// acquired GPS coordinates
uint8_t hour, minute, seconds, year, month, day;
uint16_t milliseconds;

float latitude, longitude, geoidheight, altitude;
float speed, angle, magvariation, HDOP;

char lat, lon, mag;

boolean fix;
uint8_t fixquality, satellites;

boolean paused;

// how long are max NMEA lines to parse?
#define MAXLINELENGTH 120

// we double buffer: read one line in and leave one for the main program
volatile char line1[MAXLINELENGTH];
volatile char line2[MAXLINELENGTH];
// our index into filling the current line
volatile uint8_t lineidx=0;
// pointers to the double buffers
volatile char *currentline;
volatile char *lastline;
volatile boolean recvdflag;
volatile boolean inStandbyMode;

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
	if (c <= '9')
		return c - '0';
	
	if (c <= 'F')
		return (c - 'A')+10;
		
	return 0;		
}

boolean GPS_parse(char *nmea) {
	// do checksum check

	// first look if we even have one
	if (nmea[strlen(nmea)-4] == '*') {
		uint16_t sum = parseHex(nmea[strlen(nmea)-3]) * 16;
		sum += parseHex(nmea[strlen(nmea)-2]);
		
		// check checksum
		uint8_t i;
		for (i=1; i < (strlen(nmea)-4); i++) {
			sum ^= nmea[i];
		}
		if (sum != 0) {
			// bad checksum :(
			//return false;
		}
	}

	// look for a few common sentences
	if (strstr(nmea, "$GPGGA")) {
		// found GGA
		char *p = nmea;
		// get time
		p = strchr(p, ',')+1;
		float timef = atof(p);
		uint32_t time = timef;
		hour = time / 10000;
		minute = (time % 10000) / 100;
		seconds = (time % 100);

		milliseconds = fmod(timef, 1.0) * 1000;

		// parse out latitude
		p = strchr(p, ',')+1;
		latitude = atof(p);

		p = strchr(p, ',')+1;
		if (p[0] == 'N') lat = 'N';
		else if (p[0] == 'S') lat = 'S';
		else if (p[0] == ',') lat = 0;
		else return false;

		// parse out longitude
		p = strchr(p, ',')+1;
		longitude = atof(p);

		p = strchr(p, ',')+1;
		if (p[0] == 'W') lon = 'W';
		else if (p[0] == 'E') lon = 'E';
		else if (p[0] == ',') lon = 0;
		else return false;

		p = strchr(p, ',')+1;
		fixquality = atoi(p);

		p = strchr(p, ',')+1;
		satellites = atoi(p);

		p = strchr(p, ',')+1;
		HDOP = atof(p);

		p = strchr(p, ',')+1;
		altitude = atof(p);
		p = strchr(p, ',')+1;
		p = strchr(p, ',')+1;
		geoidheight = atof(p);
		return true;
	}
	if (strstr(nmea, "$GPRMC")) {
		// found RMC
		char *p = nmea;

		// get time
		p = strchr(p, ',')+1;
		float timef = atof(p);
		uint32_t time = timef;
		hour = time / 10000;
		minute = (time % 10000) / 100;
		seconds = (time % 100);

		milliseconds = fmod(timef, 1.0) * 1000;

		p = strchr(p, ',')+1;
		// Serial.println(p);
		if (p[0] == 'A')
		fix = true;
		else if (p[0] == 'V')
		fix = false;
		else
		return false;

		// parse out latitude
		p = strchr(p, ',')+1;
		latitude = atof(p);

		p = strchr(p, ',')+1;
		if (p[0] == 'N') lat = 'N';
		else if (p[0] == 'S') lat = 'S';
		else if (p[0] == ',') lat = 0;
		else return false;

		// parse out longitude
		p = strchr(p, ',')+1;
		longitude = atof(p);

		p = strchr(p, ',')+1;
		if (p[0] == 'W') lon = 'W';
		else if (p[0] == 'E') lon = 'E';
		else if (p[0] == ',') lon = 0;
		else return false;

		// speed
		p = strchr(p, ',')+1;
		speed = atof(p);

		// angle
		p = strchr(p, ',')+1;
		angle = atof(p);

		p = strchr(p, ',')+1;
		uint32_t fulldate = atof(p);
		day = fulldate / 10000;
		month = (fulldate % 10000) / 100;
		year = (fulldate % 100);

		// we dont parse the remaining, yet!
		return true;
	}

	return false;
}

char GPS_read(void) {
	char c = 0;

	// return 0 if reading is paused or if no data available in UART 2 buffer
	if (paused) return c;
	if (!U2STAbits.URXDA) 
		return c;
	else
		c = U2RXREG;
			
	//Serial.print(c);

	if (c == '$') {
		currentline[lineidx] = 0;
		lineidx = 0;
	}
	if (c == '\n') {
		currentline[lineidx] = 0;

		if (currentline == line1) {
			currentline = line2;
			lastline = line1;
		} else {
			currentline = line1;
			lastline = line2;
		}
		lineidx = 0;
		recvdflag = true;
	}

	currentline[lineidx++] = c;
	if (lineidx >= MAXLINELENGTH)
	lineidx = MAXLINELENGTH-1;

	return c;
}
// Initialization code used by all constructor types
void GPS_common_init(void) {
	recvdflag = false;
	paused = false;
	lineidx = 0;
	currentline = line1;
	lastline = line2;

	hour = minute = seconds = year = month = day = fixquality = satellites = 0; // uint8_t
	lat = lon = mag = 0; // char
	fix = false; // boolean
	milliseconds = 0; // uint16_t
	latitude = longitude = geoidheight = altitude = speed = angle = magvariation = HDOP = 0.0; // float
}

boolean GPS_newNMEAreceived(void) {
	return recvdflag;
}

void GPS_pause(boolean p) {
	paused = p;
}

boolean is_GPS_paused() {
	return paused;	
}
	
char *GPS_lastNMEA(void) {
	recvdflag = false;
	return (char *)lastline;
}

boolean GPS_waitForSentence(char *wait4me, uint8_t max) {
	char str[20];

	uint8_t i=0;
	while (i < max) {
		if (GPS_newNMEAreceived()) {
			char *nmea = GPS_lastNMEA();
			strncpy(str, nmea, 20);
			str[19] = 0;
			i++;

			if (strstr(str, wait4me))
			return true;
		}
	}

	return false;
}

GPS_DATE_INFO GPS_getDateInfo()
{
	GPS_DATE_INFO retVal;

	retVal.hour = hour;
	retVal.minute = minute;
	retVal.seconds = seconds;
	retVal.year = year;
	retVal.month = month;
	retVal.day = day;
	retVal.milliseconds = milliseconds;
	
	return 	retVal;	
}
	
GPS_SIGNAL_INFO GPS_getSignalInfo()
{
	GPS_SIGNAL_INFO retVal;

	retVal.fix = fix;
	retVal.fixquality = fixquality;
	retVal.satellites = satellites;
	
	return retVal;
}
	
GPS_LOCATION_INFO GPS_getLocationInfo()
{
	GPS_LOCATION_INFO retVal;	
	
	retVal.latitude = latitude;
	retVal.longitude = longitude;
	retVal.geoidheight = geoidheight;
	retVal.altitude = altitude;
	retVal.speed = speed;
	retVal.angle = angle;
	retVal.magvariation = magvariation;
	retVal.HDOP = HDOP;
	
	return retVal;
}	
