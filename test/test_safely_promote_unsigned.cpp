// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <type_traits>


namespace {

template <typename U, typename P>
void check_type_promotion()
{
    namespace hc = ::hurchalla;
    static_assert(std::is_same<P, typename
                              hc::safely_promote_unsigned<U>::type>::value, "");
    static_assert(std::is_same<const P, typename
                        hc::safely_promote_unsigned<const U>::type>::value, "");
    static_assert(std::is_same<volatile P, typename
                     hc::safely_promote_unsigned<volatile U>::type>::value, "");
    static_assert(std::is_same<const volatile P, typename
               hc::safely_promote_unsigned<const volatile U>::type>::value, "");
}


TEST(HurchallaUtil, safely_promote_unsigned) {
    check_type_promotion<unsigned char, unsigned int>();
    check_type_promotion<unsigned short, unsigned int>();
    check_type_promotion<unsigned int, unsigned int>();
    check_type_promotion<unsigned long int, unsigned long int>();
    check_type_promotion<unsigned long long int, unsigned long long int>();
#if HURCHALLA_COMPILER_HAS_UINT128_T()
    check_type_promotion<__uint128_t, __uint128_t>();
#endif

    // the following statements should cause static_assert compile errors within
    // safely_promote_unsigned, if any of them are enabled:
#if 0
    namespace hc = ::hurchalla;
    using T1 = hc::safely_promote_unsigned<float>::type;
    using T2 = hc::safely_promote_unsigned<short>::type;
    using T3 = hc::safely_promote_unsigned<int>::type;
    using T4 = hc::safely_promote_unsigned<long int>::type;
    using T5 = hc::safely_promote_unsigned<long long int>::type;
#  if HURCHALLA_COMPILER_HAS_UINT128_T()
    using T6 = hc::safely_promote_unsigned<__int128_t>::type;
#  endif
#endif
}

} // end namespace
