#include <stdio.h>
#include <atlbase.h>

#include "ndishelper.h"

#define MAC_ADDR_LEN 6

wchar_t selectedNDISDeviceName[512];
wchar_t selectedNDISDeviceDesc[512];

UCHAR           HostAdapterMacAddr[MAC_ADDR_LEN];
USHORT          EthType = 0x8e88;
#define MAX_NDIS_DEVICE_NAME_LEN        256
CHAR            NdisProtDevice[] = "\\\\.\\\\NdisProt";
CHAR *          pNdisProtDevice = &NdisProtDevice[0];

HANDLE      NDISDeviceReadHandle = INVALID_HANDLE_VALUE;
HANDLE      NDISDeviceWriteHandle = INVALID_HANDLE_VALUE;

#define NDIS_RECEIVE_PKT_SIZE 64

#define LOOPBACK_TEST_PACKET_LENGTH 100
#define LOOPBACK_TEST_COUNT			1

typedef struct _ETH_HEADER
{
	UCHAR       DstAddr[MAC_ADDR_LEN];
	UCHAR       SrcAddr[MAC_ADDR_LEN];
	USHORT      EthType;
} ETH_HEADER, *PETH_HEADER;


HANDLE
OpenHandle(
	__in __nullterminated CHAR    *pDeviceName, DWORD FlagsAndAttributes
	)
{
	LPSECURITY_ATTRIBUTES   lpSecurityAttributes = NULL;

	DWORD   CreationDistribution;
	HANDLE  Handle;
	DWORD   BytesReturned;
	DWORD ShareMode = 0;

	CreationDistribution = OPEN_EXISTING;

	Handle = CreateFile(
		pDeviceName,
		GENERIC_READ | GENERIC_WRITE,
		ShareMode,
		lpSecurityAttributes,
		CreationDistribution,
		FlagsAndAttributes,
		NULL
		);
	if (Handle == INVALID_HANDLE_VALUE)
	{
		printf("Creating NDIS handle failed, error %x\n", GetLastError());
		return Handle;
	}

	//
	//  Wait for the driver to finish binding.
	//
	if (!DeviceIoControl(
		Handle,
		IOCTL_NDISPROT_BIND_WAIT,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		printf("IOCTL_NDISIO_BIND_WAIT failed, error %x\n", GetLastError());
		CloseHandle(Handle);
		Handle = INVALID_HANDLE_VALUE;
	}

	return (Handle);
}

BOOL GetMacOfHostAdapter(HANDLE Handle, PUCHAR  pSrcMacAddr)
{
	DWORD       BytesReturned;
	BOOLEAN     bSuccess;
	UCHAR       QueryBuffer[sizeof(NDISPROT_QUERY_OID) + MAC_ADDR_LEN];
	PNDISPROT_QUERY_OID  pQueryOid;

	printf("GetMacOfHostAdapter starts\n");

	pQueryOid = (PNDISPROT_QUERY_OID)&QueryBuffer[0];
	pQueryOid->Oid = OID_802_3_CURRENT_ADDRESS;
	pQueryOid->PortNumber = 0;

	bSuccess = (BOOLEAN)DeviceIoControl(
		Handle,
		IOCTL_NDISPROT_QUERY_OID_VALUE,
		(LPVOID)&QueryBuffer[0],
		sizeof(QueryBuffer),
		(LPVOID)&QueryBuffer[0],
		sizeof(QueryBuffer),
		&BytesReturned,
		NULL);

	if (bSuccess)
	{
		printf("GetMacOfHostAdapter: IoControl success, BytesReturned = %d\n", BytesReturned);
		memcpy(pSrcMacAddr, pQueryOid->Data, MAC_ADDR_LEN);
	}
	else
	{
		printf("GetMacOfHostAdapter: IoControl failed: %d\n", GetLastError());
	}

	return (bSuccess);
}

// return the device count populate the name of the selected device, if the index provide is valid
unsigned char EnumerateNetworkDevices(HANDLE  Handle, unsigned char selectDevInd)
{
	unsigned char retVal = 0;

	typedef __declspec(align(MEMORY_ALLOCATION_ALIGNMENT)) int QueryBindingCharBuf;
	QueryBindingCharBuf		Buf[1024];
	DWORD       		BufLength = sizeof(Buf);
	DWORD       		BytesWritten;
	DWORD       		i;
	PNDISPROT_QUERY_BINDING 	pQueryBinding;

	pQueryBinding = (PNDISPROT_QUERY_BINDING)Buf;

	i = 0;
	for (pQueryBinding->BindingIndex = i;
	/* NOTHING */;
		pQueryBinding->BindingIndex = ++i)
	{
		if (DeviceIoControl(
			Handle,
			IOCTL_NDISPROT_QUERY_BINDING,
			pQueryBinding,
			sizeof(NDISPROT_QUERY_BINDING),
			Buf,
			BufLength,
			&BytesWritten,
			NULL))
		{
			WCHAR* devName = (WCHAR *)((PUCHAR)pQueryBinding + pQueryBinding->DeviceNameOffset);
			WCHAR* devDesc = (WCHAR *)((PUCHAR)pQueryBinding + pQueryBinding->DeviceDescrOffset);

			printf("%2d. %ws\n     - %ws\n", pQueryBinding->BindingIndex, devName, devDesc);

			if (selectDevInd == retVal)
			{
				wcscpy(selectedNDISDeviceName, devName);
				wcscpy(selectedNDISDeviceDesc, devDesc);
			}

			memset(Buf, 0, BufLength);
			retVal++;
		}
		else
		{
			ULONG   rc = GetLastError();
			if (rc != ERROR_NO_MORE_ITEMS)
			{
				printf("EnumerateDevices: terminated abnormally, error %d\n", rc);
			}
			break;
		}
	}


	return retVal;
}

BOOL OpenNdisDevice(
	HANDLE                          Handle,
	__in __nullterminated CHAR      *pDeviceName
	)
{
	WCHAR   wNdisDeviceName[MAX_NDIS_DEVICE_NAME_LEN];
	INT     wNameLength;
	INT     NameLength = strlen(pDeviceName);
	DWORD   BytesReturned;
	INT     i;


	//
	// Convert to unicode string - non-localized...
	//
	wNameLength = 0;
	for (i = 0; i < NameLength && i < MAX_NDIS_DEVICE_NAME_LEN - 1; i++)
	{
		wNdisDeviceName[i] = (WCHAR)pDeviceName[i];
		wNameLength++;
	}

	wNdisDeviceName[i] = L'\0';

	printf("Trying to access NDIS Device: %ws\n", wNdisDeviceName);

	return (DeviceIoControl(
		Handle,
		IOCTL_NDISPROT_OPEN_DEVICE,
		(LPVOID)&wNdisDeviceName[0],
		wNameLength*sizeof(WCHAR),
		NULL,
		0,
		&BytesReturned,
		NULL));

}

ULONG NDISWriteBytes(HANDLE Handle, ULONG PacketLength, PUCHAR pWriteBuf) {
	DWORD       BytesWritten;
	BOOLEAN     bSuccess;

	printf("NDISWriteBytes starts. PacketLength=%d\n", PacketLength);

	bSuccess = (BOOLEAN)WriteFile(Handle, pWriteBuf, PacketLength, &BytesWritten, NULL);
	if (!bSuccess)
	{
		printf("NDISWriteBytes: WriteFile failed on Handle %p\n", Handle);
		return 0;
	}

	printf("NDISWriteBytes: sent %d bytes\n", BytesWritten);
	if (pWriteBuf)
	{
		free(pWriteBuf);
	}

	return BytesWritten;
}

ULONG NDISReadBytes(HANDLE Handle, ULONG PacketLength, PUCHAR pReadBuf) {
	BOOLEAN     bSuccess;
	ULONG       BytesRead;

	printf("NDISReadBytes starts. PacketLength=%d\n", PacketLength);

	pReadBuf = (PUCHAR)malloc(PacketLength);
	if (pReadBuf == NULL)
	{
		printf("NDISReadBytes: failed to alloc %d bytes\n", PacketLength);
		return 0;
	}

	bSuccess = (BOOLEAN)ReadFile(Handle, (LPVOID)pReadBuf, PacketLength, &BytesRead, NULL);
	if (!bSuccess)
	{
		printf("NDISReadBytes: ReadFile failed on Handle %p, error %x\n", Handle, GetLastError());
		return 0;
	}

	printf("NDISReadBytes: read %d bytes\n", BytesRead);
	if (pReadBuf)
	{
		free(pReadBuf);
	}

	return BytesRead;
}

void NDISLoopbackTest(HANDLE Handle, ULONG PacketLength) {
	PETH_HEADER pEthHeader;
	PUCHAR pWriteBuf = NULL;
	PUCHAR      pData;
	UINT        i;

	// send parcel to same MAC address. If setup is correct, we should receive it
	pWriteBuf = (PUCHAR)malloc(PacketLength);
	if (pWriteBuf == NULL)
	{
		printf("ndisWriteBytes: Failed to malloc %d bytes\n", PacketLength);
		return;
	}

	pEthHeader = (PETH_HEADER)pWriteBuf;
	pEthHeader->EthType = EthType;
	memcpy(pEthHeader->SrcAddr, HostAdapterMacAddr, MAC_ADDR_LEN);
	memcpy(pEthHeader->DstAddr, HostAdapterMacAddr, MAC_ADDR_LEN);

	pData = (PUCHAR)(pEthHeader + 1);
	for (i = 0; i < PacketLength - sizeof(ETH_HEADER); i++)
	{
		*pData++ = (UCHAR)i;
	}

	ULONG writeCount = NDISWriteBytes(Handle, PacketLength, pWriteBuf);

	PUCHAR readBuf = NULL;
	ULONG readCount = NDISReadBytes(Handle, PacketLength, readBuf);
	if (writeCount == readCount && readCount > 0)
	{
		printf("NDISLoopbackTest completed successfully\n");
	}
	else {
		printf("NDISLoopbackTest failed. Bytes sent and received do not match\n");
	}
}

void setupNDISDevice(unsigned char NDISDeviceIndex, DWORD FlagsAndAttributes) {
	NDISDeviceReadHandle = OpenHandle(pNdisProtDevice, FlagsAndAttributes);

	if (NDISDeviceReadHandle == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open NDIS driver %s for reading. Make sure that app is running as administrator and that NDIS Protocol Driver has been installed and started with 'net start ndisprot'.\n", pNdisProtDevice);
	}
	else {
		printf("Available network devices:\n");
		unsigned char devCount = EnumerateNetworkDevices(NDISDeviceReadHandle, NDISDeviceIndex);
		if (NDISDeviceIndex >= devCount || NDISDeviceIndex < 0)
		{
			printf("Invalid NDIS device index: %d\n", NDISDeviceIndex);
		}
		else {
			printf("Selected NDIS device: #%d - %ws (%ws)\n", NDISDeviceIndex, selectedNDISDeviceName, selectedNDISDeviceDesc);

			USES_CONVERSION;

			if (!OpenNdisDevice(NDISDeviceReadHandle, W2A(selectedNDISDeviceName)))
			{
				printf("Failed to access %ws for reading\n", selectedNDISDeviceName);
			}
			else {
				printf("Opened device %ws for reading successfully!\n", selectedNDISDeviceName);

				if (!GetMacOfHostAdapter(NDISDeviceReadHandle, HostAdapterMacAddr))
				{
					printf("Failed to obtain host adapter MAC address\n");
				}
				else
				{
					printf("Host Adapter MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", HostAdapterMacAddr[0], HostAdapterMacAddr[1], HostAdapterMacAddr[2], HostAdapterMacAddr[3], HostAdapterMacAddr[4], HostAdapterMacAddr[5]);
				}
			}
		}
	}
}

HANDLE getNDISDeviceHandle()
{
	return NDISDeviceReadHandle;
}

HANDLE getNDISDeviceWriteHandle()
{
	return NDISDeviceWriteHandle;
}