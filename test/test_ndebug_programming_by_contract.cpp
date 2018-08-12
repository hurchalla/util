/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */


// needed to test programming_by_contract with NDEBUG!
#ifndef NDEBUG
#  define NDEBUG 1
#endif


#include "hurchalla/programming_by_contract/programming_by_contract.h"
#include "testhelper_assert_handler.h"
#include "hurchalla/programming_by_contract/assert_handler.h"

#include "gtest/gtest.h"

#include <type_traits>


// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro
#ifndef PBC_EXPECT_NO_EXIT
    #define PBC_EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); \
           } while(0)
#endif



// confirm the contract-asserts have zero overhead when NDEBUG is defined
static_assert(std::is_same<decltype(precondition(true)), void>::value, "");
static_assert(std::is_same<decltype(precondition2(true)), void>::value, "");
static_assert(std::is_same<decltype(precondition3(true)), void>::value, "");
static_assert(std::is_same<decltype(postcondition(true)), void>::value, "");
static_assert(std::is_same<decltype(postcondition2(true)), void>::value, "");
static_assert(std::is_same<decltype(postcondition3(true)), void>::value, "");
static_assert(std::is_same<decltype(assert_body(true)), void>::value, "");
static_assert(std::is_same<decltype(assert_body2(true)), void>::value, "");
static_assert(std::is_same<decltype(assert_body3(true)), void>::value, "");
static_assert(std::is_same<decltype(invariant(true)), void>::value, "");
static_assert(std::is_same<decltype(invariant2(true)), void>::value, "");
static_assert(std::is_same<decltype(invariant3(true)), void>::value, "");
static_assert(std::is_same<decltype(precondition(false)), void>::value, "");
static_assert(std::is_same<decltype(precondition2(false)), void>::value, "");
static_assert(std::is_same<decltype(precondition3(false)), void>::value, "");
static_assert(std::is_same<decltype(postcondition(false)), void>::value, "");
static_assert(std::is_same<decltype(postcondition2(false)), void>::value, "");
static_assert(std::is_same<decltype(postcondition3(false)), void>::value, "");
static_assert(std::is_same<decltype(assert_body(false)), void>::value, "");
static_assert(std::is_same<decltype(assert_body2(false)), void>::value, "");
static_assert(std::is_same<decltype(assert_body3(false)), void>::value, "");
static_assert(std::is_same<decltype(invariant(false)), void>::value, "");
static_assert(std::is_same<decltype(invariant2(false)), void>::value, "");
static_assert(std::is_same<decltype(invariant3(false)), void>::value, "");




namespace {
    // since all the programming by contract asserts map to void when NDEBUG
    // is defined, we expect they will never exit/abort when NDEBUG is defined.
    TEST(PbcNdebugTest, PreconditionsHandlerLevel3) {
        setTestHandlerPreconditionAssertLevel(3);
        PBC_EXPECT_NO_EXIT(precondition(true));
        PBC_EXPECT_NO_EXIT(precondition2(true));
        PBC_EXPECT_NO_EXIT(precondition3(true));
        PBC_EXPECT_NO_EXIT(precondition(false));
        PBC_EXPECT_NO_EXIT(precondition2(false));
        PBC_EXPECT_NO_EXIT(precondition3(false));
    }
    TEST(PbcNdebugTest, PreconditionsHandlerLevel0) {
        setTestHandlerPreconditionAssertLevel(0);
        PBC_EXPECT_NO_EXIT(precondition(true));
        PBC_EXPECT_NO_EXIT(precondition2(true));
        PBC_EXPECT_NO_EXIT(precondition3(true));
        PBC_EXPECT_NO_EXIT(precondition(false));
        PBC_EXPECT_NO_EXIT(precondition2(false));
        PBC_EXPECT_NO_EXIT(precondition3(false));
    }
    TEST(PbcNdebugTest, GeneralAssertsHandlerLevel3) {
        setTestHandlerAssertLevel(3);
        PBC_EXPECT_NO_EXIT(assert_body(false));
        PBC_EXPECT_NO_EXIT(assert_body2(false));
        PBC_EXPECT_NO_EXIT(assert_body3(false));
        PBC_EXPECT_NO_EXIT(postcondition(false));
        PBC_EXPECT_NO_EXIT(postcondition2(false));
        PBC_EXPECT_NO_EXIT(postcondition3(false));
        PBC_EXPECT_NO_EXIT(invariant(false));
        PBC_EXPECT_NO_EXIT(invariant2(false));
        PBC_EXPECT_NO_EXIT(invariant3(false));
        PBC_EXPECT_NO_EXIT(assert_body(true));
        PBC_EXPECT_NO_EXIT(assert_body2(true));
        PBC_EXPECT_NO_EXIT(assert_body3(true));
        PBC_EXPECT_NO_EXIT(postcondition(true));
        PBC_EXPECT_NO_EXIT(postcondition2(true));
        PBC_EXPECT_NO_EXIT(postcondition3(true));
        PBC_EXPECT_NO_EXIT(invariant(true));
        PBC_EXPECT_NO_EXIT(invariant2(true));
        PBC_EXPECT_NO_EXIT(invariant3(true));
    }
    TEST(PbcNdebugTest, GeneralAssertsHandlerLevel0) {
        setTestHandlerAssertLevel(0);
        PBC_EXPECT_NO_EXIT(assert_body(false));
        PBC_EXPECT_NO_EXIT(assert_body2(false));
        PBC_EXPECT_NO_EXIT(assert_body3(false));
        PBC_EXPECT_NO_EXIT(postcondition(false));
        PBC_EXPECT_NO_EXIT(postcondition2(false));
        PBC_EXPECT_NO_EXIT(postcondition3(false));
        PBC_EXPECT_NO_EXIT(invariant(false));
        PBC_EXPECT_NO_EXIT(invariant2(false));
        PBC_EXPECT_NO_EXIT(invariant3(false));
        PBC_EXPECT_NO_EXIT(assert_body(true));
        PBC_EXPECT_NO_EXIT(assert_body2(true));
        PBC_EXPECT_NO_EXIT(assert_body3(true));
        PBC_EXPECT_NO_EXIT(postcondition(true));
        PBC_EXPECT_NO_EXIT(postcondition2(true));
        PBC_EXPECT_NO_EXIT(postcondition3(true));
        PBC_EXPECT_NO_EXIT(invariant(true));
        PBC_EXPECT_NO_EXIT(invariant2(true));
        PBC_EXPECT_NO_EXIT(invariant3(true));
    }
}
