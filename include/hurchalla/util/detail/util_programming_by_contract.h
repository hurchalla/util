/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of this repository --- */
 
#ifndef HURCHALLA_UTIL_UTIL_PROGRAMMING_BY_CONTRACT_H_INCLUDED
#define HURCHALLA_UTIL_UTIL_PROGRAMMING_BY_CONTRACT_H_INCLUDED

/*
The single most important programming by contract assertion macro is
    HPBC_UTIL_API_PRECONDITION(x)

  HPBC_UTIL_API_PRECONDITION is intended to check that a client of an API is
correctly calling a function in the API. Correctly calling the API needs to be a
client responsibility, since a provider can't know at compile-time whether the
values a caller uses for function arguments satisfy an API function's
preconditions. This needs to be checked at run time, which can be done via
HPBC_UTIL_API_PRECONDITION.
  If the provider writes HPBC_UTIL_API_PRECONDITION assertions, then the client
can check them at run-time to ensure that they are correctly calling an API
function.  [ Typically a provider places HPBC_UTIL_API_PRECONDITION assertions
inside a small force-inlined wrapper function(s) within an API header file, and
the wrapper function does nothing except check that the client has satisfied the
function preconditions, and then forwards the call to the real function (which
is not exposed by the API). ]

Clients can enable the HPBC_UTIL_API_PRECONDITION assertions to check that they
have satisfied the API preconditions, by defining

    HURCHALLA_UTIL_CHECK_API_PRECONDITIONS

when they compile any code that uses the API.
  If a client does not define HURCHALLA_UTIL_CHECK_API_PRECONDITIONS, then the
HPBC_UTIL_API_PRECONDITION assertion macros will do nothing and will have
zero cost. When they are enabled, the assertions will have a (usually very
small) performance penalty. And so HURCHALLA_UTIL_CHECK_API_PRECONDITIONS
is recommended for development and debug builds, and not for release builds
where performance matters.


The rest of the programming by contract assertion macros (and also
HPBC_UTIL_API_PRECONDITION) are enabled by defining
    HURCHALLA_UTIL_ENABLE_ASSERTS

Typically only a provider would define this, in order to help them check that
code internal to their library or module is correct. Ordinarily a client would
not define this macro, since they usually aren't interested in internal details
of a provider's code, and there would be a run-time performance penalty in doing
so.

The names of the of the programming by contract macros that are enabled by
defining HURCHALLA_UTIL_ENABLE_ASSERTS are:
    HPBC_UTIL_API_PRECONDITION(x)
    HPBC_UTIL_PRECONDITION(x)
    HPBC_UTIL_PRECONDITION2(x)
    HPBC_UTIL_PRECONDITION3(x)
    HPBC_UTIL_POSTCONDITION(x)
    HPBC_UTIL_POSTCONDITION2(x)
    HPBC_UTIL_POSTCONDITION3(x)
    HPBC_UTIL_INVARIANT(x)
    HPBC_UTIL_INVARIANT2(x)
    HPBC_UTIL_INVARIANT3(x)
    HPBC_UTIL_ASSERT(x)
    HPBC_UTIL_ASSERT2(x)
    HPBC_UTIL_ASSERT3(x)

    And for C++ only,
       HPBC_UTIL_CONSTEXPR_PRECONDITION(x)
       HPBC_UTIL_CONSTEXPR_POSTCONDITION(x)
       HPBC_UTIL_CONSTEXPR_INVARIANT(x)
       HPBC_UTIL_CONSTEXPR_ASSERT(x)

All assertions are purposely implemented as macros instead of inline functions,
as described at the end of this section.

Precondition asserts are intended to check that a precondition is satisfied.
Postcondition asserts are intended to check that a postcondition is satisfied.
Invariant asserts are intended to check that invariants are true.
Plain asserts are intended to check that logical assumptions are true.

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
  A level 2 assert is intended for checks that you know will be somewhat
    expensive to perform.
  A level 3 assert is for checks that are incredibly expensive, such as a check
    with a larger order of computational complexity than the function that
    contains it. It could be considered a "safe mode" assert.

If HURCHALLA_UTIL_ENABLE_ASSERTS is not defined in a translation unit, then all
  of these asserts will be replaced with a no-op, regardless of assertion level.
If HURCHALLA_UTIL_ENABLE_ASSERTS is defined, each non-CONSTEXPR assert will use
  a conditional to skip the assert check if your particular assert's level is
  greater than the program-wide assert level - as defined by
  HURCHALLA_UTIL_ASSERT_LEVEL (note if HURCHALLA_UTIL_ASSERT_LEVEL is undefined,
  it is set to the default value of 3). If this occurs and you are using link-
  time-optimization, the linker will usually see that the code will always skip
  the assert, and it will remove the getter call and the conditional and the
  assert, resulting in a no-op. However, the effects of link-time-optimization
  depend upon your linker implementation.

You can define HURCHALLA_UTIL_ENABLE_ASSERTS (or not) for any compilation
  project - for example by using -D in gcc or clang, or in MSVC++ via either
  command line option /D or by UI setting "preprocessor definitions".

The C++ CONSTEXPR assertion macros are designed for use in constexpr functions.
  If one of these assertions is evaluated at compile-time, it will cause a
  compilation error when the asserted condition is false.  If it is evaluated
  at run-time it essentially just calls HPBC_UTIL_ASSERT.

Note: the assertions are all purposely implemented as macros instead of inline
  functions. Implementing them as functions involves at least two unavoidable
  problems, which is probably why the standard assert() from assert.h is also
  implemented as a macro. First, the assserts use the C/C++ standard predefined
  macros __FILE__ and __LINE_ to show the location of an assertion failure. If
  assertions were implemented as functions, this could not work, since __FILE__
  and __LINE_ would provide locations in the called assert functions rather than
  the location of the assert in the caller. Second, the contract assertions are
  designed to guarantee zero overhead if HURCHALLA_UTIL_ENABLE_ASSERTS is
  undefined in a translation unit. If assertions were implemented as functions,
  the argument given to an assert could break that guarantee. For example
  consider: assert(isGood()). If isGood() has side effects, the compiler would
  always need to call it, even if a hypothetical assert function (instead of
  macro) had an empty body. Even if isGood() has no side effects, it's possible
  the compiler and linker would miss an optimization we ordinarily would expect
  to remove the isGood() call. Missed optimizations are unlikely, but the lack
  of guarantee and more importantly the side-effects issue would make it
  impossible to guarantee zero overhead when the asserts are not enabled.
  Additionally, such behavior would be inconsistent with the standard library
  assert() macro defined in assert.h.
Assertions implemented as macros do not have these problems. However, macros
  have the problem (amongst others) that they 'pollute' the namespace.
  If your souce code uses any names identitical to one of the macro names in
  this file, then you may either get errors while compiling, or bugs from the
  compiler silently replacing your name with a macro expansion. Consequently,
  you *must* treat all the assertion macro names in this file as reserved
  keywords throughout your projects. If the given assertion names here are
  impossible or undesirable for you to use, then you should either edit this
  file to change the macro names, or not use this file. To minimize the
  pollution aspect, the current macro names are all-caps and prefixed with
  "HPBC_" followed by the repository name in all-caps, and then followed by the
  assertion name in all-caps. Nevertheless, potential macro name collision is
  a danger with the use of any macro.
*/
/*
Some ideas in this file were inspired by
 https://www.youtube.com/watch?v=1QhtXRMp3Hg
 http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
*/

