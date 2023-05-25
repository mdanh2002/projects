#include <p24fj64ga002.h>
#include "syn6288.h"
#include "uart.h"
#include "delay.h"

uint8_t BackGroundMusic = 0x00;
uint8_t TextCodec = 0x00;
uint8_t FrameHeader[5] = { 0xFD, 0x00, 0x00, 0x00, 0x00 };

uint8_t CheckXOR(const uint8_t * Object, uint8_t Length) {
	uint8_t Result = 0, i;

	Result ^= FrameHeader[0];
	Result ^= FrameHeader[1];
	Result ^= FrameHeader[2];
	Result ^= FrameHeader[3];
	Result ^= FrameHeader[4];
	for (i = 0; i < Length; i++) {
		Result ^= Object[i];
	}
	return Result;
}

void SYN_SetBackGroundMusic(uint8_t Code) {
	BackGroundMusic = Code;
}

void SYN_SetTextCodec(uint8_t Code) {
	TextCodec = Code;
}

void SYN_SentHeader(uint16_t DataAreaLength) {
	FrameHeader[1] = (DataAreaLength & 0xFF00) >> 8;
	FrameHeader[2] = (DataAreaLength & 0xFF);
	SendUART2(FrameHeader[0]);
	SendUART2(FrameHeader[1]);
	SendUART2(FrameHeader[2]);

	// Added by MD - mixing from original code!
	SendUART2(FrameHeader[3]);
	SendUART2(FrameHeader[4]);
}

void SYN_SetBaudRate(uint8_t BaudRate) {
	// first tell the SYN6288 to change the expected baudrate
	FrameHeader[3] = 0x31;
	FrameHeader[4] = BaudRate;
	SYN_SentHeader(3);
	SendUART2(FrameHeader[3]);
	SendUART2(FrameHeader[4]);

	SendUART2(CheckXOR(FrameHeader, 0));

	// only after the above is done, should we proceed to change out actual UART baud rate.
}

void SYN_Say(const uint8_t * Text) {
	uint16_t i;
	uint16_t Length;

	for(Length = 0;;Length++){
		if(Text[Length] == '\0')
		break;
	}

	FrameHeader[3] = 0x01;
	FrameHeader[4] = BackGroundMusic | TextCodec;
	SYN_SentHeader(Length + 3);
	for(i = 0;i < Length;i++){
		SendUART2(Text[i]);
	}
	SendUART2(CheckXOR(Text,Length));

	// wait for processing, otherwise the SYN_BUSY won't turn on immediately and 
	// the delay below is useless.
	delay_ms(20);

	// wait until speaking is completed before exiting the function.
	while (SYN_BUSY);

	// wait a bit more before exiting, otherwise the chip is still busy
	// and may ignore the next command set
	delay_ms(20);
}

void SYN_SpeakingPaused(void){
	FrameHeader[3] = 0x03;
	FrameHeader[4] = 0x00;
	SYN_SentHeader(1);
	SendUART2(FrameHeader[3]);
	SendUART2(CheckXOR(FrameHeader,0));
}

void SYN_SpeakingResume(void){

	FrameHeader[3] = 0x04;
	FrameHeader[4] = 0x00;
	SYN_SentHeader(1);
	SendUART2(FrameHeader[3]);
	SendUART2(CheckXOR(FrameHeader,0));
}

void SYN_SpeakingStop(void){

	FrameHeader[3] = 0x02;
	FrameHeader[4] = 0x00;
	SYN_SentHeader(1);
	SendUART2(FrameHeader[3]);
	SendUART2(CheckXOR(FrameHeader,0));
}
