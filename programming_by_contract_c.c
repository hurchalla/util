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
