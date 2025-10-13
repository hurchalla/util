// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SIGNED_SQUARE_TO_HILO_H_INCLUDED


#include "hurchalla/util/signed_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/traits/extensible_make_signed.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include "hurchalla/util/compiler_macros.h"

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_signed_square_to_hilo_product {
  HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(ut_numeric_limits<T>::is_signed, "");

    return signed_multiply_to_hilo_product(lowProduct, u, u);
    // for benchmarking __int128_t, we should use the following instead:
//    return slow_signed_multiply_to_hilo_product::call(lowProduct, u, u);
  }
};


//#define HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO

// Inline asm summary/conclusion from these ARM64 timings - for ARM64 we should
// enable the all-asm version, for both gcc and clang.  A side benefit is the
// compiler (usually gcc) is less likely to have the bad luck cases where it
// makes bad decisions and produces terrible machine code, if we use all-asm.
//
// -- Benchmark Timings --
// Monthalf two pow array:
// gcc with partial-asm mult: no-asm 1.2040  partial-asm 1.2000  *all-asm 1.0649
// gcc with all-asm mult: no-asm 1.2074  partial-asm 1.1970  *all-asm 1.0600
// clang with all-asm mult: no-asm 1.0551  partial-asm 1.0213  *all-asm 1.0168
//
// Monthalf two pow scalar:
// gcc with partial-asm mult: no-asm 2.5405  partial-asm 2.5745  *all-asm 2.5122
// gcc with all-asm mult: no-asm 2.5412  partial-asm 2.5304  *all-asm 2.4986
// clang with all-asm mult: no-asm 2.3717  partial-asm 2.4241  *all-asm 2.3612


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    (defined(HURCHALLA_TARGET_ISA_ARM_64) || defined(HURCHALLA_TARGET_ISA_X86_64)) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

