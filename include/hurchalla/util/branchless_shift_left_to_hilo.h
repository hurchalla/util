// Copyright (c) 2026 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BRANCHLESS_SHIFT_LEFT_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_BRANCHLESS_SHIFT_LEFT_TO_HILO_H_INCLUDED

// note: when using g++ (gcc) compiler on x64 or ARM64, in order to be certain
// of getting branchless machine code (via inline asm), you must define
// HURCHALLA_ALLOW_INLINE_ASM_BRANCHLESS_SHIFTS or
// HURCHALLA_ALLOW_INLINE_ASM_ALL.
// Clang seems to have no need for inline asm and thus no need for this macro,
// since it always seems to be branchless.  MSVC doesn't support inline asm but
// it's unlikely it would need it - MSVC will very likely always be branchless
// for its supported types since it doesn't support __uint128_t.


#include "hurchalla/util/detail/platform_specific/impl_branchless_shift_left_to_hilo.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// Left shift of unsigned integer type T, to a double wide result.  Returns the
// high portion of the result, and copies the low portion to lowResult.
// Intended to compile to branchless machine code (see note above).
template <typename T>
HURCHALLA_FORCE_INLINE
T branchless_shift_left_to_hilo(T& lowResult, T a, int shift)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static constexpr int digitsT = ut_numeric_limits<T>::digits;
    static_assert(digitsT <= 2 * HURCHALLA_TARGET_BIT_WIDTH, "");

    HPBC_UTIL_API_PRECONDITION(shift >= 0);
    HPBC_UTIL_API_PRECONDITION(shift < ut_numeric_limits<T>::digits);

#if defined(HURCHALLA_ALLOW_INLINE_ASM_BRANCHLESS_SHIFTS) || \
    defined(HURCHALLA_ALLOW_INLINE_ASM_ALL)
    T highResult = detail::impl_branchless_shift_left_to_hilo<T>::call_asm(lowResult, a, shift);
#else
    T highResult = detail::impl_branchless_shift_left_to_hilo<T>::call(lowResult, a, shift);
#endif

    HPBC_UTIL_POSTCONDITION(lowResult == static_cast<T>(a << shift));
    HPBC_UTIL_POSTCONDITION(highResult == static_cast<T>((a >> (digitsT - shift - 1)) >> 1));
    return highResult;
}


} // end namespace

#endif
