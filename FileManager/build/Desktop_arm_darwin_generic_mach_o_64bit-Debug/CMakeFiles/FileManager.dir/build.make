# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /opt/homebrew/lib/python3.10/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/lib/python3.10/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/annamonastirska/Desktop/University/3/os/project/FileManager

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug

# Include any dependencies generated for this target.
include CMakeFiles/FileManager.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/FileManager.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/FileManager.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FileManager.dir/flags.make

CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o: CMakeFiles/FileManager.dir/flags.make
CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o: FileManager_autogen/mocs_compilation.cpp
CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o: CMakeFiles/FileManager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o -MF CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o -c /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/FileManager_autogen/mocs_compilation.cpp

CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.i"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/FileManager_autogen/mocs_compilation.cpp > CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.i

CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.s"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/FileManager_autogen/mocs_compilation.cpp -o CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.s

CMakeFiles/FileManager.dir/main.cpp.o: CMakeFiles/FileManager.dir/flags.make
CMakeFiles/FileManager.dir/main.cpp.o: /Users/annamonastirska/Desktop/University/3/os/project/FileManager/main.cpp
CMakeFiles/FileManager.dir/main.cpp.o: CMakeFiles/FileManager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/FileManager.dir/main.cpp.o"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/FileManager.dir/main.cpp.o -MF CMakeFiles/FileManager.dir/main.cpp.o.d -o CMakeFiles/FileManager.dir/main.cpp.o -c /Users/annamonastirska/Desktop/University/3/os/project/FileManager/main.cpp

CMakeFiles/FileManager.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/FileManager.dir/main.cpp.i"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/annamonastirska/Desktop/University/3/os/project/FileManager/main.cpp > CMakeFiles/FileManager.dir/main.cpp.i

CMakeFiles/FileManager.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/FileManager.dir/main.cpp.s"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/annamonastirska/Desktop/University/3/os/project/FileManager/main.cpp -o CMakeFiles/FileManager.dir/main.cpp.s

CMakeFiles/FileManager.dir/mainwindow.cpp.o: CMakeFiles/FileManager.dir/flags.make
CMakeFiles/FileManager.dir/mainwindow.cpp.o: /Users/annamonastirska/Desktop/University/3/os/project/FileManager/mainwindow.cpp
CMakeFiles/FileManager.dir/mainwindow.cpp.o: CMakeFiles/FileManager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/FileManager.dir/mainwindow.cpp.o"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/FileManager.dir/mainwindow.cpp.o -MF CMakeFiles/FileManager.dir/mainwindow.cpp.o.d -o CMakeFiles/FileManager.dir/mainwindow.cpp.o -c /Users/annamonastirska/Desktop/University/3/os/project/FileManager/mainwindow.cpp

CMakeFiles/FileManager.dir/mainwindow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/FileManager.dir/mainwindow.cpp.i"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/annamonastirska/Desktop/University/3/os/project/FileManager/mainwindow.cpp > CMakeFiles/FileManager.dir/mainwindow.cpp.i

CMakeFiles/FileManager.dir/mainwindow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/FileManager.dir/mainwindow.cpp.s"
	/usr/bin/llvm-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/annamonastirska/Desktop/University/3/os/project/FileManager/mainwindow.cpp -o CMakeFiles/FileManager.dir/mainwindow.cpp.s

# Object files for target FileManager
FileManager_OBJECTS = \
"CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/FileManager.dir/main.cpp.o" \
"CMakeFiles/FileManager.dir/mainwindow.cpp.o"

# External object files for target FileManager
FileManager_EXTERNAL_OBJECTS =

FileManager.app/Contents/MacOS/FileManager: CMakeFiles/FileManager.dir/FileManager_autogen/mocs_compilation.cpp.o
FileManager.app/Contents/MacOS/FileManager: CMakeFiles/FileManager.dir/main.cpp.o
FileManager.app/Contents/MacOS/FileManager: CMakeFiles/FileManager.dir/mainwindow.cpp.o
FileManager.app/Contents/MacOS/FileManager: CMakeFiles/FileManager.dir/build.make
FileManager.app/Contents/MacOS/FileManager: /opt/homebrew/Cellar/qt@5/5.15.15/lib/QtWidgets.framework/QtWidgets
FileManager.app/Contents/MacOS/FileManager: /opt/homebrew/Cellar/qt@5/5.15.15/lib/QtGui.framework/QtGui
FileManager.app/Contents/MacOS/FileManager: /opt/homebrew/Cellar/qt@5/5.15.15/lib/QtCore.framework/QtCore
FileManager.app/Contents/MacOS/FileManager: CMakeFiles/FileManager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable FileManager.app/Contents/MacOS/FileManager"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FileManager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FileManager.dir/build: FileManager.app/Contents/MacOS/FileManager
.PHONY : CMakeFiles/FileManager.dir/build

CMakeFiles/FileManager.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FileManager.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FileManager.dir/clean

CMakeFiles/FileManager.dir/depend:
	cd /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/annamonastirska/Desktop/University/3/os/project/FileManager /Users/annamonastirska/Desktop/University/3/os/project/FileManager /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug /Users/annamonastirska/Desktop/University/3/os/project/FileManager/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/CMakeFiles/FileManager.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/FileManager.dir/depend

