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
    EFI_SERIAL_IO_PROTOCOL *lpSerialProtocol = NULL;
    KE_RECT rect = { 0 };
    KE_GOP_INFO gopInfo = { 0 };
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN LocalMapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;
    KE_BOOTLOADER_INFO BootloaderInfo = { 0 };

    KeBootGetSerialProtocol(SystemTable, &lpSerialProtocol);
    KeBootSerialWrite(lpSerialProtocol, L"[+] Serial Hello World\n", 48);
    
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

    KeBootPrintDebug(SystemTable, L"Fuck you\n");

    /*
        Wait for keystroke
    */
    //while ((Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY) ;

    

    /*
        Get memory map.
        Ironically, calling GetMemoryMap will require you to allocate memory for the map itself, 
        which, in turn, will change the memory map.
    */
    do {  
        Status = SystemTable->BootServices->GetMemoryMap(
                &MemoryMapSize, 
                MemoryMap, 
                &LocalMapKey, 
                &DescriptorSize,
                &DescriptorVersion);
        if (Status == EFI_BUFFER_TOO_SMALL){
            SystemTable->BootServices->AllocatePool(EfiLoaderData, MemoryMapSize + 1, &MemoryMap);
            Status = SystemTable->BootServices->GetMemoryMap(
                    &MemoryMapSize, 
                    MemoryMap, 
                    &LocalMapKey, 
                    &DescriptorSize,
                    &DescriptorVersion);      
        }
        //DbgPrint(L"This time through the memory map loop, status = %r\n",Status);
    } while (Status != EFI_SUCCESS);

    /*
        Exit the bootloader and jump to the kernel
    */
    Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MemoryMap);

    BootloaderInfo.lpMemoryMap = MemoryMap;
    BootloaderInfo.lpGopInfo = lpGopInfo;

    // TODO: Jump to kernel kek
    //KernelEntryPoint(&BootloaderInfo);
    KeBootSerialWrite(lpSerialProtocol, L"We left the loader, now in kernel (not really)\n", 50);

    return Status;
}
