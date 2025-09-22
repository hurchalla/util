// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/experimental/x64_impl_cselect_on_bit.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>
#include <array>

namespace {


template <int BITNUM>
void test_cselect_on_bit()
{
    using namespace ::hurchalla::detail;

    uint64_t all_ones = 0xFFFFFFFFFFFFFFFFull;
    uint64_t all_zeros = 0;

    uint64_t pattern1 = 0xAAAAAAAAAAAAAAAAull;
    uint64_t pattern2 = 0x5555555555555555ull;

    std::array<uint64_t, 1> arg1_size1 = { pattern1 };
    std::array<uint64_t, 2> arg1_size2 = { pattern1, pattern1 };
    std::array<uint64_t, 3> arg1_size3 = { pattern1, pattern1, pattern1 };
    std::array<uint64_t, 4> arg1_size4 = { pattern1, pattern1, pattern1, pattern1 };

    std::array<uint64_t, 1> arg2_size1 = { pattern2 };
    std::array<uint64_t, 2> arg2_size2 = { pattern2, pattern2 };
    std::array<uint64_t, 3> arg2_size3 = { pattern2, pattern2, pattern2 };
    std::array<uint64_t, 4> arg2_size4 = { pattern2, pattern2, pattern2, pattern2 };


    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size1, arg2_size1) == arg2_size1);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size1, arg2_size1) == arg1_size1);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size2, arg2_size2) == arg2_size2);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size2, arg2_size2) == arg1_size2);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size3, arg2_size3) == arg2_size3);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size3, arg2_size3) == arg1_size3);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size4, arg2_size4) == arg2_size4);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size4, arg2_size4) == arg1_size4);


    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size1, arg2_size1) == arg1_size1);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size1, arg2_size1) == arg2_size1);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size2, arg2_size2) == arg1_size2);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size2, arg2_size2) == arg2_size2);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size3, arg2_size3) == arg1_size3);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size3, arg2_size3) == arg2_size3);

    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size4, arg2_size4) == arg1_size4);
    EXPECT_TRUE(impl_cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size4, arg2_size4) == arg2_size4);
}



TEST(HurchallaUtil, experimental_cselect_on_bit) {
    test_cselect_on_bit<0>();
    test_cselect_on_bit<1>();
    test_cselect_on_bit<2>();
    test_cselect_on_bit<3>();
    test_cselect_on_bit<4>();
    test_cselect_on_bit<5>();
    test_cselect_on_bit<6>();
    test_cselect_on_bit<7>();
    test_cselect_on_bit<8>();
    test_cselect_on_bit<9>();

    test_cselect_on_bit<10>();
    test_cselect_on_bit<11>();
    test_cselect_on_bit<12>();
    test_cselect_on_bit<13>();
    test_cselect_on_bit<14>();
    test_cselect_on_bit<15>();
    test_cselect_on_bit<16>();
    test_cselect_on_bit<17>();
    test_cselect_on_bit<18>();
    test_cselect_on_bit<19>();

    test_cselect_on_bit<20>();
    test_cselect_on_bit<21>();
    test_cselect_on_bit<22>();
    test_cselect_on_bit<23>();
    test_cselect_on_bit<24>();
    test_cselect_on_bit<25>();
    test_cselect_on_bit<26>();
    test_cselect_on_bit<27>();
    test_cselect_on_bit<28>();
    test_cselect_on_bit<29>();

    test_cselect_on_bit<30>();
    test_cselect_on_bit<31>();
    test_cselect_on_bit<32>();
    test_cselect_on_bit<33>();
    test_cselect_on_bit<34>();
    test_cselect_on_bit<35>();
    test_cselect_on_bit<36>();
    test_cselect_on_bit<37>();
    test_cselect_on_bit<38>();
    test_cselect_on_bit<39>();

    test_cselect_on_bit<40>();
    test_cselect_on_bit<41>();
    test_cselect_on_bit<42>();
    test_cselect_on_bit<43>();
    test_cselect_on_bit<44>();
    test_cselect_on_bit<45>();
    test_cselect_on_bit<46>();
    test_cselect_on_bit<47>();
    test_cselect_on_bit<48>();
    test_cselect_on_bit<49>();

    test_cselect_on_bit<50>();
    test_cselect_on_bit<51>();
    test_cselect_on_bit<52>();
    test_cselect_on_bit<53>();
    test_cselect_on_bit<54>();
    test_cselect_on_bit<55>();
    test_cselect_on_bit<56>();
    test_cselect_on_bit<57>();
    test_cselect_on_bit<58>();
    test_cselect_on_bit<59>();

    test_cselect_on_bit<60>();
    test_cselect_on_bit<61>();
    test_cselect_on_bit<62>();
    test_cselect_on_bit<63>();
}


} // end namespace
