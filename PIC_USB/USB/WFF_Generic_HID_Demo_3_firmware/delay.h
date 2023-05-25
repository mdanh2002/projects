#define CPUSPEED 20000000ULL

/*
 delay_us(x) and delay_ms(x) for C30
 */
 #define FCY       (CPUSPEED/2)  // MCU is running at FCY MIPS
 #define delay_us(x) __delay32(((x*FCY)/1000000L)) // delays x us
 #define delay_ms(x) __delay32(((x*FCY)/1000L))  // delays x ms
 
