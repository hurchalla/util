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





// Inline asm summary/conclusion from these x64 (Zen4) timings -
// For x64:
// We should use all-asm for gcc (pretty clear cut), and for clang we should use
// partial-asm (this isn't completely clear cut since it's a loss on montquarter
// - for wins see the comments in impl_unsigned_multiply_to_hilo_product.h).
// For gcc, a side benefit is the compiler is less likely to have the bad luck
// cases where it makes bad decisions and produces terrible machine code, if we
// use all-asm.
//
// -- Benchmark Timings --
//
// Montquarter two pow scalar:
// gcc with all-asm mult: no-asm 2.3755  partial-asm 2.1298  *all-asm 2.0015
// clang with all-asm mult: no-asm 1.9701  partial-asm 1.9605  all-asm 1.9282
// Montfull two pow scalar:
// gcc with all-asm mult: no-asm 2.5055  partial-asm 2.4362  *all-asm 2.0770
// clang with all-asm mult: no-asm 2.1848  partial-asm 2.1150  all-asm 2.1040
// Montfull two pow array:
// gcc with all-asm mult: no-asm 2.0061  partial-asm 1.9696  *all-asm 1.7195
// clang with all-asm mult: no-asm 1.5402  partial-asm 1.4724  all-asm 1.4796
// Montquarter two pow array:
// gcc with all-asm mult: no-asm 1.8282  partial-asm 1.7800  *all-asm 1.5213
// clang with all-asm mult: no-asm 1.3898  partial-asm 1.2932  all-asm 1.3353


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_X86_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

template <> struct impl_unsigned_square_to_hilo_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t& lowProduct, __uint128_t u)
  {
    using T = __uint128_t;
    using H = std::uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H u1 = static_cast<H>(u >> shift);

#if defined(__clang__)

    T lo_lo = static_cast<T>(u0) * static_cast<T>(u0);
    T hi_lo = static_cast<T>(u1) * static_cast<T>(u0);
    T hi_hi = static_cast<T>(u1) * static_cast<T>(u1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_lo_1 = static_cast<H>(hi_lo);
    H hi_lo_2 = static_cast<H>(hi_lo >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    __asm__ ("add %[hi_lo_1], %[hi_lo_1] \n\t"
             "adc %[hi_lo_2], %[hi_lo_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             "add %[lo_lo_1], %[hi_lo_1] \n\t"
             "adc %[hi_hi_2], %[hi_lo_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             : [hi_lo_1]"+&r"(hi_lo_1), [hi_lo_2]"+&r"(hi_lo_2),
               [hi_hi_3]"+&r"(hi_hi_3)
#  if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [lo_lo_1]"r"(lo_lo_1), [hi_hi_2]"r"(hi_hi_2)
#  else
             : [lo_lo_1]"rm"(lo_lo_1), [hi_hi_2]"rm"(hi_hi_2)
#  endif
             : "cc");
    lowProduct = (static_cast<T>(hi_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_lo_2;

#else

    H rrax = u0;
    H rrdx, lo_lo_0, lo_lo_1;
    __asm__ ("mulq %[u0] \n\t"              /* rdx:rax = rax*u0 (rax == u0); high-order bits of the product in rdx */
             "movq %%rax, %[lo_lo_0] \n\t"
             "movq %%rdx, %[lo_lo_1] \n\t"
             "movq %[u1], %%rax \n\t"
             "mulq %[u0] \n\t"              /* rdx:rax = u1*u0 */
             "movq %%rax, %[u0] \n\t"       /* u0 = hi_lo_1 */
             "movq %[u1], %%rax \n\t"       /* u1_orig = u1 */
             "movq %%rdx, %[u1] \n\t"       /* u1 = hi_lo_2 */
             "mulq %%rax \n\t"              /* rdx:rax = u1_orig*u1_orig; on output, rax = hi_hi_2, rdx = hi_hi_3 */
             "addq %[u0], %[u0] \n\t"       /* hi_lo_1 += hi_lo_1 */
             "adcq %[u1], %[u1] \n\t"       /* hi_lo_2 += hi_lo_2 */
             "adcq $0, %%rdx \n\t"          /* hi_hi_3 += carry */
             "addq %[u0], %[lo_lo_1] \n\t"  /* lo_lo_1 += hi_lo_1 */
             "adcq %[u1], %%rax \n\t"       /* hi_hi_2 += hi_lo_2 + carry */
             "adcq $0, %%rdx \n\t"          /* hi_hi_3 += carry */
             : "+&a"(rrax), [u0]"+&r"(u0), "=&d"(rrdx), [lo_lo_0]"=&r"(lo_lo_0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1)
             :
             : "cc");
    H hi_hi_2 = rrax;
    H hi_hi_3 = rrdx;
    lowProduct = (static_cast<T>(lo_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;

#endif

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, u);
        HPBC_UTIL_POSTCONDITION2(lowProduct == low2 && highProduct == high2);
    }
    return highProduct;
  }
};

#endif





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
// gcc: no-asm 2.4053   partial-asm 2.4028  all-asm 2.3802
// gcc with all-asm mult: no-asm 2.4019  partial-asm 2.4048  *all-asm 2.3789
// clang: no-asm 2.3779  partial-asm 2.3864  all-asm 2.3960
// clang with all-asm mult: no-asm 2.3968  partial-asm 2.3841  *all-asm 2.4031
//
// Montfull two pow array:
// gcc: no-asm 1.2895  partial-asm 1.2325  all-asm 1.2382
// gcc with all-asm mult: no-asm 1.2888  partial-asm 1.2321  *all-asm 1.2423
// clang: no-asm 1.2387  partial-asm 1.1748  all-asm 1.1742
// clang with all-asm mult: no-asm 1.2375  partial-asm 1.1736  *all-asm 1.1738
//
// Montquarter two pow array:
// gcc: no-asm 1.1654  partial-asm 1.1064  all-asm 1.1023
// gcc with all-asm mult: no-asm 1.1660  partial-asm 1.1059  *all-asm 1.1014
// clang: no-asm 1.0739  partial-asm 1.0458  all-asm 1.0415
// clang with all-asm mult: no-asm 1.0740  partial-asm 1.0465  *all-asm 1.0411


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

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
