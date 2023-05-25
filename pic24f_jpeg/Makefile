# MPLAB IDE generated this makefile for use with GNU make.
# Project: project.mcp
# Date: Tue Dec 31 14:04:17 2013

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

project.hex : project.cof
	$(HX) "project.cof"

project.cof : main.o uart.o ili9341.o spi_lib.o FSIO.o SD-SPI.o sd_helper.o jpeg_helper.o picojpeg.o rtcc.o
	$(CC) -mcpu=24HJ128GP202 "main.o" "uart.o" "ili9341.o" "spi_lib.o" "FSIO.o" "SD-SPI.o" "sd_helper.o" "jpeg_helper.o" "picojpeg.o" "rtcc.o" -o"project.cof" -Wl,-Tp24HJ128GP202.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1,--defsym=__ICD2RAM=1,-Map="project.map",--report-mem

main.o : rtcc.h sd_helper.h picojpeg.h jpeg_helper.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h spi_lib.h ili9341.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h uart.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h main.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

uart.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h uart.h uart.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "uart.c" -o"uart.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

ili9341.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h uart.h font.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/math.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h spi_lib.h ili9341.h ili9341.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "ili9341.c" -o"ili9341.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

spi_lib.o : ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/peripheral_30F_24H_33F/spi.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/generic/h/libpic30.h delay.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24hj128gp202.h spi_lib.h spi_lib.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "spi_lib.c" -o"spi_lib.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

FSIO.o : MDD\ File\ System/FSDefs.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/ctype.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSIO.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "MDD File System\FSIO.c" -o"FSIO.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

SD-SPI.o : MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h MDD\ File\ System/HardwareProfile.h MDD\ File\ System/GenericTypeDefs.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/SD-SPI.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "MDD File System\SD-SPI.c" -o"SD-SPI.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

sd_helper.o : uart.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h sd_helper.h sd_helper.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "sd_helper.c" -o"sd_helper.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

jpeg_helper.o : uart.h ili9341.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/assert.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/math.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSconfig.h MDD\ File\ System/SD-SPI.h MDD\ File\ System/FSDefs.h MDD\ File\ System/HardwareProfile.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/string.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdlib.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdarg.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stdio.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24HJ128GP202.h ../../../program\ files\ (x86)/microchip/mplab\ c30/support/PIC24H/h/p24Hxxxx.h MDD\ File\ System/Compiler.h MDD\ File\ System/FSconfig.h ../../../program\ files\ (x86)/microchip/mplab\ c30/include/stddef.h MDD\ File\ System/GenericTypeDefs.h MDD\ File\ System/FSIO.h jpeg_helper.h picojpeg.h jpeg_helper.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "jpeg_helper.c" -o"jpeg_helper.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

picojpeg.o : picojpeg.h picojpeg.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "picojpeg.c" -o"picojpeg.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

rtcc.o : rtcc.h rtcc.c
	$(CC) -mcpu=24HJ128GP202 -x c -c "rtcc.c" -o"rtcc.o" -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -g -Wall

clean : 
	$(RM) "main.o" "uart.o" "ili9341.o" "spi_lib.o" "FSIO.o" "SD-SPI.o" "sd_helper.o" "jpeg_helper.o" "picojpeg.o" "rtcc.o" "project.cof" "project.hex"

