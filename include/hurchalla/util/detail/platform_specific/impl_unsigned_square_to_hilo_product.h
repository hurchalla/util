// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED


#include "hurchalla/util/unsigned_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include "hurchalla/util/compiler_macros.h"
#include <cstdint>

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_unsigned_square_to_hilo_product {
  HURCHALLA_FORCE_INLINE static T call(T& lowProduct, T u)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    return unsigned_multiply_to_hilo_product(lowProduct, u, u);
    // for benchmarking __uint128_t, we should use the following instead:
//    return slow_unsigned_multiply_to_hilo_product::call(lowProduct, u, u);
  }
};



// Inline asm summary/conclusion from these ARM64 timings - for ARM64 we should
// enable the all-asm version, for both gcc and clang.  A side benefit is the
// compiler (usually gcc) is less likely to have the bad luck cases where it
// makes bad decisions and produces terrible machine code, if we use all-asm.
//
// -- Benchmark Timings --
//
// Montquarter two pow scalar:
// gcc: no-asm 2.4842  partial-asm 2.2790  all-asm 2.2798
// gcc with all-asm mult: no-asm 2.4823  partial-asm 2.2819  *all-asm 2.2792
// clang: no-asm 2.2705  partial-asm 2.3196  all-asm 2.2781
// clang with all-asm mult: no-asm 2.2957  partial-asm 2.3362  *all-asm 2.2633
//
// Montfull two pow scalar:
// (OLD bit still good) gcc: no-asm ---   partial-asm 2.4030  all-asm 2.3789
// gcc: no-asm 2.4053   partial-asm 2.4028  all-asm 2.3802
// gcc with all-asm mult: no-asm 2.4019  partial-asm 2.4048  *all-asm 2.3789
// clang: no-asm 2.3779  partial-asm 2.3864  all-asm 2.3960
// clang with all-asm mult: no-asm 2.3968  partial-asm 2.3841  *all-asm 2.4031
//
// Montfull two pow array:
// gcc: no-asm 1.2895  partial-asm 1.2325  all-asm 1.2382
// (OLD but still good)clang: no-asm ---  partial-asm 1.1739  all-asm 1.1729
// gcc with all-asm mult: no-asm 1.2888  partial-asm 1.2321  *all-asm 1.2423
// clang: no-asm 1.2387  partial-asm 1.1748  all-asm 1.1742
// clang with all-asm mult: no-asm 1.2375  partial-asm 1.1736  *all-asm 1.1738
//
// Montquarter two pow array:
// (OLD but still good)gcc: no-asm ---  partial-asm 1.0967  all-asm 1.0995
// gcc: no-asm 1.1654  partial-asm 1.1064  all-asm 1.1023
// gcc with all-asm mult: no-asm 1.1660  partial-asm 1.1059  *all-asm 1.1014
// clang: no-asm 1.0739  partial-asm 1.0458  all-asm 1.0415
// clang with all-asm mult: no-asm 1.0740  partial-asm 1.0465  *all-asm 1.0411


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    defined(HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO)

template <> struct impl_unsigned_square_to_hilo_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t& lowProduct, __uint128_t u)
  {
    using T = __uint128_t;
    using H = uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H u1 = static_cast<H>(u >> shift);

#if 0
// In limited tests benchmarking montgomery two_pow, this partial-asm version
// tended to be a little slower than the full-asm version.  Thus we disable
// this version.

    // Calculate all the cross products.
    T lo_lo = static_cast<T>(u0) * static_cast<T>(u0);
    T hi_lo = static_cast<T>(u1) * static_cast<T>(u0);
    T hi_hi = static_cast<T>(u1) * static_cast<T>(u1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H hi_lo_1 = static_cast<H>(hi_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);
    H hi_lo_2 = static_cast<H>(hi_lo >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    __asm__ ("adds %[hi_lo_1], %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[hi_lo_2] \n\t"
             "cinc %[hi_hi_3], %[hi_hi_3], hs \n\t"
             "adds %[lo_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[hi_lo_2], %[hi_hi_2] \n\t"
             "cinc %[hi_hi_3], %[hi_hi_3], hs \n\t"
             : [lo_lo_1]"+&r"(lo_lo_1), [hi_hi_2]"+&r"(hi_hi_2),
               [hi_hi_3]"+&r"(hi_hi_3)
             : [hi_lo_1]"r"(hi_lo_1), [hi_lo_2]"r"(hi_lo_2)
             : "cc");

#else
    H lo_lo_0, hi_lo_1, lo_lo_1, hi_hi_2;
    __asm__ ("mul %[lo_lo_0], %[u0], %[u0] \n\t"
             "mul %[hi_lo_1], %[u1], %[u0] \n\t"
             "umulh %[lo_lo_1], %[u0], %[u0] \n\t"
             "umulh %[u0], %[u1], %[u0] \n\t"   /* u0 = hi_lo_2 */
             "mul %[hi_hi_2], %[u1], %[u1] \n\t"
             "umulh %[u1], %[u1], %[u1] \n\t"   /* u1 = hi_hi_3 */
             "adds %[hi_lo_1], %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcs %[u0], %[u0], %[u0] \n\t"             /* hi_lo_2 += hi_lo_2 */
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             "adds %[lo_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[u0], %[hi_hi_2] \n\t"   /* hi_hi_2 += hi_lo_2 */
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             : [u0]"+&r"(u0), [lo_lo_0]"=&r"(lo_lo_0),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_lo_1]"=&r"(lo_lo_1), [hi_hi_2]"=&r"(hi_hi_2)
             :
             : "cc");
    H hi_hi_3 = u1;

#endif

    lowProduct = (static_cast<T>(lo_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, u);
        HPBC_UTIL_POSTCONDITION2(lowProduct == low2 && highProduct == high2);
    }
    return highProduct;
  }
};

#endif


}} // end namespace

#endif
