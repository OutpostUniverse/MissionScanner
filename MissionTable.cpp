#include "MissionTable.h"
#include "DllExportedVariablesReader32.h"
#include "Outpost2DLL.h"
#include <iomanip>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <array>

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


void WriteHeader();
void WriteRow(DllExportedVariableReader32& dllReader, std::string_view filename);

void WriteCell(std::string_view message, std::streamsize cellWidthInChars);
void WriteCell(int integer, std::streamsize cellWidthInChars);
void WriteCell(MissionTypes missionType, std::streamsize cellWidthInChars);
void WriteBoolCell(bool boolean, std::streamsize cellWidthInChars);

std::string ConvertMissionTypeToString(MissionTypes missionType);

constexpr std::array<std::streamsize, 7> columnWidths{ 9, 48, 22, 24, 18, 2, 5 };


void WriteTable(std::vector<std::string> missionPaths)
{
	WriteHeader();

	for (const auto& missionPath : missionPaths)
	{
		try {
			DllExportedVariableReader32 dllExportedVariables(missionPath);

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
	WriteCell("DLL NAME", columnWidths[0]);
	WriteCell("MISSION DESCRIPTION", columnWidths[1]);
	WriteCell("MAP NAME", columnWidths[2]);
	WriteCell("TECH TREE NAME", columnWidths[3]);
	WriteCell("MISSION TYPE", columnWidths[4]);
	WriteCell("#", columnWidths[5]);
	WriteCell("UNIT", columnWidths[6]);
	std::cout << std::endl;
}

void WriteRow(DllExportedVariableReader32& dllReader, std::string_view filename)
{
	try 
	{
		WriteCell(filename, columnWidths[0]);

		WriteCell(dllReader.ReadExportString("LevelDesc"), columnWidths[1]);
		WriteCell(dllReader.ReadExportString("MapName"), columnWidths[2]);
		WriteCell(dllReader.ReadExportString("TechtreeName"), columnWidths[3]);

		// Some missions do not store LevelDesc, MapName, and TechTreeName within AIModDesc
		auto aiModDesc = dllReader.ReadExport<AIModDesc>("DescBlock");
		WriteCell(static_cast<MissionTypes>(aiModDesc.missionType), columnWidths[4]);
		WriteCell(aiModDesc.numPlayers, columnWidths[5]);
		WriteBoolCell(static_cast<bool>(aiModDesc.boolUnitMission), columnWidths[6]);
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
	std::string booleanString("True");
	if (!boolean) {
		booleanString = "False";
	}

	WriteCell(booleanString, cellWidthInChars);
}



std::string ConvertMissionTypeToString(MissionTypes missionType)
{
	switch (missionType)
	{
	case Colony:
		return "Colony Game";
	case AutoDemo:
		return "Demo";
	case Tutorial:
		return "Tutorial";
	case MultiLandRush:
		return "Land Rush";
	case MultiSpaceRace:
		return "Space Race";
	case MultiResourceRace:
		return "Resource Race";
	case MultiMidas:
		return "Midas";
	case MultiLastOneStanding:
		return "Last One Standing";
	default:
		// Positive missionTypes represent campaign mission index.
		if (static_cast<int>(missionType) > 0) {
			return "Campaign";
		}

		throw std::runtime_error("An improper MissionType enum value of " + std::to_string(missionType) + " was provided.");
	}
}
