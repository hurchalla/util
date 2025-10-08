// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_UNSIGNED_MULT_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_UNSIGNED_MULT_TO_HILO_H_INCLUDED


#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <cstdint>
#if defined(_MSC_VER)
#  include <intrin.h>
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
#ifdef __clang__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wc++98-c++11-c++14-compat"
#endif

namespace hurchalla { namespace detail {


template <typename T>
struct impl_unsigned_multiply_to_hilo_product;

// Slow implementation that works for all compilers and architectures.
// Return Value:
//   Returns the high portion of the product.
// Notes:
//   - I adapted this code from https://stackoverflow.com/a/58381061
//     On ARM32 with clang it compiles nicely with T=uint64_t, using the UMAAL
//     instruction (you may need -march=armv7-a or similar)
//   - Uses static member function to disallow ADL.
struct slow_unsigned_multiply_to_hilo_product {
  template <typename T>
  HURCHALLA_FORCE_INLINE static T call(T& lowProduct, T u, T v)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    // for example, if T==uint64_t, shift ought to == 32
    static constexpr unsigned int shift = ut_numeric_limits<T>::digits / 2;
    // for example, if T==uint64_t, lowmask ought to == 0xFFFFFFFF
    static constexpr T lowmask = (static_cast<T>(1)<<shift) - static_cast<T>(1);

#if __cplusplus >= 201703L
    // The intent here is to find out if T is an integral type that the compiler
    // has in-built support for (and thus, that it could optimize well).  If
    // it is not, then we use this if clause - which ensures via recursion that
    // the compiler gets a type that it can optimize well.
    // To use the contents of this if clause, we also need sized_uint<shift> to
    // be valid, which is why we test for that too.
    //
    // It's not ideal currently that the clause relies on sized_uint<shift>,
    // since effectively that means the clause can only run when T is twice as
    // big as the largest integer the compiler supports.  E.g. - currently this
    // clause will not run when T is 4x or 8x the size of largest integer the
    // compiler knows, which is not ideal.
    if constexpr (!is_valid_sized_uint<ut_numeric_limits<T>::digits>::value &&
                  is_valid_sized_uint<static_cast<int>(shift)>::value)
    {
      using U = typename sized_uint<static_cast<int>(shift)>::type;

      U u0 = static_cast<U>(u);
      U v0 = static_cast<U>(v);
      U u1 = static_cast<U>(u >> shift);
      U v1 = static_cast<U>(v >> shift);

      // Calculate all the cross products.
      U lo_lo_0;  U lo_lo_1 = impl_unsigned_multiply_to_hilo_product<U>::call(
                                                               lo_lo_0, u0, v0);
      U hi_lo_0;  U hi_lo_1 = impl_unsigned_multiply_to_hilo_product<U>::call(
                                                               hi_lo_0, u1, v0);
      U lo_hi_0;  U lo_hi_1 = impl_unsigned_multiply_to_hilo_product<U>::call(
                                                               lo_hi_0, u0, v1);
      U hi_hi_0;  U hi_hi_1 = impl_unsigned_multiply_to_hilo_product<U>::call(
                                                               hi_hi_0, u1, v1);

      T lo_hi = (static_cast<T>(lo_hi_1) << shift) | static_cast<T>(lo_hi_0);
      T hi_hi = (static_cast<T>(hi_hi_1) << shift) | static_cast<T>(hi_hi_0);

      // See the else clause below for why this works.  It is a direct
      // translation of the corresponding part of the else clause.
      T cross = static_cast<T>(lo_lo_1) + static_cast<T>(hi_lo_0) + lo_hi;
      T high = static_cast<T>(hi_lo_1) + (cross >> shift) + hi_hi;

      lowProduct = (cross << shift) | static_cast<T>(lo_lo_0);
      return high;
    }
    else
#endif
    {
      T u0 = u & lowmask;
      T v0 = v & lowmask;
      T u1 = u >> shift;
      T v1 = v >> shift;

      // Calculate all the cross products.
      T lo_lo = u0 * v0;
      T hi_lo = u1 * v0;
      T lo_hi = u0 * v1;
      T hi_hi = u1 * v1;

      // The next statement will not overflow.  Proof: let S=2^(shift). We can see
      // that both (lo_lo >> shift) and (hi_lo & lowmask) must be less than S.
      // Therefore the max possible value of cross= (S-1) + (S-1) + (S-1)*(S-1) ==
      // S-1 + S-1 + S*S - 2*S + 1 == S*S - 1, which is the max value that can be
      // represented in type T.  Thus the calculation will never overflow.
      T cross = (lo_lo >> shift) + (hi_lo & lowmask) + lo_hi;
      // The next statement will not overflow, for the same reason as above.
      T high = (hi_lo >> shift) + (cross >> shift) + hi_hi;

      lowProduct = (cross << shift) | (lo_lo & lowmask);
      return high;
    }
  }
};



// Intended as a helper for the functions below.
// minor note: uses static member function to disallow ADL.
struct umult_to_hilo_product {
  template <typename T, typename T2>
  HURCHALLA_FORCE_INLINE static T call(T& lowProduct, T u, T v)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    static_assert(ut_numeric_limits<T2>::is_integer, "");
    static_assert(!(ut_numeric_limits<T2>::is_signed), "");
    static_assert(ut_numeric_limits<T2>::digits >=
                  2*ut_numeric_limits<T>::digits, "");
    T2 product = static_cast<T2>(static_cast<T2>(u) * static_cast<T2>(v));
    lowProduct = static_cast<T>(product);
    return  static_cast<T>(product >> ut_numeric_limits<T>::digits);
  }
};


