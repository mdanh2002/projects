#include <xc.h>
#include <string.h>
#include "trig.h"
#include "config.h"
#include "math.h"

#define M_PI    3.1415

// More decimal places
/*
__prog__ static const unsigned int __attribute__((section("sinTable16"), space(prog))) sinTable16[] = {
	0, 1144, 2287, 3430, 4571, 5712, 6850, 7987, 9121, 10252, 11380,
	12505, 13625, 14742, 15854, 16962, 18064, 19161, 20251, 21336, 22414,
	23486, 24550, 25607, 26655, 27696, 28729, 29752, 30767, 31772, 32768,
	33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243, 42125,
	42995, 43851, 44695, 45524, 46340, 47142, 47929, 48702, 49460, 50203,
	50930, 51642, 52339, 53019, 53683, 54331, 54962, 55577, 56174, 56755,
	57318, 57864, 58392, 58902, 59395, 59869, 60325, 60763, 61182, 61583,
	61965, 62327, 62671, 62996, 63302, 63588, 63855, 64103, 64331, 64539,
	64728, 64897, 65047, 65176, 65286, 65375, 65445, 65495, 65525, 65535,
};
*/

// Fewer decimal places
/*
__prog__ static const unsigned char __attribute__((section("sinTable8"), space(prog))) sinTable8[] = {
	0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44,
	49, 53, 57, 62, 66, 70, 75, 79, 83, 87,
	91, 96, 100, 104, 108, 112, 116, 120, 124, 128,
	131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
	167, 171, 174, 177, 180, 183, 186, 190, 192, 195,
	198, 201, 204, 206, 209, 211, 214, 216, 219, 221,
	223, 225, 227, 229, 231, 233, 235, 236, 238, 240,
	241, 243, 244, 245, 246, 247, 248, 249, 250, 251,
	252, 253, 253, 254, 254, 254, 255, 255, 255, 255,
};
*/

/*
https://forum.arduino.cc/index.php?topic=69723.0
Today I did some investigations in a lookup function for sin(x).  Sin(x) takes approx 120 micros and I wanted more speed, and I knew the price was precision.
As I worked only in whole degrees a lookup table was the way to go. A straightforward float[361]  (1444 bytes!) was not acceptable so folding was applied, and a mapping to bring the table back to uint8_t[91].
The code shows two tables: a 16bit and an 8bit table,
The function float isin(long x) is the core, it folds the angle, does a lookup and convert that to a float.   // isin() stands for integer_sinus()
There are straightforward implementations of icos(x) and itan(x).
The function float fsin(float x) is an interpolation function, that runs on top of isin(),
Did some first tests and isin(x) is approx 6x faster than the original sin(x) for both tables. The 16 bit table is more precise with a max error of 0.000008 compared to sin(x); The 8bit Table scores a max error of 0.0002. [whole angles only]
I think it can be done better still, so all remarks, improvements or ideas are - as allways - welcome.
Rob
*/
// Sine calculation using lookup table
// Accurate to 1 degrees only, not enough for our use, otherwise short distance calculation will be wrong
/*
float fast_sin_deg(float deg)
{
    int x = (int)deg;
    
	unsigned char pos = 1;  // positive - keeps an eye on the sign.
	if (x < 0)
	{
		x = -x;
		pos = !pos;
	}
	if (x >= 360) x %= 360;
	if (x > 180)
	{
		x -= 180;
		pos = !pos;
	}
	if (x > 90) x = 180 - x;
	
    if (pos) return sinTable8[x] * 0.003921568627; // = /255.0
	return sinTable8[x] * -0.003921568627;
    
	// if (pos) return sinTable16[x] * 0.0000152590219; // = /65535.0
	// return sinTable16[x] * -0.0000152590219;
}
float fast_cos_deg(float deg)
{
	return fast_sin_deg(90.0 - deg);
}

float fast_sin(float rad)
{
    return fast_sin_deg(rad * 180.0 / M_PI);
}
 
float fast_cos(float rad)
{
	return fast_cos_deg(rad * 180.0 / M_PI);
}
*/

// https://stackoverflow.com/questions/5000109/implement-float-sqrtfloat-x-in-c
// This is an implementation of Newton's algorithm, https://tour.golang.org/flowcontrol/8
float fast_sqrt(float x)  {
    if (x < 0)
        return 0;
    
    float z = 1.0;
    unsigned int i = 0;
    for (i=1; i <= 10; i++) {
      z -= (z*z - x) / (2*z);
    }
    return z;
}

