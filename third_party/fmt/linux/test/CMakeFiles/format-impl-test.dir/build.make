# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/comau/Git/fmt

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/comau/Git/fmt/build/release

# Include any dependencies generated for this target.
include test/CMakeFiles/format-impl-test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/format-impl-test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/format-impl-test.dir/flags.make

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.o: ../../test/format-impl-test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/format-impl-test.cc.o -c /home/comau/Git/fmt/test/format-impl-test.cc

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/format-impl-test.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/test/format-impl-test.cc > CMakeFiles/format-impl-test.dir/format-impl-test.cc.i

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/format-impl-test.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/test/format-impl-test.cc -o CMakeFiles/format-impl-test.dir/format-impl-test.cc.s

test/CMakeFiles/format-impl-test.dir/header-only-test.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/header-only-test.cc.o: ../../test/header-only-test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object test/CMakeFiles/format-impl-test.dir/header-only-test.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/header-only-test.cc.o -c /home/comau/Git/fmt/test/header-only-test.cc

test/CMakeFiles/format-impl-test.dir/header-only-test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/header-only-test.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/test/header-only-test.cc > CMakeFiles/format-impl-test.dir/header-only-test.cc.i

test/CMakeFiles/format-impl-test.dir/header-only-test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/header-only-test.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/test/header-only-test.cc -o CMakeFiles/format-impl-test.dir/header-only-test.cc.s

test/CMakeFiles/format-impl-test.dir/test-main.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/test-main.cc.o: ../../test/test-main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object test/CMakeFiles/format-impl-test.dir/test-main.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/test-main.cc.o -c /home/comau/Git/fmt/test/test-main.cc

test/CMakeFiles/format-impl-test.dir/test-main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/test-main.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/test/test-main.cc > CMakeFiles/format-impl-test.dir/test-main.cc.i

test/CMakeFiles/format-impl-test.dir/test-main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/test-main.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/test/test-main.cc -o CMakeFiles/format-impl-test.dir/test-main.cc.s

test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.o: ../../test/gtest-extra.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/gtest-extra.cc.o -c /home/comau/Git/fmt/test/gtest-extra.cc

test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/gtest-extra.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/test/gtest-extra.cc > CMakeFiles/format-impl-test.dir/gtest-extra.cc.i

test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/gtest-extra.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/test/gtest-extra.cc -o CMakeFiles/format-impl-test.dir/gtest-extra.cc.s

test/CMakeFiles/format-impl-test.dir/util.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/util.cc.o: ../../test/util.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object test/CMakeFiles/format-impl-test.dir/util.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/util.cc.o -c /home/comau/Git/fmt/test/util.cc

test/CMakeFiles/format-impl-test.dir/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/util.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/test/util.cc > CMakeFiles/format-impl-test.dir/util.cc.i

test/CMakeFiles/format-impl-test.dir/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/util.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/test/util.cc -o CMakeFiles/format-impl-test.dir/util.cc.s

test/CMakeFiles/format-impl-test.dir/__/src/os.cc.o: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/__/src/os.cc.o: ../../src/os.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object test/CMakeFiles/format-impl-test.dir/__/src/os.cc.o"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/__/src/os.cc.o -c /home/comau/Git/fmt/src/os.cc

test/CMakeFiles/format-impl-test.dir/__/src/os.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/__/src/os.cc.i"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comau/Git/fmt/src/os.cc > CMakeFiles/format-impl-test.dir/__/src/os.cc.i

test/CMakeFiles/format-impl-test.dir/__/src/os.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/__/src/os.cc.s"
	cd /home/comau/Git/fmt/build/release/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comau/Git/fmt/src/os.cc -o CMakeFiles/format-impl-test.dir/__/src/os.cc.s

# Object files for target format-impl-test
format__impl__test_OBJECTS = \
"CMakeFiles/format-impl-test.dir/format-impl-test.cc.o" \
"CMakeFiles/format-impl-test.dir/header-only-test.cc.o" \
"CMakeFiles/format-impl-test.dir/test-main.cc.o" \
"CMakeFiles/format-impl-test.dir/gtest-extra.cc.o" \
"CMakeFiles/format-impl-test.dir/util.cc.o" \
"CMakeFiles/format-impl-test.dir/__/src/os.cc.o"

# External object files for target format-impl-test
format__impl__test_EXTERNAL_OBJECTS =

bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/header-only-test.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/test-main.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/gtest-extra.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/util.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/__/src/os.cc.o
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/build.make
bin/format-impl-test: test/gtest/libgtest.a
bin/format-impl-test: test/CMakeFiles/format-impl-test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/comau/Git/fmt/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable ../bin/format-impl-test"
	cd /home/comau/Git/fmt/build/release/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/format-impl-test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/format-impl-test.dir/build: bin/format-impl-test

.PHONY : test/CMakeFiles/format-impl-test.dir/build

test/CMakeFiles/format-impl-test.dir/clean:
	cd /home/comau/Git/fmt/build/release/test && $(CMAKE_COMMAND) -P CMakeFiles/format-impl-test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/format-impl-test.dir/clean

test/CMakeFiles/format-impl-test.dir/depend:
	cd /home/comau/Git/fmt/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/comau/Git/fmt /home/comau/Git/fmt/test /home/comau/Git/fmt/build/release /home/comau/Git/fmt/build/release/test /home/comau/Git/fmt/build/release/test/CMakeFiles/format-impl-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/format-impl-test.dir/depend

