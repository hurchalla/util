// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of the programming_by_contract repository ---

#ifndef HURCHALLA_PBC_EXCEPTION_CHECKING_H_INCLUDED
#define HURCHALLA_PBC_EXCEPTION_CHECKING_H_INCLUDED

#if defined(__cplusplus)


#include <hurchalla/programming_by_contract/programming_by_contract.h>


#if defined(NDEBUG)
#   define PBC_VERIFY_THROWS(EXPRESSION_BODY, ...) \
                do { EXPRESSION_BODY; } while(0)
#   define PBC_VERIFY_NOTHROW(EXPRESSION_BODY) \
                do { EXPRESSION_BODY; } while(0)
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
            // if runAndCheckExceptionParams throws, wasHandled won't change
            runAndCheckExceptionParams<Func, Exceptions...>(
                            std::forward<Func>(f), wasHandled);
        } catch (const Ex1&) {
            wasHandled = true;
            throw;
        }
    }
    template<typename... Exceptions, typename Func>
    inline void runWithCheckedExceptions(Func&& f, const char* assertMessage,
                                         const char* filename, int line)
    {
        bool wasHandled = false;
        try {
            runAndCheckExceptionParams<Func, Exceptions...>
                                      (std::forward<Func>(f), wasHandled);
        }
        catch (const std::exception& e) {
            if (!wasHandled) {
                std::ostringstream ss;
#if 1
// To make this compile when RTTI is not enabled, change the above line to #if 0
                ss << assertMessage << " [yet exception was thrown of type: "
                   << typeid(e).name() << ", explanation: " << e.what() << "]";
#else
                ss << assertMessage
                   << " [yet unknown exception was thrown, explanation: "
                   << e.what() << "]";
#endif
                pbcAssertHandler(ss.str().c_str(), filename, line);
            }
            throw;
        } catch (...) {
            if (!wasHandled) {
                std::ostringstream ss;
                ss << assertMessage << " [yet unknown exception was thrown]";
                pbcAssertHandler(ss.str().c_str(), filename, line);
            }
            throw;
        }
    }

    }}  // end namespaces

#   define PBC_VERIFY_THROWS(EXPRESSION_BODY, ...) \
                 hurchalla::runWithCheckedExceptions<__VA_ARGS__>( \
                       [&]{EXPRESSION_BODY;}, \
                       "PBC_VERIFY_THROWS should only throw expected exceptions",\
                       __FILE__, __LINE__ \
                       )
#   define PBC_VERIFY_NOTHROW(EXPRESSION_BODY) \
                hurchalla::runWithCheckedExceptions<>( \
                       [&]{EXPRESSION_BODY;}, \
                       "PBC_VERIFY_NOTHROW should not throw exceptions", \
                       __FILE__, __LINE__ \
                       )
#endif


#endif // defined(__cplusplus)

#endif // include guard
