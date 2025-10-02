// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED

// note: when using g++ (gcc) compiler on x64 or ARM64, in order to be certain
// of getting branchless machine code (via inline asm), you must define
// HURCHALLA_ALLOW_INLINE_ASM_BRANCHLESS_SHIFTS.  Clang seems to have no need
// for inline asm and thus no need for this macro, since it always seems to be
// branchless.  MSVC doesn't support inline asm but it's unlikely it would need
// it - MSVC will very likely always be branchless for its supported types since
// it doesn't support __uint128_t.


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

    HPBC_UTIL_API_PRECONDITION(shift >= 0);
    HPBC_UTIL_API_PRECONDITION(shift < ut_numeric_limits<T>::digits);

#if defined(HURCHALLA_ALLOW_INLINE_ASM_BRANCHLESS_SHIFTS)
    T result = detail::impl_branchless_shift_right<T>::call_asm(a, shift);
#else
    T result = detail::impl_branchless_shift_right<T>::call(a, shift);
#endif

    HPBC_UTIL_POSTCONDITION(result == static_cast<T>(a >> shift));
    return result;
}


} // end namespace

#endif
