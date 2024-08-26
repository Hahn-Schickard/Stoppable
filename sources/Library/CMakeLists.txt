cmake_minimum_required(VERSION 3.12) 

#@+ ======================== User MODULE NAME configuration ============================
set(THIS HeaderOnly)
#@- =========================== END OF USER CONFIGURATION ===============================

set(MODULE ${PROJECT_NAME}_${THIS})

list(APPEND THIS_INCLUDES_DIRS
            ${INCLUDES_DIR}/
)

file(GLOB sources_list "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

INCLUDE_DIRS_AND_FILES("${THIS_INCLUDES_DIRS}" THIS_INCLUDES)

add_library(${MODULE} INTERFACE)        

#@+ ======================== User DEPENDENCIES configuration ============================
target_link_libraries(${MODULE} INTERFACE 
                      Threads::Threads
)
#@- =========================== END OF USER CONFIGURATION ===============================

target_include_directories(${MODULE} INTERFACE
                           $<BUILD_INTERFACE:${INCLUDES_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_compile_features(${MODULE} INTERFACE cxx_std_17)

install(
    TARGETS ${MODULE}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES ${THIS_INCLUDES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
