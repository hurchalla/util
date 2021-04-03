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


// Implementation note: we specialize for 8 sizes of Unroll in order to improve
// compilation time compared to the much simpler/obvious implementation in this
// comment:
//    template <std::size_t N>
//    struct Unroll {
//        template <class T>
//        HURCHALLA_FORCE_INLINE static void call(T&& lambda)
//        {
//            Unroll<N - 1>::call(lambda);
//            lambda(N - 1);
//        }
//    };
//    template <>
//    struct Unroll<static_cast<std::size_t>(0)> {
//        template <class T>
//        HURCHALLA_FORCE_INLINE static void call(T&&) {}
//    };


template <std::size_t N>
struct Unroll {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        static_assert(N >= 8, "");
        Unroll<N - 8>::call(lambda);
        lambda(N - 8);
        lambda(N - 7);
        lambda(N - 6);
        lambda(N - 5);
        lambda(N - 4);
        lambda(N - 3);
        lambda(N - 2);
        lambda(N - 1);
    }
};

template <>
struct Unroll<static_cast<std::size_t>(7)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
        lambda(static_cast<std::size_t>(2));
        lambda(static_cast<std::size_t>(3));
        lambda(static_cast<std::size_t>(4));
        lambda(static_cast<std::size_t>(5));
        lambda(static_cast<std::size_t>(6));
    }
};
template <>
struct Unroll<static_cast<std::size_t>(6)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
        lambda(static_cast<std::size_t>(2));
        lambda(static_cast<std::size_t>(3));
        lambda(static_cast<std::size_t>(4));
        lambda(static_cast<std::size_t>(5));
    }
};
template <>
struct Unroll<static_cast<std::size_t>(5)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
        lambda(static_cast<std::size_t>(2));
        lambda(static_cast<std::size_t>(3));
        lambda(static_cast<std::size_t>(4));
    }
};
template <>
struct Unroll<static_cast<std::size_t>(4)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
        lambda(static_cast<std::size_t>(2));
        lambda(static_cast<std::size_t>(3));
    }
};
template <>
struct Unroll<static_cast<std::size_t>(3)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
        lambda(static_cast<std::size_t>(2));
    }
};
template <>
struct Unroll<static_cast<std::size_t>(2)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&& lambda)
    {
        lambda(static_cast<std::size_t>(0));
        lambda(static_cast<std::size_t>(1));
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
template <>
struct Unroll<static_cast<std::size_t>(0)> {
    template <class T>
    HURCHALLA_FORCE_INLINE static void call(T&&) {}
};


} // end namespace

#endif
