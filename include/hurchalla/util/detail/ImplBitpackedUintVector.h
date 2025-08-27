// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_BITPACKED_UINT_VECTOR_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_BITPACKED_UINT_VECTOR_H_INCLUDED


#include "hurchalla/util/detail/util_programming_by_contract.h"
#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include <limits>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <stdexcept>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif

namespace hurchalla { namespace detail {


// ImplBitpackedUintVector is a storage class for unsigned integers (of
// template type U) that are known to be (and guaranteed by the user to be)
// less than 2^element_bitlen.  Element_bitlen is also a template argument.
// Using this constraint, this class provides extremely space-efficient storage
// in memory.  This class provides random access, so reads and writes should
// be close to as optimal as an array or std::vector, though this class has
// additional overhead because the accesses are in general not aligned.
//
// This class is named after a bit vector, but instead of working on elements
// that are single bit sized (like a bit vector), the elements in this class
// each take up a number of bits equal to element_bitlen.
//
// This class has some similarity to an array, in that after being constructed,
// the maximum number of elements it can hold is constant, and any element can
// be read or written.  However, similarly to a std::vector, its storage is
// allocated on the heap.


template <typename U, unsigned int element_bitlen>
struct ImplBitpackedUintVector
{
    static_assert(std::numeric_limits<U>::is_integer, "");
    static_assert(!std::numeric_limits<U>::is_signed, "");
    static_assert(element_bitlen > 0, "");
    static_assert(element_bitlen <= std::numeric_limits<U>::digits, "");

    // if element_bitlen < 8, in theory we can hold an element count that is
    // larger than max size_t.  We presume(!) uint64_t will always be enough.
    using size_type = typename std::conditional<
            (element_bitlen < 8 && sizeof(std::uint64_t) > sizeof(std::size_t)),
            std::uint64_t, std::size_t>::type;

// Reading/writing to a pointer obtained via
// reinterpret_cast<NoAliasUchar*>(some unsigned char pointer)
// would officially by undefined behavior prior to C++20, since that's when
// P0593R6 (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0593r6.html)
// was acceptd into the standard.  P0593R6 formally standardizes compilers'
// already existing practice of implicitly creating objects as needed within
// unsigned char* buffers.  See the paper for details.  My understanding of it
// is that you can view the unsigned char* as being an alias to the implicitly
// created objects, rather than the implicitly created objects being an alias
// to the unsigned char* buffer.  Note: Unsigned char* is permitted to alias
// anything; if we read/write to a reinterpret_cast<NoAliasUchar*>(uchar*), then
// NoAliasUchar objects would implicitly be created in the uchar* buffer.
//
// Avoid the undefined behavior that would exist prior to C++20.
#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    // unsigned char* is allowed to alias almost everything, so it can be
    // a performance hit to use it.  Just using __restrict__ (as we do in #else)
    // isn't enough since with restrict nothing can alias to it, but it can
    // itself still alias other things like *this.  It's awkward at best trying
    // to specify that *this doesn't alias it.
    // char8_t is an unsigned type introduced in C++20, with the same size as
    // unsigned char, and a char8_t pointer is assumed to never alias any type
    // other than its own (unlike char* and unsigned char*).
    // Again, the acceptance of P0593R6 into C++20 is what makes using char8_t*
    // reinterpret_casts on unsigned char* (in our particular case) a defined
    // behavior.  See the paper.
    using NoAliasUcharPtr = char8_t*;
#else
    using NoAliasUcharPtr = unsigned char* HURCHALLA_RESTRICT;
#endif

private:
    const size_type packed_count;
    const std::size_t vec8_bytes;
    static_assert(std::numeric_limits<unsigned char>::digits == 8, "");
    static constexpr uint8_t dataVersion = 1;
    std::unique_ptr<unsigned char[]> upvec;
    NoAliasUcharPtr vec8;

public:
    ImplBitpackedUintVector(const ImplBitpackedUintVector&) = delete;
    ImplBitpackedUintVector& operator=(const ImplBitpackedUintVector&) = delete;

    ImplBitpackedUintVector(ImplBitpackedUintVector&& other) noexcept :
          packed_count(other.packed_count),
          vec8_bytes(other.vec8_bytes),
          upvec(std::move(other.upvec)),
          vec8(reinterpret_cast<NoAliasUcharPtr>(upvec.get()))
    {}

    ImplBitpackedUintVector(size_type count) :
          packed_count(count),
          vec8_bytes(getBytesFromCount(count)),
          upvec(std::unique_ptr<unsigned char[]>(new unsigned char[vec8_bytes]())),
          vec8(reinterpret_cast<NoAliasUcharPtr>(upvec.get()))
    {}
    // constructor for deserialization.  Note: use data(), dataSizeBytes(), and
    // size() to serialize.
    ImplBitpackedUintVector(std::unique_ptr<unsigned char[]> data,
                        std::size_t data_bytes,
                        size_type element_count) :
          packed_count(element_count),
          vec8_bytes(data_bytes),
          upvec(std::move(data)),
          vec8(reinterpret_cast<NoAliasUcharPtr>(upvec.get()))
    {
        std::size_t bytes_needed = getBytesFromCount(element_count);
        if (data_bytes != bytes_needed)
            throw std::length_error("data_bytes doesn't match expected bytes needed for element_count");
    }

