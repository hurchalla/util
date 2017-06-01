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

// C++ only (C of course has no exceptions)
#if defined(__cplusplus) && !defined(HURCHALLA_EXCEPTION_CHECKING_H_INCLUDED)
#define HURCHALLA_EXCEPTION_CHECKING_H_INCLUDED

#include <hurchalla/contracts/programming_by_contract.h>


#ifndef PBC_CHECK_EXCEPTIONS
#   define PBC_CHECK_EXCEPTIONS 1
#endif


#if !PBC_CHECK_EXCEPTIONS || CPP_EXCEPTIONS_ARE_DISABLED
#   define PBC_BODY_THROWS(INVARIANT_EXPRESSION, EXPRESSION_BODY, ...) \
            PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY)
#   define PBC_BODY_NOTHROW(INVARIANT_EXPRESSION, EXPRESSION_BODY) \
            PBC_BODY(INVARIANT_EXPRESSION, EXPRESSION_BODY)
#else
    #include <sstream>
    #include <stdexcept>
    #include <typeinfo>
    namespace hurchalla { inline namespace v1 {

        template<typename Func>
        inline void runAndCheckExceptionParams(Func&& f, bool& wasHandled)
        {
            f();
            // if f() threw we won't get this far
            wasHandled = true;
        }

        template<typename Func, typename Ex1, typename... Exceptions>
        inline void runAndCheckExceptionParams(Func&& f, bool& wasHandled)
        {
            try {
                // if runAndCheckExceptionParams throws, wasHandled won't be
                // changed
                runAndCheckExceptionParams<Func, Exceptions...>
                                            (std::forward<Func>(f),
                                             wasHandled);
            } catch (const Ex1&) {
                wasHandled = true;
                throw;
            }
        }

        template<typename... Exceptions, typename Func>
        inline void runWithCheckedExceptions(Func&& f,
                                             const char* assertMessage,
                                             const char* filename,
                                             int line)
        {
            bool wasHandled = false;
            try {
                // A BrokenContract exception could be generated during other
                // contract checking, so we need to add it here to the allowed
                // exceptions
                runAndCheckExceptionParams<Func, BrokenContract, Exceptions...>
                                            (std::forward<Func>(f), wasHandled);
            }
            catch (const std::exception& e) {
                if (!wasHandled) {
                    std::ostringstream ss;
#if 1
// To make this compile when RTTI is not enabled, change the above line to #if 0
                    ss << assertMessage
                       << " [yet exception was thrown of type: "
                       << typeid(e).name() << ", explanation: "
                       << e.what()
                       << "]";
#else
                    ss << assertMessage
                       << " [yet unknown exception was thrown, explanation: "
                       << e.what()
                       << "]";
#endif
                    pbcAssertHandler(ss.str().c_str(), filename, line);
                }
                throw;
            } catch (...) {
                if (!wasHandled) {
                    std::ostringstream ss;
                    ss << assertMessage <<" [yet unknown exception was thrown]";
                    pbcAssertHandler(ss.str().c_str(), filename, line);
                }
                throw;
            }
        }

    }}  // end namespace

#   define PBC_BODY_THROWS(INVARIANT_EXPRESSION, EXPRESSION_BODY, ...) \
        ( \
        PBC_BODY( \
            INVARIANT_EXPRESSION, \
            hurchalla::runWithCheckedExceptions<__VA_ARGS__> \
                    ( \
                    [&]{EXPRESSION_BODY;}, \
                    "PBC_BODY_THROWS will only throw expected exceptions", \
                    __FILE__, \
                    __LINE__ \
                    ); \
            ) \
        )
#   define PBC_BODY_NOTHROW(INVARIANT_EXPRESSION, EXPRESSION_BODY) \
        ( \
        PBC_BODY( \
            INVARIANT_EXPRESSION, \
            hurchalla::runWithCheckedExceptions<> \
                    ( \
                    [&]{EXPRESSION_BODY;}, \
                    "PBC_BODY_NOTHROW will not throw exceptions", \
                    __FILE__, \
                    __LINE__ \
                    ); \
            ) \
        )
#endif

#endif  // include guard
