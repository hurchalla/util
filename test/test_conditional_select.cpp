// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/conditional_select.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstdint>

namespace {


template <typename T>
void test_conditional_select()
{
    namespace hc = ::hurchalla;
    T a = static_cast<T>(1);
    T b = static_cast<T>(-1);
    EXPECT_TRUE(hc::conditional_select(true, a, b) == a);
    EXPECT_TRUE(hc::conditional_select(false, a, b) == b);
    
    T tmp = hc::conditional_select<T, hc::CSelectStandardTag>(true, a, b);
    EXPECT_TRUE(tmp == a);
    
    tmp = hc::conditional_select<T, hc::CSelectStandardTag>(false, a, b);
    EXPECT_TRUE(tmp == b);
    
    tmp = hc::conditional_select<T, hc::CSelectMaskedTag>(true, a, b);
    EXPECT_TRUE(tmp == a);
    
    tmp = hc::conditional_select<T, hc::CSelectMaskedTag>(false, a, b);
    EXPECT_TRUE(tmp == b);
}


TEST(HurchallaUtil, conditional_select) {
    test_conditional_select<std::int8_t>();
    test_conditional_select<std::uint8_t>();
    test_conditional_select<std::int16_t>();
    test_conditional_select<std::uint16_t>();
    test_conditional_select<std::int32_t>();
    test_conditional_select<std::uint32_t>();
    test_conditional_select<std::int64_t>();
    test_conditional_select<std::uint64_t>();
#if (HURCHALLA_COMPILER_HAS_UINT128_T())
    test_conditional_select<__int128_t>();
    test_conditional_select<__uint128_t>();
#endif
}


} // end namespace
