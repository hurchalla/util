// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_unsigned_multiply_to_hilo_product.h"

namespace hurchalla { namespace detail {


// primary template
template <typename T>
struct impl_unsigned_square_to_hilo_product {
  HURCHALLA_FORCE_INLINE static T call(T& lowProduct, T u)
  {
      return impl_unsigned_multiply_to_hilo_product<T>::call(lowProduct, u, u);
  }
};


}} // end namespace

#endif
