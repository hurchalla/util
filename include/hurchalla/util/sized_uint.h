// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_SIZED_UINT_H_INCLUDED
#define HURCHALLA_UTIL_SIZED_UINT_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#include <cstdint>

namespace hurchalla {


// Example of use:
// template <typename T> void foo() {
//   static_assert(hurchalla::ut_numeric_limits<T>::is_integer, "");
//   static_assert(!hurchalla::ut_numeric_limits<T>::is_signed, "");
//   constexpr int bits = hurchalla::ut_numeric_limits<T>::digits;
//   using T2 = typename hurchalla::sized_uint<bits*2>::type;
// }


// primary template (catch-all for any invalid BITS value)
template <int BITS, typename DUMMY=void> struct sized_uint
{
    static constexpr bool is_valid = false;
    using type = void;
};
template <int BITS, typename DUMMY>
constexpr bool sized_uint<BITS, DUMMY>::is_valid;


template <typename DUMMY> struct sized_uint<8, DUMMY>
{
    static constexpr bool is_valid = true;
    using type = std::uint8_t;
};
template <typename DUMMY> constexpr bool sized_uint<8, DUMMY>::is_valid;

template <typename DUMMY> struct sized_uint<16, DUMMY>
{
    static constexpr bool is_valid = true;
    using type = std::uint16_t;
};
template <typename DUMMY> constexpr bool sized_uint<16, DUMMY>::is_valid;

template <typename DUMMY> struct sized_uint<32, DUMMY>
{
    static constexpr bool is_valid = true;
    using type = std::uint32_t;
};
template <typename DUMMY> constexpr bool sized_uint<32, DUMMY>::is_valid;

template <typename DUMMY> struct sized_uint<64, DUMMY>
{
    static constexpr bool is_valid = true;
    using type = std::uint64_t;
};
template <typename DUMMY> constexpr bool sized_uint<64, DUMMY>::is_valid;

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
  // Some compilers support __uint128_t, so we'll specialize with it if possible
  template <typename DUMMY> struct sized_uint<128, DUMMY>
  {
      static constexpr bool is_valid = true;
      using type = __uint128_t;
  };
  template <typename DUMMY> constexpr bool sized_uint<128, DUMMY>::is_valid;
#elif (HURCHALLA_TARGET_BIT_WIDTH >= 128)
  // presumably if a CPU target is >= 128bit, type uint128_t will exist.
  template <typename DUMMY> struct sized_uint<128, DUMMY>
  {
      static constexpr bool is_valid = true;
      using type = std::uint128_t;
  };
  template <typename DUMMY> constexpr bool sized_uint<128, DUMMY>::is_valid;
#endif


} // end namespace

#endif
