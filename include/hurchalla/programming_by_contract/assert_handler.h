/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */

#ifndef HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED
#define HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED

/* If you have defined HPBC_ENABLE_FULL_FEATURES for your main application or
   for any libraries you link to (that use programming by contract), then in
   your main application project you must compile an assert handler file that
   implements the functions declared here. Your assert handler file does not
   belong in this programming_by_contract project; it belongs in your project(s)
   that contain main(). Normally, you can just file copy one of the assert
   handlers in the example_assert_handlers folder: either assert_handler_cpp.cpp
   or assert_handler_c.c (not both!)
   If you do not define HPBC_ENABLE_FULL_FEATURES for your main application or
   any libraries you link to (that use programming by contract), then you don't
   need to provide an assert handler file.  However, the programming by contract
   features will be rather limited in that case, because all assertion macros
   will be simply mapped to the standard library assert(), and the named levels
   in all the assertion macros will also be disregarded. */

/* To enable a project containing both C and C++ code to use one single assert
handler, you can uncomment the #define HPBC_SUPPORT_MIXED_C_AND_CPP below.
Note that HPBC_SUPPORT_MIXED_C_AND_CPP is not enabled by default. This is
because it is possible (if unlikely) that a project consisting entirely of C++
files may be prevented by the extern "C" from inlining
pbcGetHandlerPreconditionAssertLevel() and pbcGetHandlerAssertLevel() during
link time optimization. It is not a problem with clang, gcc, or msvc, and is
unlikely to be a problem for other compilers. However, there is no standard for
link time optimization, and so there is no way to guarantee for every possible
C++ compiler/linker that extern "C" will allow inlining during link time
optimization.
*/

/*
#define HPBC_SUPPORT_MIXED_C_AND_CPP 1
*/


#if defined(__cplusplus) && defined(HPBC_SUPPORT_MIXED_C_AND_CPP)
extern "C" {
#endif

void hpbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line);
int hpbcGetHandlerPreconditionAssertLevel();
int hpbcGetHandlerAssertLevel();

#if defined(__cplusplus) && defined(HPBC_SUPPORT_MIXED_C_AND_CPP)
}
#endif


#endif
