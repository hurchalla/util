
if(TARGET test_programming_by_contract)
    return()
endif()

cmake_minimum_required(VERSION 3.12)
#project(test_programming_by_contract VERSION 1.0.0 LANGUAGES CXX)


if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()


include(FetchGoogleTest.cmake)


# needed for gtest_discover_tests()
include(GoogleTest)


add_executable(test_programming_by_contract
               test_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h)
set_target_properties(test_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_programming_by_contract
                      programming_by_contract
                      gtest_main)
#add_test(test_programming_by_contract  test_programming_by_contract)
gtest_discover_tests(test_programming_by_contract)


add_executable(test_ndebug_programming_by_contract
               test_ndebug_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h)
set_target_properties(test_ndebug_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_ndebug_programming_by_contract
                      programming_by_contract
                      gtest_main)
#add_test(test_ndebug_programming_by_contract
#          test_ndebug_programming_by_contract)
gtest_discover_tests(test_ndebug_programming_by_contract)

