
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


# The following ideas come from
#   https://github.com/google/googletest/blob/master/googletest/README.md
# They are further explained at https://crascit.com/2015/07/25/cmake-gtest/ and
# https://github.com/Crascit/DownloadProject/blob/master/DownloadProject.cmake
if (NOT TARGET gtest_main)
   # Download and unpack googletest at configure time
   configure_file(CMakeLists.txt.in
                  "${CMAKE_BINARY_DIR}/googletest/download/CMakeLists.txt")
   
   execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
     RESULT_VARIABLE result
     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest/download")
   if(result)
     message(FATAL_ERROR "CMake step for googletest failed: ${result}")
   endif()
   
   execute_process(COMMAND ${CMAKE_COMMAND} --build .
     RESULT_VARIABLE result
     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest/download")
   if(result)
     message(FATAL_ERROR "Build step for googletest failed: ${result}")
   endif()
   
   # Prevent overriding the parent project's compiler/linker settings on Windows
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
   
   # Add googletest to our build. This defines gtest and gtest_main targets.
   add_subdirectory("${CMAKE_BINARY_DIR}/googletest/src"
                    "${CMAKE_BINARY_DIR}/googletest/build"
                    EXCLUDE_FROM_ALL)
endif()



# needed for gtest_discover_tests()
include(GoogleTest)


add_executable(test_programming_by_contract
               test_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h
               )
target_link_libraries(test_programming_by_contract
                      programming_by_contract
                      gtest_main)
#add_test(test_programming_by_contract  test_programming_by_contract)
gtest_discover_tests(test_programming_by_contract)


add_executable(test_ndebug_programming_by_contract
               test_ndebug_programming_by_contract.cpp
               testhelper_assert_handler.cpp
               testhelper_assert_handler.h
               )
target_link_libraries(test_ndebug_programming_by_contract
                      programming_by_contract
                      gtest_main)
#add_test(test_ndebug_programming_by_contract
#          test_ndebug_programming_by_contract)
gtest_discover_tests(test_ndebug_programming_by_contract)

