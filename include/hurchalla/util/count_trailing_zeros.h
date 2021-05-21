// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_COUNT_TRAILING_ZEROS_H_INCLUDED
#define HURCHALLA_UTIL_COUNT_TRAILING_ZEROS_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#ifdef _MSC_VER
#  include <intrin.h>
#endif

namespace hurchalla {


// From the gcc docs:
// "Returns the number of trailing 0-bits in x, starting at the least
// significant bit position.  If x is 0, the result is undefined."
HURCHALLA_FORCE_INLINE int count_trailing_zeros(unsigned int x)
{
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, x);
    return static_cast<int>(index);
#else
    return __builtin_ctz(x);
#endif
}

// From the gcc docs:
// "Similar to __builtin_ctz, except the argument type is unsigned long."
HURCHALLA_FORCE_INLINE int count_trailing_zeros(unsigned long x)
{
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, x);
    return static_cast<int>(index);
#else
    return __builtin_ctzl(x);
#endif
}

// From the gcc docs:
// "Similar to __builtin_ctz, except the argument type is unsigned long long."
HURCHALLA_FORCE_INLINE int count_trailing_zeros(unsigned long long x)
{
#ifdef _MSC_VER
    static_assert(sizeof(unsigned long long) == sizeof(unsigned __int64), "");
    unsigned long index;
    _BitScanForward64(&index, x);
    return static_cast<int>(index);
#else
    return __builtin_ctzll(x);
#endif
}

// The following overloads don't have builtin equivalents, but we can just
// use the unsigned int versions
HURCHALLA_FORCE_INLINE int count_trailing_zeros(unsigned short x)
{
    return count_trailing_zeros(static_cast<unsigned int>(x));
}
HURCHALLA_FORCE_INLINE int count_trailing_zeros(unsigned char x)
{
    return count_trailing_zeros(static_cast<unsigned int>(x));
}


} // end namespace

#endif
