// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <type_traits>


namespace {

template <typename T, typename U>
void check_type_conversion()
{
    namespace ut = hurchalla::util;
    static_assert(std::is_same<U, typename
                             ut::extensible_make_unsigned<T>::type>::value, "");
    static_assert(std::is_same<const U, typename
                       ut::extensible_make_unsigned<const T>::type>::value, "");
    static_assert(std::is_same<volatile U, typename
                    ut::extensible_make_unsigned<volatile T>::type>::value, "");
    static_assert(std::is_same<const volatile U, typename
              ut::extensible_make_unsigned<const volatile T>::type>::value, "");
}


TEST(HurchallaUtil, extensible_make_unsigned) {
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    check_type_conversion<__int128_t, __uint128_t>();
    check_type_conversion<__uint128_t, __uint128_t>();
#endif
    check_type_conversion<std::int64_t, std::uint64_t>();
    check_type_conversion<std::uint64_t, std::uint64_t>();
    check_type_conversion<std::int32_t, std::uint32_t>();
    check_type_conversion<std::uint32_t, std::uint32_t>();
    check_type_conversion<std::int16_t, std::uint16_t>();
    check_type_conversion<std::uint16_t, std::uint16_t>();
    check_type_conversion<std::int8_t, std::uint8_t>();
    check_type_conversion<std::uint8_t, std::uint8_t>();

    // the following statement should cause a static_assert compile error within
    // extensible_make_unsigned, if it is enabled:
#if 0
    using T = hurchalla::util::extensible_make_unsigned<float>::type;
#endif
}

} // end namespace
