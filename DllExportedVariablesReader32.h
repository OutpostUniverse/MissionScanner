#pragma once

#include "OP2Utility/include/OP2Utility.h"
#include <string>


// Accesses exported variables from a DLL without loading the DLL into memory.
class DllExportedVariableReader32
{
public:
	DllExportedVariableReader32(const std::string& filename);

	std::string GetString(const std::string& variableName);
	int GetInt(const std::string& variableName);
	bool GetBool(const std::string& variableName);

private:
	Stream::FileReader* stream;

	bool IsPortableExecutableFile(Stream::FileReader& stream);
	bool IsDll(Stream::FileReader& stream);
};
