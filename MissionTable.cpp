#include "MissionTable.h"
#include "DllExportReader32.h"
#include "Outpost2DLL.h"
#include <iomanip>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <cstddef>
#include <array>
#include <algorithm>

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


void WriteHeader();
void WriteRow(DllExportReader32& dllReader, std::string_view filename);

void WriteCell(std::string_view message, std::streamsize cellWidthInChars);
void WriteCell(int integer, std::streamsize cellWidthInChars);
void WriteCell(MissionTypes missionType, std::streamsize cellWidthInChars);
void WriteBoolCell(bool boolean, std::streamsize cellWidthInChars);

std::string_view ConvertMissionTypeToString(MissionTypes missionType);

struct LegendEntry
{
	std::string_view key;
	std::string_view description;
};

constexpr std::array<LegendEntry, 7> columnTitles{
	LegendEntry {"DLL NAME", "Name of mission's dll file"},
	LegendEntry {"TYP", "Mission type"},
	LegendEntry {"#", "Maximum supported count of combined human and AI players"},
	LegendEntry {"U", "Is mission unit only"},
	LegendEntry {"MAP NAME", "Map filename used by the mission"},
	LegendEntry {"TECH TREE NAME", "Name of the tech tree used by the mission"},
	LegendEntry {"MISSION DESCRIPTION", "Description of the mission displayed in Outpost 2"}
};

constexpr std::array<std::streamsize, 7> columnWidths{ 9, 4, 2, 2, 18, 24, 1 };

constexpr std::array<LegendEntry, 9> missionTypes{
	// Campaign (positive missionType values)
	LegendEntry {"Cam", "Campaign"},
	// Single player
	LegendEntry {"Col", "Colony"},
	LegendEntry {"Dem", "Demo"},
	LegendEntry {"Tut", "Tutorial"},
	// Multiplayer
	LegendEntry {"MLR", "Multiplayer, Land Rush"},
	LegendEntry {"MSP", "Multiplayer, Space Race"},
	LegendEntry {"MRR", "Multiplayer, Resource Race"},
	LegendEntry {"MM", "Multiplayer, Midas"},
	LegendEntry {"ML", "Multiplayer, Last One Standing"}
};

template<std::size_t size>
std::size_t FindMaxKeyWidth(const std::array<LegendEntry, size>& container) 
{
	return std::max_element(container.begin(), container.end(),
		[](const LegendEntry& a, const LegendEntry& b)
		{
			return a.key.size() < b.key.size();
		})->key.size();
}

template<std::size_t size>
void WriteLegendPortion(std::string_view title, const std::array<LegendEntry, size>& container)
{
	std::cout << title << std::endl;

	auto cellWidth = FindMaxKeyWidth(container);
	for (const auto& entry : container) {
		std::cout << std::setw(cellWidth) << std::left << entry.key << " = " << entry.description << std::endl;
	}

	std::cout << std::endl;
}

void WriteLegend()
{
	WriteLegendPortion("MISSION TABLE LEGEND", columnTitles);
	WriteLegendPortion("MISSION TYPE LEGEND", missionTypes);
}

void WriteTable(std::vector<std::string> missionPaths)
{
	WriteHeader();

	std::sort(missionPaths.begin(), missionPaths.end());
	for (const auto& missionPath : missionPaths)
	{
		try {
			DllExportReader32 dllExportedVariables(missionPath);

			if (!dllExportedVariables.DoesExportExist("LevelDesc")) {
				continue;
			}

			WriteRow(dllExportedVariables, fs::path(missionPath).filename().replace_extension().string());
		}
		catch (const std::exception & e) {
			std::cerr << "Error attempting to open " << missionPath << " as a dll. " << e.what();
		}
	}
}

void WriteHeader()
{
	for (std::size_t i = 0; i < columnTitles.size(); ++i) {
		WriteCell(columnTitles[i].key, columnWidths[i]);
	}
	std::cout << std::endl;
}

void WriteRow(DllExportReader32& dllReader, std::string_view filename)
{
	try 
	{
		WriteCell(filename, columnWidths[0]);

		auto aiModDesc = dllReader.ReadExport<AIModDesc>("DescBlock");
		WriteCell(static_cast<MissionTypes>(aiModDesc.missionType), columnWidths[1]);
		WriteCell(aiModDesc.numPlayers, columnWidths[2]);
		WriteBoolCell(static_cast<bool>(aiModDesc.boolUnitMission), columnWidths[3]);

		// Some missions do not store LevelDesc, MapName, and TechTreeName within AIModDesc
		WriteCell(dllReader.ReadExportString("MapName"), columnWidths[4]);
		WriteCell(dllReader.ReadExportString("TechtreeName"), columnWidths[5]);
		WriteCell(dllReader.ReadExportString("LevelDesc"), columnWidths[6]);
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error attempting to write mission details for " << filename << ". " << e.what();
	}

	std::cout << std::endl;
}



void WriteCell(std::string_view message, std::streamsize cellWidthInChars)
{
	std::cout << std::setw(cellWidthInChars) << std::left << message;
}

void WriteCell(int integer, std::streamsize cellWidthInChars)
{
	WriteCell(std::to_string(integer), cellWidthInChars);
}

void WriteCell(MissionTypes missionType, std::streamsize cellWidthInChars)
{
	WriteCell(ConvertMissionTypeToString(missionType), cellWidthInChars);
}

void WriteBoolCell(bool boolean, std::streamsize cellWidthInChars)
{
	std::string booleanString("T");
	if (!boolean) {
		booleanString = "F";
	}

	WriteCell(booleanString, cellWidthInChars);
}



std::string_view ConvertMissionTypeToString(MissionTypes missionType)
{
	// Positive missionTypes represent campaign mission index.
	if (static_cast<int>(missionType) > 0) {
		return missionTypes[0].key;
	}
	// Negative values represent non-campaign game types (range -1..-8)
	auto missionTypeIndex = static_cast<std::size_t>(-missionType);
	if (missionTypeIndex < missionTypes.size()) {
		return missionTypes[missionTypeIndex].key;
	}

	throw std::runtime_error("An improper MissionType enum value of " + std::to_string(missionType) + " was provided.");
}
