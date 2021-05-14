// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_UNREACHABLE_H_INCLUDED
#define HURCHALLA_UTIL_UNREACHABLE_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"

namespace hurchalla {


// For details on the idea of unreachable(), see
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0627r3.pdf
// See also https://stackoverflow.com/questions/60802864/emulating-gccs-builtin-unreachable-in-visual-studio
[[ noreturn ]] HURCHALLA_FORCE_INLINE void unreachable()
{
#ifdef _MSC_VER
    __assume(false);         // visual c++
#elif defined(__GNUC__)
    __builtin_unreachable();  // gcc, clang, icc
#endif
}


} // end namespace

#endif
