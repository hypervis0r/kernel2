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
    
    /* Say hi */
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World\r\n"); // EFI Applications use Unicode and CRLF, a la Windows
    if (EFI_ERROR(Status))
        return Status;

    KeBootEnableGOP(SystemTable, &gopInfo);

    KeBootClearScreen(&gopInfo, 0);
    /* Now wait for a keystroke before continuing, otherwise your
       message will flash off the screen before you see it.
 
       First, we need to empty the console input buffer to flush
       out any keystrokes entered before this point */
    Status = SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
    if (EFI_ERROR(Status))
        return Status;
 
    /* Now wait until a key becomes available.  This is a simple
       polling implementation.  You could try and use the WaitForKey
       event instead if you like */
    while ((Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY) ;
 
    return Status;
}
