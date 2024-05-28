// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "uint128.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/count_trailing_zeros.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/programming_by_contract.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>


namespace {


template <typename U>
void test_ctz128(U input, int expected_output)
{
    namespace hc = ::hurchalla;
    int output = hc::count_trailing_zeros(input);
    EXPECT_TRUE(expected_output == output);
    // whitebox test the function default_count_trailing_zeros(), which would
    // likely not be called otherwise.
    output = hc::detail::default_count_trailing_zeros::call(input);
    EXPECT_TRUE(expected_output == output);
}

template <typename U>
void test128()
{
    U x128 = static_cast<U>(1) << 68;
    test_ctz128(x128, 68);
    x128 = x128 + (static_cast<U>(1) << 23);
    test_ctz128(x128, 23);
    x128 = x128 + 3;
    test_ctz128(x128, 0);
    x128 = static_cast<U>(1) << 127;
    test_ctz128(x128, 127);
    x128 = static_cast<U>(1) << 64;
    test_ctz128(x128, 64);
    x128 = x128 + (static_cast<U>(1) << 63);
    test_ctz128(x128, 63);
}



TEST(HurchallaUtil, count_trailing_zeros) {

    namespace hc = ::hurchalla;
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned char>(6)) == 1);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned short>(260)) == 2);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned short>(512)) == 9);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned short>(103)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned int>(5120352)) == 5);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned long>(166068224ul)) == 17);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned long long>(9223372036854775808ull)) == 63);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<unsigned long long>(9223372036854775809ull)) == 0);

    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint8_t>(3)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint8_t>(20)) == 2);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint8_t>(128)) == 7);

    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint16_t>(257)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint16_t>(258)) == 1);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint16_t>(1)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint16_t>(24)) == 3);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint16_t>(1 << 15)) == 15);

    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint32_t>(65536)) == 16);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint32_t>(65537)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint32_t>(1)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint32_t>(96)) == 5);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint32_t>(1) << 31) == 31);

    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint64_t>(1) << 32) == 32);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint64_t>(1) << 31) == 31);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint64_t>(1)) == 0);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint64_t>(2)) == 1);
    EXPECT_TRUE(hc::count_trailing_zeros(static_cast<uint64_t>(1) << 63) == 63);
    EXPECT_TRUE(hc::count_trailing_zeros((static_cast<uint64_t>(1) << 63) +
                                         (static_cast<uint64_t>(1) << 35)) == 35);

#if (HURCHALLA_COMPILER_HAS_UINT128_T())
    test128<__uint128_t>();
#endif
    test128<uint128>();
}

} // end namespace
