# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dada/workspace/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dada/workspace/sylar

# Include any dependencies generated for this target.
include CMakeFiles/echoServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/echoServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/echoServer.dir/flags.make

CMakeFiles/echoServer.dir/example/echoServer.cc.o: CMakeFiles/echoServer.dir/flags.make
CMakeFiles/echoServer.dir/example/echoServer.cc.o: example/echoServer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dada/workspace/sylar/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/echoServer.dir/example/echoServer.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/echoServer.dir/example/echoServer.cc.o -c /home/dada/workspace/sylar/example/echoServer.cc

CMakeFiles/echoServer.dir/example/echoServer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/echoServer.dir/example/echoServer.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dada/workspace/sylar/example/echoServer.cc > CMakeFiles/echoServer.dir/example/echoServer.cc.i

CMakeFiles/echoServer.dir/example/echoServer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/echoServer.dir/example/echoServer.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dada/workspace/sylar/example/echoServer.cc -o CMakeFiles/echoServer.dir/example/echoServer.cc.s

# Object files for target echoServer
echoServer_OBJECTS = \
"CMakeFiles/echoServer.dir/example/echoServer.cc.o"

# External object files for target echoServer
echoServer_EXTERNAL_OBJECTS =

bin/echoServer: CMakeFiles/echoServer.dir/example/echoServer.cc.o
bin/echoServer: CMakeFiles/echoServer.dir/build.make
bin/echoServer: /bin/libsylar.so
bin/echoServer: CMakeFiles/echoServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dada/workspace/sylar/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/echoServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/echoServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/echoServer.dir/build: bin/echoServer

.PHONY : CMakeFiles/echoServer.dir/build

CMakeFiles/echoServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/echoServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/echoServer.dir/clean

CMakeFiles/echoServer.dir/depend:
	cd /home/dada/workspace/sylar && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dada/workspace/sylar /home/dada/workspace/sylar /home/dada/workspace/sylar /home/dada/workspace/sylar /home/dada/workspace/sylar/CMakeFiles/echoServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/echoServer.dir/depend

