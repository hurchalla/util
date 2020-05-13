/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */

#ifndef HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED
#define HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED

/*
These are the main contract assertion macros:
    precondition(x), precondition2(x), precondition3(x)
    postcondition(x), postcondition2(x), postcondition3(x)
    invariant(x), invariant2(x), invariant3(x)
    assert_logic(x), assert_logic2(x), assert_logic3(x)
They are purposely implemented as macros instead of inline functions, as
described at the end of this section.

Precondition asserts are intended to check that a precondition is satisfied.
Postcondition asserts are intended to check that a postcondition is satisfied.
Invariant asserts are intended to check that invariants are true.
Logic asserts are intended to check that logical assumptions are true.

The number at the end of an assertion macro name specifies the assertion level.
The assertion macros without a number at the end (precondition, postcondition,
invariant, assumption) can be viewed as having an implicit level of 1. For
more detail:
  An unnumbered (implicit level 1) assert can be viewed as a "normal" assert
    and is the go-to assert level, useful for when you either don't care about
    levels at all, or for when an assert is unremarkable in the time it will
    take to check while the program is running. [Note that if you are able to
    perform a check at compile time, you should always prefer static_assert()
    in C++ or _Static_assert() in C, since it is always free of cost and fails
    during compilation.]
  A level 2 assert is intended for checks that you know will be unusually
    expensive to perform.
  A level 3 assert is for checks that are incredibly expensive, such as a check
    with a larger order of computational complexity than the function that
    contains it. It could be considered a "safe mode" assert.

When NDEBUG is defined in a translation unit, all of these asserts will be
  replaced with a no-op, regardless of assertion level.
When NDEBUG is undefined, each assert will be replaced with first a call to a
  trivial getter function [pbcGetHandlerAssertLevel()] to determine the program-
  wide assert level. Then, this will be followed by a conditional to skip the
  assert check if your particular assert's level is greater than the program-
  wide assert level. If this occurs and you are using link-time-optimization,
  the linker will typically see that the code will always skip the assert, and
  it will remove the getter call and the conditional and the assert, resulting
  in a no-op. However, the effects of link-time-optimization are implementation
  dependent.

For a given compiler, by default NDEBUG is typically defined in release builds
  and undefined in debug builds. It's a standard macro (see the C or C++
  documentation of assert()) and you can predefine it (or not) for any
  compilation project - for example by using -D in gcc or clang, or in MSVC++
  via either command line option /D or by UI setting "preprocessor definitions".

Note: the assertions are all purposely implemented as macros instead of inline
  functions. Implementing them as functions involves at least two unavoidable
  problems, which is very likely the reason the standard assert() from assert.h
  is also implemented as a macro. First, the assserts use the C/C++ standard
  predefined macros __FILE__ and __LINE_ to show the location of an assertion
  failure. If assertions were implemented as functions, this could not work,
  since __FILE__ and __LINE_ would provide locations in the called functions
  rather than the location of the assert in the caller. Second, the contract
  assertions are designed to guarantee zero overhead when NDEBUG is defined in a
  translation unit. If assertions were implemented as functions, the argument
  given to an assert could break that guarantee. For example consider:
  assert(isGood()). If isGood() has side effects, the compiler would always need
  to call it, even if a hypothetical assert function (instead of macro) had an
  empty body. Even if isGood() has no side effects, it's possible the compiler
  and linker would miss an optimization we would expect to remove the isGood()
  call. Missed optimizations might be unlikely, but the lack of guarantee and
  the side-effects issue would make the use-cases for asserts more complicated.
Assertions implemented as macros do not have these problems. However, macros
  have the problem that they 'pollute' the namespace, and this problem is
  worsened in our case by the fact that this file uses uncapitalized common
  names for its assertion macros- such as 'precondition' (the use of non-caps
  is in keeping with the standard 'assert' macro, with the intention that these
  macros not be overly distracting in code). Consequently, you must treat all
  the assertion macro names as reserved keywords throughout your projects. If
  the given assertion names here are impossible or undesirable for you to use,
  you can edit this file to change the macro names.
*/
/*
Some ideas in this file were inspired by
 https://www.youtube.com/watch?v=1QhtXRMp3Hg
 http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
*/

/*
Ordinarily, you shouldn't change anything in this file.
*/

#if defined(NDEBUG)
#  if 1
     /* PBC_DO_NOTHING() is written to avoid 'unused variable' warnings during
        compilation. Note: for C++ prior to C++20, the line below will not
        compile if the arg contains a lambda. You can use the alternative
        version if necessary. */
#    define PBC_DO_NOTHING(...) ((void)sizeof(__VA_ARGS__))
#  else
     /* Alternative version for C++ pre-20 with lambdas in contract args. */
#    define PBC_DO_NOTHING(...) \
                       do { true ? ((void)0) : ((void)(__VA_ARGS__)); } while(0)
#  endif

#  define precondition(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define precondition2(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define precondition3(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define postcondition(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define postcondition2(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define postcondition3(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define invariant(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define invariant2(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define invariant3(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define assert_logic(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define assert_logic2(...) PBC_DO_NOTHING(__VA_ARGS__)
#  define assert_logic3(...) PBC_DO_NOTHING(__VA_ARGS__)

#  if defined(__cplusplus)
#     define PBC_FALSE_VALUE (false)
#  else
#     define PBC_FALSE_VALUE (0)
#  endif
#  define PRECONDITION_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define PRECONDITION2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define PRECONDITION3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_LOGIC_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_LOGIC2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_LOGIC3_MACRO_IS_ACTIVE PBC_FALSE_VALUE

