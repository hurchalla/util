// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_BITPACKED_UINT_VECTOR_H_INCLUDED
#define HURCHALLA_UTIL_BITPACKED_UINT_VECTOR_H_INCLUDED


#include "hurchalla/util/detail/ImplBitpackedUintVector.h"
#include "hurchalla/util/programming_by_contract.h"
#include <cstdint>
#include <limits>
#include <memory>

namespace hurchalla {


// This class is intended to provide the following, in order of importance:
// 1) Provides a very large integer buffer (often expected to be above
//    1 billion integers) with minimal memory use, when all of the integers
//    are known to use only a limited number of bits (e.g. uint16_t values
//    that never use more than 13 bits).
// 2) Provides portable serialization and deserialization functions (i.e.
//    endianness and alignment are non-issues).
// 3) Excellent performance, with efficient utilization of CPU caches.
//    Requirements 1 and 2 may acceptably cause loss in performance, though
//    read/write speed should be extremely good (perhaps near optimal) when
//    accesses of memory or CPU cache is a bottleneck.


template <typename U, unsigned int element_bitlen>
struct BitpackedUintVector
{
    static_assert(std::numeric_limits<U>::is_integer, "");
    static_assert(!std::numeric_limits<U>::is_signed, "");
    static_assert(element_bitlen <= std::numeric_limits<U>::digits, "");
    static_assert(0 < element_bitlen && element_bitlen <= 32, "");
    using size_type = typename
                  detail::ImplBitpackedUintVector<U, element_bitlen>::size_type;

    BitpackedUintVector(const BitpackedUintVector&) = delete;
    BitpackedUintVector(BitpackedUintVector&& other) :
            impl_buv(std::move(other.impl_buv)) {}

    BitpackedUintVector(size_type count) : impl_buv(count) {}

    // constructor for deserialization.  Note: use data(), dataSizeBytes(), and
    // size() to serialize.
    BitpackedUintVector(std::unique_ptr<unsigned char[]> data,
                        std::size_t data_bytes,
                        size_type element_count) :
            impl_buv(std::move(data), data_bytes, element_count) {}


    HURCHALLA_FORCE_INLINE void setAt(size_type index, U value)
    {
        HPBC_PRECONDITION2(value <= max_allowed_value());
        HPBC_PRECONDITION2(index < size());
        impl_buv.setAt(index, value);
    }

    HURCHALLA_FORCE_INLINE U getAt(size_type index) const
    {
        HPBC_PRECONDITION2(index < size());
        U value = impl_buv.getAt(index);
        HPBC_POSTCONDITION2(value <= max_allowed_value());
        return value;
    }

    // returns the number of packed elements in this vector
    HURCHALLA_FORCE_INLINE size_type size() const
    {
        return impl_buv.size();
    }

    // returns the maximum value that fits within element_bitlen bits.
    HURCHALLA_FORCE_INLINE static constexpr U max_allowed_value()
    {
        return decltype(impl_buv)::max_allowed_value();
    }

    // returns 0 if element_count is an invalid size
    HURCHALLA_FORCE_INLINE static constexpr
    std::size_t dataSizeBytes(size_type element_count)
    {
        return decltype(impl_buv)::dataSizeBytes(element_count);
    }

    HURCHALLA_FORCE_INLINE std::size_t dataSizeBytes() const
    {
        return impl_buv.dataSizeBytes();
    }

    HURCHALLA_FORCE_INLINE const unsigned char* data() const
    {
        return impl_buv.data();
    }

    // You can use getFormatID() to get a data format ID to store (or transmit)
    // along with serialized data that you got from data().
    // Then later you or another person can call getFormatID() as part of a
    // handshake to ensure matching format ID, prior to using that serialized
    // data (for the BitpackedUintVector constructor).
    HURCHALLA_FORCE_INLINE static constexpr uint32_t getFormatID()
    {
        return decltype(impl_buv)::getFormatID();
    }

private:
    detail::ImplBitpackedUintVector<U, element_bitlen> impl_buv;
};


} // end namespace

#endif
