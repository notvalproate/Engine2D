add_executable(example1 src/example1.cpp)

target_link_libraries(example1 
    PRIVATE
        notval::Engine2D
        SDL2::SDL2main
        )

target_compile_options(example1
    PRIVATE
        ${CXX_WARNING_FLAGS}
)

include(cppcheck_analysis)
add_to_cppcheck(example1 example1.cpp)

set(ASSET_COPY_DIRECTORY $<TARGET_FILE_DIR:Engine2D>/assets)

add_custom_command(
    TARGET example1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${ASSET_COPY_DIRECTORY}
)
