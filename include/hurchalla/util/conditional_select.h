// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_CONDITIONAL_SELECT_H_INCLUDED
#define HURCHALLA_UTIL_CONDITIONAL_SELECT_H_INCLUDED


#include "hurchalla/util/detail/impl_conditional_select.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include <type_traits>

namespace hurchalla {


using CSelectStandardTag = detail::ImplCSelectStandardTag;
using CSelectMaskedTag = detail::ImplCSelectMaskedTag;
using CSelectDefaultTag = detail::ImplCSelectDefaultTag;

// Returns (cond) ? a : b.
// If PerfTag is ConditionalSelectStandardTag, this will be evaluated (usually)
// using conditional select or conditional move instructions, typically compiled
// from the ternary operation ((cond) ? a : b).
// If PerfTag is ConditionalSelectMaskedTag, it will be evaluated using
// bitwise operations via bitmasks.  
template <typename T, class PerfTag = CSelectDefaultTag>
HURCHALLA_FORCE_INLINE T conditional_select(bool cond, T a, T b)
{
    static_assert(ut_numeric_limits<T>::is_integer, "");
    return detail::impl_conditional_select<T, PerfTag>::call(cond, a, b);
}


} // end namespace

#endif
