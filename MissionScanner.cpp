#include "MissionTable.h"
#include "OP2Utility.h"
#include <iostream>
#include <stdexcept>


const std::string version("1.0.0");

void OutputHelp();
std::vector<std::string> FindMissionPaths(const std::vector<std::string>& arguments);


std::vector<std::string> GatherArguments(int argc, char** argv) 
{
	// start at index 1 to skip exectuable's name  
	std::vector<std::string> arguments;
	
	for (int i = 1; i < argc; ++i) { 
		arguments.push_back(argv[i]);
	}

	return arguments;
}

// Does not recursively search subdirectories
int main(int argc, char** argv)
{
	try {
		auto arguments = GatherArguments(argc, argv);

		// Help Switch
		if (arguments.size() == 0 || 
			FindAndRemoveSwitch(arguments, { "-H", "-?", "--Help" })) 
		{
			OutputHelp();
			return 0;
		}
		const auto missionPaths = FindMissionPaths(argc, argv);
		const auto missionPaths = FindMissionPaths(arguments);
		if (missionPaths.size() > 0) {
			WriteTable(missionPaths);
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Run without arguments to see usage message." << std::endl << std::endl;

		return 1;
	}

	return 0;
}

std::vector<std::string> FindMissionPaths(const std::vector<std::string>& arguments)
{
	std::vector<std::string> missionPaths;

	for (const auto& argument : arguments)
	{
		if (XFile::IsDirectory(argument)) {
			std::vector<std::string> directoryFilenames = XFile::GetFilenamesFromDirectory(argument, ".dll");

			for (auto& filename : directoryFilenames) {
				filename = XFile::Append(argument, filename);
			}

			missionPaths.insert(missionPaths.end(), directoryFilenames.begin(), directoryFilenames.end());
		}
		else if (XFile::IsFile(argument)) {
			missionPaths.push_back(argument);
		}
		else
		{
			throw std::runtime_error("The following path was provided, but does not appear as a valid file path or directory.");
		}
	}

	return missionPaths;
}

void OutputHelp()
{
	std::cout << std::endl;
	std::cout << "Mission Scanner, version " << version << std::endl;
	std::cout << "Developed by Brett208 (Vagabond)" << std::endl;
	std::cout << std::endl;
	std::cout << "Review the publically exported infromation contained in Outpost 2 mission DLLs" << std::endl;
	std::cout << std::endl;
	std::cout << "+++ COMMANDS +++" << std::endl;
	std::cout << "  * MissionScanner (archivename.(vol|clm) | directory)..." << std::endl;
	std::cout << std::endl;
	std::cout << "+++ OPTIONAL ARGUMENTS +++" << std::endl;
	std::cout << "  -H / --Help / -?: Displays help information." << std::endl;
	std::cout << "For more information about Outpost 2, visit the Outpost Universe website at http://outpost2.net." << std::endl;
	std::cout << std::endl;
}
