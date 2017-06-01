/*
Copyright (c) 2017 by Jeff Hurchalla.  Code covered by the MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED
#define HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED

/*
These are the main assertion macros:
    precondition0(x), precondition1(x), precondition2(x), precondition3(x)
    assert_body0(x), assert_body1(x), assert_body2(x), assert_body3(x)
    postcondition0(x), postcondition1(x), postcondition2(x), postcondition3(x)
    invariant0(x), invariant1(x), invariant2(x), invariant3(x)

The precondition asserts should be used to check that a precondition is
  satisfied.
The postcondition asserts should be used to check that a postcondition is
  satisfied.
The invariant asserts should be used to check that invariants are true.
The body asserts should be used to check that logic internal to a function is
  satisfied.

The number at the end of the assertion macro name specifies the assertion level:
    A level 0 assert is a convenience macro mapped to static_assert, and hence
      will always be active at compile time.
    A level 1 assert can be used either as a catch-all assert when you don't
      want to specify assert levels; or as more strictly intended, for checks
      that are either extremely low cost, or absolutely critical regardless of
      cost. [Note that if you are able to perform a check at compile time, you
      should always prefer the level 0 assert - since it maps to static_assert,
      it's free of cost and fails nicely at compile time.] A level 1 assert is a
      no-op when building at a lower assertion level.
    A level 2 assert is the normal assert level. It is a no-op when building at
      a lower assertion level.
    A level 3 assert is for checks that are incredibly expensive, such as a
      check with a larger order of computational complexity than the function
      that contains it. It could be considered a "safe mode" assert. This assert
      is a no-op when building at a lower assertion level.

When you compile any file that uses these assertion macros, there will be a
particular assertion level used for the compilation. This assertion level
determines which asserts are compiled into no-ops and which asserts are compiled
into active run time checks of the assertion, as described above. You can set
the assertion level for the files you compile by predefining a macro for your
C/C++ compiler command options (typically in your Makefile or in your build
project settings) that is called:
    PBC_ASSERT_LEVEL, and setting it to a value of 0, 1, 2, or 3
A value of 0 means all asserts will be disabled, with the exception of the
level0 static_asserts. If you do not predefine PBC_ASSERT_LEVEL, it will be
defined below in this file with a default value. The default value ensures for
debug mode (or more precisely, when NDEBUG is not defined) that all asserts
level two or below are active, and it ensures for release mode (when NDEBUG is
defined) that no asserts are active except for level0 static asserts.

You can optionally set a specific level for precondition asserts by predefining
a macro called:
    PBC_ASSERT_LEVEL_PRE, and setting it to a value of 0, 1, 2, or 3
PBC_ASSERT_LEVEL_PRE (if set) overrides PBC_ASSERT_LEVEL for precondition
asserts. Preconditions have a dedicated assert level because they detect
programming errors (usually) on the caller's side, whereas all other asserts
detect programming errors (usually) on the callee's side.


 -- You can change any of the default macros here. --
(However if you want project-specific macro settings, it's better to predefine
them in your compiler command options)
Note that NDEBUG is used *only* for determining default settings; NDEBUG will
not disable any asserts when a programmer has predefined PBC_ASSERT_LEVEL, and
will not disable precondition asserts when a programmer has predefined
PBC_ASSERT_LEVEL_PRE.
For a given compiler, NDEBUG is usually defined for release builds and undefined
for debug builds, but regardless, it's a standard macro (see assert() docs).
*/
#if defined(NDEBUG)
#   define DEFAULT_ASSERT_LEVEL            0
#   define DEFAULT_ASSERT_LEVEL_PRE        0
#   define DEFAULT_NEGATIVE_TESTING_IS_ENABLED 0
#else
#   define DEFAULT_ASSERT_LEVEL            2
#   define DEFAULT_ASSERT_LEVEL_PRE        2
#   define DEFAULT_NEGATIVE_TESTING_IS_ENABLED 1
#endif

/* (C++ only) The above defaults include a default for whether negative testing
is enabled (1 for true, 0 for false).  If the default macro setting above isn't 
what you want, usually you should predefine PBC_NEGATIVE_TESTING_IS_ENABLED
to your desired value in your compiler project settings. Optionally, you could
instead change the default macro setting above. The negative testing assertion
macro is IS_PRECONDITION_VIOLATION(x) (used within an if statement) but note
that it only really makes sense to use this assertion in unit testing (thus a
unit testing project should define PBC_NEGATIVE_TESTING_IS_ENABLED to 1 in
compiler settings.)
*/

