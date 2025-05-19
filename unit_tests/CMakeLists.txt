cmake_minimum_required(VERSION 3.6)

set(THIS Unit_Tests_Runner)

file(GLOB StoppableTestsSuite "${CMAKE_CURRENT_LIST_DIR}/StoppableTestsSuite/*")

#@+ ====================== User TEST_SUITES configuration ==========================
list(APPEND TEST_SUITES
            ${TestSuite}
)
#@- ========================= END OF USER CONFIGURATION ============================

add_executable(${THIS})
target_sources(${THIS} PRIVATE
                       "testRunner.cpp"
                       ${TEST_SUITES}
)

#@+ ==================== User TEST_DEPENDCIES configuration =======================
list(APPEND TEST_DEPENDCIES
            ${PROJECT_NAME}
)
#@- ========================= END OF USER CONFIGURATION ===========================

target_link_libraries(${THIS} PUBLIC
                              ${TEST_DEPENDCIES}
                              GTest::gtest
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
)

IMPORT_TARGET_DLLS(${THIS})
