#include "boot/boot.h"

EFI_STATUS KeBootLoadFile(EFI_SYSTEM_TABLE *SystemTable, WCHAR* FilePath, VOID** Buffer, UINTN *BufferSize)
{
    EFI_STATUS status = 0;
    EFI_GUID SimpleFileSystemGUID = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_GUID FileInfoGUID =         EFI_FILE_INFO_ID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* lpSfs = NULL;
    EFI_FILE_PROTOCOL* hRootFs = NULL;
    EFI_FILE_PROTOCOL* hFile = NULL;
    EFI_FILE_INFO fileInfo = { 0 };
    UINTN fileInfoSize = sizeof(fileInfo);

    if (!SystemTable || !FilePath || !Buffer)
        return -1;

    status = SystemTable->BootServices->LocateProtocol(&SimpleFileSystemGUID, NULL, &lpSfs);
    if(EFI_ERROR(status))
    {
        KeBootPrintDebug(SystemTable, L"Unable to locate SFS");
        return -1;
    }

    status = lpSfs->OpenVolume(lpSfs, &hRootFs);
    if (EFI_ERROR(status))
        return -1;

    status = hRootFs->Open(hRootFs, &hFile, FilePath, EFI_FILE_MODE_READ, NULL);
    if (EFI_ERROR(status))
        return -1;
    
    status = hFile->GetInfo(hFile, &FileInfoGUID, &fileInfoSize, &fileInfo);
    if (EFI_ERROR(status))
        return -1;

    UINTN pagesToAllocate = EFI_SIZE_TO_PAGES(fileInfo->FileSize);
    status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, 
                                                        EfiLoaderData, 
                                                        pagesToAllocate,
                                                        *Buffer);
    if (EFI_ERROR(status))
        return -1;

    UINTN bytesAllocated = EFI_PAGES_TO_SIZE(pagesToAllocate);
    status = hFile->Read(hFile, &bytesAllocated, *Buffer);
    if (EFI_ERROR(status))
        return -1;

    *BufferSize = bytesAllocated;

    return EFI_SUCCESS;
}

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
    VOID* KernelBuffer = NULL;
    UINTN KernelBufferLength = 0;

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

    status = KeBootLoadFile(SystemTable, L"KERNEL.EXE", &KernelBuffer, &KernelBufferLength);
    if (EFI_ERROR(status))
    {
        KeBootSerialWrite(lpSerialProtocol, L"[-] Failed to load KERNEL.EXE\n", 31);
        return -1;
    }

    P_KE_PE_IMAGE kernelImage = NULL;
    status = KeBootLoadPe(SystemTable, KernelBuffer, kernelImage);
    if (EFI_ERROR(status))
    {
        KeBootSerialWrite(lpSerialProtocol, L"[-] Failed to load PE\n", 23);
        return -1;
    }

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
    KeBootCallPe(kernelImage, &BootloaderInfo);
    KeBootSerialWrite(lpSerialProtocol, L"We left the loader, now in kernel (not really)\n", 50);

    return Status;
}
