cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

project(Engine2D VERSION 1.0)

if(MSVC)
    set(CXX_COMPILER_FLAGS /W4 /MP)
else()
    set(CXX_COMPILER_FLAGS -Wall)
endif()

message("CXX_COMPILER_FLAGS: ${CXX_COMPILER_FLAGS}")

message("CXX_COMPILER_FLAGS: ${CXX_COMPILER_FLAGS}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Vendor Dependencies

# If false, will use find_package to try to find prebuilt and installed dependencies. If true, will build dependencies from source.
option(ENGINE2D_BUILD_DEPENDENCIES "Build dependencies from source" ON)

if(ENGINE2D_BUILD_DEPENDENCIES)
    message(STATUS "Building dependencies from source! To use prebuilt dependencies, set ENGINE2D_BUILD_DEPENDENCIES to OFF")

    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build all dependencies as a shared libraries" FORCE)
    set(BOX2D_BUILD_SHARED ON CACHE BOOL "Build Box2D as a shared library" FORCE)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build Box2D unit tests" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build Box2D testbed" FORCE)
    
    add_subdirectory(vendor EXCLUDE_FROM_ALL)
else()
    message(STATUS "Using prebuilt dependencies if found on the system! To build dependencies from source, set ENGINE2D_BUILD_DEPENDENCIES to ON")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(Box2D REQUIRED)
endif()

# Some issue with Box2D not respecting the global X_OUTPUT_DIRECTORY correctly, so we set it manually

set_target_properties(box2d PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
)

# Project Config and Dependencies

find_package(CppCheck)

set(ALL_ANALYSIS_TARGETS)

add_subdirectory(src Engine2D)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/Engine2D" CACHE PATH "Set install prefix" FORCE)
    endif()
endif()

# CppCheck Analysis

if(CppCheck_FOUND)
    add_custom_target(analysis)
    add_dependencies(analysis ${ALL_ANALYSIS_TARGETS})
    set_target_properties(analysis PROPERTIES EXCLUDE_FROM_ALL TRUE)
    message("Added cppcheck target for analysis: ${ALL_ANALYSIS_TARGETS}")
endif()

# Build examples

option(ENGINE2D_BUILD_EXAMPLES "Build example projects" OFF)

if (ENGINE2D_BUILD_EXAMPLES)
  MESSAGE(STATUS "Building examples is enabled")
  add_subdirectory(examples)
endif()