template <> struct impl_signed_square_to_hilo_product<__int128_t> {
  HURCHALLA_FORCE_INLINE static
  __int128_t call(__uint128_t& lowProduct, __int128_t u)
  {
    using T = __int128_t;

    // Assert the CPU uses two's complement, and arithmetic right shift
    static_assert(static_cast<T>(-1) == ~(static_cast<T>(0)), "");
    static_assert((static_cast<T>(-1) >> 1) == static_cast<T>(-1), "");

    using S = T;  // S for signed
    using U = typename extensible_make_unsigned<T>::type;  // U for unsigned

    static constexpr int digitsU = ut_numeric_limits<U>::digits;

    static_assert(is_valid_sized_uint<digitsU/2>::value, "");
    using H = typename sized_uint<digitsU/2>::type;   // unsigned half size of U
    using I = typename extensible_make_signed<H>::type;  // signed version of H

    static constexpr int shift = digitsU / 2;

    H u0 = static_cast<H>(u);
    // arithmetic (sign-extending) right shift required
    I u1 = static_cast<I>(u >> shift);

#if 0
// For ARM64, In limited tests benchmarking montgomery two_pow, this partial-asm
// version tended to be a little slower than the full-asm version.  Thus we
// disable this version for ARM64.

    H sign_mask_u = static_cast<H>(u1 >> (shift - 1));
    H u0_or_zero = sign_mask_u & u0;

    U lo_lo = static_cast<U>(u0) * static_cast<U>(u0);

    U unfinished_hi_lo = static_cast<U>(static_cast<H>(u1)) * static_cast<U>(u0);
    H hi_lo_1 = static_cast<H>(unfinished_hi_lo);
    H unfinished_hi_lo_2 = static_cast<H>(unfinished_hi_lo >> shift);

    S hi_hi = static_cast<S>(u1) * static_cast<S>(u1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    H tmp3A = u0_or_zero;
    H hi_lo_2 = unfinished_hi_lo_2;

#  if defined(HURCHALLA_TARGET_ISA_ARM_64)
    __asm__ ("sub %[hi_lo_2], %[hi_lo_2], %[tmp3A] \n\t"   /* hi_lo_2 = unfinished_hi_lo_2 - u0_or_zero */
             "asr %[tmp3A], %[hi_lo_2], #63 \n\t"          /* tmp3A = sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[hi_lo_2] \n\t"
             "adc %[tmp3A], %[tmp3A], %[tmp3A] \n\t"
             "adds %[hi_lo_1], %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_hi_2], %[hi_lo_2] \n\t"
             "adc %[tmp3A], %[hi_hi_3], %[tmp3A] \n\t"
             : [hi_lo_2]"+&r"(hi_lo_2), [tmp3A]"+&r"(tmp3A),
               [hi_lo_1]"+&r"(hi_lo_1)
             : [lo_lo_1]"r"(lo_lo_1),
               [hi_hi_2]"r"(hi_hi_2), [hi_hi_3]"r"(hi_hi_3)
             : "cc");
#  elif defined(HURCHALLA_TARGET_ISA_X86_64)
    __asm__ (
             "subq %[tmp3A], %[hi_lo_2] \n\t"   /* hi_lo_2 = unfinished_hi_lo_2 - u0_or_zero */
             "movq %[hi_lo_2], %[tmp3A] \n\t"   /* tmp3A = hi_lo_2 */
             "sarq $63, %[tmp3A] \n\t"          /* tmp3A = sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "addq %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcq %[hi_lo_2], %[hi_lo_2] \n\t"
             "adcq %[tmp3A], %[tmp3A] \n\t"     /* tmp3A = sign_extension_hi_lo_2 + sign_extension_hi_lo_2 + carry */
             "addq %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcq %[hi_hi_2], %[hi_lo_2] \n\t" /* u0 += hi_hi_2 + carry */
             "adcq %[hi_hi_3], %[tmp3A] \n\t"   /* u1 += hi_hi_3 + carry */
             : [hi_lo_2]"+&r"(hi_lo_2), [tmp3A]"+&r"(tmp3A),
               [hi_lo_1]"+&r"(hi_lo_1)
#    if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [lo_lo_1]"r"(lo_lo_1),
               [hi_hi_2]"r"(hi_hi_2), [hi_hi_3]"r"(hi_hi_3)
#    else
             : [lo_lo_1]"rm"(lo_lo_1),
               [hi_hi_2]"rm"(hi_hi_2), [hi_hi_3]"rm"(hi_hi_3)
#    endif
             : "cc");
#  else
#    error "inline asm has not been written for your target ISA"
#  endif

    lowProduct = (static_cast<U>(hi_lo_1) << shift) | static_cast<U>(lo_lo_0);
    S highProduct = static_cast<S>((static_cast<U>(tmp3A) << shift) |
                                   (static_cast<U>(hi_lo_2)));

#else
// all-asm

#  if defined(HURCHALLA_TARGET_ISA_ARM_64)
    H lo_lo_0, hi_lo_1, lo_lo_1, hi_lo_2, sign2;
    __asm__ (
             "mul %[lo_lo_0], %[u0], %[u0] \n\t"
             "mul %[hi_lo_1], %[u1], %[u0] \n\t"
             "umulh %[lo_lo_1], %[u0], %[u0] \n\t"
             "umulh %[hi_lo_2], %[u1], %[u0] \n\t"
             "and %[u0], %[u0], %[u1], asr #63 \n\t"   /* u0 = u0_or_zero,  2 cycles */
             "sub %[hi_lo_2], %[hi_lo_2], %[u0] \n\t"  /* hi_lo_2 = unfinished_hi_lo_2 - u0_or_zero */
             "mul %[u0], %[u1], %[u1] \n\t"            /* u0 = hi_hi_2 */
             "smulh %[u1], %[u1], %[u1] \n\t"          /* u1 = hi_hi_3 */
             "asr %[sign2], %[hi_lo_2], #63 \n\t"      /* sign2 = sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[hi_lo_2] \n\t"
             "adc %[sign2], %[sign2], %[sign2] \n\t"   /* result3 = carry + sign2 + sign2 */
             "adds %[hi_lo_1], %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[u0], %[hi_lo_2] \n\t" /* hi_lo_2 = hi_hi_2 + hi_lo_2 + carry */
             "adc %[sign2], %[u1], %[sign2] \n\t"      /* result3 = hi_hi_3 + result3 + carry */
             : [u0]"+&r"(u0), [lo_lo_0]"=&r"(lo_lo_0),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_lo_1]"=&r"(lo_lo_1), [hi_lo_2]"=&r"(hi_lo_2),
               [sign2]"=&r"(sign2)
             :
             : "cc");
    lowProduct = (static_cast<U>(hi_lo_1) << shift) | static_cast<U>(lo_lo_0);
    S highProduct = static_cast<S>((static_cast<U>(sign2) << shift) |
                                   (static_cast<U>(hi_lo_2)));
#  elif defined(HURCHALLA_TARGET_ISA_X86_64)
    H rrax = u0;
    H rrdx, lo_lo_0, lo_lo_1, hi_lo_1;
    __asm__ (
             "mulq %[u0] \n\t"             /* rdx:rax = rax*u0 (rax == u0); high-order bits of the product in rdx */
             "movq %%rax, %[lo_lo_0] \n\t"
             "movq %%rdx, %[lo_lo_1] \n\t"
             "movq %[u1], %%rax \n\t"
             "mulq %[u0] \n\t"             /* MUL. rdx:rax = u1*u0; rdx = unfinished_hi_lo_2 */
             "movq %%rax, %[hi_lo_1] \n\t"

             "movq %[u1], %%rax \n\t"
             "sarq $63, %%rax \n\t"
             "andq %%rax, %[u0] \n\t"      /* u0 = u0_or_zero */
             "subq %[u0], %%rdx \n\t"      /* hi_lo_2 = unfinished_hi_lo_2 - u0_or_zero */
             "movq %%rdx, %[u0] \n\t"      /* u0 = hi_lo_2 */

             "movq %[u1], %%rax \n\t"
             "imulq %[u1] \n\t"            /* IMUL. rdx:rax = u1*u1; on output, rax = hi_hi_2, rdx = hi_hi_3 */

             "movq %[u0], %[u1] \n\t"      /* u1 = hi_lo_2 */
             "sarq $63, %[u1] \n\t"        /* sign_extension_hi_lo_2 = hi_lo_2 >> 63 */

             "addq %[hi_lo_1], %[hi_lo_1] \n\t"
             "adcq %[u0], %[u0] \n\t"      /* u0 = hi_lo_2 + hi_lo_2 + carry */
             "adcq %[u1], %[u1] \n\t"      /* u1 = sign_extension_hi_lo_2 + sign_extension_hi_lo_2 + carry */
             "addq %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcq %%rax, %[u0] \n\t"      /* u0 += hi_hi_2 + carry */
             "adcq %%rdx, %[u1] \n\t"      /* u1 += hi_hi_3 + carry */
             : "+&a"(rrax), "=&d"(rrdx), [u0]"+&r"(u0),
               [lo_lo_0]"=&r"(lo_lo_0), [lo_lo_1]"=&r"(lo_lo_1),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1)
             :
             : "cc");
    lowProduct = (static_cast<U>(hi_lo_1) << shift) | static_cast<U>(lo_lo_0);
    S highProduct = static_cast<S>((static_cast<U>(u1) << shift) |
                                   (static_cast<U>(u0)));
#  else
#    error "inline asm has not been written for your target ISA"
#  endif


#endif

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        U low2;
        S high2 = slow_signed_multiply_to_hilo_product::call(low2, u, u);
        HPBC_UTIL_POSTCONDITION2(lowProduct == low2 && highProduct == high2);
    }
    return highProduct;
  }
};

#endif



}} // end namespace

#endif
