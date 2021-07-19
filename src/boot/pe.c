#include "boot/pe.h"

EFI_STATUS KeBootLoadPe(EFI_SYSTEM_TABLE *SystemTable, VOID* Buffer, P_KE_PE_IMAGE Image)
{
    PIMAGE_DOS_HEADER lpDosHeader = NULL; 
    PIMAGE_PE_HEADERS lpPeHeaders = NULL;
    PIMAGE_SECTION_HEADER lpSectionHeader = NULL;
    EFI_PHYSICAL_ADDRESS *lpSectionLocation = NULL;
    EFI_ALLOCATE_TYPE allocateType = 0;
    KE_PE_IMAGE peImage = { 0 };

    if (!SystemTable || !Buffer || !Image)
        return -1;
    
    /*
        First bytes of PE file is DOS Header
    */
    lpDosHeader = (PIMAGE_DOS_HEADER)Buffer;

    /*
        Verify `Buffer` is valid PE file
    */
    if (lpDosHeader->e_magic != DOS_HEADER_MAGIC)
        return -1;

    /*
        PE Headers are located at IMAGE_DOS_HEADER.e_lfanew
    */
    lpPeHeaders = (PIMAGE_PE_HEADERS)(Buffer + lpDosHeader->e_lfanew);

    /*
        Verify PE Headers Signature
    */
    if (lpPeHeaders->Signature != PE_HEADERS_MAGIC)
        return -1;

    /*
        We only support AMD64 PE files
        DEATH TO 32 BIT.
    */
    if (lpPeHeaders->OptionalHeader->Magic != OPTIONAL_HEADER_AMD64_MAGIC)
        return -1;

    /*
        Iterate over every PE section and load it into virtual memory
    */
    lpSectionHeader = (PIMAGE_SECTION_HEADER)(lpPeHeaders + 
                                                sizeof(DWORD) + 
                                                sizeof(IMAGE_FILE_HEADER) + 
                                                lpPeHeaders->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < lpPeHeaders->FileHeader.NumberOfSections; i++) 
    {
        /*if (lpSectionHeader->Characteristics & IMAGE_SCN_CNT_CODE)
            allocateType = EfiLoaderCode;
        else
            allocateType = EfiLoaderData;*/
        lpSectionLocation = lpSectionHeader->VirtualAddress;

        SystemTable->BootServices->AllocatePages(AllocateAddress,
                                                EfiLoaderData,
                                                EFI_SIZE_TO_PAGES(lpSectionHeader->SizeOfRawData),
                                                &lpSectionLocation);

        lpSectionHeader += sizeof(IMAGE_SECTION_HEADER);
    }
    
    // TODO: Fix relocations (lets hope this is a later todo and not a soon todo)
    
    peImage.EntryPoint = lpPeHeaders->OptionalHeader->AddressOfEntryPoint;
    peImage.ImageBase = lpPeHeaders->OptionalHeader->ImageBase;
    peImage.ImageSize = lpPeHeaders->OptionalHeader->SizeOfImage;

    *Image = peImage;

    return EFI_SUCCESS;
}

EFI_STATUS KeBootCallPe(P_KE_PE_IMAGE Image, VOID* Parameter)
{
    if (!Image)
        return -1;
    
    EFI_STATUS (*EntryPoint)(VOID*) = Image->EntryPoint;

    EFI_STATUS Result = (*EntryPoint)(Parameter);

    return Result;
}
