cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

project(Engine2D VERSION 1.0)

if(MSVC)
    set(CXX_WARNING_FLAGS /W4)
else()
    set(CXX_WARNING_FLAGS -Wall)
endif()

set(CXX_COMPILER_FLAGS ${CXX_WARNING_FLAGS})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Vendor Dependencies

set(BUILD_SHARED_LIBS ON)
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)

add_subdirectory(vendor)

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
