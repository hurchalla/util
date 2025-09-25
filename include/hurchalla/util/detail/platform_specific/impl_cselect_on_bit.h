// Copyright (c) 2025 Jeffrey Hurchalla.
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_CSELECT_ON_BIT_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_CSELECT_ON_BIT_H_INCLUDED


#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include <cstdint>
#include <type_traits>
#include <array>

namespace hurchalla { namespace detail {



#if defined(HURCHALLA_TARGET_ISA_X86_64) && !defined(_MSC_VER) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_ALL) || defined(HURCHALLA_ALLOW_INLINE_ASM_CSELECT_ON_BIT))

// ---------------------------------- X64 --------------------------------------




template <int BITNUM, class Enable = void>
struct impl_cselect_on_bit {
// Normally we want #if 0, so that this class is empty of all functions - this
// will catch bad calls by a client and produce a compilation error.
// This #if clause remains as runnable documentation of what all specializations
// do.  Possibly, using #if 1 might be useful for testing.
#if 0
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? arg1[i] : arg2[i];
        }
        return result;
    }

    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? arg1[i] : arg2[i];
        }
        return result;
    }
#endif
};



#if defined(__clang__)
    // clang inline asm does not properly support multiple constraits, so we
    // only use "r" here.  See https://github.com/llvm/llvm-project/issues/20571

# define HURCHALLA_CMOVNZ_1_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0])
# define HURCHALLA_CMOVZ_1_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0])
# define HURCHALLA_CMOVNS_1_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0])
# define HURCHALLA_CMOVS_1_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0])

# define HURCHALLA_CMOVNZ_2_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
# define HURCHALLA_CMOVZ_2_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
# define HURCHALLA_CMOVNS_2_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
# define HURCHALLA_CMOVS_2_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])

# define HURCHALLA_CMOVNZ_3_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t" \
                 "cmovnzq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
# define HURCHALLA_CMOVZ_3_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t" \
                 "cmovzq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
# define HURCHALLA_CMOVNS_3_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t" \
                 "cmovnsq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
# define HURCHALLA_CMOVS_3_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t" \
                 "cmovsq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])

# define HURCHALLA_CMOVNZ_4_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t" \
                 "cmovnzq %[arg1_2], %[result_2] \n\t" \
                 "cmovnzq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
# define HURCHALLA_CMOVZ_4_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t" \
                 "cmovzq %[arg1_2], %[result_2] \n\t" \
                 "cmovzq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
# define HURCHALLA_CMOVNS_4_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t" \
                 "cmovnsq %[arg1_2], %[result_2] \n\t" \
                 "cmovnsq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
# define HURCHALLA_CMOVS_4_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t" \
                 "cmovsq %[arg1_2], %[result_2] \n\t" \
                 "cmovsq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])


#else   // gcc
    // gcc inline asm properly supports multiple constraits (unlike clang), so
    // we use "rm" here.  See https://github.com/llvm/llvm-project/issues/20571

# define HURCHALLA_CMOVNZ_1_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0])
# define HURCHALLA_CMOVZ_1_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0])
# define HURCHALLA_CMOVNS_1_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0])
# define HURCHALLA_CMOVS_1_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 : [result_0]"+r"(result[0]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0])

# define HURCHALLA_CMOVNZ_2_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1])
# define HURCHALLA_CMOVZ_2_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1])
# define HURCHALLA_CMOVNS_2_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1])
# define HURCHALLA_CMOVS_2_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t"\
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1])

# define HURCHALLA_CMOVNZ_3_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t" \
                 "cmovnzq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2])
# define HURCHALLA_CMOVZ_3_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t" \
                 "cmovzq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2])
# define HURCHALLA_CMOVNS_3_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t" \
                 "cmovnsq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2])
# define HURCHALLA_CMOVS_3_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t" \
                 "cmovsq %[arg1_2], %[result_2] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2])

