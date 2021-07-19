#include "kmain.h"

EFI_STATUS KeBootRenderRect(P_KE_GOP_INFO lpGopInfo, P_KE_RECT lpRect)
{
    if (!lpGopInfo || !lpRect)
        return -1;
    
    /* Lets not write past the vertical or horizontal resolution */
    if (lpRect->y > lpGopInfo->VerticalResolution)
        lpRect->y = lpGopInfo->VerticalResolution;
    if (lpRect->x > lpGopInfo->HorizontalResolution)
        lpRect->x = lpGopInfo->HorizontalResolution;

    for (int y = 0; y < lpRect->y; y++)
    {
        for (int x = 0; x < lpRect->x; x++)
        {
            *((UINT32*)(lpGopInfo->FrameBufferBase + lpGopInfo->Pitch * y + 4 * x)) = lpRect->color;
        }       
    }
}

EFI_STATUS KeBootClearScreen(P_KE_GOP_INFO lpGopInfo, UINT32 color)
{
    EFI_STATUS result = 0;
    KE_RECT rect = { 0 };

    if (!lpGopInfo)
        return -1;
    
    rect.x = lpGopInfo->HorizontalResolution;
    rect.y = lpGopInfo->VerticalResolution;
    rect.color = color;

    result = KeBootRenderRect(lpGopInfo, &rect);

    return result;
}

EFI_STATUS KeMain(P_KE_BOOTLOADER_INFO BootloaderInfo)
{
    KeBootClearScreen(&(BootloaderInfo->gopInfo), 0xFF0000);

    return EFI_SUCCESS;
}
