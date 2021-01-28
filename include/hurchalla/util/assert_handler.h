/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of this repository --- */

#ifndef HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED
#define HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED

/* This file is intended solely to be used in conjunction with and in support of
   programming_by_contract.h (it's where the macros discussed here, such as
   HPBC_ENABLE_FULL_FEATURES, are checked). */

/* If you define the macro HPBC_ENABLE_FULL_FEATURES while compiling your C++ or
   C files, or if any libraries you link to defined this macro while compiling
   (presumably because your organization has chosen across multiple projects to
   use programming by contract with full features enabled), then in your main
   application project you must compile an assert handler file that implements
   the functions declared here. Your assert handler file that you must compile
   belongs in your project that contains main(); it does not belong in this util
   module.
   Normally, an easy way you can do this is to just file copy one of the assert
   handlers in this repository's example_assert_handlers folder: you can use
   either assert_handler_cpp.cpp or assert_handler_c.c (not both!)
   If you do not define HPBC_ENABLE_FULL_FEATURES and none of the libraries you
   link to define it, then you don't need to provide an assert handler file.
   However, the programming by contract features will be limited in that case,
   because all assertion macros will be simply mapped to the standard library
   assert(), and the named levels in all the assertion macros will all be
   disregarded. */

/* To enable a project containing both C and C++ code to use one single assert
   handler, you can uncomment the #define HPBC_SUPPORT_MIXED_C_AND_CPP below.
   Note that HPBC_SUPPORT_MIXED_C_AND_CPP is not enabled by default. This is
   because it is possible (if unlikely) that a project consisting entirely of
   C++ files may be prevented by the extern "C" from inlining
   pbcGetHandlerPreconditionAssertLevel() and pbcGetHandlerAssertLevel() during
   link time optimization. It is not a problem with clang, gcc, or msvc, and is
   unlikely to be a problem for other compilers. However, there is no standard
   for link time optimization, and so there is no way to guarantee for every
   possible C++ compiler/linker that extern "C" will allow inlining during link
   time optimization.
*/

/*
#define HPBC_SUPPORT_MIXED_C_AND_CPP 1
*/

/* These functions are purposefully declared in the global namespace (similarly
   to how the HPBC macros necessarily exist at a global level).  This is done
   to be compatibile with both C and C++. */


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
