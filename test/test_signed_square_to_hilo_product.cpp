// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


// Strictly for testing purposes, we make sure to enable the inline-asm function
// versions of signed_square_to_hilo_product.  In their postconditions they
// will call the corresponding non-inline asm version to check their results, so
// we won't miss unit testing of the "normal" function versions too, so long as
// we also enable util's postcondition checking.
#undef HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO
#define HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO
#undef HURCHALLA_UTIL_ENABLE_ASSERTS
#define HURCHALLA_UTIL_ENABLE_ASSERTS
#undef HURCHALLA_UTIL_ASSERT_LEVEL
#define HURCHALLA_UTIL_ASSERT_LEVEL 3


#include "hurchalla/util/signed_square_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "gtest/gtest.h"
#include <cstdint>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif

namespace {


template <typename T>
void test_signed_square_to_hilo_product()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(hc::ut_numeric_limits<T>::is_signed, "");

    // As with signed_square_to_hilo_product, this file assumes two's
    // complement and arithmetic right shift for signed integers.

    // Assert two's complement
    static_assert(static_cast<T>(-1) == ~(static_cast<T>(0)), "");
    // Assert arithmetic right shift
    static_assert((static_cast<T>(-1) >> 1) == static_cast<T>(-1), "");

    using U = typename hc::extensible_make_unsigned<T>::type;

    T tmax = hc::ut_numeric_limits<T>::max();
    T tmin = hc::ut_numeric_limits<T>::min();
    static constexpr int udigits = hc::ut_numeric_limits<U>::digits;
    T hi, a;
    U lo;

    a = 0;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 1;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = 2;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 4);
    a = 3;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 9);
    a = 4;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 16);
    a = 5;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 25);
    a = 9;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 81);


    a = tmax;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 1 && lo == 1);

    a = static_cast<T>(tmax-1);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 2 && lo == 4);

    a = static_cast<T>(tmax-2);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 3 && lo == 9);

    a = static_cast<T>(tmax-3);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 4 && lo == 16);

    a = static_cast<T>(tmax-4);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 5 && lo == 25);


    a = -1;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = -2;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 4);
    a = -3;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 9);
    a = -4;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 16);
    a = -5;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 25);
    a = -9;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 81);


    a = tmin;
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) && lo == 0);

    a = static_cast<T>(tmin+1);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 1 && lo == 1);

    a = static_cast<T>(tmin+2);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 2 && lo == 4);

    a = static_cast<T>(tmin+3);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 3 && lo == 9);

    a = static_cast<T>(tmin+4);
    hi = hc::signed_square_to_hilo_product(lo, a);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 4 && lo == 16);
}


void test_signed_square_exhaustive_int16()
{
    static_assert(-1 == ~0, "");         // the tests assume two's complement
    static_assert((-1 >> 1) == -1, "");  // and arithmetic right shift

    using std::int16_t;
    namespace hc = ::hurchalla;
    constexpr int16_t tmax = hc::ut_numeric_limits<int16_t>::max();
    constexpr int16_t tmin = hc::ut_numeric_limits<int16_t>::min();

    for (std::int64_t i=tmin; i<=tmax; ++i) {
        int16_t i16 = static_cast<int16_t>(i);
        std::uint16_t lo;
        int16_t hi = hc::signed_square_to_hilo_product(lo, i16);
        std::int64_t ref_hi = (i*i) >> 16;
        unsigned int ref_lo = static_cast<unsigned int>((i*i) & 0xFFFF);
        EXPECT_TRUE(hi == ref_hi && lo == ref_lo);
    }
}



TEST(HurchallaUtil, signed_square_to_hilo_product) {
    test_signed_square_to_hilo_product<std::int8_t>();
    test_signed_square_to_hilo_product<std::int16_t>();
    test_signed_square_to_hilo_product<std::int32_t>();
    test_signed_square_to_hilo_product<std::int64_t>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_signed_square_to_hilo_product<__int128_t>();
#endif

    test_signed_square_exhaustive_int16();
}


} // end unnamed namespace
