// Copyright (c) 2026 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_BRANCHLESS_TWO_TIMES_SHIFT_LEFT_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_BRANCHLESS_TWO_TIMES_SHIFT_LEFT_TO_HILO_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {



// primary template
template <typename T, class Enable = void>
struct impl_branchless_two_times_shift_left_to_hilo {
  // handles types T that are two times larger than the native bit width

  template <int bits, typename DUMMY=void>
  struct log2bits {
    // this assert should eventually reject any non-power of 2 during recursion,
    // which is what we want when calling log2 on the number of bits of a type.
    static_assert(bits % 2 == 0 && bits >= 2, "");
    static constexpr int value = 1 + log2bits<bits/2,DUMMY>::value;
  };
  template <typename DUMMY>
  struct log2bits<1, DUMMY> {
    static constexpr int value = 0;
  };


  HURCHALLA_FORCE_INLINE static T call_asm(T& lowResult, T a, int shift)
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
    // Gcc very often uses branches for the compiler generated shifts for
    // __uint128_t, which is terrible for perf.  We avoid this via inline asm.
    // Clang doesn't seem to generate shifts for __uint128_t, but the asm here
    // is considerably simpler than what clang would generate, so we use it for
    // clang too.
    static_assert(std::numeric_limits<T>::digits == 128, "");
    static_assert(std::numeric_limits<H>::digits == 64, "");
    uint64_t ushift64 = static_cast<uint64_t>(ushift);
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    //     And thus  not(x) % 64 == (64 - 1 - x) % 64
    uint64_t ushift_not = static_cast<uint64_t>(~ushift);
    uint64_t part0 = alo;
    __asm__ ("shrdq %%cl, %[ahi], %[alo] \n\t"    /* alo = part1 */
             "shrq %%cl, %[ahi] \n\t"             /* ahi = part2 */
             "movl %k[ushift64], %%ecx \n\t"
             "shlq %%cl, %[part0] \n\t"
             "add %[part0], %[part0] \n\t"
             "xorl %%ecx, %%ecx \n\t"
             "testb $64, %b[ushift64] \n\t"
             "xorl %k[ushift64], %k[ushift64] \n\t"
             "cmovzq %[part0], %%rcx \n\t"
             "cmovzq %[alo], %[part0] \n\t"
             "cmovzq %[ahi], %[alo] \n\t"
             "cmovzq %[ushift64], %[ahi] \n\t"
             : [ahi]"+&r"(ahi), [alo]"+&r"(alo), [ushift64]"+&r"(ushift64),
               [ushift_not]"+&c"(ushift_not), [part0]"+&r"(part0)
             :
             : "cc");
    lowResult = (static_cast<T>(part0) << 64) | ushift_not;
    return (static_cast<T>(ahi) << 64) | alo;

#elif defined(HURCHALLA_TARGET_ISA_ARM_64)
    // Gcc very often uses branches for the compiler generated shifts for
    // __uint128_t, which is terrible for perf.  We avoid this via inline asm.
    // Clang doesn't seem to generate shifts for __uint128_t, but the asm here
    // is considerably simpler than what clang would generate for a __uint128_t
    // shift, so we use it for clang too.
    static_assert(ut_numeric_limits<T>::digits == 128, "");
    static_assert(ut_numeric_limits<H>::digits == 64, "");
    uint32_t ushift32 = static_cast<uint32_t>(ushift);
    uint32_t ushift_not = static_cast<uint32_t>(~ushift32);
    uint64_t hi_shifted = (ahi << (ushift32 % 64)) << 1;
    uint64_t part0 = (alo << (ushift32 % 64)) << 1;
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    //     And thus  not(x) % 64 == (64 - 1 - x) % 64
    uint64_t cross_shifted = alo >> (ushift_not % 64);

    uint64_t part1 = hi_shifted | cross_shifted;
    uint64_t part2 = ahi >> (ushift_not % 64);

