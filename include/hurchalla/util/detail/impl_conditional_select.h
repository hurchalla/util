// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_CONDITIONAL_SELECT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_CONDITIONAL_SELECT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/programming_by_contract.h"
#include <type_traits>

namespace hurchalla { namespace detail {


struct ImplCSelectDefaultTag {};
struct ImplCSelectStandardTag {};
struct ImplCSelectMaskedTag {};


// primary template
template <typename T, class PTag>
struct impl_conditional_select {
  HURCHALLA_FORCE_INLINE static T call(bool cond, T a, T b)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(std::is_same<PTag, ImplCSelectStandardTag>::value, "");
    // It would be nice if there were a compiler intrinsic for emitting a
    // conditional move (cmov x86) or conditional select (csel ARM), but so far
    // no compiler provides it.  Nonetheless, the ternary operator generally
    // causes compilers to emit cmov on x86 and csel on ARM.
    // Note: implementing this with inline asm would probably work poorly.  The
    // compiler has an efficiency advantage that we do not, because it knows
    // what condition code flags were set by previous arithmetic instructions.
    // If we used inline asm, we would be forcing the compiler to write the
    // condition code from some previous instruction into a register, and then
    // we would need to retest that register in asm to regenerate the condition
    // code.
#if 1
    return (cond) ? a : b;
#else
    // clang's __builtin_unpredictable is interesting, though in practice it
    // doesn't seem to make any difference in generating cmov/csel.
# if defined(__clang__) && ((__clang_major__ > 3) || \
                           ((__clang_major__ == 3) && (__clang_minor__ >= 8)))
    return (__builtin_unpredictable(cond)) ? a : b;
# else
    return (cond) ? a : b;
# endif
#endif
  }
};

// specialization for ImplCSelectMaskedTag
template <typename T>
struct impl_conditional_select<T, ImplCSelectMaskedTag> {
  HURCHALLA_FORCE_INLINE static T call(bool cond, T a, T b)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    using U = typename extensible_make_unsigned<T>::type;
    using P = typename safely_promote_unsigned<U>::type;
    static_assert(!(ut_numeric_limits<T>::is_signed) ||
                    ((static_cast<T>(-1) == ~(static_cast<T>(0))) &&
                     (static_cast<T>(static_cast<U>(static_cast<T>(-1))) ==
                        static_cast<T>(-1))),
                 "If T is signed, it must use two's complement representation");
    P condp = static_cast<P>(cond);
    P mask = static_cast<P>(static_cast<P>(0) - condp);
    P maskflip = static_cast<P>(condp - static_cast<P>(1));
    P selection = (mask & static_cast<P>(a)) | (maskflip & static_cast<P>(b));
    T result = static_cast<T>(selection);
    HPBC_POSTCONDITION2(result == ((cond) ? a : b));
    return result;
  }
};

// specialization for ImplCSelectDefaultTag
template <typename T>
struct impl_conditional_select<T, ImplCSelectDefaultTag> {
#ifdef HURCHALLA_PREFER_MASKING_WITHIN_CSELECT
  // For this default implementation, let's use masks only if T is equal or
  // smaller in size to the native register size.
  template <typename T1 = T>
  HURCHALLA_FORCE_INLINE static
  typename std::enable_if<(ut_numeric_limits<T1>::digits <=
                             HURCHALLA_TARGET_BIT_WIDTH), T>::type
  call(bool cond, T a, T b)
  {
    return impl_conditional_select<T, ImplCSelectMaskedTag>::call(cond, a, b);
  }
  template <typename T1 = T>
  HURCHALLA_FORCE_INLINE static
  typename std::enable_if<(ut_numeric_limits<T1>::digits >
                             HURCHALLA_TARGET_BIT_WIDTH), T>::type
  call(bool cond, T a, T b)
  {
    return (cond) ? a : b;
  }
#else
  HURCHALLA_FORCE_INLINE static T call(bool cond, T a, T b)
  {
    return impl_conditional_select<T, ImplCSelectStandardTag>::call(cond, a, b);
  }
#endif
};


}} // end namespace

#endif