/*
Details that are not handled by this file:
   It could be useful to print the call stack.  Unfortunately this is platform
specific and nontrivial; see backtrace() and backtrace_symbols() on unix, and
possibly CaptureStackBackTrace() on Windows.  Also C++ functions on the stack
will have name mangling, and you'd want to unmangle when logging; see 
__cxa_demangle() for gcc and perhaps UnDecorateSymbolName() for MSVC.
Overall, https://oroboro.com/stack-trace-on-crash/ may be useful.
*/



#ifdef _MSC_VER
#  define HPBC_UTIL_DO_NOTHING(...) (false ? (void)(__VA_ARGS__) : (void)0)
#else
#  define HPBC_UTIL_DO_NOTHING(...) ((void)(true || (__VA_ARGS__)))
#endif



#if defined(HURCHALLA_UTIL_ENABLE_ASSERTS) || \
    defined(HURCHALLA_UTIL_CHECK_API_PRECONDITIONS)

#  if defined(__cplusplus)
#    include <cstdio>
#    include <cstdlib>
#  else
#    include <stdio.h>
#    include <stdlib.h>
#  endif
      /* We can (probably) detect if exceptions are enabled by checking the
         gcc/clang macro __EXCEPTIONS, msvc's macro _CPPUNWIND, and the official
         but not always supported C++98 macro __cpp_exceptions. */
