#define MAX_CAMERA_PIXELS 16384

void camStepDelay(void);
void camClockL(void);
void camClockH(void);
void camInit(void);
void camSendRegisters1(void);
void camStartAR(void);
void camResetAR(void);
unsigned char camWaitRead(void);
void camClockAR(void);
char cam_init_and_check(void);
