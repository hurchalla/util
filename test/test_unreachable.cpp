// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/unreachable.h"
#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <array>


namespace {


// it's difficult to test unreachable(), since the only thing it should affect
// is compiler optimization of subsequent code.  The goal of unreachable() is
// that we hope the compiler will remove any code thatcomes after unreachable()
// in the same scope.
// It should be undefined behavior for unreachable() to execute, and in correct
// code it should be impossible to ever execute it.

// Probably the best we can do to test it is to create a scope that is
// impossible to reach, and call unreachable() from there.

void call_unreachable(bool makecall) {
    if (makecall)
        ::hurchalla::unreachable();
}

TEST(HurchallaUtil, unreachable) {
    call_unreachable(false);
}


} // end namespace
