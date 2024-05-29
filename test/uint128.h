// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_TEST_UINT_H_INCLUDED
#define HURCHALLA_UTIL_TEST_UINT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/programming_by_contract.h"
#include <cstdint>


// helper struct: just enough of a uint128 shell to test count_leading_zeros()
// and count_trailing_zeros().
struct uint128 {
    uint64_t hi, lo;
    uint128() : hi(0), lo(0) {}
    uint128(uint64_t x) : hi(0), lo(x) {}
    uint128(const uint128& u) : hi(u.hi), lo(u.lo) {}
    explicit operator unsigned long long() const
    {
        static_assert(hurchalla::ut_numeric_limits<unsigned long long>::digits == 64, "");
        return lo;
    }
    explicit operator unsigned long() const
    {
        static_assert(hurchalla::ut_numeric_limits<unsigned long>::digits <= 64, "");
        return static_cast<unsigned long>(lo);
    }
    uint128& operator=(uint128 u)
    {
        hi = u.hi;
        lo = u.lo;
        return *this;
    }
    bool operator!=(uint128 u) const
    {
        return hi != u.hi || lo != u.lo;
    }
#ifdef __GNUC__ 
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif
    uint128 operator<<(int shift) const
    {
        uint128 tmp = *this;
        HPBC_ASSERT2(shift < 128 && shift >= 0);
        if (shift == 0) {
        }
        else if (shift >= 64) {
            tmp.hi = tmp.lo << (shift - 64);
            tmp.lo = 0;
        }
        else {
            tmp.hi = (tmp.hi << shift) | (tmp.lo >> (64 - shift));
            tmp.lo = tmp.lo << shift;
        }
        return tmp;
    }
    uint128 operator>>(int shift) const
    {
        uint128 tmp = *this;
        HPBC_ASSERT2(shift < 128 && shift >= 0);
        if (shift == 0) {
        }
        else if (shift >= 64) {
            tmp.lo = tmp.hi >> (shift - 64);
            tmp.hi = 0;
        }
        else {
            tmp.lo = (tmp.lo >> shift) | (tmp.hi << (64 - shift));
            tmp.hi = tmp.hi >> shift;
        }
        return tmp;
    }
#ifdef __GNUC__ 
#pragma GCC diagnostic pop 
#endif
    uint128 operator+(uint128 x) const
    {
        uint128 tmp = *this;
        uint64_t oldlo = tmp.lo;
        tmp.lo += x.lo;
        uint64_t carry = 0;
        if (tmp.lo < oldlo)
            ++carry;
        tmp.hi += (x.hi + carry);
        return tmp;
    }
};

template<typename U>
struct hurchalla::ut_numeric_limits<uint128, U> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr int digits = 128;
};
// This section is only needed prior to C++17, and can cause deprecation
// warnings if enabled after C++17
#if __cplusplus < 201703L
template <typename U>
constexpr bool hurchalla::ut_numeric_limits<uint128, U>::is_specialized;
template <typename U>
constexpr bool hurchalla::ut_numeric_limits<uint128, U>::is_signed;
template <typename U>
constexpr bool hurchalla::ut_numeric_limits<uint128, U>::is_integer;
template <typename U>
constexpr int hurchalla::ut_numeric_limits<uint128, U>::digits;
#endif


#endif
