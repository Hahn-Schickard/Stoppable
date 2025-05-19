cmake_minimum_required(VERSION 3.6)

FUNCTION(PRINT_FILES file_list)
    if(VERBOSE_FILE_INCLUSION)
        if(NOT file_list)
            message(STATUS "| VERBOSE_OUTPUT |     no files found")
        else()
            foreach(thing ${file_list})
                message(STATUS "| VERBOSE_OUTPUT |   |-- ${thing}")
            endforeach()
        endif()
    endif()
ENDFUNCTION()

FUNCTION(PRINT_TARGET_PROPERTIES target_name)
    if(VERBOSE_FILE_INCLUSION)
        message(STATUS "| VERBOSE_OUTPUT | ${target_name} target properties are::")

        get_target_property(include_dirs ${target_name} INCLUDE_DIRECTORIES)
        if(include_dirs)
            message(STATUS "| VERBOSE_OUTPUT | ==Include Directories==")
            PRINT_FILES("${include_dirs}")
        endif()

        get_target_property(sources_list ${target_name} SOURCES)
        if(sources_list)
            message(STATUS "| VERBOSE_OUTPUT | =====Source Files=====")
            PRINT_FILES("${sources_list}")
        endif()

        get_target_property(linked_libs ${target_name} LINK_LIBRARIES)
        if(linked_libs)
            message(STATUS "| VERBOSE_OUTPUT | =====Linked Libraries=====")
            PRINT_FILES("${linked_libs}")
        endif()

        message("-------------------------------------------------------------------------------------------------------------------------------------")
        message(" ")
    endif()
ENDFUNCTION()

FUNCTION(INCLUDE_DIRS directory_list)
    foreach(directory ${directory_list})
        include_directories(${directory})
    endforeach()
ENDFUNCTION()

FUNCTION(INCLUDE_DIRS_AND_FILES directory_list included_file_list)
    INCLUDE_DIRS("${directory_list}")
    foreach(directory ${directory_list})
        file(GLOB tmp_file_list ${directory}/*.hpp)
        list(APPEND included_file_list ${tmp_file_list})
    endforeach()
    set(${included_file_list} ${${included_file_list}} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(ADD_PUBLIC_HEADERS input_headers)
    get_property(public_headers GLOBAL PROPERTY PUBLIC_HEADERS)
    list(APPEND public_headers ${input_headers})
    list(REMOVE_DUPLICATES public_headers)
    set_property(GLOBAL PROPERTY PUBLIC_HEADERS "${public_headers}")
ENDFUNCTION()

FUNCTION(IMPORT_TARGET_DLLS target)
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "EXECUTABLE")    
        if(WIN32)
        message(STATUS "Importing ${target} .dll files")
        add_custom_command(TARGET ${target} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy
                            -t $<TARGET_FILE_DIR:${target}>
                            $<TARGET_RUNTIME_DLLS:${target}>
                            COMMAND_EXPAND_LISTS
        )
        # Workaround to get conan .dll files into build
        foreach(directory ${CONAN_RUNTIME_LIB_DIRS})
            file(GLOB dir_dependencies "${directory}/*.dll")
            if(dir_dependencies)
                list(APPEND dependencies "${dir_dependencies}")
            endif()
        endforeach()
        add_custom_command(TARGET ${target} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy
                            -t $<TARGET_FILE_DIR:${target}>
                            ${dependencies}
                            COMMAND_EXPAND_LISTS
        )
        else()
            message(STATUS "Not a Windows OS, skipping .dll file import for ${target} target")
        endif()
    else()
        message(STATUS "Target ${target} is not an executable")
    endif ()
ENDFUNCTION()

FUNCTION(INSTALL_TARGET target)
    message(STATUS "Installing Target")
    install(
        TARGETS ${target}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY
        ARCHIVE
        RUNTIME           
    )
ENDFUNCTION()

FUNCTION(INSTALL_PUBLIC_HEADERS)
    set(install_dir ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
    message(STATUS "Installing public headers to ${install_dir}")
    get_property(headers GLOBAL PROPERTY PUBLIC_HEADERS)
    foreach(header ${headers})
        message(STATUS "- ${header}")
    endforeach()
    install(
        FILES ${headers}
        DESTINATION ${install_dir}
    )
ENDFUNCTION()
