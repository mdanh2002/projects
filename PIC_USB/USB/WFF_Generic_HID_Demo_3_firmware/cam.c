/**
 * @file       cam.c
 * @brief      gameboy camera interface for AT90S4433
 *             see the ccd device doc to understand the api: M64282FP.pdf
 * 
 * @author     Laurent Saint-Marcel (lstmarcel@yahoo.fr) http://sophiateam.undrgnd.free.fr/microcontroller/camera/
 * @date       2005/07/05
 */

#include "delay.h"
#include "cam.h"
#include "camCom.h"
#include <p18F4550.h>

// default value for registers
unsigned char camReg1[8]={ 0x80, 0x03, 0x00, 0x30, 0x01, 0x00, 0x01, 0x23 };
unsigned char camClockSpeed = 0x0A;

//############################################################################
//####   PRIVATE FUNCTION : Communication with the AR chip                ####
//############################################################################
// Delay used between each signal sent to the AR (four per xck cycle).
void camStepDelay(){
  	unsigned char u=camClockSpeed;
	int i;
	while(u--) 
	{
		Nop();
	}
}

// Set the clock signal Low
void camClockL()
{
	CAM_CLOCK = 0;
}

// Set the clock signal High
void camClockH()
{
	CAM_CLOCK = 1;
}

// Sends a 'reset' pulse to the AR chip.
void camResetAR()
{
 camClockL(); // clock low
 CAM_RESET = 0; 
 camStepDelay();
 camStepDelay();

 camClockH(); // clock low	
 camStepDelay();
 camStepDelay();
 CAM_RESET = 1;
}

// Sets one of the 8 8-bit registers in the AR chip.
void camSetRegAR(unsigned char regaddr, unsigned char regval)
{
 unsigned char bitmask;

 // Write 3-bit address.
 for(bitmask = 4; bitmask >= 1; bitmask >>= 1){
  camClockL();
  camStepDelay();
  CAM_LOAD = 0;
  if(regaddr & bitmask) 
	CAM_SIN = 1;
  else 
	CAM_SIN = 0;

  camStepDelay();

  camClockH();
  camStepDelay();
  CAM_SIN = 0;
  camStepDelay();
 }

 // Write 7 most significant bits of 8-bit data.
 for(bitmask = 128; bitmask >= 2; bitmask>>=1){
  camClockL();
  camStepDelay();
  if(regval & bitmask) 
	CAM_SIN = 1;	
  else CAM_SIN = 0;

  camStepDelay();

  camClockH();
  camStepDelay();
  CAM_SIN = 0;
  camStepDelay();
 }

 // Write LSB of data along with 'load' signal and the rest of xck.
 camClockL();
 camStepDelay();
 if(regval & 1) 
	CAM_SIN = 1;
 else 
	CAM_SIN = 0;
 camStepDelay();

 camClockH();
 camStepDelay();
 CAM_LOAD = 1;
 camStepDelay();
}

// Sends a 'start' pulse to the AR chip.
void camStartAR()
{
 camClockL();
 camStepDelay();
 CAM_SIN = 0;
 CAM_LOAD = 0;
 CAM_START = 1;
 camStepDelay();

 camClockH();
 camStepDelay();
 
 CAM_START = 0;

 camStepDelay();
}

// Sends a blank 'xck' pulse to the AR chip.
void camClockAR()
{
 camClockL();
 camStepDelay();
 camStepDelay();

 camClockH();
 camStepDelay();
 camStepDelay();
}

// Wait read signal from the AR chip
unsigned char camWaitRead()
{
   camClockL();
   camStepDelay();
   camStepDelay();

   camClockH();
   camStepDelay();
   if(CAM_READ == 1) {
     camStepDelay();
	 return 1;
   } else {
     camStepDelay();
	 return 0;
   }
}


//############################################################################
//#### PUBLIC FUNCTIONS                                                   ####
//############################################################################
// Initialise the IO ports for the camera
void camInit()
{
	CAM_CLOCK = 0;
	CAM_RESET = 0;
	CAM_LOAD = 0;
	CAM_START = 0;
	CAM_SIN = 0;

}

// Send the 8 register values to the AR chip
void camSendRegisters1()
{
	unsigned char reg;
	camResetAR();
	for(reg=0; reg<8; ++reg) {
		camSetRegAR(reg, camReg1[reg]);
	}
}

char cam_init_and_check(void)
{
	int pixelReceived;
	char isOK;
	int waitCount = 0;

	// initialized the gameboy camera	
	camInit();
	camSendRegisters1();
	camStartAR();

#define MAX_WAIT_COUNT 1000

	while(!camWaitRead() && waitCount < MAX_WAIT_COUNT)
	{
		waitCount++;
	}

	//timeout waiting
	if (waitCount >= MAX_WAIT_COUNT) 
	{
		return 0;
	}

	pixelReceived = 0;
	while (CAM_READ == 1)
	{
		camClockAR();
		pixelReceived++;

		// unexpected, camera return too many pixels
		if (pixelReceived > MAX_CAMERA_PIXELS) break;
	}

	return (pixelReceived == MAX_CAMERA_PIXELS);
}

