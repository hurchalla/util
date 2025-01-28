// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>


namespace {

TEST(HurchallaUtil, sized_uint) {
    namespace hc = ::hurchalla;
    static_assert(std::is_same<std::uint8_t, hc::sized_uint<8>::type>::value, "");
    static_assert(std::is_same<std::uint16_t, hc::sized_uint<16>::type>::value, "");
    static_assert(std::is_same<std::uint32_t, hc::sized_uint<32>::type>::value, "");
    static_assert(std::is_same<std::uint64_t, hc::sized_uint<64>::type>::value, "");
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    static_assert(std::is_same<__uint128_t, hc::sized_uint<128>::type>::value, "");
#endif

    static_assert(hc::is_valid_sized_uint<8>::value, "");
    static_assert(hc::is_valid_sized_uint<16>::value, "");
    static_assert(hc::is_valid_sized_uint<32>::value, "");
    static_assert(hc::is_valid_sized_uint<64>::value, "");
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    static_assert(hc::is_valid_sized_uint<128>::value, "");
#endif
    EXPECT_TRUE(hc::is_valid_sized_uint<8>::value);
    EXPECT_TRUE(hc::is_valid_sized_uint<16>::value);
    EXPECT_TRUE(hc::is_valid_sized_uint<32>::value);
    EXPECT_TRUE(hc::is_valid_sized_uint<64>::value);
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    EXPECT_TRUE(hc::is_valid_sized_uint<128>::value);
#endif

    static_assert(!hc::is_valid_sized_uint<4>::value, "");
    static_assert(!hc::is_valid_sized_uint<9>::value, "");
    EXPECT_FALSE(hc::is_valid_sized_uint<4>::value);
    EXPECT_FALSE(hc::is_valid_sized_uint<9>::value);
}

} // end namespace
