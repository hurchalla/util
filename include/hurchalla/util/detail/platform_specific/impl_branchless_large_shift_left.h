// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_BRANCHLESS_LARGE_SHIFT_LEFT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_BRANCHLESS_LARGE_SHIFT_LEFT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {


// primary template
template <typename T, class Enable = void>
struct impl_branchless_large_shift_left {
  // handles types T that are two times larger than the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    constexpr int bitsT = ut_numeric_limits<T>::digits;
    HPBC_UTIL_PRECONDITION2(0 <= shift);
    HPBC_UTIL_PRECONDITION2(bitsT - static_cast<int>(HURCHALLA_TARGET_BIT_WIDTH) <= shift);
    HPBC_UTIL_PRECONDITION2(shift < bitsT);

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    // due to the static_assert just above, we know that
    HPBC_UTIL_ASSERT2(HURCHALLA_TARGET_BIT_WIDTH <= shift && shift < bitsT);

    constexpr int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    static_assert(is_valid_sized_uint<HALFBITS>::value, "");
    using H = hurchalla::sized_uint<HALFBITS>::type;

    H lo_a = static_cast<H>(a);
    HPBC_UTIL_ASSERT2(shift >= HALFBITS);
    HPBC_UTIL_ASSERT2(shift < 2*HALFBITS);
    H hi_result = lo_a << (shift - HALFBITS);

    T result = static_cast<T>(hi_result) << HALFBITS;

    return result;
  }
};

template <typename T>
struct impl_branchless_large_shift_left<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    constexpr int bitsT = ut_numeric_limits<T>::digits;
    HPBC_UTIL_PRECONDITION2(0 <= shift);
    HPBC_UTIL_PRECONDITION2(bitsT - static_cast<int>(HURCHALLA_TARGET_BIT_WIDTH) <= shift);
    HPBC_UTIL_PRECONDITION2(shift < bitsT);

    return static_cast<T>(a << shift);
  }
};


}} // end namespace

#endif
