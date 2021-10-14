// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


#include "hurchalla/util/signed_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace {


template <typename T>
void test_signed_multiply_to_hilo_product()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(hc::ut_numeric_limits<T>::is_signed, "");

    // As with signed_multiply_to_hilo_product, this file assumes two's
    // complement and arithmetic right shift for signed integers.
    static_assert(-1 == ~0, "");         // two's complement
    static_assert((-1 >> 1) == -1, "");  // arithmetic right shift

    using U = typename hc::extensible_make_unsigned<T>::type;

    T tmax = hc::ut_numeric_limits<T>::max();
    T tmin = hc::ut_numeric_limits<T>::min();
    U umax = hc::ut_numeric_limits<U>::max();
    int udigits = hc::ut_numeric_limits<U>::digits;
    T hi, a, b;
    U lo;

    a = 5; b = 6;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 30);
    a = 9; b = 7;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 63);
    a = 4; b = 4;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 16);

    a = 0; b = 0;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 0; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 1; b = 0;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 0; b = 2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = tmax; b = 0;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);

    a = 1; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = 1; b = 2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 2);
    a = 4; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 4);
    a = tmax; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(tmax));
    a = 1; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(tmax));

    a = 2; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(umax - 1));
    a = tmax; b = 2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(umax - 1));

    a = tmax; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 1 && lo == 1);

    a = tmax; b = static_cast<T>(tmax-1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 2
                && lo == (static_cast<U>(1)<<(udigits-1)) + 2);

    a = static_cast<T>(tmax-2); b = static_cast<T>(tmax-1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 3
                && lo == (static_cast<U>(1)<<(udigits-1)) + 6);

    a = static_cast<T>(tmax-4); b = static_cast<T>(tmax-4);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 5 && lo == 25);


    // negative number and zero
    a = 0; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = -1; b = 0;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 0; b = -2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = tmin; b = 0;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);


    // both negative numbers
    a = -5; b = -6;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 30);
    a = -9; b = -7;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 63);
    a = -4; b = -4;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 16);

    a = -1; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = -1; b = -2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 2);
    a = -4; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 4);

    a = tmin; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(1)<<(udigits-1));
    a = -1; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == static_cast<U>(1)<<(udigits-1));

    a = -2; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 1 && lo == 0);
    a = tmin; b = -2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == 1 && lo == 0);

    a = tmin; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) && lo == 0);

    a = tmin; b = static_cast<T>(tmin+1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 1
                && lo == (static_cast<U>(1)<<(udigits-1)));

    a = static_cast<T>(tmin+2); b = static_cast<T>(tmin+1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 2
                && lo == (static_cast<U>(1)<<(udigits-1)) + 2);

    a = static_cast<T>(tmin+4); b = static_cast<T>(tmin+4);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == (static_cast<T>(1)<<(udigits-2)) - 4 && lo == 16);


    // one negative and one positive number
    a = -5; b = 6;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-30));
    a = 9; b = -7;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-63));
    a = -4; b = 4;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-16));

    a = -1; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-1));
    a = 1; b = -2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-2));
    a = 4; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-4));

    a = tmin; b = 1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(tmin));
    a = 1; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(tmin));

    a = tmax; b = -1;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-tmax));
    a = -1; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == static_cast<U>(-tmax));

    a = 2; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == 0);
    a = tmin; b = 2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == 0);

    a = -2; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == 2);
    a = tmax; b = -2;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -1 && lo == 2);

    a = tmin; b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2))
                && lo == (static_cast<U>(1)<<(udigits-1)));
    a = tmax; b = tmin;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2))
                && lo == (static_cast<U>(1)<<(udigits-1)));

    a = tmax; b = static_cast<T>(tmin+1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2))
                && lo == static_cast<U>(-1));
    a = static_cast<T>(tmin+1); b = tmax;
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2))
                && lo == static_cast<U>(-1));

    a = static_cast<T>(tmin+2); b = static_cast<T>(tmax-1);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2)) + 1
                && lo == static_cast<U>(-4));
    a = static_cast<T>(tmax-1); b = static_cast<T>(tmin+2);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2)) + 1
                && lo == static_cast<U>(-4));

    a = static_cast<T>(tmin+4); b = static_cast<T>(tmax-4);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2)) + 4
                && lo == (static_cast<U>(1)<<(udigits-1)) - 20);
    a = static_cast<T>(tmax-4); b = static_cast<T>(tmin+4);
    hi = hc::signed_multiply_to_hilo_product(lo, a, b);
    EXPECT_TRUE(hi == -(static_cast<T>(1)<<(udigits-2)) + 4
                && lo == (static_cast<U>(1)<<(udigits-1)) - 20);
}


void test_signed_multiply_exhaustive_int8()
{
    static_assert(-1 == ~0, "");         // the tests assume two's complement
    static_assert((-1 >> 1) == -1, "");  // and arithmetic right shift

    using std::int8_t;
    namespace hc = ::hurchalla;
    int8_t tmax = hc::ut_numeric_limits<int8_t>::max();
    int8_t tmin = hc::ut_numeric_limits<int8_t>::min();

    for (int i=tmin; i<=tmax; i++) {
        for (int j=tmin; j<=tmax; j++) {
            int8_t i8 = static_cast<int8_t>(i);
            int8_t j8 = static_cast<int8_t>(j);
            std::uint8_t lo;
            int8_t hi = hc::signed_multiply_to_hilo_product(lo, i8, j8);
            int ref_hi = (i*j) >> 8;
            unsigned int ref_lo = static_cast<unsigned int>((i*j) & 0xFF);
            EXPECT_TRUE(hi == ref_hi && lo == ref_lo);
        }
    }
}


TEST(HurchallaUtil, signed_multiply_to_hilo_product) {
    test_signed_multiply_to_hilo_product<std::int8_t>();
    test_signed_multiply_to_hilo_product<std::int16_t>();
    test_signed_multiply_to_hilo_product<std::int32_t>();
    test_signed_multiply_to_hilo_product<std::int64_t>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_signed_multiply_to_hilo_product<__int128_t>();
#endif

    test_signed_multiply_exhaustive_int8();
}


} // end unnamed namespace
