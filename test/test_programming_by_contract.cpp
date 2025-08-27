// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of this repository ---

// needed to test enabled programming_by_contract
#ifndef HURCHALLA_UTIL_ENABLE_ASSERTS
#  define HURCHALLA_UTIL_ENABLE_ASSERTS 1
#endif

#include "hurchalla/util/detail/util_programming_by_contract.h"

#include "gtest/gtest.h"

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif


#if defined(__cpp_lib_optional) || __cplusplus >= 201703L || \
   (defined(_HAS_CXX17) && _HAS_CXX17 > 0)
#   include <optional>
#   define STD_OPTIONAL_IS_AVAILABLE 1
#endif


// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro.
// This seems to be acceptable in the context of these tests though.
#ifndef HPBC_UTIL_EXPECT_NO_EXIT
    #define HPBC_UTIL_EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); \
           } while(0)
#endif

#define HPBC_UTIL_EXPECT_EXIT(EXPRESSION) do \
    { EXPECT_EXIT(EXPRESSION, [](int){return true;}, ""); \
    } while(0)


namespace {
    TEST(HPBCTest, API_Preconditions) {
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_API_PRECONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_API_PRECONDITION(true));
    }


    TEST(HPBCTest, GeneralAsserts) {
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT2(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(true));

#if HURCHALLA_UTIL_ASSERT_LEVEL == 3
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT2(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION2(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION2(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT2(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT3(false));
#elif HURCHALLA_UTIL_ASSERT_LEVEL == 2
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(false));
#elif HURCHALLA_UTIL_ASSERT_LEVEL == 1
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_ASSERT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_PRECONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_INVARIANT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(false));
#elif HURCHALLA_UTIL_ASSERT_LEVEL == 0
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_ASSERT3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_PRECONDITION3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION3(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT2(false));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_INVARIANT3(false));
#else
#  error "unknown value for HURCHALLA_UTIL_ASSERT_LEVEL"
#endif
    }


    constexpr int test_constexpr_asserts()
    {
        // We use the comma operator to form a single statement here, since
        // C++11 needs to have essentially nothing but a return statement in
        // constexpr functions.
        // For C++14 and beyond, you could just write the function normally,
        // with the asserts on their own semicolon delimited lines, prior to a
        // separate return statement.
        return HPBC_UTIL_CONSTEXPR_ASSERT(true),
            HPBC_UTIL_CONSTEXPR_PRECONDITION(true),
            HPBC_UTIL_CONSTEXPR_POSTCONDITION(true),
            HPBC_UTIL_CONSTEXPR_INVARIANT(true),
        #if 0
            // Unfortunately I'm not aware of any way we can automate tests that
            // should show that code doesn't compile, but the following code by
            // design should fail to compile.
            HPBC_UTIL_CONSTEXPR_ASSERT(0 > 1),
            HPBC_UTIL_CONSTEXPR_PRECONDITION(false),
            HPBC_UTIL_CONSTEXPR_POSTCONDITION(false),
            HPBC_UTIL_CONSTEXPR_INVARIANT(false),
        #endif
            0;
    }

    TEST(HPBCTest, ConstexprAsserts) {
        // this statement should be evaluated at compile-time
        constexpr int x = test_constexpr_asserts();
        (void)x;
        // the following statements should be evaluated at run-time
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_CONSTEXPR_ASSERT(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_CONSTEXPR_PRECONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_CONSTEXPR_POSTCONDITION(true));
        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_CONSTEXPR_INVARIANT(true));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_CONSTEXPR_ASSERT(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_CONSTEXPR_PRECONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_CONSTEXPR_POSTCONDITION(false));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_CONSTEXPR_INVARIANT(false));
    }


#ifdef STD_OPTIONAL_IS_AVAILABLE
    TEST(HPBCTest, PostconditionCompareOriginalValue) {
        // We'll normally want to use std::optional (requires C++17)
        // or boost::optional (pre C++17):
        std::optional<std::string> origVal;
        if (HPBC_UTIL_POSTCONDITION_MACRO_IS_ACTIVE)
            origVal = "A) This is a starting value for the string right now";

        //... preconditions ...
        //... function body ...
        std::string endingVal("B) This is an ending value for the string");
        //... more function body ...

        HPBC_UTIL_EXPECT_NO_EXIT(HPBC_UTIL_POSTCONDITION(origVal.has_value()
                                     && origVal < endingVal));
        HPBC_UTIL_EXPECT_EXIT(HPBC_UTIL_POSTCONDITION(!(origVal.has_value()
                                     && origVal < endingVal)));
    }
#endif

}  // end namespace


#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
