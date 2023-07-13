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
CMAKE_SOURCE_DIR = /home/yeray/proyectos/mcoVanetza

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yeray/proyectos/mcoVanetza/build

# Include any dependencies generated for this target.
include vanetza/facilities/CMakeFiles/facilities.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include vanetza/facilities/CMakeFiles/facilities.dir/compiler_depend.make

# Include the progress variables for this target.
include vanetza/facilities/CMakeFiles/facilities.dir/progress.make

# Include the compile flags for this target's objects.
include vanetza/facilities/CMakeFiles/facilities.dir/flags.make

vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/flags.make
vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o: ../vanetza/facilities/cam_functions.cpp
vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yeray/proyectos/mcoVanetza/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o -MF CMakeFiles/facilities.dir/cam_functions.cpp.o.d -o CMakeFiles/facilities.dir/cam_functions.cpp.o -c /home/yeray/proyectos/mcoVanetza/vanetza/facilities/cam_functions.cpp

vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/facilities.dir/cam_functions.cpp.i"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yeray/proyectos/mcoVanetza/vanetza/facilities/cam_functions.cpp > CMakeFiles/facilities.dir/cam_functions.cpp.i

vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/facilities.dir/cam_functions.cpp.s"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yeray/proyectos/mcoVanetza/vanetza/facilities/cam_functions.cpp -o CMakeFiles/facilities.dir/cam_functions.cpp.s

vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/flags.make
vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o: ../vanetza/facilities/path_history.cpp
vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yeray/proyectos/mcoVanetza/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o -MF CMakeFiles/facilities.dir/path_history.cpp.o.d -o CMakeFiles/facilities.dir/path_history.cpp.o -c /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_history.cpp

vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/facilities.dir/path_history.cpp.i"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_history.cpp > CMakeFiles/facilities.dir/path_history.cpp.i

vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/facilities.dir/path_history.cpp.s"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_history.cpp -o CMakeFiles/facilities.dir/path_history.cpp.s

vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/flags.make
vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o: ../vanetza/facilities/path_point.cpp
vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o: vanetza/facilities/CMakeFiles/facilities.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yeray/proyectos/mcoVanetza/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o -MF CMakeFiles/facilities.dir/path_point.cpp.o.d -o CMakeFiles/facilities.dir/path_point.cpp.o -c /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_point.cpp

vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/facilities.dir/path_point.cpp.i"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_point.cpp > CMakeFiles/facilities.dir/path_point.cpp.i

vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/facilities.dir/path_point.cpp.s"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yeray/proyectos/mcoVanetza/vanetza/facilities/path_point.cpp -o CMakeFiles/facilities.dir/path_point.cpp.s

# Object files for target facilities
facilities_OBJECTS = \
"CMakeFiles/facilities.dir/cam_functions.cpp.o" \
"CMakeFiles/facilities.dir/path_history.cpp.o" \
"CMakeFiles/facilities.dir/path_point.cpp.o"

# External object files for target facilities
facilities_EXTERNAL_OBJECTS =

lib/static/libvanetza_facilities.a: vanetza/facilities/CMakeFiles/facilities.dir/cam_functions.cpp.o
lib/static/libvanetza_facilities.a: vanetza/facilities/CMakeFiles/facilities.dir/path_history.cpp.o
lib/static/libvanetza_facilities.a: vanetza/facilities/CMakeFiles/facilities.dir/path_point.cpp.o
lib/static/libvanetza_facilities.a: vanetza/facilities/CMakeFiles/facilities.dir/build.make
lib/static/libvanetza_facilities.a: vanetza/facilities/CMakeFiles/facilities.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yeray/proyectos/mcoVanetza/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library ../../lib/static/libvanetza_facilities.a"
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && $(CMAKE_COMMAND) -P CMakeFiles/facilities.dir/cmake_clean_target.cmake
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/facilities.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
vanetza/facilities/CMakeFiles/facilities.dir/build: lib/static/libvanetza_facilities.a
.PHONY : vanetza/facilities/CMakeFiles/facilities.dir/build

vanetza/facilities/CMakeFiles/facilities.dir/clean:
	cd /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities && $(CMAKE_COMMAND) -P CMakeFiles/facilities.dir/cmake_clean.cmake
.PHONY : vanetza/facilities/CMakeFiles/facilities.dir/clean

vanetza/facilities/CMakeFiles/facilities.dir/depend:
	cd /home/yeray/proyectos/mcoVanetza/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yeray/proyectos/mcoVanetza /home/yeray/proyectos/mcoVanetza/vanetza/facilities /home/yeray/proyectos/mcoVanetza/build /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities /home/yeray/proyectos/mcoVanetza/build/vanetza/facilities/CMakeFiles/facilities.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : vanetza/facilities/CMakeFiles/facilities.dir/depend

