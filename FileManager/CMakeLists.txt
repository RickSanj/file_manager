cmake_minimum_required(VERSION 3.16)

project(FileManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Manually specify the path to QuaZip's CMake config
find_package(ZLIB REQUIRED)

#set(QuaZip-Qt6_DIR "/opt/homebrew/Cellar/quazip/1.4/lib/cmake/QuaZip-Qt6-1.4")
#find_package(QuaZip-Qt6 REQUIRED)



set(QUALIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qua/quazip/1.4")

include_directories(${QUALIB_DIR}/include)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        mainwindow.h
        dirOperations.cpp
        fileOperations.cpp
        customMenu.cpp
        keyboard.cpp
        cmdline.cpp
        multipleOperations.cpp
        archive.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FileManager
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(FileManager SHARED ${PROJECT_SOURCES})
    else()
        add_executable(FileManager ${PROJECT_SOURCES})
    endif()
endif()

set_target_properties(FileManager PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
#"${CMAKE_CURRENT_SOURCE_DIR}/qua/quazip/1.4/include/QuaZip-Qt6-1.4/quazip"
#/opt/homebrew/Cellar/quazip/1.4/include/quazip
target_include_directories(FileManager PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/qua/quazip/1.4/include/QuaZip-Qt6-1.4/quazip")

install(TARGETS FileManager
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(FileManager PRIVATE Qt6::Widgets)

if (WIN32)
    target_link_libraries(FileManager PRIVATE ${QUALIB_DIR}/lib/quazip.dll
    )
else()
    target_link_libraries(FileManager PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            ${QUALIB_DIR}/lib/libquazip1-qt6.dylib
            ZLIB::ZLIB
    )
endif()

# Set properties for macOS bundle
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FileManager)
endif()



if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FileManager)
endif()