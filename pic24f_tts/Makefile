# MPLAB IDE generated this makefile for use with GNU make.
# Project: project.mcp
# Date: Tue Jul 22 08:10:35 2014

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

project.hex : project.cof
	$(HX) "project.cof"

project.cof : main.o uart.o reciter.o sam.o syn6288.o
	$(CC) -mcpu=24FJ64GA002 "main.o" "uart.o" "reciter.o" "sam.o" "syn6288.o" -o"project.cof" -Wl,-Tp24FJ64GA002.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1,--defsym=__ICD2RAM=1,-Map="project.map",--report-mem

main.o : syn6288.h SAM/data_types.h SAM/reciter.h SAM/data_types.h SAM/sam.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h uart.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24fj64ga002.h main.c
	$(CC) -mcpu=24FJ64GA002 -x c -c "main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

uart.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24fj64ga002.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h uart.h uart.c
	$(CC) -mcpu=24FJ64GA002 -x c -c "uart.c" -o"uart.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

reciter.o : uart.h SAM/ReciterTabs.h SAM/data_types.h SAM/reciter.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h SAM/reciter.c
	$(CC) -mcpu=24FJ64GA002 -x c -c "SAM\reciter.c" -o"reciter.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

sam.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/include/ctype.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/peripheral_24F/GenericTypeDefs.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24FJ64GA002.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24Fxxxx.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/peripheral_24F/PIC24F_periph_features.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/peripheral_24F/uart.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24fj64ga002.h SAM/SamTabs.h SAM/data_types.h SAM/reciter.h SAM/data_types.h SAM/sam.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h SAM/sam.c
	$(CC) -mcpu=24FJ64GA002 -x c -c "SAM\sam.c" -o"sam.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

syn6288.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h uart.h syn6288.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24F/h/p24fj64ga002.h syn6288.c
	$(CC) -mcpu=24FJ64GA002 -x c -c "syn6288.c" -o"syn6288.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

clean : 
	$(RM) "main.o" "uart.o" "reciter.o" "sam.o" "syn6288.o" "project.cof" "project.hex"