/*
Some ideas in this file are inspired by
 https://www.youtube.com/watch?v=1QhtXRMp3Hg
 http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
*/



/* ------------------------------------------------------------------------ */
/* -- You probably shouldn't change anything in the file below this line -- */
/* ------------------------------------------------------------------------ */

#if defined(PBC_ASSERT_LEVEL) && (1 - PBC_ASSERT_LEVEL - 1 == 2)
#   error "PBC_ASSERT_LEVEL is defined, but with no value or with a negative " \
          "value"
#endif
#if defined(PBC_ASSERT_LEVEL_PRE) && (1 - PBC_ASSERT_LEVEL_PRE - 1 == 2)
#   error "PBC_ASSERT_LEVEL_PRE is defined, but with no value or with a " \
          "negative value"
#endif
#if defined(__cplusplus) && defined(PBC_NEGATIVE_TESTING_IS_ENABLED) && \
            (1 - PBC_NEGATIVE_TESTING_IS_ENABLED - 1 == 2)
#   error "PBC_NEGATIVE_TESTING_IS_ENABLED is defined, but with no value or " \
          "with a negative value"
#endif
#if defined(__cplusplus) && defined(CPP_EXCEPTIONS_ARE_DISABLED) && \
            (1 - CPP_EXCEPTIONS_ARE_DISABLED - 1 == 2)
#   error "CPP_EXCEPTIONS_ARE_DISABLED is defined, but with no value or with " \
          "a negative value"
#endif


#if defined(__cplusplus) && !defined(PBC_NEGATIVE_TESTING_IS_ENABLED)
#   define PBC_NEGATIVE_TESTING_IS_ENABLED  DEFAULT_NEGATIVE_TESTING_IS_ENABLED
#endif

#if defined(PBC_ASSERT_LEVEL)
#   if !defined(PBC_ASSERT_LEVEL_PRE)
#       define PBC_ASSERT_LEVEL_PRE  PBC_ASSERT_LEVEL
#   endif
#else
#   define PBC_ASSERT_LEVEL  DEFAULT_ASSERT_LEVEL
#   if !defined(PBC_ASSERT_LEVEL_PRE)
#       define PBC_ASSERT_LEVEL_PRE  DEFAULT_ASSERT_LEVEL_PRE
#   endif
#endif

#if ((PBC_ASSERT_LEVEL < 0) || (PBC_ASSERT_LEVEL_PRE < 0))
#   error "Invalid assert level for PBC_ASSERT_LEVEL or PBC_ASSERT_LEVEL_PRE " \
          "macros"
#endif

#if ((PBC_ASSERT_LEVEL == 0) && (PBC_ASSERT_LEVEL_PRE == 0))
#   define PBC_ASSERTS_ARE_ENABLED 0
#else
#   define PBC_ASSERTS_ARE_ENABLED 1
#endif



#if defined(__cplusplus)
#   define precondition0(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define assert_body0(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define postcondition0(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define invariant0(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#else
    /* this assumes C11 support in your compiler.  To emulate static_assert
     pre-C11, see http://stackoverflow.com/questions/3385515/static-assert-in-c
     For visual studio C, see _STATIC_ASSERT */
#   define precondition0(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define assert_body0(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define postcondition0(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#   define invariant0(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#endif



#if !PBC_ASSERTS_ARE_ENABLED
#   define precondition1(...) ((void)0)
#   define precondition2(...) ((void)0)
#   define precondition3(...) ((void)0)
#   define assert_body1(...) ((void)0)
#   define assert_body2(...) ((void)0)
#   define assert_body3(...) ((void)0)
#   define postcondition1(...) ((void)0)
#   define postcondition2(...) ((void)0)
#   define postcondition3(...) ((void)0)
#   define invariant1(...) ((void)0)
#   define invariant2(...) ((void)0)
#   define invariant3(...) ((void)0)
#else
    void pbcAssertHandler(const char* failedAssertion,
                          const char* filename,
                          int line);

#   define BASIC_ASSERT(...) do { if (!(__VA_ARGS__)) { \
            pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); \
            } } while(0)
