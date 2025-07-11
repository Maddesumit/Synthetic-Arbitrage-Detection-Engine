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
include CMakeFiles/performance_benchmark.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/performance_benchmark.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/performance_benchmark.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/performance_benchmark.dir/flags.make

CMakeFiles/performance_benchmark.dir/codegen:
.PHONY : CMakeFiles/performance_benchmark.dir/codegen

CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o: CMakeFiles/performance_benchmark.dir/flags.make
CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o: /Users/sumitmadde/Desktop/Synthetic\ Pair\ Deviation\ Engine/src/benchmarks/performance_benchmark.cpp
CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o: CMakeFiles/performance_benchmark.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o -MF CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o.d -o CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o -c "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/benchmarks/performance_benchmark.cpp"

CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/benchmarks/performance_benchmark.cpp" > CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.i

CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/src/benchmarks/performance_benchmark.cpp" -o CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.s

# Object files for target performance_benchmark
performance_benchmark_OBJECTS = \
"CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o"

# External object files for target performance_benchmark
performance_benchmark_EXTERNAL_OBJECTS =

performance_benchmark: CMakeFiles/performance_benchmark.dir/src/benchmarks/performance_benchmark.cpp.o
performance_benchmark: CMakeFiles/performance_benchmark.dir/build.make
performance_benchmark: libarbitrage_performance.a
performance_benchmark: src/core/libarbitrage_core.a
performance_benchmark: src/data/libarbitrage_data.a
performance_benchmark: src/utils/libarbitrage_utils.a
performance_benchmark: /Applications/anaconda3/lib/libboost_system.dylib
performance_benchmark: /Applications/anaconda3/lib/libboost_filesystem.dylib
performance_benchmark: /Applications/anaconda3/lib/libboost_atomic.dylib
performance_benchmark: /Applications/anaconda3/lib/libboost_thread.dylib
performance_benchmark: /opt/homebrew/Cellar/openssl@3/3.5.0/lib/libssl.dylib
performance_benchmark: /opt/homebrew/Cellar/openssl@3/3.5.0/lib/libcrypto.dylib
performance_benchmark: CMakeFiles/performance_benchmark.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable performance_benchmark"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/performance_benchmark.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/performance_benchmark.dir/build: performance_benchmark
.PHONY : CMakeFiles/performance_benchmark.dir/build

CMakeFiles/performance_benchmark.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/performance_benchmark.dir/cmake_clean.cmake
.PHONY : CMakeFiles/performance_benchmark.dir/clean

CMakeFiles/performance_benchmark.dir/depend:
	cd "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/CMakeFiles/performance_benchmark.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/performance_benchmark.dir/depend

