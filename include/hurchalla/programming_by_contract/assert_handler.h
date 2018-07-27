/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */

#ifndef HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED
#define HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED

/* You should compile and link either assert_handler.cpp or assert_handler.c
   (not both!) */

/* To enable a project containing both C and C++ code to use one single assert
handler, you can uncomment the #define SUPPORT_MIXED_C_AND_CPP below.
Note that SUPPORT_MIXED_C_AND_CPP is not enabled by default. This is because it
is possible (if unlikely) that a project consisting entirely of C++ files may be
prevented by the extern "C" from inlining pbcGetHandlerPreconditionAssertLevel()
and pbcGetHandlerAssertLevel() during link time optimization. It is not a
problem with clang, gcc, or msvc, and is unlikely to be a problem for other
compilers. However, there is no standard for link time optimization, and so
there is no way to guarantee for every possible C++ compiler/linker that extern
"C" will allow inlining during link time optimization.
*/

/*
#define SUPPORT_MIXED_C_AND_CPP 1
*/


#if defined(__cplusplus) && defined(SUPPORT_MIXED_C_AND_CPP)
extern "C" {
#endif

void pbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line);
int pbcGetHandlerPreconditionAssertLevel();
int pbcGetHandlerAssertLevel();

#if defined(__cplusplus) && defined(SUPPORT_MIXED_C_AND_CPP)
}
#endif


#endif
