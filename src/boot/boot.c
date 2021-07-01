#include "boot/boot.h"

EFI_STATUS KeBootPrintDebug(EFI_SYSTEM_TABLE *SystemTable, WCHAR* buffer)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
    KE_RECT rect = { 0 };
    KE_GOP_INFO gopInfo = { 0 };

    /*
        Disable watchdog timer
    */
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    /*
        Enable the Graphics Output Protocol and set the video mode.
        TODO: Separate the enable from the video mode set.
    */
    KeBootEnableGOP(SystemTable, &gopInfo);

    KeBootClearScreen(&gopInfo, 0);

    /*
        Wait for keystroke
    */
    while ((Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY) ;
 
    return Status;
}