// Simple and more accurate implementation but slower due to usage of for loop
// https://stackoverflow.com/questions/2284860/how-does-c-compute-sin-and-other-math-functions
// Taylor expansion: sin(x) := x - x^3/3! + x^5/5! - x^7/7! + ...
// You would keep adding terms until either the difference between them is lower than an accepted tolerance level or just for a finite amount of steps (faster, but less precise). 
float fast_sin(float rad)
{
	float res = 0, pow = rad, fact = 1;
    unsigned char i = 0;
	for (i = 0; i<5; i++)
	{
		res += pow / fact;
		pow *= -1 * rad*rad;
		fact *= (2 * (i + 1))*(2 * (i + 1) + 1);
	}
	return res;
}

float fast_cos(float rad)
{
	return fast_sin(M_PI/2 - rad);
}

float fast_tan(float rad)
{
	return fast_sin(rad) / fast_cos(rad);
}

float fast_asin(float x)
{
    return M_PI/2 - fast_acos(x);
}

// Adapted from https://forum.arduino.cc/index.php?topic=489657.0
// X = -1.0 .. 1.0
float fast_acos(float x)
{
	float negate = (x < 0) ? 1.0 : 0.0;
	x = x > 0 ? x : -x;
	float ret = -0.0187293;
	ret = ret * x;
	ret = ret + 0.0742610;
	ret = ret * x;
	ret = ret - 0.2121144;
	ret = ret * x;
	ret = ret + M_PI / 2;
	ret = ret * fast_sqrt(1.0 - x);
	ret = ret - 2 * negate * ret;
	return negate * M_PI + ret;
}

// How to find a fast floating-Point atan2 approximation
// https://www.dsprelated.com/showarticle/1052.php
float fast_atan(float z)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    return (n1 + n2 * z * z) * z;
}

float fast_atan2(float y, float x)
{
    if (x != 0.0f)
    {
        if (fabs(x) > fabs(y))
        {
            const float z = y / x;
            if (x > 0.0)
            {
                // atan2(y,x) = atan(y/x) if x > 0
                return fast_atan(z);
            }
            else if (y >= 0.0)
            {
                // atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
                return fast_atan(z) + M_PI;
            }
            else
            {
                // atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
                return fast_atan(z) - M_PI;
            }
        }
        else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
        {
            const float z = x / y;
            if (y > 0.0)
            {
                // atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
                return -fast_atan(z) + M_PI/2;
            }
            else
            {
                // atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
                return -fast_atan(z) - M_PI/2;
            }
        }
    }
    else
    {
        if (y > 0.0f) // x = 0, y > 0
        {
            return M_PI/2;
        }
        else if (y < 0.0f) // x = 0, y < 0
        {
            return -M_PI/2;
        }
    }
    return 0.0f; // x,y = 0. Could return NaN instead.
}

unsigned int distanceFromPoint(float lat1, float lng1, float lat2, float lng2)
{
	float degToRadFactor = M_PI/180.0;

	float cosLat1 = fast_cos(degToRadFactor * lat1);
	float sinLat1 = fast_sin(degToRadFactor * lat1);
	float cosLng1 = fast_cos(degToRadFactor * lng1);
	float sinLng1 = fast_sin(degToRadFactor * lng1);
	float cosLat2 = fast_cos(degToRadFactor * lat2);
	float sinLat2 = fast_sin(degToRadFactor * lat2);
	float cosLng2 = fast_cos(degToRadFactor * lng2);
	float sinLng2 = fast_sin(degToRadFactor * lng2);

	// longitude and latitude are in degrees, no need to convert to radians
	// radius_of_earth = 6378 km
    float res = 6378 * fast_acos(cosLat1*cosLng1*cosLat2*cosLng2 + cosLat1*sinLng1*cosLat2*sinLng2 + sinLat1*sinLat2);
	return (unsigned int)res;
}

const char const *bearings[] = {"NE", "E", "SE", "S", "SW", "W", "NW", "N"};

// Determine the direction from one lat/lon to another 
// https://stackoverflow.com/questions/3209899/determine-compass-direction-from-one-lat-lon-to-the-other
void compassDirFromPoint(float lat1, float lng1, float lat2, float lng2, char dirStr[])
{
    float degToRadFactor = M_PI/180.0;
    float dLon = (lng2 - lng1) * degToRadFactor;    
    float y = fast_sin(dLon) * fast_cos(lat2 * degToRadFactor);    
    float x = fast_cos(lat1 * degToRadFactor) * fast_sin(lat2 * degToRadFactor) - fast_sin(lat1 * degToRadFactor) * fast_cos(lat2 * degToRadFactor) * fast_cos(dLon);
    float brng = fast_atan2(y, x) * 180.0 / M_PI;
    
    float index = brng - 22.5;
    if (index < 0)
        index += 360;
    
    unsigned char ind = (unsigned char)(index / 45);
    strcpy(dirStr, bearings[ind]);
}