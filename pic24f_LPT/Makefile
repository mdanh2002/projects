# MPLAB IDE generated this makefile for use with GNU make.
# Project: project.mcp
# Date: Sun Nov 24 11:12:09 2013

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

project.hex : project.cof
	$(HX) "project.cof"

project.cof : main.o spi_lib.o st7735.o uart.o FSIO.o SD-SPI.o vs1053.o sd_helper.o
	$(CC) -mcpu=24HJ128GP202 "main.o" "spi_lib.o" "st7735.o" "uart.o" "FSIO.o" "SD-SPI.o" "vs1053.o" "sd_helper.o" -o"project.cof" -Wl,-Tp24HJ128GP202.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1,--defsym=__ICD2RAM=1,-Map="project.map",--report-mem

main.o : sd_helper.h vs1053.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h colors.h spi_lib.h st7735.h uart.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h main.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

spi_lib.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/peripheral_30F_24H_33F/spi.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h spi_lib.h spi_lib.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "spi_lib.c" -o"spi_lib.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

st7735.o : colors.h uart.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h font.h spi_lib.h st7735.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h st7735.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "st7735.c" -o"st7735.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

uart.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h uart.h uart.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "uart.c" -o"uart.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

FSIO.o : MDD\ File\ System/FSDefs.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/ctype.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSIO.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "MDD File System\FSIO.c" -o"FSIO.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

SD-SPI.o : MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/GenericTypeDefs.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/SD-SPI.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "MDD File System\SD-SPI.c" -o"SD-SPI.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

vs1053.o : uart.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h vs1053.h spi_lib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h vs1053.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "vs1053.c" -o"vs1053.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

sd_helper.o : uart.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h sd_helper.h sd_helper.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "sd_helper.c" -o"sd_helper.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

clean : 
	$(RM) "main.o" "spi_lib.o" "st7735.o" "uart.o" "FSIO.o" "SD-SPI.o" "vs1053.o" "sd_helper.o" "project.cof" "project.hex"

