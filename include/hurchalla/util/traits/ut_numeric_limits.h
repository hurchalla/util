// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_NUMERIC_LIMITS_H_INCLUDED
#define HURCHALLA_UTIL_NUMERIC_LIMITS_H_INCLUDED


#include "hurchalla/util/compiler_macros.h"
#include <limits>
#include <climits>

namespace hurchalla {


// This traits struct replicates and extends std::numeric_limits.
// Example of use:
// uint64_t max64 = hurchalla::ut_numeric_limits<uint64_t>::max();

// Rationale for ut_numeric_limits:
// The specializations on __int128_t and __uint128_t are the reasons to use
// ut_numeric_limits instead of always relying upon std::numeric_limits.
// Std::numeric_limits is too problematic on its own when it comes to the 128
// bit int compiler extensions.  More specifically, gcc/clang/icc provide
// __int128_t and __uint128_t, but these compilers do not always specialize
// std::numeric_limits for those types in all situations where the types are
// valid, available, and usable.  For example, clang does not provide
// specializations if it is passed the flag -std=c++11 (or any other version
// xx in -std=c++xx), despite the fact that __int128_t and __uint128_t are
// still available.  Without being passed the flag -std=c++xx, clang usually
// does provide the std::numeric_limits specializations.  See
// https://quuxplusone.github.io/blog/2019/02/28/is-int128-integral/
//
// By providing our own traits struct ut_numeric_limits, we can ensure that
// ut_numeric_limits is always specialized for __int128_t and __uint128_t when
// those types are valid and usable.



// Primary template.  Implementation note: it has a dummy default parameter U so
// that specializations can be partial specializations that are templated on U
// (rather than being full explicit specializations).  This allows them to
// out-of-line define their static member variables within this header without
// any danger of ODR violations.  Unfortunately out-of-line definitions are
// necessary up until C++17 (17 allows the inline keyword to be used on member
// variables, which is much cleaner).  For more information, see
// https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
// and also
// https://stackoverflow.com/questions/45210631/odr-violation-with-template-specializations
// https://stackoverflow.com/questions/34552380/why-cs-vector-templated-class-doesnt-break-one-definition-rule
// https://en.cppreference.com/w/cpp/language/static
// https://en.cppreference.com/w/cpp/language/storage_duration
// https://en.cppreference.com/w/cpp/language/definition
template <typename T, typename U=void>
struct ut_numeric_limits : std::numeric_limits<T> {
    // If you are getting an error on this template, then you should provide a
    // specialization of ut_numeric_limits for your type, or alternatively you
    // can specialize std::numeric_limits for your type- in which case I would
    // recommend reading
    // https://stackoverflow.com/questions/16122912/is-it-ok-to-specialize-stdnumeric-limitst-for-user-defined-number-like-class/38670996
};

// We need these 3 partial specializations so that a cv-qualified type T can
// still match (through the inherited parent) the specializations below.
// ---
template <typename T, typename U>
struct ut_numeric_limits<const T, U> : public ut_numeric_limits<T> {};

template <typename T, typename U>
struct ut_numeric_limits<volatile T, U> : public ut_numeric_limits<T> {};

template <typename T, typename U>
struct ut_numeric_limits<const volatile T, U> : public ut_numeric_limits<T> {};



#if (HURCHALLA_COMPILER_HAS_UINT128_T())

// Specializations for __int128_t and __uint128_t.
template<typename U>
struct ut_numeric_limits<__int128_t, U> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style= std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 127;
    static constexpr int digits10 = 38;  // digits*std::log10(2)  (rounded down)
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    // the value used below for 'traps' is a bit speculative, but I'd expect ok
    static constexpr bool traps = std::numeric_limits<long long>::traps;
    static constexpr bool tinyness_before = false;
    // the logic used below for 'min' is a bit speculative, but I'd expect ok
    static constexpr __int128_t min() noexcept  {
        // we assume __int128_t uses the same method of representation as long.
        // Almost certainly the representation will be two's complement, but if
        // not, we assume below it is at least the same as for long.  (Note:
        // for C++20, two's complement is the only representation allowed by the
        // standard for signed integral types; see
        // https://en.cppreference.com/w/cpp/language/types )
        // See the comments below in max() for further relevant discussion.
        return (std::numeric_limits<long>::min() +
                        std::numeric_limits<long>::max()) - max();
    }
    static constexpr __int128_t lowest() noexcept { return min(); }
    static constexpr __int128_t max() noexcept
    {
        // Return (2^127)-1 without overflow
        return ((static_cast<__int128_t>(1) << 126) - 1) +
                                            (static_cast<__int128_t>(1) << 126);
        // The return value relies on the strong evidence that  (2^127)-1  must
        // be the max value of __int128_t.
        // For two's complement, ones' complement, signed magnitude, or offset
        // binary/excess-k, this would be the correct value.  It would be wrong
        // for base -2 (negative two) representation, but it seems exceedingly
        // unlikely that a compiler would use that representation or anything
        // different from two's complement/ones' complement/signed magnitude.
        // When I look at the generated asm from the latest versions (as of
        // 6/7/20) of gcc clang and icc, adding or subtracting two __int128_t
        // values, or casting a single __int128_t to int64_t, all appear to rely
        // on two's complement.
        //
        // Nevertheless we can't be absolutely certain this value is correct for
        // all compilers and compiler versions.  Unfortunately I don't see any
        // good reliable way to get the max __int128_t value from the compiler,
        // considering that we already know clang/gcc won't specialize
        // std::numeric_limits for __int128_t  under a number of circumstances.
        //
        // Note: Perhaps one way to get the correct value when the compiler does
        // not provide it, would be to compile and run a small program with gcc
        // extensions enabled (which should get the compiler to provide a
        // specialization std::numeric_limits<__int128_t>) and print the max
        // value in a dedicated header file.  Then during a build, that utility
        // program would build and run first, and then in a subsequent build,
        // this ut_numeric_limits header file would #include the header that the
        // utility program wrote.  This adds an awkward extra step to building,
        // and extra complication overall.  I believe the very strong guess used
        // above is preferable to the added complexity.
    }
    static constexpr __int128_t epsilon() noexcept { return 0; }
    static constexpr __int128_t round_error() noexcept { return 0; }
    static constexpr __int128_t infinity() noexcept { return 0; }
    static constexpr __int128_t quiet_NaN() noexcept { return 0; }
    static constexpr __int128_t signaling_NaN() noexcept { return 0; }
    static constexpr __int128_t denorm_min() noexcept { return 0; }
};
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_specialized;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_signed;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_integer;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_exact;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::has_infinity;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::has_quiet_NaN;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::has_signaling_NaN;
template <typename U>
constexpr std::float_denorm_style ut_numeric_limits<__int128_t, U>::has_denorm;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::has_denorm_loss;
template <typename U>
constexpr std::float_round_style ut_numeric_limits<__int128_t, U>::round_style;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_iec559;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_bounded;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::is_modulo;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::digits;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::digits10;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::max_digits10;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::radix;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::min_exponent;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::min_exponent10;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::max_exponent;
template <typename U>
constexpr int ut_numeric_limits<__int128_t, U>::max_exponent10;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::traps;
template <typename U>
constexpr bool ut_numeric_limits<__int128_t, U>::tinyness_before;

