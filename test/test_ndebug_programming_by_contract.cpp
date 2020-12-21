// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of this repository ---

// needed to test programming_by_contract with NDEBUG!
#ifndef NDEBUG
#  define NDEBUG 1
#endif

#include "hurchalla/util/programming_by_contract.h"
#include "testhelper_assert_handler.h"
#include "hurchalla/util/assert_handler.h"

#include "gtest/gtest.h"

#include <type_traits>


// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro
#ifndef HPBC_EXPECT_NO_EXIT
    #define HPBC_EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); \
           } while(0)
#endif


// confirm the contract-asserts have zero overhead when NDEBUG is defined
static_assert(std::is_same<decltype(HPBC_PRECONDITION(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_PRECONDITION2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_PRECONDITION3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT2(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT3(true)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_PRECONDITION(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_PRECONDITION2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_PRECONDITION3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_POSTCONDITION3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_ASSERT3(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT2(false)), void>::value, "");
static_assert(std::is_same<decltype(HPBC_INVARIANT3(false)), void>::value, "");


namespace {
    // since all the programming by contract asserts map to void when NDEBUG
    // is defined, we expect they will never exit/abort when NDEBUG is defined.
    TEST(HpbcNdebugTest, PreconditionsHandlerLevel3) {
        setTestHandlerPreconditionAssertLevel(3);
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(false));
    }
    TEST(HpbcNdebugTest, PreconditionsHandlerLevel0) {
        setTestHandlerPreconditionAssertLevel(0);
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(false));
    }
    TEST(HpbcNdebugTest, GeneralAssertsHandlerLevel3) {
        setTestHandlerAssertLevel(3);
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT(true));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT3(true));
    }
    TEST(HpbcNdebugTest, GeneralAssertsHandlerLevel0) {
        setTestHandlerAssertLevel(0);
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT3(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT(true));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_INVARIANT3(true));
    }
}
