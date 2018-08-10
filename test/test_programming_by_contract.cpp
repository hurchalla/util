/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */


// needed to test programming_by_contract without NDEBUG!
#ifdef NDEBUG
#  undef NDEBUG
#endif


#include "hurchalla/programming_by_contract/programming_by_contract.h"
#include "testhelper_assert_handler.h"
#include "hurchalla/programming_by_contract/assert_handler.h"

#include "gtest/gtest.h"



#if defined(__cpp_lib_optional) || __cplusplus >= 201703L || (defined(_HAS_CXX17) && _HAS_CXX17 > 0)
#   include <optional>
#   define STD_OPTIONAL_IS_AVAILABLE 1
#endif



// This macro doesn't strictly test that the expression does not exit -
// If the expression exits with a value of 0 it will pass this macro.
// This seems to be acceptable in the context of these tests though.
#ifndef EXPECT_NO_EXIT
    #define EXPECT_NO_EXIT(EXPRESSION) do \
           { EXPECT_EXIT(EXPRESSION; exit(0), testing::ExitedWithCode(0), ""); } while(0)
#endif



namespace {
	TEST(PBCTest, PreconditionsHandlerLevel3) {
        setTestHandlerPreconditionAssertLevel(3);
		EXPECT_EXIT(precondition(false), [](int){return true;}, "");
		EXPECT_EXIT(precondition2(false), [](int){return true;}, "");
		EXPECT_EXIT(precondition3(false), [](int){return true;}, "");

        EXPECT_NO_EXIT(precondition(true));
        EXPECT_NO_EXIT(precondition2(true));
        EXPECT_NO_EXIT(precondition3(true));
	}

#ifndef PBC_WRAP_STDLIB_ASSERT
	TEST(PBCTest, PreconditionsHandlerLevel2) {
        setTestHandlerPreconditionAssertLevel(2);
		EXPECT_EXIT(precondition(false), [](int){return true;}, "");
		EXPECT_EXIT(precondition2(false), [](int){return true;}, "");
        EXPECT_NO_EXIT(precondition3(false));

        EXPECT_NO_EXIT(precondition(true));
        EXPECT_NO_EXIT(precondition2(true));
        EXPECT_NO_EXIT(precondition3(true));
	}

	TEST(PBCTest, PreconditionsHandlerLevel1) {
        setTestHandlerPreconditionAssertLevel(1);
		EXPECT_EXIT(precondition(false), [](int){return true;}, "");
        EXPECT_NO_EXIT(precondition2(false));
        EXPECT_NO_EXIT(precondition3(false));

        EXPECT_NO_EXIT(precondition(true));
        EXPECT_NO_EXIT(precondition2(true));
        EXPECT_NO_EXIT(precondition3(true));
	}

	TEST(PBCTest, PreconditionsHandlerLevel0) {
        setTestHandlerPreconditionAssertLevel(0);
        EXPECT_NO_EXIT(precondition(false));
        EXPECT_NO_EXIT(precondition2(false));
        EXPECT_NO_EXIT(precondition3(false));

        EXPECT_NO_EXIT(precondition(true));
        EXPECT_NO_EXIT(precondition2(true));
        EXPECT_NO_EXIT(precondition3(true));
	}
#endif

