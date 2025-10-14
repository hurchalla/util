// Copyright (c) 2025 Jeffrey Hurchalla
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





// Inline asm summary/conclusion from these X64 (Zen4) timings -
// For x64:
// We should use all-asm for gcc (pretty clear cut), and for clang we should use
// partial-asm (this loses on montquarter scalar, but is a win or tossup
// everywhere else, and we prefer to prioritize array pow over scalar pow anyway
// since array is faster).
// For gcc, a side benefit is the compiler is less likely to have the bad luck
// cases where it makes bad decisions and produces terrible machine code, if we
// use all-asm.
//
// -- Benchmark Timings --
// Montquarter two pow scalar:
// gcc with all-asm square: no-asm 2.2583  partial-asm 2.3200  *all-asm 2.0015
// clang with partial-asm square: no-asm 2.0142  partial-asm 1.9678  all-asm 1.9605
// clang with all-asm square: no-asm 1.9672  (tossup)partial-asm 1.9283  (tossup)all-asm 1.9282
// Montfull two pow scalar:
// gcc with all-asm square: no-asm 2.4615  partial-asm 2.5842  *all-asm 2.0770
// clang with partial-asm square: no-asm 2.0737  *partial-asm 2.0189  all-asm 2.1128
// clang with all-asm square: no-asm 2.1354  partial-asm 2.1175  all-asm 2.1040
// Montfull two pow array:
// gcc with all-asm square: no-asm 1.7254  partial-asm 1.7241  *all-asm 1.7195
// clang with partial-asm square: no-asm 1.4690  *partial-asm 1.4633  all-asm 1.4724
// clang with all-asm square: no-asm 1.4805  partial-asm 1.4858  all-asm 1.4796
// Montquarter two pow array:
// gcc with all-asm square: no-asm 1.5246  (tossup)partial-asm 1.5214  (tossup)all-asm 1.5213
// clang with partial-asm square: no-asm 1.2943  (tossup)partial-asm 1.2928  (tossup)all-asm 1.2932
// clang with all-asm square: no-asm 1.3358  (tossup)partial-asm 1.3360  (tossup)all-asm 1.3353


// X64 (Zen 4) Updated Timings:
// These should be quite a bit more reliable than the above for estimating perf.
// Conclusions: gcc use all-asm.  clang prefer partial asm, but all-asm is fine.
// This is incidentally almost the same conclusions as above.
// Montquarter two pow scalar:
// gcc with all-asm square: no-asm 2.2558  partial-asm 2.3230  all-asm 2.0051
// clang with all-asm square: no-asm 2.0114  partial-asm 1.9669  all-asm 1.9605
// Montfull two pow scalar:
// gcc with all-asm square: no-asm 2.4131  partial-asm 2.4531  all-asm 2.0792
// clang with all-asm square: no-asm 2.0694  partial-asm 2.0221  all-asm 2.0339
// Montfull two pow array:
// gcc with all-asm square: no-asm 1.9274  partial-asm 1.9011  all-asm 1.5146
// clang with all-asm square: no-asm 1.6425  partial-asm 1.5238  all-asm 1.5561
// Montquarter two pow array:
// gcc with all-asm square: no-asm 1.7247  partial-asm 1.9813  all-asm 1.4472
// clang with all-asm square: no-asm 1.5986  partial-asm 1.4786  all-asm 1.4797


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_X86_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

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

