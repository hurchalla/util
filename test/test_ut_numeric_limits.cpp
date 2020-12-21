// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <type_traits>


namespace {

template <typename T>
void verify_is_specialized()
{
    namespace ut = hurchalla::util;
    // verify usage during both compile-time and run-time
    static_assert(ut::ut_numeric_limits<T>::is_specialized, "");
    static_assert(ut::ut_numeric_limits<const T>::is_specialized, "");
    static_assert(ut::ut_numeric_limits<volatile T>::is_specialized, "");
    static_assert(ut::ut_numeric_limits<const volatile T>::is_specialized, "");
    EXPECT_TRUE(ut::ut_numeric_limits<T>::is_specialized);
    EXPECT_TRUE(ut::ut_numeric_limits<const T>::is_specialized);
    EXPECT_TRUE(ut::ut_numeric_limits<volatile T>::is_specialized);
    EXPECT_TRUE(ut::ut_numeric_limits<const volatile T>::is_specialized);
}

template <typename T>
void verify_not_specialized()
{
    namespace ut = hurchalla::util;
    // verify usage during both compile-time and run-time
    static_assert(!ut::ut_numeric_limits<T>::is_specialized, "");
    static_assert(!ut::ut_numeric_limits<const T>::is_specialized, "");
    static_assert(!ut::ut_numeric_limits<volatile T>::is_specialized, "");
    static_assert(!ut::ut_numeric_limits<const volatile T>::is_specialized, "");
    EXPECT_FALSE(ut::ut_numeric_limits<T>::is_specialized);
    EXPECT_FALSE(ut::ut_numeric_limits<const T>::is_specialized);
    EXPECT_FALSE(ut::ut_numeric_limits<volatile T>::is_specialized);
    EXPECT_FALSE(ut::ut_numeric_limits<const volatile T>::is_specialized);
}


TEST(HurchallaUtil, ut_numeric_limits) {
    verify_is_specialized<char>();
    verify_is_specialized<signed char>();
    verify_is_specialized<unsigned char>();
    verify_is_specialized<short>();
    verify_is_specialized<unsigned short>();
    verify_is_specialized<int>();
    verify_is_specialized<unsigned int>();
    verify_is_specialized<long int>();
    verify_is_specialized<unsigned long int>();
    verify_is_specialized<long long int>();
    verify_is_specialized<unsigned long long int>();

    verify_is_specialized<float>();
    verify_is_specialized<double>();

    verify_is_specialized<std::int8_t>();
    verify_is_specialized<std::uint8_t>();
    verify_is_specialized<std::int16_t>();
    verify_is_specialized<std::uint16_t>();
    verify_is_specialized<std::int32_t>();
    verify_is_specialized<std::uint32_t>();
    verify_is_specialized<std::int64_t>();
    verify_is_specialized<std::uint64_t>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    verify_is_specialized<__int128_t>();
    verify_is_specialized<__uint128_t>();
    // Do a sanity check on digits10 to see if other variables are correct too.
    namespace ut = hurchalla::util;
    static_assert(ut::ut_numeric_limits<__int128_t>::digits10 == 38, "");
    EXPECT_TRUE(ut::ut_numeric_limits<__int128_t>::digits10 == 38);
#endif

    struct A {};
    verify_not_specialized<A>();

    // Ideally we would do even more tests on more of the members, but testing
    // that 'is_specialized' works as expected is probably representative for
    // catching any incorrect instantiations, which is my main concern.
}

} // end namespace
