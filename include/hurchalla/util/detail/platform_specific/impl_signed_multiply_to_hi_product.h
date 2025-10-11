// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SIGNED_MULTIPLY_TO_HI_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SIGNED_MULTIPLY_TO_HI_H_INCLUDED


#include "hurchalla/util/signed_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/traits/extensible_make_signed.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <cstdint>

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_signed_multiply_to_hi_product {
  HURCHALLA_FORCE_INLINE static
  T call(T u, T v)
  {
      typename extensible_make_unsigned<T>::type lowProduct; 
      return signed_multiply_to_hilo_product(lowProduct, u, v);
    // for benchmarking __int128_t, we should use the following instead:
//    return slow_signed_multiply_to_hilo_product::call(lowProduct, u, v);
  }
};



//#define HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    (defined(HURCHALLA_TARGET_ISA_ARM_64) || defined(HURCHALLA_TARGET_ISA_X86_64)) && \
    defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO)

template <> struct impl_signed_multiply_to_hi_product<__int128_t> {
  HURCHALLA_FORCE_INLINE static
  __int128_t call(__int128_t u, __int128_t v)
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
    H v0 = static_cast<H>(v);
    I v1 = static_cast<I>(v >> shift);

# if 1
// all-asm

#  if defined(HURCHALLA_TARGET_ISA_ARM_64)
    H lo_lo_1, hi_lo_1, lo_hi_1, hi_lo_2, lo_hi_2;
    __asm__ (
             "umulh %[lo_lo_1], %[u0], %[v0] \n\t"
             "mul %[hi_lo_1], %[u1], %[v0] \n\t"
             "mul %[lo_hi_1], %[u0], %[v1] \n\t"
             "umulh %[hi_lo_2], %[u1], %[v0] \n\t"
             "umulh %[lo_hi_2], %[u0], %[v1] \n\t"
             "and %[v0], %[v0], %[u1], asr #63 \n\t"   /* v0 = v0_or_zero,  2 cycles */
             "and %[u0], %[u0], %[v1], asr #63 \n\t"   /* u0 = u0_or_zero,  2 cycles */
             "sub %[hi_lo_2], %[hi_lo_2], %[v0] \n\t"  /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "sub %[lo_hi_2], %[lo_hi_2], %[u0] \n\t"  /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "mul %[v0], %[u1], %[v1] \n\t"            /* v0 = hi_hi_2 */
             "smulh %[u1], %[u1], %[v1] \n\t"          /* u1 = hi_hi_3 */
             "asr %[u0], %[hi_lo_2], #63 \n\t"         /* u0 = sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "asr %[v1], %[lo_hi_2], #63 \n\t"         /* v1 = sign_extension_lo_hi_2 = lo_hi_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[lo_hi_2] \n\t"
             "adc %[u0], %[u0], %[v1] \n\t"            /* result3 = carry + sign_extension_hi_lo_2 + sign_extension_lo_hi_2 */
             "adds %[hi_lo_1], %[lo_hi_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[v0], %[hi_lo_2] \n\t"
             "adc %[u0], %[u1], %[u0] \n\t"            /* result3 = hi_hi_3 + result3 + carry */
             : [u0]"+&r"(u0), [v0]"+&r"(v0),
               [lo_lo_1]"=&r"(lo_lo_1),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [v1]"+&r"(v1), [lo_hi_1]"=&r"(lo_hi_1),
               [hi_lo_2]"=&r"(hi_lo_2), [lo_hi_2]"=&r"(lo_hi_2)
             :
             : "cc");
    S highProduct = static_cast<S>((static_cast<U>(u0) << shift) |
                                   (static_cast<U>(hi_lo_2)));
#  elif defined(HURCHALLA_TARGET_ISA_X86_64)
    H rrax = u0;
    H rrdx, lo_lo_1, hi_lo_1, lo_hi_1;
    __asm__ (
             "mulq %[v0] \n\t"             /* rdx:rax = rax*v0 (rax == u0); high-order bits of the product in rdx */
             "movq %%rdx, %[lo_lo_1] \n\t"
             "movq %[u1], %%rax \n\t"
             "mulq %[v0] \n\t"             /* MUL. rdx:rax = u1*v0; rdx = unfinished_hi_lo_2 */
             "movq %%rax, %[hi_lo_1] \n\t"

             "movq %[u1], %%rax \n\t"
             "sarq $63, %%rax \n\t"
             "andq %%rax, %[v0] \n\t"      /* v0 = v0_or_zero */
             "subq %[v0], %%rdx \n\t"      /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "movq %%rdx, %[v0] \n\t"      /* v0 = hi_lo_2 */

             "movq %[u0], %%rax \n\t"
             "mulq %[v1] \n\t"             /* MUL. rdx:rax = u0*v1; rdx = unfinished_lo_hi_2 */
             "movq %%rax, %[lo_hi_1] \n\t"

             "movq %[v1], %%rax \n\t"      /* prep the next imul by setting rax */

             "sarq $63, %[v1] \n\t"
             "andq %[v1], %[u0] \n\t"      /* u0 = u0_or_zero */
             "subq %[u0], %%rdx \n\t"      /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "movq %%rdx, %[v1] \n\t"      /* v1 = lo_hi_2 */

             "imulq %[u1] \n\t"            /* IMUL. rdx:rax = u1*v1; on output, rax = hi_hi_2, rdx = hi_hi_3 */

             "movq %[v0], %[u0] \n\t"      /* u0 = hi_lo_2 */
             "movq %[v1], %[u1] \n\t"      /* u1 = lo_hi_2 */
             "sarq $63, %[u0] \n\t"        /* sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "sarq $63, %[u1] \n\t"        /* sign_extension_lo_hi_2 = lo_hi_2 >> 63 */

             "addq %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcq %[v1], %[v0] \n\t"      /* v0 = hi_lo_2 + lo_hi_2 */
             "adcq %[u1], %[u0] \n\t"      /* u0 = sign_extension_hi_lo_2 + sign_extension_lo_hi_2 */
             "addq %[lo_hi_1], %[hi_lo_1] \n\t"
             "adcq %%rax, %[v0] \n\t"      /* v0 += hi_hi_2 */
             "adcq %%rdx, %[u0] \n\t"      /* u0 += hi_hi_3 */
             : "+&a"(rrax), "=&d"(rrdx), [v0]"+&r"(v0),
               [lo_lo_1]"=&r"(lo_lo_1),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [u0]"+&r"(u0), [v1]"+&r"(v1),
               [lo_hi_1]"=&r"(lo_hi_1)
             :
             : "cc");
    S highProduct = static_cast<S>((static_cast<U>(u0) << shift) |
                                   (static_cast<U>(v0)));
#  else
#    error "inline asm has not been written for your target ISA"
#  endif



# else
// For ARM64: In limited tests benchmarking montgomery two_pow, this partial-asm
// version tended to be a little slower than the full-asm version.  Thus we
// disable this version for ARM64.

    H sign_mask_u = static_cast<H>(u1 >> (shift - 1));
    H sign_mask_v = static_cast<H>(v1 >> (shift - 1));
    H v0_or_zero = sign_mask_u & v0;
    H u0_or_zero = sign_mask_v & u0;

    U lo_lo = static_cast<U>(u0) * static_cast<U>(v0);

    U unfinished_hi_lo = static_cast<U>(static_cast<H>(u1)) * static_cast<U>(v0);
    H hi_lo_1 = static_cast<H>(unfinished_hi_lo);
    H unfinished_hi_lo_2 = static_cast<H>(unfinished_hi_lo >> shift);

    U unfinished_lo_hi = static_cast<U>(u0) * static_cast<U>(static_cast<H>(v1));
    H lo_hi_1 = static_cast<H>(unfinished_lo_hi);
    H unfinished_lo_hi_2 = static_cast<H>(unfinished_lo_hi >> shift);

    S hi_hi = static_cast<S>(u1) * static_cast<S>(v1);

    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    H tmp3A = v0_or_zero;
    H tmp3B = u0_or_zero;
    H hi_lo_2 = unfinished_hi_lo_2;
    H lo_hi_2 = unfinished_lo_hi_2;

#  if defined(HURCHALLA_TARGET_ISA_ARM_64)
    __asm__ ("sub %[hi_lo_2], %[hi_lo_2], %[tmp3A] \n\t"   /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "sub %[lo_hi_2], %[lo_hi_2], %[tmp3B] \n\t"   /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "asr %[tmp3A], %[hi_lo_2], #63 \n\t"          /* sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "asr %[tmp3B], %[lo_hi_2], #63 \n\t"          /* sign_extension_lo_hi_2 = lo_hi_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[lo_hi_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[lo_hi_2] \n\t"
             "adc %[tmp3A], %[tmp3A], %[tmp3B] \n\t"
             "adds %[hi_lo_1], %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_hi_2], %[hi_lo_2] \n\t"
             "adc %[tmp3A], %[hi_hi_3], %[tmp3A] \n\t"
             : [hi_lo_2]"+&r"(hi_lo_2), [tmp3A]"+&r"(tmp3A),
               [lo_hi_2]"+&r"(lo_hi_2), [tmp3B]"+&r"(tmp3B),
               [hi_lo_1]"+&r"(hi_lo_1)
             : [lo_hi_1]"r"(lo_hi_1), [lo_lo_1]"r"(lo_lo_1),
               [hi_hi_2]"r"(hi_hi_2), [hi_hi_3]"r"(hi_hi_3)
             : "cc");
#  elif defined(HURCHALLA_TARGET_ISA_X86_64)
    __asm__ ("subq %[tmp3A], %[hi_lo_2] \n\t"   /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "subq %[tmp3B], %[lo_hi_2] \n\t"   /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "movq %[hi_lo_2], %[tmp3A] \n\t"
             "movq %[lo_hi_2], %[tmp3B] \n\t"
             "sarq $63, %[tmp3A] \n\t"          /* sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "sarq $63, %[tmp3B] \n\t"          /* sign_extension_lo_hi_2 = lo_hi_2 >> 63 */
             "addq %[lo_hi_1], %[hi_lo_1] \n\t"
             "adcq %[lo_hi_2], %[hi_lo_2] \n\t"
             "adcq %[tmp3B], %[tmp3A] \n\t"
             "addq %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcq %[hi_hi_2], %[hi_lo_2] \n\t"
             "adcq %[hi_hi_3], %[tmp3A] \n\t"
             : [hi_lo_2]"+&r"(hi_lo_2), [tmp3A]"+&r"(tmp3A),
               [lo_hi_2]"+&r"(lo_hi_2), [tmp3B]"+&r"(tmp3B),
               [hi_lo_1]"+&r"(hi_lo_1)
#    if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [lo_hi_1]"r"(lo_hi_1), [lo_lo_1]"r"(lo_lo_1),
               [hi_hi_2]"r"(hi_hi_2), [hi_hi_3]"r"(hi_hi_3)
#    else
             : [lo_hi_1]"rm"(lo_hi_1), [lo_lo_1]"rm"(lo_lo_1),
               [hi_hi_2]"rm"(hi_hi_2), [hi_hi_3]"rm"(hi_hi_3)
#    endif
             : "cc");
#  else
#    error "inline asm has not been written for your target ISA"
#  endif

    S highProduct = static_cast<S>((static_cast<U>(tmp3A) << shift) |
                                   (static_cast<U>(hi_lo_2)));
# endif

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        U low2;
        S high2 = slow_signed_multiply_to_hilo_product::call(low2, u, v);
        HPBC_UTIL_POSTCONDITION2(highProduct == high2);
    }
    return highProduct;
  }
};

#endif


}} // end namespace

#endif