# define HURCHALLA_CMOVNZ_4_ARG1_TO_RESULT \
                 "cmovnzq %[arg1_0], %[result_0] \n\t" \
                 "cmovnzq %[arg1_1], %[result_1] \n\t" \
                 "cmovnzq %[arg1_2], %[result_2] \n\t" \
                 "cmovnzq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3])
# define HURCHALLA_CMOVZ_4_ARG1_TO_RESULT \
                 "cmovzq %[arg1_0], %[result_0] \n\t" \
                 "cmovzq %[arg1_1], %[result_1] \n\t" \
                 "cmovzq %[arg1_2], %[result_2] \n\t" \
                 "cmovzq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"rm"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3])
# define HURCHALLA_CMOVNS_4_ARG1_TO_RESULT \
                 "cmovnsq %[arg1_0], %[result_0] \n\t" \
                 "cmovnsq %[arg1_1], %[result_1] \n\t" \
                 "cmovnsq %[arg1_2], %[result_2] \n\t" \
                 "cmovnsq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3])
# define HURCHALLA_CMOVS_4_ARG1_TO_RESULT \
                 "cmovsq %[arg1_0], %[result_0] \n\t" \
                 "cmovsq %[arg1_1], %[result_1] \n\t" \
                 "cmovsq %[arg1_2], %[result_2] \n\t" \
                 "cmovsq %[arg1_3], %[result_3] \n\t" \
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3]) \
                 : [value]"r"(value), \
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3])

#endif




// partial specialization for 0 <= BITNUM <= 6
template <int BITNUM>
struct impl_cselect_on_bit<BITNUM, typename std::enable_if<0 <= BITNUM && BITNUM <= 6>::type> {

    static constexpr uint64_t mask = static_cast<uint64_t>(1) << BITNUM;

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVZ_1_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVZ_2_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVZ_3_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVZ_4_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVNZ_1_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVNZ_2_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVNZ_3_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testb %[mask], %b[value] \n\t"
                 HURCHALLA_CMOVNZ_4_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
};


// partial specialization for  8 <= BITNUM <= 30  excluding BITNUM 15
template <int BITNUM>
struct impl_cselect_on_bit<BITNUM, typename std::enable_if<BITNUM != 15 && 8 <= BITNUM && BITNUM <= 30>::type> {

    static constexpr uint64_t mask = static_cast<uint64_t>(1) << BITNUM;

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVZ_1_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVZ_2_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVZ_3_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVZ_4_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVNZ_1_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVNZ_2_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVNZ_3_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testl %[mask], %k[value] \n\t"
                 HURCHALLA_CMOVNZ_4_ARG1_TO_RESULT,
                 [mask]"i"(mask)
                 : "cc");
        return result;
    }
};



// partial specialization for BITNUM == 7
template <>
struct impl_cselect_on_bit<7, void> {

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVNS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVNS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVNS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVNS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testb %b[value], %b[value] \n\t"
                 HURCHALLA_CMOVS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
};


// partial specialization for BITNUM == 15
template <>
struct impl_cselect_on_bit<15, void> {

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVNS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVNS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVNS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVNS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testw %w[value], %w[value] \n\t"
                 HURCHALLA_CMOVS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
};


// partial specialization for BITNUM == 31
template <>
struct impl_cselect_on_bit<31, void> {

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVNS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVNS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVNS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVNS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testl %k[value], %k[value] \n\t"
                 HURCHALLA_CMOVS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
};


// partial specialization for BITNUM == 63
template <>
struct impl_cselect_on_bit<63, void> {

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVNS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVNS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVNS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVNS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVS_1_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVS_2_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVS_3_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("testq %q[value], %q[value] \n\t"
                 HURCHALLA_CMOVS_4_ARG1_TO_RESULT
                 : "cc");
        return result;
    }
};



// partial specialization for 32 <= BITNUM <= 62
template <int BITNUM>
struct impl_cselect_on_bit<BITNUM, typename std::enable_if<32 <= BITNUM && BITNUM <= 62>::type> {

