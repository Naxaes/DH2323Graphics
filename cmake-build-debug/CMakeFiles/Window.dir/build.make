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
include CMakeFiles/Window.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Window.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Window.dir/flags.make

CMakeFiles/Window.dir/source/window.cpp.o: CMakeFiles/Window.dir/flags.make
CMakeFiles/Window.dir/source/window.cpp.o: ../source/window.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Window.dir/source/window.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Window.dir/source/window.cpp.o -c /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/source/window.cpp

CMakeFiles/Window.dir/source/window.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Window.dir/source/window.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/source/window.cpp > CMakeFiles/Window.dir/source/window.cpp.i

CMakeFiles/Window.dir/source/window.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Window.dir/source/window.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/source/window.cpp -o CMakeFiles/Window.dir/source/window.cpp.s

# Object files for target Window
Window_OBJECTS = \
"CMakeFiles/Window.dir/source/window.cpp.o"

# External object files for target Window
Window_EXTERNAL_OBJECTS =

Window: CMakeFiles/Window.dir/source/window.cpp.o
Window: CMakeFiles/Window.dir/build.make
Window: CMakeFiles/Window.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Window"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Window.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Window.dir/build: Window

.PHONY : CMakeFiles/Window.dir/build

CMakeFiles/Window.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Window.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Window.dir/clean

CMakeFiles/Window.dir/depend:
	cd /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-debug/CMakeFiles/Window.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Window.dir/depend

