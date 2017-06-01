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
#include <atomic>

// You can write a custom pbcAssertHandler implementation below to handle pbc
// assert violations. A few possible custom handlers might be:
//   - A handler that prints to stderr and calls abort().
//   - A handler that prints to stderr and spins/sleeps and wakes periodically
//     to look for a debugger or signal.
//   - A handler that wraps assertion info in hurchalla::BrokenContract (or
//     derived class) and throws
// Note that
//   - A handler for a plugin architecture should probably always use (C++)
//     exceptions in order to avoid crashing the host in the event of a plugin
//     contract violation.
//   - If you want to log to file in your handler, it could be useful to print
//     the call stack.  Unfortunately this is platform specific and nontrivial;
//     see backtrace() and backtrace_symbols() on unix, and possibly
//     CaptureStackBackTrace() on Windows. Also C++ functions on the stack will
//     have name mangling, and you'd want to unmangle when logging; see
//     __cxa_demangle() for gcc and perhaps UnDecorateSymbolName() for MSVC.
//     Overall, https://oroboro.com/stack-trace-on-crash/ may be useful.

#if PBC_ASSERTS_ARE_ENABLED
    // Some examples:
#   if 0
#       include <sstream>
        // If your assert handler throws, the exception must be derived from
        // hurchalla::BrokenContract
        void pbcAssertHandler(const char* failedAssertion,
                              const char* filename,
                              int line)
        {
            std::ostringstream ss;
            ss << "Assert failed (" << failedAssertion << "): " << filename
               << " line " << line;
            throw hurchalla::BrokenContract(ss.str());
        }
#   else
#       include <iostream>
#       include <cstdlib>
        void pbcAssertHandler(const char* failedAssertion,
                              const char* filename,
                              int line)
        {
            std::cerr << "Assert failed (" << failedAssertion << "): "
                      << filename << " line " << line << "\n";
            std::abort();
            // You could potentially use __debugbreak() for msvc, and
            // __builtin_trap() for gcc
        }
#   endif


    // Since std::atomic's value constructor is constexpr, the following global
    // variable will be statically initialized before main() runs. (see
    // https://akrzemi1.wordpress.com/2012/05/27/constant-initialization/ )

    std::atomic<bool> globalPbcNegativeTestingIsActive_(false);

#endif
