# FindBox2D.cmake
# Locate the Box2D library
# This script defines the following variables and target:
#
#   Box2D_FOUND        - Set to TRUE if the library was found
#   Box2D_INCLUDE_DIRS - Include directories for Box2D
#   Box2D_LIBRARIES    - The Box2D library
#   Box2D_RUNTIME      - Path to the Box2D runtime library (.dll, .so, or .dylib)
#   Box2D::Box2D       - Imported target for linking

# Option to use static or shared library
option(BOX2D_USE_STATIC "Use the static Box2D library" OFF)

find_path(Box2D_INCLUDE_DIR
          NAMES box2d/box2d.h
          HINTS ENV Box2D_ROOT
          PATH_SUFFIXES include box2d/include)

if (BOX2D_USE_STATIC)
    find_library(Box2D_LIBRARY
                 NAMES Box2D box2d
                 HINTS ENV Box2D_ROOT
                 PATH_SUFFIXES lib lib64 box2d/lib box2d/lib64
                 NAMES_PER_DIR Box2D_static box2d_static)
else()
    find_library(Box2D_LIBRARY
                 NAMES Box2D box2d
                 HINTS ENV Box2D_ROOT
                 PATH_SUFFIXES lib lib64 box2d/lib box2d/lib64
                 NAMES_PER_DIR Box2D_shared box2d_shared)
    find_file(Box2D_RUNTIME
                 NAMES Box2D.dll libBox2D.dll Box2Dd.dll Box2D.so libBox2D.so Box2D.dylib libBox2D.dylib
                 HINTS ENV Box2D_ROOT
                 PATH_SUFFIXES lib lib64 box2d/lib box2d/lib64 bin box2d/bin)
endif()

if (Box2D_INCLUDE_DIR AND Box2D_LIBRARY)
    set(Box2D_FOUND TRUE)
    set(Box2D_INCLUDE_DIRS ${Box2D_INCLUDE_DIR})
    set(Box2D_LIBRARIES ${Box2D_LIBRARY})

	message(STATUS "\nBox2D successfully found!\nInclude directory: ${Box2D_INCLUDE_DIRS}\nLibrary: ${Box2D_LIBRARIES}\n")

    if(Box2D_RUNTIME)
        message(STATUS "Box2D runtime found! Path: ${Box2D_RUNTIME}\n")
    else()
        message(WARNING "Box2D runtime not found! You may need to copy the Box2D runtime (.dll, .so or .dylib) to run examples.\n")
    endif()

    add_library(Box2D::Box2D UNKNOWN IMPORTED)
    set_target_properties(Box2D::Box2D PROPERTIES
                           IMPORTED_LOCATION ${Box2D_LIBRARY}
                           INTERFACE_INCLUDE_DIRECTORIES ${Box2D_INCLUDE_DIRS})
else()
    set(Box2D_FOUND FALSE)

    if (NOT Box2D_INCLUDE_DIR)
        message(STATUS "Box2D include directory not found")
    endif()

    if (NOT Box2D_LIBRARY)
        message(STATUS "Box2D library not found")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Box2D REQUIRED_VARS Box2D_INCLUDE_DIR Box2D_LIBRARY)
