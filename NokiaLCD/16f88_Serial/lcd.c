/*
Interface with Nokia 3510i LCD purchased from ebay (http://cgi.ebay.com.sg/ws/eBayISAPI.dll?ViewItem&item=200452329046&ssPageName=STRK:MEWNX:IT)

LCD pinout:

1. LED
2. SCLK
3. SDATA(MOSI)
4. NC (stands for Not Connected?)
5. REST
6. CS
7. GND
8. VCC

Connections:

RB3 --- 10k resistor   --- SCLK
RB4 --- 10k resistor   --- SDATA
RB6 --- 10k resistor   --- REST
RB7 --- 10k resistor   --- CS
5V  --- 3.3V Regulator --- LED, VCC
VCC --- 1nF capacitor  --- GND (prevent flickering)

Nokia 3510i LCD is desinged to work with 3.3V, but due to an internal voltage clamp 5V can be used for SCLK, SDATA, REST and CS
as long as 3.3V is applied to VCC and LED properly. A voltage divider can't be used to convert between 5V and 3.3V due to the LCD
internal resistance and current consumption.

With the above connections we can only write to the LCD but can't read back the LCD response (3.3v is not high enough to register 
as logic '1' in the PIC). Reading is required when we need to get the LCD version number, etc. In particular, function LCD_Initialize()
calls function readdata() to get LCD type (defined as 'ver', can be 'A', 'B', 'C' 'D'). Without reading of data, LCD type has to be hard-coded.
In this case, LCD type is 'B'. If the type is incorrect, color may not be displayed properly.
*/

#include "lcd.h"
#include "pic.h"

// delay for approximately the given amount of msec
// correct on PIC16F88 @ 20MHz
void delayms(unsigned  int ii)
{
	
	unsigned int i,x;
	for (x=0;x<ii;x++)
	{
	for (i=0;i<230;i++);
	}
}

// Send a command/data code to the LCD
// cd = 0: command
// cd = 1: data
void send(unsigned char value, unsigned char cd) 
 { 
  unsigned char i,dt; 
	cs=0;
	sclk=0;
   if(cd == 0) 
      sdata=0;
   else 
     sdata=1;
     sclk=1;    

	dt=value;
	for(i=0;i<8;i++)
	{ 
		sclk=0; 	
		if(dt&0x80)
			sdata=1;
		else
			sdata=0;
		dt=dt<<1;		
		sclk=1; 
	}	
	cs=1;
}

// Get the LCD type
/*
void readdata()  
{
	TRISB = 0b00000010;
		
	unsigned char i,j;
	unsigned char ch[6];
	send(0x09, 0);      
    cs=0;
	sclk=0;
		
	for (i=0;i<6;i++)
	{
		ch[i]=0;
		for (j=0;j<8;j++)
		{			
			sclk=1;
			if (sdata==0)
			{
				ch[i]=ch[i]|(1<<(7-j));
			}
			sclk=0;	
		}
	 }

	TRISB = 0b00000000;
	
	 switch(ch[0])
	 {
	 	case 255:
			Ver='A';
			break;
		case 127:
			Ver='B';
			break;
		case 0:
			if(ch[1]+ch[2]+ch[3]==0)
			{
				Ver='D';
			}else
			{
				Ver='C';
			}
			break;
	 }
	cs=1;
}
*/

