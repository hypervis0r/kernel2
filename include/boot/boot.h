#pragma once

#include <efi.h>
#include <efilib.h>

#include "boot/graphics.h"
#include "boot/serial.h"

#define EFI_PAGE_SHIFT  12
#define EFI_PAGE_MASK   0xFFF

#define EFI_SIZE_TO_PAGES (Size) (((Size) >> EFI_PAGE_SHIFT) + (((Size) & EFI_PAGE_MASK) ? 1 : 0))
#define EFI_PAGES_TO_SIZE (Pages) ((Pages) << EFI_PAGE_SHIFT)


typedef struct KE_BOOTLOADER_INFO
{
    EFI_MEMORY_DESCRIPTOR *lpMemoryMap;
    P_KE_GOP_INFO lpGopInfo;
} KE_BOOTLOADER_INFO, *P_KE_BOOTLOADER_INFO;

EFI_STATUS KeBootPrintDebug(EFI_SYSTEM_TABLE *SystemTable, WCHAR* buffer);
