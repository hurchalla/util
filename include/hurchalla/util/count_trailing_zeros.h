// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_COUNT_TRAILING_ZEROS_H_INCLUDED
#define HURCHALLA_UTIL_COUNT_TRAILING_ZEROS_H_INCLUDED


#include "hurchalla/util/detail/impl_count_trailing_zeros.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/programming_by_contract.h"
#include "hurchalla/util/compiler_macros.h"

namespace hurchalla {


// From the gcc docs:
// "Returns the number of trailing 0-bits in x, starting at the least
// significant bit position.  If x is 0, the result is undefined."
template <typename T>
HURCHALLA_FORCE_INLINE int count_trailing_zeros(T x)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(!ut_numeric_limits<T>::is_signed, "");
    HPBC_PRECONDITION2(x != 0);

    return detail::impl_count_trailing_zeros<T>::call(x);
}


} // end namespace

#endif