    // returns the number of packed elements in this vector
    size_type size() const
    {
        return packed_count;
    }

    // returns the maximum value that fits within element_bitlen bits.
    static constexpr U max_allowed_value()
    {
        // We don't use (static_cast<U>(1) << (element_bitlen))
        // because that would be undefined behavior if
        // (element_bitlen == std::numeric_limits<U>::digits).
        // Instead we build up the expression using half that value.
        return (static_cast<U>(1) << (element_bitlen-1))
                - 1 + (static_cast<U>(1) << (element_bitlen-1));
    }

    static constexpr uint32_t getFormatID()
    {
        // The constant used below should never be changed (change the
        // internal dataVersion if this file changes to use a different
        // data format/data encoding).  The exact constant value used below
        // was just a random number, but it needs to stay constant.
        return UINT32_C(2132441242) + dataVersion;
    }

    // returns 0 if count is an invalid size
    static constexpr
    std::size_t dataSizeBytes(size_type count)
    {
        std::size_t starting_byte = 0;
        std::size_t bit_offset = 0;
        bool overflowed = false;
        attemptGetLocationFromIndex(count, starting_byte, bit_offset, overflowed);
        if (overflowed)
           return 0;

        constexpr std::size_t MAXSIZET= std::numeric_limits<std::size_t>::max();
        std::size_t bytes_needed = starting_byte;
        if (bit_offset != 0) {
            HPBC_UTIL_CONSTEXPR_ASSERT(bit_offset < 8);
            if (bytes_needed == MAXSIZET)
                return 0;
            else
                ++bytes_needed;
        }
        // We want to allocate one byte more than we strictly need, so that
        // setAt() can safely write one byte beyond where it strictly should.
        if (bytes_needed == MAXSIZET)
            return 0;
        else
            ++bytes_needed;
        return bytes_needed;
    }

    std::size_t dataSizeBytes() const
    {
        return vec8_bytes;
    }

    const unsigned char* data() const
    {
        return upvec.get();
    }


private:
    // count is the number of packed elements we wish to allocate,
    // each of size element_bitlen.
    static std::size_t getBytesFromCount(size_type count)
    {
        std::size_t bytes_needed = dataSizeBytes(count);
        if (bytes_needed == 0)
           throw std::length_error("BitpackedUintVector size too large, would overflow");

        // Since dataSizeBytes() returned without overflow, we know that any
        // value <= 'count' can be converted into 'starting_byte' and
        // 'bit_offset' correctly (i.e. without overflow).  This lets us
        // establish the class invariant that any index that satisfies
        // (index < count) will be converted without overflow by
        // attemptGetLocationFromIndex().

        return bytes_needed;
    }


    // if this function returns with overflowed == false,
    // then it guarantees any value <= 'index' can be converted into
    // 'starting_byte' and 'bit_offset' correctly (i.e. without overflow).
    HURCHALLA_FORCE_INLINE static constexpr
    void attemptGetLocationFromIndex(size_type index,
          std::size_t& starting_byte, std::size_t& bit_offset, bool& overflowed)
    {
        constexpr size_type MAX_ST = std::numeric_limits<size_type>::max();
        static_assert(element_bitlen <= MAX_ST, "");
        constexpr size_type ELEM_BITLEN = static_cast<size_type>(element_bitlen);
        overflowed = false;
#if 0
#  error "do not use this code section - it does not account for overflow danger"
        // We essentially need to do the following, but without the
        // potential for overflow.
        size_type total_bits = index * ELEM_BITLEN;
        starting_byte = total_bits / 8;
        bit_offset = total_bits % 8;
#else
        // We re-express index as
        // index == (index/8)*8 + (index%8)
        // Instead of using  starting_byte = (index * ELEM_BITLEN) / 8
        // we use  starting_byte = ((index/8)*8 + (index%8)) * ELEM_BITLEN / 8
        // This gives us
        // starting_byte = ((index/8)*ELEM_BITLEN) + ((index%8)*ELEM_BITLEN/8)

        // if ((index/8)*ELEM_BITLEN > MAX_ST)
        if ((index/8) > (MAX_ST/ELEM_BITLEN))
           overflowed = true;

        size_type part1_bytes = (index/8) * ELEM_BITLEN;
        // static assert that (index%8)*ELEM_BITLEN will not overflow.
        static_assert(ELEM_BITLEN <= MAX_ST / (8-1), "");
        size_type part2_bytes = (index%8) * ELEM_BITLEN / 8;
        // if (part1_bytes + part2_bytes > MAX_ST)
        if (part1_bytes > MAX_ST - part2_bytes)
           overflowed = true;
        size_type sum = part1_bytes + part2_bytes;

        // According to https://en.cppreference.com/w/cpp/types/size_t,
        // "std::size_t can store the maximum size of a theoretically possible
        // object of any type (including array)."
        // Therefore starting_byte must be able to fit within size_t.
        if (sum > std::numeric_limits<std::size_t>::max())
           overflowed = true;
        starting_byte = static_cast<std::size_t>(sum);

        using P = typename safely_promote_unsigned<size_type>::type;
        // Below: (index*ELEM_BITLEN) might overflow, but since we use the
        // product mod 8, that's ok so long as we use unsigned arithmetic.
        bit_offset = static_cast<std::size_t>(
                                     (static_cast<P>(index) * ELEM_BITLEN) % 8);
#endif
        HPBC_UTIL_CONSTEXPR_POSTCONDITION(bit_offset < 8);
    }



// 8 bit, 16 bit, 24, 32 etc functions
private:
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE typename std::enable_if<(BITS % 8 == 0), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        constexpr decltype(element_bitlen) element_bytes =
                      static_cast<decltype(element_bitlen)>(element_bitlen / 8);
          // The constructor established a class invariant that any
          // index < size() will convert correctly by attemptGetLocationFromIndex().
          // Thus we don't need to check for overflow of  index * element_bytes
        starting_byte = static_cast<std::size_t>(index * element_bytes);
        bit_offset = 0;

