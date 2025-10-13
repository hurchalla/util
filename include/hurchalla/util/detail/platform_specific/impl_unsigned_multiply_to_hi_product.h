// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_UNSIGNED_MULTIPLY_TO_HI_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_UNSIGNED_MULTIPLY_TO_HI_H_INCLUDED


#include "hurchalla/util/unsigned_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <cstdint>

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_unsigned_multiply_to_hi_product {
  HURCHALLA_FORCE_INLINE static T call(T u,T v)
  {
      T lowProduct;
      return unsigned_multiply_to_hilo_product(lowProduct, u, v);
    // for benchmarking __uint128_t, we should use the following instead:
//    return slow_unsigned_multiply_to_hilo_product::call(lowProduct, u, v);
  }
};



#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_X86_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

template <> struct impl_unsigned_multiply_to_hi_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t u, __uint128_t v)
  {
    using T = __uint128_t;
    using H = std::uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H v0 = static_cast<H>(v);
    H u1 = static_cast<H>(u >> shift);
    H v1 = static_cast<H>(v >> shift);

#if defined(__clang__)

    // Calculate all the cross products.
    T lo_lo = static_cast<T>(u0) * static_cast<T>(v0);
    T hi_lo = static_cast<T>(u1) * static_cast<T>(v0);
    T lo_hi = static_cast<T>(u0) * static_cast<T>(v1);
    T hi_hi = static_cast<T>(u1) * static_cast<T>(v1);

    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_lo_1 = static_cast<H>(hi_lo);
    H hi_lo_2 = static_cast<H>(hi_lo >> shift);

    H lo_hi_1 = static_cast<H>(lo_hi);
    H lo_hi_2 = static_cast<H>(lo_hi >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    __asm__ ("add %[hi_lo_1], %[lo_lo_1] \n\t"
             "adc %[lo_hi_2], %[hi_lo_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             "add %[lo_hi_1], %[lo_lo_1] \n\t"
             "adc %[hi_hi_2], %[hi_lo_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             : [lo_lo_1]"+&r"(lo_lo_1), [hi_lo_2]"+&r"(hi_lo_2),
               [hi_hi_3]"+&r"(hi_hi_3)
#  if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [hi_lo_1]"r"(hi_lo_1), [lo_hi_2]"r"(lo_hi_2),
               [lo_hi_1]"r"(lo_hi_1), [hi_hi_2]"r"(hi_hi_2)
#  else
             : [hi_lo_1]"rm"(hi_lo_1), [lo_hi_2]"rm"(lo_hi_2),
               [lo_hi_1]"rm"(lo_hi_1), [hi_hi_2]"rm"(hi_hi_2)
#  endif
             : "cc");
    hi_hi_2 = hi_lo_2;

#else

    H rrax = u0;
    H rrdx, lo_lo_1, hi_lo_2;
    __asm__ ("mulq %[v0] \n\t"             /* rdx:rax = rax*v0 (rax == u0); high-order bits of the product in rdx */
             "movq %%rdx, %[lo_lo_1] \n\t"
             "movq %[u1], %%rax \n\t"
             "mulq %[v0] \n\t"             /* rdx:rax = u1*v0 */
             "movq %%rax, %[v0] \n\t"      /* v0 = hi_lo_1 */
             "movq %%rdx, %[hi_lo_2] \n\t"
             "movq %[u0], %%rax \n\t"
             "mulq %[v1] \n\t"             /* rdx:rax = u0*v1 */
             "movq %%rax, %[u0] \n\t"      /* u0 = lo_hi_1 */
             "movq %[u1], %%rax \n\t"
             "movq %%rdx, %[u1] \n\t"      /* u1 = lo_hi_2 */
             "mulq %[v1] \n\t"             /* rdx:rax = u1*v1; on output, rax = hi_hi_2, rdx = hi_hi_3 */
             "addq %[v0], %[lo_lo_1] \n\t" /* lo_lo_1 += hi_lo_1 */
             "adcq %[u1], %[hi_lo_2] \n\t" /* hi_lo_2 += lo_hi_2 + carry */
             "adcq $0, %%rdx \n\t"         /* hi_hi_3 += carry */
             "addq %[u0], %[lo_lo_1] \n\t" /* lo_lo_1 += lo_hi_1 */
             "adcq %%rax, %[hi_lo_2] \n\t" /* hi_lo_2 += hi_hi_2 + carry */
             "adcq $0, %%rdx \n\t"         /* hi_hi_3 += carry */
             : "+&a"(rrax), "=&d"(rrdx), [v0]"+&r"(v0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1), [hi_lo_2]"=&r"(hi_lo_2),
               [u0]"+&r"(u0)
#  if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [v1]"r"(v1)
#  else
             : [v1]"rm"(v1)
#  endif
             : "cc");
    H hi_hi_2 = hi_lo_2;
    H hi_hi_3 = rrdx;

#endif

    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, v);
        HPBC_UTIL_POSTCONDITION2(highProduct == high2);
    }
    return highProduct;
  }
};

#endif



#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

template <> struct impl_unsigned_multiply_to_hi_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t u, __uint128_t v)
  {
    using T = __uint128_t;
    using H = uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H v0 = static_cast<H>(v);
    H u1 = static_cast<H>(u >> shift);
    H v1 = static_cast<H>(v >> shift);

    H lo_lo_1, hi_lo_1, lo_hi_1, hi_hi_2;
    __asm__ (
             "umulh %[lo_lo_1], %[u0], %[v0] \n\t"
             "mul %[hi_lo_1], %[u1], %[v0] \n\t"
             "mul %[lo_hi_1], %[u0], %[v1] \n\t"
             "umulh %[v0], %[u1], %[v0] \n\t"   /* v0 = hi_lo_2 */
             "mul %[hi_hi_2], %[u1], %[v1] \n\t"
             "umulh %[u0], %[u0], %[v1] \n\t"   /* u0 = lo_hi_2 */
             "umulh %[u1], %[u1], %[v1] \n\t"   /* u1 = hi_hi_3 */
             "adds %[lo_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[v0], %[hi_hi_2] \n\t"   /* hi_hi_2 += hi_lo_2 */
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             "adds %[lo_lo_1], %[lo_hi_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[u0], %[hi_hi_2] \n\t"   /* hi_hi_2 += lo_hi_2 */
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             : [u0]"+&r"(u0), [v0]"+&r"(v0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_hi_1]"=&r"(lo_hi_1), [hi_hi_2]"=&r"(hi_hi_2)
             : [v1]"r"(v1)
             : "cc");
    H hi_hi_3 = u1;

    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, v);
        HPBC_UTIL_POSTCONDITION2(highProduct == high2);
    }
    return highProduct;
  }
};

#endif


}} // end namespace

#endif
