// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/count_trailing_zeros.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>


namespace {

TEST(HurchallaUtil, count_trailing_zeros) {
    using namespace hurchalla;

    unsigned char xc = 6;
    EXPECT_TRUE(count_trailing_zeros(xc) == 1);
    unsigned short xs = 260;
    EXPECT_TRUE(count_trailing_zeros(xs) == 2);
    xs = 512;
    EXPECT_TRUE(count_trailing_zeros(xs) == 9);
    unsigned int xi = 5120352;
    EXPECT_TRUE(count_trailing_zeros(xi) == 5);
    unsigned long xl = 166068224;
    EXPECT_TRUE(count_trailing_zeros(xl) == 17);
    unsigned long long xll = 9223372036854775808ull;
    EXPECT_TRUE(count_trailing_zeros(xll) == 63);
#if (HURCHALLA_COMPILER_HAS_UINT128_T())
    __uint128_t x128 = static_cast<__uint128_t>(1) << 68;
    EXPECT_TRUE(count_trailing_zeros(x128) == 68);
#endif
}

} // end namespace
