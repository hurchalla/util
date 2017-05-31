
#include <hurchalla/contracts/programming_by_contract.h>

/*
 You can write a custom pbcAssertHandler implementation below to handle pbc
 assert violations. A few possible custom handlers might be:
    - A handler that prints to stderr and calls abort().
    - A handler that prints to stderr and spins/sleeps and wakes periodically to
      look for a debugger or signal.
 Note that if you want to log to file in your handler, it could be useful to
 print the call stack.  Unfortunately this is platform specific and nontrivial;
 see backtrace() and backtrace_symbols() on unix, and possibly
 CaptureStackBackTrace() on Windows.
*/
#if PBC_ASSERTS_ARE_ENABLED
#   include <stdio.h>
#   include <stdlib.h>
    void pbcAssertHandler(const char* failedAssertion,
                          const char* filename,
                          int line)
    {
        fprintf(stderr, "Assert failed (%s): %s line %d\n", failedAssertion,
                filename, line);
        abort();
    }
#endif
