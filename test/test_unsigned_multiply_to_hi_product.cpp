// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


// Strictly for testing purposes, we make sure to enable the inline-asm function
// versions of unsigned_multiply_to_hi_product.  In their postconditions they
// will call the corresponding non-inline asm version to check their results, so
// we won't miss unit testing of the "normal" function versions too, so long as
// we also enable util's postcondition checking.
#undef HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO
#define HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO
#undef HURCHALLA_UTIL_ENABLE_ASSERTS
#define HURCHALLA_UTIL_ENABLE_ASSERTS
#undef HURCHALLA_UTIL_ASSERT_LEVEL
#define HURCHALLA_UTIL_ASSERT_LEVEL 3


#include "hurchalla/util/unsigned_multiply_to_hi_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace {


template <typename T, template <typename> class F>
void test_unsigned_multiply_to_hi_product()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(!(hc::ut_numeric_limits<T>::is_signed), "");
    static constexpr int digitsT = hc::ut_numeric_limits<T>::digits;

    T tmax = hc::ut_numeric_limits<T>::max();
    T hi, a, b;

    a = static_cast<T>(1) << (digitsT/2);
    b = a;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 1);

    b = static_cast<T>(2*b + 1);
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 2);

    a = 5; b = 6;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 9; b = 7;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 4; b = 4;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);

    a = 0; b = 0;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 0; b = 1;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 1; b = 0;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 0; b = 2;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = tmax; b = 0;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);

    a = 1; b = 1;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 1; b = 2;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 4; b = 1;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = tmax; b = 1;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);
    a = 1; b = tmax;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 0);

    a = 2; b = tmax;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 1);
    a = tmax; b = 2;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == 1);

    a = tmax; b = tmax;
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-1));

    a = tmax; b = static_cast<T>(tmax-1);
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-2));

    a = static_cast<T>(tmax-2); b = static_cast<T>(tmax-1);
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-4));

    a = static_cast<T>(tmax-4); b = static_cast<T>(tmax-4);
    hi = F<T>::call(a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-9));
}


void test_unsigned_multiply_exhaustive_uint8()
{
    using std::uint8_t;
    namespace hc = ::hurchalla;
    uint8_t tmax = hc::ut_numeric_limits<uint8_t>::max();

    for (int i=0; i<=tmax; i++) {
        for (int j=0; j<=tmax; j++) {
            uint8_t i8 = static_cast<uint8_t>(i);
            uint8_t j8 = static_cast<uint8_t>(j);
            uint8_t hi = hc::unsigned_multiply_to_hi_product(i8, j8);
            int ref_hi = (i*j) >> 8;
            EXPECT_TRUE(hi == ref_hi);
        }
    }
}



template <typename T>
struct umthp {
    static T call(T u, T v)
    {
        return hurchalla::unsigned_multiply_to_hi_product(u, v);
    }
};



TEST(HurchallaUtil, unsigned_multiply_to_hi_product) {
    test_unsigned_multiply_to_hi_product<std::uint8_t, umthp>();
    test_unsigned_multiply_to_hi_product<std::uint16_t, umthp>();
    test_unsigned_multiply_to_hi_product<std::uint32_t, umthp>();
    test_unsigned_multiply_to_hi_product<std::uint64_t, umthp>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_unsigned_multiply_to_hi_product<__uint128_t, umthp>();
#endif

    test_unsigned_multiply_exhaustive_uint8();
}



} // end unnamed namespace
