# FindSDL2.cmake
# Locate the SDL2 library
# This script defines the following variables and targets:
#
#   SDL2_FOUND         - Set to TRUE if the library was found
#   SDL2_INCLUDE_DIRS  - Include directories for SDL2
#   SDL2_LIBRARIES     - The SDL2 libraries
#   SDL2_RUNTIME       - Path to the SDL2 runtime library (.dll, .so, or .dylib)
#   SDL2::SDL2         - Imported target for linking the SDL2 library
#   SDL2::SDL2main     - Imported target for linking the SDL2main library

# Option to use static or shared library
option(SDL2_USE_STATIC "Use the static SDL2 library" OFF)

find_path(SDL2_INCLUDE_DIR
          NAMES SDL2/SDL.h
          HINTS ENV SDL2_ROOT
          PATH_SUFFIXES include SDL2/include)

if (SDL2_USE_STATIC)
    find_library(SDL2_LIBRARY
                 NAMES SDL2 SDL2-static
                 HINTS ENV SDL2_ROOT
                 PATH_SUFFIXES lib lib64 SDL2/lib SDL2/lib64)
    find_library(SDL2MAIN_LIBRARY
                 NAMES SDL2main SDL2main-static
                 HINTS ENV SDL2_ROOT
                 PATH_SUFFIXES lib lib64 SDL2/lib SDL2/lib64)
else()
    find_library(SDL2_LIBRARY
                 NAMES SDL2 SDL2d
                 HINTS ENV SDL2_ROOT
                 PATH_SUFFIXES lib lib64 SDL2/lib SDL2/lib64)
    find_library(SDL2MAIN_LIBRARY
                 NAMES SDL2main SDL2maind
                 HINTS ENV SDL2_ROOT
                 PATH_SUFFIXES lib lib64 SDL2/lib SDL2/lib64)
    find_file(SDL2_RUNTIME
                 NAMES SDL2.dll libSDL2.dll SDL2.so libSDL2.so SDL2.dylib libSDL2.dylib
                 HINTS ENV SDL2_ROOT
                 PATH_SUFFIXES lib lib64 SDL2/lib SDL2/lib64 bin SDL2/bin)
endif()

if (SDL2_INCLUDE_DIR AND SDL2_LIBRARY AND SDL2MAIN_LIBRARY)
    set(SDL2_FOUND TRUE)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}/SDL2)
    set(SDL2_LIBRARIES ${MINGW_LIBRARY} ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})

    message(STATUS "\nSDL2 successfully found!\nSDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}\nSDL2_LIBRARIES: ${SDL2_LIBRARIES}\n")

    if(SDL2_RUNTIME)
        message(STATUS "SDL2 runtime found! Path: ${SDL2_RUNTIME}\n")
    else()
        message(WARNING "SDL2 runtime not found! You may need to copy the SDL2 runtime (.dll, .so or .dylib) to run examples.\n")
    endif()

    add_library(SDL2::SDL2 UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
                           IMPORTED_LOCATION ${SDL2_LIBRARY}
                           INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIRS})

    add_library(SDL2::SDL2main_internal UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2main_internal PROPERTIES
                           IMPORTED_LOCATION ${SDL2MAIN_LIBRARY}
                           INTERFACE_LINK_LIBRARIES SDL2::SDL2)

    add_library(SDL2::SDL2main INTERFACE IMPORTED)

    if(MINGW)
        set_property(TARGET SDL2::SDL2main PROPERTY
            INTERFACE_LINK_LIBRARIES "mingw32")
    endif()

    set_property(TARGET SDL2::SDL2main APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES SDL2::SDL2main_internal)
else()
    set(SDL2_FOUND FALSE)

    if (NOT SDL2_INCLUDE_DIR)
        message(STATUS "SDL2 include directory not found")
    endif()

    if (NOT SDL2_LIBRARY)
        message(STATUS "SDL2 library not found")
    endif()

    if (NOT SDL2MAIN_LIBRARY)
        message(STATUS "SDL2main library not found")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2MAIN_LIBRARY)