        if (HPBC_UTIL_ASSERT2_MACRO_IS_ACTIVE) {
            std::size_t start, offset;
            bool overflowed;
            attemptGetLocationFromIndex(index, start, offset, overflowed);
            HPBC_UTIL_ASSERT2(overflowed == false);
            HPBC_UTIL_ASSERT2(offset == bit_offset);
            HPBC_UTIL_ASSERT2(start == starting_byte);
        }
    }

public:

// 8 bit functions:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 8), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());
        std::size_t index_st = static_cast<std::size_t>(index);
        vec8[index_st] = static_cast<unsigned char>(value);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 8), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());
        std::size_t index_st = static_cast<std::size_t>(index);
        U value = vec8[index_st];
        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return value;
    }

// 16 bit functions:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 16), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 16, "");
        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(value);
        ptr[starting_byte + 1] = static_cast<unsigned char>(value >> 8);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 16), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 16, "");
        U value = static_cast<U>( vec8[starting_byte + 0] +
                               (static_cast<U>(vec8[starting_byte + 1]) << 8) );
        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return value;
    }

// 24 bit functions:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 24), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 24, "");
        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(value);
        ptr[starting_byte + 1] = static_cast<unsigned char>(value >> 8);
        ptr[starting_byte + 2] = static_cast<unsigned char>(value >> 16);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 24), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 24, "");
        U value = vec8[starting_byte + 0] +
                  (static_cast<U>(vec8[starting_byte + 1]) << 8) +
                  (static_cast<U>(vec8[starting_byte + 2]) << 16);
        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return value;
    }

// 32 bit functions:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 32), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 32, "");
        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(value);
        ptr[starting_byte + 1] = static_cast<unsigned char>(value >> 8);
        ptr[starting_byte + 2] = static_cast<unsigned char>(value >> 16);
        ptr[starting_byte + 3] = static_cast<unsigned char>(value >> 24);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 32), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        static_assert(std::numeric_limits<U>::digits >= 32, "");
        U value = vec8[starting_byte + 0] +
                  (static_cast<U>(vec8[starting_byte + 1]) << 8) +
                  (static_cast<U>(vec8[starting_byte + 2]) << 16) +
                  (static_cast<U>(vec8[starting_byte + 3]) << 24);
        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return value;
    }



// 1,2,4 bit functions:
private:
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE
    typename std::enable_if<(BITS == 1)||(BITS == 2)||(BITS == 4), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        static_assert(8 % element_bitlen == 0, "");
        constexpr decltype(element_bitlen) elements_per_byte = 8/element_bitlen;
        static_assert(elements_per_byte != 0, "");

        starting_byte = static_cast<std::size_t>(index / elements_per_byte);
        using P = typename safely_promote_unsigned<size_type>::type;
          // Note: (index*element_bitlen) might overflow, but that's ok so long
          // as we use unsigned arithmetic, since we use the product mod 8.
        bit_offset = static_cast<std::size_t>(
                                 (static_cast<P>(index) * element_bitlen) % 8);

        if (HPBC_UTIL_ASSERT2_MACRO_IS_ACTIVE) {
            std::size_t start, offset;
            bool overflowed;
            attemptGetLocationFromIndex(index, start, offset, overflowed);
            HPBC_UTIL_ASSERT2(overflowed == false);
            HPBC_UTIL_ASSERT2(offset == bit_offset);
            HPBC_UTIL_ASSERT2(start == starting_byte);
        }
          // We know  bit_offset ≡ index*element_bitlen  (mod 8)
          // thus     bit_offset + k*8 = index*element_bitlen  for some k
          // Since    8 ≡ 0  (mod element_bitlen)
          // we have  bit_offset ≡ 0  (mod element_bitlen)
        HPBC_UTIL_ASSERT2(bit_offset % element_bitlen == 0);
          // If element_bitlen == 1, then since bit_offset < 8,
          //    element_bitlen + bit_offset <= 8.
          // If element_bitlen == 2, then since bit_offset < 8,
          //    bit_offset must be 0 or 2 or 4 or 6.  Thus
          //    element_bitlen + bit_offset <= 8.
          // If element_bitlen == 4, then since bit_offset < 8,
          //    bit_offset must be 0 or 4.  Thus
          //    element_bitlen + bit_offset <= 8.
        HPBC_UTIL_POSTCONDITION2(element_bitlen + bit_offset <= 8);
        HPBC_UTIL_POSTCONDITION2(bit_offset < 8);
    }
public:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 1)||(BITS == 2)||(BITS == 4), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
          // from getLocationFromIndex() we know
        HPBC_UTIL_ASSERT2(element_bitlen + bit_offset <= 8);

        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 8, "");

        uint8_t newbits = static_cast<uint8_t>(
                            static_cast<uint8_t>(value) << bit_offset);

        constexpr uint8_t mask = static_cast<uint8_t>((1 << element_bitlen) - 1);
        auto mask2 = ~(mask << bit_offset);
        auto oldbits = mask2 & vec8[starting_byte];

        vec8[starting_byte] = static_cast<unsigned char>(oldbits | newbits);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 1) || (BITS == 2) || (BITS == 4), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
          // from getLocationFromIndex() we know
        HPBC_UTIL_ASSERT2(element_bitlen + bit_offset <= 8);

        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 8, "");

        uint8_t byte = static_cast<uint8_t>(vec8[starting_byte] >> bit_offset);
        constexpr uint8_t mask = static_cast<uint8_t>((1 << element_bitlen) - 1);
        uint8_t value = mask & byte;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }


// 3,5,6,7 bit functions:
private:
    // note that this function should work for any element_bitlen < 8
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE
    typename std::enable_if<(BITS == 3) || (BITS == 5) ||
                            (BITS == 6) || (BITS == 7), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());
          // The constructor established a class invariant that any
          // index < size() will convert correctly by attemptGetLocationFromIndex().
          // Thus we don't need to check overflowed below- it is always false.
        bool overflowed;
        attemptGetLocationFromIndex(index, starting_byte, bit_offset, overflowed);
        HPBC_UTIL_ASSERT2(overflowed == false);
        // attemptGetLocationFromIndex() guarantees bit_offset < 8
        HPBC_UTIL_POSTCONDITION2(bit_offset < 8);
    }
public:
    // note that this function should work for any element_bitlen < 8
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 3) || (BITS == 5) ||
                            (BITS == 6) || (BITS == 7), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 8, "");

        uint16_t oldword = static_cast<uint16_t>(
                        vec8[starting_byte + 0] +
                        (static_cast<uint16_t>(vec8[starting_byte + 1]) << 8) );
        uint16_t newword = static_cast<uint16_t>(
                                    static_cast<uint16_t>(value) << bit_offset);

        constexpr uint8_t mask= (static_cast<uint8_t>(1) << element_bitlen) - 1;
        uint16_t mask2 = static_cast<uint16_t>(
                                  ~(static_cast<uint16_t>(mask) << bit_offset));
        uint16_t word = static_cast<uint16_t>((mask2 & oldword) | newword);

        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(word);
        ptr[starting_byte + 1] = static_cast<unsigned char>(word >> 8);
    }
    // note that this function should work for any element_bitlen < 8
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 3) || (BITS == 5) ||
                            (BITS == 6) || (BITS == 7), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 8, "");

        uint16_t word = static_cast<uint16_t>(
                        vec8[starting_byte + 0] +
                        (static_cast<uint16_t>(vec8[starting_byte + 1]) << 8) );

        word = static_cast<uint16_t>(word >> bit_offset);
        constexpr uint8_t mask = (static_cast<uint8_t>(1) << element_bitlen) - 1;
        uint16_t value = mask & word;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }


// 9,10,12 bit functions:
private:
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE
    typename std::enable_if<(BITS == 9)||(BITS == 10)||(BITS == 12), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

          // We would like to set  starting_byte = (index * element_bitlen) / 8
          // but (index * element_bitlen) might overflow.  So instead:
        using EBT = decltype(element_bitlen);
        static_assert(element_bitlen >= 8, "");
        constexpr EBT spill = element_bitlen - 8;
          static_assert(spill == 1 || spill == 2 || spill == 4, "");
          // We know  element_bitlen == 8 + spill
          // so  starting_byte = (index * (8 + spill)) / 8
          // and thus  starting_byte = index + (index * spill / 8)
        static_assert(8 % spill == 0, "");
        constexpr EBT spills_per_byte = 8 / spill;
          static_assert(spills_per_byte == 2 || spills_per_byte == 4 ||
                        spills_per_byte == 8, "");
        starting_byte = static_cast<std::size_t>(index + index/spills_per_byte);
          constexpr auto ST_MAX = std::numeric_limits<size_type>::max();
          // The constructor established a class invariant that any
          // index < size() can be converted correctly into starting_byte and
          // bit_offset form, so long as the intermediate calculations are done
          // without overflow.  Therefore we can assert
          HPBC_UTIL_ASSERT2(index <= ST_MAX - index / spills_per_byte);
          // Thus we know the sum in starting_byte couldn't have overflowed.

        using P = typename safely_promote_unsigned<size_type>::type;
          // Below: (index*element_bitlen) might overflow, but since we use the
          // product mod 8, that's ok so long as we use unsigned arithmetic.
        bit_offset = static_cast<std::size_t>(
                                 (static_cast<P>(index) * element_bitlen) % 8);

        if (HPBC_UTIL_ASSERT2_MACRO_IS_ACTIVE) {
            std::size_t start, offset;
            bool overflowed;
            attemptGetLocationFromIndex(index, start, offset, overflowed);
            HPBC_UTIL_ASSERT2(overflowed == false);
            HPBC_UTIL_ASSERT2(offset == bit_offset);
            HPBC_UTIL_ASSERT2(start == starting_byte);
        }

        static_assert(element_bitlen % spill == 0, "");
          // We know  bit_offset ≡ index*element_bitlen  (mod 8)
          // thus     bit_offset + k*8 = index*element_bitlen  for some k
          // Since    8 ≡ 0  (mod spill)   And   element_bitlen ≡ 0  (mod spill)
          // we have  bit_offset ≡ 0  (mod spill)
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        HPBC_UTIL_ASSERT2(bit_offset % spill == 0);
          // If element_bitlen == 9, then since bit_offset <= 7,
          //    element_bitlen + bit_offset <= 16.
          // If element_bitlen == 10, then spill == 2 and so
          //    bit_offset must be 0 or 2 or 4 or 6.  Thus
          //    element_bitlen + bit_offset <= 16.
          // If element_bitlen == 12, then spill == 4 and so
          //    bit_offset must be 0 or 4.  Thus
          //    element_bitlen + bit_offset <= 16.
        HPBC_UTIL_POSTCONDITION2(element_bitlen + bit_offset <= 16);
        HPBC_UTIL_POSTCONDITION2(bit_offset < 8);
    }
