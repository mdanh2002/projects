Super 8086 Box 
==============

Super 8086 Box is a 8088 emulator with MT-32, Adlib sound card, Covox/Disney, joystick and NE2000 emulation. It is developed by ToughDev (http://www.toughdev.com) 
The emulator is based on 8086 Tiny Plus, available for download from here https://jaybertsoftware.weebly.com/8086-tiny-plus.html
8086 Tiny Plus was originally improved by Julian Olds from 8086 Tiny, developed by Adrian Cable, which is also available here https://github.com/adriancable/8086tiny

TODO / WISHLIST
====================
(in order of ascending level of difficulty)

EASY 
Add ability to select config file at startup if default.cfg cannot be found
Stop immediately if config file parsing error is detected
Export NE2000 ID Byte to config (for compatability with some software)
Add Emulation > About Dialog and change app name / project name to Super 8086 Box. Add installer with sample disk image.
Check randomized MAC address idea (maybe force manufacturer bytes, otherwise app cannot receive incoming data - router refusal, or just remove it).
Maybe randomize the last 2 numbers only.

MEDIUM
Add configuration manager (load / create different config). Maybe in C#
Add ability to capture frame (to BMP, JPG?) and video (use FFMPEG?)
Menu option to send special keys (Ctrl-Alt-Del, Ctrl-Esc, Alt-Tab to app).
Added graphics indicator for floppy disk, hard disk and network access (bandwidth up/down) on the right of menu bar
Improve choppy PC speaker sound (figure out why it does not work with SBPlay on PC speaker for WAV files)
	- BIOS INT for delay issues (check DIGPAK PC speaker / Covox driver for the delay)
	- Write WAV player using Turbo C for PC speaker & Disnet / Covox
Revisit full-screen method (Direct X, OpenGDI, etc.). Add option for phosphore monochrome (amber, green etc.). Color mask to be selected in config.
Improve debugger behaviour
	- Default memory dump should be in same ES segment as instruction pointer 
	- Show instruction opcodes 
	- Show previous instruction during debugger stepping 
	- Highlight current instruction bytes in memory dump 
	- Highlight address of bytes pointed to by instruction param, where applicable
	- Button to dump arbitrary range of memory immediately, and not until next instruction 
	- Checkbox to save disassembly & memory dump (as binary or as text)
	- Option to modify arbitrary memory area (and update instruction list)
Add some of the config file options to menu for changing at runtime and maybe config manager app written in C#/.NET to start multi-instances
Add option to save state (memory content, values of various registers, etc.)

	
	
HARD 
Implement printer emulation and add BIOS INT 17H (parallel port services)
Add option to choose between printer / Disney emulation 
Add parallel port data redirection (for Disney Sound Source / printer)
Add BIOS INT 15H joystick (for AlleyCat, may not be possible)
Add BASIC in ROM support (cassette basic https://github.com/jeffpar/pcjs/tree/master/devices/pcx86/rom)
Add Sound Blaster 16 support using https://github.com/ntddk/temu/blob/master/hw/sb16.c. Figure out how to do DMA.
Add floppy drive B support and add (or check?) ability to boot from hard drive directly. Currently cannot boot from HDD (DOS version?)
Add VGA support.
Add option to save state 
Add CD-ROM emulation (ISO file mount)

BUGS TO BE FIXED 
MIDI_DEVICE in config file is currently ignored (always use default device)
Some CGA games do not work properly (partial display), e.g. Grand Prix Circuit or PAKU. Need to emulate unimplemented CGA modes 
Stuck ALT key if app is not used for a while (suspect due to MENU in window mode). Stuck SHIFT key in some games after a while, e.g. Prince of Persia. Most obvious in Norton Commander status bar.
NC5 can't run. NC3 is OK.
(Btw, DOS Controller does not scan INT13 extension. Hence network drivers are not shown)
Sometimes restarting the emulator will cause x1 display shown in x2 dialog. Not sure how this is possible. Cannot replicate.
Occasional repeated alphanumeric key strokes and occasional minor display corruption
Occasionally emulator runs slowly, especially when PC was busy when app starts. Emulator speeds reported at 20Mhz (got to do with TimerTicks(4) call, and not 1).
MTCP telnet works, but cursor is weird. Same as VRE/VRD (cursor moves to next line after launch) - because VRE/VRD supports VGA only.
BKED works fine, in black and white mode.

NOT DOING
Implement various unimplemented 80188 instructions (e.g. IMUL). This causes Turbo Debugger 2.01, HIEW and some other software to crash.
Use Insight DOS Debugger instead, which uses purely 8086 instructions. 
(Insight Debugger has issues on modern computers, TD has no such issues)



DEVELOPMENT NOTES
====================
Research PCEM

Confirm NE2000 / WinPCAP emulation (prosmicuous mode) on other network cards and on XP/Win7/Win8/WIN10 (32-bit/64-bit)
Make sure WinPCAP still works if VirtualBox is started while app is running (it may say Send Fails and need to restart)

Need to test compatibility with NCSA Telnet and NETAMER for DOS (PPPD.COM - packet driver dialup emulator)
Test other DOS browser (Elinks.)

MINUET browser for DOS works but does not support virtual host 
MINUET FTP works (some difficulties while navigating directory with long names)

With INSW fix, NE2000 works well in both 8-bit and 16-bit mode.
Tested with NE2000 8-bit and 16-bit packet drivers as well as with NDIS2 driver for UMC9003 ISA Network Card (SUPER2.DOS)
Can mount and copy files from Win7 Virtual Box.
6MB transfer in 110sec (~55KB/sec) running at 4.77MHz
Checksum test with DOS CRC.EXE matches (file integrity maintained)	

UE2000.DOS driver is buggy (mount share drive OK, random crash)
Does not work with Microsoft NE2000 default driver
Some drivers check for correct ID (EN0_RTL8029ID1, EN0_RTL8029ID0) before loading
For example UMC9008 expects 08 90 bytes

---------

REP INSW (80186 emulation) in 8086TinyPlusVCC.cpp (opcode 59, line 869) is buggy due to type
The code simply repeats the read and update the same value in IOPorts in a for loop without the rest of the emulator execution running
DOS application will simply reads 90 because memory is never updated!
Fixed by ToughDev

Affected: NE2000 16-bit drivers

Most NE2000 NDIS drivers works in 16-bit mode
NE2000 16-bit packet driver attempts to detect 80186 CPU, initialize the NE2000 in 16-bit mode and uses REP INSW instruction, which failed in the original 8086 Tiny Plus code due to above bug
Modified NE2000 8-bit packet driver works well.

https://jeffpar.github.io/kbarchive/kb/090/Q90336/

NE2000 - 16-bit default
NE1000 - 8-bit default

Implementing proper LPT1 emulation is hard as the BIOS provided does not have any codes  for int 17h
Most DOS app uses int 17h to access the printer!

xfs191 does not work, crash with weird chars (8088 not supported, need at least V20)
http://www.vcfed.org/forum/showthread.php?67947-NFS-on-XT
http://freedos.sourceforge.net/wiki/index.php/NFS#8088
http://www.bttr-software.de/forum/mix_entry.php?id=13587
need xt-191.zip (not found?)

xfs185
xfs176 
xfs171.zip

XFS185 does not crash but use IMUL (also cannot work)

Arachne does not work even on CGA (hang with mouse)

NE2000 is at IRQ 7 (INT 0x0F)
This is originally for a LPT1 printer to signal printer not busy
but since our LPT1 is only for Disney/Covox, which does not use IRQ, this works just fine

NE2000 org packet driver also supports 16-bit
need mod for 8-bit http://www.vcfed.org/forum/showthread.php?41081-NE2000-Packet-drivers-for-8-bit-slots
(COM file provided works, ASM src code is not correct, only differ by single IN AL, DX instr, which can't be true)

https://www.winvistatips.com/threads/ndisprot-and-multiple-handles-to-the-same-device.188832/
concurrent read write on NDISprot
NDISPROT Read is blocking, also cannot use 2 handles, 1 for read, 1 for write
https://stackoverflow.com/questions/16608321/windows-ndis-driver-concurrent-read-write-on-a-single-device-irp-mj-read-write
"Thanks, the trick with the two handles works. Initially, this didn't work since the driver only allowed one handle to bind to a specific interface. 
But I had to alter the driver to allow multiple clients to be connected anyway. So now every client connects twice, once for read and once for write..."
need driver modification for 2 handles
Also NDISprot will not work for our use if set up on Wifi connection (802.11 vs 802.3 packets), thus defeating the purposes.


https://www.codeproject.com/Articles/5292/Raw-Ethernet-Packet-Sending
how to send raw Ethernet packet on Windows using NDIS driver

https://wiki.osdev.org/Ne2000
https://www.intervalzero.com/library/RTX/WebHelp/Content/PROJECTS/Subsystem/NE2000_Compatible_ISA_NIC.htm
https://www.embeddedrelated.com/showthread/comp.arch.embedded/181011-1.php
(NE2000 tutorial)

https://www.mcselec.com/index.php?option=com_content&task=view&id=90&Itemid=57
(library in VB)

https://github.com/MisterTea/MAMEHub/blob/master/Sources/Emulator/src/emu/bus/isa/ne2000.c
(too much OOP, cannot use cleanly)

https://github.com/OBattler/PCem-X/blob/master/PCem/ne2000.c
(some junk code)

https://github.com/sheepdog/qemu/blob/master/hw/net/ne2000.c 
(cleaner)

https://github.com/ntddk/temu/blob/master/hw/ne2000.c
(the best)

https://searchcode.com/codesearch/view/40125409/
http://www.jbox.dk/sanos/source/sys/dev/ne2000.c.html
(more info on NE2000 implementation)

NE2000 drivers for 8-bit slot
http://www.vcfed.org/forum/showthread.php?41081-NE2000-Packet-drivers-for-8-bit-slots

The original driver supports 16-bit slot only, and can be found in Crynwr packet drivers collection
http://www.crynwr.com/drivers/
http://wiki.freedos.org/wiki/index.php/Crynwr
https://archive.org/details/PKTD11A_ZIP

https://support.riverbed.com/content/support/software/steelcentral-npm/airpcap.html AirPCAP
For wifi capture 

https://www.winpcap.org/
Supports Windows XP - Windows 8 / 10. Still works but not maintained

https://nmap.org/npcap/
Actively maintained. SUpports Windows 7 and above 

WinPCAP can only work on Ethernet cards. Not wireless cards.
Wifi uses 802.11 frames whereas Ethernet uses 802.3 frames (and 802.1Q for VLAN). The 2 types of frame are similar and not the same
Since DOS network tools are meant for Ethernet and send 802.3 frames, we can't use AirPCAP to interface 80866 tiny with Wireless cards.
If we do, frames meant for Ethernet will be sent to the Wireless cards and confuse the device.
Additionally, 802.11 frames (except the header) are also encrypted (following the Wifi encryption) which will not be easy to implement.
802.11 frames have 4 address MAC fields whereas 802.3 frames only have 1.
AirPCAP sample contains low level interface codes (sending PPI, scan for AP etc.) which are not suitable.

WinPCAP/AirPCAP works at layer 2 (link layer) whereas NDIS drivers works above level 2 and below level 3 (network layer).
NDIS drivers can work with both Ethernet / Wireless cards and it encapsulates the network packets into suitable frame for the interface (wifi/ethernet) before sending
them down to level 2.



--- BIOS reports no LPT ports, lpt1addr in bios_data area in bios_cga.asm is 0. 
--- Compiles BIOS with NASM 2.11 DOS version, https://www.nasm.us/pub/nasm/releasebuilds/2.11/dos/
--- Joystick programming tutorial 
http://www.fysnet.net/joystick.htm
http://www.epanorama.net/documents/joystick/pc_joystick.html#programming
Just implement port 0x201 read (BIOS INT 15h for joystick not implemented)

https://docs.microsoft.com/en-us/windows/win32/multimedia/joystick-reference


Patch Insight DOS Debugger to use Ctrl-ENTER for resident activate [DONE]
Default key is Ctrl-Esc (not possible when running on emulator in Windows)
Responsible code is in RESIDENT.INC. Insight overrides INT 03 (breakpoint), INT 16 (Keyboard Interrupt) and INT 16 (Read key stroke)
INT 16 is purely used to detect if Insight is already resident (via magic code etc.)
INT 09 is overidden by means of set_09_vector method which is located in TRACE.INC, which calls int_09_handler

		push	ax
		in	al,60h
		cmp	al,SCAN_CODE
		jne	@@quit
		push	ds
		xor	ax,ax
		mov	ds,ax
		mov	al,[417h]
		test	al,SHIFT_CODE
		jnz	int_09_execute
		pop	ds

ScanCode and Shift code is defined in INSIGHT.INC

SCAN_CODE		equ	1 ; ESC (https://www.millisecond.com/support/docs/v5/html/language/scancodes.htm)
SHIFT_CODE		equ	4

Changing SCAN_CODE to 28 (0x0C) allows Ctrl-ESC to work as popup key. 
This can be done by patching byte at offset 53C5 for Insight 1.24 from 3C01 to 3C1C
(There is a bug in Hiew, or maybe by design, it does not continue searching for the next occurence from the last match, you'll need to move the cursor a few bytes down)
Also need to replace "ESC" with "ENT" (same bytes) for correct prompt.


Games tested with Adlib/MT-32
- Paku (sound works, does not support unimplemented CGA modes)
- Bumpy (sound works, EGA/VGA only)
- Gobliiins (only static heard, maybe require new Adlib model)
- Sticky Town (only static heard, maybe require new Adlib model)
- Secret of Monkey Island
- Prince of Persia
- Prehistorik
- Cycles Arcade Racing
- Grand Prix Circuit

Games tested with Disney Sound Source
- SBPLAY with Digipak (sounds plays too slow (2.1 times), maybe due to app issues, since same issues exists when using PC speaker. Just noises generated.
- Commander Keen - Keen Dreams (patch code to show Disney Sound Source, detected but wrong sound played because Disney/SoundBlaster samples were never ready)

- All dogs go to heaven (no sound generated, hang at end of demo scene with continuous INT10 output in console logs)

- Spell It Plus! (LPT only used for speech, most sounds on PC speaker)
- Chuck Yeager's Air Combat (starts with MCGA Covox1. Need yeager.com to crack). Param in .EXE file after extracting with UNP411.
- 688 Attack Sub 
    High pitch background noise low volume in Adlib mode 
	Issues with TITLE.EXE (first menu to select video mode doesn't show, need to press 3 for CGA). 
	After that menu for selecting audio can show normally and main game will proceed.
	GAME.EXE accepts "3" as parameter for CGA mode. It relies on TITLE.EXE to set video mode and cannot run on its own, 
	unless we write our own app to set video mode and run "GAME.EXE 3" manually.
	
	BIOS has issues with the following INT 10, AH = 0Eh calls

	mov ah, 0Eh
	mov al, 97    ; 'a'
	mov bl, 0fh	  ; foreground color (in graphics modes)
	mov bh, 00h	  ; display page (in text mode, aka alphanumeric / alpha mode)
	int 10h

	Display page is from 0-7 (usually 0-1), see http://www.ctyme.com/intr/rb-0069.htm#Table10
	Some apps (TITLE.EXE above) passes 70h to BH, which works fine in DosBox, but shows nothing in our 8088 emulator.
	The app continues to work if correct key is pressed, just that output is not displayed

	Responsible function is at int10_write_char_tty (bios_cga.asm line 1521)

	Line 1575 
	; Get video page offset into BP
	mov	al, [vid_page_size+1-bios_data]
	mul	byte bh
	mov	ah, al
	mov	al, 0
	mov	bp, ax

	Line 1686 :

	int10_write_char_tty_set_crtc_cursor:
	cmp	bh, [disp_page-bios_data]
	jne	int10_write_char_tty_done

	Temporarily fixed by setting BH to 0 if BH is greater than 7



- Prince of Persia
- King's Quest V
- Arachnophobia
- The Treehouse  (intro sound on PC speakers, rest of game on LPT sound)
- Electrobody (CGA support) / Electroman (no CGA support, sound still plays). Sounds a bit too slow/chopped off, need frequency adjustment
- Super Jeopardy
- Stickybear Town Builder

f_prot 2.25A, hangs at load
http://annex.retroarchive.org/cdrom/cotc-23/VIRUS/index.html

Others
- BKAV384 (graphics does not show, require VGA) - dated 2001-10-22 14:36:40
- d2_319.zip (requires VGA card)

VET for DOS 9.531
http://minuszerodegrees.net/software/software.htm 

Debugging needs to use Insight 1.24 (https://www.bttr-software.de/products/insight/) which uses solely 8088 instructions. Turbo Debugger detects 80187 CPU and attempts to use IMUL which is not implemented.
Turbo Debugger can be configured to be resident and activated using configurable hot key 
Insight activation key is Ctrl-Esc (which cannot be used in Windows) and cannot be changed (need code compilation)

Instruction timing implementation is 8086 Tiny Plus is flawed. Calls to TimerTick(4) causes the emulator to update 4 instructions per interval.
CPU Speed detected as 30MHz in SI.EXE and CHECKIT. Varies at times and depending on nTicks param passed to TimerTick
This causes Covox Speech Thing / Disney Sound application to play too slowly. Reducing to TimerTick(1) and application plays slightly faster (around 1.5 times too slow)
but then PC speaker audio becomes choppy.

PC Speaker Audio will also not work with SBPlay (high pitch tone generated, nothing else).

Prince of Persia and other games (Tree House) has no issues.

Joystick tested with

+ Dave, works well
+ AV Arcade Volleyball, works fine
+ Prince of Persia 1, works well
+ Prehistorik, works well
+ California games, works fine
+ Commander Keen - Keen Dreams - works well
+ King's Quest 4 - works fine
+ Eclipse - works fine

+ Bruce Lee, works in menu. Hard to enter joystick calibration routine.
+ PTROOPER, works well. Cannot see bullet but that's because of unimplented INT10 mode.
+ Tetris, works fine. Tends to drop multiple pieces but that might be just because of the game
+ Sokoban 1988 99 levels, joysticks work, hang after a few screens (maybe due to something else)

+ ALLEYCAT, cannot work. Uses BIOS INT 15h/AH=84h for reading joystick, not port 0x201. INT15 not present on XT
(http://www.oocities.org/garyneal_71/GameLib/joystickBiosRoutines.html)
+ Secret of Monkey Island. Does not work at all. works in DOSBOX only to move mouse. Cannot click. Useless.


https://docs.microsoft.com/en-us/windows/win32/medfound/audioresampler
Windows DSP API for resampling
cannot run on XP

PATCHING BIOS
==================== 
See BIOS Data Area, equipment list
http://www.bioscentral.com/misc/bda.htm
http://stanislavs.org/helppc/bios_data_area.html
https://matrix.home.xs4all.nl/system/bda.html

Need to modify lpt1address in BIOS and modify equipment word to reflect number of parallel ports:

Bits 15-14	Number of installed parallel ports
 	00b	none
 	01b	1 parallel port
 	10b	2 parallel ports
 	11b	3 parallel ports

With updated BIOS most Sysinfo tools correctly reports number of parallel port counts, except MSD (which reports 0 parallel ports since it uses BIOS INT 17H to confirm the port status) 
https://faculty.kfupm.edu.sa/COE/aimane/assembly/pagegen-141.aspx.htm
and Checkit (which always reports 3 parallel ports available at default address regardless of BIOS settings)

LIBRARY USED
====================

YM3812 Emulator

https://raw.githubusercontent.com/ljbade/wolf4sdl/master/mame/fmopl.cpp
https://raw.githubusercontent.com/ljbade/wolf4sdl/master/mame/fmopl.h
http://198.100.149.83/ser/roms1/src/emu/sound/3812intf.c


Changes in Super 8086 Box 
====================
Added full screen support (F12 to exit, overridden by VS breakpoint)
Added Adlib sound card (YM3812) support
Added MT-32 (UART mode) support
Added NE2000 network card support
Miscellaneous bug fixes


Version 1.34 changes
====================
Improved MCGA compatibility. 
This involved intercepting every access to video memory, so some changes to the CPU
emulation were required. 
However, this is a useful stepping stone on the way to VGA support.
Microsoft Word 5.5 now works correctly in mode 11.
Support for CGA palette modification via MCGA palette registers added.
Various BIOS bug fixes.


Version 1.32 changes
====================
Added implementation of the INS and OUTS instructions.
Improved CGA emulation to support CRTC page offset in 320x200 graphics modes.
Improved BIOS compatibility.
BIOS bug fixes.
Improved PIT 8253 support - added mode 0 support for timer 0.
Implemented dummy port 0x0201 to make the system behave correctly for no joystick present.
Fixed SDL debug break on interrupt entry point handling.

Version 1.30
============
Initial release



MODIFICATIONS TO CPU EMULATION
==============================

Warning fixes:
--------------
The code has been cleaned up a bit to remove as many warnings as possible.

NOTE: 
When compiling using gcc with optimisation, make sure you use 
  -fno-strict-aliasing.
The code makes extensive use of type-punned pointers. Without this flag the compiler
assumes strict aliasing and optimises accordingly. This may generate incorrect
code when type-punning is used.

Other changes:
--------------
Exit via CS:IP = 0000:0000 has been removed. An exit flag is now used instead.
This is required because I found a program that carefully disables interrupts,
copies a small function over the interrupt vector table and then calls 0000:0000
to execute the function. It then restores the interrupt table and enables interrupts.
Why? I have no idea, but that is what it does!
To make this program work the exit on CS:IP = 0000:0000 cannot be used.

The CPU emulation has been extended to support some of the new instructions
in the 80186 and NEC V20/V30 CPUs.
The new instructions supported are:
  PUSH imm  : Push immediate
  PUSHA     : Push all registers
  POPA      : Pop all registers
  ENTER     : Format stack for procedure entry 
  LEAVE     : Restore stack on procedure exit
  INS, OUTS : I/O string operations
The instructions not currently supported are:
  BOUND    : This instruction is not useful on a PC
             On a bounds error INT 5 is triggered. On a PC this is the 
             print screen vector.
  IMUL imm : It could be useful, but I don't understand Adrian's MUL_MACRO
             well enough to have a stab at implementng it yet.
             It is low priority as I am yet to see a program actually use this.
             
This version is modified to separate the hardware emulation code from the 
CPU emulation. 
The H/W emulation and host system interface is provided by an interface class. 
Different H/W and host operating systems can be supported by replacing the
implemention of the interface class.

Currently the the following H/W interface classes are provided:
  . MS Windows API,
  . SDL 2

BIOS
====
Also provided is a BIOS that supports a CGA/MCGA graphics card.
This BIOS provides a far more complete support for INT 10h than the 
original 8086tiny BIOS.
It also provies support for INT 14h (serial port).
This BIOS is for a more accurate H/W emulation than the original 8086 tiny
and is NOT compatible with the original 8086 tiny.
This BIOS only uses video memory for video output (no text mode via ANSI).

MS Windows Implementation
=========================
The MS Windows implementation has been tested on Windows 7 and Windows 8.
Clicking in the emulation window will lock mouse input to the emulation.
To release the mouse either press F12 or Alt-Tab.

The MS Windows implementation of the interface class provides support for the 
following:

Graphics: 
  CGA/MCGA
  Graphics modes supported:
    0, 1 : 40x25 text
    2, 3 : 80x25 text
    4, 5 : 320x200, 4 colour CGA mode
    6    : 640x200 B/W CGA mode
    11   : 640x480 B/W MCGA
    13   : 320x200, 256 colours

Sound:
  PC Speaker

Serial Ports:
  Microsoft 2 button serial mouse emulation using system mouse
  Linking of emulation serial ports to host system serial ports.
  TCP/IP serial port emulation over network.

In addition the following features are supported:
  Emulator reset (cold reboot)
  Load/save of configuration files
  On the fly floppy disk swapping
  CPU speed control
  Serial port configuration
  Sound Configuration
  Display scaling (1x or 2x)
  Switch between CGA 8x8 or VGA 8x16 text font
  Built in debugger:
    . register display, 
    . disassembly, 
    . memory display, 
    . single step,
    . break points,
    . CPU tracing - dump last 64K instructions executed.


Limitations:
============
Currently, only boot from floppy disk image is supported.



SDL Implementation
==================
The SDL implementation has been tested on Windows 7 and Windows 8.
Clicking in the emulation window will lock mouse input to the emulation.
To release the mouse either press F12 or Alt-Tab.

Features are the same as for the Windows implementation.

Portability issues:
  1) Serial port hardware interface is currently only available for windows.
  2) The network code for serial over TCP was written for winsock.
     There may be compatability issues under linux.



Configuration File Format
=========================
When the emulation starts the initial configuration is read from default.cfg

The configuration file must follow exactly the following format. 
There should be no extra space or blank like, otherwise the app may crash. 
neither the order of the sections nor the name of the section should not be changed.
Only edit the values for each section to match your configuration.

[BIOS]
<bios filename>
[FD]
<floppy disk image file name>
[HD]
<hard disk image file name or NIL for none>
[CPU_SPEED]
<CPU speed in Hz>
[NE2000_MAC]
Mac address for the emulated NE2000 card, e.g. 00:1B:44:11:3A:B7. 
[NETWORK_DEVICE_INDEX]
Index of the host network device to be used for NE2000 emulation. The card should support prosmicuous mode. Refer to console log for device list. Only certain Ethernet devices are supported currently. 
[COM1]
<commport specifier>. Valid values: UNUSED, MOUSE, SERIAL_SERVER or SERIAL_CLIENT.
[COM2]
<commport specifier>
[COM3]
<commport specifier>
[COM4]
<commport specifier>
[SOUND_ENABLE]
<enable flag,  0 = off, 1 = on>
[SOUND_SAMPLE_RATE]
<sample rate in Hz>
[SOUND_VOLUME]
<volume % (0 .. 100)>
[MIDI_DEVICE]
Index of MIDI device to be used for MT-32 support. Refer to console log for device list. Set to -1 to disable MT-32 emulation
[JOYSTICK_DEVICES]
Set 1 to emulate a single joystick, 2 for dual joysticks, 0 to disable.
[ADLIB_ENABLE]
Set 1 to enable Adlib emulation, 0 to disable
[LPT1_MODE]
Set 2 for Disney Sound Source, 1 for Covox Speech Thing, 0 to disable
No normal parallel port behaviour (printer, text file) for now.
[FULL_SCREEN_ADJUST_X_PIXELS]
Any positive/negative integer. See note below
[FULL_SCREEN_ADJUST_Y_PIXELS]
Any positive/negative integer. See note below
[FULL_SCREEN_METHOD]
Set to 0/1/2. See note below


Full screen implementation:
	FULL_SCREEN_ADJUST_X_PIXELS and FULL_SCREEN_ADJUST_Y_PIXELS are the number of pixels which will be added/subtracted from the full screen width/height 
	before passing to StretchDIBits. Adjust this value to help reduce artifacts caused by StretchDIBits.
	Set FULL_SCREEN_METHOD to 0 to use StretchDIBits with default SetStretchBltMode flag (fast with artifact), 1 to use STRETCH_HALFTONE (slow) 
	or 2 to use internal algorithm.

The <comport specifier> can be:
  UNUSED  
    - com port is not in use
  MOUSE   
    - emulated microsoft serial mouse using the system pointing device
  TCP_SERVER:<port>
    - emulated over TCP/IP stream socket as server, connection port is <port>
      TCP data stream is the raw bytes sent/received on the serial port.
  TCP_CLIENT:<address>:<port>
    - emulated over TCP/IP stream socket as a client. 
      Connects to another 8086tiny running a TCP_SERVER serial port running on 
      host at <address>, connection port <port>.
      TCP data stream is the raw bytes sent/received on the serial port.
  COM:<port name> 
   - Link the COMM port to <port name> system serial port

NOTES: 
TCP/IP COM port emulation ignores baud rate (the baud rates at the two ends 
do not have to match for data to be sent/received)
It also makes the serial control lines act like a null modem cable when connected.

There is a bug in the windows sound system under windows 7.
As a result you must match the H/W sample rate in the sound emulation for sounds to 
be played correctly.


TESTED PROGRAMS
===============
OS versions:
  FreeDos (8086 build)
  DOS 3.3
  DOS 4.0
  DOS 5.0
  ELKS 1.5

Driver/TSRs:
  trumouse
    Works, but if the mouse is on COM3 or COM4 you must specify the 
    port on the command line (eg. For COM3: trumouse /C3)
  ctmouse
    Works automatically with mouse on any COM port.
  nnansi

Apps:
  MS-DOS EDIT (DOS 5.0) - requires loadfix
  QBASIC (DOS 5.0) - requires loadfix
  Microsoft Word 5.5

Games:
  Alley Cat
  Apple Panic
  Bard's Tale 1
  Bureaucracy
    Requires an ANSI driver loaded.
  Carrier Command
  Defender
    Needs a slow CPU. Also has the worst keyboard controls ever.
  Digger
    Run with the /C command line parameter for CGA.
  Elite
  Elite+ 
    Use MCGA 256 colours
  Eye of the Beholder
    Need to directly run start1.exe.
    Works with both CGA and MCGA/VGA. Strongly recommend MCGA/VGA.
  Faery Tale
  Heroes of Might and Magic 1
  Jump Man Lives (APOGEE)
  Legend
  Monuments of Mars
  Nord and Bert couldn't make head nor tail of it.
    Requires an ANSI driver loaded.
  Overkill
  Pango
    Needs a really slow CPU speed. Try 3 MHz.
  Pharoah's Tomb
  Populous
  Powermonger
    Requires loadfix.
    Needs a faster CPU to be playable, try 20 Mhz.
    Needs lots of RAM free. Use DOS 3.3 as it uses less RAM.
  Rogue 
  Sim City
  Starflight
  Starflight 2 
    Select VGA 256 colour - uses mode 13, so it is MCGA compatible.
  Starglider
  Starglider II
  Super Boulder Dash
  The Fool's Errand 
    Select VGA - uses mode 13, so it is MCGA compatible.
    Prologue requires loadfix and a CPU speed of at least 8 MHz to run correctly.
    Load/Save to hard disk is problematic, looks like a bug in the code.
    Trying to edit the filename When C: is selected locks up on DOSBOX and 
    causes graphic corruption in 8086 tiny.
    Save to A: is OK
  Ultima 1
  Xfighter