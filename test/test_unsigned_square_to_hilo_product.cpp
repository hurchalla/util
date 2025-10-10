// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


// Strictly for testing purposes, we make sure to enable the inline-asm function
// versions of unsigned_square_to_hilo_product.  In their postconditions they
// will call the corresponding non-inline asm version to check their results, so
// we won't miss unit testing of the "normal" function versions too, so long as
// we also enable util's postcondition checking.
#undef HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO
#define HURCHALLA_ALLOW_INLINE_ASM_SQUARE_TO_HILO
#undef HURCHALLA_UTIL_ENABLE_ASSERTS
#define HURCHALLA_UTIL_ENABLE_ASSERTS
#undef HURCHALLA_UTIL_ASSERT_LEVEL
#define HURCHALLA_UTIL_ASSERT_LEVEL 3


#include "hurchalla/util/unsigned_square_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace {


template <typename T, template <typename> class F>
void test_unsigned_square_to_hilo_product()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(!(hc::ut_numeric_limits<T>::is_signed), "");
    T tmax = hc::ut_numeric_limits<T>::max();
    T hi, lo, a;

    a = 0;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 1;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = 2;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 4);
    a = 3;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 9);
    a = 4;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 16);
    a = 5;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 25);
    a = 9;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == 0 && lo == 81);

    a = tmax;
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == static_cast<T>(tmax-1) && lo == 1);

    a = static_cast<T>(tmax-1);
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == static_cast<T>(tmax-3) && lo == 4);

    a = static_cast<T>(tmax-2);
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == static_cast<T>(tmax-5) && lo == 9);

    a = static_cast<T>(tmax-3);
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == static_cast<T>(tmax-7) && lo == 16);

    a = static_cast<T>(tmax-4);
    hi = F<T>::call(lo, a);
    EXPECT_TRUE(hi == static_cast<T>(tmax-9) && lo == 25);
}


void test_unsigned_square_exhaustive_uint16()
{
    using std::uint16_t;
    namespace hc = ::hurchalla;
    uint16_t tmax = hc::ut_numeric_limits<uint16_t>::max();

    for (std::uint64_t i=0; i<=tmax; ++i) {
        uint16_t i16 = static_cast<uint16_t>(i);
        uint16_t lo;
        uint16_t hi = hc::unsigned_square_to_hilo_product(lo, i16);
        uint64_t ref_hi = (i*i) >> 16;
        uint64_t ref_lo = static_cast<uint64_t>((i*i) & 0xFFFF);
        EXPECT_TRUE(hi == ref_hi && lo == ref_lo);
    }
}


template <typename T>
struct usthp {
    static T call(T& lowProduct, T u)
    {
        return hurchalla::unsigned_square_to_hilo_product(lowProduct, u);
    }
};
/*
template <typename T>
struct usthp_slow {
    static T call(T& lowProduct, T u)
    {
        return hurchalla::detail::slow_unsigned_square_to_hilo_product::call(lowProduct, u);
    }
};
*/


TEST(HurchallaUtil, unsigned_square_to_hilo_product) {
    test_unsigned_square_to_hilo_product<std::uint8_t, usthp>();
    test_unsigned_square_to_hilo_product<std::uint16_t, usthp>();
    test_unsigned_square_to_hilo_product<std::uint32_t, usthp>();
    test_unsigned_square_to_hilo_product<std::uint64_t, usthp>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_unsigned_square_to_hilo_product<__uint128_t, usthp>();
#endif

    test_unsigned_square_exhaustive_uint16();
}


} // end unnamed namespace
