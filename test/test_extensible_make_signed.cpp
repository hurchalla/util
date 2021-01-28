// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/traits/extensible_make_signed.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <type_traits>


namespace {

template <typename T, typename S>
void check_type_conversion()
{
    namespace hc = hurchalla;
    static_assert(std::is_same<S, typename
                               hc::extensible_make_signed<T>::type>::value, "");
    static_assert(std::is_same<const S, typename
                         hc::extensible_make_signed<const T>::type>::value, "");
    static_assert(std::is_same<volatile S, typename
                      hc::extensible_make_signed<volatile T>::type>::value, "");
    static_assert(std::is_same<const volatile S, typename
                hc::extensible_make_signed<const volatile T>::type>::value, "");
}


TEST(HurchallaUtil, extensible_make_signed) {
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    check_type_conversion<__int128_t, __int128_t>();
    check_type_conversion<__uint128_t, __int128_t>();
#endif
    check_type_conversion<std::int64_t, std::int64_t>();
    check_type_conversion<std::uint64_t, std::int64_t>();
    check_type_conversion<std::int32_t, std::int32_t>();
    check_type_conversion<std::uint32_t, std::int32_t>();
    check_type_conversion<std::int16_t, std::int16_t>();
    check_type_conversion<std::uint16_t, std::int16_t>();
    check_type_conversion<std::int8_t, std::int8_t>();
    check_type_conversion<std::uint8_t, std::int8_t>();

    // the following statement should cause a static_assert compile error within
    // extensible_make_signed, if it is enabled:
#if 0
    using T = hurchalla::extensible_make_signed<float>::type;
#endif
}

} // end namespace
