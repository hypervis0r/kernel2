#pragma once

#include <efi.h>
#include <efilib.h>

#include "boot/graphics.h"

EFI_STATUS KeBootPrintDebug(EFI_SYSTEM_TABLE *SystemTable, WCHAR* buffer);
