#ifndef _TRIG_HELPER_H_
#define	_TRIG_HELPER_H_

// Fast square root approximation
float fast_sqrt(float n);

// Faster (but less accurate) trigometry functions using approximations
// Useful because using any kinds of trigometry or floating point functions (even atof) 
// will result in const table being placed in program memory (due to existing large code size) and app will crash.
float fast_sin(float rad);
float fast_cos(float rad);
float fast_tan(float rad);
float fast_asin(float x);
float fast_acos(float x);
float fast_atan(float z);
float fast_atan2(float y, float x);

// Distance between 2 GPS coordinates, long and lat in degrees
unsigned int distanceFromPoint(float lat1, float lng1, float lat2, float lng2);

// Determine the direction from one lat/lon to another
void compassDirFromPoint(float lat1, float lng1, float lat2, float lng2, char dirStr[]);
#endif