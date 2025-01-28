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
//
// See also the utility trait class at the bottom of this file,
// is_valid_sized_uint<int BITS>


// Primary template (catch-all for any invalid BITS value).
// Has no type member.
template <int BITS, typename DUMMY=void> struct sized_uint
{
};

template <typename DUMMY> struct sized_uint<8, DUMMY>
{
    using type = std::uint8_t;
};
template <typename DUMMY> struct sized_uint<16, DUMMY>
{
    using type = std::uint16_t;
};
template <typename DUMMY> struct sized_uint<32, DUMMY>
{
    using type = std::uint32_t;
};
template <typename DUMMY> struct sized_uint<64, DUMMY>
{
    using type = std::uint64_t;
};

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
  // Some compilers support __uint128_t, so we'll specialize with it if possible
  template <typename DUMMY> struct sized_uint<128, DUMMY>
  {
      using type = __uint128_t;
  };
#elif (HURCHALLA_TARGET_BIT_WIDTH >= 128)
  // presumably if a CPU target is >= 128bit, type uint128_t will exist.
  template <typename DUMMY> struct sized_uint<128, DUMMY>
  {
      using type = std::uint128_t;
  };
#endif



// Utility trait to determine (at compile time) if a particular sized_uint is
// valid.  Usage example:
//   constexpr bool is_valid = is_valid_sized_uint<32>::value;
//
// Primary template:
template <int BITS, typename DUMMY = void>
struct is_valid_sized_uint
{ static constexpr bool value = false; };
#if __cplusplus < 201703L
  template <int BITS, typename DUMMY>
  constexpr bool is_valid_sized_uint<BITS, DUMMY>::value;
#endif

template <typename T>
struct helper_sized_uint_void {
  using type = void;
};
template <int BITS>
struct is_valid_sized_uint<BITS,
        typename helper_sized_uint_void<typename sized_uint<BITS>::type>::type>
{ static constexpr bool value = true; };
#if __cplusplus < 201703L
  template <int BITS>
  constexpr bool is_valid_sized_uint<BITS, typename
      helper_sized_uint_void<typename sized_uint<BITS>::type>::type>::value;
#endif



} // end namespace

#endif
