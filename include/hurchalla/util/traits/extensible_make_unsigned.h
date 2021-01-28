// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_EXTENSIBLE_MAKE_UNSIGNED_H_INCLUDED
#define HURCHALLA_UTIL_EXTENSIBLE_MAKE_UNSIGNED_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#include <type_traits>

namespace hurchalla {


// primary template
template <typename T>
struct extensible_make_unsigned {
    static_assert(std::is_integral<T>::value,
          "You'll need to specialize extensible_make_unsigned for your type T");
    using type = typename std::make_unsigned<T>::type;
};

// ---Specializations---
//
// You can use the following as an example of how to specialize for a particular
// integer type you wish to use.
//
// ** If you specialize this type, you should probably also specialize
// extensible_make_signed (in extensible_make_signed.h) **

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
// explicit specializations for __int128_t
template<>
struct extensible_make_unsigned<__int128_t> {
    using type = __uint128_t;
};
template<>
struct extensible_make_unsigned<const __int128_t> {
    using type = const __uint128_t;
};
template<>
struct extensible_make_unsigned<volatile __int128_t> {
    using type = volatile __uint128_t;
};
template<>
struct extensible_make_unsigned<const volatile __int128_t> {
    using type = const volatile __uint128_t;
};

// explicit specializations for __uint128_t
template<>
struct extensible_make_unsigned<__uint128_t> {
    using type = __uint128_t;
};
template<>
struct extensible_make_unsigned<const __uint128_t> {
    using type = const __uint128_t;
};
template<>
struct extensible_make_unsigned<volatile __uint128_t> {
    using type = volatile __uint128_t;
};
template<>
struct extensible_make_unsigned<const volatile __uint128_t> {
    using type = const volatile __uint128_t;
};
#endif


}  // end namespace

#endif