#if defined(__clang__)

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
             "adcq %[u1], %[hi_lo_2] \n\t" /* hi_lo_2 += lo_hi_2 + carry */
             "adcq $0, %%rdx \n\t"         /* hi_hi_3 += carry */
             "addq %[u0], %[lo_lo_1] \n\t" /* lo_lo_1 += lo_hi_1 */
             "adcq %%rax, %[hi_lo_2] \n\t" /* hi_lo_2 += hi_hi_2 + carry */
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

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, v);
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
//   (The tests with no-asm square don't count for much since no-asm square is
//   slower than we'd likely use.)
//
// -- Benchmark Timings --
// Montquarter two pow scalar:
// gcc with no-asm square: no-asm 2.5080  partial-asm 2.5965  *all-asm 2.4842
// gcc with partial-asm square: no-asm 2.3966  partial-asm 2.3788  *all-asm 2.2763
// gcc with full-asm square: no-asm 2.4084  partial-asm 2.3592  *all-asm 2.2766
// clang with no-asm square: no-asm 2.3635   partial-asm 2.2705  *all-asm 2.2956
// clang with all-asm square: no-asm 2.3637   partial-asm 2.2819  *all-asm 2.2616
//
// Montfull two pow scalar:
// gcc with no-asm square: no-asm 2.4965   partial-asm 2.4014  *all-asm 2.4900
// gcc with all-asm square: no-asm 2.4931   partial-asm 2.4444  *all-asm 2.3789
// clang with no-asm square: no-asm 2.4950  partial-asm 2.3779  *all-asm 2.3932
// clang with partial-asm square: no-asm 2.4879  partial-asm 2.3864  *all-asm 2.3833
//
// Montfull two pow array:
// gcc with no-asm square: no-asm 1.2892  partial-asm 1.2894  *all-asm 1.2895
// gcc with partial-asm square: no-asm 1.2311  partial-asm 1.2349  *all-asm 1.2318
// clang with no-asm square: no-asm 1.2395  partial-asm 1.2391   *all-asm 1.2387
// clang with all-asm square: no-asm 1.1737  partial-asm 1.1729  *all-asm 1.1737
//
// Montquarter two pow array:
// gcc with no-asm square: no-asm 1.1738  partial-asm 1.1697  *all-asm 1.1654
// gcc with partial-asm square: no-asm 1.0992  partial-asm 1.0967  *all-asm 1.1075
// gcc with all-asm square: no-asm 1.0986  partial-asm 1.0983  *all-asm 1.1016
// clang with no-asm square: no-asm 1.0739  partial-asm 1.0741  *all-asm 1.0750
// clang with all-asm square: no-asm 1.0411  partial-asm 1.0415  *all-asm 1.0413


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO) || defined(HURCHALLA_ALLOW_INLINE_ASM_ALL))

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

#if 0
// In limited tests benchmarking montgomery two_pow, this partial-asm version
// tended to be a little slower than the full-asm version.  Thus we disable
// this version.

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
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             "adds %[lo_lo_1], %[lo_hi_1], %[lo_lo_1] \n\t"
             "adcs %[hi_hi_2], %[u0], %[hi_hi_2] \n\t"   /* hi_hi_2 += lo_hi_2 */
             "cinc %[u1], %[u1], hs \n\t"                /* hi_hi_3 += carry */
             : [u0]"+&r"(u0), [v0]"+&r"(v0), [lo_lo_0]"=&r"(lo_lo_0),
               [lo_lo_1]"=&r"(lo_lo_1), [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [lo_hi_1]"=&r"(lo_hi_1), [hi_hi_2]"=&r"(hi_hi_2)
             : [v1]"r"(v1)
             : "cc");
// note: we could use cset in place of the first cinc, and then end with an adcs
// instead of the final cinc.  But it probably makes no difference either way -
// in a previous commit I had an extra inline asm version that implemented this
// alternative, and limited benchmarking suggested the perf is identical.

    H hi_hi_3 = u1;
#endif

    lowProduct = (static_cast<T>(lo_lo_1) << shift) | lo_lo_0;
    T highProduct = (static_cast<T>(hi_hi_3) << shift) | hi_hi_2;

    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        T low2;
        T high2 = slow_unsigned_multiply_to_hilo_product::call(low2, u, v);
        HPBC_UTIL_POSTCONDITION2(lowProduct == low2 && highProduct == high2);
    }
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
