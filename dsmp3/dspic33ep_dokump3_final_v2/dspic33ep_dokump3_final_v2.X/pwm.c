#include <xc.h>
#include "pwm.h"

// The PWM should only be enabled when audio needs to be played
// Otherwise because the PWM is constantly running, the amplifier will be working all the time 
// which will cause high power consumption even when audio playback is paused.
#ifdef USE_NATIVE_PWM
// Init native PWM module
void initPWM_native(int value)
{
    /*
    For the dsPIC33EP512MC502, from Microchip DS70645 note:

    PTPER, PHASEx, SPHASEx = FOSC / (FPWM * PWM Input Clock Prescaler)

    FPWM = Desired PWM frequency
    FOSC = Oscillator output (120 MHz for 60 MIPS)
    PWM Input Clock Prescaler = Value defined in the PCLKDIV<2:0> bits (PTCON2<2:0)
    
    Can run 8-bit PWM @ 140MHz (max).
    
    To run higher PWM speeds, we need a dsPIC with auxiliary PLL. Only dsPICE with USB supports auxiliary PLL.
    */ 
    PTCONbits.PTEN = 0;             // Disable PWM module 
    
    PTCON2bits.PCLKDIV = 0b000;     // PWM divide by 1    
    
    PWMCON1bits.ITB = 0;            // PTPER register provides timing for this PWM generator        
    PWMCON1bits.MDCS = 0;           // PDCx register provides duty cycle information for this PWM generator
    PWMCON1bits.MTBS = 0;           // PWM Generator uses the primary master time 
    
    PWMCON2bits.ITB = 0;            // PTPER register provides timing for this PWM generator        
    PWMCON2bits.MDCS = 0;           // PDCx register provides duty cycle information for this PWM generator
    PWMCON2bits.MTBS = 0;           // PWM Generator uses the primary master time 
    
    IOCON1bits.PENH = 1;            // PWM1H enabled
    IOCON1bits.PENL = 0;            // PWM1L disabled        
    IOCON1bits.PMOD = 0;            // PWM1 I/O pin pair is in the Complementary Output mode
    
    IOCON2bits.PENH = 1;            // PWM2H enabled
    IOCON2bits.PENL = 0;            // PWM2L disabled        
    IOCON2bits.PMOD = 0;            // PWM2 I/O pin pair is in the Complementary Output mode
    
    IOCON3bits.PENH = 0;            // PWM3H disabled
    IOCON3bits.PENL = 0;            // PWM3L disabled
    
    DTR1    = 0;                     // Deadtime setting PWM1
    ALTDTR1 = 0;                     // Alternate deadtime setting PWM1
    PHASE1  = 0;                     // No phase shift PWM1
    
    DTR2    = 0;                     // Deadtime setting PWM2
    ALTDTR2 = 0;                     // Alternate deadtime setting PWM2
    PHASE2  = 0;                     // No phase shift PWM2
    
    PTPER = value;                  //  Master Time Base Period Register
    
    PDC1 = value / 2;               // Default 50% duty cycle PWM1
    PDC2 = value / 2;               // Default 50% duty cycle PWM2
}

void enablePWM(unsigned char isEnabled)
{        
    PTCONbits.PTEN = isEnabled;             // Enable PWM module
}

unsigned char isPWMEnabled()
{
    return (PTCONbits.PTEN);
}
#endif

#ifdef USE_TIMER_PWM
// Init PWM using Output Compare Module
void initPWM_OC(int value)
{
    // Disable Timer 2
	T2CONbits.TON = 0; 
    
    // Clear register
    OC1CON1 = 0;
    OC1CON2 = 0;

    // Output Compare Config
	OC1CON1bits.OCM = 0b000;        // Disable Output Compare Module    
	OC1R  = value / 2;              // PWM Initial Duty Cycle, e.g. duty cycle until the first period is complete
	OC1CON1bits.OCTSEL = 0;         // Select Timer 2 as output compare time base
    OC1CON1bits.OCM = 0b110;        // Edge-Aligned PWM mode: Output set high when OCxTMR = 0 and set low when OCxTMR = OCxR    
    OC1CON2bits.SYNCSEL = 0b01100;  // Timer2 synchronizes or triggers OCx 
	 
	// Timer 2 Config
	T2CONbits.TON = 0;              // Disable Timer
	T2CONbits.TCS = 0;              // Select internal instruction cycle clock
	T2CONbits.TGATE = 0;            // Disable Gated Timer mode
	T2CONbits.TCKPS = 0;            // 1:1 Prescaler
    T2CONbits.T32 = 0;              // 16-bit timer    
	TMR2 = 0x00;                    // Clear timer register
	PR2 = value;                    // Load the period value
    OC1R = 0;                       // Initial Value
}

void enablePWM(unsigned char isEnabled)
{        
    T2CONbits.TON = isEnabled;             // Enable Timer 2 for PWM module
}

unsigned char isPWMEnabled()
{
    return (T2CONbits.TON);
}
#endif