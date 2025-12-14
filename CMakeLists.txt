cmake_minimum_required(VERSION 3.20)
project(htk-core VERSION 0.1.0 LANGUAGES CXX)

# macOS: Add Homebrew Qt6 to path
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(Eigen3 REQUIRED)

# Source files
set(SOURCES
        src/main.cpp
        src/core/HeadTracker.cpp
        src/input/WebcamTracker.cpp
        src/ui/PreviewWidget.cpp
)

set(HEADERS
        src/core/TrackingData.h
        src/core/HeadTracker.h
        src/input/WebcamTracker.h
        src/ui/PreviewWidget.h
        src/output/FreeTrackOutput.cpp
        src/output/TrackIROutput.cpp
)

# Add output files only on Windows
if(WIN32)
    list(APPEND SOURCES
            src/output/FreeTrackOutput.cpp
            src/output/TrackIROutput.cpp
    )
    list(APPEND HEADERS
            src/output/FreeTrackOutput.h
            src/output/TrackIROutput.h
    )
endif()

# Executable
if(WIN32)
    add_executable(htk_core WIN32 ${SOURCES} ${HEADERS})
elseif(APPLE)
    add_executable(htk_core MACOSX_BUNDLE ${SOURCES} ${HEADERS})
else()
    add_executable(htk_core ${SOURCES} ${HEADERS})
endif()

set_target_properties(htk_core PROPERTIES OUTPUT_NAME "htk-core")

# ESSENTIAL
add_custom_command(TARGET htk_core POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:htk_core>/resources
)

# Include directories
target_include_directories(htk_core PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(htk_core PRIVATE
        ${OpenCV_LIBS}
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGL
        Eigen3::Eigen
)

# Windows-specific
if(WIN32)
    target_compile_definitions(htk_core PRIVATE UNICODE _UNICODE)
endif()

# Install
if(APPLE)
    install(TARGETS htk_core
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin)
else()
    install(TARGETS htk_core RUNTIME DESTINATION bin)
endif()