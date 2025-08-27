// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of this repository ---

// we need to test programming_by_contract with asserts disabled
#ifdef HURCHALLA_UTIL_ENABLE_ASSERTS
#  undef HURCHALLA_UTIL_ENABLE_ASSERTS
#endif

#include "hurchalla/util/detail/util_programming_by_contract.h"

#include "gtest/gtest.h"

#include <type_traits>


// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro
#ifndef HPBC_UTIL_EXPECT_NO_EXIT
    #define HPBC_UTIL_EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); \
           } while(0)
#endif


// confirm that the non-constexpr contract-asserts have zero overhead when
// HURCHALLA_UTIL_ENABLE_ASSERTS is not defined
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_PRECONDITION3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_POSTCONDITION3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_ASSERT3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_UTIL_INVARIANT3(false)), void>::value, "");


namespace {
    // since all the non-constexpr programming by contract asserts map to void
    // when HURCHALLA_UTIL_ENABLE_ASSERTS is undefined, we expect they will
    // never exit/abort when HURCHALLA_UTIL_ENABLE_ASSERTS is undefined.
    TEST(HpbcInactiveTest, GeneralAsserts) {
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_API_PRECONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_API_PRECONDITION(true));

        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(false));

        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(true));
    }
}
