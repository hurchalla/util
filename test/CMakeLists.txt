# --- This file is distributed under the MIT Open Source License, as detailed
# in the file "LICENSE.TXT" in the root of this repository ---

if(TARGET test_hurchalla_util)
    return()
endif()

cmake_minimum_required(VERSION 3.14)
#project(test_hurchalla_util VERSION 1.0.0 LANGUAGES CXX)


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
include(EnableMaxWarnings.cmake)


# needed for gtest_discover_tests()
include(GoogleTest)



add_executable(test_hurchalla_util
               test_extensible_make_signed.cpp
               test_extensible_make_unsigned.cpp
               test_safely_promote_unsigned.cpp
               test_ut_numeric_limits.cpp
               test_sized_uint.cpp)

EnableMaxWarnings(test_hurchalla_util)

set_target_properties(test_hurchalla_util
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_hurchalla_util
                      hurchalla_util
                      gtest_main)
#add_test(test_hurchalla_util  test_hurchalla_util)
gtest_discover_tests(test_hurchalla_util)



add_executable(test_programming_by_contract
               test_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h)

EnableMaxWarnings(test_programming_by_contract)

set_target_properties(test_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_programming_by_contract
                      hurchalla_util
                      gtest_main)
#add_test(test_programming_by_contract  test_programming_by_contract)
gtest_discover_tests(test_programming_by_contract)



add_executable(test_ndebug_programming_by_contract
               test_ndebug_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h)

EnableMaxWarnings(test_ndebug_programming_by_contract)

set_target_properties(test_ndebug_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_ndebug_programming_by_contract
                      hurchalla_util
                      gtest_main)
#add_test(test_ndebug_programming_by_contract
#          test_ndebug_programming_by_contract)
gtest_discover_tests(test_ndebug_programming_by_contract)
