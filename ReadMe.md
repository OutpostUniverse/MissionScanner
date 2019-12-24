# Outpost 2 Mission Scanner

Developed by Brett208 (Vagabond) and Hooman

A console application to review the publicly exported information contained in Outpost 2 mission DLLs. Written using C++17 features. Tested on Windows and Linux. Windows project files compile using MSVC with Visual Studio 2017. A Linux makefile is also included. MissionScanner fully supports x86 and x64 compilation. For more information on Outpost 2, visit https://outpost2.net.

## Usage

MissionScanner (archivename.(vol|clm) | directory)... [-L]

#### Optional Arguments
 * -H / --Help / -?: Displays help information
 * -L / --Legend: Remove legend

#### Example Commands

MissionScanner C:/Outpost2
MissionScanner e01.dll e02.dll --Legend
MissionScanner Outpost2/ -L
