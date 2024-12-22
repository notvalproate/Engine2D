set(Box2D_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/CMakeLists.txt")
set(SDL2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDL2/CMakeLists.txt")
set(SDL2_image_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image/CMakeLists.txt")

macro(fetch_dependencies)
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMODULE_RESULT
    )

    if(NOT GIT_SUBMODULE_RESULT EQUAL 0)
        message(FATAL_ERROR "Git Submodule update failed with code ${GIT_SUBMODULE_RESULT}")
    endif()
endmacro()

if(NOT EXISTS ${Box2D_PATH})
    message(STATUS "Box2D not found. Trying to initialize submodules...")
    fetch_dependencies()
else()
    message(STATUS "Box2D found.")
endif()

if(NOT EXISTS ${SDL2_PATH})
    message(STATUS "SDL2 not found. Trying to initialize submodules...")
    fetch_dependencies()
else()
    message(STATUS "SDL2 found.")
endif()

if(NOT EXISTS ${SDL2_image_PATH})
    message(STATUS "SDL2_image not found. Trying to initialize submodules...")
    fetch_dependencies()
else()
    message(STATUS "SDL2_image found.")
endif()

add_subdirectory(Box2D)
add_subdirectory(SDL2)
add_subdirectory(SDL2_image)