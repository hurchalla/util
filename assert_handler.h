/* --- This file is distributed under the MIT Open Source License,
   as detailed in "_LICENSE (MIT).TXT" in this directory --- */

#ifndef HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED
#define HURCHALLA_PBC_ASSERT_HANDLER_H_INCLUDED

/* You should compile and link either assert_handler.cpp or assert_handler.c
   (not both!) */
   
   
/* To enable a project containing both C and C++ code to use one single assert
handler, you can surround these function declarations using the typical
#ifdef __cplusplus and extern "C" trick.  However, this trick is not a general
solution for all needs - it restricts what your pbcAssertHandler can do; marking
these functions as extern C will result in undefined behavior if
pbcAssertHandler throws, even if all your code is C++. That's fine for mixed
C/C++ code, since presumably in that case you would never write a handler that
throws. It just isn't a general solution.
*/
void pbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line);
int pbcGetHandlerPreconditionAssertLevel();
int pbcGetHandlerAssertLevel();


#endif