public:
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 9)||(BITS == 10)||(BITS == 12), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

          // from getLocationFromIndex() we know
        HPBC_UTIL_ASSERT2(element_bitlen + bit_offset <= 16);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        uint16_t oldword = static_cast<uint16_t>(
                (static_cast<uint16_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint16_t>(vec8[starting_byte + 1]) << 8) );
        uint16_t newword = static_cast<uint16_t>(
                               static_cast<uint16_t>(value) << bit_offset );

        static_assert(element_bitlen < 16, "");
        constexpr uint16_t mask = (static_cast<uint16_t>(1) << element_bitlen) - 1;
        uint16_t mask2 = static_cast<uint16_t>( ~(mask << bit_offset) );
        uint16_t word = (mask2 & oldword) | newword;

        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(word);
        ptr[starting_byte + 1] = static_cast<unsigned char>(word >> 8);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 9)||(BITS == 10)||(BITS == 12), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);

          // from getLocationFromIndex() we know
        HPBC_UTIL_ASSERT2(element_bitlen + bit_offset <= 16);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        uint16_t word = static_cast<uint16_t>(
                (static_cast<uint16_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint16_t>(vec8[starting_byte + 1]) << 8) );

        word = static_cast<uint16_t>(word >> bit_offset);
        static_assert(element_bitlen < 16, "");
        constexpr uint16_t mask = (static_cast<uint16_t>(1) << element_bitlen) - 1;
        uint16_t value = mask & word;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }


// 11,13,14,15 bit functions:
private:
    // note that this function should work for any  8 < element_bitlen < 16
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE
    typename std::enable_if<(BITS == 11) || (BITS == 13) ||
                            (BITS == 14) || (BITS == 15), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());
          // The constructor established a class invariant that any
          // index < size() will convert correctly by attemptGetLocationFromIndex().
          // Thus we don't need to check overflowed below- it is always false.
        bool overflowed;
        attemptGetLocationFromIndex(index, starting_byte, bit_offset, overflowed);
        HPBC_UTIL_ASSERT2(overflowed == false);
        // attemptGetLocationFromIndex() guarantees bit_offset < 8
        HPBC_UTIL_POSTCONDITION2(bit_offset < 8);
    }
public:
    // note that this function should work for any  8 < element_bitlen < 16
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 11) || (BITS == 13) ||
                            (BITS == 14) || (BITS == 15), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 16, "");

        uint32_t oldword =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16);
        uint32_t newword = static_cast<uint32_t>(value) << bit_offset;

        constexpr uint16_t mask = (static_cast<uint16_t>(1) << element_bitlen) - 1;
        uint32_t mask2 = ~(static_cast<uint32_t>(mask) << bit_offset);
        uint32_t word = (mask2 & oldword) | newword;

        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(word);
        ptr[starting_byte + 1] = static_cast<unsigned char>(word >> 8);
        ptr[starting_byte + 2] = static_cast<unsigned char>(word >> 16);
    }
    // note that this function should work for any  8 < element_bitlen < 16
    template <int BITS = element_bitlen>
    typename std::enable_if<(BITS == 11) || (BITS == 13) ||
                            (BITS == 14) || (BITS == 15), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 16, "");

        uint32_t word =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16);

        word = word >> bit_offset;
        constexpr uint16_t mask = (static_cast<uint16_t>(1) << element_bitlen) - 1;
        uint32_t value = mask & word;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }



// 17 to 23 bit, and 25 to 31 bit functions:
// (note these could be optimized for bitdepths 17 18 20 25 26 28 - similar to
// the optimizations made above for the 9 10 12 bit functions)
private:
    template <int BITS = element_bitlen>
    HURCHALLA_FORCE_INLINE
    typename std::enable_if<((16 < BITS && BITS < 24) ||
                             (24 < BITS && BITS < 32)), void>::type
    getLocationFromIndex(size_type index,
                      std::size_t& starting_byte, std::size_t& bit_offset) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());
          // The constructor established a class invariant that any
          // index < size() will convert correctly by attemptGetLocationFromIndex().
          // Thus we don't need to check overflowed below- it is always false.
        bool overflowed;
        attemptGetLocationFromIndex(index, starting_byte, bit_offset, overflowed);
        HPBC_UTIL_ASSERT2(overflowed == false);
        // attemptGetLocationFromIndex() guarantees bit_offset < 8
        HPBC_UTIL_POSTCONDITION2(bit_offset < 8);
    }
