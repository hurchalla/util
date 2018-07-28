cmake_minimum_required(VERSION 3.12)
project(programming_by_contract VERSION 1.0.0 LANGUAGES C CXX)

if(TARGET programming_by_contract)
    return()
endif()


set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/programming_by_contract/assert_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hurchalla/programming_by_contract/programming_by_contract.h
    )

add_library(programming_by_contract INTERFACE)
target_sources(programming_by_contract INTERFACE $<BUILD_INTERFACE:${header_files}>)
target_include_directories(programming_by_contract INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(programming_by_contract SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)


get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if("CXX" IN_LIST languages)
    target_compile_features(programming_by_contract INTERFACE 
              cxx_variadic_macros
              cxx_static_assert
              cxx_decltype
              cxx_deleted_functions
              )
endif()
if("C" IN_LIST languages)
    if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
         # MSVC doesn't provide _Static_assert for C, but I don't want this to
         # cause CMake to abort in the very common scenario that MSVC is being
         # used only for C++ but the parent CMake project() doesn't specify a
         # language (thereby defaulting to C++ and C).
         # If you indeed need to use C (and thus _Static_assert), then see the
         # FATAL_ERROR message below for info on how to solve the problem that
         # _Static_assert is unsupported by MSVC.

         if(NOT "c_static_assert" IN_LIST CMAKE_C_COMPILE_FEATURES)
              message(FATAL_ERROR "
Error: _Static_assert is not known to your C compiler (${CMAKE_C_COMPILER_ID}).
Note: You can solve this problem by emulating _Static_assert (see the comment on _Static_assert in include/hurchalla/programming_by_contract/programming_by_contract.h).  Alternatively, you could remove the macros in programming_by_contract.h that use _Static_assert.  Either of these solutions will allow you to remove this FATAL_ERROR message from this CMakeLists.txt.
")
         endif()
    endif()
    # the FATAL_ERROR message above handles when the target_compile_feature
    # c_static_assert is missing
    target_compile_features(programming_by_contract INTERFACE
              c_variadic_macros
              )
endif()



if(NOT DEFINED default_value_pbc_wrap_stdlib_assert)
    set(default_value_pbc_wrap_stdlib_assert OFF)
endif()
#You can turn the following option ON/OFF with cmake by using the -D flag. Example:
#cmake -DPBC_WRAP_STDLIB_ASSERT=ON ..
option(PBC_WRAP_STDLIB_ASSERT "Option PBC_WRAP_STDLIB_ASSERT: If you set PBC_WRAP_STDLIB_ASSERT=ON (you can do this via command line 'cmake -DPBC_WRAP_STDLIB_ASSERT=ON ...'), all the programming by contract assertion macros will use the standard library assert() and will always be active when NDEBUG is undefined and will always be inactive when NDEBUG is defined.  Setting this option to ON effectively reduces much of the programming by contract functionality, but it means you (or clients) will not need to provide or link any custom assert handler." ${default_value_pbc_wrap_stdlib_assert})

if(PBC_WRAP_STDLIB_ASSERT)
    target_compile_definitions(programming_by_contract INTERFACE
              PBC_WRAP_STDLIB_ASSERT
              )
endif()

