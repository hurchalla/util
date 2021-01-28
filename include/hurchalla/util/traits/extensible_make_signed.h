// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_EXTENSIBLE_MAKE_SIGNED_H_INCLUDED
#define HURCHALLA_UTIL_EXTENSIBLE_MAKE_SIGNED_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#include <type_traits>

namespace hurchalla {


// primary template
template <typename T>
struct extensible_make_signed {
    static_assert(std::is_integral<T>::value,
            "You'll need to specialize extensible_make_signed for your type T");
    using type = typename std::make_signed<T>::type;
};

// ---Specializations---
//
// You can use the following as an example of how to specialize for a particular
// integer type you wish to use.
//
// ** If you specialize this type, you should probably also specialize
// extensible_make_unsigned (in extensible_make_unsigned.h) **

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
// explicit specializations for __int128_t
template<>
struct extensible_make_signed<__int128_t> {
    using type = __int128_t;
};
template<>
struct extensible_make_signed<const __int128_t> {
    using type = const __int128_t;
};
template<>
struct extensible_make_signed<volatile __int128_t> {
    using type = volatile __int128_t;
};
template<>
struct extensible_make_signed<const volatile __int128_t> {
    using type = const volatile __int128_t;
};

// explicit specializations for __uint128_t
template<>
struct extensible_make_signed<__uint128_t> {
    using type = __int128_t;
};
template<>
struct extensible_make_signed<const __uint128_t> {
    using type = const __int128_t;
};
template<>
struct extensible_make_signed<volatile __uint128_t> {
    using type = volatile __int128_t;
};
template<>
struct extensible_make_signed<const volatile __uint128_t> {
    using type = const volatile __int128_t;
};
#endif


}  // end namespace

#endif
