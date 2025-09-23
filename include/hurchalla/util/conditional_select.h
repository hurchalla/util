// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_CONDITIONAL_SELECT_H_INCLUDED
#define HURCHALLA_UTIL_CONDITIONAL_SELECT_H_INCLUDED


#include "hurchalla/util/detail/impl_conditional_select.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include <type_traits>

namespace hurchalla {

// Currently this function uses the C++ ternary operator to implement the core
// functionality, or bitwise operations and bitmasks (in the case of
// CSelectMaskedTag).  Neither method can guarantee branchless operation, since
// the implementation currently does not use any assembly.  Though the ternary
// operator is idiomatic for indicating to the compiler that you want it to use
// conditional select or conditional move, and bitmasks and bitwise operations
// are obviously idiomatic for using bitmasks and bitwise ops in machine code,
// the compiler can and will disregard these "hints" and translate this C++ code
// into machine code with branches whenever it believes it can produce better
// results by doing so.  The compiler has no way to know we definitively want
// branchless code, because currently no compiler provides any intrinsic,
// directive, or extension that requires it to use branchless machine code (or
// even that can request branchless machine code), and likewise, currently there
// is no C++ attribute to indicate this.  For this reason, conditional_select()
// serves overwhelmingly as a way to clearly indicate (to yourself) places in
// your code that should ideally be implemented with a branchless conditional
// select/move, rather than as a way to actually achieve the goal.  You can
// quickly implement your code, and later write assembly (or perhaps implement
// this function with assembly) if you need it.  It is much much more effective
// to later search for the string "conditional_select" than it is to later
// attempt to search across all your project code for a ternary operator "?" or
// an "&" for bitwise operations - even after rejecting occurences of "?" and
// "&" that have no relation to ternary operation or bitwise operations, those
// symbols can not remind you whether (or not) you definitely wanted or needed
// branchless code, just as those symbols can not provide any definitive answer
// as to whether the compiler will produce branchless machine code.

// For a limited conditional select (it selects based on whether a particular
// bit is 0 or 1 in a variable) which is implemented with assembly (currently
// only x64 and ARM64) and thus that can provide you with guaranteed branchless
// machine code for the asm supported ISAs, see cselect_on_bit.h.  Note that you
// must define a macro, as described in the file, to enable the inline asm that
// guarantees the branchless code.



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
