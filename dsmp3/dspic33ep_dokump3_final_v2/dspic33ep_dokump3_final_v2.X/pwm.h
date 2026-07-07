// different method to play audio. Exactly one directive should be defined

// define the audio device mode
// need to put a constant for each define, otherwise #elif when used with #ifdef will fail with
// "elif with no expression" error message

// if define, use DSPIC33EP native PWM module, else use the Output Compare as PWM
#define USE_NATIVE_PWM

// if defined, use timer PWM instead of native PWM
// #define USE_TIMER_PWM

// #define PWM_MODE8
// #define PWM_MODE10
// #define PWM_MODE11
#define PWM_MODE12
// #define PWM_MODE13

// PWM_BITS = N
// PWM_BITS_REM8 = N - 8
// PWM_BITS_REM16 = 16 - N
// PWM_VALUE = 2^N

#ifdef PWM_MODE8
// 8-bit PWM (~700 KHz)
#define PWM_BITS_REM8       0        
#define PWM_BITS_REM16      8        
#define PWM_VALUE           256       
#endif

#ifdef PWM_MODE10
// 10-bit PWM (~175kHz)
#define PWM_BITS_REM8       2            
#define PWM_BITS_REM16      6
#define PWM_VALUE           1024         
#endif 

#ifdef PWM_MODE11
// 11-bit PWM (~88kHz)
#define PWM_BITS_REM8       3            
#define PWM_BITS_REM16      5
#define PWM_VALUE           2048         
#endif 

#ifdef PWM_MODE12
// 12-bit PWM (~45 kHz)
#define PWM_BITS_REM8       4          
#define PWM_BITS_REM16      4
#define PWM_VALUE           4096       
#endif 

#ifdef PWM_MODE13
// 13-bit PWM (~22kHz)
#define PWM_BITS_REM8       5          
#define PWM_BITS_REM16      3
#define PWM_VALUE           8192       
#endif 

void enablePWM(unsigned char isEnabled);
unsigned char isPWMEnabled();

#ifdef USE_NATIVE_PWM
void initPWM_native(int value);
#endif

#ifdef USE_TIMER_PWM
void initPWM_OC(int value);
#endif
