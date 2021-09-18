# --- This file is distributed under the MIT Open Source License, as detailed
# in the file "LICENSE.TXT" in the root of this repository ---

if(TARGET hurchalla_util)
    return()
endif()

cmake_minimum_required(VERSION 3.14)
project(hurchalla_util VERSION 1.0.0 LANGUAGES C CXX)

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


add_library(hurchalla_util INTERFACE)

target_sources(hurchalla_util INTERFACE
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/assert_handler.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/compiler_macros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/count_trailing_zeros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/programming_by_contract.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/sized_uint.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/unreachable.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/Unroll.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/unsigned_multiply_to_hilo_product.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/extensible_make_signed.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/extensible_make_unsigned.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/safely_promote_unsigned.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/ut_numeric_limits.h>
               )


install(DIRECTORY
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    DESTINATION include)


target_include_directories(hurchalla_util
               INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(hurchalla_util
               INTERFACE $<INSTALL_INTERFACE:include>)
#use this instead?
#target_include_directories(hurchalla_util SYSTEM
#               INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)


# Check for required C++ features only if the C++ compiler ID is non-empty.
target_compile_features(hurchalla_util INTERFACE
          $<$<NOT:$<CXX_COMPILER_ID:>>:
               cxx_variadic_macros
               cxx_decltype
               cxx_deleted_functions
           >)

target_compile_features(hurchalla_util INTERFACE
          $<$<NOT:$<C_COMPILER_ID:>>:
               c_variadic_macros
           >)


# You can turn the following option ON/OFF with cmake by using the -D flag.
# Example: cmake -DHPBC_ENABLE_FULL_FEATURES=ON ..
option(HPBC_ENABLE_FULL_FEATURES
       "Option HPBC_ENABLE_FULL_FEATURES: If you set HPBC_ENABLE_FULL_FEATURES=ON (you can do this via command line 'cmake -DHPBC_ENABLE_FULL_FEATURES=ON ...'), all the programming by contract assertion macros will be fully functional, and you will need to provide a custom assert handler function.  When this option is not set to ON, all the macros are just remapped to the standard library assert(), and no custom assert handler is necessary.  This effectively reduces much of the programming by contract functionality, but it is appealingly simple, and it allows someone to link to your code without knowing anything about programming by contract (or assert handlers)."
       OFF)

if(HPBC_ENABLE_FULL_FEATURES)
    target_compile_definitions(hurchalla_util INTERFACE
              HPBC_ENABLE_FULL_FEATURES
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

