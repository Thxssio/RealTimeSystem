# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/thxssio/RealTimeSystem/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/thxssio/RealTimeSystem/server/build

# Include any dependencies generated for this target.
include CMakeFiles/GPSReader.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/GPSReader.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/GPSReader.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/GPSReader.dir/flags.make

CMakeFiles/GPSReader.dir/src/main.c.o: CMakeFiles/GPSReader.dir/flags.make
CMakeFiles/GPSReader.dir/src/main.c.o: ../src/main.c
CMakeFiles/GPSReader.dir/src/main.c.o: CMakeFiles/GPSReader.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/thxssio/RealTimeSystem/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/GPSReader.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/GPSReader.dir/src/main.c.o -MF CMakeFiles/GPSReader.dir/src/main.c.o.d -o CMakeFiles/GPSReader.dir/src/main.c.o -c /home/thxssio/RealTimeSystem/server/src/main.c

CMakeFiles/GPSReader.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/GPSReader.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thxssio/RealTimeSystem/server/src/main.c > CMakeFiles/GPSReader.dir/src/main.c.i

CMakeFiles/GPSReader.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/GPSReader.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thxssio/RealTimeSystem/server/src/main.c -o CMakeFiles/GPSReader.dir/src/main.c.s

CMakeFiles/GPSReader.dir/src/gps/gps.c.o: CMakeFiles/GPSReader.dir/flags.make
CMakeFiles/GPSReader.dir/src/gps/gps.c.o: ../src/gps/gps.c
CMakeFiles/GPSReader.dir/src/gps/gps.c.o: CMakeFiles/GPSReader.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/thxssio/RealTimeSystem/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/GPSReader.dir/src/gps/gps.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/GPSReader.dir/src/gps/gps.c.o -MF CMakeFiles/GPSReader.dir/src/gps/gps.c.o.d -o CMakeFiles/GPSReader.dir/src/gps/gps.c.o -c /home/thxssio/RealTimeSystem/server/src/gps/gps.c

CMakeFiles/GPSReader.dir/src/gps/gps.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/GPSReader.dir/src/gps/gps.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thxssio/RealTimeSystem/server/src/gps/gps.c > CMakeFiles/GPSReader.dir/src/gps/gps.c.i

CMakeFiles/GPSReader.dir/src/gps/gps.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/GPSReader.dir/src/gps/gps.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thxssio/RealTimeSystem/server/src/gps/gps.c -o CMakeFiles/GPSReader.dir/src/gps/gps.c.s

CMakeFiles/GPSReader.dir/src/utils/utils.c.o: CMakeFiles/GPSReader.dir/flags.make
CMakeFiles/GPSReader.dir/src/utils/utils.c.o: ../src/utils/utils.c
CMakeFiles/GPSReader.dir/src/utils/utils.c.o: CMakeFiles/GPSReader.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/thxssio/RealTimeSystem/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/GPSReader.dir/src/utils/utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/GPSReader.dir/src/utils/utils.c.o -MF CMakeFiles/GPSReader.dir/src/utils/utils.c.o.d -o CMakeFiles/GPSReader.dir/src/utils/utils.c.o -c /home/thxssio/RealTimeSystem/server/src/utils/utils.c

CMakeFiles/GPSReader.dir/src/utils/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/GPSReader.dir/src/utils/utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thxssio/RealTimeSystem/server/src/utils/utils.c > CMakeFiles/GPSReader.dir/src/utils/utils.c.i

CMakeFiles/GPSReader.dir/src/utils/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/GPSReader.dir/src/utils/utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thxssio/RealTimeSystem/server/src/utils/utils.c -o CMakeFiles/GPSReader.dir/src/utils/utils.c.s

# Object files for target GPSReader
GPSReader_OBJECTS = \
"CMakeFiles/GPSReader.dir/src/main.c.o" \
"CMakeFiles/GPSReader.dir/src/gps/gps.c.o" \
"CMakeFiles/GPSReader.dir/src/utils/utils.c.o"

# External object files for target GPSReader
GPSReader_EXTERNAL_OBJECTS =

bin/GPSReader: CMakeFiles/GPSReader.dir/src/main.c.o
bin/GPSReader: CMakeFiles/GPSReader.dir/src/gps/gps.c.o
bin/GPSReader: CMakeFiles/GPSReader.dir/src/utils/utils.c.o
bin/GPSReader: CMakeFiles/GPSReader.dir/build.make
bin/GPSReader: CMakeFiles/GPSReader.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/thxssio/RealTimeSystem/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable bin/GPSReader"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/GPSReader.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/GPSReader.dir/build: bin/GPSReader
.PHONY : CMakeFiles/GPSReader.dir/build

CMakeFiles/GPSReader.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/GPSReader.dir/cmake_clean.cmake
.PHONY : CMakeFiles/GPSReader.dir/clean

CMakeFiles/GPSReader.dir/depend:
	cd /home/thxssio/RealTimeSystem/server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/thxssio/RealTimeSystem/server /home/thxssio/RealTimeSystem/server /home/thxssio/RealTimeSystem/server/build /home/thxssio/RealTimeSystem/server/build /home/thxssio/RealTimeSystem/server/build/CMakeFiles/GPSReader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/GPSReader.dir/depend

