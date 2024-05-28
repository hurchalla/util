// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_COUNT_LEADING_ZEROS_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_COUNT_LEADING_ZEROS_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/programming_by_contract.h"
#include "hurchalla/util/compiler_macros.h"
#include <cstdint>
#include <type_traits>
#ifdef _MSC_VER
#  include <intrin.h>
#endif
/*
// using c++20's std::countl_zero() via this include would be nice, but it
// doesn't actually help us in this file because it doesn't support any
// extended types beyond unsigned long long (i.e. it can't handle __uint128_t),
// and compilers already have intrinsics for all other types we need.
#ifdef __has_include
#  if __has_include(<bit>)
#    include <bit>
#  endif
#endif
#if __cpp_lib_bitops
//  ... int count = std::countl_zero(x);
#endif
*/

namespace hurchalla { namespace detail {


struct default_count_leading_zeros {
    template <typename T>
    static HURCHALLA_FORCE_INLINE int call(T x)
    {
        HPBC_PRECONDITION2(x != 0);
        constexpr int digitsT = ut_numeric_limits<T>::digits;
/*
    // I commented this out so that I can test this function with all types.
    // ...impl_count_leading_zeros should have handled the types listed below.
        static_assert(!std::is_same<T, std::uint8_t>::value, "");
        static_assert(!std::is_same<T, std::uint16_t>::value, "");
        static_assert(!std::is_same<T, std::uint32_t>::value, "");
#if !defined(_MSC_VER) || (HURCHALLA_TARGET_BIT_WIDTH >= 64)
        static_assert(!std::is_same<T, std::uint64_t>::value, "");
#endif
*/
/*
        // simple version, suboptimal
        HPBC_PRECONDITION2(x != 0);
        constexpr int initial_shift = digitsT - 1;
        int shift = initial_shift;
        while (((x >> shift) & 1) == 0) {
            --shift;
            assert(shift >= 0);
        }
        return initial_shift - shift;
*/

#if !defined(_MSC_VER) || HURCHALLA_TARGET_BIT_WIDTH >= 64
        using U = unsigned long long;
#else
        using U = unsigned long;
#endif
        constexpr int digitsU = ut_numeric_limits<U>::digits;
        static_assert(digitsT >= digitsU, "");
        constexpr int initial_shift = digitsT - digitsU;
        int shift = initial_shift;
        U word = static_cast<U>(x >> shift);
        while (word == 0) {
            // static_assert to ensure shift can never become negative.  Given
            // that we know by precondition that x != 0, the smallest shift can
            // become is 0, because at that point we would get a word != 0.
            static_assert(digitsT % digitsU == 0, "");
            shift = shift - digitsU;
            HPBC_ASSERT2(shift >= 0);
            word = static_cast<U>(x >> shift);
        }
#ifndef _MSC_VER
        return __builtin_clzll(word) + initial_shift - shift;
#else
        unsigned long index;
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        static_assert(sizeof(U) == sizeof(unsigned __int64), "");
        _BitScanReverse64(&index, word);
# else
        _BitScanReverse(&index, word);
# endif
        return static_cast<int>(digitsU) - 1 - static_cast<int>(index) + initial_shift - shift;
#endif
    }
};



struct impl_count_leading_zeros {

    // From the gcc docs on __builtin_clz:
    // "Returns the number of leading 0-bits in x, starting at the most
    // significant bit position. If x is 0, the result is undefined."
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
       (ut_numeric_limits<T>::digits <= ut_numeric_limits<unsigned int>::digits)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        using U = unsigned int;
        U x = static_cast<U>(a);
#ifndef _MSC_VER
        constexpr int digitsU = ut_numeric_limits<U>::digits;
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return __builtin_clz(x) - (digitsU - digitsT);
#else
        unsigned long index;
        _BitScanReverse(&index, x);
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return static_cast<int>(digitsT) - 1 - static_cast<int>(index);
#endif
    }

    // From the gcc docs on __builtin_clzl:
    // "Similar to __builtin_clz, except the argument type is unsigned long."
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
     (ut_numeric_limits<T>::digits > ut_numeric_limits<unsigned int>::digits) &&
     (ut_numeric_limits<T>::digits <= ut_numeric_limits<unsigned long>::digits)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        using U = unsigned long;
        U x = static_cast<U>(a);
#ifndef _MSC_VER
        constexpr int digitsU = ut_numeric_limits<U>::digits;
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return __builtin_clzl(x) - (digitsU - digitsT);
#else
        unsigned long index;
        _BitScanReverse(&index, x);
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return static_cast<int>(digitsT) - 1 - static_cast<int>(index);
#endif
    }

