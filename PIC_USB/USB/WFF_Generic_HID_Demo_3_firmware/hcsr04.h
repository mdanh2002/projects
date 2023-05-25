#define HC_TRIG_OUT LATAbits.LATA5
#define HC_ECHO_INP PORTAbits.RA4

#define HC_TIMEOUT 0
#define HC_NOTINRANGE 1
#define HC_DETECTED 2

// the HC-SR04 distance finder module
unsigned char hc_getDistance(unsigned int *distanceInCm);
