# --- This file is distributed under the MIT Open Source License, as detailed
# in the file "LICENSE.TXT" in the root of this repository ---

if(TARGET programming_by_contract)
    return()
endif()

cmake_minimum_required(VERSION 3.12)
project(programming_by_contract VERSION 1.0.0 LANGUAGES C CXX)

# if this is the top level CMakeLists.txt, let IDEs group projects into folders.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

option(TEST_HURCHALLA_LIBS
       "Build tests for all Hurchalla library projects."
       OFF)

# if this is the top level CMakeLists.txt, add testing options, and enable
# testing when testing options have been set to ON.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(TEST_HURCHALLA_TOP_LEVEL
           "Build tests for the top level Hurchalla library project."
           OFF)
    if(TEST_HURCHALLA_TOP_LEVEL OR TEST_HURCHALLA_LIBS)
        enable_testing()
        #include(CTest)
    endif()
elseif(TEST_HURCHALLA_LIBS)
    # If TEST_HURCHALLA_LIBS is set to ON, enable_testing() should have been
    # called either directly or indirectly by the top level project. (Note that
    # if a project calls include(CTest), the included CTest.cmake defines a
    # BUILT_TESTING option and calls enable_testing if BUILD_TESTING is ON.)
    if (NOT CMAKE_TESTING_ENABLED)
        message(FATAL_ERROR "Fatal error: TEST_HURCHALLA_LIBS was set, but enable_testing() was never called")
    endif()
endif()



set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/programming_by_contract/assert_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/programming_by_contract/programming_by_contract.h
    )

add_library(programming_by_contract INTERFACE)
target_sources(programming_by_contract INTERFACE $<BUILD_INTERFACE:${header_files}>)
target_include_directories(programming_by_contract INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(programming_by_contract SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)


# Check for required C++ features only if the C++ compiler ID is non-empty.
target_compile_features(programming_by_contract INTERFACE 
          $<$<NOT:$<CXX_COMPILER_ID:>>:
               cxx_variadic_macros
               cxx_static_assert
               cxx_decltype
               cxx_deleted_functions
           >)

target_compile_features(programming_by_contract INTERFACE
          $<$<NOT:$<C_COMPILER_ID:>>:
               c_variadic_macros
           >)
# MSVC doesn't provide _Static_assert for C, but we don't want this to cause
# CMake to abort in the very common scenario that MSVC is being used only for
# C++ but the parent CMake project() doesn't specify a language (thereby
# defaulting to C++ and C). Therefore I only check here for the c_static_assert
# feature if both the C compiler ID is non-empty and the ID is non-MSVC.
# Note: If you are using MSVC and you indeed need to use C, then you'll need to
# emulate _Static_assert in
# include/hurchalla/programming_by_contract/programming_by_contract.h (see the
# comment there on _Static_assert). Alternatively, you could remove the macros
# in programming_by_contract.h that use _Static_assert. Either of these
# solutions would allow your C files to compile with MSVC.
target_compile_features(programming_by_contract INTERFACE
          $<$<AND:$<NOT:$<C_COMPILER_ID:>>,$<NOT:$<C_COMPILER_ID:MSVC>>>:
               c_static_assert
           >)



#You can turn the following option ON/OFF with cmake by using the -D flag. Example:
#cmake -DPBC_WRAP_STDLIB_ASSERT=ON ..
option(PBC_WRAP_STDLIB_ASSERT "Option PBC_WRAP_STDLIB_ASSERT: If you set PBC_WRAP_STDLIB_ASSERT=ON (you can do this via command line 'cmake -DPBC_WRAP_STDLIB_ASSERT=ON ...'), all the programming by contract assertion macros will use the standard library assert() and will always be active when NDEBUG is undefined and will always be inactive when NDEBUG is defined.  Setting this option to ON effectively reduces much of the programming by contract functionality, but it means you (or clients) will not need to provide or link any custom assert handler." OFF)

if(PBC_WRAP_STDLIB_ASSERT)
    target_compile_definitions(programming_by_contract INTERFACE
              PBC_WRAP_STDLIB_ASSERT
              )
endif()



# ***Tests***

# if this is the top level CMakeLists.txt
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(TEST_HURCHALLA_TOP_LEVEL OR TEST_HURCHALLA_LIBS)
        add_subdirectory(test)
    endif()
elseif(TEST_HURCHALLA_LIBS)
    add_subdirectory(test)
endif()

