/* --- This file is distributed under the MIT Open Source License,
   as detailed in "_LICENSE (MIT).TXT" in this directory --- */

#ifndef HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED
#define HURCHALLA_PROGRAMMING_BY_CONTRACT_H_INCLUDED

/*
These are the main contract assertion macros:
    precondition(x), precondition2(x), precondition3(x)
    assert_body(x), assert_body2(x), assert_body3(x)
    postcondition(x), postcondition2(x), postcondition3(x)
    invariant(x), invariant2(x), invariant3(x)

Precondition asserts are intended to check that a precondition is satisfied.
Postcondition asserts are intended to check that a postcondition is satisfied.
Invariant asserts are intended to check that invariants are true.
Body asserts are intended to check that logic internal to a function is correct.

There are also four contract assertion macros that map simply to the function
static_assert(). These macros exist in order to clarify intent. They are checks
that can and will be made at compile-time:
    precondition_static(x), assert_body_static(x), postcondition_static(x),
    invariant_static(x)

The number at the end of an assertion macro name specifies the assertion level.
The assertion macros without a number at the end (precondition, assert_body,
postcondition, invariant) can be viewed as having an implicit 1. For more
detail:
  An unnumbered (implicit level 1) assert can be viewed as a "normal" assert
    and is the go-to assert level, useful for when you either don't care about
    levels at all, or for when an assert is unremarkable in the time it will
    take to check while the program is running. [Note that if you are able to
    perform a check at compile time, you should always prefer one of the static
    asserts, since they are free of cost and fail nicely at compile time.]
  A level 2 assert is intended for checks that you know will be unusually
    expensive to perform.
  A level 3 assert is for checks that are incredibly expensive, such as a check
    with a larger order of computational complexity than the function that
    contains it. It could be considered a "safe mode" assert.

Any non-static assert, regardless of level, will be translated into a no-op when
  compiled in a translation unit with NDEBUG defined. When compiling without
  NDEBUG defined, the assert will be translated into first a call to a trivial
  getter function to determine the program-wide assert level. This is followed
  by a branch to skip the assert if the program-wide assert level is less than
  that particular assert's level. For that case, if you are using link-time-
  optimization, the linker will typically see that the code will always skip the
  assert, and remove the getter call and the branch and the assert, resulting in
  a no-op. However, the effects of link-time-optimization are implementation
  dependent.

For a given compiler, by default NDEBUG is typically defined in release builds
  and undefined in debug builds.  It's a standard macro (see the C or C++
  documentation of assert()) and you can predefine it (or not) for any
  compilation project - for example by using -D in gcc or clang, or in MSVC via
  either /D or setting "preprocessor definitions" in the UI.
*/
/*
Some ideas in this file are inspired by
 https://www.youtube.com/watch?v=1QhtXRMp3Hg
 http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
*/

/*
Ordinarily, you shouldn't change anything in this file.
*/

#if defined(__cplusplus)
#  define precondition_static(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define assert_body_static(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define postcondition_static(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define invariant_static(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#else
   /* this assumes C11 support in your compiler.  To emulate static_assert
     pre-C11, see http://stackoverflow.com/questions/3385515/static-assert-in-c
     For visual studio C, see _STATIC_ASSERT */
#  define precondition_static(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define assert_body_static(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define postcondition_static(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#  define invariant_static(...) _Static_assert(__VA_ARGS__, #__VA_ARGS__)
#endif

#if defined(NDEBUG)
#  define precondition(...) ((void)0)
#  define precondition2(...) ((void)0)
#  define precondition3(...) ((void)0)
#  define assert_body(...) ((void)0)
#  define assert_body2(...) ((void)0)
#  define assert_body3(...) ((void)0)
#  define postcondition(...) ((void)0)
#  define postcondition2(...) ((void)0)
#  define postcondition3(...) ((void)0)
#  define invariant(...) ((void)0)
#  define invariant2(...) ((void)0)
#  define invariant3(...) ((void)0)

#  if defined(__cplusplus)
#     define PBC_FALSE_VALUE false
#  else
#     define PBC_FALSE_VALUE 0
#  endif
#  define PRECONDITION_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define PRECONDITION2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define PRECONDITION3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_BODY_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_BODY2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define ASSERT_BODY3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define POSTCONDITION3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT2_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#  define INVARIANT3_MACRO_IS_ACTIVE PBC_FALSE_VALUE
#else
#  include <hurchalla/programming_by_contract/assert_handler.h>

   /* If this is C++, we can (probably) detect if exceptions are enabled by
      checking the gcc/clang macro __EXCEPTIONS, the msvc macro _CPPUNWIND, and
      the official but not always supported C++98 macro __cpp_exceptions. */
#  if defined(__cplusplus) && (defined(__EXCEPTIONS) || defined(_CPPUNWIND) || \
                      (defined(__cpp_exceptions) && __cpp_exceptions != 0))
      // C++ with exceptions: treat an exception as a failure during an assert
#     define PBC_BASIC_ASSERT(...) do { \
                     bool assertPassed = false; \
                     try { if (__VA_ARGS__) assertPassed = true; } \
                     catch (...) {} \
                     if (!assertPassed) { \
                         pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); } \
                 } while(0)
#  else   /* C, or C++ without exceptions */
#     define PBC_BASIC_ASSERT(...) do { if (__VA_ARGS__) {} else  { \
                pbcAssertHandler(#__VA_ARGS__, __FILE__, __LINE__); } } while(0)
#  endif

#  define PBC_LEVEL_ASSERT(LEVEL, ...) do { \
                       if (pbcGetHandlerAssertLevel() >= LEVEL) { \
                            PBC_BASIC_ASSERT(__VA_ARGS__); } } while(0)
#  define PBC_LEVEL_ASSERT_PRE(LEVEL, ...) do { \
                       if (pbcGetHandlerPreconditionAssertLevel() >= LEVEL) { \
                            PBC_BASIC_ASSERT(__VA_ARGS__); } } while(0)

#  define precondition(...) PBC_LEVEL_ASSERT_PRE(1, __VA_ARGS__)
#  define precondition2(...) PBC_LEVEL_ASSERT_PRE(2, __VA_ARGS__)
#  define precondition3(...) PBC_LEVEL_ASSERT_PRE(3, __VA_ARGS__)
#  define assert_body(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define assert_body2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define assert_body3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)
#  define postcondition(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define postcondition2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define postcondition3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)
#  define invariant(...) PBC_LEVEL_ASSERT(1, __VA_ARGS__)
#  define invariant2(...) PBC_LEVEL_ASSERT(2, __VA_ARGS__)
#  define invariant3(...) PBC_LEVEL_ASSERT(3, __VA_ARGS__)

#  define PRECONDITION_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 1)
#  define PRECONDITION2_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 2)
#  define PRECONDITION3_MACRO_IS_ACTIVE \
                       (pbcGetHandlerPreconditionAssertLevel() >= 3)
#  define ASSERT_BODY_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define ASSERT_BODY2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define ASSERT_BODY3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
#  define POSTCONDITION_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define POSTCONDITION2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define POSTCONDITION3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
#  define INVARIANT_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 1)
#  define INVARIANT2_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 2)
#  define INVARIANT3_MACRO_IS_ACTIVE (pbcGetHandlerAssertLevel() >= 3)
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
 An alternative to the "if (POSTCONDITION_MACRO_IS_ACTIVE)" clause above
is to use the following (though I view it as a hack):
   postcondition(origVal = p->a, origHasValue = 1, 1);
*/

/* C++ example:
   Note we'll normally want to use std::optional (requires C++17)
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
