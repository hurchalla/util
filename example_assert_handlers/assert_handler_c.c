/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of this repository --- */

#include "hurchalla/util/assert_handler.h"


/* This file is an example, intended to demonstrate the use of an assert handler
with Programming By Contract (see programming_by_contract.h and assert_handler.h
in hurchalla/util).

You can write a custom hpbcAssertHandler implementation below to handle pbc
assert violations. A few possible custom handlers might be:
  - A handler that prints to stderr and calls abort().
  - A handler that prints to stderr and spins/sleeps and wakes periodically to
    look for a debugger or signal.
Note that if you want to log to file in your handler, it could be useful to
print the call stack.  Unfortunately this is platform specific and nontrivial;
see backtrace() and backtrace_symbols() on unix, and possibly
CaptureStackBackTrace() on Windows. */

#include <stdio.h>
#include <stdlib.h>
void hpbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line)
{
    fprintf(stderr, "Assert failed (%s): %s line %d\n", failedAssertion,
            filename, line);
    abort();
}

/*  You can set your executable's contract assert level by creating a predefined
macro specific to compiling this file, called:
    HPBC_ASSERT_LEVEL
  If predefined, this macro must be set so that 0 <= HPBC_ASSERT_LEVEL <= 3.
Keeping in mind that the unnumbered asserts have an implicit level number of 1,
when you set HPBC_ASSERT_LEVEL lower than some value X, the contract-asserts
HPBC_INVARIANTx(), HPBC_POSTCONDITIONx(), HPBC_ASSERTx(), etc become effectively
disabled after linking. If you are using link-time-optimization, typically those
particular contract-asserts will be completely removed from the code.  If you do
not predefine HPBC_ASSERT_LEVEL, it will be defined below in this file with a
default value (the default ensures asserts are active for debug mode and
inactive for release mode).
  You can optionally set your executable's level specifically for precondition
asserts by predefining a macro called:
    HPBC_ASSERT_LEVEL_PRE
  If you do not predefine it, it will be set to the HPBC_ASSERT_LEVEL. This
macro is separate from HPBC_ASSERT_LEVEL because preconditions are special -
they detect programming errors (usually) on the caller's side, whereas all other
types of asserts detect programming errors (usually) on the callee's side. If
you predefine HPBC_ASSERT_LEVEL_PRE, its value will be used instead of
HPBC_ASSERT_LEVEL for enabling/disabling precondition asserts. You must set this
macro to a value such that 0 <= HPBC_ASSERT_LEVEL_PRE <= 3.
*/
/*  You can change the defaults for your executable's assert levels here.
Note that if you want settings specific to your particular project/executable,
it's best to predefine HPBC_ASSERT_LEVEL and HPBC_ASSERT_LEVEL_PRE rather than
relying on these defaults. Note also that NDEBUG is used only for determining
default assertion levels - it does not disable precondition asserts when you
have predefined HPBC_ASSERT_LEVEL_PRE, and does not disable any other asserts
when you have predefined HPBC_ASSERT_LEVEL.
*/
#if defined(NDEBUG)
#   define HPBC_DEFAULT_ASSERT_LEVEL            0
#   define HPBC_DEFAULT_ASSERT_LEVEL_PRE        0
#else
#   define HPBC_DEFAULT_ASSERT_LEVEL            3
#   define HPBC_DEFAULT_ASSERT_LEVEL_PRE        3
#endif




/* ------------------------------------------------------------------------ */
/* -- You usually shouldn't change anything below this line -- */
/* ------------------------------------------------------------------------ */

#if defined(HPBC_ASSERT_LEVEL) && (1 - HPBC_ASSERT_LEVEL - 1 == 2)
#   error "HPBC_ASSERT_LEVEL is defined but with no value or negative value"
#endif
#if defined(HPBC_ASSERT_LEVEL_PRE) && (1 - HPBC_ASSERT_LEVEL_PRE - 1 == 2)
#   error "HPBC_ASSERT_LEVEL_PRE is defined but with no value or negative value"
#endif
#if defined(HPBC_ASSERT_LEVEL)
#   if !defined(HPBC_ASSERT_LEVEL_PRE)
#       define HPBC_ASSERT_LEVEL_PRE  HPBC_ASSERT_LEVEL
#   endif
#else
#   define HPBC_ASSERT_LEVEL  HPBC_DEFAULT_ASSERT_LEVEL
#   if !defined(HPBC_ASSERT_LEVEL_PRE)
#       define HPBC_ASSERT_LEVEL_PRE  HPBC_DEFAULT_ASSERT_LEVEL_PRE
#   endif
#endif
#if ((HPBC_ASSERT_LEVEL_PRE < 0) || (HPBC_ASSERT_LEVEL_PRE > 3))
#   error "Invalid assert level for HPBC_ASSERT_LEVEL_PRE"
#endif
#if ((HPBC_ASSERT_LEVEL < 0) || (HPBC_ASSERT_LEVEL > 3))
#   error "Invalid assert level for HPBC_ASSERT_LEVEL"
#endif

int hpbcGetHandlerPreconditionAssertLevel()
{
    return HPBC_ASSERT_LEVEL_PRE;
}

int hpbcGetHandlerAssertLevel()
{
    return HPBC_ASSERT_LEVEL;
}
