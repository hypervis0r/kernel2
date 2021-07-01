#include "boot/serial.h"

EFI_STATUS KeBootGetSerialProtocol(EFI_SYSTEM_TABLE *lpSystemTable, EFI_SERIAL_IO_PROTOCOL **lpSerialProtocol)
{
    EFI_SERIAL_IO_PROTOCOL* tempSerialProtocol = NULL;
    EFI_GUID serialGuid = EFI_SERIAL_IO_PROTOCOL_GUID; 
    EFI_STATUS result = 0;

    if (!lpSystemTable || !lpSerialProtocol)
        return -1;

    result = lpSystemTable->BootServices->LocateProtocol(&serialGuid, NULL, &tempSerialProtocol);
    if (result != EFI_SUCCESS)
        return -1;

    *lpSerialProtocol = tempSerialProtocol;

    return EFI_SUCCESS;
}

EFI_STATUS KeBootSerialWrite(EFI_SERIAL_IO_PROTOCOL* lpSerialProtocol, WCHAR* lpData, UINT32 size)
{
    EFI_STATUS result = 0;

    if (!lpData || !lpSerialProtocol)
        return -1;

    result = lpSerialProtocol->Write(lpSerialProtocol, &size, lpData);
    
    return result;
}