    static constexpr uint8_t bitnum = static_cast<uint8_t>(BITNUM);

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovncq %[arg1_0], %[result_0] \n\t"
                 : [result_0]"+r"(result[0])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]),
#else
                   [arg1_0]"rm"(arg1[0]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovncq %[arg1_0], %[result_0] \n\t"
                 "cmovncq %[arg1_1], %[result_1] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovncq %[arg1_0], %[result_0] \n\t"
                 "cmovncq %[arg1_1], %[result_1] \n\t"
                 "cmovncq %[arg1_2], %[result_2] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovncq %[arg1_0], %[result_0] \n\t"
                 "cmovncq %[arg1_1], %[result_1] \n\t"
                 "cmovncq %[arg1_2], %[result_2] \n\t"
                 "cmovncq %[arg1_3], %[result_3] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovcq %[arg1_0], %[result_0] \n\t"
                 : [result_0]"+r"(result[0])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]),
#else
                   [arg1_0]"rm"(arg1[0]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovcq %[arg1_0], %[result_0] \n\t"
                 "cmovcq %[arg1_1], %[result_1] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovcq %[arg1_0], %[result_0] \n\t"
                 "cmovcq %[arg1_1], %[result_1] \n\t"
                 "cmovcq %[arg1_2], %[result_2] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("btq %[bitnum], %q[value] \n\t"
                 "cmovcq %[arg1_0], %[result_0] \n\t"
                 "cmovcq %[arg1_1], %[result_1] \n\t"
                 "cmovcq %[arg1_2], %[result_2] \n\t"
                 "cmovcq %[arg1_3], %[result_3] \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
#if defined(__clang__)   // https://github.com/llvm/llvm-project/issues/20571
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3]),
#else
                   [arg1_0]"rm"(arg1[0]), [arg1_1]"rm"(arg1[1]), [arg1_2]"rm"(arg1[2]), [arg1_3]"rm"(arg1[3]),
#endif
                   [bitnum]"J"(bitnum)
                 : "cc");
        return result;
    }
};






  // end of #if defined(HURCHALLA_TARGET_ISA_X86_64) && !defined(_MSC_VER) ...
#elif defined(HURCHALLA_TARGET_ISA_ARM_64) && !defined(_MSC_VER) && \
    (defined(HURCHALLA_ALLOW_INLINE_ASM_ALL) || defined(HURCHALLA_ALLOW_INLINE_ASM_CSELECT_ON_BIT))

// --------------------------------- ARM64 -------------------------------------





template <int BITNUM, class Enable = void>
struct impl_cselect_on_bit {
// Normally we want #if 0, so that this class is empty of all functions - this
// will catch bad calls by a client and produce a compilation error.
// This #if clause remains as runnable documentation of what all specializations
// do.  Possibly, using #if 1 might be useful for testing.
#if 0
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? arg1[i] : arg2[i];
        }
        return result;
    }

    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? arg1[i] : arg2[i];
        }
        return result;
    }
#endif
};


// specialization for BITNUM == 31
template <>
struct impl_cselect_on_bit<31, void> {
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], pl \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], pl \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], mi \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("cmp %w[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], mi \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }
};


// specialization for BITNUM == 63
template <>
struct impl_cselect_on_bit<63, void> {
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], pl \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], pl \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], pl \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], pl \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], pl \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], mi \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("cmp %[value], #0 \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], mi \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], mi \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], mi \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], mi \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }
};


// partial specialization for  0 <= BITNUM <= 62  excluding BITNUM 31 (and 63)
template <int BITNUM>
struct impl_cselect_on_bit<BITNUM, typename std::enable_if<BITNUM != 31 && 0 <= BITNUM && BITNUM <= 62>::type> {

