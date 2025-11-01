cmake_minimum_required(VERSION 3.20)
project(OrbitView VERSION 0.1.0 LANGUAGES CXX)

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
    add_executable(OrbitView WIN32 ${SOURCES} ${HEADERS})
elseif(APPLE)
    add_executable(OrbitView MACOSX_BUNDLE ${SOURCES} ${HEADERS})
else()
    add_executable(OrbitView ${SOURCES} ${HEADERS})
endif()

# Include directories
target_include_directories(OrbitView PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(OrbitView PRIVATE
        ${OpenCV_LIBS}
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGL
        Eigen3::Eigen
)

# Windows-specific
if(WIN32)
    target_compile_definitions(OrbitView PRIVATE UNICODE _UNICODE)
endif()

# Install
if(APPLE)
    install(TARGETS OrbitView
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin)
else()
    install(TARGETS OrbitView RUNTIME DESTINATION bin)
endif()