#else
#  if defined(PBC_WRAP_STDLIB_ASSERT)
#     if defined(__cplusplus)
#        include <cassert>
#     else
#        include <assert.h>
#     endif
#     define pbcGetHandlerAssertLevel() 3
#     define pbcGetHandlerPreconditionAssertLevel() 3
#     define PBC_LEVEL_ASSERT(LEVEL, ...) assert(__VA_ARGS__)
#     define PBC_LEVEL_ASSERT_PRE(LEVEL, ...) assert(__VA_ARGS__)
#  else   /* the normal Programming by Contract features */
#     include "hurchalla/programming_by_contract/assert_handler.h"

      /* If this is C++, we can (probably) detect if exceptions are enabled by
         checking the gcc/clang macro __EXCEPTIONS, msvc's macro _CPPUNWIND, and
         the official but not always supported C++98 macro __cpp_exceptions. */
#     if defined(__cplusplus) && (defined(__EXCEPTIONS) || defined(_CPPUNWIND) \
                        || (defined(__cpp_exceptions) && __cpp_exceptions != 0))
         // C++ with exceptions: treat an exception as a failure during assert
#        define PBC_BASIC_ASSERT(...) do { \
                       bool assertPassed = false; \
                       try { if (__VA_ARGS__) assertPassed = true; } \
                       catch (...) {} \
                       if (!assertPassed) \
                           pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); \
                   } while(0)
#     else   /* C, or C++ without exceptions */
#        define PBC_BASIC_ASSERT(...) do { if (__VA_ARGS__) {} else  { \
                pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); } } while(0)
#     endif

#     define PBC_LEVEL_ASSERT(LEVEL, ...) do { \
                        if (pbcGetHandlerAssertLevel() >= LEVEL) { \
                             PBC_BASIC_ASSERT(__VA_ARGS__); } } while(0)
#     define PBC_LEVEL_ASSERT_PRE(LEVEL, ...) do { \
                        if (pbcGetHandlerPreconditionAssertLevel() >= LEVEL) { \
                             PBC_BASIC_ASSERT(__VA_ARGS__); } } while(0)
#  endif
#  define precondition(...) PBC_LEVEL_ASSERT_PRE(1, __VA_ARGS__)
#  define precondition2(...) PBC_LEVEL_ASSERT_PRE(2, __VA_ARGS__)
#  define precondition3(...) PBC_LEVEL_ASSERT_PRE(3, __VA_ARGS__)
#  define postcondition(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define postcondition2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define postcondition3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)
#  define invariant(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define invariant2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define invariant3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)
#  define assert_logic(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define assert_logic2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define assert_logic3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)

#  define PRECONDITION_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 1)
#  define PRECONDITION2_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 2)
#  define PRECONDITION3_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 3)
#  define POSTCONDITION_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define POSTCONDITION2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define POSTCONDITION3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
#  define INVARIANT_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define INVARIANT2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define INVARIANT3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
#  define ASSERT_LOGIC_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define ASSERT_LOGIC2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define ASSERT_LOGIC3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
#endif  /* NDEBUG */



/* For postconditions where we need to save a value at the function's start: */
/* C example:
void foo(Widget* p)
{
   int origVal;
   _Bool origHasValue = 0;
   if (POSTCONDITION_MACRO_IS_ACTIVE) {
      origVal = p->a;
      origHasValue = 1;
   }
   ... preconditions ...

   ... function body ...   
   changeWidget(p);
   ... more function body ...

   postcondition(origHasValue && origVal < p->a);
}
*/

/* C++ example:
   We'll normally want to use std::optional (requires C++17)
   or boost::optional (pre C++17):
void bar()
{
   std::optional<int> origVal;
   if (POSTCONDITION_MACRO_IS_ACTIVE)
      origVal = getValue();
   ... preconditions ...
   ... function body ...
   postcondition(origVal.has_value() && origVal < getValue());
}
*/



#if defined(NDEBUG)
#  define PBC_INVARIANTS_CHECK(METHOD_NAME) ((void)0)
#else
#  define PBC_INVARIANTS_CHECK(METHOD_NAME) do { \
                                    if (pbcGetHandlerAssertLevel() > 0) { \
                                        METHOD_NAME(); \
                                    } } while(0)
#endif


#if defined(__cplusplus)
#  if defined(NDEBUG)
#     define PBC_INVARIANTS_GUARD(INVARIANTS_METHOD_NAME) ((void)0)
#  else
#     define PBC_INVARIANTS_GUARD(INVARIANTS_METHOD_NAME) \
         using OuterClassPtrPbcInvariantsGuard = decltype(this); \
         class PbcClassInvariantsGuard { \
         public: \
            explicit \
            PbcClassInvariantsGuard(OuterClassPtrPbcInvariantsGuard ptr) \
               : pOuter(ptr) \
            { \
               if (pbcGetHandlerAssertLevel() > 0) \
                  pOuter->INVARIANTS_METHOD_NAME(); \
            } \
            ~PbcClassInvariantsGuard() \
            { \
               if (pbcGetHandlerAssertLevel() > 0) \
                  pOuter->INVARIANTS_METHOD_NAME(); \
            } \
            PbcClassInvariantsGuard(const PbcClassInvariantsGuard&) = delete; \
            PbcClassInvariantsGuard& operator=(const PbcClassInvariantsGuard&) = delete; \
         private: \
            OuterClassPtrPbcInvariantsGuard pOuter; \
         } pbcClassInvariantsGuardVar(this)
#  endif
#endif  /* defined(__cplusplus) */


#endif  /* HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED */
