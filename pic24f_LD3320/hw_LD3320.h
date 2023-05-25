/*
Macros used when accessing the LD3320ARZ control hardware.
*/

#ifndef __HW_LD3320_H__
#define __HW_LD3320_H__

// FIFO
#define REG_FIFO_DATA               0x01
#define REG_FIFO_INT                0x02
#define REG_FIFO_EXT                0x05
#define REG_FIFO_STATUS_R           0x06
#define REG_FIFO_CLR                0x08

#define REG_FIFO_DATA_UPPERL        0x20
#define REG_FIFO_DATA_UPPERH        0x21
#define REG_FIFO_DATA_LOWERL        0x22
#define REG_FIFO_DATA_LOWERH        0x23
#define REG_FIFO_DATA_MCUL          0x24
#define REG_FIFO_DATA_MCUH          0x25
#define REG_FIFO_DATA_DSPL          0x26
#define REG_FIFO_DATA_DSPH          0x27

#define REG_FIFO_EXT_UPPERL         0x38
#define REG_FIFO_EXT_UPPERH         0x3A
#define REG_FIFO_EXT_LOWERL         0x3C
#define REG_FIFO_EXT_LOWERH         0x3E
#define REG_FIFO_EXT_MCUL           0x40
#define REG_FIFO_EXT_MCUH           0x42
#define REG_FIFO_EXT_DSPL           0x44
#define REG_FIFO_EXT_DSPH           0x46

// CLOCK
#define REG_CLOCK1_SET              0x11
#define REG_CLOCK2_SET              0x19
#define REG_CLOCK3_SET              0x1B
#define REG_CLOCK4_SET              0x1D
#define REG_CLOCK5_SET              0x79

// ADC set
#define REG_ADC_SET                 0x1C
#define REG_ADC_CONTROL             0x1E
#define REG_ADC_GAIN                0x35

// ASR
#define REG_ASR_CONTROL             0x37
#define REG_ASR_ASR_STATUS_R        0xBF
#define REG_ASR_DSP_STATUS_R        0xB2
#define REG_ASR_LENGHT_TIME         0xB8
#define REG_ASR_LENGHT_ADD          0xB9
#define REG_ASR_END                 0xBC
#define REG_ASR_KEY_INDEX           0xC1
#define REG_ASR_KEY_ADD             0xC3
#define REG_ASR_RESULT_NUM          0xBA
#define REG_ASR_RESULT1             0xC5
#define REG_ASR_RESULT2             0xC7
#define REG_ASR_RESULT3             0xC9
#define REG_ASR_RESULT4             0xCB

// DSP
#define REG_DSP_SET                 0x17
#define REG_DSP_SLEEP               0xCD

// ???
#define REG_INT_ENABLE              0x29
#define REG_INT_NUM_R               0x2B
#define REG_INT_AUX                 0xBA

#define REG_RESET                   0x1F
#define REG_SLEEP                   0xCF
#define REG_INIT_SET                0xBD

#define REG_MP3_CONTROL             0x33

#define REG_HEADSET_L               0x81
#define REG_HEADSET_R               0x83
#define REG_FEEDBACK_SET            0x85
#define REG_LINEOUT_SELECT          0x8F
#define REG_SPEAK_VOL               0x8E
#define REG_GAIN                    0x8D
#define REG_ANALOG_CONTORL1         0x87
#define REG_ANALOG_CONTORL2         0x89

//#define CFG_FIFO_DATA_INT_ENABLE

// command
#define CMD_SPI_READ                0x05
#define CMD_SPI_WRITE               0x04
#define CMD_INIT_ASR                0x00
#define CMD_INIT_MP3                0x02
#define CMD_DSP_ACTIVE              0x48
#define CMD_DSP_SLEEP               0x4C
#define CMD_ADC_INIT                0x00
#define CMD_ADC_EN                  0x0B
#define CMD_ANALOG_CONTORL_INIT     0x03
#define CMD_ANALOG_CONTORL_MP3      0xFF
#define CMD_SLEEP_EN                0x43
#define CMD_DSP_SLEEP_EN            0x04
#define CMD_ASR_RECOGNITION_Start   0x06  
#define CMD_ASR_KEY_ADD             0x04
#define CMD_FIFO_DATA_CLR           0x01
#define CMD_FIFO_EXT_CLR            0x04
#define CMD_OUT_INIT                0x00
#define CMD_MP3_START               0x01
#define CMD_MP3_STOP                0x00
#define CMD_FEEDBACK_INIT_MP3       0x5A
#define CMD_SPEAK_EN                0x70 | (1<<3)
#define CMD_FIFO_INT_EN             1<<2
#define CMD_FIFODATA_INT_EN         0x01

#define CAUSE_MP3_SONG_END           0x20
#define MASK_FIFO_STATUS_AFULL      0x08

#endif
