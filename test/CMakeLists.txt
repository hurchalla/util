# --- This file is distributed under the MIT Open Source License, as detailed
# in the file "LICENSE.TXT" in the root of this repository ---

if(TARGET test_hurchalla_util)
    return()
endif()

# later versions are probably fine, but are untested
cmake_minimum_required(VERSION 3.14...4.03)

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



# This library's main CMakeLists.txt specifies little for the C++ standard, and
# will thus compile using whatever standard has been set (or has been defaulted)
# by a CMakeLists.txt that is using/consuming the library.  However, inside this
# unit-testing CMakeLists.txt, we want precise control of the standard and so we
# set the standard below.
#
# We set CMAKE_CXX_EXTENSIONS to OFF because otherwise CMake will pass to gcc
# (and probably clang) -std=gnu++XX instead of -std=c++XX (replace XX with 11
# or 14 or some other C++ version).  Note this may not always even be sufficient
# for all circumstances -
# A while ago I used a 'modern CMake' approach by writing
#    target_compile_features(MyTarget INTERFACE cxx_std_11)
#    set_target_properties(MyTarget PROPERTIES CXX_EXTENSIONS OFF)
# and when I used CMake to compile with gcc version 7, CMake didn't pass any
# flags to the compiler for the compiler standard at all.  Presumably this was
# because gcc7 compiler used as its default C++11, and CMake thus saw no reason
# to pass a flag for the standard.  But in fact gcc7 was defaulting to gnu++11,
# so effectively my request was ignored to have CXX_EXTENSIONS off.  I'm not
# sure if this remains a potential problem with the approach below.


# for testing, we can ensure the library compiles and runs correctly using
# standard C++11 (with the exception of BitpackedUintVector which needs C++14).
# Note that test_BitpackedUintVector.cpp will not be built/run when
# FORCE_TEST_HURCHALLA_CPP11_STANDARD is defined, since BitpackedUintVector
# needs C++14.

if(FORCE_TEST_HURCHALLA_CPP11_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



include(FetchGoogleTest.cmake)
include(EnableMaxWarnings.cmake)


# needed for gtest_discover_tests()
include(GoogleTest)



add_executable(test_hurchalla_util
               test_compiler_macros.cpp
               test_conditional_select.cpp
               test_count_leading_zeros.cpp
               test_count_trailing_zeros.cpp
               test_cselect_on_bit.cpp
               test_extensible_make_signed.cpp
               test_extensible_make_unsigned.cpp
               test_is_equality_comparable.cpp
               test_safely_promote_unsigned.cpp
               test_branchless_shifts.cpp
               test_signed_multiply_to_hilo_product.cpp
               test_sized_uint.cpp
               test_unreachable.cpp
               test_Unroll.cpp
               test_unsigned_multiply_to_hilo_product.cpp
               test_ut_numeric_limits.cpp)

EnableMaxWarnings(test_hurchalla_util)

set_target_properties(test_hurchalla_util
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_hurchalla_util
                      hurchalla_util
                      gtest_main)
#add_test(test_hurchalla_util  test_hurchalla_util)
gtest_discover_tests(test_hurchalla_util)



if(NOT FORCE_TEST_HURCHALLA_CPP11_STANDARD)
    add_executable(test_hurchalla_util_cpp14
                   test_BitpackedUintVector.cpp)

    EnableMaxWarnings(test_hurchalla_util_cpp14)

    set_target_properties(test_hurchalla_util_cpp14
                          PROPERTIES FOLDER "Tests")
    target_link_libraries(test_hurchalla_util_cpp14
                          hurchalla_util
                          gtest_main)
    #add_test(test_hurchalla_util_cpp14  test_hurchalla_util_cpp14)
    gtest_discover_tests(test_hurchalla_util_cpp14)
endif()



add_executable(test_programming_by_contract
               test_programming_by_contract.cpp)

EnableMaxWarnings(test_programming_by_contract)

set_target_properties(test_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_programming_by_contract
                      hurchalla_util
                      gtest_main)
#add_test(test_programming_by_contract  test_programming_by_contract)
gtest_discover_tests(test_programming_by_contract)



add_executable(test_inactive_programming_by_contract
               test_inactive_programming_by_contract.cpp)

EnableMaxWarnings(test_inactive_programming_by_contract)

set_target_properties(test_inactive_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_inactive_programming_by_contract
                      hurchalla_util
                      gtest_main)
#add_test(test_inactive_programming_by_contract
#          test_inactive_programming_by_contract)
gtest_discover_tests(test_inactive_programming_by_contract)