public:
// 17 to 23 bit:
    template <int BITS = element_bitlen>
    typename std::enable_if<(16 < BITS && BITS < 24), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 24, "");

        uint32_t oldword =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16) +
                (static_cast<uint32_t>(vec8[starting_byte + 3]) << 24);
        uint32_t newword = static_cast<uint32_t>(value) << bit_offset;

        constexpr uint32_t mask = (static_cast<uint32_t>(1) << element_bitlen) - 1;
        uint32_t mask2 = ~(static_cast<uint32_t>(mask) << bit_offset);
        uint32_t word = (mask2 & oldword) | newword;

        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(word);
        ptr[starting_byte + 1] = static_cast<unsigned char>(word >> 8);
        ptr[starting_byte + 2] = static_cast<unsigned char>(word >> 16);
        ptr[starting_byte + 3] = static_cast<unsigned char>(word >> 24);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(16 < BITS && BITS < 24), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 24, "");

        uint32_t word =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16) +
                (static_cast<uint32_t>(vec8[starting_byte + 3]) << 24);

        word = word >> bit_offset;
        constexpr uint32_t mask = (static_cast<uint32_t>(1) << element_bitlen) - 1;
        uint32_t value = mask & word;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }

// 25 to 31 bit:
    template <int BITS = element_bitlen>
    typename std::enable_if<(24 < BITS && BITS < 32), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 32, "");

        uint32_t oldword32 =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16) +
                (static_cast<uint32_t>(vec8[starting_byte + 3]) << 24);
        uint64_t oldword = oldword32 +
                (static_cast<uint64_t>(vec8[starting_byte + 4]) << 32);
        uint64_t newword = static_cast<uint64_t>(value) << bit_offset;

        constexpr uint32_t mask = (static_cast<uint32_t>(1) << element_bitlen) - 1;
        uint64_t mask2 = ~(static_cast<uint64_t>(mask) << bit_offset);
        uint64_t word = (mask2 & oldword) | newword;

        // mitigate the perf hit if the compiler assumes vec8 could alias *this
        NoAliasUcharPtr ptr = vec8;
        ptr[starting_byte + 0] = static_cast<unsigned char>(word);
        ptr[starting_byte + 1] = static_cast<unsigned char>(word >> 8);
        ptr[starting_byte + 2] = static_cast<unsigned char>(word >> 16);
        ptr[starting_byte + 3] = static_cast<unsigned char>(word >> 24);
        ptr[starting_byte + 4] = static_cast<unsigned char>(word >> 32);
    }
    template <int BITS = element_bitlen>
    typename std::enable_if<(24 < BITS && BITS < 32), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());

        std::size_t starting_byte, bit_offset;
        getLocationFromIndex(index, starting_byte, bit_offset);
        HPBC_UTIL_ASSERT2(bit_offset < 8);
        static_assert(element_bitlen < 32, "");

        uint32_t word32 =
                (static_cast<uint32_t>(vec8[starting_byte + 0]) << 0) +
                (static_cast<uint32_t>(vec8[starting_byte + 1]) << 8) +
                (static_cast<uint32_t>(vec8[starting_byte + 2]) << 16) +
                (static_cast<uint32_t>(vec8[starting_byte + 3]) << 24);
        uint64_t word64 = word32 +
                (static_cast<uint64_t>(vec8[starting_byte + 4]) << 32);

        uint32_t word = static_cast<uint32_t>(word64 >> bit_offset);
        constexpr uint32_t mask = (static_cast<uint32_t>(1) << element_bitlen) - 1;
        uint32_t value = mask & word;

        HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
        return static_cast<U>(value);
    }
};







// The code below is completely untested, and also requires C++17.
// It should not be considered finished, but it could be a good starting
// point if you want to add support for generic element_bitlen > 32.

#if 0