template<typename U>
struct ut_numeric_limits<__uint128_t, U> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style= std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = 128;
    static constexpr int digits10 = 38;  // digits*std::log10(2)  (rounded down)
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    // the value used below for 'traps' is a bit speculative, but I'd expect ok
    static constexpr bool traps= std::numeric_limits<unsigned long long>::traps;
    static constexpr bool tinyness_before = false;
    static constexpr __uint128_t min() noexcept  { return 0; }
    static constexpr __uint128_t lowest() noexcept { return 0; }
    static constexpr __uint128_t max() noexcept
    {
        // rely upon modulo (2^128) wrap-around on subtraction below
        return static_cast<__uint128_t>(0) - static_cast<__uint128_t>(1);
    }
    static constexpr __uint128_t epsilon() noexcept { return 0; }
    static constexpr __uint128_t round_error() noexcept { return 0; }
    static constexpr __uint128_t infinity() noexcept { return 0; }
    static constexpr __uint128_t quiet_NaN() noexcept { return 0; }
    static constexpr __uint128_t signaling_NaN() noexcept { return 0; }
    static constexpr __uint128_t denorm_min() noexcept { return 0; }
};
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_specialized;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_signed;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_integer;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_exact;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::has_infinity;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::has_quiet_NaN;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::has_signaling_NaN;
template <typename U>
constexpr std::float_denorm_style ut_numeric_limits<__uint128_t, U>::has_denorm;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::has_denorm_loss;
template <typename U>
constexpr std::float_round_style ut_numeric_limits<__uint128_t, U>::round_style;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_iec559;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_bounded;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::is_modulo;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::digits;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::digits10;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::max_digits10;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::radix;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::min_exponent;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::min_exponent10;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::max_exponent;
template <typename U>
constexpr int ut_numeric_limits<__uint128_t, U>::max_exponent10;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::traps;
template <typename U>
constexpr bool ut_numeric_limits<__uint128_t, U>::tinyness_before;

#endif


}  // end namespace

#endif
