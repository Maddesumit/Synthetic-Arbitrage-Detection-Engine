# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build"

# Include any dependencies generated for this target.
include CMakeFiles/arbitrage_monitoring.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/arbitrage_monitoring.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/arbitrage_monitoring.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/arbitrage_monitoring.dir/flags.make

CMakeFiles/arbitrage_monitoring.dir/codegen:
.PHONY : CMakeFiles/arbitrage_monitoring.dir/codegen

CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o: CMakeFiles/arbitrage_monitoring.dir/flags.make
CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o: /Users/sumitmadde/Desktop/Synthetic\ Pair\ Deviation\ Engine/src/monitoring/SystemMonitoring.cpp
CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o: CMakeFiles/arbitrage_monitoring.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o -MF CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o.d -o CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o -c "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/monitoring/SystemMonitoring.cpp"

CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/monitoring/SystemMonitoring.cpp" > CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.i

CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/monitoring/SystemMonitoring.cpp" -o CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.s

# Object files for target arbitrage_monitoring
arbitrage_monitoring_OBJECTS = \
"CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o"

# External object files for target arbitrage_monitoring
arbitrage_monitoring_EXTERNAL_OBJECTS =

libarbitrage_monitoring.a: CMakeFiles/arbitrage_monitoring.dir/src/monitoring/SystemMonitoring.cpp.o
libarbitrage_monitoring.a: CMakeFiles/arbitrage_monitoring.dir/build.make
libarbitrage_monitoring.a: CMakeFiles/arbitrage_monitoring.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libarbitrage_monitoring.a"
	$(CMAKE_COMMAND) -P CMakeFiles/arbitrage_monitoring.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arbitrage_monitoring.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/arbitrage_monitoring.dir/build: libarbitrage_monitoring.a
.PHONY : CMakeFiles/arbitrage_monitoring.dir/build

CMakeFiles/arbitrage_monitoring.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/arbitrage_monitoring.dir/cmake_clean.cmake
.PHONY : CMakeFiles/arbitrage_monitoring.dir/clean

CMakeFiles/arbitrage_monitoring.dir/depend:
	cd "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles/arbitrage_monitoring.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/arbitrage_monitoring.dir/depend

