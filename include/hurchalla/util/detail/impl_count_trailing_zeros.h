// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_COUNT_TRAILING_ZEROS_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_COUNT_TRAILING_ZEROS_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/programming_by_contract.h"
#include "hurchalla/util/compiler_macros.h"
#include <cstdint>
#include <type_traits>
#ifdef _MSC_VER
#  include <intrin.h>
#endif
/*
// using c++20's std::countr_zero() via this include would be nice, but it
// doesn't actually help us in this file because it doesn't support any
// extended types beyond unsigned long long (i.e. it can't handle __uint128_t),
// and compilers already have intrinsics for all other types we need.
#ifdef __has_include
#  if __has_include(<bit>)
#    include <bit>
#  endif
#endif
#if __cpp_lib_bitops
//  ... int count = std::countr_zero(x);
#endif
*/

namespace hurchalla { namespace detail {


struct default_count_trailing_zeros {
    template <typename T>
    static HURCHALLA_FORCE_INLINE int call(T x)
    {
        HPBC_PRECONDITION2(x != 0);
/*
    // I commented this out so that I can test this function with all types.
    // ...impl_count_trailing_zeros should have handled the types listed below.
        static_assert(!std::is_same<T, std::uint8_t>::value, "");
        static_assert(!std::is_same<T, std::uint16_t>::value, "");
        static_assert(!std::is_same<T, std::uint32_t>::value, "");
#if !defined(_MSC_VER) || (HURCHALLA_TARGET_BIT_WIDTH >= 64)
        static_assert(!std::is_same<T, std::uint64_t>::value, "");
#endif
*/
/*
        // super simple version, suboptimal
        int count = 0;
        while ((x & 1u) == 0u) {
            x = x >> 1;
            ++count;
        }
        return count;
*/
#if !defined(_MSC_VER) || HURCHALLA_TARGET_BIT_WIDTH >= 64
        using U = unsigned long long;
#else
        using U = unsigned long;
#endif
        constexpr int digitsT = ut_numeric_limits<T>::digits;
        constexpr int digitsU = ut_numeric_limits<U>::digits;
        static_assert(digitsT % digitsU == 0, "");

        int shift = 0;
        U word = static_cast<U>(x);

        HPBC_INVARIANT2(0 <= shift && shift < digitsT);
        while (word == 0) {
            shift = shift + digitsU;
            HPBC_INVARIANT2(0 <= shift && shift < digitsT);
            // proof of invariant:  assume prior to assignment above we had
            // shift < digitsT.  Then since digitsT % digitsU == 0, and the
            // initial value of shift == 0, after the assignment above we
            // would get either shift < digitsT or shift == digitsT.  But
            // shift == digitsT would mean that all bits of x had already been
            // checked to see if any were set, and since we are still in this
            // loop we know no bits were set, which is impossible since x != 0.
            // Therefore after assignment we would have shift < digitsT.
            // By induction the initial setting of shift < digitsT proves that
            // we always have shift < digitsT in this loop.  (0 <= shift is
            // trivial to prove.)
            word = static_cast<U>(x >> shift);
        }
#ifndef _MSC_VER
        return __builtin_ctzll(word) + shift;
#else
        unsigned long index;
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        static_assert(sizeof(U) == sizeof(unsigned __int64), "");
        _BitScanForward64(&index, word);
# else
        _BitScanForward(&index, word);
# endif
        return static_cast<int>(index) + shift;
#endif
    }
};



struct impl_count_trailing_zeros {

    // From the gcc docs on __builtin_ctz:
    // "Returns the number of trailing 0-bits in x, starting at the least
    // significant bit position.  If x is 0, the result is undefined."
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::is_integer) && 
       (!ut_numeric_limits<T>::is_signed) &&
       (ut_numeric_limits<T>::digits <= ut_numeric_limits<unsigned int>::digits)
       , int>::type
    call(T a)
    {
        HPBC_PRECONDITION2(a != 0);
        unsigned int x = static_cast<unsigned int>(a);
#ifdef _MSC_VER
        unsigned long index;
        _BitScanForward(&index, x);
        return static_cast<int>(index);
#else
        return __builtin_ctz(x);
#endif
    }

