#pragma once

#include "PEDataStructures.h"
#include "OP2Utility.h"
#include <vector>
#include <string>
#include <cstddef>
#include <type_traits>


// Access exported variables from a 32 bit DLL without loading the DLL into memory.
class DllExportedVariableReader32
{
public:
	DllExportedVariableReader32(const std::string& filename);

	std::string GetString(const std::string& variableName);
	int GetInt(const std::string& variableName);
	bool GetBool(const std::string& variableName);

	template <typename StructType>
	StructType GetStruct(const std::string& exportName)
	{
		static_assert(std::is_trivially_copyable_v<StructType>, "Type must be trivially copyable");

		stream.Seek(GetExportedFileOffset(exportName));

		StructType structValue;
		stream.Read(structValue);
		return structValue;
	}

	bool DoesExportExist(const std::string& variableName);

private:
	Stream::FileReader stream;

	std::vector<SectionTable> sectionTables;
	std::vector<std::string> exportNameTable;
	std::vector<std::uint32_t> exportAddressTable;

	bool IsPortableExecutableFile();
	bool IsDll(const CoffHeader& coffHeader);
	SectionTable FindSectionTableContainingRva(std::uint32_t rva);
	void LoadNameTable(std::uint32_t rva, const SectionTable& sectionTable, std::size_t count);
	std::uint32_t RvaToFileOffset(std::uint32_t rva, const SectionTable& sectionTable);
	std::string ReadNullTerminatedString(std::size_t maxCount = SIZE_MAX);
	std::size_t GetExportOrdinal(const std::string& variableName);
	std::uint32_t GetExportedFileOffset(const std::string& variableName);
};
