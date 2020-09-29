cmake_minimum_required(VERSION 3.6)

set(THIS Unit_Tests_Runner)

file(GLOB StoppableTask_TestsSuite "${CMAKE_CURRENT_LIST_DIR}/StoppableTask_TestsSuite/*")

find_package(GTest REQUIRED)

add_executable(${THIS})
target_sources(${THIS} PRIVATE 
                       "testRunner.cpp"
                       ${StoppableTask_TestsSuite}
)

target_link_libraries(${THIS} PUBLIC
                              GTest::GTest
                              Stoppable_Task
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
)

set_target_properties(${THIS} PROPERTIES
                              CXX_STANDARD 17
)
