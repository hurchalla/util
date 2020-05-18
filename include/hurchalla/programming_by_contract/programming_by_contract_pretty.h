/* --- This file is distributed under the MIT Open Source License, as detailed
   in "LICENSE.TXT" in the root of the programming_by_contract repository --- */

#ifndef HURCHALLA_PROGRAMMING_BY_CONTRACT_PRETTY_H_INCLUDED
#define HURCHALLA_PROGRAMMING_BY_CONTRACT_PRETTY_H_INCLUDED


#include "hurchalla/programming_by_contract/programming_by_contract.h"

/*
This header file is intended for inclusion ONLY by source files. It is NOT
  intended to be included by other header files.
This header file defines "pretty" and non-obtrusive macro names for the contract
  assertions. Using these names generally looks and feels more natural than
  using the longer, all-caps, and a bit ugly macro names defined by
  programming_by_contract.h. However, like all macros, the macros here 'pollute'
  the global namespace with macro names that could potentially conflict with a
  variable or function (or possibly another macro) using the same name. These
  "pretty" macro names are particularly dangerous due to the fact that they are
  uncapitalized common names- such as 'precondition' (the use of non-caps is in
  keeping with the standard 'assert' macro, with the intention that these macros
  not be overly distracting in code). Consequently, you must carefully treat all
  the assertion macro names from this header as if they were reserved keywords,
  throughout any source file that includes it.  Note: most macro names limit the
  chances of name collisions by using all upper case and unusual names, and
  furthermore most macros are not defined in headers (or they are #undef'ed by
  the end of the header).
When you include this header in one of your source files, you can probably
  manage the danger of name conflicts without too much difficulty. It is quite
  problematic to include this header in one of your own header files though.
  Doing so would mean implicitly passing on to the rest of the world (that
  #includes your header) the requirement to treat these assertion macro names as
  reserved keywords. And unfortunately, users of your header may be completely
  unaware of this requirement, and of the name collision danger these macros
  invisibly introduce. Or users may be aware, but unable or unwilling to manage
  the danger. It's best to keep any name space issue locally limited to your
  own source files, where you are the only one responsible for handling it.
*/

#define precondition(...) HPBC_PRECONDITION(__VA_ARGS__)
#define precondition2(...) HPBC_PRECONDITION2(__VA_ARGS__)
#define precondition3(...) HPBC_PRECONDITION3(__VA_ARGS__)
#define postcondition(...) HPBC_POSTCONDITION(__VA_ARGS__)
#define postcondition2(...) HPBC_POSTCONDITION2(__VA_ARGS__)
#define postcondition3(...) HPBC_POSTCONDITION3(__VA_ARGS__)
#define invariant(...) HPBC_INVARIANT(__VA_ARGS__)
#define invariant2(...) HPBC_INVARIANT2(__VA_ARGS__)
#define invariant3(...) HPBC_INVARIANT3(__VA_ARGS__)
#define assert_logic(...) HPBC_ASSERT(__VA_ARGS__)
#define assert_logic2(...) HPBC_ASSERT2(__VA_ARGS__)
#define assert_logic3(...) HPBC_ASSERT3(__VA_ARGS__)


#endif
