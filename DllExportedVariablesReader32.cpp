#include "DllExportedVariablesReader32.h"
#include <array>
#include <stdexcept>
#include <algorithm>

// http://www.delphibasics.info/home/delphibasicsarticles/anin-depthlookintothewin32portableexecutablefileformat-part1
// http://www.delphibasics.info/home/delphibasicsarticles/anin-depthlookintothewin32portableexecutablefileformat-part2
// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format


DllExportedVariableReader32::DllExportedVariableReader32(const std::string& filename) :
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

	ImageDataDirectory& exportTableEntry = imageDataDirectories[0];

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

SectionTable DllExportedVariableReader32::FindSectionTableContainingRva(std::uint32_t rva)
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

void DllExportedVariableReader32::LoadNameTable(std::uint32_t rva, const SectionTable& sectionTable, std::size_t count)
{
	stream.Seek(RvaToFileOffset(rva, sectionTable));
	std::vector<uint32_t> exportNamePointerTable(count);
	stream.Read(exportNamePointerTable);

	for (std::size_t i = 0; i < exportNamePointerTable.size(); ++i)
	{
		stream.Seek(RvaToFileOffset(exportNamePointerTable[i], sectionTable));
		exportNameTable.push_back(ReadNullTerminatedString());
	}
}

// Reads a string until a null terminator is encountered or maxCount is reached
std::string DllExportedVariableReader32::ReadNullTerminatedString(std::size_t maxCount)
{
	std::string str;

	char c;
	for (std::size_t i = 0; i < maxCount; ++i)
	{ 
		stream.Read(c);
		if (c == '\0') {
			break;
		}

		str.push_back(c);
	}

	return str;
}

// File Offset = RVA - Virtual Offset + Raw Offset.
std::uint32_t DllExportedVariableReader32::RvaToFileOffset(std::uint32_t rva, const SectionTable& sectionTable)
{
	return rva - sectionTable.virtualAddress + sectionTable.pointerToRawData;
}

bool DllExportedVariableReader32::IsPortableExecutableFile()
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

bool DllExportedVariableReader32::IsDll(const CoffHeader& coffHeader)
{
	return (coffHeader.characteristics & 0x2000) == 0x2000; // IMAGE_FILE_DLL
}

std::string DllExportedVariableReader32::GetString(const std::string& variableName)
{
	stream.Seek(GetExportedFileOffset(variableName));
	return ReadNullTerminatedString();
}

std::size_t DllExportedVariableReader32::GetExportOrdinal(const std::string& variableName)
{
	for (std::size_t i = 0; i < exportNameTable.size(); ++i) {
		if (variableName == exportNameTable[i]) {
			return i;
		}
	}

	throw std::runtime_error("Requested exported variable name of " + variableName + " was not found in the DLL");
}

std::uint32_t DllExportedVariableReader32::GetExportedFileOffset(const std::string& variableName)
{
	const auto rva = exportAddressTable[GetExportOrdinal(variableName)];

	return RvaToFileOffset(rva, FindSectionTableContainingRva(rva));
}

bool DllExportedVariableReader32::DoesExportExist(const std::string& variableName)
{
	return std::find(exportNameTable.begin(), exportNameTable.end(), variableName) != exportNameTable.end();
}
