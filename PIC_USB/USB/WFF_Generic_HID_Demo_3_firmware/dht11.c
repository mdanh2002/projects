#include "dht11.h"
#include <delays.h>
#include "p18F4550.h"
#include "uart.h"
#include <string.h>

// correct @ 20 MHz
// delay in multiple of 1uS
void delay_us(int count)
{
	char j;
	for (j=0;j<count;j++)
		Delay10TCYx(1);
}

// delay in multiple of 1ms
void delay_ms(int count)
{
	char j;
	for (j=0;j<count;j++)
		Delay1KTCYx(12);
}

unsigned char dht11_read(unsigned char* temperature, unsigned char* humidity)
{
	unsigned char j;
	unsigned char cnt = 7;
	unsigned char idx = 0;
	unsigned char bits[5];
	unsigned char sum;
	unsigned int waitCount;

	// default all 0
	for (j=0;j<5;j++)
		bits[j]=0;
	
	// pulse the bus low to start the data reading
	DHT11_DIR=0;
	DHT11_OUT=0;
	Delay1KTCYx(23); //at least 18ms, starting pulse for the DH11 to know that we're reading data
	DHT11_OUT=1;
	Delay10TCYx(40); //at least 30uS for the DH11 to be ready
	DHT11_DIR=1;

	// wait for the data line to turn low, e.g. start bit
	// if the wait is too long, abort operation, e.g. sensor has failed
	waitCount = 10000;
	while(DHT11_IN==1)
		if (waitCount-- == 0) return 0;

	// wait for start bit
	waitCount = 10000;
	while(DHT11_IN==0)
		if (waitCount-- == 0) return 0;

	waitCount = 10000;
	while(DHT11_IN==1)
		if (waitCount-- == 0) return 0;

	// read 40-bit of data
	for(j=0;j<40;j++)
	{
		waitCount = 10000;
		while(DHT11_IN==0)
			if (waitCount-- == 0) return 0;

		Delay10TCYx(30); //at least 30ms, after which if the bus is still high, considered this bit a 1

		// if the bit is high, set the bit and wait until bus turn low, e.g. next bit
		if(DHT11_IN==1)
		{
			bits[idx] |= (1<<cnt);
			
			waitCount = 10000;
			while (DHT11_IN==1)
				if (waitCount-- == 0) return 0;
		}

		// finish a byte, move on to next
		if (cnt==0)
		{
			cnt = 7;
			idx++;
		}
		else
			cnt--;
	}

	// checksum
	sum = (bits[0]+bits[2])&0xFF;

	// return the integral part of the temperature and humidity only
	*temperature = bits[2];
	*humidity = bits[0];

	return (sum==bits[4]);
}