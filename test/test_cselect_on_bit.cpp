// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

// enable cselect_on_bit asm versions (if available for the compiled platform),
// for testing purposes
#define HURCHALLA_ALLOW_INLINE_ASM_CSELECT_ON_BIT


#include "hurchalla/util/cselect_on_bit.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <type_traits>
#include <cstdint>
#include <array>
#include <random>

namespace {


template <typename U>
U generate_random_value(std::mt19937_64& gen,
                        std::uniform_int_distribution<uint64_t>& distrib64)
{
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127 4293)
#endif
   if (hurchalla::ut_numeric_limits<U>::digits > 64) {
      uint64_t u1 = distrib64(gen);
      uint64_t u2 = distrib64(gen);
      U val = static_cast<U>((static_cast<U>(u2) << 64u) | static_cast<U>(u1));
      return val;
   } else {
      return static_cast<U>(distrib64(gen));
   }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
}


template <int BITNUM, size_t ARRAY_SIZE>
std::array<uint64_t,ARRAY_SIZE>
ref_csel_on_bit_eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
{
    std::array<uint64_t,ARRAY_SIZE> result;
    HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
        result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? arg1[i] : arg2[i];
    }
    return result;
}

template <int BITNUM, size_t ARRAY_SIZE>
std::array<uint64_t,ARRAY_SIZE>
ref_csel_on_bit_ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
{
    std::array<uint64_t,ARRAY_SIZE> result;
    HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
        result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? arg1[i] : arg2[i];
    }
    return result;
}



template <int BITNUM>
void test_cselect_on_bit()
{
    using namespace ::hurchalla;

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


    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  pattern1, pattern2) == pattern2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, pattern1, pattern2) == pattern1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  pattern1, pattern2) == pattern1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, pattern1, pattern2) == pattern2);

    int64_t i64_1 = static_cast<int64_t>(pattern1);
    int64_t i64_2 = static_cast<int64_t>(pattern2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  i64_1, i64_2) == i64_2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, i64_1, i64_2) == i64_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  i64_1, i64_2) == i64_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, i64_1, i64_2) == i64_2);

    int64_t u32_1 = static_cast<uint32_t>(pattern1);
    int64_t u32_2 = static_cast<uint32_t>(pattern2);
    int64_t i32_1 = static_cast<int32_t>(pattern1);
    int64_t i32_2 = static_cast<int32_t>(pattern2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  i32_1, i32_2) == i32_2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, i32_1, i32_2) == i32_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  i32_1, i32_2) == i32_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, i32_1, i32_2) == i32_2);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  u32_1, u32_2) == u32_2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, u32_1, u32_2) == u32_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  u32_1, u32_2) == u32_1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, u32_1, u32_2) == u32_2);

#if HURCHALLA_COMPILER_HAS_UINT128_T()
    __uint128_t u1_128 = (static_cast<__uint128_t>(pattern1) << 64) | pattern1;
    __uint128_t u2_128 = (static_cast<__uint128_t>(pattern2) << 64) | pattern2;
    __int128_t i1_128 = static_cast<__int128_t>(u1_128);
    __int128_t i2_128 = static_cast<__int128_t>(u2_128);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  u1_128, u2_128) == u2_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, u1_128, u2_128) == u1_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  u1_128, u2_128) == u1_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, u1_128, u2_128) == u2_128);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones,  i1_128, i2_128) == i2_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, i1_128, i2_128) == i1_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones,  i1_128, i2_128) == i1_128);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, i1_128, i2_128) == i2_128);
#endif

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size1, arg2_size1) == arg2_size1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size1, arg2_size1) == arg1_size1);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size2, arg2_size2) == arg2_size2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size2, arg2_size2) == arg1_size2);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size3, arg2_size3) == arg2_size3);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size3, arg2_size3) == arg1_size3);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_ones, arg1_size4, arg2_size4) == arg2_size4);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(all_zeros, arg1_size4, arg2_size4) == arg1_size4);


    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size1, arg2_size1) == arg1_size1);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size1, arg2_size1) == arg2_size1);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size2, arg2_size2) == arg1_size2);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size2, arg2_size2) == arg2_size2);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size3, arg2_size3) == arg1_size3);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size3, arg2_size3) == arg2_size3);

    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_ones, arg1_size4, arg2_size4) == arg1_size4);
    EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(all_zeros, arg1_size4, arg2_size4) == arg2_size4);


    unsigned int seed = 0;
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<uint64_t> distrib64;

    for (int i=0; i<10000; ++i) {
        uint64_t value = generate_random_value<uint64_t>(gen, distrib64);

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, pattern1, pattern2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? pattern1 : pattern2));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, pattern1, pattern2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? pattern1 : pattern2));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, i64_1, i64_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? i64_1 : i64_2));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, i64_1, i64_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? i64_1 : i64_2));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, u32_1, u32_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? u32_1 : u32_2));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, u32_1, u32_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? u32_1 : u32_2));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, i32_1, i32_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? i32_1 : i32_2));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, i32_1, i32_2) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? i32_1 : i32_2));

#if HURCHALLA_COMPILER_HAS_UINT128_T()
        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value,  u1_128, u2_128) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? u1_128 : u2_128));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, u1_128, u2_128) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? u1_128 : u2_128));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value,  i1_128, i2_128) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? i1_128 : i2_128));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, i1_128, i2_128) ==
                 (((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? i1_128 : i2_128));
#endif

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, arg1_size1, arg2_size1) ==
                         ref_csel_on_bit_eq_0<BITNUM>(value, arg1_size1, arg2_size1));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, arg1_size1, arg2_size1) ==
                         ref_csel_on_bit_ne_0<BITNUM>(value, arg1_size1, arg2_size1));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, arg1_size2, arg2_size2) ==
                         ref_csel_on_bit_eq_0<BITNUM>(value, arg1_size2, arg2_size2));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, arg1_size2, arg2_size2) ==
                         ref_csel_on_bit_ne_0<BITNUM>(value, arg1_size2, arg2_size2));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, arg1_size3, arg2_size3) ==
                         ref_csel_on_bit_eq_0<BITNUM>(value, arg1_size3, arg2_size3));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, arg1_size3, arg2_size3) ==
                         ref_csel_on_bit_ne_0<BITNUM>(value, arg1_size3, arg2_size3));

        EXPECT_TRUE(cselect_on_bit<BITNUM>::eq_0(value, arg1_size4, arg2_size4) ==
                         ref_csel_on_bit_eq_0<BITNUM>(value, arg1_size4, arg2_size4));
        EXPECT_TRUE(cselect_on_bit<BITNUM>::ne_0(value, arg1_size4, arg2_size4) ==
                         ref_csel_on_bit_ne_0<BITNUM>(value, arg1_size4, arg2_size4));
    }
}



TEST(HurchallaUtil, cselect_on_bit) {
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
