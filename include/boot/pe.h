#pragma once

#include <efi.h>

#include "boot/boot.h"

#define DOS_HEADER_MAGIC 0x5a4d
#define PE_HEADERS_MAGIC 0x00004550
#define OPTIONAL_HEADER_AMD64_MAGIC 0x020b

#define IMAGE_SCN_CNT_CODE 0x00000020

typedef struct KE_PE_IMAGE
{
    VOID* ImageBase;
    UINTN ImageSize;
    VOID* EntryPoint;
} KE_PE_IMAGE, *P_KE_PE_IMAGE;

typedef struct _IMAGE_DOS_HEADER
{
     UINT16 e_magic;
     UINT16 e_cblp;
     UINT16 e_cp;
     UINT16 e_crlc;
     UINT16 e_cparhdr;
     UINT16 e_minalloc;
     UINT16 e_maxalloc;
     UINT16 e_ss;
     UINT16 e_sp;
     UINT16 e_csum;
     UINT16 e_ip;
     UINT16 e_cs;
     UINT16 e_lfarlc;
     UINT16 e_ovno;
     UINT16 e_res[4];
     UINT16 e_oemid;
     UINT16 e_oeminfo;
     UINT16 e_res2[10];
     UINT32 e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
  UINT16  Machine;
  UINT16  NumberOfSections;
  UINT32 TimeDateStamp;
  UINT32 PointerToSymbolTable;
  UINT32 NumberOfSymbols;
  UINT16  SizeOfOptionalHeader;
  UINT16  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER {
  UINT16                 Magic;
  CHAR8                 MajorLinkerVersion;
  CHAR8                 MinorLinkerVersion;
  UINT32                SizeOfCode;
  UINT32                SizeOfInitializedData;
  UINT32                SizeOfUninitializedData;
  UINT32                AddressOfEntryPoint;
  UINT32                BaseOfCode;
  UINT64            ImageBase;
  UINT32                SectionAlignment;
  UINT32                FileAlignment;
  UINT16                 MajorOperatingSystemVersion;
  UINT16                 MinorOperatingSystemVersion;
  UINT16                 MajorImageVersion;
  UINT16                 MinorImageVersion;
  UINT16                 MajorSubsystemVersion;
  UINT16                 MinorSubsystemVersion;
  UINT32                Win32VersionValue;
  UINT32                SizeOfImage;
  UINT32                SizeOfHeaders;
  UINT32                CheckSum;
  UINT16                 Subsystem;
  UINT16                 DllCharacteristics;
  UINT64            SizeOfStackReserve;
  UINT64            SizeOfStackCommit;
  UINT64            SizeOfHeapReserve;
  UINT64            SizeOfHeapCommit;
  UINT32                LoaderFlags;
  UINT32                NumberOfRvaAndSizes;
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_PE_HEADERS {
  UINT32                   Signature;
  IMAGE_FILE_HEADER       FileHeader;
  IMAGE_OPTIONAL_HEADER   OptionalHeader;
} IMAGE_PE_HEADERS, *PIMAGE_PE_HEADERS;

typedef struct _IMAGE_SECTION_HEADER {
  CHAR8  Name[8];
  union {
    UINT32 PhysicalAddress;
    UINT32 VirtualSize;
  } Misc;
  UINT32 VirtualAddress;
  UINT32 SizeOfRawData;
  UINT32 PointerToRawData;
  UINT32 PointerToRelocations;
  UINT32 PointerToLinenumbers;
  UINT16  NumberOfRelocations;
  UINT16  NumberOfLinenumbers;
  UINT32 Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

EFI_STATUS KeBootLoadPe(EFI_SYSTEM_TABLE *SystemTable, VOID* Buffer, P_KE_PE_IMAGE Image);

EFI_STATUS KeBootCallPe(P_KE_PE_IMAGE Image, VOID* Parameter);
