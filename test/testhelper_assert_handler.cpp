// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of this repository ---


// This file is *not* an example of a normal assert handler. For the purposes of
// testing it implements functions  setTestHandlerPreconditionAssertLevel(int);
// and  setTestHandlerAssertLevel(int);
// which are not recommended for any normal assert handler file.
//
// See util/example_assert_handlers/ for examples of some normal assert handlers


#include "testhelper_assert_handler.h"
#include "hurchalla/util/assert_handler.h"
#include <iostream>
#include <cstdlib>


#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmissing-noreturn"
#endif
void hpbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line)
{
    std::cerr << "Assert failed (" << failedAssertion << "): " << filename
              << " line " << line << "\n";
    std::abort();
}
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif


// Evil globals should be ok for the unit tests this file is written for.
static int globalPbcAssertLevelPre = 3;
static int globalPbcAssertLevel = 3;


int hpbcGetHandlerPreconditionAssertLevel()
{
    return globalPbcAssertLevelPre;
}

int hpbcGetHandlerAssertLevel()
{
    return globalPbcAssertLevel;
}

void setTestHandlerPreconditionAssertLevel(int level)
{
    globalPbcAssertLevelPre = level;
}

void setTestHandlerAssertLevel(int level)
{
    globalPbcAssertLevel = level;
}