#  if defined(__cplusplus) && (defined(__EXCEPTIONS) || defined(_CPPUNWIND) \
                        || (defined(__cpp_exceptions) && __cpp_exceptions != 0))
      // with exceptions: treat an exception as a failure during assert
#     define HPBC_UTIL_BASIC_ASSERT(...) \
               do { \
                  bool assertPassed = false; \
                  try { if (__VA_ARGS__) assertPassed = true; } \
                  catch (...) {} \
                  if (!assertPassed) { \
                     fprintf(stderr, "Assert failed (%s): file %s, line %d\n", \
                              #__VA_ARGS__, __FILE__, __LINE__); \
                     std::abort(); \
                  } \
               } while(0)
#  else   /* without exceptions */
#     define HPBC_UTIL_BASIC_ASSERT(...) \
               do { \
                  if (__VA_ARGS__) {} \
                  else { \
                     fprintf(stderr, "Assert failed (%s): file %s, line %d\n", \
                              #__VA_ARGS__, __FILE__, __LINE__); \
                     abort(); \
                  } \
               } while(0)
#  endif

#  define HPBC_UTIL_API_PRECONDITION(...) do { \
                            HPBC_UTIL_BASIC_ASSERT(__VA_ARGS__); } while(0)

#else
#  define HPBC_UTIL_API_PRECONDITION(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)

#endif



#if !defined(HURCHALLA_UTIL_ENABLE_ASSERTS)

#  define HPBC_UTIL_PRECONDITION(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_PRECONDITION2(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_PRECONDITION3(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION2(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION3(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_INVARIANT(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_INVARIANT2(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_INVARIANT3(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_ASSERT(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_ASSERT2(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_ASSERT3(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)

#  if defined(__cplusplus)
#     define HPBC_UTIL_FALSE_VALUE (false)
#  else
#     define HPBC_UTIL_FALSE_VALUE (0)
#  endif
#  define HPBC_UTIL_PRECONDITION_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_PRECONDITION2_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_PRECONDITION3_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_POSTCONDITION_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_INVARIANT_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_INVARIANT2_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_INVARIANT3_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_ASSERT_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_ASSERT2_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE
#  define HPBC_UTIL_ASSERT3_MACRO_IS_ACTIVE HPBC_UTIL_FALSE_VALUE

#else

#  define HPBC_UTIL_DEFAULT_ASSERT_LEVEL       3

#  if defined(HURCHALLA_UTIL_ASSERT_LEVEL) && (1 - HURCHALLA_UTIL_ASSERT_LEVEL - 1 == 2)
#    error "HURCHALLA_UTIL_ASSERT_LEVEL is defined but with no value, or negative value"
#  endif
#  if !defined(HURCHALLA_UTIL_ASSERT_LEVEL)
#    define HURCHALLA_UTIL_ASSERT_LEVEL  HPBC_UTIL_DEFAULT_ASSERT_LEVEL
#  endif
#  if ((HURCHALLA_UTIL_ASSERT_LEVEL < 0) || (HURCHALLA_UTIL_ASSERT_LEVEL > 3))
#    error "Invalid assert level for HURCHALLA_UTIL_ASSERT_LEVEL"
#  endif

#  define HPBC_UTIL_LEVEL_ASSERT(LEVEL, ...) do { \
                       if (HURCHALLA_UTIL_ASSERT_LEVEL >= LEVEL) { \
                            HPBC_UTIL_BASIC_ASSERT(__VA_ARGS__); } } while(0)

