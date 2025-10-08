// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SIGNED_SQUARE_TO_HILO_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_signed_multiply_to_hilo_product.h"

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_signed_square_to_hilo_product {
  HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u)
  {
      return impl_signed_multiply_to_hilo_product<T>::call(lowProduct, u, u);
  }
};


}} // end namespace

#endif
