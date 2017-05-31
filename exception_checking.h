
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
