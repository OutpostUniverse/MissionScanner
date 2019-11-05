# To see a list of available make targets from bash, type:
# make <space> <tab> <tab>


include makefile-generic.mk


# Set default compiler toolchain (can still override from command line)
config := clang


CPPFLAGS := -IOP2Utility/include/ -IOutpost2DLL/ -DOP2="" -D__fastcall="" -D__cdecl=""
CXXFLAGS := -std=c++17 -O2 -g -Wall -Wno-unknown-pragmas
LDFLAGS := -LOP2Utility/
LDLIBS := -lOP2Utility -lstdc++fs

.PHONY: all op2utility clean-op2utility clean-all-op2utility

all: missionScanner

op2utility:
	+make -C OP2Utility/ CXX="$(CXX)"

clean-op2utility:
	make -C OP2Utility/ CXX="$(CXX)" clean

clean-all-op2utility:
	make -C OP2Utility/ CXX="$(CXX)" clean-all

missionScanner.exe: | op2utility

clean: clean-op2utility
clean-all: clean-all-op2utility

$(eval $(call DefineCppProject,missionScanner,missionScanner.exe,./*.cpp))


# Docker and CircleCI commands
# $(eval $(call DefineDockerImage,.circleci/,outpostuniverse/gcc-mingw-wine-googletest-circleci,1.2))
# $(eval $(call DefineCircleCi))


ifdef Outpost2Path

.PHONY: run

run:
	./missionScanner.exe "$(Outpost2Path)"

endif
