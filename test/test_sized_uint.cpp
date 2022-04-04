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

    static_assert(hc::sized_uint<8>::is_valid, "");
    static_assert(hc::sized_uint<16>::is_valid, "");
    static_assert(hc::sized_uint<32>::is_valid, "");
    static_assert(hc::sized_uint<64>::is_valid, "");
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    static_assert(hc::sized_uint<128>::is_valid, "");
#endif
    EXPECT_TRUE(hc::sized_uint<8>::is_valid);
    EXPECT_TRUE(hc::sized_uint<16>::is_valid);
    EXPECT_TRUE(hc::sized_uint<32>::is_valid);
    EXPECT_TRUE(hc::sized_uint<64>::is_valid);
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    EXPECT_TRUE(hc::sized_uint<128>::is_valid);
#endif
    
    EXPECT_TRUE(false); // temporary, to check whether github actions sees the failure

    static_assert(!hc::sized_uint<4>::is_valid, "");
    static_assert(!hc::sized_uint<9>::is_valid, "");
    EXPECT_FALSE(hc::sized_uint<4>::is_valid);
    EXPECT_FALSE(hc::sized_uint<9>::is_valid);
}

} // end namespace
