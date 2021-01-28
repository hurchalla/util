// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/Unroll.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <array>


namespace {

TEST(HurchallaUtil, Unroll) {
    namespace hc = hurchalla;
    {
        constexpr int COUNT = 5;
        int sum1 = 0;
        for (int i=0; i<COUNT; ++i)
            sum1 += i;

        int sum2 = 0;
        // it's nice to use HURCHALLA_INLINE_LAMBDA to ensure that there won't
        // be any function calls to a lambda.  This generally should result in
        // the exact same code as if we had unrolled a loop manually
        hc::Unroll<COUNT>::call([&](std::size_t i) HURCHALLA_INLINE_LAMBDA {
            sum2 += static_cast<int>(i);
        });
        EXPECT_TRUE(sum1 == sum2);
    }
    {
        std::array<int, 3> arr = {{ 2, 3, 4 }};
        int product1 = 1;
        for (auto x : arr)
            product1 *= x;

        int product2 = 1;
        hc::Unroll<arr.size()>::call([&](std::size_t i) HURCHALLA_INLINE_LAMBDA{
            product2 *= arr[i];
        });
        EXPECT_TRUE(product1 == product2);
    }
    {
        constexpr unsigned int COUNT = 10;
        std::vector<unsigned int> vec1;
        for (unsigned int i = 0; i < COUNT; ++i)
            vec1.push_back(i);

        std::vector<unsigned int> vec2;
        // although I usually would and do use HURCHALLA_INLINE_LAMBDA, we don't
        // have to.  The reality is that at any optimization level O2 or higher,
        // it's exceedingly likely that the compiler will inline a small lamda
        // without specifically being told to do so.
        hc::Unroll<COUNT>::call([&](std::size_t i) {
            vec2.push_back(static_cast<unsigned int>(i));
        });
        EXPECT_TRUE(vec1 == vec2);
    }
}

} // end namespace
