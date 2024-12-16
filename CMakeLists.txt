cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_HELLO_WORLD")

if(MSVC)
    set(CXX_WARNING_FLAGS /W4)
    set(CXX_ERROR_WARNING /WX)
else()
    set(CXX_WARNING_FLAGS -Wall -Wextra -Wpedantic)
    set(CXX_ERROR_WARNING -Werror)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(CMakeApp VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(Catch2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CppCheck)

set(ALL_ANALYSIS_TARGETS)

add_subdirectory(cprint)
add_subdirectory(cats)
add_subdirectory(vmath)

set(SOURCES
    src/main.cpp
    src/Cat.cpp
)

add_executable(CMakeApp ${SOURCES})

target_link_libraries(CMakeApp 
    PRIVATE
        cprint
        CMakeApp::cats
        CMakeApp::vmath
        ZLIB::ZLIB
)

target_include_directories(CMakeApp 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(CMakeApp 
    PRIVATE
        ${CXX_WARNING_FLAGS}
        ${CXX_ERROR_WARNING}
)

if(WIN32)
    target_compile_definitions(CMakeApp 
        PRIVATE
            _PLATFORM_WIN_32
    )
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/CMakeApp" CACHE PATH "Set install prefix" FORCE)
    endif()
endif()

add_custom_command(TARGET CMakeApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeApp> $<TARGET_FILE_DIR:CMakeApp>
    COMMAND_EXPAND_LISTS
)

# CppCheck Analysis

include(cppcheck_analysis)
add_to_cppcheck(CMakeApp SOURCES)

if(CppCheck_FOUND)
    add_custom_target(analysis)
    add_dependencies(analysis ${ALL_ANALYSIS_TARGETS})
    set_target_properties(analysis PROPERTIES EXCLUDE_FROM_ALL TRUE)
    message("Added cppcheck target for analysis: ${ALL_ANALYSIS_TARGETS}")
endif()

# Unit Testing

enable_testing()

include(catch2_testing)

add_to_catch2(test_cats 
    CMakeApp::cats
)

add_to_catch2(test_fibbo)