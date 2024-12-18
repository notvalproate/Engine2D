cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(CXX_WARNING_FLAGS /W4)
    set(CXX_ERROR_WARNING /WX)
else()
    set(CXX_WARNING_FLAGS -Wall -Wextra -Wpedantic)
    set(CXX_ERROR_WARNING -Werror)
endif()

set(CXX_COMPILER_FLAGS ${CXX_WARNING_FLAGS} ${CXX_ERROR_WARNING})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Project Config and Dependencies

project(Engine2D VERSION 1.0)

find_package(SDL2 REQUIRED)
find_package(CppCheck)

set(ALL_ANALYSIS_TARGETS)

add_subdirectory(src Engine2D)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/Engine2D" CACHE PATH "Set install prefix" FORCE)
    endif()
endif()

add_custom_command(TARGET CMakeApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Engine2D> $<TARGET_FILE_DIR:Engine2D>
    COMMAND_EXPAND_LISTS
)

# CppCheck Analysis

if(CppCheck_FOUND)
    add_custom_target(analysis)
    add_dependencies(analysis ${ALL_ANALYSIS_TARGETS})
    set_target_properties(analysis PROPERTIES EXCLUDE_FROM_ALL TRUE)
    message("Added cppcheck target for analysis: ${ALL_ANALYSIS_TARGETS}")
endif()

# Build examples

if (${BUILD_EXAMPLES})
  MESSAGE(STATUS "Building examples is enabled")
  add_subdirectory(examples)
endif()