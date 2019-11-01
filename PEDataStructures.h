#pragma once

#include <cstdint>

// Represent Portable Executable (PE) headers
// See WINNT.H for formal definitions of structures below 


#pragma pack(push, 1) // Make sure structure is byte aligned

struct ExportDirectoryTable
{
	std::uint32_t exportFlags;
	std::uint32_t timeDateStamp;
	std::uint16_t majorVersion;
	std::uint16_t minorVersion;
	std::uint32_t nameRva;
	std::uint32_t ordinalBase;
	std::uint32_t addressTableEntries;
	std::uint32_t numberOfNamePointers;
	std::uint32_t exportAddressTableRva;
	std::uint32_t namePointerRva;
	std::uint32_t ordinalTableRva;
};

static_assert(40 == sizeof(ExportDirectoryTable), "Export DirectoryTable is an unexpected size");


struct CoffHeader
{
	std::uint16_t machine;
	std::uint16_t numberOfSections;
	std::uint32_t timeDateStamp;
	std::uint32_t pointerToSymbolTable;
	std::uint32_t numberOfSymbols;
	std::uint16_t sizeOfOptionalHeader;
	std::uint16_t characteristics;
};

static_assert(20 == sizeof(CoffHeader), "CoffHeader is an unexpected size");


struct Image32Bit
{
	std::uint16_t magic;
	std::uint8_t majorLinkerVersion;
	std::uint8_t minorLinkerVersion;
	std::uint32_t sizeOfCode;
	std::uint32_t sizeOfInitializedData;
	std::uint32_t sizeOfUninitializedData;
	std::uint32_t addressOfEntryPoint;
	std::uint32_t baseOfCode;
	std::uint32_t baseofData;
	std::uint32_t imageBase;
	std::uint32_t sectionAlignment;
	std::uint32_t fileAlignment;
	std::uint16_t majorOperatingSystemVersion;
	std::uint16_t minorOperatingSystemVersion;
	std::uint16_t majorImageVersion;
	std::uint16_t minorImageVersion;
	std::uint16_t majorSubsystemVersion;
	std::uint16_t minorSubsystemVersion;
	std::uint32_t win32VersionValue;
	std::uint32_t sizeOfImage;
	std::uint32_t sizeOfHeaders;
	std::uint32_t checkSum;
	std::uint16_t subsystem;
	std::uint16_t dllCharacteristics;
	std::uint32_t sizeOfStackReserve;
	std::uint32_t sizeOfStackCommit;
	std::uint32_t sizeOfHeapReserve;
	std::uint32_t sizeOfHeapCommit;
	std::uint32_t loaderFlags;
	std::uint32_t numberOfRvaAndSizes;
};

static_assert(96 == sizeof(Image32Bit), "Image32Bit is an unexpected size");


struct SectionTable
{
	char name[8];
	std::uint32_t virtualSize;
	std::uint32_t virtualAddress;
	std::uint32_t sizeOfRawData;
	std::uint32_t pointerToRawData;
	std::uint32_t pointerToRelocations;
	std::uint32_t pointerToLineNumbers;
	std::uint16_t numberOfReolcations;
	std::uint16_t numberOfLineNumbers;
	std::uint32_t characteristics;
};

static_assert(40 == sizeof(SectionTable), "SectionTable is an unexpected size");


struct ImageDataDirectory
{
	// Relative Virtual Address (RVA) of the table
	std::uint32_t virtualAddress;

	// Size in bytes
	std::uint32_t size;
};

static_assert(8 == sizeof(ImageDataDirectory), "ImageDataDirectory is an unexpected size");

#pragma pack(pop)