    static constexpr uint64_t mask = static_cast<uint64_t>(1) << BITNUM;

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    eq_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], eq \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    eq_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], eq \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], eq \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    eq_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], eq \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], eq \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], eq \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    eq_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], eq \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], eq \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], eq \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], eq \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }

    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 1>
    ne_0(uint64_t value, std::array<uint64_t, 1> arg1, std::array<uint64_t, 1> arg2)
    {
        std::array<uint64_t, 1> result;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[arg2_0], ne \n\t"
                 : [result_0]"=r"(result[0])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]),
                   [arg2_0]"r"(arg2[0])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 2>
    ne_0(uint64_t value, std::array<uint64_t, 2> arg1, std::array<uint64_t, 2> arg2)
    {
        std::array<uint64_t, 2> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], ne \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], ne \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+r"(result[1])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 3>
    ne_0(uint64_t value, std::array<uint64_t, 3> arg1, std::array<uint64_t, 3> arg2)
    {
        std::array<uint64_t, 3> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], ne \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], ne \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], ne \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+r"(result[2])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2])
                 : "cc");
        return result;
    }
    static HURCHALLA_FORCE_INLINE std::array<uint64_t, 4>
    ne_0(uint64_t value, std::array<uint64_t, 4> arg1, std::array<uint64_t, 4> arg2)
    {
        std::array<uint64_t, 4> result = arg2;
        __asm__ ("tst %[value], %[mask] \n\t"
                 "csel %[result_0], %[arg1_0], %[result_0], ne \n\t"
                 "csel %[result_1], %[arg1_1], %[result_1], ne \n\t"
                 "csel %[result_2], %[arg1_2], %[result_2], ne \n\t"
                 "csel %[result_3], %[arg1_3], %[result_3], ne \n\t"
                 : [result_0]"+&r"(result[0]), [result_1]"+&r"(result[1]), [result_2]"+&r"(result[2]), [result_3]"+r"(result[3])
                 : [value]"r"(value), [mask]"i"(mask),
                   [arg1_0]"r"(arg1[0]), [arg1_1]"r"(arg1[1]), [arg1_2]"r"(arg1[2]), [arg1_3]"r"(arg1[3])
                 : "cc");
        return result;
    }
};





  // end of #elif defined(HURCHALLA_TARGET_ISA_ARM_64) && !defined(_MSC_VER) ...
#else

// ------------------------------- NO ASM --------------------------------------



template <int BITNUM, class Enable = void>
struct impl_cselect_on_bit {
    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    eq_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) == 0) ? arg1[i] : arg2[i];
        }
        return result;
    }

    template <size_t ARRAY_SIZE>
    static HURCHALLA_FORCE_INLINE std::array<uint64_t,ARRAY_SIZE>
    ne_0(uint64_t value, std::array<uint64_t,ARRAY_SIZE> arg1, std::array<uint64_t,ARRAY_SIZE> arg2)
    {
        std::array<uint64_t,ARRAY_SIZE> result;
        HURCHALLA_REQUEST_UNROLL_LOOP for (size_t i=0; i<ARRAY_SIZE; ++i) {
            result[i] = ((value & (static_cast<uint64_t>(1) << BITNUM)) != 0) ? arg1[i] : arg2[i];
        }
        return result;
    }
};


#endif
// -----------------------------------------------------------------------------



// impl_cselect_scalar_on_bit is for ALL configurations (asm and not, for all
// platforms)


template <int BITNUM>
struct impl_cselect_scalar_on_bit {

    // for T <= 32 bit
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits <= 32), T>::type
    eq_0(uint64_t value, T arg1, T arg2)
    {
        // on a 16/32 bit machine that we don't have any inline asm for, we'd
        // prefer not to take a (very small) risk that the compiler might create
        // slower code from casting to/from uint64_t, just to eventually do a
        // ternary operation.  Hence we check HURCHALLA_TARGET_BIT_WIDTH
#if HURCHALLA_TARGET_BIT_WIDTH >= 64
        std::array<uint64_t,1> ret = impl_cselect_on_bit<BITNUM>::eq_0(value,
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg1)},
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg2)});
        return static_cast<T>(ret[0]);
