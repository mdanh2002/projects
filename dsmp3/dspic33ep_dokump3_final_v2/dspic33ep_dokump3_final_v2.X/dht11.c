#include <xc.h>
#include <string.h>
#include "config.h"
#include "dht11.h"

unsigned char dht11_read(unsigned char* temperature, unsigned char* humidity)
{
#define MAX_WAIT_COUNT 60000
    
	unsigned char j;
	unsigned char cnt = 7;
	unsigned char idx = 0;
	unsigned char bits[5];
	unsigned char sum;
	unsigned int waitCount;

	// default all 0
	memset(bits, 0, sizeof(bits));
	
    // default direction for DHT11 to reset any previous read
    DHT11_DIR = 0;
    DHT11_OUT = 1;
    delay_ms(500);
        
	// pulse the bus low to start the data reading
	DHT11_DIR=0;
	DHT11_OUT=0;
	delay_ms(23); //at least 18ms, starting pulse for the DH11 to know that we're reading data
	DHT11_OUT=1;
	delay_us(40); //at least 30uS for the DH11 to be ready
	DHT11_DIR=1;

	// wait for the data line to turn low, e.g. start bit
	// if the wait is too long, abort operation, e.g. sensor has failed
	waitCount = MAX_WAIT_COUNT;
	while(DHT11_IN==1)
    {
		if (waitCount-- == 0) 
        {
            // SendUART('Z');
            return 0;
        }
    }

	// wait for start bit
	waitCount = MAX_WAIT_COUNT;
	while(DHT11_IN==0)
    {
		if (waitCount-- == 0) 
        {
            // SendUART('Y');
            return 0;
        }
    }

	waitCount = MAX_WAIT_COUNT;
	while(DHT11_IN==1)
    {
		if (waitCount-- == 0) 
        {
            // SendUART('X');
            return 0;
        }
    }
        
	// read 40-bit of data
	for(j=0;j<40;j++)
	{
		waitCount = MAX_WAIT_COUNT;
		while (DHT11_IN == 0)
        {
			if (waitCount-- == 0) 
            {
                // SendUART('A');
                return 0;
            }
        }

		delay_us(40);

		// if the bit is high, set the bit and wait until bus turn low, e.g. next bit
		if (DHT11_IN == 1)
		{
			bits[idx] |= (1<<cnt);
			
			waitCount = MAX_WAIT_COUNT;
			while (DHT11_IN==1)
            {
				if (waitCount-- == 0) 
                {
                    // SendUART('B');
                    return 0;
                }
            }
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
	sum = (bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF;

	// return the integral part of the temperature and humidity only
	*temperature = bits[2];
	*humidity = bits[0];
    
    /*
    char buf[32];
    sprintf(buf, "%d %d %d %d %d %d", bits[0], bits[1], bits[2], bits[3], bits[4], sum);
    SendUARTStr(buf);
    */
    
	return (sum == bits[4]);
}