#  define HPBC_UTIL_PRECONDITION(...) HPBC_UTIL_LEVEL_ASSERT(1, __VA_ARGS__)
#  define HPBC_UTIL_PRECONDITION2(...) HPBC_UTIL_LEVEL_ASSERT(2, __VA_ARGS__)
#  define HPBC_UTIL_PRECONDITION3(...) HPBC_UTIL_LEVEL_ASSERT(3, __VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION(...) HPBC_UTIL_LEVEL_ASSERT(1, __VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION2(...) HPBC_UTIL_LEVEL_ASSERT(2, __VA_ARGS__)
#  define HPBC_UTIL_POSTCONDITION3(...) HPBC_UTIL_LEVEL_ASSERT(3, __VA_ARGS__)
#  define HPBC_UTIL_INVARIANT(...) HPBC_UTIL_LEVEL_ASSERT(1, __VA_ARGS__)
#  define HPBC_UTIL_INVARIANT2(...) HPBC_UTIL_LEVEL_ASSERT(2, __VA_ARGS__)
#  define HPBC_UTIL_INVARIANT3(...) HPBC_UTIL_LEVEL_ASSERT(3, __VA_ARGS__)
#  define HPBC_UTIL_ASSERT(...) HPBC_UTIL_LEVEL_ASSERT(1, __VA_ARGS__)
#  define HPBC_UTIL_ASSERT2(...) HPBC_UTIL_LEVEL_ASSERT(2, __VA_ARGS__)
#  define HPBC_UTIL_ASSERT3(...) HPBC_UTIL_LEVEL_ASSERT(3, __VA_ARGS__)

#  define HPBC_UTIL_PRECONDITION_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 1)
#  define HPBC_UTIL_PRECONDITION2_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 2)
#  define HPBC_UTIL_PRECONDITION3_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 3)
#  define HPBC_UTIL_POSTCONDITION_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 1)
#  define HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 2)
#  define HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 3)
#  define HPBC_UTIL_INVARIANT_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 1)
#  define HPBC_UTIL_INVARIANT2_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 2)
#  define HPBC_UTIL_INVARIANT3_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 3)
#  define HPBC_UTIL_ASSERT_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 1)
#  define HPBC_UTIL_ASSERT2_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 2)
#  define HPBC_UTIL_ASSERT3_MACRO_IS_ACTIVE (HURCHALLA_UTIL_ASSERT_LEVEL >= 3)

#endif



#if defined(__cplusplus) && defined(HURCHALLA_UTIL_ENABLE_ASSERTS)
   // this section was adapted from the ideas in
   // https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
   // https://gist.github.com/oliora/928424f7675d58fadf49c70fdba70d2f
#  include "hurchalla/util/compiler_macros.h"
#  include <utility>
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  endif
   template <class L>
   void hurchalla_hpbc_util_forward_lambda(L&& lambda) noexcept 
   {
      std::forward<L>(lambda)();
   }
#  if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic pop
#  endif
#  ifdef _MSC_VER
#    define HPBC_UTIL_CONSTEXPR_ASSERT(...) ((void)(HURCHALLA_LIKELY(__VA_ARGS__) ? \
                             (void)0 : hurchalla_hpbc_util_forward_lambda( \
                             [](){ HPBC_UTIL_ASSERT(!#__VA_ARGS__);}), (void)0))
#  else
#    define HPBC_UTIL_CONSTEXPR_ASSERT(...) ((void)(HURCHALLA_LIKELY(__VA_ARGS__) ? \
                             (void)0 : hurchalla_hpbc_util_forward_lambda( \
                             [](){ HPBC_UTIL_ASSERT(#__VA_ARGS__ == nullptr);}), (void)0))
#  endif
#  define HPBC_UTIL_CONSTEXPR_PRECONDITION(...) HPBC_UTIL_CONSTEXPR_ASSERT(__VA_ARGS__)
#  define HPBC_UTIL_CONSTEXPR_POSTCONDITION(...) HPBC_UTIL_CONSTEXPR_ASSERT(__VA_ARGS__)
#  define HPBC_UTIL_CONSTEXPR_INVARIANT(...) HPBC_UTIL_CONSTEXPR_ASSERT(__VA_ARGS__)

#else

#  define HPBC_UTIL_CONSTEXPR_ASSERT(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_CONSTEXPR_PRECONDITION(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_CONSTEXPR_POSTCONDITION(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)
#  define HPBC_UTIL_CONSTEXPR_INVARIANT(...) HPBC_UTIL_DO_NOTHING(__VA_ARGS__)

#endif


#endif
