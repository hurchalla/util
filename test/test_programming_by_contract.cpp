// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of this repository ---

// needed to test programming_by_contract without NDEBUG!
#ifdef NDEBUG
#  undef NDEBUG
#endif

#include "hurchalla/util/programming_by_contract.h"
#include "testhelper_assert_handler.h"
#include "hurchalla/util/assert_handler.h"

#include "gtest/gtest.h"


#if defined(__cpp_lib_optional) || __cplusplus >= 201703L || \
   (defined(_HAS_CXX17) && _HAS_CXX17 > 0)
#   include <optional>
#   define STD_OPTIONAL_IS_AVAILABLE 1
#endif


// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro.
// This seems to be acceptable in the context of these tests though.
#ifndef HPBC_EXPECT_NO_EXIT
    #define HPBC_EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); \
           } while(0)
#endif

#define HPBC_EXPECT_EXIT(EXPRESSION) do \
    { EXPECT_EXIT(EXPRESSION, [](int){return true;}, ""); \
    } while(0)


namespace {
    TEST(HPBCTest, PreconditionsHandlerLevel3) {
        setTestHandlerPreconditionAssertLevel(3);
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION3(false));

        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
    }

#ifdef HPBC_ENABLE_FULL_FEATURES
    TEST(HPBCTest, PreconditionsHandlerLevel2) {
        setTestHandlerPreconditionAssertLevel(2);
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(false));

        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
    }

    TEST(HPBCTest, PreconditionsHandlerLevel1) {
        setTestHandlerPreconditionAssertLevel(1);
        HPBC_EXPECT_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(false));

        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
    }

    TEST(HPBCTest, PreconditionsHandlerLevel0) {
        setTestHandlerPreconditionAssertLevel(0);
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(false));

        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION2(true));
        HPBC_EXPECT_NO_EXIT(HPBC_PRECONDITION3(true));
    }
