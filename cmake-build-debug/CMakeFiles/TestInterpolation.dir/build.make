# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/TestInterpolation.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TestInterpolation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TestInterpolation.dir/flags.make

CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o: CMakeFiles/TestInterpolation.dir/flags.make
CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o: ../tests/interpolation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o -c /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/tests/interpolation.cpp

CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/tests/interpolation.cpp > CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.i

CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/tests/interpolation.cpp -o CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.s

# Object files for target TestInterpolation
TestInterpolation_OBJECTS = \
"CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o"

# External object files for target TestInterpolation
TestInterpolation_EXTERNAL_OBJECTS =

TestInterpolation: CMakeFiles/TestInterpolation.dir/tests/interpolation.cpp.o
TestInterpolation: CMakeFiles/TestInterpolation.dir/build.make
TestInterpolation: CMakeFiles/TestInterpolation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TestInterpolation"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TestInterpolation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TestInterpolation.dir/build: TestInterpolation

.PHONY : CMakeFiles/TestInterpolation.dir/build

CMakeFiles/TestInterpolation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TestInterpolation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TestInterpolation.dir/clean

CMakeFiles/TestInterpolation.dir/depend:
	cd /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles/TestInterpolation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TestInterpolation.dir/depend
