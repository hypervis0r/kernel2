#include "boot/pe.h"

UINT32 Rva2Offset(UINT32 dwRva, UINTN uiBaseAddress)
{
	UINT16 wIndex = 0;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_PE_HEADERS pNtHeaders = NULL;

	pNtHeaders = (PIMAGE_PE_HEADERS)(uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew);

	pSectionHeader = (PIMAGE_SECTION_HEADER)((UINTN)(&pNtHeaders->OptionalHeader) + pNtHeaders->FileHeader.SizeOfOptionalHeader);

	if (dwRva < pSectionHeader[0].PointerToRawData)
		return dwRva;

	for (wIndex = 0; wIndex < pNtHeaders->FileHeader.NumberOfSections; wIndex++)
	{
		if (dwRva >= pSectionHeader[wIndex].VirtualAddress && dwRva < (pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].SizeOfRawData))
			return (dwRva - pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].PointerToRawData);
	}

	return NULL;
}
    
EFI_STATUS KeBootLoadPe(EFI_SYSTEM_TABLE *SystemTable, VOID* Buffer, P_KE_PE_IMAGE Image)
{
    PIMAGE_DOS_HEADER lpDosHeader = NULL; 
    PIMAGE_PE_HEADERS lpPeHeaders = NULL;
    PIMAGE_SECTION_HEADER lpSectionHeader = NULL;
    EFI_PHYSICAL_ADDRESS *lpSectionLocation = NULL;
    EFI_ALLOCATE_TYPE allocateType = 0;
    KE_PE_IMAGE peImage = { 0 };
    UINTN ImageBase = 0;


    if (!SystemTable || !Buffer)
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
    if (lpPeHeaders->OptionalHeader.Magic != OPTIONAL_HEADER_AMD64_MAGIC)
        return -1;

    /*
        Get first section header
        We can use this to get every section after
    */
    lpSectionHeader = (PIMAGE_SECTION_HEADER)(lpPeHeaders + 
                                                sizeof(UINT32) + 
                                                sizeof(IMAGE_FILE_HEADER) + 
                                                lpPeHeaders->FileHeader.SizeOfOptionalHeader);

    /*
        Allocate memory at ImageBase
    */
    ImageBase = lpPeHeaders->OptionalHeader.ImageBase;
    SystemTable->BootServices->AllocatePages(AllocateAddress,
                                                EfiLoaderData,
                                                EFI_SIZE_TO_PAGES(lpPeHeaders->OptionalHeader.SizeOfImage),
                                                &ImageBase);

    /* Copy DOS Header */
    SystemTable->BootServices->CopyMem(ImageBase, lpDosHeader, sizeof(IMAGE_DOS_HEADER));

    /* Copy PE Headers */
    SystemTable->BootServices->CopyMem(ImageBase + lpDosHeader->e_lfanew, 
                                        lpPeHeaders, 
                                        sizeof(UINT32) + 
                                            sizeof(IMAGE_FILE_HEADER) + 
                                            lpPeHeaders->FileHeader.SizeOfOptionalHeader);

    /* Copy Section Headers */
    SystemTable->BootServices->CopyMem(ImageBase + 
                                            lpDosHeader->e_lfanew + 
                                            sizeof(UINT32) + 
                                            sizeof(IMAGE_FILE_HEADER) + 
                                            lpPeHeaders->FileHeader.SizeOfOptionalHeader, 
                                        lpSectionHeader, 
                                        lpPeHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));

    /*
        Iterate over every PE section and load it into virtual memory
    */
    for (UINT32 i = 0; i < lpPeHeaders->FileHeader.NumberOfSections; i++) 
    {
        if (lpSectionHeader->Characteristics & IMAGE_SCN_CNT_CODE)
            allocateType = EfiLoaderCode;
        else
            allocateType = EfiLoaderData;
        lpSectionLocation = lpSectionHeader->VirtualAddress;

        SystemTable->BootServices->AllocatePages(AllocateAddress,
                                                allocateType,
                                                EFI_SIZE_TO_PAGES(lpSectionHeader->SizeOfRawData),
                                                &lpSectionLocation);

        SystemTable->BootServices->CopyMem(lpSectionLocation, lpSectionHeader->PointerToRawData, lpSectionHeader->SizeOfRawData);

        lpSectionHeader += sizeof(IMAGE_SECTION_HEADER);
    }
    
    // TODO: Fix relocations (lets hope this is a later todo and not a soon todo)
    
    peImage.EntryPoint = lpPeHeaders->OptionalHeader.AddressOfEntryPoint + ImageBase;
    peImage.ImageBase = ImageBase;
    peImage.ImageSize = lpPeHeaders->OptionalHeader.SizeOfImage;

    *Image = peImage;

    return EFI_SUCCESS;
}

EFI_STATUS KeBootCallPe(P_KE_PE_IMAGE Image, VOID* Parameter)
{
    if (!Image)
        return -1;
 
    asm volatile (
        "mov %1, %%rax\n\t"
        "mov %0, %%rcx\n\t"
        "jmp %1\n\t"
        : : "r" (Parameter), "r" (Image->EntryPoint) : "rax", "rcx");

    return EFI_SUCCESS;
}
