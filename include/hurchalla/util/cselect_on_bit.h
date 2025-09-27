// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_CSELECT_ON_BIT_H_INCLUDED
#define HURCHALLA_UTIL_CSELECT_ON_BIT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/detail/platform_specific/impl_cselect_on_bit.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
#include <array>
#include <cstdint>

namespace hurchalla {

// note: in order to make these functions use inline asm, you must compile with
// either gcc or clang, you must compile for x64 or ARM64, and you must define
// HURCHALLA_ALLOW_INLINE_ASM_CSELECT_ON_BIT


template <int BITNUM>
struct cselect_on_bit {
    static_assert(0 <= BITNUM && BITNUM < 64, "");

    // If bit BITNUM in value is equal to 0, returns arg1.  Else returns arg2.
    // T must be an integral type, up to 128 bit.
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    T eq_0(uint64_t value, T arg1, T arg2)
    {
        static_assert(ut_numeric_limits<T>::is_integer, "");
        static_assert(ut_numeric_limits<T>::digits <= 128, "");

        T ret = detail::impl_cselect_scalar_on_bit<BITNUM>::eq_0(value, arg1, arg2);

        HPBC_UTIL_POSTCONDITION2(ret == ((value&(1ull<<BITNUM)) == 0) ? arg1 : arg2);
        return ret;
    }

    // If bit BITNUM in value is not equal to 0, returns arg1.  Else returns arg2.
    // T must be an integral type, up to 128 bit.
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    T ne_0(uint64_t value, T arg1, T arg2)
    {
        static_assert(ut_numeric_limits<T>::is_integer, "");
        static_assert(ut_numeric_limits<T>::digits <= 128, "");

        T ret = detail::impl_cselect_scalar_on_bit<BITNUM>::ne_0(value, arg1, arg2);

        HPBC_UTIL_POSTCONDITION2(ret == ((value&(1ull<<BITNUM)) != 0) ? arg1 : arg2);
        return ret;
    }


    // Uint64_t array versions.  We support array sizes from 1 to 4

    // If bit BITNUM in value is equal to 0, returns arg1.  Else returns arg2.
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE
    std::array<uint64_t,ARRAY_SIZE>
    eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        static_assert(1 <= ARRAY_SIZE && ARRAY_SIZE <= 4, "");

        auto ret = detail::impl_cselect_on_bit<BITNUM>::eq_0(value, arg1, arg2);

        HPBC_UTIL_POSTCONDITION2(ret == (((value&(1ull<<BITNUM)) == 0) ? arg1 : arg2));
        return ret;
    }

    // If bit BITNUM in value is not equal to 0, returns arg1.  Else returns arg2.
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE
    std::array<uint64_t,ARRAY_SIZE>
    ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        static_assert(1 <= ARRAY_SIZE && ARRAY_SIZE <= 4, "");

        auto ret = detail::impl_cselect_on_bit<BITNUM>::ne_0(value, arg1, arg2);

        HPBC_UTIL_POSTCONDITION2(ret == (((value&(1ull<<BITNUM)) != 0) ? arg1 : arg2));
        return ret;
    }
};


} // end namespace

#endif
