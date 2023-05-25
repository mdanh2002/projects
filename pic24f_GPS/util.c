#include <string.h>
#include <stdlib.h>

/*******************************************************************
* FUNCTION: ftoa
* AUTHOR = Michael L Anderson, PhD
* COMPANY = Black Oak Engineering
* URL = www.blackoakeng.com
* EMAIL = office @ blackoakeng.com < remove spaces
* COMPILER = MPLAB C18 C comp v3.35
* DATE = 10 Feb 10
*
* PARAMETERS: float f
*
* DESCRIPTION: Converts a float to fixed point string. This is obviously
* quite limited but it works well for many embedded situations and it is
* very compact. Note - C18 compiler printf() fcns do not support
* floating point args; this is one simple way to fill the deficit.
*
* RETURNS: C string pointer
*

*******************************************************************/

char bfr1[21], bfr2[11];

char *ftoa(float f)
{
	int ctr = 0;
	unsigned u;
	
	if (f < 0.0) { bfr1[ctr++] = '-'; f *= -1; }
	bfr1[ctr] = 0; // Terminate C string
	
	if (f > 0xFFFF)
	{
	strcpy(bfr1, "Overrange\r\n");
	return(bfr1);
	}
	
	u = f;
	itoa(bfr2, u, 10);
	strcat(bfr1, bfr2);
	ctr = strlen(bfr1);
	bfr1[ctr++] = '.';
	bfr1[ctr] = 0;
	
	f -= u;
	u = 10000 * f; // 4 sig figs after fixed dec pt
	
	if (u < 9) strcpy(bfr2, "000"); // Pack the leading zeros. This actually
	else if (u < 99) strcpy(bfr2, "00"); // compiles tighter than a loop.
	else if (u < 999) strcpy(bfr2, "0");
	else bfr2[0] = 0;
	
	strcat(bfr1, bfr2);
	itoa(bfr2, u, 10);
	strcat(bfr1, bfr2);	
	
	return bfr1;
} 

