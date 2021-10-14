// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_SAFELY_PROMOTE_UNSIGNED_H_INCLUDED
#define HURCHALLA_UTIL_SAFELY_PROMOTE_UNSIGNED_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include <type_traits>

namespace hurchalla {


// safely_promote_unsigned<T> is intended to protect against the undefined
// behavior and unexpected results that can arise from unsigned integral
// promotion in C++.  For details on these issues, see
// https://jeffhurchalla.com/2019/01/16/c-c-surprises-and-undefined-behavior-due-to-unsigned-integer-promotion/
// https://www.nayuki.io/page/summary-of-c-cpp-integer-rules

// If an unsigned type T would get promoted to (signed) 'int', we want to make
// sure that the type safely_promote_unsigned<T> provides is 'unsigned int'.
// Otherwise the type it provides is T.


template <typename T>
struct safely_promote_unsigned_helper {
    using type = decltype(0u + static_cast<T>(0));
};
template <typename T>
struct safely_promote_unsigned_helper<const T> {
    using type = const typename safely_promote_unsigned_helper<T>::type;
};
template <typename T>
struct safely_promote_unsigned_helper<volatile T> {
    using type = volatile typename safely_promote_unsigned_helper<T>::type;
};
template <typename T>
struct safely_promote_unsigned_helper<const volatile T> {
    using type= const volatile typename safely_promote_unsigned_helper<T>::type;
};

// Note that this primary template will be enabled for non-native unsigned
// integral types.  C++ never promotes non-native integer types, so this primary
// template just provides back the type T.
template <typename T, typename Enable = void>
struct safely_promote_unsigned {
    // If you get a compile error on these static_asserts despite the fact that
    // you know that your type T is an unsigned integral type, then ideally you
    // should add a specialization for your type to ut_numeric_limits (see
    // ut_numeric_limits.h).  A quick and dirty alternative is to remove the
    // static asserts below, though of course this removes safety checking
    // against someone improperly using safely_promote_unsigned.
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    using type = T;
};

// Note that this specialization will be enabled for native unsigned integral
// types (std::is_unsigned<T>::value is true only for native unsigned int types)
template <typename T>
struct safely_promote_unsigned<T, 
                    typename std::enable_if<std::is_unsigned<T>::value>::type> {
    using type = typename safely_promote_unsigned_helper<T>::type;
};


} // end namespace

#endif