    // From the gcc docs on __builtin_clzll:
    // "Similar to __builtin_clz, except the argument type is unsigned long
    // long."
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
       (ut_numeric_limits<T>::digits > ut_numeric_limits<unsigned long>::digits) &&
       (ut_numeric_limits<T>::digits <= ut_numeric_limits<unsigned long long>::digits)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        using U = unsigned long long;
        U x = static_cast<U>(a);
#ifndef _MSC_VER
        constexpr int digitsU = ut_numeric_limits<U>::digits;
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return __builtin_clzll(x) - (digitsU - digitsT);
#else
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        static_assert(sizeof(U) == sizeof(unsigned __int64), "");
        unsigned long index;
        _BitScanReverse64(&index, x);
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        return static_cast<int>(digitsT) - 1 - static_cast<int>(index);
# else
        static_assert(ut_numeric_limits<U>::digits == 64, "");
        static_assert(ut_numeric_limits<unsigned long>::digits >= 32, "");
        unsigned long hi = static_cast<unsigned long>(x >> 32);
        unsigned long lo = static_cast<unsigned long>(x);

        if (hi != 0) {
            unsigned long index;
            _BitScanReverse(&index, hi);
            constexpr int digitsT = ut_numeric_limits<T>::digits;
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 32);
        } else {
            // given that hi == 0, the precondition of a != 0 means that lo != 0
            HPBC_ASSERT2(lo != 0);
            unsigned long index;
            _BitScanReverse(&index, lo);
            return static_cast<int>(digitsT) - 1 - static_cast<int>(index);
        }
# endif
#endif
    }


    // handle T that is larger than unsigned long long, with T digits <= 128bit
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
       (ut_numeric_limits<T>::digits > ut_numeric_limits<unsigned long long>::digits) &&
       (ut_numeric_limits<T>::digits <= 128)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        T x = a;
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        static_assert(digitsT == 128, "");

#ifndef _MSC_VER
        using ULL = unsigned long long;
        constexpr int digitsULL = ut_numeric_limits<ULL>::digits;
        static_assert(digitsULL == 64, "");
        ULL hi = static_cast<ULL>(x >> 64);
        ULL lo = static_cast<ULL>(x);
        if (hi != 0)
            return __builtin_clzll(hi);
        else {
            // given that hi == 0, the precondition of a != 0 means that lo != 0.
            HPBC_ASSERT2(lo != 0);
            return __builtin_clzll(lo) + 64;
        }
#else   // MSVC++
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        using U64 = unsigned __int64;
        // we can't rely on numeric_limits having a specialization for
        // unsigned __int64, so we use sizeof instead.
        static_assert(sizeof(U64) * CHAR_BIT == 64, "");
        U64 hi = static_cast<U64>(x >> 64);
        U64 lo = static_cast<U64>(x);
        if (hi != 0) {
            unsigned long index;
            _BitScanReverse64(&index, hi);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 64);
        }
        else {
            // given that hi == 0, the precondition of a != 0 means that lo != 0.
            HPBC_ASSERT2(lo != 0);
            unsigned long index;
            _BitScanReverse64(&index, lo);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 0);
        }
# else
        using UL = unsigned long;
        static_assert(ut_numeric_limits<UL>::digits == 32, "");
        UL word0 = static_cast<UL>(x >> 96);
        UL word1 = static_cast<UL>(x >> 64);
        UL word2 = static_cast<UL>(x >> 32);
        UL word3 = static_cast<UL>(x);
        UL index;
        if (word0 != 0) {
            _BitScanReverse(&index, word0);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 96);
        else if (word1 != 0) {
            _BitScanReverse(&index, word1);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 64);
        }
        else if (word2 != 0) {
            _BitScanReverse(&index, word2);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 32);
        } else {
            // given that  word0 == 0 && word1 == 0 && word2 == 0, the precondition
            // of a != 0 means that word3 != 0.
            HPBC_ASSERT2(word3 != 0);
            _BitScanReverse(&index, word3);
            return static_cast<int>(digitsT) - 1 - (static_cast<int>(index) + 0);
        }
# endif
#endif
    }


    // handle T that is larger than unsigned long long, with T digits > 128bit
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
       (ut_numeric_limits<T>::digits > ut_numeric_limits<unsigned long long>::digits) &&
       (ut_numeric_limits<T>::digits > 128)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        return default_count_leading_zeros::call(a);
    }
};


}} // end namespace

#endif