#else
        return ((value & (static_cast<uint64_t>(1)<<BITNUM)) == 0) ? arg1 : arg2;
#endif
    }
    // for 64 bit T
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits > 32) &&
                            (ut_numeric_limits<T>::digits <= 64), T>::type
    eq_0(uint64_t value, T arg1, T arg2)
    {
        std::array<uint64_t,1> ret = impl_cselect_on_bit<BITNUM>::eq_0(value,
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg1)},
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg2)});
        return static_cast<T>(ret[0]);
    }
    // for 128 bit T
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits > 64) &&
                            (ut_numeric_limits<T>::digits <= 128), T>::type
    eq_0(uint64_t value, T arg1, T arg2)
    {
        using U = typename ::hurchalla::extensible_make_unsigned<T>::type;
        U u1 = static_cast<U>(arg1);
        U u2 = static_cast<U>(arg2);
        std::array<uint64_t, 2> arr1 =
             { static_cast<uint64_t>(u1), static_cast<uint64_t>(u1 >> 64) };
        std::array<uint64_t, 2> arr2 =
             { static_cast<uint64_t>(u2), static_cast<uint64_t>(u2 >> 64) };
        std::array<uint64_t, 2> sel = impl_cselect_on_bit<BITNUM>::eq_0(value, arr1, arr2);
        U selection = (static_cast<U>(sel[1]) << 64) | static_cast<U>(sel[0]);
        return static_cast<T>(selection);
    }

    // for T <= 32 bit
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits <= 32), T>::type
    ne_0(uint64_t value, T arg1, T arg2)
    {
        // on a 16/32 bit machine that we don't have any inline asm for, we'd
        // prefer not to take a (very small) risk that the compiler might create
        // slower code from casting to/from uint64_t, just to eventually do a
        // ternary operation.  Hence we check HURCHALLA_TARGET_BIT_WIDTH
#if HURCHALLA_TARGET_BIT_WIDTH >= 64
        std::array<uint64_t,1> ret = impl_cselect_on_bit<BITNUM>::ne_0(value,
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg1)},
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg2)});
        return static_cast<T>(ret[0]);
#else
        return ((value & (static_cast<uint64_t>(1)<<BITNUM)) != 0) ? arg1 : arg2;
#endif
    }
    // for 64 bit T
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits > 32) &&
                            (ut_numeric_limits<T>::digits <= 64), T>::type
    ne_0(uint64_t value, T arg1, T arg2)
    {
        std::array<uint64_t,1> ret = impl_cselect_on_bit<BITNUM>::ne_0(value,
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg1)},
                          std::array<uint64_t,1>{static_cast<uint64_t>(arg2)});
        return static_cast<T>(ret[0]);
    }
    // for 128 bit T
    template <typename T>
    static HURCHALLA_FORCE_INLINE
    typename std::enable_if<(ut_numeric_limits<T>::digits > 64) &&
                            (ut_numeric_limits<T>::digits <= 128), T>::type
    ne_0(uint64_t value, T arg1, T arg2)
    {
        using U = typename ::hurchalla::extensible_make_unsigned<T>::type;
        U u1 = static_cast<U>(arg1);
        U u2 = static_cast<U>(arg2);
        std::array<uint64_t, 2> arr1 =
             { static_cast<uint64_t>(u1), static_cast<uint64_t>(u1 >> 64) };
        std::array<uint64_t, 2> arr2 =
             { static_cast<uint64_t>(u2), static_cast<uint64_t>(u2 >> 64) };
        std::array<uint64_t, 2> sel = impl_cselect_on_bit<BITNUM>::ne_0(value, arr1, arr2);
        U selection = (static_cast<U>(sel[1]) << 64) | static_cast<U>(sel[0]);
        return static_cast<T>(selection);
    }
};


}}  // end namespace

#endif