#   if defined(__cplusplus) && PBC_NEGATIVE_TESTING_IS_ENABLED
#       define BASIC_ASSERT_PRE(...) do { if (!(__VA_ARGS__)) { \
                if (globalPbcNegativeTestingIsActive_) \
                    throw hurchalla::PbcNegativeTestPreconditionViolation(); \
                else \
                    pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); \
                } } while(0)
#   else
#       define BASIC_ASSERT_PRE(...) BASIC_ASSERT(__VA_ARGS__)
#   endif


#   if !defined(PBC_ASSERT_LEVEL_PRE)
#       error "macro is required but undefined"
#   endif
#   if (PBC_ASSERT_LEVEL_PRE >= 1)
#       define precondition1(...) BASIC_ASSERT_PRE(__VA_ARGS__)
#   else
#       define precondition1(...) ((void)0)
#   endif
#   if (PBC_ASSERT_LEVEL_PRE >= 2)
#       define precondition2(...) BASIC_ASSERT_PRE(__VA_ARGS__)
#   else
#       define precondition2(...) ((void)0)
#   endif
#   if (PBC_ASSERT_LEVEL_PRE >= 3)
#       define precondition3(...) BASIC_ASSERT_PRE(__VA_ARGS__)
#   else
#       define precondition3(...) ((void)0)
#   endif

#   if !defined(PBC_ASSERT_LEVEL)
#       error "macro is required but undefined"
#   endif
#   if (PBC_ASSERT_LEVEL >= 1)
#       define assert_body1(...) BASIC_ASSERT(__VA_ARGS__)
#       define postcondition1(...) BASIC_ASSERT(__VA_ARGS__)
#       define invariant1(...) BASIC_ASSERT(__VA_ARGS__)
#   else
#       define assert_body1(...) ((void)0)
#       define postcondition1(...) ((void)0)
#       define invariant1(...) ((void)0)
#   endif
#   if (PBC_ASSERT_LEVEL >= 2)
#       define assert_body2(...) BASIC_ASSERT(__VA_ARGS__)
#       define postcondition2(...) BASIC_ASSERT(__VA_ARGS__)
#       define invariant2(...) BASIC_ASSERT(__VA_ARGS__)
#   else
#       define assert_body2(...) ((void)0)
#       define postcondition2(...) ((void)0)
#       define invariant2(...) ((void)0)
#   endif
#   if (PBC_ASSERT_LEVEL >= 3)
#       define assert_body3(...) BASIC_ASSERT(__VA_ARGS__)
#       define postcondition3(...) BASIC_ASSERT(__VA_ARGS__)
#       define invariant3(...) BASIC_ASSERT(__VA_ARGS__)
#   else
#       define assert_body3(...) ((void)0)
#       define postcondition3(...) ((void)0)
#       define invariant3(...) ((void)0)
#   endif
#endif	/* PBC_ASSERTS_ARE_ENABLED */



/* All code below this line is C++ only */

#if defined(__cplusplus)


// helpers to let us create HEADER_DEFS_PBC_ODR
#define CONCAT_HEADER_DEFS_PBC_ODR(x,y) PBC_ ## x ## y
#define EVALUATE_HEADER_DEFS_PBC_ODR(x,y)  CONCAT_HEADER_DEFS_PBC_ODR(x,y)

// PBC_ODR_NAME_MACRO should be used as part of an inline namespace in order to
// prevent ODR violations in cases where anything using PBC assertions/macros (a
// class, template, or inline function) is defined in a header file, rather than
// simply declared in a header file.  For all such cases, the relevent code
// should be defined inside the PBC_ODR_NAME_MACRO inline namespace.  A function
// that is FORCE_INLINE'd is an exception to this rule, though it may be easiest
// to still use this rule for that case too.  Client code should never directly
// use PBC_ODR_NAME_MACRO when calling or otherwise referring to the code, since
// PBC_ODR_NAME_MACRO is (or should be) used as an inline namespace.
#define PBC_ODR_NAME_MACRO  EVALUATE_HEADER_DEFS_PBC_ODR(PBC_ASSERT_LEVEL_PRE, \
                                                          PBC_ASSERT_LEVEL)


#   if PBC_NEGATIVE_TESTING_IS_ENABLED && PBC_ASSERT_LEVEL_PRE == 0
#       error "PBC_ASSERT_LEVEL_PRE must be defined to a value > 0 when " \
              "PBC_NEGATIVE_TESTING_IS_ENABLED is true"
