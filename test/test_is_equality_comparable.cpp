// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#include "hurchalla/util/traits/is_equality_comparable.h"
#include "hurchalla/util/compiler_macros.h"


namespace NS {
struct CannotA {};
struct CanB { bool operator==(const CanB&) { return true; } };
struct CanC { bool operator==(CanC) { return true; } };

struct CanD {
    friend bool operator==(const CanD&, const CanD&) { return true; }
};

struct CanE {};
inline bool operator==(const CanE&, const CanE&) { return true; }

struct CannotF { int operator==(const CannotF&) { return 0; } };
struct CanG : public CanD {};
struct CanH : public CanE {};

struct CanI {};
struct Base1 { Base1(CanI) {} };
inline bool operator==(const Base1&, const Base1&) { return true; }

struct CannotJ {};
struct Base2 {
    Base2(CannotJ) {}
    friend bool operator==(const Base2&, const Base2&) { return true; }
};

struct CannotK {
    friend bool operator==(const CannotK&, int) { return true; }
};
} // end namespace NS


namespace {

namespace hc = ::hurchalla;
static_assert(!hc::is_equality_comparable<NS::CannotA>::value, "");
static_assert(hc::is_equality_comparable<NS::CanB>::value, "");
static_assert(hc::is_equality_comparable<NS::CanC>::value, "");
static_assert(hc::is_equality_comparable<NS::CanD>::value, "");
static_assert(hc::is_equality_comparable<NS::CanE>::value, "");
// is_equality_comparable expects operator== to return bool (not int)
static_assert(!hc::is_equality_comparable<NS::CannotF>::value, "");
// ADL will identify CanD as an associated class of CanG and find operator==
static_assert(hc::is_equality_comparable<NS::CanG>::value, "");
// ADL will identify NS as an associated namespace of CanH and find operator==
static_assert(hc::is_equality_comparable<NS::CanH>::value, "");
static_assert(hc::is_equality_comparable<NS::CanI>::value, "");
// ADL does not identify Base2 as an associated class of CannotJ,
// and thus won't find operator==
#ifndef _MSC_VER   // MSVC gets this wrong (compiler bug), so skip it
static_assert(!hc::is_equality_comparable<NS::CannotJ>::value, "");
#endif
static_assert(!hc::is_equality_comparable<NS::CannotK>::value, "");

} // end namespace
