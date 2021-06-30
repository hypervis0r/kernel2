#include "boot/graphics.h"

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

EFI_STATUS KeBootEnableGOP(EFI_SYSTEM_TABLE *SystemTable, P_KE_GOP_INFO lpGopInfo)
{
    EFI_STATUS status = 0;
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = NULL;
    UINTN SizeOfInfo, numModes, nativeMode = 0;

    if (!lpGopInfo)
        return -1;

    /*
        Locate GOP Protocol
    */
    status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, &gop);
    if(EFI_ERROR(status))
    {
        KeBootPrintDebug(SystemTable, L"Unable to locate GOP");
        return -1;
    }

    /*
        Query current video mode
    */
    status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED)
        status = gop->SetMode(gop, 0); /* We have to set the mode because UEFI shit */

    if(EFI_ERROR(status)) 
    {
        KeBootPrintDebug(SystemTable, L"Unable to get native mode");
        return -1;
    } 
   
    /*
        Set info 
    */
    nativeMode = gop->Mode->Mode;
    numModes = gop->Mode->MaxMode;

    /*
        Query all available video modes     
    */
    for (int i = 0; i < numModes; i++) {
        status = gop->QueryMode(gop, i, &SizeOfInfo, &info);
        KeBootPrintDebug(SystemTable, L"Found mode lol\r\n");
        /*Print(L"mode %03d width %d height %d format %x%s",
            i,
            info->HorizontalResolution,
            info->VerticalResolution,
            info->PixelFormat,
            i == nativeMode ? "(current)" : ""
        );*/
    }

    lpGopInfo->FrameBufferBase = gop->Mode->FrameBufferBase;
    lpGopInfo->FrameBufferSize = gop->Mode->FrameBufferSize;
    lpGopInfo->VerticalResolution = gop->Mode->Info->VerticalResolution;
    lpGopInfo->HorizontalResolution = gop->Mode->Info->HorizontalResolution;
    lpGopInfo->Pitch = 4 * gop->Mode->Info->PixelsPerScanLine;

    return EFI_SUCCESS;
}


