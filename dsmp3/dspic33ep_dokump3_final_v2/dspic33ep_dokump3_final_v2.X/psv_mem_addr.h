// This file contains memory addresses for large constants in PSV (Program Space Visbility) memory 
// Explicit declarations into X-memory (PSV-remappable), above MP3 constants, are needed
// Otherwise as the constant table grows large, the linker will be confused and allocate
// the constant section into address larger than 16-bit (e.g. 0x50e73, 24-bit?). This will cause
// the code to crash shortly after accessing the constant variables.

// It is very important to make sure that include files with PSV section declarations are not included multiple times
// otherwise the linker will attempt to process the declaration multiple times and generate a very misleading error message
// e.g. address for input section "array" (0xfae0) conflicts with output section "array" (0xfb10) as the "array" section is
// processed twice from double attempts to include that file!

// The architecture allows const in code space to be mapped into data memory at address 0xD000 offset until 0xFFFF
// By mapping the large into constants (0x8000 + 0x7000 = 0xF000), we have approximately 4KB of space for mapping.

// Alternatively if the data is very large and can be purely accessed as an array without using pointers, 
// we can simply declare it using __prog__, refer to icons.h and font.h for more information.

#define FATFS_TBLDEF_MEM_ADDR       (0x6084)    // right after mp3data which starts at 0x5000 (+0x8000 = 0xD000, remappable X-memory) with length 0x1084
#define FATFS_TBLDEF_MEM_LENGTH     (0x100)

#define FATFS_EXCVT_MEM_ADDR        (FATFS_TBLDEF_MEM_ADDR + FATFS_TBLDEF_MEM_LENGTH)
#define FATFS_EXCVT_MEM_LENGTH      (0x80)

#define FATFS_CVT1_MEM_ADDR        (FATFS_EXCVT_MEM_ADDR + FATFS_EXCVT_MEM_LENGTH)
#define FATFS_CVT1_MEM_LENGTH      (0x1F4)

#define FATFS_CVT2_MEM_ADDR        (FATFS_CVT1_MEM_ADDR + FATFS_CVT1_MEM_LENGTH)
#define FATFS_CVT2_MEM_LENGTH      (0xBC)

#define MAD_CRC_MEM_ADDR            (FATFS_CVT2_MEM_ADDR + FATFS_CVT2_MEM_LENGTH)
#define MAD_CRC_MEM_LENGTH          (0x200)