#   endif
#   if PBC_NEGATIVE_TESTING_IS_ENABLED && CPP_EXCEPTIONS_ARE_DISABLED
#       error "PBC_NEGATIVE_TESTING_IS_ENABLED must not be true (it will be " \
              "unavailable) when CPP_EXCEPTIONS_ARE_DISABLED is true"
#   endif

	// I consider it dangerous to define PBC_HANDLER_WILL_ALWAYS_BE_NOEXCEPT to
    // 1, but if you are absolutely certain (and *correct*) that your code will
    // never be linked with a pbcAssertHandler that can throw, it would be safe
    // to define to 1 and might provide a slight performance advantage while
    // contracts are enabled.  It would be a very bad idea to define
    // PBC_HANDLER_WILL_ALWAYS_BE_NOEXCEPT to 1 if you are writing library code
    // that allows clients to link their own custom pbcAssertHandler, because
    // you can not control whether their pbcAssertHandler throws.  In general,
    // since you are already operating at some performance disadvantage while
    // contracts are enabled, I expect the benefit of optimizing (via this
    // macro) this situation would rarely be worthwhile.

//#   define PBC_HANDLER_WILL_ALWAYS_BE_NOEXCEPT 1

#   if CPP_EXCEPTIONS_ARE_DISABLED || !PBC_ASSERTS_ARE_ENABLED || \
       (!PBC_NEGATIVE_TESTING_IS_ENABLED && PBC_HANDLER_WILL_ALWAYS_BE_NOEXCEPT)
#       define PBC_NOEXCEPT noexcept
#   else
#       define PBC_NOEXCEPT
#   endif


#   if !PBC_NEGATIVE_TESTING_IS_ENABLED
        namespace hurchalla { inline namespace v1 {
            inline bool isPbcPreconditionViolation(void (*func)())
                                                    { func(); return false; }
        }}
#       define IS_PRECONDITION_VIOLATION(EXPRESSION_UNDER_TEST) \
                hurchalla::isPbcPreconditionViolation([]{ static_assert(0, \
                "IS_PRECONDITION_VIOLATION() can not be called unless " \
                "PBC_NEGATIVE_TESTING_IS_ENABLED is defined to be nonzero"); })
#   else
#       include <atomic>
        // global variable to support negative testing - it should not be used
        // outside of programming_by_contract.c/h. It gets temporarily changed
        // during IS_PRECONDITION_VIOLATION() calls.
        extern std::atomic<bool> globalPbcNegativeTestingIsActive_;

        namespace hurchalla { inline namespace v1 {
            // nothing should ever throw PbcNegativeTestPreconditionViolation,
            // other than ASSERTx_PRE() macros
            class PbcNegativeTestPreconditionViolation final {};

            // While negative testing, it's normal for func to throw
            // PbcNegativeTestPreconditionViolation (via ASSERTx_PRE)
            template<typename Lambda>
            inline bool isPbcPreconditionViolation(Lambda func,
                                                   const char* filename,
                                                   int line)
            {
                if (globalPbcNegativeTestingIsActive_.exchange(true)) {
                    // If we get in here, globalPbcNegativeTestingIsActive_ was
                    // already true.
                    pbcAssertHandler("IS_PRECONDITION_VIOLATION misuse: " \
                                     "negative testing was already active.",
                                     filename, line);
                    return false;	// most assert handlers don't return, so we
                                    // won't usually reach this line.
                }
                // When leaving this scope, set
                // globalPbcNegativeTestingIsActive_ to false
                struct NTG {~NTG(){ globalPbcNegativeTestingIsActive_ = false; }
                    } guard;
                bool violationFound = false;
                try {
                    func();
                } catch (const PbcNegativeTestPreconditionViolation&) {
                    violationFound = true;
                }
                return violationFound;
            }
        }}
#       define IS_PRECONDITION_VIOLATION(EXPRESSION_UNDER_TEST) \
                            hurchalla::isPbcPreconditionViolation( \
                                            [&]{ EXPRESSION_UNDER_TEST; }, \
                                            __FILE__, \
                                            __LINE__)
#   endif


#   if PBC_ASSERTS_ARE_ENABLED && !CPP_EXCEPTIONS_ARE_DISABLED
        #include <stdexcept>
        namespace hurchalla { inline namespace v1 {
            class BrokenContract : public std::logic_error {	public:
                explicit BrokenContract(const std::string& what_arg) :
                            std::logic_error(what_arg) {}
                explicit BrokenContract(const char* what_arg) :
                            std::logic_error(what_arg) {}
            };
        }}
