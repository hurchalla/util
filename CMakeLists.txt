if(TARGET programming_by_contract)
    return()
endif()

cmake_minimum_required(VERSION 3.12)
project(programming_by_contract VERSION 1.0.0 LANGUAGES C CXX)


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