// primary template
template <typename T>
struct impl_unsigned_multiply_to_hilo_product {
  HURCHALLA_FORCE_INLINE static T call(T& lowProduct, T u,T v)
  {
#ifdef HURCHALLA_COMPILE_ERROR_ON_SLOW_MATH
#  ifndef HURCHALLA_TARGET_BIT_WIDTH
#    error "HURCHALLA_TARGET_BIT_WIDTH must be defined"
#  endif
      static_assert(ut_numeric_limits<T>::is_integer, "");
      static_assert(!(ut_numeric_limits<T>::is_signed), "");
      static_assert(ut_numeric_limits<T>::digits > HURCHALLA_TARGET_BIT_WIDTH,
        "For T <= the native bit depth, it makes sense to issue the compile error requested by HURCHALLA_COMPILE_ERROR_ON_SLOW_MATH.  For T larger than the native bit depth, it is more difficult to avoid using a slow math routine.");
#endif
      return slow_unsigned_multiply_to_hilo_product::call(lowProduct, u, v);
  }
};

// Note that when using these simple specializations, the generated asm from
// clang/icc/gcc is generally quite good.
// GCC for ARM seems to make the worst generated asm, but it's not so bad as to
// make inline asm seem worthwhile.
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint8_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint8_t call(std::uint8_t& lowProduct, std::uint8_t u, std::uint8_t v)
  {
    // Note we could have used 'T2 = unsigned int' since 'int' is >= 16bit.
    using T2 = std::uint16_t;
    return umult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint16_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint16_t call(std::uint16_t& lowProduct, std::uint16_t u,std::uint16_t v)
  {
    using T2 = std::uint32_t;
    return umult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
// --------------------------------------------------------------------------
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint32_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint32_t call(std::uint32_t& lowProduct, std::uint32_t u,std::uint32_t v)
  {
    using T2 = std::uint64_t;
    return umult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
// Note for MSVC: 'uint32_t' specializations using intrinsics don't improve the
// asm generated compared to the implementation above, and so intrinsic versions
// are not present here.  For reference, the intrinsics would have been
// __emulu (for X86) and _arm_umull (for ARM32).
// --------------------------------------------------------------------------


// The following fast 'uint64_t' specializations use intrinsics (MSVC), or
// compiler extensions (__uint128_t on GNU compatible compilers).
// Assembly versions for x86 or ARM aren't needed - clang/gcc/icc generate
// assembly that is good enough via __uint128_t, and MSVC does well using
// the intrinsics.

// MSVC + x64
#if defined(_MSC_VER) && defined(HURCHALLA_TARGET_ISA_X86_64)
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint64_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint64_t call(std::uint64_t& lowProduct, std::uint64_t u,std::uint64_t v)
  {
    using std::uint64_t;
    uint64_t highProduct;
    lowProduct = _umul128(u, v, &highProduct);
    if (HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpHi, tmpLo;
        tmpHi = slow_unsigned_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_UTIL_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
    }
    return highProduct;
  }
};
// MSVC + ARM64
#elif defined(_MSC_VER) && defined(_M_ARM64)
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint64_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint64_t call(std::uint64_t& lowProduct, std::uint64_t u,std::uint64_t v)
  {
    using std::uint64_t;
    uint64_t highProduct = __umulh(u, v);
    // avoid undefined behavior that could result if uint64_t would be promoted
    // to (signed) 'int'.  Promotion of uint64_t is HIGHLY(!) unlikely, but
    // since the C++ standard doesn't disallow 'int' type larger than 64 bit, we
    // should write code that's safe after the C++ "usual arithmetic conversion"
    // rules apply.
    using P = typename safely_promote_unsigned<uint64_t>::type;
    lowProduct = static_cast<uint64_t>(static_cast<P>(u)*v);
    if (HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpHi, tmpLo;
        tmpHi = slow_unsigned_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_UTIL_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
    }
    return highProduct;
  }
};
#elif (HURCHALLA_COMPILER_HAS_UINT128_T())
template <> struct impl_unsigned_multiply_to_hilo_product<std::uint64_t> {
  HURCHALLA_FORCE_INLINE static
  std::uint64_t call(std::uint64_t& lowProduct, std::uint64_t u,std::uint64_t v)
  {
    using T2 = __uint128_t;
    return umult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
#endif





// On ARM64, benchmarking on M2, using inline asm for this function provided
// ~3-6% perf boost.  The benefit was greatest for throughput-bound situations
// (though latency-bound improved too).  Naturally it does not affect *signed*
// multiplies, because this is an unsigned mul.
// Measurements were with gcc and clang.
//
// On x64, benchmarking on Zen4, I obtained similar perf improvements, though
// with gcc, the benefit was at times extremely large - gcc seemed to sometimes
// make terrible choices in generating asm from the plain C++ (non-inline-asm)
// function.


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_X86_64) && \
    defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO)

template <> struct impl_unsigned_multiply_to_hilo_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t& lowProduct, __uint128_t u, __uint128_t v)
  {
    using T = __uint128_t;
    using H = std::uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H v0 = static_cast<H>(v);
    H u1 = static_cast<H>(u >> shift);
    H v1 = static_cast<H>(v >> shift);

#if 0
// This benchmarks slower than the #else for clang and gcc - it's perhaps 5-8%
// slower on clang, and just terrible on gcc (around 50-80%? slower).  Gcc
// must(?) be creating terrible asm for the mults, prior to the inline asm.

    // Calculate all the cross products.
    T lo_lo = static_cast<T>(u0) * static_cast<T>(v0);
    T hi_lo = static_cast<T>(u1) * static_cast<T>(v0);
    T lo_hi = static_cast<T>(u0) * static_cast<T>(v1);
    T hi_hi = static_cast<T>(u1) * static_cast<T>(v1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_lo_1 = static_cast<H>(hi_lo);
    H hi_lo_2 = static_cast<H>(hi_lo >> shift);

    H lo_hi_1 = static_cast<H>(lo_hi);
    H lo_hi_2 = static_cast<H>(lo_hi >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    __asm__ ("add %[hi_lo_1], %[lo_lo_1] \n\t"
             "adc %[hi_lo_2], %[hi_hi_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             "add %[lo_hi_1], %[lo_lo_1] \n\t"
             "adc %[lo_hi_2], %[hi_hi_2] \n\t"
             "adc $0, %[hi_hi_3] \n\t"
             : [lo_lo_1]"+&r"(lo_lo_1), [hi_hi_2]"+&r"(hi_hi_2),
               [hi_hi_3]"+&r"(hi_hi_3)
#  if defined(__clang__)       /* https://bugs.llvm.org/show_bug.cgi?id=20197 */
             : [hi_lo_1]"r"(hi_lo_1), [hi_lo_2]"r"(hi_lo_2),
               [lo_hi_1]"r"(lo_hi_1), [lo_hi_2]"r"(lo_hi_2)
#  else
             : [hi_lo_1]"rm"(hi_lo_1), [hi_lo_2]"rm"(hi_lo_2),
               [lo_hi_1]"rm"(lo_hi_1), [lo_hi_2]"rm"(lo_hi_2)
#  endif
             : "cc");

#else
// This benchmarks best for both gcc and clang.

    H rrax = u0;
    H rrdx, lo_lo_0, lo_lo_1, hi_lo_2;
    __asm__ ("mulq %[v0] \n\t"             /* rdx:rax = rax*v0 (rax == u0); high-order bits of the product in rdx */
             "movq %%rax, %[lo_lo_0] \n\t"
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
             "adcq %%rax, %[hi_lo_2] \n\t" /* hi_lo_2 += hi_hi_2 + carry */
             "adcq $0, %%rdx \n\t"         /* hi_hi_3 += carry */
             "addq %[u0], %[lo_lo_1] \n\t" /* lo_lo_1 += lo_hi_1 */
             "adcq %[u1], %[hi_lo_2] \n\t" /* hi_lo_2 += lo_hi_2 + carry */
             "adcq $0, %%rdx \n\t"         /* hi_hi_3 += carry */
             : "+&a"(rrax), "=&d"(rrdx), [v0]"+&r"(v0), [lo_lo_0]"=&r"(lo_lo_0),
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

    lowProduct = (static_cast<T>(lo_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;
    return highProduct;
  }
};

#endif


//#define HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO

#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO)

template <> struct impl_unsigned_multiply_to_hilo_product<__uint128_t> {
  HURCHALLA_FORCE_INLINE static
  __uint128_t call(__uint128_t& lowProduct, __uint128_t u, __uint128_t v)
  {
    using T = __uint128_t;
    using H = uint64_t;
    static constexpr unsigned int shift = 64;

    H u0 = static_cast<H>(u);
    H v0 = static_cast<H>(v);
    H u1 = static_cast<H>(u >> shift);
    H v1 = static_cast<H>(v >> shift);

#if 1
    // Calculate all the cross products.
    T lo_lo = static_cast<T>(u0) * static_cast<T>(v0);
    T hi_lo = static_cast<T>(u1) * static_cast<T>(v0);
    T lo_hi = static_cast<T>(u0) * static_cast<T>(v1);
    T hi_hi = static_cast<T>(u1) * static_cast<T>(v1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_lo_1 = static_cast<H>(hi_lo);
    H hi_lo_2 = static_cast<H>(hi_lo >> shift);

    H lo_hi_1 = static_cast<H>(lo_hi);
    H lo_hi_2 = static_cast<H>(lo_hi >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    __asm__ ("adds %[lo_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[hi_lo_2], %[hi_hi_2] \n\t"
             "cinc %[hi_hi_3], %[hi_hi_3], hs \n\t"
             "adds %[lo_lo_1],%[lo_hi_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[lo_hi_2], %[hi_hi_2] \n\t"
             "cinc %[hi_hi_3], %[hi_hi_3], hs \n\t"
             : [lo_lo_1]"+&r"(lo_lo_1), [hi_hi_2]"+&r"(hi_hi_2),
               [hi_hi_3]"+&r"(hi_hi_3)
             : [hi_lo_1]"r"(hi_lo_1), [hi_lo_2]"r"(hi_lo_2),
               [lo_hi_1]"r"(lo_hi_1), [lo_hi_2]"r"(lo_hi_2)
             : "cc");

#elif 0
// At the moment this is testing faster than the above on gcc,
// and slower on clang, using two_pow benchmarking.  My assumption
// is that once impl_unsigned_square_to_hilo() gets its own code,
// this will be a win - I'm guessing square is using this code at
// the moment and thus not able to use the simple optimization of
// skipping a redundant mult.
// So for now this section is disabled.

    H lo_lo_0, lo_lo_1, hi_lo_1, lo_hi_1, hi_hi_2;
    __asm__ ("mul %[lo_lo_0], %[u0], %[v0] \n\t"
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
             : [u0]"+&r"(u0), [v0]"+&r"(v0), [lo_lo_0]"=&r"(lo_lo_0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_hi_1]"=&r"(lo_hi_1), [hi_hi_2]"=&r"(hi_hi_2)
             : [v1]"r"(v1)
             : "cc");
    H hi_hi_3 = u1;

#else
    H lo_lo_0, lo_lo_1, hi_lo_1, lo_hi_1, hi_hi_2;
    __asm__ ("mul %[lo_lo_0], %[u0], %[v0] \n\t"
             "umulh %[lo_lo_1], %[u0], %[v0] \n\t"
             "mul %[hi_lo_1], %[u1], %[v0] \n\t"
             "mul %[lo_hi_1], %[u0], %[v1] \n\t"
             "umulh %[v0], %[u1], %[v0] \n\t"   /* v0 = hi_lo_2 */
             "mul %[hi_hi_2], %[u1], %[v1] \n\t"
             "umulh %[u0], %[u0], %[v1] \n\t"   /* u0 = lo_hi_2 */
             "umulh %[u1], %[u1], %[v1] \n\t"   /* u1 = hi_hi_3 */
             "adds %[lo_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[v0], %[hi_hi_2] \n\t"   /* hi_hi_2 += hi_lo_2 */
             "cset %[v0], hs \n\t"                       /* v0 = hi_hi_2_carry */
             "adds %[lo_lo_1], %[lo_hi_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[u0], %[hi_hi_2] \n\t"   /* hi_hi_2 += lo_hi_2 */
             "adcs %[u1], %[v0], %[u1] \n\t"             /* hi_hi_3 += hi_hi_2_carry + carry */
             : [u0]"+&r"(u0), [v0]"+&r"(v0), [lo_lo_0]"=&r"(lo_lo_0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_hi_1]"=&r"(lo_hi_1), [hi_hi_2]"=&r"(hi_hi_2)
             : [v1]"r"(v1)
             : "cc");
    H hi_hi_3 = u1;

#endif

    lowProduct = (static_cast<T>(lo_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;
    return highProduct;
  }
};

#endif





}} // end namespace


#ifdef __clang__
#  pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
