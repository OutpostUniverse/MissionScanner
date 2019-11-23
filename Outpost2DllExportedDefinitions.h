#pragma once

// Pulled from Outpost2DLL project to remove formal compilation reference to the entire Outpost2DLL project


// Structure for important data exports needed for OP2 to recognize the level
struct AIModDesc
{
	// Important level details
	int missionType;			// Mission type (defined above) or mission number (positive values) for campaign games
	int numPlayers;				// Number of players (on a multipalyer map)
	int maxTechLevel;			// Maximum tech level (Set to 12 to enable all techs)
	int boolUnitMission;		// Set to 1 to disable most reports (suitable for unit-only missions)
	// Extra baggage that doesn't need to be set properly
	const char* mapName;
	const char* levelDesc;
	const char* techtreeName;
	int checksum;
};


// Mission types, and the corresponding DLL name prefix
// Note: For campaign games, use a positive level number, and a prefix of e (Eden) or p (Plymouth)
// Note: Multiplayer games include the max number of players (denoted by <x>).
//   Example: ml6_99.dll  (Multiplayer, Last One Standing, Max of 6 players)
enum MissionTypes
{
	Colony = -1, //0xFF	// c
	AutoDemo = -2, //0xFF	// a
	Tutorial = -3, //0xFD	// t

	MultiLandRush = -4, //0xFC	// mu<x>
	MultiSpaceRace = -5, //0xFB	// mf<x>
	MultiResourceRace = -6, //0xFA	// mr<x>
	MultiMidas = -7, //0xF9	// mm<x>
	MultiLastOneStanding = -8, //0xF8	// ml<x>
};
