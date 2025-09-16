// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SHIFT_LEFT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SHIFT_LEFT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {



// primary template
template <typename T, class Enable = void>
struct impl_shift_left {
  // handles types T that are two times larger than the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);

    static_assert(is_valid_sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::value, "");
    using H = hurchalla::sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::type;

    constexpr unsigned int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    H ahi = static_cast<H>(a >> HALFBITS);
    H alo = static_cast<H>(a);
    static_cast<void>(ahi); // silence unused variable warnings
    static_cast<void>(alo); // silence unused variable warnings
    unsigned int ushift = static_cast<unsigned int>(shift);


#if defined(HURCHALLA_TARGET_ISA_X86_64)
# if defined(__GNUC__) && !defined(__clang__)   // gcc
    static_assert(ut_numeric_limits<T>::digits == 128, "");
    static_assert(ut_numeric_limits<H>::digits == 64, "");
    // gcc very often uses branches for the compiler generated shifts for
    // __uint128_t, which is terrible for perf.  We avoid this via inline asm.
    uint64_t zero = 0;
    __asm__ ("shldq %%cl, %[alo], %[ahi] \n\t"
             "shlq %%cl, %[alo] \n\t"
             "testb $64, %%cl \n\t"
             "cmovnzq %[alo], %[ahi] \n\t"
             "cmovnzq %[zero], %[alo] \n\t"
             : [alo]"+&r"(alo), [ahi]"+&r"(ahi)
             : [zero]"r"(zero), [ushift]"c"(ushift)
             : "cc");
    T result = (static_cast<T>(ahi) << 64) | alo;

# else  // clang, if T == __uint128_t, since MSVC doesn't support __uint128_t
    // Clang compiler's plain shift is fine, since clang never seems to use
    // branches for __uint128_t shifts.
    T result = a << ushift;
# endif

#elif defined(HURCHALLA_TARGET_ISA_ARM_64)
# if defined(__GNUC__) && !defined(__clang__)   // gcc
    // We use this code since it's almost as good as gcc's basic compiler
    // generated shift, and it prevents gcc's frequent use of branches in the
    // gcc compiler's normal shift when T == __uint128_t.
    H lo_shifted = alo << (ushift % HALFBITS);
    H mask = static_cast<H>(ushift >> 6) - 1u;
    H lo_result = lo_shifted & mask;

    H hi_shifted = ahi << (ushift % HALFBITS);
    //H cross_shifted = (alo >> ((HALFBITS - 1 - ushift) % HALFBITS)) >> 1;
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    unsigned int ushift_not = ~ushift;
    //H cross_shifted = (alo >> ((HALFBITS + ushift_not) % HALFBITS)) >> 1;
    H cross_shifted = (alo >> (ushift_not % HALFBITS)) >> 1;

    H hi_result = (lo_shifted & ~mask) | ((hi_shifted | cross_shifted) & mask);
    T result = (static_cast<T>(hi_result) << HALFBITS) | lo_result;
# else  // clang, if T == __uint128_t, since MSVC doesn't support __uint128_t
#  if 1
    // Clang compiler's plain shift is fine, since clang never seems to use
    // branches for __uint128_t shifts.
    T result = a << ushift;
#  else
    // For arm64 clang, the following produces the best asm, but a plain shift
    // is perfectly fine and much much simpler, so it's disabled (for now).
    // Prospectively we might want to use it if we detect that clang begins
    // producing branches for shifts, but so far clang hasn't done that when we
    // use T = __uint128_t.
    H lo_shifted = alo << (ushift % HALFBITS);
    H hi_shifted = ahi << (ushift % HALFBITS);

    // H cross_shifted = (alo >> ((HALFBITS - 1u - ushift) % HALFBITS)) >> 1;
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    unsigned int ushift_not = ~ushift;
    // H cross_shifted = (alo >> ((HALFBITS + ushift_not) % HALFBITS)) >> 1;
    H cross_shifted = (alo >> (ushift_not % HALFBITS)) >> 1;
    H hi_result = hi_shifted | cross_shifted;

    H lo_result = (ushift & HALFBITS) ? 0 : lo_shifted;
    hi_result = (ushift & HALFBITS) ? lo_shifted : hi_result;

    T result = (static_cast<T>(hi_result) << HALFBITS) | lo_result;
#  endif
# endif

#else   // RISC-V, PPC, etc
    T result = a << ushift;
#endif

    return result;
  }
};


template <typename T>
struct impl_shift_left<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);

    return static_cast<T>(a << shift);
  }
};


}} // end namespace

#endif
