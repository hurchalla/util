// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/compiler_macros.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <array>


// the following seems to work, but the only reliable (sort of?) way to do it
// is to use the __OPTIMIZE__ macro.  The seems like a hack and too fragile to
// enable, and so it is disabled with #if 0 for now.
#if 0
// the compilers will ignore HURCHALLA_RESTRICT when no optimization level is
// set.  We use __OPTIMIZE__ to check if the optimization level is nonzero.

#ifdef __OPTIMIZE__

// purposely declared here but defined nowhere
extern void link_fail();

namespace {

HURCHALLA_NEVER_INLINE
void test_restrict(int* HURCHALLA_RESTRICT a, int* HURCHALLA_RESTRICT b)
{
    *a = 1;
    *b = 2;
    // If HURCHALLA_RESTRICT is working properly, the following clause will be
    // discarded since the conditional must always be false with restricted
    // pointers.
    // If HURCHALLA_RESTRICT is not working properly, this test will fail at
    // compile-time due to a linking error - the compiler will try to find a
    // definition for link_fail(), but link_fail purposely has no definition.
    if (*a == 2)
        link_fail();
}

TEST(HurchallaUtil, compiler_macros_restrict) {
    int a = 2;
    int b = 2;
    test_restrict(&a, &b);
}

} // end namespace

#endif
#endif


// There are no normal tests here.  We just are making sure that all the macros
// we try compile correctly.

namespace {

HURCHALLA_NEVER_INLINE
void test_restrict2(int* HURCHALLA_RESTRICT a, int* HURCHALLA_RESTRICT b)
{
    *a = 1;
    *b = 2;
}

HURCHALLA_FORCE_INLINE
void test_force_inline() {}

HURCHALLA_NEVER_INLINE
void test_never_inline() {}

void test_foo() {}

HURCHALLA_FLATTEN
void test_flatten()
{
    test_foo();  // HURCHALLA_FLATTEN should cause test_foo() to be inlined
}

TEST(HurchallaUtil, compiler_macros) {
    int a = 2;
    int b = 2;
    test_restrict2(&a, &b);

    test_force_inline();

    auto lambda = [&]() HURCHALLA_INLINE_LAMBDA {};
    lambda();

    test_flatten();

    constexpr int N = HURCHALLA_TARGET_BIT_WIDTH;
    static_assert(N > 0, "");

    HURCHALLA_REQUEST_UNROLL_LOOP for (int i=0; i<N; ++i)
        test_never_inline();

#if HURCHALLA_COMPILER_HAS_UINT128_T()
    for (__uint128_t i=0; i<2; ++i)
        test_foo();
#endif

    if HURCHALLA_LIKELY(a <= b)
        test_foo();

    if HURCHALLA_UNLIKELY(a > b)
        test_foo();

    int srcdst = 2;
    int val = 3;
    // this is (hopefully) an optimized form of  srcdst = (0<N) ? val : srcdst;
    HURCHALLA_CMOV(0 < N, srcdst, val);
    if (srcdst == 3)
        test_foo();
}

} // end namespace
