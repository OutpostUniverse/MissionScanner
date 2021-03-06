#pragma once

#include "PEDataStructures.h"
#include "OP2Utility.h"
#include <vector>
#include <string>
#include <cstddef>
#include <type_traits>


// Access exported variables from a 32 bit DLL without loading the DLL into memory.
class DllExportReader32
{
public:
	explicit DllExportReader32(const std::string& filename);

	std::string ReadExportString(const std::string& exportName);

	template <typename DataType>
	DataType ReadExport(const std::string& exportName)
	{
		static_assert(std::is_trivially_copyable_v<DataType>, "Type must be trivially copyable");

		stream.Seek(GetExportedFileOffset(exportName));

		DataType value;
		stream.Read(value);
		return value;
	}

	bool DoesExportExist(const std::string& exportName);

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
	std::size_t GetExportOrdinal(const std::string& exportName);
	std::uint32_t GetExportedFileOffset(const std::string& exportName);
};
