// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/BitpackedUintVector.h"
#include "hurchalla/util/sized_uint.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <random>
#include <cstring>

namespace {



template <typename U, unsigned int BITS>
void check_buv(std::vector<uint64_t>& vec)
{
    namespace hc = ::hurchalla;
    static_assert(BITS > 0, "");

    U tmp = (static_cast<U>(1) << (BITS-1));
    U mask = static_cast<U>(tmp + (tmp - 1));

    hc::BitpackedUintVector<U, BITS> buv(vec.size());
    using size_type = typename decltype(buv)::size_type;

    EXPECT_TRUE(buv.size() == vec.size());


    EXPECT_TRUE(buv.max_allowed_value() == mask);

    // nothing to test with getFormatID() except to check it compiles
    uint64_t id = buv.getFormatID();
    (void)id;

    for (size_type i = 0; i < vec.size(); ++i) {
        U val = static_cast<U>(mask & vec[i]);
        buv.setAt(i, val);
        EXPECT_TRUE(val == buv.getAt(i));
    }
    for (size_type i = 0; i < vec.size(); ++i) {
        U val = static_cast<U>(mask & vec[i]);
        EXPECT_TRUE(val == buv.getAt(i));
    }

    // test serialization / deserialization

    // serialize
    const unsigned char* data = buv.data();
    std::size_t num_bytes = buv.dataSizeBytes();
    size_type num_elements = buv.size();

    // use memcpy to roughly fake write/read to file, network, etc.
    std::unique_ptr<unsigned char[]> buffer(new unsigned char[num_bytes]());
    std::memcpy(buffer.get(), data, num_bytes);

    // deserialize
    hc::BitpackedUintVector<U, BITS> buv2(std::move(buffer), num_bytes,
                                          num_elements);
    EXPECT_TRUE(buv2.size() == buv.size());
    EXPECT_TRUE(buv2.size() == vec.size());

    for (size_type i = 0; i < buv2.size(); ++i) {
        U val = static_cast<U>(mask & vec[i]);
        EXPECT_TRUE(buv2.getAt(i) == val);
        EXPECT_TRUE(buv2.getAt(i) == buv.getAt(i));
    }
}



template <int x>
struct HighestSetBit
{
    static_assert(x > 1, "");
    static constexpr int value = 1 + HighestSetBit<x/2>::value;
};
template <>
struct HighestSetBit<1>
{
    static constexpr int value = 0;
};


template <int BITS>
void test_bpuv(std::vector<uint64_t>& vec1, std::vector<uint64_t>& vec2,
               std::vector<uint64_t>& vec3, std::vector<uint64_t>& vec4,
               std::vector<uint64_t>& vec5, std::vector<uint64_t>& vec6)
{
    namespace hc = ::hurchalla;

    //using U = uint64_t;   // uint64_t should work fine
    constexpr int tmp = 1 << HighestSetBit<BITS>::value;
    constexpr int tmp2 = (BITS < 8) ? 8 : ((BITS == tmp) ? tmp : 2*tmp);
    using U = typename hc::sized_uint<tmp2>::type;

    check_buv<U, BITS>(vec1);
    check_buv<U, BITS>(vec2);
    check_buv<U, BITS>(vec3);
    check_buv<U, BITS>(vec4);
    check_buv<U, BITS>(vec5);
    check_buv<U, BITS>(vec6);
}



TEST(HurchallaUtil, BitpackedUintVector) {
    constexpr uint64_t max64 = std::numeric_limits<uint64_t>::max();
    std::mt19937_64 mt;
    std::vector<uint64_t> vec1, vec2, vec3, vec4, vec5, vec6;

    vec1.push_back(0);

    vec2.push_back(max64);

    vec3.push_back(static_cast<uint64_t>(mt()));

    vec4.push_back(0);
    vec4.push_back(max64);
    vec4.push_back(0);
    vec4.push_back(max64);
    vec4.push_back(0);
    vec4.push_back(max64);

    vec5.push_back(max64);
    vec5.push_back(0);
    vec5.push_back(max64);
    vec5.push_back(0);
    vec5.push_back(max64);
    vec5.push_back(0);

    for (int i=0; i<3000; ++i)
        vec6.push_back(static_cast<uint64_t>(mt()));

    test_bpuv<1>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<2>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<3>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<4>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<5>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<6>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<7>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<8>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<9>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<10>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<11>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<12>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<13>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<14>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<15>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<16>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<17>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<18>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<19>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<20>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<21>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<22>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<23>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<24>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<25>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<26>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<27>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<28>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<29>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<30>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<31>(vec1, vec2, vec3, vec4, vec5, vec6);
    test_bpuv<32>(vec1, vec2, vec3, vec4, vec5, vec6);
}


} // end namespace