    uint64_t lo_lo;
    __asm__ ("tst %w[ushift32], #64 \n\t"
             "csel %[lo_lo], xzr, %[part0], ne \n\t"
             "csel %[part0], %[part0], %[part1], ne \n\t"
             "csel %[part1], %[part1], %[part2], ne \n\t"
             "csel %[part2], %[part2], xzr, ne \n\t"
             : [part0]"+&r"(part0), [part1]"+&r"(part1), [part2]"+&r"(part2),
               [lo_lo]"=&r"(lo_lo)
             : [ushift32]"r"(ushift32)
             : "cc");
    lowResult = (static_cast<T>(part0) << 64) | lo_lo;
    return (static_cast<T>(part2) << 64) | part1;

#else   // RISC-V, PPC, etc.  No asm is written for these ISAs yet, so call the
        // non-asm version.
    return call(lowResult, a, shift);
#endif
  }


  HURCHALLA_FORCE_INLINE static T call(T& lowResult, T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static constexpr int digitsT = ut_numeric_limits<T>::digits;
    static_assert(digitsT == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < digitsT);

    unsigned int ushift = static_cast<unsigned int>(shift);

    static_assert(is_valid_sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::value, "");
    using H = hurchalla::sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::type;

    constexpr unsigned int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    H ahi = static_cast<H>(a >> HALFBITS);
    H alo = static_cast<H>(a);

#if defined(HURCHALLA_AVOID_CSELECT) || (defined(__GNUC__) && !defined(__clang__))
    // We use this code since it's almost as good as gcc's basic compiler
    // generated shift, and it prevents gcc's frequent use of branches in the
    // gcc compiler's normal shift when T == __uint128_t.
    H part0 = (alo << (ushift % HALFBITS)) << 1;
    H mask = static_cast<H>(ushift >> (log2bits<HALFBITS>::value)) - 1u;
    HPBC_UTIL_ASSERT2(mask == 0 || mask == static_cast<H>(-1));
    H lolo_result = part0 & mask;

    H part1 = (ahi << (ushift % HALFBITS)) << 1;
    // H cross_shifted = alo >> ((HALFBITS - 1u - ushift) % HALFBITS);
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    unsigned int ushift_not = ~ushift;
    // H cross_shifted = alo >> ((HALFBITS + ushift_not) % HALFBITS);
    H cross_shifted = alo >> (ushift_not % HALFBITS);
    part1 = part1 | cross_shifted;

    H part2 = ahi >> (ushift_not % HALFBITS);

    H lohi_result = (part0 & ~mask) | (part1 & mask);
    lowResult = (static_cast<T>(lohi_result) << HALFBITS) | lolo_result;

    H hilo_result = (part1 & ~mask) | (part2 & mask);
    H hihi_result = (part2 & ~mask);
    return (static_cast<T>(hihi_result) << HALFBITS) | hilo_result;

#else
    // We can usually assume we're compiling with clang in this section, unless
    // we're compiling for an ISA like RISC-V or PPC or something even more
    // unusual.  Note MSVC doesn't support __uint128_t, so it would be unusual
    // to be inside this primary template (which requires a type >= 128bit) when
    // compiling with MSVC.
#  if 0
    // This section would be basically fine with clang, since clang never seems
    // to use branches for simple __uint128_t shifts.  However, for arm64 clang,
    // the #else code produces much better asm than this section, and for x64
    // clang, the #else produces slightly better asm.
    static_cast<void>(ahi);    // silence unused variable warnings
    static_cast<void>(alo);    // silence unused variable warnings
    static_cast<void>(ushift); // silence unused variable warnings
    lowResult = static_cast<T>((a << shift) << 1);
    int rshift = digitsT - (shift + 1);
    HPBC_UTIL_ASSERT2(0 <= rshift && rshift < digitsT);
    return static_cast<T>(a >> rshift);
#  else
    H part0 = (alo << (ushift % HALFBITS)) << 1;
    H part1 = (ahi << (ushift % HALFBITS)) << 1;

    // H cross_shifted = alo >> ((HALFBITS - 1u - ushift) % HALFBITS);
    //     As an identity: -x == not(x) + 1
    //     As an identity: not(x) == -1 - x
    unsigned int ushift_not = ~ushift;
    // H cross_shifted = alo >> ((HALFBITS + ushift_not) % HALFBITS);
    H cross_shifted = alo >> (ushift_not % HALFBITS);
    part1 = part1 | cross_shifted;

    H part2 = ahi >> (ushift_not % HALFBITS);

    H lolo_result = (ushift & HALFBITS) ? 0 : part0;
    H lohi_result = (ushift & HALFBITS) ? part0 : part1;
    H hilo_result = (ushift & HALFBITS) ? part1 : part2;
    H hihi_result = (ushift & HALFBITS) ? part2 : 0;

    lowResult = (static_cast<T>(lohi_result) << HALFBITS) | lolo_result;
    return (static_cast<T>(hihi_result) << HALFBITS) | hilo_result;
#  endif

#endif
  }
};





template <typename T>
struct impl_branchless_two_times_shift_left_to_hilo<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width

  // we don't provide (or need) asm for types T <= the size of the native bit width
  HURCHALLA_FORCE_INLINE static T call_asm(T& lowResult, T a, int shift)
  {
    return call(lowResult, a, shift);
  }

  HURCHALLA_FORCE_INLINE static T call(T& lowResult, T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static constexpr int digitsT = ut_numeric_limits<T>::digits;
    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < digitsT);

    lowResult = static_cast<T>((a << shift) << 1);
    int rshift = digitsT - (shift + 1);
    HPBC_UTIL_ASSERT2(0 <= rshift && rshift < digitsT);
    return static_cast<T>(a >> rshift);
  }
};


}} // end namespace

#endif