    // From the gcc docs on __builtin_ctzl:
    // "Similar to __builtin_ctz, except the argument type is unsigned long."
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
        unsigned long x = static_cast<unsigned long>(a);
#ifdef _MSC_VER
        unsigned long index;
        _BitScanForward(&index, x);
        return static_cast<int>(index);
#else
        return __builtin_ctzl(x);
#endif
    }

    // From the gcc docs on __builtin_ctzll:
    // "Similar to __builtin_ctz, except the argument type is unsigned long
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
        unsigned long long x = static_cast<unsigned long long>(a);
#ifndef _MSC_VER
        return __builtin_ctzll(x);
#else
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        static_assert(sizeof(unsigned long long)==sizeof(unsigned __int64), "");
        unsigned long index;
        _BitScanForward64(&index, x);
        return static_cast<int>(index);
# else
        static_assert(ut_numeric_limits<unsigned long>::digits == 32, "");
        static_assert(ut_numeric_limits<unsigned long long>::digits == 64, "");
        unsigned long hi = static_cast<unsigned long>(x >> 32);
        unsigned long lo = static_cast<unsigned long>(x);
        if (lo != 0) {
            unsigned long index;
            _BitScanForward(&index, lo);
            return static_cast<int>(index);
        } else {
            // given that lo == 0, the precondition of a != 0 means that hi != 0
            HPBC_ASSERT2(hi != 0);
            unsigned long index;
            _BitScanForward(&index, hi);
            return static_cast<int>(index + 32);
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
        if (lo != 0)
            return __builtin_ctzll(lo);
        else {
            // given that lo == 0, the precondition of a != 0 means that hi != 0.
            HPBC_ASSERT2(hi != 0);
            return __builtin_ctzll(hi) + 64;
        }
#else   // MSVC++
# if HURCHALLA_TARGET_BIT_WIDTH >= 64
        using U64 = unsigned __int64;
        // we can't rely on numeric_limits having a specialization for
        // unsigned __int64, so we use sizeof instead.
        static_assert(sizeof(U64) * CHAR_BIT == 64, "");
        U64 hi = static_cast<U64>(x >> 64);
        U64 lo = static_cast<U64>(x);
        if (lo != 0) {
            unsigned long index;
            _BitScanForward64(&index, lo);
            return static_cast<int>(index);
        }
        else {
            // given that lo == 0, the precondition of a != 0 means that hi != 0.
            HPBC_ASSERT2(hi != 0);
            unsigned long index;
            _BitScanForward64(&index, hi);
            return static_cast<int>(index + 64);
        }
# else
        using UL = unsigned long;
        static_assert(ut_numeric_limits<UL>::digits == 32, "");
        UL word3 = static_cast<UL>(x);
        UL word2 = static_cast<UL>(x >> 32);
        UL word1 = static_cast<UL>(x >> 64);
        UL word0 = static_cast<UL>(x >> 96);
        UL index;
        if (word3 != 0) {
            _BitScanForward(&index, word3);
            return static_cast<int>(index);
        }
        else if (word2 != 0) {
            _BitScanForward(&index, word2);
            return static_cast<int>(index + 32);
        }
        else if (word1 != 0) {
            _BitScanForward(&index, word1);
            return static_cast<int>(index + 64);
        } else {
            // given that  word3 == 0 && word2 == 0 && word1 == 0, the precondition
            // of a != 0 means that word0 != 0.
            HPBC_ASSERT2(word0 != 0);
            _BitScanForward(&index, word0);
            return static_cast<int>(index + 96);
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
        return default_count_trailing_zeros::call(a);
    }
};


}} // end namespace

#endif
