#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=CH376/ch376.c FATFS/ccsbcs.c FATFS/diskio.c FATFS/ff.c MP3/mad_bits.c MP3/mad_decoder.c MP3/mad_frame.c MP3/mad_huffman.c MP3/mad_layer3.c MP3/mad_memory.c MP3/mad_stream.c MP3/mad_synth.c MP3/mad_pt_alrd.S MP3/mad_pt_data.S MP3/mad_pt_im12.S MP3/mad_pt_im36.S MP3/mad_pt_inls.S MP3/mad_pt_inst.S MP3/mad_pt_msst.S MP3/mad_pt_ovlp.S MP3/mad_pt_ovlz.S MP3/mad_pt_requ.S MP3/mad_pt_synt.S main.c queue.c spi_lib.c uart.c pwm.c st7565.c i2c_lib.c si473x.c dht11.c 24lc256.c trig.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/CH376/ch376.o ${OBJECTDIR}/FATFS/ccsbcs.o ${OBJECTDIR}/FATFS/diskio.o ${OBJECTDIR}/FATFS/ff.o ${OBJECTDIR}/MP3/mad_bits.o ${OBJECTDIR}/MP3/mad_decoder.o ${OBJECTDIR}/MP3/mad_frame.o ${OBJECTDIR}/MP3/mad_huffman.o ${OBJECTDIR}/MP3/mad_layer3.o ${OBJECTDIR}/MP3/mad_memory.o ${OBJECTDIR}/MP3/mad_stream.o ${OBJECTDIR}/MP3/mad_synth.o ${OBJECTDIR}/MP3/mad_pt_alrd.o ${OBJECTDIR}/MP3/mad_pt_data.o ${OBJECTDIR}/MP3/mad_pt_im12.o ${OBJECTDIR}/MP3/mad_pt_im36.o ${OBJECTDIR}/MP3/mad_pt_inls.o ${OBJECTDIR}/MP3/mad_pt_inst.o ${OBJECTDIR}/MP3/mad_pt_msst.o ${OBJECTDIR}/MP3/mad_pt_ovlp.o ${OBJECTDIR}/MP3/mad_pt_ovlz.o ${OBJECTDIR}/MP3/mad_pt_requ.o ${OBJECTDIR}/MP3/mad_pt_synt.o ${OBJECTDIR}/main.o ${OBJECTDIR}/queue.o ${OBJECTDIR}/spi_lib.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/pwm.o ${OBJECTDIR}/st7565.o ${OBJECTDIR}/i2c_lib.o ${OBJECTDIR}/si473x.o ${OBJECTDIR}/dht11.o ${OBJECTDIR}/24lc256.o ${OBJECTDIR}/trig.o
POSSIBLE_DEPFILES=${OBJECTDIR}/CH376/ch376.o.d ${OBJECTDIR}/FATFS/ccsbcs.o.d ${OBJECTDIR}/FATFS/diskio.o.d ${OBJECTDIR}/FATFS/ff.o.d ${OBJECTDIR}/MP3/mad_bits.o.d ${OBJECTDIR}/MP3/mad_decoder.o.d ${OBJECTDIR}/MP3/mad_frame.o.d ${OBJECTDIR}/MP3/mad_huffman.o.d ${OBJECTDIR}/MP3/mad_layer3.o.d ${OBJECTDIR}/MP3/mad_memory.o.d ${OBJECTDIR}/MP3/mad_stream.o.d ${OBJECTDIR}/MP3/mad_synth.o.d ${OBJECTDIR}/MP3/mad_pt_alrd.o.d ${OBJECTDIR}/MP3/mad_pt_data.o.d ${OBJECTDIR}/MP3/mad_pt_im12.o.d ${OBJECTDIR}/MP3/mad_pt_im36.o.d ${OBJECTDIR}/MP3/mad_pt_inls.o.d ${OBJECTDIR}/MP3/mad_pt_inst.o.d ${OBJECTDIR}/MP3/mad_pt_msst.o.d ${OBJECTDIR}/MP3/mad_pt_ovlp.o.d ${OBJECTDIR}/MP3/mad_pt_ovlz.o.d ${OBJECTDIR}/MP3/mad_pt_requ.o.d ${OBJECTDIR}/MP3/mad_pt_synt.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/queue.o.d ${OBJECTDIR}/spi_lib.o.d ${OBJECTDIR}/uart.o.d ${OBJECTDIR}/pwm.o.d ${OBJECTDIR}/st7565.o.d ${OBJECTDIR}/i2c_lib.o.d ${OBJECTDIR}/si473x.o.d ${OBJECTDIR}/dht11.o.d ${OBJECTDIR}/24lc256.o.d ${OBJECTDIR}/trig.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/CH376/ch376.o ${OBJECTDIR}/FATFS/ccsbcs.o ${OBJECTDIR}/FATFS/diskio.o ${OBJECTDIR}/FATFS/ff.o ${OBJECTDIR}/MP3/mad_bits.o ${OBJECTDIR}/MP3/mad_decoder.o ${OBJECTDIR}/MP3/mad_frame.o ${OBJECTDIR}/MP3/mad_huffman.o ${OBJECTDIR}/MP3/mad_layer3.o ${OBJECTDIR}/MP3/mad_memory.o ${OBJECTDIR}/MP3/mad_stream.o ${OBJECTDIR}/MP3/mad_synth.o ${OBJECTDIR}/MP3/mad_pt_alrd.o ${OBJECTDIR}/MP3/mad_pt_data.o ${OBJECTDIR}/MP3/mad_pt_im12.o ${OBJECTDIR}/MP3/mad_pt_im36.o ${OBJECTDIR}/MP3/mad_pt_inls.o ${OBJECTDIR}/MP3/mad_pt_inst.o ${OBJECTDIR}/MP3/mad_pt_msst.o ${OBJECTDIR}/MP3/mad_pt_ovlp.o ${OBJECTDIR}/MP3/mad_pt_ovlz.o ${OBJECTDIR}/MP3/mad_pt_requ.o ${OBJECTDIR}/MP3/mad_pt_synt.o ${OBJECTDIR}/main.o ${OBJECTDIR}/queue.o ${OBJECTDIR}/spi_lib.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/pwm.o ${OBJECTDIR}/st7565.o ${OBJECTDIR}/i2c_lib.o ${OBJECTDIR}/si473x.o ${OBJECTDIR}/dht11.o ${OBJECTDIR}/24lc256.o ${OBJECTDIR}/trig.o

