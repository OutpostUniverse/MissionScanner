#include "MissionTable.h"
#include "DllExportedVariablesReader32.h"
#include "Outpost2DLL.h"
#include <iomanip>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <array>
#include <filesystem>
namespace fs = std::filesystem;

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
			WriteRow(dllExportedVariables, fs::path(missionPath).filename().replace_extension().string());
		}
		catch (const std::exception & e) {

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
	WriteCell(filename, columnWidths[0]);
	WriteCell(dllReader.GetString("LevelDesc"), columnWidths[1]);
	WriteCell(dllReader.GetString("MapName"), columnWidths[2]);
	WriteCell(dllReader.GetString("TechTreeName"), columnWidths[3]);

	WriteCell(dllReader.GetInt("LevelDesc"), columnWidths[1]);

	//TODO: Figure out how to open a struct from a dll on file
	// Some missions do not store LevelDesc, MapName, and TechTreeName within AIModDesc
	//AIModDesc* aiModDescPointer = (AIModDesc*)(GetProcAddress(dllHandle, "DescBlock"));
	//WriteCell(static_cast<MissionTypes>(aiModDescPointer->missionType), columnWidths[4]);
	//WriteCell(aiModDescPointer->numPlayers, columnWidths[5]);
	//WriteBoolCell(static_cast<bool>(aiModDescPointer->boolUnitMission), columnWidths[6]);

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
