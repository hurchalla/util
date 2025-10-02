// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BRANCHLESS_SMALL_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_BRANCHLESS_SMALL_SHIFT_RIGHT_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_branchless_small_shift_right.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// Logical right shift of unsigned integer type T, by a "small" amount.
// Compiles with no conditional branches, optimized for a small shift.
// A "small" shift of a type T integer must satisfy the following:
//     0 <= shift < bitsT    (and)
//     shift < HURCHALLA_TARGET_BIT_WIDTH
template <typename T>
HURCHALLA_FORCE_INLINE
T branchless_small_shift_right(T a, int shift)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static_assert(ut_numeric_limits<T>::digits <= 2 * HURCHALLA_TARGET_BIT_WIDTH, "");

    HPBC_UTIL_API_PRECONDITION(0 <= shift && shift < ut_numeric_limits<T>::digits);
    HPBC_UTIL_API_PRECONDITION(shift < HURCHALLA_TARGET_BIT_WIDTH);

    T result = detail::impl_branchless_small_shift_right<T>::call(a, shift);

    HPBC_UTIL_POSTCONDITION(result == static_cast<T>(a >> shift));
    return result;
}


} // end namespace

#endif
