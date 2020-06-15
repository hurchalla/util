# --- This file is distributed under the MIT Open Source License, as detailed
# in the file "LICENSE.TXT" in the root of this repository ---

if(TARGET test_programming_by_contract)
    return()
endif()

cmake_minimum_required(VERSION 3.13)
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

# Set compiler warnings.  We want our library headers to compiler cleanly
# with as many warnings as possible, so that we know users will not get
# compiler warnings from our headers.
if(MSVC)
    target_compile_options(test_programming_by_contract PRIVATE /W4 /WX)
else()
    target_compile_options(test_programming_by_contract PRIVATE -Werror
          -Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -Wcast-qual
          -Wmissing-include-dirs -Wredundant-decls -Wnon-virtual-dtor
          -Wstrict-overflow=4 -Wconversion -Wsign-conversion -Wfloat-equal
          -Winvalid-pch -Wstack-protector -Wpacked -Wnull-dereference
          -Wuninitialized -Wregister -Wunused -Wunused-parameter -Wformat=2
          -Wdisabled-optimization -Wvla -Wold-style-cast -Wdouble-promotion
          -Winline -Wundef -Wformat-nonliteral -Wmissing-noreturn
          -Wzero-as-null-pointer-constant -Weffc++ -Wctor-dtor-privacy
          -Wmissing-declarations)
    if((NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")) OR
               (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.1))
        target_compile_options(test_programming_by_contract PRIVATE
                -Wextra-semi)
    endif()

    # additional compiler specific warnings
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(test_programming_by_contract PRIVATE
                -ferror-limit=3  -Wcast-align -Wmismatched-tags)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(test_programming_by_contract PRIVATE
                -fmax-errors=3 -Wlogical-op -Wnoexcept
                -Wduplicated-cond -Wduplicated-branches
                -Wstringop-overflow=3 -Wsuggest-final-methods -Wplacement-new=2
                -Wsuggest-final-types -Wsuggest-override -Walloc-zero
                -Wnormalized -Wformat-truncation=2 -Wformat-overflow=2
                -Wformat-signedness -Wstrict-null-sentinel -Wopenmp-simd
                -Wunsafe-loop-optimizations -Wvector-operation-performance)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.1)
            target_compile_options(test_programming_by_contract PRIVATE
                               -Wcast-align=strict -Wcomma-subscript -Wvolatile)
        else()
            target_compile_options(test_programming_by_contract PRIVATE
                               -Wcast-align)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        # TODO: investigate icc warnings
    endif()
endif()

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

# Set compiler warnings.  We want our library headers to compiler cleanly
# with as many warnings as possible, so that we know users will not get
# compiler warnings from our headers.
if(MSVC)
    target_compile_options(test_ndebug_programming_by_contract PRIVATE /W4 /WX)
else()
    target_compile_options(test_ndebug_programming_by_contract PRIVATE -Werror
          -Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -Wcast-qual
          -Wmissing-include-dirs -Wredundant-decls -Wnon-virtual-dtor
          -Wstrict-overflow=4 -Wconversion -Wsign-conversion -Wfloat-equal
          -Winvalid-pch -Wstack-protector -Wpacked -Wnull-dereference
          -Wuninitialized -Wregister -Wunused -Wunused-parameter -Wformat=2
          -Wdisabled-optimization -Wvla -Wold-style-cast -Wdouble-promotion
          -Winline -Wundef -Wformat-nonliteral -Wmissing-noreturn
          -Wzero-as-null-pointer-constant -Weffc++ -Wctor-dtor-privacy
          -Wmissing-declarations)
    if((NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")) OR
               (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.1))
        target_compile_options(test_ndebug_programming_by_contract PRIVATE
                -Wextra-semi)
    endif()

    # additional compiler specific warnings
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(test_ndebug_programming_by_contract PRIVATE
                -ferror-limit=3  -Wcast-align -Wmismatched-tags)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(test_ndebug_programming_by_contract PRIVATE
                -fmax-errors=3 -Wlogical-op -Wnoexcept
                -Wduplicated-cond -Wduplicated-branches
                -Wstringop-overflow=3 -Wsuggest-final-methods -Wplacement-new=2
                -Wsuggest-final-types -Wsuggest-override -Walloc-zero
                -Wnormalized -Wformat-truncation=2 -Wformat-overflow=2
                -Wformat-signedness -Wstrict-null-sentinel -Wopenmp-simd
                -Wunsafe-loop-optimizations -Wvector-operation-performance)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.1)
            target_compile_options(test_ndebug_programming_by_contract PRIVATE
                               -Wcast-align=strict -Wcomma-subscript -Wvolatile)
        else()
            target_compile_options(test_ndebug_programming_by_contract PRIVATE
                               -Wcast-align)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        # TODO: investigate icc warnings
    endif()
endif()

set_target_properties(test_ndebug_programming_by_contract
                      PROPERTIES FOLDER "Tests")
target_link_libraries(test_ndebug_programming_by_contract
                      programming_by_contract
                      gtest_main)
#add_test(test_ndebug_programming_by_contract
#          test_ndebug_programming_by_contract)
gtest_discover_tests(test_ndebug_programming_by_contract)
