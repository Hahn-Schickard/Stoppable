#@+ ======================== User TARGET NAME configuration =============================
set(THIS ThreadPool)
#@- =========================== END OF USER CONFIGURATION ===============================

set(TARGET ${PROJECT_NAME})

#@+ =========================== User TARGET configuration ===============================
file(GLOB THIS_INCLUDES ${INCLUDES_DIR}/*.hpp)
file(GLOB SOURCEFILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(${TARGET})
target_sources(${TARGET}
    PUBLIC
        ${THIS_INCLUDES}
    PRIVATE
        ${SOURCEFILES}
)
#@- =========================== END OF USER CONFIGURATION ===============================

target_compile_features(${TARGET} INTERFACE cxx_std_17)

target_include_directories(${TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${INCLUDES_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

PRINT_TARGET_PROPERTIES(${TARGET})

INSTALL_TARGET(${TARGET})
INSTALL_HEADERS(HEADERS "${THIS_INCLUDES}")