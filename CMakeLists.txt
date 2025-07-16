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
    option(TEST_HURCHALLA_UTIL
           "Build tests for the Hurchalla util library project."
           ON)
    if(TEST_HURCHALLA_UTIL OR TEST_HURCHALLA_LIBS)
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
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/BitpackedUintVector.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/compiler_macros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/conditional_select.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/count_leading_zeros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/count_trailing_zeros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/programming_by_contract.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/signed_multiply_to_hilo_product.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/sized_uint.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/unreachable.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/Unroll.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/unsigned_multiply_to_hilo_product.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/ImplBitpackedUintVector.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/impl_conditional_select.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/impl_count_leading_zeros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/impl_count_trailing_zeros.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/platform_specific/impl_signed_multiply_to_hilo_product.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/detail/platform_specific/impl_unsigned_multiply_to_hilo_product.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/extensible_make_signed.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/extensible_make_unsigned.h>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/util/traits/is_equality_comparable.h>
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



#   This CMakeLists.txt specifies very little for the C++ standard, and will
# thus mostly compile using whatever standard has been set (or has been
# defaulted) by a CMakeLists.txt that is using/consuming the library.
#   We limit our checking for C++11 to a few features that we use (see below).
# In principle it would be nice for this CMakeLists.txt to specify that it needs
# at least C++11, and check for it... but to be safe I want to avoid giving
# CMake extra temptation to set the C++ standard.  An example of the danger - we
# don't want one library's target to get compiled with -std=c++11 due to its
# CMakeLists.txt specifying C++11, and then another different library's target
# to get compiled with -std=c++17 due to its CMakeLists.txt specifying C++17,
# and then for those two different object files to get linked together.  Perhaps
# CMake carefully ensures that mixing standards never happens, but maybe not.
# We don't have a strong reason to check for C++11 beyond the feature checks
# below; it's unlikely any compiler in this day wouldn't support C++11, and if
# not, there would be fairly obvious compile errors.  In any event, ABI
# compatibility is a pain, with few guarantees.  See
# https://stackoverflow.com/questions/46746878/is-it-safe-to-link-c17-c14-and-c11-objects
# For info on how/why mixing the standard could potentially result in errors:
# https://github.com/abseil/abseil-cpp/issues/259
# https://cgold.readthedocs.io/en/latest/tutorials/toolchain/globals/cxx-standard.html
# https://stackoverflow.com/questions/10717106/can-different-gcc-dialects-be-linked-together
# https://gcc.gnu.org/wiki/Cxx11AbiCompatibility
# https://cullmann.io/posts/cpp-standard-version-mix-up/

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
    if(TEST_HURCHALLA_UTIL OR TEST_HURCHALLA_LIBS)
        add_subdirectory(test)
    endif()
elseif(TEST_HURCHALLA_LIBS)
    add_subdirectory(test)
endif()

