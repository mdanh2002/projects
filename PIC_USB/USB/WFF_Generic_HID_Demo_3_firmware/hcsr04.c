#include "p18F4550.h"
#include "hcsr04.h"
#include <delays.h>

unsigned char hc_getDistance(unsigned int *distanceInCm)
{
	unsigned int temp;

	// set trigger to on
	HC_TRIG_OUT = 1;
	Delay10TCYx(10);
	HC_TRIG_OUT = 0;

	temp = 0;

	// wait until signal turn high
	while (!HC_ECHO_INP && temp < 65535)
	{
		temp++;
	}
	if (temp >= 65535) return HC_TIMEOUT; //timeout, perhaps module not connected

	// wait until signal turn low
	while (HC_ECHO_INP && temp < 60000) 
	{
		temp++;
	}
	if (temp >= 60000) return HC_NOTINRANGE; //timeout, perhaps module not connected

	// the distance is the amount of time the echo signal remain high
	// need to be calibrated against a known obstacle
	*distanceInCm = temp / 35;

	return HC_DETECTED;
}