#if (__cplusplus >= 201703L)   // requires C++ 17 or greater, for if constexpr
    template <int BITS = element_bitlen>
    typename std::enable_if<(16 < BITS), void>::type
    setAt(size_type index, U value)
    {
        HPBC_UTIL_PRECONDITION2(value <= max_allowed_value());
        HPBC_UTIL_PRECONDITION2(index < size());
        // The constructor and resize() maintain a class invariant that any
        // index < size() will convert correctly by attemptGetLocationFromIndex().
        // Thus we don't need to check overflowed below- it is always false

        size_type starting_byte, bit_offset;
        bool overflowed;
        attemptGetLocationFromIndex(index, starting_byte, bit_offset, overflowed);
        HPBC_UTIL_ASSERT2(overflowed == false);
        HPBC_UTIL_ASSERT2(bit_offset < 8);

        uint8_t mask = (static_cast<uint8_t>(1) << bit_offset) - 1;
        uint8_t oldbits = mask & vec8[starting_byte + 0];
        uint8_t newbits = static_cast<uint8_t>(value << bit_offset);
        vec8[starting_byte + 0] = oldbits | newbits;

          // Section for complete byte copies, with any bit_offset
        uint32_t val32 = static_cast<uint32_t>(value);
        if constexpr (element_bitlen >= 16)
            vec8[starting_byte + 1] = static_cast<uint8_t>(val32 >> (8 - bit_offset));
        if constexpr (element_bitlen >= 24)
            vec8[starting_byte + 2] = static_cast<uint8_t>(val32 >> (16 - bit_offset));
        if constexpr (element_bitlen >= 32)
            vec8[starting_byte + 3] = static_cast<uint8_t>(val32 >> (24 - bit_offset));
        if constexpr (element_bitlen >= 40) {
            static_assert(std::numeric_limits<U>::digits >= 40, "");
            val32 = static_cast<uint32_t>(value >> (32 - bit_offset));
            vec8[starting_byte + 4] = static_cast<uint8_t>(val32);
            if constexpr (element_bitlen >= 48)
                vec8[starting_byte + 5] = static_cast<uint8_t>(val32 >> 8);
            if constexpr (element_bitlen >= 56)
                vec8[starting_byte + 6] = static_cast<uint8_t>(val32 >> 16);
            if constexpr (element_bitlen >= 64)
                vec8[starting_byte + 7] = static_cast<uint8_t>(val32 >> 24);
        }
        if constexpr (element_bitlen >= 72) {
            static_assert(std::numeric_limits<U>::digits >= 72, "");
            val32 = static_cast<uint32_t>(value >> (64 - bit_offset));
            vec8[starting_byte + 8] = static_cast<uint8_t>(val32);
            if constexpr (element_bitlen >= 80)
                vec8[starting_byte + 9] = static_cast<uint8_t>(val32 >> 8);
            if constexpr (element_bitlen >= 88)
                vec8[starting_byte + 10] = static_cast<uint8_t>(val32 >> 16);
            if constexpr (element_bitlen >= 96)
                vec8[starting_byte + 11] = static_cast<uint8_t>(val32 >> 24);
        }
        if constexpr (element_bitlen >= 104) {
            static_assert(std::numeric_limits<U>::digits >= 104, "");
            val32 = static_cast<uint32_t>(value >> (96 - bit_offset));
            vec8[starting_byte + 12] = static_cast<uint8_t>(val32);
            if constexpr (element_bitlen >= 112)
                vec8[starting_byte + 13] = static_cast<uint8_t>(val32 >> 8);
            if constexpr (element_bitlen >= 120)
                vec8[starting_byte + 14] = static_cast<uint8_t>(val32 >> 16);
            if constexpr (element_bitlen >= 128)
                vec8[starting_byte + 15] = static_cast<uint8_t>(val32 >> 24);
        }
          // We'd need to write more clauses above to support larger
          // element_bitlen.  For now, don't try to support unlimited sizes.
        static_assert(element_bitlen <= 128, "");


        if constexpr (element_bitlen % 8 == 0) {
            HPBC_UTIL_ASSERT2(bit_offset == 0);
            // there's nothing more to do.
        } else {
              // when 8 <= element_bitlen < 16, we need (conceptually at least)
              // to write 3 bytes total.
              // when 16 <= element_bitlen < 24, we need 4 bytes total.
              // Etc.
            static_assert(element_bitlen >= 8, "");
            constexpr size_type num_bytes_written = element_bitlen/8;
            static_assert(num_bytes_written >= 1, "");
              // We still need to (conceptually at least) write to 2 more bytes


            auto num_mask_newbits = bit_offset + (element_bitlen % 8);

            static_assert(element_bitlen >= 8, "");
              // so  element_bitlen - 8 >= (element_bitlen - 8) % 8 == element_bitlen % 8
              // and thus,  element_bitlen - 8 >= element_bitlen % 8
              // i.e.  element_bitlen >= 8 + element_bitlen % 8
              // And since  bit_offset < 8
              // element_bitlen > bit_offset + element_bitlen % 8 == num_mask_newbits
              // Therefore
            HPBC_UTIL_ASSERT2(element_bitlen > num_mask_newbits);
              // Since bit_offset <= 7 and (element_bitlen % 8) <= 7,
              // num_mask_newbits == bit_offset + (element_bitlen % 8) <= 14
            HPBC_UTIL_ASSERT2(0 <= num_mask_newbits && num_mask_newbits <= 14);

            uint16_t mask2 = ~((static_cast<uint16_t>(1) << num_mask_newbits) - 1);


            // given that element_bitlen % 8 != 0, and that value's type
            // presumably uses whole bytes,
            static_assert(element_bitlen < std::numeric_limits<decltype(value)>::digits, "");
            static_assert(element_bitlen >= 16, "");
            value = value >> (element_bitlen - num_mask_newbits);

            mask = static_cast<uint8_t>(mask2);
            oldbits = mask & vec8[starting_byte + num_bytes_written];
            newbits = static_cast<uint8_t>(value);
            vec8[starting_byte + num_bytes_written] = oldbits | newbits;

              // In the special case (element_bitlen % 8 <= 1), we have
              // num_mask_newbits = bit_offset + (element_bitlen % 8) <= bit_offset + 1
              // Since we know bit_offset <= 7, we have
              // num_mask_newbits <= 8
              // So in this case, the second byte never needs any newbits
              // written to it: so for efficiency we do nothing.
            if constexpr (element_bitlen % 8 <= 1) {
// TODO: To improve efficiency further, I suspect any of  element_bitlen % 8 == 1 or 2 or 4
// could be a special case, not just (element_bitlen % 8 <= 1) here.
            }
            else {
                mask = static_cast<uint8_t>(mask2 >> 8);
                oldbits = mask & vec8[starting_byte + num_bytes_written + 1];
                newbits = static_cast<uint8_t>(value >> 8);
                vec8[starting_byte + num_bytes_written + 1] = oldbits | newbits;
            }
        }
    }


    template <int BITS = element_bitlen>
    typename std::enable_if<(16 < BITS), U>::type
    getAt(size_type index) const
    {
        HPBC_UTIL_PRECONDITION2(index < size());
        // The constructor and resize() maintain a class invariant that any
        // index < size() will convert correctly by attemptGetLocationFromIndex().
        // Thus we don't need to check overflowed below- it is always false

        size_type starting_byte, bit_offset;
        bool overflowed;
        attemptGetLocationFromIndex(index, starting_byte, bit_offset, overflowed);
        HPBC_UTIL_ASSERT2(overflowed == false);
        HPBC_UTIL_ASSERT2(bit_offset < 8);

        static_assert(element_bitlen >= 8, "");
        U value = (static_cast<U>(vec8[starting_byte + 0]) << 0);

        if constexpr (element_bitlen >= 16)
            value += (static_cast<U>(vec8[starting_byte + 1]) << 8);
        if constexpr (element_bitlen >= 24)
            value += (static_cast<U>(vec8[starting_byte + 2]) << 16);
        if constexpr (element_bitlen >= 32)
            value += (static_cast<U>(vec8[starting_byte + 3]) << 24);
        if constexpr (element_bitlen >= 40)
            value += (static_cast<U>(vec8[starting_byte + 4]) << 32);
        if constexpr (element_bitlen >= 48)
            value += (static_cast<U>(vec8[starting_byte + 5]) << 40);
        if constexpr (element_bitlen >= 56)
            value += (static_cast<U>(vec8[starting_byte + 6]) << 48);
        if constexpr (element_bitlen >= 64)
            value += (static_cast<U>(vec8[starting_byte + 7]) << 56);

        if constexpr (element_bitlen >= 72)
            value += (static_cast<U>(vec8[starting_byte + 8]) << 64);
        if constexpr (element_bitlen >= 80)
            value += (static_cast<U>(vec8[starting_byte + 9]) << 72);
        if constexpr (element_bitlen >= 88)
            value += (static_cast<U>(vec8[starting_byte + 10]) << 80);
        if constexpr (element_bitlen >= 96)
            value += (static_cast<U>(vec8[starting_byte + 11]) << 88);
        if constexpr (element_bitlen >= 104)
            value += (static_cast<U>(vec8[starting_byte + 12]) << 96);
        if constexpr (element_bitlen >= 112)
            value += (static_cast<U>(vec8[starting_byte + 13]) << 104);
        if constexpr (element_bitlen >= 120)
            value += (static_cast<U>(vec8[starting_byte + 14]) << 112);
        if constexpr (element_bitlen >= 128)
            value += (static_cast<U>(vec8[starting_byte + 15]) << 120);

          // We'd need to write more clauses above to support larger
          // element_bitlen.  For now, don't try to support unlimited sizes.
        static_assert(element_bitlen <= 128, "");

        if constexpr (element_bitlen % 8 == 0) {
            HPBC_UTIL_ASSERT2(bit_offset == 0);
            return value;
        } else {
            value = value >> bit_offset;

              // when 8 <= element_bitlen < 16, we need to read 3 bytes total.
              // when 16 <= element_bitlen < 24, we need 4 bytes total.
              // Etc.
            static_assert(element_bitlen >= 8, "");
            constexpr size_type num_bytes_read = element_bitlen/8;
            static_assert(num_bytes_read >= 1, "");

              // read two extra bytes to account for potential bit_offset > 0
              // along with (element_bitlen % 8) != 0
            uint16_t last_two_bytes =
               (static_cast<uint16_t>(vec8[starting_byte + num_bytes_read]) << 0) +
               (static_cast<uint16_t>(vec8[starting_byte + num_bytes_read + 1]) << 8);

            auto num_mask_newbits = bit_offset + (element_bitlen % 8);
            uint16_t mask2 = (static_cast<uint16_t>(1) << num_mask_newbits) - 1;
            last_two_bytes = last_two_bytes & mask2;

            // given that element_bitlen % 8 != 0, and U presumably using whole bytes,
            static_assert(element_bitlen < std::numeric_limits<U>::digits, "");
            static_assert(element_bitlen >= 16, "");
            value += (static_cast<U>(last_two_bytes) << (element_bitlen - num_mask_newbits));

            HPBC_UTIL_POSTCONDITION2(value <= max_allowed_value());
            return value;

            // TODO if constexpr (element_bitlen % 8 == 1 or 2 or 4) (?)  we could skip reading the final byte
        }
    }
#endif

#endif


}} // end namespace


#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
