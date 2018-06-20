// --- This file is distributed under the MIT Open Source License,
// as detailed in "_LICENSE (MIT).TXT" in this directory ---

#include <hurchalla/programming_by_contract/assert_handler.h>


/* You can write a custom pbcAssertHandler implementation below to handle pbc
assert violations. A few possible custom handlers might be:
  - A handler that prints to stderr and calls abort().
  - A handler that prints to stderr and spins/sleeps and wakes periodically to
    look for a debugger or signal.
  - A handler that wraps assertion info in hurchalla::BrokenContract (or derived
    class) and throws
Note that
  - A handler for a plugin architecture should probably always use (C++)
    exceptions in order to avoid crashing the host in the event of a plugin 
    contract violation.
  - If you want to log to file in your handler, it could be useful to print the
    call stack.  Unfortunately this is platform specific and nontrivial; see
    backtrace() and backtrace_symbols() on unix, and possibly
    CaptureStackBackTrace() on Windows. Also C++ functions on the stack will
    have name mangling, and you'd want to unmangle when logging; see 
    __cxa_demangle() for gcc and perhaps UnDecorateSymbolName() for MSVC.
    Overall, https://oroboro.com/stack-trace-on-crash/ may be useful.
Some examples follow: */
#if 0
#   include <sstream>
    // If your assert handler throws, the exception must be derived from
    // hurchalla::BrokenContract
    void pbcAssertHandler(const char* failedAssertion, const char* filename,
                          int line)
    {
        std::ostringstream ss;  
        ss << "Assert failed (" << failedAssertion << "): " << filename
           << " line " << line;
        throw hurchalla::BrokenContract(ss.str());
    }
#else
#   include <iostream>
#   include <cstdlib>
    void pbcAssertHandler(const char* failedAssertion, const char* filename,
                          int line)
    {
        std::cerr << "Assert failed (" << failedAssertion << "): " << filename
                  << " line " << line << "\n";
        std::abort();
        // You could potentially use __debugbreak() for msvc, and
        // __builtin_trap() for gcc
    }
#endif

/*  You can set your executable's contract assert level by creating a predefined
macro specific to compiling this file, called:
    PBC_ASSERT_LEVEL
  If predefined, this macro must be set so that 0 <= PBC_ASSERT_LEVEL <= 3. When
you set PBC_ASSERT_LEVEL lower than some value X, the contract-asserts
invariantX(), postconditionX(), assert_bodyX(), etc become effectively disabled
after linking (with the exception of the level0 static_asserts). If you are
using link-time-optimization, typically those particular contract-asserts will
be completely removed from the code.  If you do not predefine PBC_ASSERT_LEVEL,
it will be defined below in this file with a default value (the default ensures
asserts are active for debug mode and inactive for release mode).
  You can optionally set your executable's level specifically for precondition
asserts by predefining a macro called:
    PBC_ASSERT_LEVEL_PRE
  If you do not predefine it, it will be set to the PBC_ASSERT_LEVEL. This macro
is separate from PBC_ASSERT_LEVEL because preconditions are special - they
detect programming errors (usually) on the caller's side, whereas all other
types of asserts detect programming errors (usually) on the callee's side. If
you predefine PBC_ASSERT_LEVEL_PRE, its value will be used instead of
PBC_ASSERT_LEVEL for enabling/disabling precondition asserts. You must set this
macro to a value such that 0 <= PBC_ASSERT_LEVEL_PRE <= 3.
*/
/*  You can change the defaults for your executable's assert levels here.
Note that if you want settings specific to your particular project/executable,
it's best to predefine PBC_ASSERT_LEVEL and PBC_ASSERT_LEVEL_PRE rather than
relying on these defaults. Note also that NDEBUG is used only for determining
default assertion levels -  - it does not disable precondition asserts when you
have predefined PBC_ASSERT_LEVEL_PRE, and does not disable any other asserts
when you have predefined PBC_ASSERT_LEVEL.
*/
#if defined(NDEBUG)
#   define PBC_DEFAULT_ASSERT_LEVEL            0
#   define PBC_DEFAULT_ASSERT_LEVEL_PRE        0
#else
#   define PBC_DEFAULT_ASSERT_LEVEL            3
#   define PBC_DEFAULT_ASSERT_LEVEL_PRE        3
#endif




/* ------------------------------------------------------------------------ */
/* -- You usually shouldn't change anything below this line -- */
/* ------------------------------------------------------------------------ */

#if defined(PBC_ASSERT_LEVEL) && (1 - PBC_ASSERT_LEVEL - 1 == 2)
#   error "PBC_ASSERT_LEVEL is defined, but with no value or negative value"
#endif
#if defined(PBC_ASSERT_LEVEL_PRE) && (1 - PBC_ASSERT_LEVEL_PRE - 1 == 2)
#   error "PBC_ASSERT_LEVEL_PRE is defined, but with no value or negative value"
#endif
#if defined(PBC_ASSERT_LEVEL)
#   if !defined(PBC_ASSERT_LEVEL_PRE)
#       define PBC_ASSERT_LEVEL_PRE  PBC_ASSERT_LEVEL
#   endif
#else
#   define PBC_ASSERT_LEVEL  PBC_DEFAULT_ASSERT_LEVEL
#   if !defined(PBC_ASSERT_LEVEL_PRE)
#       define PBC_ASSERT_LEVEL_PRE  PBC_DEFAULT_ASSERT_LEVEL_PRE
#   endif
#endif
#if ((PBC_ASSERT_LEVEL_PRE < 0) || (PBC_ASSERT_LEVEL_PRE > 3))
#   error "Invalid assert level for PBC_ASSERT_LEVEL_PRE"
#endif
#if ((PBC_ASSERT_LEVEL < 0) || (PBC_ASSERT_LEVEL > 3))
#   error "Invalid assert level for PBC_ASSERT_LEVEL"
#endif

int pbcGetHandlerPreconditionAssertLevel()
{
    return PBC_ASSERT_LEVEL_PRE;
}

int pbcGetHandlerAssertLevel()
{
    return PBC_ASSERT_LEVEL;
}
