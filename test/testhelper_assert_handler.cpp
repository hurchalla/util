// --- This file is distributed under the MIT Open Source License, as detailed
// in "LICENSE.TXT" in the root of the programming_by_contract repository ---

#include "testhelper_assert_handler.h"
#include "hurchalla/programming_by_contract/assert_handler.h"


// This file is *not* an example of a normal assert handler. For the purposes of
// testing it implements functions  setTestHandlerPreconditionAssertLevel(int);
// and  setTestHandlerAssertLevel(int);
// which are not recommended for any normal assert handler file.
//
// See programming_by_contract\example_assert_handlers\  for examples of some
// normal assert handlers.


#include <iostream>
#include <cstdlib>
void pbcAssertHandler(const char* failedAssertion, const char* filename,
                      int line)
{
    std::cerr << "Assert failed (" << failedAssertion << "): " << filename
              << " line " << line << "\n";
    std::abort();
}


// Evil globals should be ok for the unit tests this file is written for.
static int globalPbcAssertLevelPre = 3;
static int globalPbcAssertLevel = 3;


int pbcGetHandlerPreconditionAssertLevel()
{
    return globalPbcAssertLevelPre;
}

int pbcGetHandlerAssertLevel()
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