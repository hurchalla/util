// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


#include "hurchalla/util/unsigned_multiply_to_hilo_product.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace {


template <typename T, template <typename> class F>
void test_unsigned_multiply_to_hilo_product()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(!(hc::ut_numeric_limits<T>::is_signed), "");
    T tmax = hc::ut_numeric_limits<T>::max();
    T hi, lo, a, b;

    a = 5; b = 6;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 30);
    a = 9; b = 7;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 63);
    a = 4; b = 4;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 16);

    a = 0; b = 0;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 0; b = 1;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 1; b = 0;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = 0; b = 2;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);
    a = tmax; b = 0;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 0);

    a = 1; b = 1;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 1);
    a = 1; b = 2;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 2);
    a = 4; b = 1;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == 4);
    a = tmax; b = 1;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == tmax);
    a = 1; b = tmax;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 0 && lo == tmax);

    a = 2; b = tmax;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 1 && lo == static_cast<T>(tmax - 1));
    a = tmax; b = 2;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == 1 && lo == static_cast<T>(tmax - 1));

    a = tmax; b = tmax;
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-1) && lo == 1);

    a = tmax; b = static_cast<T>(tmax-1);
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-2) && lo == 2);

    a = static_cast<T>(tmax-2); b = static_cast<T>(tmax-1);
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-4) && lo == 6);

    a = static_cast<T>(tmax-4); b = static_cast<T>(tmax-4);
    hi = F<T>::call(lo, a, b);
    EXPECT_TRUE(hi == static_cast<T>(tmax-9) && lo == 25);
}



template <typename T>
struct umthp {
    static T call(T& lowProduct, T u, T v)
    {
        return hurchalla::unsigned_multiply_to_hilo_product(lowProduct, u, v);
    }
};
template <typename T>
struct umthp_slow {
    static T call(T& lowProduct, T u, T v)
    {
        return hurchalla::detail::slow_unsigned_multiply_to_hilo_product::call(lowProduct, u, v);
    }
};


TEST(HurchallaUtil, unsigned_multiply_to_hilo_product) {
    test_unsigned_multiply_to_hilo_product<std::uint8_t, umthp>();
    test_unsigned_multiply_to_hilo_product<std::uint16_t, umthp>();
    test_unsigned_multiply_to_hilo_product<std::uint32_t, umthp>();
    test_unsigned_multiply_to_hilo_product<std::uint64_t, umthp>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_unsigned_multiply_to_hilo_product<__uint128_t, umthp>();
#endif
}

TEST(HurchallaUtil, slow_unsigned_multiply_to_hilo_product) {
    test_unsigned_multiply_to_hilo_product<std::uint64_t, umthp_slow>();
}



} // end unnamed namespace
