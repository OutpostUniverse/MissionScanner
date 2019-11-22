#include "DllExportReader32.h"
#include <array>
#include <stdexcept>
#include <algorithm>

// http://www.delphibasics.info/home/delphibasicsarticles/anin-depthlookintothewin32portableexecutablefileformat-part1
// http://www.delphibasics.info/home/delphibasicsarticles/anin-depthlookintothewin32portableexecutablefileformat-part2
// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format


DllExportReader32::DllExportReader32(const std::string& filename) :
	stream(filename)
{	
	if (!IsPortableExecutableFile()) {
		throw std::runtime_error("A file or stream was passed that was not labeled as a Portable Exectuable.");
	}

	CoffHeader coffHeader;
	stream.Read(coffHeader);

	if (!IsDll(coffHeader)) { 
		throw std::runtime_error("A file or stream was passed that was not labeled as a DLL");
	}

	Image32Bit image32Bit;
	stream.Read(image32Bit);

	if (image32Bit.magic != 0x10b) {
		throw std::runtime_error("The dll or executable must be compiled using 32 bit architecture");
	}

	std::vector<ImageDataDirectory> imageDataDirectories(image32Bit.numberOfRvaAndSizes);
	stream.Read(imageDataDirectories);

	sectionTables.resize(coffHeader.numberOfSections);
	stream.Read(sectionTables);

	const ImageDataDirectory& exportTableEntry = imageDataDirectories[0];

	// No export table is available to pull from
	if (exportTableEntry.virtualAddress == 0) {
		return;
	}

	SectionTable sectionTable = FindSectionTableContainingRva(exportTableEntry.virtualAddress);

	stream.Seek(RvaToFileOffset(exportTableEntry.virtualAddress, sectionTable));
	ExportDirectoryTable exportDirectoryTable;
	stream.Read(exportDirectoryTable);

	LoadNameTable(exportDirectoryTable.namePointerRva, sectionTable, exportDirectoryTable.numberOfNamePointers);
	
	std::vector<std::uint16_t> exportOrdinalTable(exportNameTable.size());
	stream.Seek(RvaToFileOffset(exportDirectoryTable.ordinalTableRva, sectionTable));
	stream.Read(exportOrdinalTable);

	exportAddressTable.resize(exportNameTable.size());
	stream.Seek(RvaToFileOffset(exportDirectoryTable.exportAddressTableRva, sectionTable));
	stream.Read(exportAddressTable);
}

SectionTable DllExportReader32::FindSectionTableContainingRva(std::uint32_t rva)
{
	for (const SectionTable& sectionTable : sectionTables)
	{
		if (rva >= sectionTable.virtualAddress &&
			rva <= sectionTable.virtualAddress + sectionTable.virtualSize)
		{
			return sectionTable;
		}
	}

	throw std::runtime_error("Provided rva value of " + std::to_string(rva) + " could not be matched to a Section Table");
}

void DllExportReader32::LoadNameTable(std::uint32_t rva, const SectionTable& sectionTable, std::size_t count)
{
	stream.Seek(RvaToFileOffset(rva, sectionTable));
	std::vector<uint32_t> exportNamePointerTable(count);
	stream.Read(exportNamePointerTable);

	for (std::size_t i = 0; i < exportNamePointerTable.size(); ++i)
	{
		stream.Seek(RvaToFileOffset(exportNamePointerTable[i], sectionTable));
		exportNameTable.push_back(stream.ReadNullTerminatedString());
	}
}

// File Offset = RVA - Virtual Offset + Raw Offset.
std::uint32_t DllExportReader32::RvaToFileOffset(std::uint32_t rva, const SectionTable& sectionTable)
{
	return rva - sectionTable.virtualAddress + sectionTable.pointerToRawData;
}

bool DllExportReader32::IsPortableExecutableFile()
{
	stream.Seek(0x3c); // Seek to signature pointer

	std::uint32_t peSignatureOffset;
	stream.Read(peSignatureOffset);

	stream.Seek(peSignatureOffset);

	constexpr std::array<char, 4> peSignature{ 'P', 'E', '\0', '\0' };
	std::array<char, 4> signature;
	stream.Read(signature);

	return signature == peSignature;
}

bool DllExportReader32::IsDll(const CoffHeader& coffHeader)
{
	return (coffHeader.characteristics & 0x2000) == 0x2000; // IMAGE_FILE_DLL
}

std::string DllExportReader32::ReadExportString(const std::string& exportName)
{
	stream.Seek(GetExportedFileOffset(exportName));
	return stream.ReadNullTerminatedString();
}

std::size_t DllExportReader32::GetExportOrdinal(const std::string& exportName)
{
	for (std::size_t i = 0; i < exportNameTable.size(); ++i) {
		if (exportName == exportNameTable[i]) {
			return i;
		}
	}

	throw std::runtime_error("Requested exported variable name of " + exportName + " was not found in the DLL");
}

std::uint32_t DllExportReader32::GetExportedFileOffset(const std::string& exportName)
{
	const auto rva = exportAddressTable[GetExportOrdinal(exportName)];

	return RvaToFileOffset(rva, FindSectionTableContainingRva(rva));
}

bool DllExportReader32::DoesExportExist(const std::string& exportName)
{
	return std::find(exportNameTable.begin(), exportNameTable.end(), exportName) != exportNameTable.end();
}
