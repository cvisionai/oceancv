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
include CMakeFiles/descriptors.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/descriptors.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/descriptors.dir/flags.make

CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o: CMakeFiles/descriptors.dir/flags.make
CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o: ../examples/descriptors/descriptors.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tschoening/dev/repos/oceancv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o -c /Users/tschoening/dev/repos/oceancv/examples/descriptors/descriptors.cpp

CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tschoening/dev/repos/oceancv/examples/descriptors/descriptors.cpp > CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.i

CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tschoening/dev/repos/oceancv/examples/descriptors/descriptors.cpp -o CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.s

# Object files for target descriptors
descriptors_OBJECTS = \
"CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o"

# External object files for target descriptors
descriptors_EXTERNAL_OBJECTS =

descriptors: CMakeFiles/descriptors.dir/examples/descriptors/descriptors.cpp.o
descriptors: CMakeFiles/descriptors.dir/build.make
descriptors: liboceancv_util.dylib
descriptors: liboceancv_ml.dylib
descriptors: /usr/local/lib/libopencv_imgcodecs.3.4.0.dylib
descriptors: liboceancv_img.dylib
descriptors: /usr/local/lib/libopencv_imgproc.3.4.0.dylib
descriptors: /usr/local/lib/libopencv_core.3.4.0.dylib
descriptors: libMPEG7.dylib
descriptors: CMakeFiles/descriptors.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tschoening/dev/repos/oceancv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable descriptors"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/descriptors.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/descriptors.dir/build: descriptors

.PHONY : CMakeFiles/descriptors.dir/build

CMakeFiles/descriptors.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/descriptors.dir/cmake_clean.cmake
.PHONY : CMakeFiles/descriptors.dir/clean

CMakeFiles/descriptors.dir/depend:
	cd /Users/tschoening/dev/repos/oceancv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tschoening/dev/repos/oceancv /Users/tschoening/dev/repos/oceancv /Users/tschoening/dev/repos/oceancv/build /Users/tschoening/dev/repos/oceancv/build /Users/tschoening/dev/repos/oceancv/build/CMakeFiles/descriptors.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/descriptors.dir/depend
