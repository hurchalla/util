// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---


#include "hurchalla/util/branchless_shift_left.h"
#include "hurchalla/util/branchless_shift_right.h"
#include "hurchalla/util/branchless_large_shift_left.h"
#include "hurchalla/util/branchless_small_shift_right.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <random>
#include <vector>

namespace {


template <typename U>
U generate_random_value(std::mt19937_64& gen,
                        std::uniform_int_distribution<uint64_t>& distrib64)
{
   if HURCHALLA_CPP17_CONSTEXPR (hurchalla::ut_numeric_limits<U>::digits > 64) {
      uint64_t u1 = distrib64(gen);
      uint64_t u2 = distrib64(gen);
      U val = static_cast<U>((static_cast<U>(u2) << 64u) | static_cast<U>(u1));
      return val;
   } else {
      return static_cast<U>(distrib64(gen));
   }
}


template <typename T>
void test_branchless_shifts()
{
    namespace hc = ::hurchalla;
    static_assert(hc::ut_numeric_limits<T>::is_integer, "");
    static_assert(!(hc::ut_numeric_limits<T>::is_signed), "");

    unsigned int seed = 0;
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<uint64_t> distrib64;

    constexpr int bitsT = hc::ut_numeric_limits<T>::digits;
    constexpr int bitsNative = HURCHALLA_TARGET_BIT_WIDTH;
    constexpr int smallShiftMax = (bitsT < bitsNative) ? bitsT : bitsNative;
    // a large shift requires  0 <= shift, and  bitsT - bitsNative <= shift < bitsT
    constexpr int largeShiftMin = (bitsT < bitsNative) ? 0 : bitsT - bitsNative;

    constexpr T maxT = hc::ut_numeric_limits<T>::max();

    std::vector<T> vec;

    vec.push_back(0);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(maxT);
    vec.push_back(maxT - 1);
    vec.push_back(maxT - 2);
    vec.push_back(maxT - 3);
    vec.push_back(maxT/2);
    vec.push_back(maxT/2 - 1);
    vec.push_back(maxT/2 - 2);
    vec.push_back(maxT/2 - 3);
    vec.push_back(maxT/2 + 1);
    vec.push_back(maxT/2 + 2);
    vec.push_back(maxT/2 + 3);

    for (int i=0; i<10000; ++i) {
        T val = generate_random_value<T>(gen, distrib64);
        vec.push_back(val);
    }

    for (auto val : vec) {
        for (int i=0; i<bitsT; ++i) {
            EXPECT_TRUE(hc::branchless_shift_left(val, i) == static_cast<T>(val << i));
            EXPECT_TRUE(hc::branchless_shift_right(val, i) == static_cast<T>(val >> i));
        }
        for (int i=0; i<smallShiftMax; ++i) {
            EXPECT_TRUE(hc::branchless_small_shift_right(val, i) == (val >> i));
        }
        for (int i=largeShiftMin; i<bitsT; ++i) {
            EXPECT_TRUE(hc::branchless_large_shift_left(val, i) == static_cast<T>(val << i));
        }
    }
}


TEST(HurchallaUtil, left_and_right_branchless_shifts) {
    test_branchless_shifts<std::uint8_t>();
    test_branchless_shifts<std::uint16_t>();
    test_branchless_shifts<std::uint32_t>();
    test_branchless_shifts<std::uint64_t>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    test_branchless_shifts<__uint128_t>();
#endif
}


} // end unnamed namespace
