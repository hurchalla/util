// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IS_EQUALITY_COMPARABLE_H_INCLUDED
#define HURCHALLA_UTIL_IS_EQUALITY_COMPARABLE_H_INCLUDED

#include <type_traits>
#include <utility>

namespace hurchalla {


namespace detail {   // internal impl for is_equality_comparable
#if 1
  // This #if version requires that an equality operator return type bool.
  // adapted from https://stackoverflow.com/a/36360646
  template <typename T, typename = bool>
  struct impl_is_equality_comparable : std::false_type {};
  
  template <typename T>
  struct impl_is_equality_comparable<T,
      decltype(std::declval<T>()==std::declval<T>())> 
      : std::true_type {};
#else
  // This #if version does not require that an equality operator return bool.
  // adapted from https://stackoverflow.com/a/44531666
  template <typename T, typename = void>
  struct impl_is_equality_comparable : std::false_type {};
  
  template <typename T>
  struct impl_is_equality_comparable<T,
      decltype(std::declval<T>()==std::declval<T>(), void(), void())>
      : std::true_type {};
#endif
} // end namespace detail

template <typename T>
struct is_equality_comparable : detail::impl_is_equality_comparable<T> {};


// Unfortunately, is_equality_comparable<CanonicalValue>::value is only capable
// of letting us know if an equality operator exists (and that it returns bool).
// In a perfect world we would like to also get assurance that the following is
// true:
// x == x
// (x == y) == (y == x)
// !(x == y) == (x != y)
// !(x != y) == (x == y)
// if (x == y && y == z) then (x == z)
// Has ops == and !=, such that there exists  convertableToT (op) T  and
// T (op) convertableToT  and they return bool.  (we only have assurance that
// there exists a function for T == convertableToT)


}  // end namespace hurchalla

#endif
