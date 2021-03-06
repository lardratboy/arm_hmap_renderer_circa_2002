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
CMAKE_SOURCE_DIR = /home/bpt/hmap

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bpt/hmap/build

# Include any dependencies generated for this target.
include rom/CMakeFiles/rom.dir/depend.make

# Include the progress variables for this target.
include rom/CMakeFiles/rom.dir/progress.make

# Include the compile flags for this target's objects.
include rom/CMakeFiles/rom.dir/flags.make

rom/CMakeFiles/rom.dir/crt0.s.obj: rom/CMakeFiles/rom.dir/flags.make
rom/CMakeFiles/rom.dir/crt0.s.obj: /home/bpt/gba-toolchain/lib/rom/crt0.s
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bpt/hmap/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object rom/CMakeFiles/rom.dir/crt0.s.obj"
	cd /home/bpt/hmap/build/rom && /home/bpt/gba-toolchain/arm-gnu-toolchain/bin/arm-none-eabi-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/rom.dir/crt0.s.obj -c /home/bpt/gba-toolchain/lib/rom/crt0.s

rom/CMakeFiles/rom.dir/gba-syscalls.c.obj: rom/CMakeFiles/rom.dir/flags.make
rom/CMakeFiles/rom.dir/gba-syscalls.c.obj: /home/bpt/gba-toolchain/lib/rom/gba-syscalls.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bpt/hmap/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object rom/CMakeFiles/rom.dir/gba-syscalls.c.obj"
	cd /home/bpt/hmap/build/rom && /home/bpt/gba-toolchain/arm-gnu-toolchain/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/rom.dir/gba-syscalls.c.obj   -c /home/bpt/gba-toolchain/lib/rom/gba-syscalls.c

rom/CMakeFiles/rom.dir/gba-syscalls.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/rom.dir/gba-syscalls.c.i"
	cd /home/bpt/hmap/build/rom && /home/bpt/gba-toolchain/arm-gnu-toolchain/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bpt/gba-toolchain/lib/rom/gba-syscalls.c > CMakeFiles/rom.dir/gba-syscalls.c.i

rom/CMakeFiles/rom.dir/gba-syscalls.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/rom.dir/gba-syscalls.c.s"
	cd /home/bpt/hmap/build/rom && /home/bpt/gba-toolchain/arm-gnu-toolchain/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bpt/gba-toolchain/lib/rom/gba-syscalls.c -o CMakeFiles/rom.dir/gba-syscalls.c.s

# Object files for target rom
rom_OBJECTS = \
"CMakeFiles/rom.dir/crt0.s.obj" \
"CMakeFiles/rom.dir/gba-syscalls.c.obj"

# External object files for target rom
rom_EXTERNAL_OBJECTS =

rom/librom.a: rom/CMakeFiles/rom.dir/crt0.s.obj
rom/librom.a: rom/CMakeFiles/rom.dir/gba-syscalls.c.obj
rom/librom.a: rom/CMakeFiles/rom.dir/build.make
rom/librom.a: rom/CMakeFiles/rom.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bpt/hmap/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library librom.a"
	cd /home/bpt/hmap/build/rom && $(CMAKE_COMMAND) -P CMakeFiles/rom.dir/cmake_clean_target.cmake
	cd /home/bpt/hmap/build/rom && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rom.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rom/CMakeFiles/rom.dir/build: rom/librom.a

.PHONY : rom/CMakeFiles/rom.dir/build

rom/CMakeFiles/rom.dir/clean:
	cd /home/bpt/hmap/build/rom && $(CMAKE_COMMAND) -P CMakeFiles/rom.dir/cmake_clean.cmake
.PHONY : rom/CMakeFiles/rom.dir/clean

rom/CMakeFiles/rom.dir/depend:
	cd /home/bpt/hmap/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bpt/hmap /home/bpt/gba-toolchain/lib/rom /home/bpt/hmap/build /home/bpt/hmap/build/rom /home/bpt/hmap/build/rom/CMakeFiles/rom.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rom/CMakeFiles/rom.dir/depend

