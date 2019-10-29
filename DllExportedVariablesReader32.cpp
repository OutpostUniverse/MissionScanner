#include "DllExportedVariablesReader32.h"
#include <array>
#include <stdexcept>
#include <cstddef>

struct ImageDataDirectory;

#pragma pack(push, 1) // Make sure structure is byte aligned

struct ImageDataDirectory
{
	// Relative Virtual Address (RVA) of the table (address of the table relative to the base address of the image when the table is loaded). Differs from an item's position within the file on disk (file pointer)
	std::uint32_t virtualAddress;

	// Size in bytes
	std::uint32_t size;
};

static_assert(8 == sizeof(ImageDataDirectory), "ImageDataDirectory is an unexpected size");

#pragma pack(pop)

//DllExportedVariableReader32::DllExportedVariableReader32(const std::string& filename)
//{
//	Stream::FileReader reader(filename);
//	DllExportedVariableReader32(reader);
//}

DllExportedVariableReader32::DllExportedVariableReader32(const std::string& filename)
{
	stream = new Stream::FileReader(filename);
	
	if (!IsPortableExecutableFile(*stream)) {
		throw std::runtime_error("A file or stream was passed that was not labeled as a Portable Exectuable.");
	}

	if (!IsDll(*stream)) { 
		throw std::runtime_error("A file or stream was passed that was not labeled as a DLL");
	}

	stream->SeekForward(96); // Seek to Export Table entry

	ImageDataDirectory exportTable;
	stream->Read(exportTable);

	int test = exportTable.virtualAddress;

	std::vector<std::string> ExportNameTable;



	free(stream);
}

bool DllExportedVariableReader32::IsPortableExecutableFile(Stream::FileReader& stream)
{
	stream.SeekForward(0x3c);

	std::uint32_t peSignatureOffset;
	stream.Read(peSignatureOffset);

	stream.Seek(peSignatureOffset);

	constexpr std::array<char, 4> peSignature{ 'P', 'E', '\0', '\0' };
	std::array<char, 4> signature;
	stream.Read(signature);

	return signature == peSignature;
}

bool DllExportedVariableReader32::IsDll(Stream::FileReader& stream)
{
	stream.SeekForward(18);

	std::uint16_t characteristic;
	stream.Read(characteristic);
	return (characteristic & 0x2000) == 0x2000; // IMAGE_FILE_DLL
}

std::string DllExportedVariableReader32::GetString(const std::string& variableName)
{
	return std::string();
}

int DllExportedVariableReader32::GetInt(const std::string& variableName)
{
	return 0;
}

bool DllExportedVariableReader32::GetBool(const std::string& variableName)
{
	return true;
}
