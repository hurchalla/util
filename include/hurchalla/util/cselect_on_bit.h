// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_CSELECT_ON_BIT_H_INCLUDED
#define HURCHALLA_UTIL_CSELECT_ON_BIT_H_INCLUDED


#include "hurchalla/util/detail/platform_specific/impl_cselect_on_bit.h"
#include "hurchalla/util/compiler_macros.h"
#include <array>
#include <cstdint>

namespace hurchalla {

// note: in order to make these functions use inline asm, you must compile with
// either gcc or clang, you must compile for x64 or ARM64, and you must define
// HURCHALLA_ALLOW_INLINE_ASM_ALL or HURCHALLA_ALLOW_INLINE_ASM_CSELECT_ON_BIT


template <int BITNUM>
struct cselect_on_bit {
    static_assert(0 <= BITNUM && BITNUM < 64, "");

    // If bit BITNUM in value is equal to 0, returns arg1.  Else returns arg2.
    static HURCHALLA_FORCE_INLINE
    uint64_t eq_0(uint64_t value, uint64_t arg1, uint64_t arg2)
    {
        std::array<uint64_t,1> ret = detail::impl_cselect_on_bit<BITNUM>::eq_0(
            value, std::array<uint64_t,1>{arg1}, std::array<uint64_t,1>{arg2});
        return ret[0];
    }

    // If bit BITNUM in value is not equal to 0, returns arg1.  Else returns arg2.
    static HURCHALLA_FORCE_INLINE
    uint64_t ne_0(uint64_t value, uint64_t arg1, uint64_t arg2)
    {
        std::array<uint64_t,1> ret = detail::impl_cselect_on_bit<BITNUM>::ne_0(
            value, std::array<uint64_t,1>{arg1}, std::array<uint64_t,1>{arg2});
        return ret[0];
    }

    // Array versions.  We support array sizes from 1 to 4

    // If bit BITNUM in value is equal to 0, returns arg1.  Else returns arg2.
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        static_assert(1 <= ARRAY_SIZE && ARRAY_SIZE <= 4, "");
        return detail::impl_cselect_on_bit<BITNUM>::eq_0(value, arg1, arg2);
    }

    // If bit BITNUM in value is not equal to 0, returns arg1.  Else returns arg2.
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        static_assert(1 <= ARRAY_SIZE && ARRAY_SIZE <= 4, "");
        return detail::impl_cselect_on_bit<BITNUM>::ne_0(value, arg1, arg2);
    }
};


} // end namespace

#endif
