// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_SIGNED_SQUARE_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_SIGNED_SQUARE_TO_HILO_H_INCLUDED

// note: in order to get the inline asm (potentially faster) version of this
// function, you must define HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO.  This
// doesn't apply to MSVC since MSVC doesn't support inline asm.


#include "hurchalla/util/detail/platform_specific/impl_signed_square_to_hilo_product.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"

namespace hurchalla {


// signed_square_to_hilo_product() calculates a 'double-width' multiplication
// product.  This behavior differs from 'standard' signed multiplication which
// has undefined behavior if the product is too large to fit in type T (i.e.
// overflow is UB).  Since this function outputs to a double-width product, it
// never overflows the double-width, and as a result this function never
// produces undefined behavior.
//
// Returns the high-bit portion of the product, and stores the low-bit portion
// (as an unsigned T) in lowProduct.
template <typename T>
HURCHALLA_FORCE_INLINE
T signed_square_to_hilo_product(
                    typename extensible_make_unsigned<T>::type& lowProduct, T a)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(ut_numeric_limits<T>::is_signed, "");
    // POSTCONDITION: Stores the low-bits portion of the product (a*b) in
    //                lowProduct.
    // POSTCONDITION: Returns the high-bits portion of the product (a*b).

    T highProduct = detail::impl_signed_square_to_hilo_product<T>::call(
                                                                 lowProduct, a);
    HPBC_UTIL_POSTCONDITION(highProduct >= 0);
    return highProduct;
}


} // end namespace

#endif
