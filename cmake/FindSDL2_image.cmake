# FindSDL2_image.cmake
# Locate the SDL2_image library
# This script defines the following variables and target:
#
#   SDL2_IMAGE_FOUND         - Set to TRUE if the library was found
#   SDL2_IMAGE_INCLUDE_DIRS  - Include directories for SDL2_image
#   SDL2_IMAGE_LIBRARIES     - The SDL2_image library
#   SDL2::SDL2_image         - Imported target for linking

# Option to use static or shared library
option(SDL2_IMAGE_USE_STATIC "Use the static SDL2_image library" OFF)

find_path(SDL2_IMAGE_INCLUDE_DIR
          NAMES SDL2/SDL_image.h
          HINTS ENV SDL2_IMAGE_ROOT
          PATH_SUFFIXES include SDL2_image/include)

if (SDL2_IMAGE_USE_STATIC)
    find_library(SDL2_IMAGE_LIBRARY
                 NAMES SDL2_image SDL2_image-static
                 HINTS ENV SDL2_IMAGE_ROOT
                 PATH_SUFFIXES lib lib64 bin SDL2_image/lib SDL2_image/lib64)
else()
    find_library(SDL2_IMAGE_LIBRARY
                 NAMES SDL2_image
                 HINTS ENV SDL2_IMAGE_ROOT
                 PATH_SUFFIXES lib lib64 bin SDL2_image/lib SDL2_image/lib64)
endif()

# Check if both the include directory and library are found
if (SDL2_IMAGE_INCLUDE_DIR AND SDL2_IMAGE_LIBRARY)
    set(SDL2_IMAGE_FOUND TRUE)
    set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR}/SDL2)
    set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})

    message(STATUS "SDL2_image successfully found!\nSDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}\nSDL2_IMAGE_LIBRARIES: ${SDL2_IMAGE_LIBRARIES}\n")

    add_library(SDL2::SDL2_image UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2_image PROPERTIES
                           IMPORTED_LOCATION ${SDL2_IMAGE_LIBRARY}
                           INTERFACE_INCLUDE_DIRECTORIES ${SDL2_IMAGE_INCLUDE_DIRS})
else()
    set(SDL2_IMAGE_FOUND FALSE)

    if (NOT SDL2_IMAGE_INCLUDE_DIR)
        message(STATUS "SDL2_image include directory not found")
    endif()

    if (NOT SDL2_IMAGE_LIBRARY)
        message(STATUS "SDL2_image library not found")
    endif()
endif()

# Mark the result for use with find_package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image REQUIRED_VARS SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