	TEST(PBCTest, GeneralAssertsHandlerLevel3) {
        setTestHandlerAssertLevel(3);
		EXPECT_EXIT(assert_body(false), [](int){return true;}, "");
		EXPECT_EXIT(assert_body2(false), [](int){return true;}, "");
		EXPECT_EXIT(assert_body3(false), [](int){return true;}, "");
		EXPECT_EXIT(postcondition(false), [](int){return true;}, "");
		EXPECT_EXIT(postcondition2(false), [](int){return true;}, "");
		EXPECT_EXIT(postcondition3(false), [](int){return true;}, "");
		EXPECT_EXIT(invariant(false), [](int){return true;}, "");
		EXPECT_EXIT(invariant2(false), [](int){return true;}, "");
		EXPECT_EXIT(invariant3(false), [](int){return true;}, "");

        EXPECT_NO_EXIT(assert_body(true));
        EXPECT_NO_EXIT(assert_body2(true));
        EXPECT_NO_EXIT(assert_body3(true));
        EXPECT_NO_EXIT(postcondition(true));
        EXPECT_NO_EXIT(postcondition2(true));
        EXPECT_NO_EXIT(postcondition3(true));
        EXPECT_NO_EXIT(invariant(true));
        EXPECT_NO_EXIT(invariant2(true));
        EXPECT_NO_EXIT(invariant3(true));
	}

#ifndef PBC_WRAP_STDLIB_ASSERT
	TEST(PBCTest, GeneralAssertsHandlerLevel2) {
        setTestHandlerAssertLevel(2);
		EXPECT_EXIT(assert_body(false), [](int){return true;}, "");
		EXPECT_EXIT(assert_body2(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(assert_body3(false));
		EXPECT_EXIT(postcondition(false), [](int){return true;}, "");
		EXPECT_EXIT(postcondition2(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(postcondition3(false));
		EXPECT_EXIT(invariant(false), [](int){return true;}, "");
		EXPECT_EXIT(invariant2(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(invariant3(false));

        EXPECT_NO_EXIT(assert_body(true));
        EXPECT_NO_EXIT(assert_body2(true));
        EXPECT_NO_EXIT(assert_body3(true));
        EXPECT_NO_EXIT(postcondition(true));
        EXPECT_NO_EXIT(postcondition2(true));
        EXPECT_NO_EXIT(postcondition3(true));
        EXPECT_NO_EXIT(invariant(true));
        EXPECT_NO_EXIT(invariant2(true));
        EXPECT_NO_EXIT(invariant3(true));
	}

	TEST(PBCTest, GeneralAssertsHandlerLevel1) {
        setTestHandlerAssertLevel(1);
		EXPECT_EXIT(assert_body(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(assert_body2(false));
		EXPECT_NO_EXIT(assert_body3(false));
		EXPECT_EXIT(postcondition(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(postcondition2(false));
		EXPECT_NO_EXIT(postcondition3(false));
		EXPECT_EXIT(invariant(false), [](int){return true;}, "");
		EXPECT_NO_EXIT(invariant2(false));
		EXPECT_NO_EXIT(invariant3(false));

        EXPECT_NO_EXIT(assert_body(true));
        EXPECT_NO_EXIT(assert_body2(true));
        EXPECT_NO_EXIT(assert_body3(true));
        EXPECT_NO_EXIT(postcondition(true));
        EXPECT_NO_EXIT(postcondition2(true));
        EXPECT_NO_EXIT(postcondition3(true));
        EXPECT_NO_EXIT(invariant(true));
        EXPECT_NO_EXIT(invariant2(true));
        EXPECT_NO_EXIT(invariant3(true));
	}

	TEST(PBCTest, GeneralAssertsHandlerLevel0) {
        setTestHandlerAssertLevel(0);
		EXPECT_NO_EXIT(assert_body(false));
		EXPECT_NO_EXIT(assert_body2(false));
		EXPECT_NO_EXIT(assert_body3(false));
		EXPECT_NO_EXIT(postcondition(false));
		EXPECT_NO_EXIT(postcondition2(false));
		EXPECT_NO_EXIT(postcondition3(false));
		EXPECT_NO_EXIT(invariant(false));
		EXPECT_NO_EXIT(invariant2(false));
		EXPECT_NO_EXIT(invariant3(false));

        EXPECT_NO_EXIT(assert_body(true));
        EXPECT_NO_EXIT(assert_body2(true));
        EXPECT_NO_EXIT(assert_body3(true));
        EXPECT_NO_EXIT(postcondition(true));
        EXPECT_NO_EXIT(postcondition2(true));
        EXPECT_NO_EXIT(postcondition3(true));
        EXPECT_NO_EXIT(invariant(true));
        EXPECT_NO_EXIT(invariant2(true));
        EXPECT_NO_EXIT(invariant3(true));
	}
#endif


#ifdef STD_OPTIONAL_IS_AVAILABLE
	TEST(PBCTest, PostconditionCompareOriginalValue) {
        setTestHandlerAssertLevel(3);

        // We'll normally want to use std::optional (requires C++17)
        // or boost::optional (pre C++17):
        std::optional<std::string> origVal;
        if (POSTCONDITION_MACRO_IS_ACTIVE)
            origVal = "A) This is a starting value for the string right now";

        //... preconditions ...
        //... function body ...
        std::string endingVal("B) This is an ending value for the string");
        //... more function body ...

        EXPECT_NO_EXIT(postcondition(origVal.has_value()
                                     && origVal < endingVal));
        EXPECT_EXIT(postcondition(!(origVal.has_value() && origVal < endingVal))
                    , [](int){return true;}, "");
    }
#endif



    class Parent
    {
    public:
        Parent(bool passContract) : contractWillPass(passContract) {
            // ...a bunch of code...

            //checkClassInvariants();
            PBC_INVARIANTS_CHECK(checkClassInvariants);
        };
        virtual ~Parent() {
            //checkClassInvariants();
            PBC_INVARIANTS_CHECK(checkClassInvariants);

            // ...a bunch of code...
        }
        void foo() {
            precondition2(contractWillPass);
            PBC_INVARIANTS_GUARD(checkClassInvariants);
            //PBC_METHOD_BODY(checkClassInvariants(), fooImpl());
            
            fooImpl();
            
            postcondition2(contractWillPass);
        }
    protected:
        void checkClassInvariants() {
            invariant3(contractWillPass);
            invariant2(contractWillPass);
        }
    private:
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

            PBC_INVARIANTS_CHECK(checkClassInvariants);
        };
        virtual ~Derived() {
            PBC_INVARIANTS_CHECK(checkClassInvariants);

            // ...a bunch of code...
        }
        void foo() {
            precondition(contractWillPass);
            PBC_INVARIANTS_GUARD(checkClassInvariants);
            //PBC_METHOD_BODY(checkClassInvariants(), fooImpl());

            postcondition(contractWillPass);
        }
    protected:
        void checkClassInvariants() {
            Parent::checkClassInvariants();
            invariant2(contractWillPass);
        }
    private:
        void fooImpl() override {
        }

        bool contractWillPass;
    };

/*
	TEST(PBCTest, PbcBodyThrowingHandlerLevel3) {
		EXPECT_THROW(assert_body(false), hurchalla::BrokenContract);
		EXPECT_NO_THROW(assert_body2(false));
	}
*/
    TEST(PBCTest, ParentClass) {
		setTestHandlerPreconditionAssertLevel(3);
		setTestHandlerAssertLevel(3);
        EXPECT_NO_EXIT({Parent a(true);});
        EXPECT_EXIT({Parent a(false);}, [](int){return true;}, "");
        
        {
            Parent a(true);
            EXPECT_NO_EXIT(a.foo(););
        }
    }

    TEST(PBCTest, DerivedClass) {
		setTestHandlerPreconditionAssertLevel(3);
		setTestHandlerAssertLevel(3);
        EXPECT_NO_EXIT({Derived a(true, true);});
        EXPECT_EXIT({Derived a(false, false);}, [](int){return true;}, "");
        EXPECT_EXIT({Derived a(false, true);}, [](int){return true;}, "");
        EXPECT_EXIT({Derived a(true, false);}, [](int){return true;}, "");

        {
            Derived a(true, true);
            EXPECT_NO_EXIT(a.foo(););
        }
    }

}

