cmake_minimum_required(VERSION 3.12) 

set(THIS Stoppable_Task)

list(APPEND THIS_INCLUDES_DIRS
            ${INCLUDES_DIR}/
)

file(GLOB sources_list "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

INCLUDE_DIRS_AND_FILES("${THIS_INCLUDES_DIRS}" THIS_INCLUDES)


add_library(${THIS} INTERFACE)        

target_link_libraries(${THIS} INTERFACE 
                              Threads::Threads
)

target_include_directories(${THIS} INTERFACE
                           $<BUILD_INTERFACE:${INCLUDES_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS ${THIS}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    FILES ${THIS_INCLUDES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
