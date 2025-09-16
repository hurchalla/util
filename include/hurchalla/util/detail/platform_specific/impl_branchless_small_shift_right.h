// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_BRANCHLESS_SMALL_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_BRANCHLESS_SMALL_SHIFT_RIGHT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {


// primary template
template <typename T, class Enable = void>
struct impl_branchless_small_shift_right {
  // handles types T that are two times larger than the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < HURCHALLA_TARGET_BIT_WIDTH);

#if defined(HURCHALLA_TARGET_ISA_X86_64)
    // x86 clang and gcc produce optimal asm, if we limit the shift amount
    T result = a >> (static_cast<unsigned int>(shift) % HURCHALLA_TARGET_BIT_WIDTH);
#else
    constexpr int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    static_assert(is_valid_sized_uint<HALFBITS>::value, "");
    using H = hurchalla::sized_uint<HALFBITS>::type;

    H hi_a = static_cast<H>(a >> HALFBITS);
    H lo_a = static_cast<H>(a);

    H hi_result = hi_a >> shift;
    HPBC_UTIL_ASSERT2(shift <= HALFBITS - 1);
    H lo_result = ((hi_a << (HALFBITS - 1 - shift)) << 1) | (lo_a >> shift);

    T result = (static_cast<T>(hi_result) << HALFBITS) | lo_result;
#endif

    return result;
  }
};

template <typename T>
struct impl_branchless_small_shift_right<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    HPBC_UTIL_PRECONDITION2(0 <= shift && shift < ut_numeric_limits<T>::digits);
    HPBC_UTIL_PRECONDITION2(shift < HURCHALLA_TARGET_BIT_WIDTH);

    return static_cast<T>(a >> shift);
  }
};


}} // end namespace

#endif
