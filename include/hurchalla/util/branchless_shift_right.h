// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_branchless_shift_right.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// Logical right shift of unsigned integer type T.  Compiles with no
// conditional branches.
template <typename T>
HURCHALLA_FORCE_INLINE
T branchless_shift_right(T a, int shift)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static_assert(ut_numeric_limits<T>::digits <= 2 * HURCHALLA_TARGET_BIT_WIDTH, "");

    HPBC_UTIL_PRECONDITION2(shift >= 0);
    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);

    return detail::impl_branchless_shift_right<T>::call(a, shift);
}


} // end namespace

#endif
