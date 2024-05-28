// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "uint128.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/count_leading_zeros.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/programming_by_contract.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>


namespace {


template <typename U>
void test_clz128(U input, int expected_output)
{
    namespace hc = ::hurchalla;
    int output = hc::count_leading_zeros(input);
    EXPECT_TRUE(expected_output == output);
    // whitebox test the function default_count_leading_zeros(), which would
    // likely not be called otherwise.
    output = hc::detail::default_count_leading_zeros::call(input);
    EXPECT_TRUE(expected_output == output);
}

template <typename U>
void test128()
{
    U x128 = static_cast<U>(1) << 68;
    test_clz128(x128, 59);
    x128 = x128 + (static_cast<U>(1) << 23);
    test_clz128(x128, 59);
    x128 = x128 + (static_cast<U>(1) << 72);
    test_clz128(x128, 55);
    x128 = static_cast<U>(1) << 127;
    test_clz128(x128, 0);
    x128 = static_cast<U>(1) << 63;
    test_clz128(x128, 64);
    x128 = x128 + (static_cast<U>(1) << 64);
    test_clz128(x128, 63);
    x128 = 1;
    test_clz128(x128, 127);
    x128 = 5;
    test_clz128(x128, 125);
}



TEST(HurchallaUtil, count_leading_zeros) {
    namespace hc = ::hurchalla;
    constexpr int digitsUS = hc::ut_numeric_limits<unsigned short>::digits;
    constexpr int digitsUI = hc::ut_numeric_limits<unsigned int>::digits;
    constexpr int digitsUL = hc::ut_numeric_limits<unsigned long>::digits;
    constexpr int digitsULL = hc::ut_numeric_limits<unsigned long long>::digits;

    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned char>(6)) == 5);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned short>(260)) == digitsUS - 9);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned short>(512)) == digitsUS - 10);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned short>(1)) == digitsUS - 1);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned short>(1 << (digitsUS - 1))) == 0);
    EXPECT_TRUE(hc::count_leading_zeros((static_cast<unsigned int>(1)<<(digitsUI - 2)) + 29) == 1);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned int>(1)) == digitsUI - 1);
    EXPECT_TRUE(hc::count_leading_zeros((static_cast<unsigned long>(1)<<(digitsUL - 4)) + 255) == 3);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned long>(1)) == digitsUL - 1);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned long>(255)) == digitsUL - 8);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned long long>(1)) == digitsULL - 1);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned long long>(1)<<(digitsULL - 1)) == 0);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<unsigned long long>(367)) == digitsULL - 9);

    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint8_t>(1)) == 7);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint8_t>(3)) == 6);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint8_t>(20)) == 3);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint8_t>(128)) == 0);

    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint16_t>(254)) == 8);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint16_t>(256)) == 7);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint16_t>(1)) == 15);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint16_t>(1 << 15)) == 0);

    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint32_t>(65536)) == 15);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint32_t>(65535)) == 16);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint32_t>(1)) == 31);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint32_t>(1) << 31) == 0);

    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint64_t>(1) << 32) == 31);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint64_t>(1) << 31) == 32);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint64_t>(1)) == 63);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint64_t>(2)) == 62);
    EXPECT_TRUE(hc::count_leading_zeros(static_cast<uint64_t>(1) << 63) == 0);
    EXPECT_TRUE(hc::count_leading_zeros((static_cast<uint64_t>(1) << 48) +
                                         (static_cast<uint64_t>(1) << 35)) == 15);

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
    test128<__uint128_t>();
#endif
    test128<uint128>();
}

} // end namespace
