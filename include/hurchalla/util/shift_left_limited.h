// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_SHIFT_LEFT_LIMITED_H_INCLUDED
#define HURCHALLA_UTIL_SHIFT_LEFT_LIMITED_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_shift_left_limited.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// The best performing version of left shift (much than operator "<<"), when
// the type T is larger than the native register size, and the shift amount is
// less than the native register bit width.  Performance is the same otherwise.
// Requires as a precondition that shift < HURCHALLA_TARGET_BIT_WIDTH.
template <typename T>
HURCHALLA_FORCE_INLINE
T shift_left_limited(T a, int shift)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static_assert(ut_numeric_limits<T>::digits <= 2 * HURCHALLA_TARGET_BIT_WIDTH, "");

    HPBC_UTIL_PRECONDITION2(shift >= 0);
    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);
    HPBC_UTIL_PRECONDITION2(shift < HURCHALLA_TARGET_BIT_WIDTH);

    return detail::impl_shift_left_limited<T>::call(a, shift);
}


} // end namespace

#endif