#endif

    TEST(HPBCTest, GeneralAssertsHandlerLevel3) {
        setTestHandlerAssertLevel(3);
        HPBC_EXPECT_EXIT(HPBC_ASSERT(false));
        HPBC_EXPECT_EXIT(HPBC_ASSERT2(false));
        HPBC_EXPECT_EXIT(HPBC_ASSERT3(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION2(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION3(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT2(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT3(false));

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

#ifdef HPBC_ENABLE_FULL_FEATURES
    TEST(HPBCTest, GeneralAssertsHandlerLevel2) {
        setTestHandlerAssertLevel(2);
        HPBC_EXPECT_EXIT(HPBC_ASSERT(false));
        HPBC_EXPECT_EXIT(HPBC_ASSERT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT2(false));
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

    TEST(HPBCTest, GeneralAssertsHandlerLevel1) {
        setTestHandlerAssertLevel(1);
        HPBC_EXPECT_EXIT(HPBC_ASSERT(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_ASSERT3(false));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION2(false));
        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION3(false));
        HPBC_EXPECT_EXIT(HPBC_INVARIANT(false));
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

    TEST(HPBCTest, GeneralAssertsHandlerLevel0) {
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
#endif

    constexpr int test_constexpr_asserts()
    {
        // We use the comma operator to form a single statement here, since
        // C++11 needs to have essentially nothing but a return statement in
        // constexpr functions.
        // For C++14 and beyond, you could just write the function normally,
        // with the asserts on their own semicolon delimited lines, prior to a
        // separate return statement.
        return HPBC_CONSTEXPR_ASSERT(true),
            HPBC_CONSTEXPR_PRECONDITION(true),
            HPBC_CONSTEXPR_POSTCONDITION(true),
            HPBC_CONSTEXPR_INVARIANT(true),
        #if 0
            // Unfortunately I'm not aware of any way we can automate tests that
            // should show that code doesn't compile, but the following code by
            // design should fail to compile.
            HPBC_CONSTEXPR_ASSERT(0 > 1),
            HPBC_CONSTEXPR_PRECONDITION(false),
            HPBC_CONSTEXPR_POSTCONDITION(false),
            HPBC_CONSTEXPR_INVARIANT(false),
        #endif
            0;
    }

    TEST(HPBCTest, ConstexprAsserts) {
        // this statement should be evaluated at compile-time
        constexpr int x = test_constexpr_asserts();
        (void)x;
        // the following statements should be evaluated at run-time
        HPBC_EXPECT_NO_EXIT(HPBC_CONSTEXPR_ASSERT(true));
        HPBC_EXPECT_NO_EXIT(HPBC_CONSTEXPR_PRECONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_CONSTEXPR_POSTCONDITION(true));
        HPBC_EXPECT_NO_EXIT(HPBC_CONSTEXPR_INVARIANT(true));
        HPBC_EXPECT_EXIT(HPBC_CONSTEXPR_ASSERT(false));
        HPBC_EXPECT_EXIT(HPBC_CONSTEXPR_PRECONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_CONSTEXPR_POSTCONDITION(false));
        HPBC_EXPECT_EXIT(HPBC_CONSTEXPR_INVARIANT(false));
    }


#ifdef STD_OPTIONAL_IS_AVAILABLE
    TEST(HPBCTest, PostconditionCompareOriginalValue) {
        setTestHandlerAssertLevel(3);

        // We'll normally want to use std::optional (requires C++17)
        // or boost::optional (pre C++17):
        std::optional<std::string> origVal;
        if (HPBC_POSTCONDITION_MACRO_IS_ACTIVE)
            origVal = "A) This is a starting value for the string right now";

        //... preconditions ...
        //... function body ...
        std::string endingVal("B) This is an ending value for the string");
        //... more function body ...

        HPBC_EXPECT_NO_EXIT(HPBC_POSTCONDITION(origVal.has_value()
                                     && origVal < endingVal));
        HPBC_EXPECT_EXIT(HPBC_POSTCONDITION(!(origVal.has_value()
                                     && origVal < endingVal)));
    }
#endif


    class Parent
    {
    public:
        Parent(bool passContract) : contractWillPass(passContract) {
            // ...a bunch of code...

            checkClassInvariantsLocal();
        }
        virtual ~Parent() {
            checkClassInvariantsLocal();

            // ...a bunch of code...
        }
        void foo() {
            HPBC_PRECONDITION2(contractWillPass);
            HPBC_INVARIANTS_GUARD(checkClassInvariants);
            
            fooImpl();
            
            HPBC_POSTCONDITION2(contractWillPass);
        }
    protected:
        void checkClassInvariants() {
            // If we inherited from a class Grandparent, we'd also call here:
            // Grandparent::checkClassInvariants();
            checkClassInvariantsLocal();
        }
    private:
        void checkClassInvariantsLocal() {
            HPBC_INVARIANT3(contractWillPass);
            HPBC_INVARIANT2(contractWillPass);
        }
        virtual void fooImpl() {
        }

        bool contractWillPass;
    };


    class Derived : public Parent
    {
    public:
        Derived(bool passContract, bool passParentContract) :
                                            Parent(passParentContract), 
                                            contractWillPass(passContract) {
            // ...a bunch of code...

            checkClassInvariantsLocal();
        }
        virtual ~Derived() override {
            checkClassInvariantsLocal();

            // ...a bunch of code...
        }
        void foo() {
            HPBC_PRECONDITION(contractWillPass);
            HPBC_INVARIANTS_GUARD(checkClassInvariants);

            fooImpl();

            HPBC_POSTCONDITION(contractWillPass);
        }
    protected:
        void checkClassInvariants() {
            Parent::checkClassInvariants();
            checkClassInvariantsLocal();
        }
    private:
        void checkClassInvariantsLocal() {
            HPBC_INVARIANT2(contractWillPass);
        }
        void fooImpl() override {
        }

        bool contractWillPass;
    };


    TEST(HPBCTest, ParentClass) {
        setTestHandlerPreconditionAssertLevel(3);
        setTestHandlerAssertLevel(3);
        HPBC_EXPECT_NO_EXIT({Parent a(true);});
        HPBC_EXPECT_EXIT({Parent a(false);});
        {
            Parent a(true);
            HPBC_EXPECT_NO_EXIT(a.foo(););
        }
    }

    TEST(HPBCTest, DerivedClass) {
        setTestHandlerPreconditionAssertLevel(3);
        setTestHandlerAssertLevel(3);
        HPBC_EXPECT_NO_EXIT({Derived a(true, true);});
        HPBC_EXPECT_EXIT({Derived a(false, false);});
        HPBC_EXPECT_EXIT({Derived a(false, true);});
        HPBC_EXPECT_EXIT({Derived a(true, false);});
        {
            Derived a(true, true);
            HPBC_EXPECT_NO_EXIT(a.foo(););
        }
    }

}