#   endif


#   if PBC_ASSERT_LEVEL == 0
#       define PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY) do { \
                EXPRESSION_BODY; \
                } while (0)
#   elif CPP_EXCEPTIONS_ARE_DISABLED
#       define PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY) do { \
                { INVARIANT_EXPRESSION; } \
                { EXPRESSION_BODY; } \
                { INVARIANT_EXPRESSION;	} \
                } while (0)
#   else
        namespace hurchalla { inline namespace v1 {
            template<typename Lambda1, typename Lambda2>
            inline void pbcBody(Lambda1&& funcInvariant, Lambda2&& funcBody)
            {
                funcInvariant(); // I'm not sure if I should check invariants
                // here (before the function body, in addition to after the
                // body).  Possibly, there may be two types of class invariants:
                // ones that can be defined in terms of the interface, and ones
                // that need the private implementation. Two functions could be
                // used in that case, but I suspect one invariant type will
                // almost always be sufficient.
                
                // if funcBody() is noexcept, then I could omit the try/catch
                // scaffolding to improve performance a bit.  However, this
                // likely isn't worth the added complexity of yet another macro
                // to do it, given that invariants are enabled here (probably
                // harming performance much more than try/catch).
                try {
                    funcBody();
                } catch (const hurchalla::BrokenContract&) {    // if a PBC
                    // handler throws an exception, the exception must
                    // be derived from BrokenContract.
                    
                    // Since we already have a broken contract exception, we
                    // want to rethrow immediately to keep the exception info of
                    // where it broke.
                    throw;
                } catch (...) {
                    funcInvariant();	// note: depending on pbcAssertHandler,
                    // funcInvariant() might be able to throw a PBC exception.
                    throw;
                }
                // note: The function try catch mechanism I use above to ensure
                // invariants are maintained is cooincidentally a partial check
                // for basic exception safety (which is that all invariants are
                // preserved and no resources are leaked).  It would be nice to
                // somehow also detect if resources were leaked, but regardless
                // the partial check of exception safety is still good.
                funcInvariant();
            }
        }}
#       define PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY) \
                hurchalla::pbcBody([&]{INVARIANT_EXPRESSION;}, \
                                   [&]{EXPRESSION_BODY;})


        // Alternate, all macro implementation.  Very ugly but probably faster,
        // and also C++ 03 compatible
#       if 0
#           define PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY) do { \
					/* I'm not sure if I should check invariants here (before \
                     the function body, in addition to after the body).  \
                     Possibly, there may be two types of class invariants: \
                     ones that can be defined in terms of the interface, and \
                     ones that need the private implementation. Two functions \
                     could be used in that case, but I suspect one invariant \
                     type will almost always be sufficient. */ \
                    { INVARIANT_EXPRESSION; } \
                    /* if EXPRESSION_BODY is noexcept, then I could omit this \
                     try/catch scaffolding to improve performance a bit.  \
                     However, this likely isn't worth the added complexity of \
                     yet another macro to do it, given that invariants are \
                     enabled here (probably harming performance much more than \
                     try/catch). */ \
                    try { \
                        EXPRESSION_BODY; \
                    } catch (const hurchalla::BrokenContract&) {	/* if a PBC\
                        handler throws an exception, the exception must be \
                        derived from BrokenContract */ \
                        /* If we already have a broken contract exception, we \
                         want to rethrow immediately to keep the exception \
                         info of where it broke */ \
                        throw; \
                    } catch (...) { \
                        INVARIANT_EXPRESSION;	/* note: depending on \
                            pbcAssertHandler, this might throw- effectively \
                            replacing the currently caught exception */ \
                        throw; \
                    } \
                    /* note: The function try catch mechanism I use above to \
                     ensure invariants are maintained is cooincidentally a \
                     partial check of basic exception safety (which is that \
                     all invariants are preserved and no resources are leaked).\
                     It would be nice to detect if resources were leaked, but \
                     a partial check is still good. */ \
                    { INVARIANT_EXPRESSION; } \
                    } while (0)
#       endif
#   endif
#endif	/* defined(__cplusplus) */


#endif  /* HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED */
