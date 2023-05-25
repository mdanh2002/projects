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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED="MDD File System/FSIO.c" "MDD File System/SD-SPI.c" main.c uart.c spi_lib.c st7735.c sd_helper.c ld3320.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED="${OBJECTDIR}/MDD File System/FSIO.o" "${OBJECTDIR}/MDD File System/SD-SPI.o" ${OBJECTDIR}/main.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/spi_lib.o ${OBJECTDIR}/st7735.o ${OBJECTDIR}/sd_helper.o ${OBJECTDIR}/ld3320.o
POSSIBLE_DEPFILES="${OBJECTDIR}/MDD File System/FSIO.o.d" "${OBJECTDIR}/MDD File System/SD-SPI.o.d" ${OBJECTDIR}/main.o.d ${OBJECTDIR}/uart.o.d ${OBJECTDIR}/spi_lib.o.d ${OBJECTDIR}/st7735.o.d ${OBJECTDIR}/sd_helper.o.d ${OBJECTDIR}/ld3320.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/MDD\ File\ System/FSIO.o ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o ${OBJECTDIR}/main.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/spi_lib.o ${OBJECTDIR}/st7735.o ${OBJECTDIR}/sd_helper.o ${OBJECTDIR}/ld3320.o

# Source Files
SOURCEFILES=MDD File System/FSIO.c MDD File System/SD-SPI.c main.c uart.c spi_lib.c st7735.c sd_helper.c ld3320.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ64GA002
MP_LINKER_FILE_OPTION=,-Tp24FJ64GA002.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/MDD\ File\ System/FSIO.o: MDD\ File\ System/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/MDD\ File\ System 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/FSIO.o.d 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/FSIO.o.ok ${OBJECTDIR}/MDD\ File\ System/FSIO.o.err 
	@${RM} "${OBJECTDIR}/MDD File System/FSIO.o" 
	@${FIXDEPS} "${OBJECTDIR}/MDD File System/FSIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MDD File System/FSIO.o.d" -o "${OBJECTDIR}/MDD File System/FSIO.o" "MDD File System/FSIO.c"    
	
${OBJECTDIR}/MDD\ File\ System/SD-SPI.o: MDD\ File\ System/SD-SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/MDD\ File\ System 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.d 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.ok ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.err 
	@${RM} "${OBJECTDIR}/MDD File System/SD-SPI.o" 
	@${FIXDEPS} "${OBJECTDIR}/MDD File System/SD-SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MDD File System/SD-SPI.o.d" -o "${OBJECTDIR}/MDD File System/SD-SPI.o" "MDD File System/SD-SPI.c"    
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/uart.o: uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o.ok ${OBJECTDIR}/uart.o.err 
	@${RM} ${OBJECTDIR}/uart.o 
	@${FIXDEPS} "${OBJECTDIR}/uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart.o.d" -o ${OBJECTDIR}/uart.o uart.c    
	
${OBJECTDIR}/spi_lib.o: spi_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/spi_lib.o.d 
	@${RM} ${OBJECTDIR}/spi_lib.o.ok ${OBJECTDIR}/spi_lib.o.err 
	@${RM} ${OBJECTDIR}/spi_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/spi_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi_lib.o.d" -o ${OBJECTDIR}/spi_lib.o spi_lib.c    
	
${OBJECTDIR}/st7735.o: st7735.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/st7735.o.d 
	@${RM} ${OBJECTDIR}/st7735.o.ok ${OBJECTDIR}/st7735.o.err 
	@${RM} ${OBJECTDIR}/st7735.o 
	@${FIXDEPS} "${OBJECTDIR}/st7735.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/st7735.o.d" -o ${OBJECTDIR}/st7735.o st7735.c    
	
${OBJECTDIR}/sd_helper.o: sd_helper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/sd_helper.o.d 
	@${RM} ${OBJECTDIR}/sd_helper.o.ok ${OBJECTDIR}/sd_helper.o.err 
	@${RM} ${OBJECTDIR}/sd_helper.o 
	@${FIXDEPS} "${OBJECTDIR}/sd_helper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/sd_helper.o.d" -o ${OBJECTDIR}/sd_helper.o sd_helper.c    
	
${OBJECTDIR}/ld3320.o: ld3320.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ld3320.o.d 
	@${RM} ${OBJECTDIR}/ld3320.o.ok ${OBJECTDIR}/ld3320.o.err 
	@${RM} ${OBJECTDIR}/ld3320.o 
	@${FIXDEPS} "${OBJECTDIR}/ld3320.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ld3320.o.d" -o ${OBJECTDIR}/ld3320.o ld3320.c    
	
else
${OBJECTDIR}/MDD\ File\ System/FSIO.o: MDD\ File\ System/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/MDD\ File\ System 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/FSIO.o.d 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/FSIO.o.ok ${OBJECTDIR}/MDD\ File\ System/FSIO.o.err 
	@${RM} "${OBJECTDIR}/MDD File System/FSIO.o" 
	@${FIXDEPS} "${OBJECTDIR}/MDD File System/FSIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MDD File System/FSIO.o.d" -o "${OBJECTDIR}/MDD File System/FSIO.o" "MDD File System/FSIO.c"    
	
${OBJECTDIR}/MDD\ File\ System/SD-SPI.o: MDD\ File\ System/SD-SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/MDD\ File\ System 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.d 
	@${RM} ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.ok ${OBJECTDIR}/MDD\ File\ System/SD-SPI.o.err 
	@${RM} "${OBJECTDIR}/MDD File System/SD-SPI.o" 
	@${FIXDEPS} "${OBJECTDIR}/MDD File System/SD-SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MDD File System/SD-SPI.o.d" -o "${OBJECTDIR}/MDD File System/SD-SPI.o" "MDD File System/SD-SPI.c"    
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/uart.o: uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o.ok ${OBJECTDIR}/uart.o.err 
	@${RM} ${OBJECTDIR}/uart.o 
	@${FIXDEPS} "${OBJECTDIR}/uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart.o.d" -o ${OBJECTDIR}/uart.o uart.c    
	
${OBJECTDIR}/spi_lib.o: spi_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/spi_lib.o.d 
	@${RM} ${OBJECTDIR}/spi_lib.o.ok ${OBJECTDIR}/spi_lib.o.err 
	@${RM} ${OBJECTDIR}/spi_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/spi_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi_lib.o.d" -o ${OBJECTDIR}/spi_lib.o spi_lib.c    
	
${OBJECTDIR}/st7735.o: st7735.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/st7735.o.d 
	@${RM} ${OBJECTDIR}/st7735.o.ok ${OBJECTDIR}/st7735.o.err 
	@${RM} ${OBJECTDIR}/st7735.o 
	@${FIXDEPS} "${OBJECTDIR}/st7735.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/st7735.o.d" -o ${OBJECTDIR}/st7735.o st7735.c    
	
${OBJECTDIR}/sd_helper.o: sd_helper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/sd_helper.o.d 
	@${RM} ${OBJECTDIR}/sd_helper.o.ok ${OBJECTDIR}/sd_helper.o.err 
	@${RM} ${OBJECTDIR}/sd_helper.o 
	@${FIXDEPS} "${OBJECTDIR}/sd_helper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/sd_helper.o.d" -o ${OBJECTDIR}/sd_helper.o sd_helper.c    
	
${OBJECTDIR}/ld3320.o: ld3320.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ld3320.o.d 
	@${RM} ${OBJECTDIR}/ld3320.o.ok ${OBJECTDIR}/ld3320.o.err 
	@${RM} ${OBJECTDIR}/ld3320.o 
	@${FIXDEPS} "${OBJECTDIR}/ld3320.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ld3320.o.d" -o ${OBJECTDIR}/ld3320.o ld3320.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/pic24f_LD3320.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
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
