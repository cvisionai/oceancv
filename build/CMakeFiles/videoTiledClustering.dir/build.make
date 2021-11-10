# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.19.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.19.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tschoening/dev/repos/oceancv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tschoening/dev/repos/oceancv/build

# Include any dependencies generated for this target.
include CMakeFiles/videoTiledClustering.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/videoTiledClustering.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/videoTiledClustering.dir/flags.make

CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o: CMakeFiles/videoTiledClustering.dir/flags.make
CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o: ../bin/uwi/videoTiledClustering.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tschoening/dev/repos/oceancv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o -c /Users/tschoening/dev/repos/oceancv/bin/uwi/videoTiledClustering.cpp

CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tschoening/dev/repos/oceancv/bin/uwi/videoTiledClustering.cpp > CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.i

CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tschoening/dev/repos/oceancv/bin/uwi/videoTiledClustering.cpp -o CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.s

# Object files for target videoTiledClustering
videoTiledClustering_OBJECTS = \
"CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o"

# External object files for target videoTiledClustering
videoTiledClustering_EXTERNAL_OBJECTS =

videoTiledClustering: CMakeFiles/videoTiledClustering.dir/bin/uwi/videoTiledClustering.cpp.o
videoTiledClustering: CMakeFiles/videoTiledClustering.dir/build.make
videoTiledClustering: /usr/local/lib/libopencv_videoio.3.4.0.dylib
videoTiledClustering: libMPEG7.dylib
videoTiledClustering: liboceancv_util.dylib
videoTiledClustering: liboceancv_img.dylib
videoTiledClustering: liboceancv_cudaimg.dylib
videoTiledClustering: /usr/local/lib/libopencv_imgcodecs.3.4.0.dylib
videoTiledClustering: /usr/local/lib/libopencv_imgproc.3.4.0.dylib
videoTiledClustering: /usr/local/lib/libopencv_core.3.4.0.dylib
videoTiledClustering: CMakeFiles/videoTiledClustering.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tschoening/dev/repos/oceancv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable videoTiledClustering"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/videoTiledClustering.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/videoTiledClustering.dir/build: videoTiledClustering

.PHONY : CMakeFiles/videoTiledClustering.dir/build

CMakeFiles/videoTiledClustering.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/videoTiledClustering.dir/cmake_clean.cmake
.PHONY : CMakeFiles/videoTiledClustering.dir/clean

CMakeFiles/videoTiledClustering.dir/depend:
	cd /Users/tschoening/dev/repos/oceancv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tschoening/dev/repos/oceancv /Users/tschoening/dev/repos/oceancv /Users/tschoening/dev/repos/oceancv/build /Users/tschoening/dev/repos/oceancv/build /Users/tschoening/dev/repos/oceancv/build/CMakeFiles/videoTiledClustering.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/videoTiledClustering.dir/depend

