// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_BRANCHLESS_SHIFT_RIGHT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {


// primary template
template <typename T, class Enable = void>
struct impl_branchless_shift_right {
  // handles types T that are two times larger than the native bit width


  HURCHALLA_FORCE_INLINE static T call_asm(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < ut_numeric_limits<T>::digits);

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
    __asm__ ("shrdq %%cl, %[ahi], %[alo] \n\t"
             "shrq %%cl, %[ahi] \n\t"
             "testb $64, %%cl \n\t"
             "cmovnzq %[ahi], %[alo] \n\t"
             "cmovnzq %[zero], %[ahi] \n\t"
             : [alo]"+&r"(alo), [ahi]"+&r"(ahi)
             : [zero]"r"(zero), [ushift]"c"(ushift)
             : "cc");
    T result = (static_cast<T>(ahi) << 64) | alo;
# else  // clang, if T == __uint128_t, since MSVC doesn't support __uint128_t
    // Clang compiler's plain shift is fine, since clang never seems to use
    // branches for __uint128_t shifts.
    T result = a >> ushift;
# endif

#elif defined(HURCHALLA_TARGET_ISA_ARM_64)
# if defined(__GNUC__) && !defined(__clang__)   // gcc
    static_assert(ut_numeric_limits<T>::digits == 128, "");
    static_assert(ut_numeric_limits<H>::digits == 64, "");
    uint32_t ushift32 = static_cast<uint32_t>(ushift);
    uint32_t ushift_not = static_cast<uint32_t>(~ushift32);
    uint64_t ahi_left1 = ahi << 1;
    uint64_t hi_shifted = ahi >> (ushift32 % 64);
    uint64_t lo_shifted = alo >> (ushift32 % 64);
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    //     And thus  not(x) % 64 == (64 - 1 - x) % 64
    uint64_t cross_shifted = ahi_left1 << (ushift_not % 64);
    uint64_t lo_result = cross_shifted | lo_shifted;
//    lo_result = (ushift32 & 64) ? hi_shifted : lo_result;
//    uint64_t hi_result = (ushift32 & 64) ? 0 : hi_shifted;
    uint64_t hi_result;
    __asm__ ("tst %w[ushift32], #64 \n\t"
             "csel %[lo_result], %[hi_shifted], %[lo_result], ne \n\t"
             "csel %[hi_result], xzr, %[hi_shifted], ne \n\t"
             : [lo_result]"+&r"(lo_result), [hi_result]"=r"(hi_result)
             : [ushift32]"r"(ushift32), [hi_shifted]"r"(hi_shifted)
             : "cc");
    T result = (static_cast<T>(hi_result) << 64) | lo_result;
# else  // clang, if T == __uint128_t, since MSVC doesn't support __uint128_t
    // Clang compiler's plain shift is fine, since clang never seems to use
    // branches for __uint128_t shifts.
    T result = a >> ushift;
# endif

#else   // RISC-V, PPC, etc.  No asm is written for these ISAs yet.
    T result = a >> ushift;
#endif

    return result;
  }


  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < ut_numeric_limits<T>::digits);

    unsigned int ushift = static_cast<unsigned int>(shift);

#if defined(HURCHALLA_TARGET_ISA_ARM_64)
# if defined(__GNUC__) && !defined(__clang__)   // gcc
    // We use this code since it's almost as good as gcc's basic compiler
    // generated shift, and it seems to prevent gcc's frequent use of branches
    // in the gcc compiler's normal shift when T == __uint128_t.
    static_assert(is_valid_sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::value, "");
    using H = hurchalla::sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::type;
    constexpr unsigned int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    H ahi = static_cast<H>(a >> HALFBITS);
    H alo = static_cast<H>(a);

    H hi_shifted = ahi >> (ushift % HALFBITS);
    H mask = static_cast<H>(ushift >> 6) - 1u;
    H hi_result = hi_shifted & mask;

    H lo_shifted = alo >> (ushift % HALFBITS);
    //H cross_shifted = (ahi << ((HALFBITS - 1 - ushift) % HALFBITS)) << 1;
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    unsigned int ushift_not = ~ushift;
    //H cross_shifted = (ahi << ((HALFBITS + ushift_not) % HALFBITS)) << 1;
    H ahi_left1 = ahi << 1;
    H cross_shifted = ahi_left1 << (ushift_not % HALFBITS);

    H lo_result = (hi_shifted & ~mask) | ((lo_shifted | cross_shifted) & mask);
    T result = (static_cast<T>(hi_result) << HALFBITS) | lo_result;
# else  // clang, if T == __uint128_t, since MSVC doesn't support __uint128_t
    // Clang compiler's plain shift is fine, since clang never seems to use
    // branches for __uint128_t shifts.
    T result = a >> ushift;
# endif

#else   // X86, RISC-V, PPC, etc
    //note: For gcc we might want to use specific code like we do for ARM64
    // above - quite possibly using the exact same code.
    T result = a >> ushift;
#endif

    return result;
  }
};



template <typename T>
struct impl_branchless_shift_right<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width

  // we don't provide (or need) asm for types T <= the size of the native bit width
  HURCHALLA_FORCE_INLINE static T call_asm(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < ut_numeric_limits<T>::digits);

    return static_cast<T>(a >> shift);
  }

  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < ut_numeric_limits<T>::digits);

    return static_cast<T>(a >> shift);
  }
};


}} // end namespace

#endif
