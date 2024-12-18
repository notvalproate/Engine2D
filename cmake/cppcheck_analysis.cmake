if(NOT CppCheck_FOUND)
    find_package(CppCheck)
endif()

macro(add_to_cppcheck _target _sources)
    if(CppCheck_FOUND)
        get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

        foreach(dir ${dirs})
            list(APPEND cppcheck_includes "-I${dir}")
        endforeach()

        list(APPEND ALL_ANALYSIS_TARGETS "${_target}_analysis")

        if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
            set(ALL_ANALYSIS_TARGETS ${ALL_ANALYSIS_TARGETS} PARENT_SCOPE)
        endif()

        separate_arguments(tmp_args UNIX_COMMAND ${CPPCHECK_ARG})

        add_custom_target(${_target}_analysis)
        set_target_properties(${_target}_analysis PROPERTIES EXCLUDE_FROM_ALL TRUE)

        add_custom_command(TARGET ${_target}_analysis PRE_BUILD
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMAND ${CPPCHECK_BIN} ${tmp_args} ${cppcheck_includes} ${${_sources}}
            DEPENDS ${${_sources}}
            COMMENT "Running cppcheck: ${_target}"
            VERBATIM
        )
        message("Added cppcheck target for ${_target}")
    endif()
endmacro()