// Initialize the LCD
void LCD_Initialize()  
{
	unsigned char i;
 	rest=1;
	cs=1;
	sdata=0;
	sclk=1;
	rest=0;
	delayms(50);
	rest=1;
	delayms(50);
	
   send(0x00, 0);      // NOP
   delayms(5); 

   send(0x01, 0);      // LCD Software Reset 
   delayms(5); 
  
   send(0xC6, 0);      // Initial Escape 
      
   send(0xB9, 0);      // Refresh set 
   send(0x00, 1); 

	//TODO: Unable to read LCD response due to voltage difference. Must hard code LCD type
   //readdata();           
   Ver='B';
       
   send(0xB5, 0);      // Gamma Curve Set 
   send(0x01, 1); 

   send(0xbd, 0);      //common driver output select
   if(Ver=='D')
   {
   		send(0x04, 1); 
   }
   else
   {
   		send(0x00, 1); 
   }

   send(0xBE, 0);      // Power Control 
   send(0x03, 1); 

    
   send(0x11, 0);      // Sleep out 

   send(0xBA, 0);      // Power Control 
   send(0x7F, 1); 
   send(0x03, 1);
   
   send(0x36, 0);      
   if (Ver=='A')
   {
   		 send(0x00|0x08,1); 	   //RGB
    }
		else
	{
	  	 send(0x00,1); 	   //RGB
	}
	  
   send(0xB7, 0);      // Temperature gradient set 
   for(i=0; i<14; i++) 
   { 
      send(0, 1); 
   } 


   send(0x29, 0);      //display ON	 
   	   
   send(0x03, 0);      // Booster Voltage ON 
      
   delayms(20);         // Booster Voltage stabilisieren lassen      
    
   send(0x20, 0);     //display inversion OFF

   send(0x25, 0);      // Write contrast   
   switch(Ver)							   
   {
   		case 'A':		
			send(70, 1);  
			break;
   		case 'B':	  
			send(67, 1);  
			break;
		case 'C':
			//send(74, 1); 
		   	send(66, 1); 
		//	send(64, 1);   
			break;
		case 'D':		  
			send(49, 1); 
			break;
			
   } 
      	
}

//************************
//Set the color mode to work with the LCD
//color=1: 4096 colors
//color=0: 256 colors
//************************
void LCD_ColorSet(unsigned char Color)
{

	if (Color==1) {
	send(0x3a, 0);		//interface pixel format
	send(0x03, 1);       //0x03 4096 colors
	 
	}
	else
	{
	send(0x3a, 0);		//interface pixel format
	send(0x02, 1);       //0x02 256 colors
	 
	send(0x2d, 0);		
	if (Ver=='B'||Ver=='C')
	 {
	  	//red
		send(~0x00, 1);
		send(~0x02, 1);
		send(~0x03, 1);
		send(~0x04, 1);
		send(~0x05, 1);
		send(~0x06, 1);
		send(~0x08, 1);
		send(~0x0f, 1);
		
		//green
		send(~0x00, 1);
		send(~0x02, 1);
		send(~0x03, 1);
		send(~0x04, 1);
		send(~0x05, 1);
		send(~0x06, 1);
		send(~0x08, 1);
		send(~0x0f, 1);
		//blue
		send(~0x00, 1);
		send(~0x03, 1);
		send(~0x06, 1);
		send(~0x0f, 1);
	 }else
	{
	  	 //red
		send(0x00, 1);
		send(0x02, 1);
		send(0x03, 1);
		send(0x04, 1);
		send(0x05, 1);
		send(0x06, 1);
		send(0x08, 1);
		send(0x0f, 1);
		
		//green
		send(0x00, 1);
		send(0x02, 1);
		send(0x03, 1);
		send(0x04, 1);
		send(0x05, 1);
		send(0x06, 1);
		send(0x08, 1);
		send(0x0f, 1);
		//blue
		send(0x00, 1);
		send(0x03, 1);
		send(0x06, 1);
		send(0x0f, 1);
	}

    
   }
}

// Call this to get access to a particular region on the LCD before writing the pixel value
// top left (x1, y1)
// bottom right (x2, y2)
void addset(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)  
{
    send(0x2a,0 );//column address set
    send(x1,1 );
	send(x2,1 );
 	send(0x2B,0 );//page address set
    send(y1,1 );
	send(y2,1 );
 	send(0x2C,0 );//memory write
}

//************************
//Fill the entire LCD screen with the given color value
//color=1: 4096 colors
//color=0: 256 colors
//************************
void LCD_Clear(unsigned int value,unsigned char Color)
{
	unsigned char x, y;
	addset(0,0,97,66);
	if (Color==1)	
	{
		for(y = 0; y < 67; y ++)
		{
			for(x = 0; x < 49;x ++)
			{	send(value>>4,1);
				send(((value&0x0f)<<4)|(value>>8),1);
				send(value,1);
			}	
		 }
	}
	else
	{
		for(y = 0; y < 67; y ++)
		{
			for(x = 0; x < 98; x ++)
				{
				send(value,1);
				}
		}
	} 	
}
