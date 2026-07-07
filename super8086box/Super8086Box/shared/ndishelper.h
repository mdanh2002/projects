#pragma once

#include <Windows.h>
#include <winioctl.h>
#include <ntddndis.h>


#include "protuser.h"

ULONG NDISWriteBytes(HANDLE Handle, ULONG PacketLength, PUCHAR pWriteBuf);
ULONG NDISReadBytes(HANDLE Handle, ULONG PacketLength, PUCHAR pReadBuf);

void setupNDISDevice(unsigned char NDISDeviceIndex, DWORD FlagsAndAttributes);

HANDLE getNDISDeviceHandle();