# Source Files
SOURCEFILES=CH376/ch376.c FATFS/ccsbcs.c FATFS/diskio.c FATFS/ff.c MP3/mad_bits.c MP3/mad_decoder.c MP3/mad_frame.c MP3/mad_huffman.c MP3/mad_layer3.c MP3/mad_memory.c MP3/mad_stream.c MP3/mad_synth.c MP3/mad_pt_alrd.S MP3/mad_pt_data.S MP3/mad_pt_im12.S MP3/mad_pt_im36.S MP3/mad_pt_inls.S MP3/mad_pt_inst.S MP3/mad_pt_msst.S MP3/mad_pt_ovlp.S MP3/mad_pt_ovlz.S MP3/mad_pt_requ.S MP3/mad_pt_synt.S main.c queue.c spi_lib.c uart.c pwm.c st7565.c i2c_lib.c si473x.c dht11.c 24lc256.c trig.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MC502
MP_LINKER_FILE_OPTION=,--script=p33EP512MC502.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/CH376/ch376.o: CH376/ch376.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/CH376" 
	@${RM} ${OBJECTDIR}/CH376/ch376.o.d 
	@${RM} ${OBJECTDIR}/CH376/ch376.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CH376/ch376.c  -o ${OBJECTDIR}/CH376/ch376.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/CH376/ch376.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/CH376/ch376.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/ccsbcs.o: FATFS/ccsbcs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/ccsbcs.o.d 
	@${RM} ${OBJECTDIR}/FATFS/ccsbcs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/ccsbcs.c  -o ${OBJECTDIR}/FATFS/ccsbcs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/ccsbcs.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/ccsbcs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/diskio.o: FATFS/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/diskio.o.d 
	@${RM} ${OBJECTDIR}/FATFS/diskio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/diskio.c  -o ${OBJECTDIR}/FATFS/diskio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/diskio.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/diskio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/ff.o: FATFS/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/ff.o.d 
	@${RM} ${OBJECTDIR}/FATFS/ff.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/ff.c  -o ${OBJECTDIR}/FATFS/ff.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/ff.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/ff.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_bits.o: MP3/mad_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_bits.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_bits.c  -o ${OBJECTDIR}/MP3/mad_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_bits.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_decoder.o: MP3/mad_decoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_decoder.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_decoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_decoder.c  -o ${OBJECTDIR}/MP3/mad_decoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_decoder.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_decoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_frame.o: MP3/mad_frame.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_frame.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_frame.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_frame.c  -o ${OBJECTDIR}/MP3/mad_frame.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_frame.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_frame.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_huffman.o: MP3/mad_huffman.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_huffman.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_huffman.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_huffman.c  -o ${OBJECTDIR}/MP3/mad_huffman.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_huffman.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_huffman.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_layer3.o: MP3/mad_layer3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_layer3.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_layer3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_layer3.c  -o ${OBJECTDIR}/MP3/mad_layer3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_layer3.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_layer3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_memory.o: MP3/mad_memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_memory.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_memory.c  -o ${OBJECTDIR}/MP3/mad_memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_memory.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_stream.o: MP3/mad_stream.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_stream.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_stream.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_stream.c  -o ${OBJECTDIR}/MP3/mad_stream.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_stream.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_stream.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_synth.o: MP3/mad_synth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_synth.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_synth.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_synth.c  -o ${OBJECTDIR}/MP3/mad_synth.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_synth.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_synth.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/queue.o: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.o.d 
	@${RM} ${OBJECTDIR}/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  queue.c  -o ${OBJECTDIR}/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/queue.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/spi_lib.o: spi_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi_lib.o.d 
	@${RM} ${OBJECTDIR}/spi_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi_lib.c  -o ${OBJECTDIR}/spi_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi_lib.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/spi_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart.o: uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/pwm.o: pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/pwm.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/pwm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/st7565.o: st7565.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/st7565.o.d 
	@${RM} ${OBJECTDIR}/st7565.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  st7565.c  -o ${OBJECTDIR}/st7565.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/st7565.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/st7565.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/i2c_lib.o: i2c_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c_lib.o.d 
	@${RM} ${OBJECTDIR}/i2c_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c_lib.c  -o ${OBJECTDIR}/i2c_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/i2c_lib.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/i2c_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/si473x.o: si473x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/si473x.o.d 
	@${RM} ${OBJECTDIR}/si473x.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  si473x.c  -o ${OBJECTDIR}/si473x.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/si473x.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/si473x.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/dht11.o: dht11.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/dht11.o.d 
	@${RM} ${OBJECTDIR}/dht11.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  dht11.c  -o ${OBJECTDIR}/dht11.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/dht11.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/dht11.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/24lc256.o: 24lc256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/24lc256.o.d 
	@${RM} ${OBJECTDIR}/24lc256.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  24lc256.c  -o ${OBJECTDIR}/24lc256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/24lc256.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/24lc256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/trig.o: trig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trig.o.d 
	@${RM} ${OBJECTDIR}/trig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trig.c  -o ${OBJECTDIR}/trig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/trig.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/trig.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/CH376/ch376.o: CH376/ch376.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/CH376" 
	@${RM} ${OBJECTDIR}/CH376/ch376.o.d 
	@${RM} ${OBJECTDIR}/CH376/ch376.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CH376/ch376.c  -o ${OBJECTDIR}/CH376/ch376.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/CH376/ch376.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/CH376/ch376.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/ccsbcs.o: FATFS/ccsbcs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/ccsbcs.o.d 
	@${RM} ${OBJECTDIR}/FATFS/ccsbcs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/ccsbcs.c  -o ${OBJECTDIR}/FATFS/ccsbcs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/ccsbcs.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/ccsbcs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/diskio.o: FATFS/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/diskio.o.d 
	@${RM} ${OBJECTDIR}/FATFS/diskio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/diskio.c  -o ${OBJECTDIR}/FATFS/diskio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/diskio.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/diskio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FATFS/ff.o: FATFS/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FATFS" 
	@${RM} ${OBJECTDIR}/FATFS/ff.o.d 
	@${RM} ${OBJECTDIR}/FATFS/ff.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FATFS/ff.c  -o ${OBJECTDIR}/FATFS/ff.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FATFS/ff.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/FATFS/ff.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_bits.o: MP3/mad_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_bits.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_bits.c  -o ${OBJECTDIR}/MP3/mad_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_bits.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_decoder.o: MP3/mad_decoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_decoder.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_decoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_decoder.c  -o ${OBJECTDIR}/MP3/mad_decoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_decoder.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_decoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_frame.o: MP3/mad_frame.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_frame.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_frame.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_frame.c  -o ${OBJECTDIR}/MP3/mad_frame.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_frame.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_frame.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_huffman.o: MP3/mad_huffman.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_huffman.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_huffman.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_huffman.c  -o ${OBJECTDIR}/MP3/mad_huffman.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_huffman.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_huffman.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_layer3.o: MP3/mad_layer3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_layer3.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_layer3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_layer3.c  -o ${OBJECTDIR}/MP3/mad_layer3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_layer3.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_layer3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_memory.o: MP3/mad_memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_memory.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_memory.c  -o ${OBJECTDIR}/MP3/mad_memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_memory.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_stream.o: MP3/mad_stream.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_stream.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_stream.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_stream.c  -o ${OBJECTDIR}/MP3/mad_stream.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_stream.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_stream.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/MP3/mad_synth.o: MP3/mad_synth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_synth.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_synth.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MP3/mad_synth.c  -o ${OBJECTDIR}/MP3/mad_synth.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_synth.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_synth.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/queue.o: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.o.d 
	@${RM} ${OBJECTDIR}/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  queue.c  -o ${OBJECTDIR}/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/queue.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/spi_lib.o: spi_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi_lib.o.d 
	@${RM} ${OBJECTDIR}/spi_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi_lib.c  -o ${OBJECTDIR}/spi_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi_lib.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/spi_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart.o: uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/pwm.o: pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/pwm.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/pwm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/st7565.o: st7565.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/st7565.o.d 
	@${RM} ${OBJECTDIR}/st7565.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  st7565.c  -o ${OBJECTDIR}/st7565.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/st7565.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/st7565.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/i2c_lib.o: i2c_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c_lib.o.d 
	@${RM} ${OBJECTDIR}/i2c_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c_lib.c  -o ${OBJECTDIR}/i2c_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/i2c_lib.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/i2c_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/si473x.o: si473x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/si473x.o.d 
	@${RM} ${OBJECTDIR}/si473x.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  si473x.c  -o ${OBJECTDIR}/si473x.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/si473x.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/si473x.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/dht11.o: dht11.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/dht11.o.d 
	@${RM} ${OBJECTDIR}/dht11.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  dht11.c  -o ${OBJECTDIR}/dht11.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/dht11.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/dht11.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/24lc256.o: 24lc256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/24lc256.o.d 
	@${RM} ${OBJECTDIR}/24lc256.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  24lc256.c  -o ${OBJECTDIR}/24lc256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/24lc256.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/24lc256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/trig.o: trig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trig.o.d 
	@${RM} ${OBJECTDIR}/trig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trig.c  -o ${OBJECTDIR}/trig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/trig.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-code -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/trig.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/MP3/mad_pt_alrd.o: MP3/mad_pt_alrd.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_alrd.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_alrd.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_alrd.S  -o ${OBJECTDIR}/MP3/mad_pt_alrd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_alrd.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_alrd.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_alrd.o.d" "${OBJECTDIR}/MP3/mad_pt_alrd.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_data.o: MP3/mad_pt_data.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_data.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_data.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_data.S  -o ${OBJECTDIR}/MP3/mad_pt_data.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_data.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_data.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_data.o.d" "${OBJECTDIR}/MP3/mad_pt_data.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_im12.o: MP3/mad_pt_im12.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im12.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im12.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_im12.S  -o ${OBJECTDIR}/MP3/mad_pt_im12.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_im12.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_im12.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_im12.o.d" "${OBJECTDIR}/MP3/mad_pt_im12.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_im36.o: MP3/mad_pt_im36.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im36.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im36.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_im36.S  -o ${OBJECTDIR}/MP3/mad_pt_im36.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_im36.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_im36.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_im36.o.d" "${OBJECTDIR}/MP3/mad_pt_im36.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_inls.o: MP3/mad_pt_inls.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inls.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inls.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_inls.S  -o ${OBJECTDIR}/MP3/mad_pt_inls.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_inls.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_inls.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_inls.o.d" "${OBJECTDIR}/MP3/mad_pt_inls.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_inst.o: MP3/mad_pt_inst.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inst.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inst.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_inst.S  -o ${OBJECTDIR}/MP3/mad_pt_inst.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_inst.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_inst.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_inst.o.d" "${OBJECTDIR}/MP3/mad_pt_inst.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_msst.o: MP3/mad_pt_msst.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_msst.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_msst.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_msst.S  -o ${OBJECTDIR}/MP3/mad_pt_msst.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_msst.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_msst.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_msst.o.d" "${OBJECTDIR}/MP3/mad_pt_msst.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_ovlp.o: MP3/mad_pt_ovlp.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlp.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlp.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_ovlp.S  -o ${OBJECTDIR}/MP3/mad_pt_ovlp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_ovlp.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_ovlp.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_ovlp.o.d" "${OBJECTDIR}/MP3/mad_pt_ovlp.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_ovlz.o: MP3/mad_pt_ovlz.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlz.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlz.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_ovlz.S  -o ${OBJECTDIR}/MP3/mad_pt_ovlz.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_ovlz.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_ovlz.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_ovlz.o.d" "${OBJECTDIR}/MP3/mad_pt_ovlz.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_requ.o: MP3/mad_pt_requ.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_requ.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_requ.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_requ.S  -o ${OBJECTDIR}/MP3/mad_pt_requ.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_requ.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_requ.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_requ.o.d" "${OBJECTDIR}/MP3/mad_pt_requ.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_synt.o: MP3/mad_pt_synt.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_synt.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_synt.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_synt.S  -o ${OBJECTDIR}/MP3/mad_pt_synt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_synt.o.d"  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_synt.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_synt.o.d" "${OBJECTDIR}/MP3/mad_pt_synt.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/MP3/mad_pt_alrd.o: MP3/mad_pt_alrd.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_alrd.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_alrd.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_alrd.S  -o ${OBJECTDIR}/MP3/mad_pt_alrd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_alrd.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_alrd.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_alrd.o.d" "${OBJECTDIR}/MP3/mad_pt_alrd.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_data.o: MP3/mad_pt_data.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_data.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_data.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_data.S  -o ${OBJECTDIR}/MP3/mad_pt_data.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_data.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_data.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_data.o.d" "${OBJECTDIR}/MP3/mad_pt_data.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_im12.o: MP3/mad_pt_im12.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im12.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im12.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_im12.S  -o ${OBJECTDIR}/MP3/mad_pt_im12.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_im12.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_im12.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_im12.o.d" "${OBJECTDIR}/MP3/mad_pt_im12.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_im36.o: MP3/mad_pt_im36.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im36.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_im36.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_im36.S  -o ${OBJECTDIR}/MP3/mad_pt_im36.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_im36.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_im36.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_im36.o.d" "${OBJECTDIR}/MP3/mad_pt_im36.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_inls.o: MP3/mad_pt_inls.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inls.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inls.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_inls.S  -o ${OBJECTDIR}/MP3/mad_pt_inls.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_inls.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_inls.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_inls.o.d" "${OBJECTDIR}/MP3/mad_pt_inls.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_inst.o: MP3/mad_pt_inst.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inst.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_inst.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_inst.S  -o ${OBJECTDIR}/MP3/mad_pt_inst.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_inst.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_inst.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_inst.o.d" "${OBJECTDIR}/MP3/mad_pt_inst.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_msst.o: MP3/mad_pt_msst.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_msst.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_msst.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_msst.S  -o ${OBJECTDIR}/MP3/mad_pt_msst.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_msst.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_msst.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_msst.o.d" "${OBJECTDIR}/MP3/mad_pt_msst.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_ovlp.o: MP3/mad_pt_ovlp.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlp.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlp.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_ovlp.S  -o ${OBJECTDIR}/MP3/mad_pt_ovlp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_ovlp.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_ovlp.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_ovlp.o.d" "${OBJECTDIR}/MP3/mad_pt_ovlp.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_ovlz.o: MP3/mad_pt_ovlz.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlz.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_ovlz.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_ovlz.S  -o ${OBJECTDIR}/MP3/mad_pt_ovlz.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_ovlz.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_ovlz.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_ovlz.o.d" "${OBJECTDIR}/MP3/mad_pt_ovlz.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_requ.o: MP3/mad_pt_requ.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_requ.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_requ.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_requ.S  -o ${OBJECTDIR}/MP3/mad_pt_requ.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_requ.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_requ.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_requ.o.d" "${OBJECTDIR}/MP3/mad_pt_requ.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/MP3/mad_pt_synt.o: MP3/mad_pt_synt.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/MP3" 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_synt.o.d 
	@${RM} ${OBJECTDIR}/MP3/mad_pt_synt.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  MP3/mad_pt_synt.S  -o ${OBJECTDIR}/MP3/mad_pt_synt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MP3/mad_pt_synt.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/MP3/mad_pt_synt.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	@${FIXDEPS} "${OBJECTDIR}/MP3/mad_pt_synt.o.d" "${OBJECTDIR}/MP3/mad_pt_synt.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/dspic33ep_dokump3_final_v2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
