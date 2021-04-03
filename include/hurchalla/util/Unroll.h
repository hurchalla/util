// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_UNROLL_H_INCLUDED
#define HURCHALLA_UTIL_UNROLL_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#include <cstdint>

namespace hurchalla {


// This class provides a way to automatically fully unroll a loop of length N.
// See https://stackoverflow.com/a/28232338
// When you define your lambda for use with this class, you should probably mark
// it with HURCHALLA_INLINE_LAMBDA, to ensure that your unrolled code doesn't
// end up with function calls to the lambda.

template <std::size_t N>
struct Unroll {
    static_assert(N > 0, "");
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        Unroll<N - 1>::call(lambda);
        lambda(N - 1);
    }
};
template <>
struct Unroll<static_cast<std::size_t>(1)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
    }
};


} // end namespace

#endif
