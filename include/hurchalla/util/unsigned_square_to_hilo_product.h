// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_UNSIGNED_SQUARE_TO_HILO_H_INCLUDED

// note: in order to get the inline asm (potentially faster) version of this
// function, you must define HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO.  This
// doesn't apply to MSVC since MSVC doesn't support inline asm.


#include "hurchalla/util/detail/platform_specific/impl_unsigned_square_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"

namespace hurchalla {


// unsigned_square_to_hilo_product() calculates a 'double-width' multiplication
// product.  This behavior differs from a 'standard' multiply which drops the
// highest bits of the product whenever overflow occurs.
//
// Returns the high-bit portion of the product, and stores the low-bit portion
// in lowProduct.
template <typename T>
HURCHALLA_FORCE_INLINE
T unsigned_square_to_hilo_product(T& lowProduct, T a)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!(ut_numeric_limits<T>::is_signed), "");
    // POSTCONDITION: Stores the low-bits portion of the product (a*a) in
    //                lowProduct.
    // POSTCONDITION: Returns the high-bits portion of the product (a*a).

    return detail::impl_unsigned_square_to_hilo_product<T>::call(lowProduct, a);
}


} // end namespace

#endif
