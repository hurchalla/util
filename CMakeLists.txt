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
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
         # MSVC doesn't provide _Static_assert for C, but I don't want this to
         # cause CMake to abort in the very common scenario that MSVC is being
         # used only for C++ but the parent CMake project() doesn't specify a
         # language (thereby defaulting to C++ and C).
         target_compile_features(programming_by_contract INTERFACE
                   c_variadic_macros
#                   c_static_assert
                   )
    else()
         target_compile_features(programming_by_contract INTERFACE
                   c_variadic_macros
                   c_static_assert
                   )
    endif()
    # For C, _Static_assert can be emulated - see the comment in
    # include/hurchalla/programming_by_contract/programming_by_contract.h
    # Alternatively, you could just remove the macros in programming_by_contract.h
    # that use _Static_assert, which would in turn let you remove the
    # target_compile_features requirement on c_static_assert above.
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

