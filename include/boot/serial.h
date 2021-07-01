#pragma once

#include <efi.h>
#include <efilib.h>

EFI_STATUS KeBootGetSerialProtocol(EFI_SYSTEM_TABLE *lpSystemTable, EFI_SERIAL_IO_PROTOCOL **lpSerialProtocol);
EFI_STATUS KeBootSerialWrite(EFI_SERIAL_IO_PROTOCOL* lpSerialProtocol, WCHAR* lpData, UINT32 size);
