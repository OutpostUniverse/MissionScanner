#include "MissionTable.h"
#include "WindowsErrorCode.h"
#include "Outpost2DLL.h"
#include <windows.h>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <array>


void WriteHeader();
void WriteRow(HINSTANCE dllHandle);

void WriteCell(std::string_view message, std::streamsize cellWidthInChars);
void WriteCell(int integer, std::streamsize cellWidthInChars);
void WriteCell(MissionTypes missionType, std::streamsize cellWidthInChars);
void WriteBoolCell(bool boolean, std::streamsize cellWidthInChars);

HINSTANCE LoadMissionDll(const std::string& dllPath);
bool IsOutpost2MissionDll(HINSTANCE dllHandle);
std::string ConvertMissionTypeToString(MissionTypes missionType);

constexpr std::array<std::streamsize, 6> columnWidths{ 48, 22, 24, 18, 2, 5 };


void WriteTable(std::vector<std::string> missionPaths)
{
	WriteHeader();

	for (const auto& missionPath : missionPaths)
	{
		auto missionHandle = LoadMissionDll(missionPath);
		if (missionHandle != nullptr)
		{
			WriteRow(missionHandle);
			FreeModule(missionHandle);
		}
	}
}

void WriteHeader()
{
	WriteCell("MISSION DESCRIPTION", columnWidths[0]);
	WriteCell("MAP NAME", columnWidths[1]);
	WriteCell("TECH TREE NAME", columnWidths[2]);
	WriteCell("MISSION TYPE", columnWidths[3]);
	WriteCell("#", columnWidths[4]);
	WriteCell("UNIT", columnWidths[5]);
	std::cout << std::endl;
}

void WriteRow(HINSTANCE dllHandle)
{
	WriteCell(reinterpret_cast<char*>(GetProcAddress(dllHandle, "LevelDesc")), columnWidths[0]);
	WriteCell(reinterpret_cast<char*>(GetProcAddress(dllHandle, "MapName")), columnWidths[1]);
	WriteCell(reinterpret_cast<char*>(GetProcAddress(dllHandle, "TechtreeName")), columnWidths[2]);

	// Some missions do not store LevelDesc, MapName, and TechTreeName within AIModDesc
	AIModDesc* aiModDescPointer = (AIModDesc*)(GetProcAddress(dllHandle, "DescBlock"));
	WriteCell(static_cast<MissionTypes>(aiModDescPointer->missionType), columnWidths[3]);
	WriteCell(aiModDescPointer->numPlayers, columnWidths[4]);
	WriteBoolCell(static_cast<bool>(aiModDescPointer->boolUnitMission), columnWidths[5]);

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


HINSTANCE LoadMissionDll(const std::string& dllPath)
{
	auto dllHandle = LoadLibraryExA(dllPath.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);

	if (dllHandle == nullptr) {
		std::cerr << "Unable to load DLL at " << dllPath << ". " << GetLastErrorString() << std::endl;
		return nullptr;
	}

	if (!IsOutpost2MissionDll(dllHandle)) {
		FreeModule(dllHandle);
		return nullptr;
	}

	return dllHandle;
}

bool IsOutpost2MissionDll(HINSTANCE dllHandle)
{
	return GetProcAddress(dllHandle, "LevelDesc") != nullptr;
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
