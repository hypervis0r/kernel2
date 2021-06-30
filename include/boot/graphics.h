#pragma once

#include <efi.h>
#include <efilib.h>

typedef struct _KE_GOP_INFO
{
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINT32 FrameBufferSize;
    UINT32 VerticalResolution;
    UINT32 HorizontalResolution;
    UINT32 Pitch;
} KE_GOP_INFO, *P_KE_GOP_INFO;

typedef struct _KE_RECT
{
    UINT32 x;
    UINT32 y;
    UINT32 color;
} KE_RECT, *P_KE_RECT;

EFI_STATUS KeBootRenderRect(P_KE_GOP_INFO lpGopInfo, P_KE_RECT lpRect);

EFI_STATUS KeBootClearScreen(P_KE_GOP_INFO lpGopInfo, UINT32 color);

EFI_STATUS KeBootEnableGOP(EFI_SYSTEM_TABLE *SystemTable, P_KE_GOP_INFO lpGopInfo);
