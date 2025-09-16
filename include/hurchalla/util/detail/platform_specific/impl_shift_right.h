// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SHIFT_RIGHT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SHIFT_RIGHT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {




// ---   !!!!!!!
//
// --- This is currently a PLACEHOLDER version until I create the real impl,
// --- similar to how I wrote the corresponding function in impl_shift_left.h
//
// ---   !!!!!!!


// primary template
template <typename T, class Enable = void>
struct impl_shift_right {
  // handles types T that are two times larger than the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    static_assert(ut_numeric_limits<T>::digits == 2 * HURCHALLA_TARGET_BIT_WIDTH, "");
    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);

    static_assert(is_valid_sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::value, "");
    using H = hurchalla::sized_uint<HURCHALLA_TARGET_BIT_WIDTH>::type;

    constexpr unsigned int HALFBITS = HURCHALLA_TARGET_BIT_WIDTH;
    H ahi = static_cast<H>(a >> HALFBITS);
    H alo = static_cast<H>(a);
    static_cast<void>(ahi); // silence unused variable warnings
    static_cast<void>(alo); // silence unused variable warnings
    unsigned int ushift = static_cast<unsigned int>(shift);


// !!!!! TODO !!!!!!
    T result = a >> ushift;     // Replace this.
// !!!!! TODO !!!!!!

    return result;
  }
};


template <typename T>
struct impl_shift_right<T, typename
        std::enable_if<(ut_numeric_limits<T>::digits <= HURCHALLA_TARGET_BIT_WIDTH)>::type> {
  // handles types T that are smaller than or the same size as the native bit width
  HURCHALLA_FORCE_INLINE static T call(T a, int shift)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");

    HPBC_UTIL_PRECONDITION2(shift < ut_numeric_limits<T>::digits);

    return static_cast<T>(a >> shift);
  }
};


}} // end namespace

#endif
