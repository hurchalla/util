// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BRANCHLESS_LARGE_SHIFT_LEFT_H_INCLUDED
#define HURCHALLA_UTIL_BRANCHLESS_LARGE_SHIFT_LEFT_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_branchless_large_shift_left.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// Left shift of unsigned integer type T, by a "large" amount.  Compiles with no
// conditional branches, optimized for a large shift.
// A "large" shift of a type T integer must satisfy the following:
//     0 <= shift   (and)
//     bitsT - HURCHALLA_TARGET_BIT_WIDTH <= shift < bitsT
template <typename T>
HURCHALLA_FORCE_INLINE
T branchless_large_shift_left(T a, int shift)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static_assert(ut_numeric_limits<T>::digits <= 2 * HURCHALLA_TARGET_BIT_WIDTH, "");

    constexpr int bitsT = ut_numeric_limits<T>::digits;
    HPBC_UTIL_PRECONDITION2(0 <= shift);
    HPBC_UTIL_PRECONDITION2(bitsT - static_cast<int>(HURCHALLA_TARGET_BIT_WIDTH) <= shift);
    HPBC_UTIL_PRECONDITION2(shift < bitsT);

    T result = detail::impl_branchless_large_shift_left<T>::call(a, shift);

    HPBC_UTIL_POSTCONDITION2(result == static_cast<T>(a << shift));
    return result;
}


} // end namespace

#endif
