# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /opt/CLion/CLion-2022.3.3/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /opt/CLion/CLion-2022.3.3/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/juandiego/CLionProjects/bplustree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/juandiego/CLionProjects/bplustree/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/bplus_tree.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/bplus_tree.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/bplus_tree.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bplus_tree.dir/flags.make

CMakeFiles/bplus_tree.dir/main.cpp.o: CMakeFiles/bplus_tree.dir/flags.make
CMakeFiles/bplus_tree.dir/main.cpp.o: /home/juandiego/CLionProjects/bplustree/main.cpp
CMakeFiles/bplus_tree.dir/main.cpp.o: CMakeFiles/bplus_tree.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/juandiego/CLionProjects/bplustree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bplus_tree.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bplus_tree.dir/main.cpp.o -MF CMakeFiles/bplus_tree.dir/main.cpp.o.d -o CMakeFiles/bplus_tree.dir/main.cpp.o -c /home/juandiego/CLionProjects/bplustree/main.cpp

CMakeFiles/bplus_tree.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bplus_tree.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/juandiego/CLionProjects/bplustree/main.cpp > CMakeFiles/bplus_tree.dir/main.cpp.i

CMakeFiles/bplus_tree.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bplus_tree.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/juandiego/CLionProjects/bplustree/main.cpp -o CMakeFiles/bplus_tree.dir/main.cpp.s

# Object files for target bplus_tree
bplus_tree_OBJECTS = \
"CMakeFiles/bplus_tree.dir/main.cpp.o"

# External object files for target bplus_tree
bplus_tree_EXTERNAL_OBJECTS =

bplus_tree: CMakeFiles/bplus_tree.dir/main.cpp.o
bplus_tree: CMakeFiles/bplus_tree.dir/build.make
bplus_tree: CMakeFiles/bplus_tree.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/juandiego/CLionProjects/bplustree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bplus_tree"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bplus_tree.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bplus_tree.dir/build: bplus_tree
.PHONY : CMakeFiles/bplus_tree.dir/build

CMakeFiles/bplus_tree.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bplus_tree.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bplus_tree.dir/clean

CMakeFiles/bplus_tree.dir/depend:
	cd /home/juandiego/CLionProjects/bplustree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/juandiego/CLionProjects/bplustree /home/juandiego/CLionProjects/bplustree /home/juandiego/CLionProjects/bplustree/cmake-build-debug /home/juandiego/CLionProjects/bplustree/cmake-build-debug /home/juandiego/CLionProjects/bplustree/cmake-build-debug/CMakeFiles/bplus_tree.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bplus_tree.dir/depend

