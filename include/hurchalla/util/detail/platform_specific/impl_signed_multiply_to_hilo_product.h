// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SIGNED_MULT_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SIGNED_MULT_TO_HILO_H_INCLUDED


#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/programming_by_contract.h"
#include <cstdint>
#if defined(_MSC_VER)
#  include <intrin.h>
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif

namespace hurchalla { namespace detail {


// Slow implementation that works for all compilers and architectures.
// Return Value:
//   Returns the high portion of the product.
// Notes:
//   - I adapted this code from https://web.archive.org/web/20190109005441/http://www.hackersdelight.org/hdcodetxt/muldws.c.txt
//        Henry Warren writes that his function is "derived from Knuth's
//        Algorithm M from [Knu2] section 4.3.1.".  He is referring to "The Art
//        of Computer Programming", volume 2, 3rd edition; but we can note that
//        Knuth's Algorithm M calculates an unsigned multiplication, and so it
//        isn't of much interest.
//   - To easily view clang/gcc/etc generated asm, you can view (but probably
//        you should replace the slow_signed_multiply_to_hilo_product::call()
//        function with the latest version from this file below)
//        https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGe1wAyeAyYAHI%2BAEaYxCBmZgBspAAOqAqETgwe3r56KWmOAkEh4SxRMXGJdpgOGUIETMQEWT5%2BXLaY9gUMdQ0ERWGR0bEJtvWNzTltCmN9wQOlQxUAlLaoXsTI7BzmAMzByN5YANQmO27I0/iCp9gmGgCCu/uHmCdnBACeSZgA%2BgTETEIChud0eZj2DAOXmOpzctDwLCBIIeT0hLze53hiIIwJ2txRBLBeyoDCwVCOAAk5AAlNwU%2B4BAL3H4AFXu1IA4tgWT8LABJHkAdT5ABEWRTQbsjkc8OSyXN0BAfj9VAAOeI/eKSZVLE4AdjcJgNR3lIUVPwAsj8ABpapaS8HS6Wm15U2n0xnMtmc7m8gU/YViilHLUOnbSjqyk2YGhmpU/PA7dW6o2G40u81WvnW5Nhp3R2Ouml0hlM1nsrk8/lC0Xio47Mx5o6RuUxhXxpgNZAILU6/VptwF9uWn7si12pvOtshSnFj1l72Vv01oMhyRNltDuPKhosPup1NbzCZ0fUi32lGOp0u2fu0teiu%2B6sB2vBhsb%2BGtwvmn4pADu0RJMg%2B4GoeGbxkkQEgQOR4/soyhuDqk4yl%2Bw6/qgAHEEBvY/CmoHptOx4Qdh2q4f2YGEXBCE4XhMEmAArIaDz5ixrHgaedI/PBbh2uRBHfvGWrPmRB78cOATKLxomDgxTH3KxClTgJyoSTRyFKXMt4lp65Y%2BlW/qBnWoaXuGTodEo6lHlp84Pnpy4vqu74mfmriyh%2BUbsQmTBSfhg6eYQhh4D40EUcpWb3D5dGMaCimxXF8UJYl/neaRF5EvmN5utpC6PvpK5GeuznNrQFnMfmAD05VHFQYi0BETDIAA1ihRwAUcaBeLQ6AMGAHAENVwToEcBAIK8YzAJg/UMnIvUKEczCOAAbq8ESENGSQjUcTBUAQ0QxRVVW0KgqCNcEwBbf1STEMelHtagLBJHQ0RHCwDXEKkyHPNCrywhcBBXAQyLpU6UZCHyABa2CWvc1pvCKpwikcch8qELK9haMOWZlc73rpS7PoZwbGcDEafkcYOQ9DsMIwjSMoyyDZU1jhHWbji5PgZr71o2RWk6DENQxj1M7PDIt06jXAakLzOFqzOns3lDl1pLyHmZglnRO9xAnHEWU2XjHP5cGf5MHNXikpRpBbaSd2dd1vX9VE0boF4mzoOYPMk82pJuUVrlUGG/uSiZUYQHrbO5fZhMYmuaVSlZ4c6W4ADyFrKHyATYNSPz0kIPzIxLZiqqyEC6hogdk55FPYMnABiPz01wRchcasn7fFECeQchjAD8r3aCQ%2B4JOY8RHPG3dGH3TAD8QypvLcYs7EstGHic0VlR3nn09gAQ/CnacZ1nKbDwkY/Ktvu/7%2BnmfUvPtNcDsGgaMvTbY3eSep9fWc5/cecFyyTdi4slLkcLgFcPKUXjByUIchEIiRPqPTy0DYGWhRsnbO8D4gjzXnJRKYAwBb1RjvPen9D7UmPlg0%2B%2BCu4GEnpg7BbcN5xSgTAuB88jiSD4oOFhKCfhw1ppwke2DlTIMQhaNBGC75iy4C/Iqb9srMivmQn%2Bf9G7N2AbqMBJk1avxZonMsSib4qPzmooBIDy7aJ9gHQkoJdoPQMLtDEnxvjMDYEcFkVtnGMFYK8OQCNFqoDwO7PEoJpjEFdv1LwBAfgMB8NEPAwEsRAiOCAI4lwQAgFiWwYgCSfhJJxLCFkNx9RWBRHqUWpTHgh3JGHHGH8D5GNziYwuZjZH3EquTb4yA8BiDwAALyYF0Oa/BtbKmCAQQBfxrZDWVF4cZkyCAADpbGYHsYM9W7wvjeLcX4kJDwwkRKOFEmJcScmJIREiM4YzBALKtrs7AJT25pPqI4ZAd0GDTEwKoK6RwIjHVoDKBQPw0jADNHDYa4SNmVPzNMQZCT3mfO%2BdrP5nhAUJkEJgCa2tab/C8FCp5sLXkIt2kimUggTR4GAMk%2B%2BZg9SnEqUaCpKy1mONhF41xvigZoA%2BSSn5KKAXHKyfE852JcRwPmc3AgdybgZLwECkFZp6XMqSA4n6myXE%2BKRlygQiK%2BX/KOdEoVZy8kXIKVc9FEzJXSrxLKoF4zMV7R2JUuxKr1lOK2RyrVez7jct1drcZBqTnZNyfksV1zLXFylV67AGT8BUrNQyh4LrVVso9Zq%2B5oTcUOEDUakNpqw0/DmTcq10bHlMMJfC31vLkX6rlcCylYLaY1RKvi8tLzK06urb82tdqMVYvBbi1t8knQVreVWr5PyA1xupdIouSqylMqTas11rL1XbM5d68dpL%2BU5tOXm0VsJZkSsjdamNIA60KuPPO%2B4ya3Wpo1Ts7VPKJ01tRYKvdIrLlwKLRGv4p7bUWodcQa9t7V1uHZemp9fqyWRMNR%2Bk1B7zU/tudG2NlLLkMvBEHGxS6WVqvA2mtxRTvUHOzV83aHy8ARHoFPRqvxzaXqGkaaFI723AVNkoRoEB0nnt7btYAAJaCFJlSARaYg8WkCeaxD2ABNNYvVaAApCMeYaqA0ldJ6fCPprxyOMDSNR34r06OFso6ClTIyjgfDWNrLx7iPb2idU8rwaQjDDS2QOwjrweNGfo6ZxVZxiNnq8dexlIXwSh30Yo0hjTf7NIAeo0uaVQMbMNCRrN/VdOUYM7R3zjHD0WoWcU5jTmXPnVs7TI9xbI0hfKSB5dKazhA1Ixl1QFH9M0Z8yZvLSHj1/CK3qFj0pnNnTc98cFlXf2A0cwusLOxsNVIW81tJR0/xdYbcePunVHAqo%2BH8VAPwEB0H21dDAhzitlWS%2B6h9rxAv7VHe49qtUIBFNPit5Q70XYOCtiyI532jiLTStKc7w7pSjp%2BBx6IBAIDvuDZ%2Bs1bhAsAftViq2Ht7PXphWx8HChONQ/wdDuDsOENfsR7x%2BtZn0ArB1o2OIDnE0g6OB0izXzWAqswFbKMxHRYih/b2KNChDs7SOGsYACB%2BpEH4YvT2mO4Vjs7S%2Bo5fmVMBoF7KfqtMYfCuJ/D0n06cSM6OI2abDOmckGbKoVn9AOfki5wjXn2oo0rdegoZqIuxeqcl4jDQqha6%2B7977glbHiUK5%2B0702zVabcax6IaYwm8QQBkbCWEqudq6gALTPNlz8GPU2Ec3AT3T4ODOfteA0OCrwOtR5h5dxjp0P3Fpl9potSvRxq%2BNVr9KEvXBy9SIeSnqbg2HuLW7033vaTBcD6LwdI4bgxDIE6m62qw1RrtXejjo4J3Ps4mWUw0P%2B2jrl7LwAKn%2B%2BXY3%2BYfuHbyWpjX3eT8N47w9o6B28CH6OPfsB5%2B69HCv4d8vd//tP96dp8WQV8QhWtM87FGB%2Bo/w6AlNUB%2BpUBlpiAqAVtFlpR3tjoqBUl6B%2BohAEZGx6JsBuMJ8lh0DBRXhRAGA0lMB1YmEOkRpBlu1NoIBn8D8QQ8Rx81ddRDAhoIAr92DXsMJndGpdQWBnNHZXh6B19GDqChAd8TcqpQDogYwSAxoV9XpVAN9Uh2tXgxNvBXhUByRkA19cREYIAhA08ZEdYLAx5LDrDrA7CrClgj8LDnDPcnkOl7CbDyYrCfChAj8hAjgM8zBAifCR8RZaYAigiM82hWpDtuxAVl9XhND/txN1CmCqDfk6DFCjhrorpMAlAMUhpghRsbt0D3EEBnNkjHtaB58HEMhWo4D5pMBkDhdkDUCMIFCL9V8dDwVWD98b8QlOD%2B9j5bD%2BC8Br8W828xjW99tDtH8OllCWiIDCVVloCmjFN5oED2johOi/wrYLMRovNNVrpTYBAto5omA/llpujv9DtRd%2BiBChiF4%2B9SCfCIATC%2BiOC3juCfDf88AQsmE3sPszsxZPjTCMRYQuDU9%2Bwx42Cb8hC/wRDC8mFroCB1hqCHiEBa9QsZtjdM1wls0FAVtyczRNtaBttaBdsiAX9n9N8zsBt9pLt71107MRi2N9oftRBFMIAIM3FMtdCcs1sKc48gstlsEQTTsvsHsvA/sAd9pgcZdXlsdccCcg0tdQ0xSkc%2B1ohUc4h0cv9QcscIcuNNdjUtSAsRML11t0B9SaczBUSGcwdTSoc/CaYxYAA/DQe0w0wfF0nHSHKAd04Yh5BwyIsWKw302nIEhnYbVzII2mYjWwxKVMtMrw6qM3PLJheM86OQDza7c3NrKjDrJgYzBjW07UkAYLJ1aUDpfMizCs0UwkafZnC3exdnFqXZbne3P9VqYXLwUXcXIY7nbmQPWXYPUlANP8cvQnTU/NWEe5NDeNOaKqI3QfU3bWFnDs63L1Hs8ZPnK2NvAcocj3D0r3H3f3P3ccolLdH5II48yPMHHPWEfA%2BPRPRrM4P8dPUBWM/MfM0vcFZ802XPe5aHaY4Q8PJ0/80/IC6PECxc/PZvSUyCl3aCp0DpBoQgBANgIlbjBtNPTLK4YAXUHJU8/vPIzAAARzmWuiGgs3APFz/DU3hBCGBCYSCK8AiMRgrx%2BNakfyCOH3BWQtDP4oJPoKqlnzqIX0cWON6PXwZIcAUDuOlHzIRLf3vzPxTI6T%2BXNnQAUFSQYgsB9O0L/AgDMFIDMG/OCNMvMtID/GsAcMsF/Ponhg4p/0mPYLFi4vf0gNyRfLODfOIIb3QrrKql0tJAMrXgsDT3/DsqsvcOsDiosr/GcKtmSsstSusNiowjsuspyrMpSsSucrAVcqeQfPmNfybwAOAtj0Cvz1L1CsZ3CrWEisMvohioyoSqcosAyqyqpy6v6uCL6vTz6uKtsNKrcoZyCIBP/18uH3pTCu7T0qiqMoKvitSsdJ8LGpkXStyosoSsdIYimqeTUs8FQG4rmKmL4tqtAvz2/MWuauWrauipMpGpssmqnydCCIIAenBWeJ8NutfPzyOiOg/JTIwqqmYp6j6l2JQJWzHiUFeEYtbzmEB0huev0vas6v2syvGp2oGtxu6tGtxv6uOtOuGgekbzFiBrODAt%2BqSAguRKgsWp0taqxter2sKvsvT0%2BvcoZsuoZrHwctrKeoio5rWoJq5ryvcNiPJpbO%2BueQBHQHQHoHBWfz/0cNprcCCr5Kprpyeuhodjhv2MRtoJWP6lYswHRqWvFtWo6vWoOqGqStJrStsqdtloJvlumqVqYBVvoEurCT9tVrVVeLEu0papWuxsdvsrdoJo%2Bu9qeSlK336O1rAqDv9utqhLOD8uAgCrcDAu/JXiYUSgGOutPjTpBtQtEKapmspQQEwJTtplmq1v%2BGDoDp8IFseo6SNthqQL2IRu43NpRqtrjjRMmkxI8tF0boiVxNq3xPpyL1zKRi1HLwPId2vWXqEFXtpnXr6wJPCxqUixIQaW/iaX/kmQ0TSmXrkEAXLyVxmQKznWNy3rvoqyftVFm3m1BBvvBVvufsqS3vBSEEAVjIAofopKhz8SELuS2lgYiDjiZPHoxOIGoJJIwhFPJPEMpLwB2z2zpOO1BIcAyR5NoAGKtiYCtgQZqxOoeCCMY0gYgGgar1QCtiCMofJl%2BUQcH3RMnvQdWwYewapJpMqvpKIYIBIaeyOgoaoadLxIW3odtMgYOwajo2ICBSwHhCHKYZQrYbgc4eobKUH3YfQG0EuogCYFhGfjeFrl%2BRSSOC0vKq2lMbMH6IiCsZTB2FsaYHsccaYXzM1pptJKbKwa21wepPwcOzEelIkZQCkdYbHjkCWA4aYaWGoaNMopQaxNfwzz9rMZsryfXMlHnvuA4BWFoE4Hol4D8A4C0FIFQE4ENEsC1us02B1h2B4FIAkdqbKZWEahAEkHiEWQ0AAE54gRmzANAdhJAzBJANB4hVQ9R6J9BOBJBqnNBeAGmOBeADKfTumtAVg4BYAkA0AHonoyAKBPj7pHp6AYhgAFBmAkgBcEC%2BA6Bdp1HKAIgNnSBVpmBiAPhOBOnTm2BBBk4GBqTvmsBXojBxBuBeB8BroahloDKenSAvlqgokthOn7UKm4X9AqMAR/mPAsBAXeB/gERSWVhUCmB7mAA1PATAP8ZOFxUlmQQQEQMQdgKQNl%2BQJQNQb53QNoWhYwawawfFiIAyyAFYVADaDIFFtPZOcMNPS4BGUwZpywLgPUHZjoaoLoFwUkCYVoNF0kfoEoMoXIVIdIAQQ1i1/IDIU1wYGIKYHVmoAQHocYTwFoPQKoV17oGYB1hYJ10YXoG1qYf1uYM1oYXahQVprlrp66LFsplZjgKp0gGpuprZtUeINPVe4AZAN5Pk8JBgUQseJpqwSwK2XAQgM3XYOIjwM5259pmRMljZ5eUgfp%2BiHYRZEZqZjQPUMZ%2BiJZkZjp6QXFtZtN75rZnZkAPZ1t8pzgMwdZ1Fqdrpud0gZAtIZwSQIAA%3D%3D%3D
//   - On ARM32 with T=int64_t, clang and gcc produce assembly that is much
//        longer (and likely slower) than the asm they generate for the unsigned
//        mult function slow_unsigned_multiply_to_hilo_product::call().  ARM
//        does not seem to have great instructions for signed multiplication
//        (e.g. a UMAAL equivalent) like it does for unsigned multiplication, so
//        it might not be possible to write great assembly for this even by
//        hand.  The Hacker's Delight alternative implementation mentioned in
//        comments below produces even worse asm.
//   - On x64 with T=__int128, clang and gcc produce assembly that is much
//        longer (and likely slower) than the asm they generate for the unsigned
//        mult function slow_unsigned_multiply_to_hilo_product().  As with ARM,
//        I'm pessimistic that concise asm is possible for signed mult.  The
//        Hacker's Delight alternative implementation mentioned in comments
//        below produces even worse asm.
//   - Uses static member function to disallow ADL.
struct slow_signed_multiply_to_hilo_product {
  template <typename T> HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u, T v)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(ut_numeric_limits<T>::is_signed, "");
    // Assert the CPU architecture uses two's complement representation for
    // signed integers.  When analyzing the logic for this function I assumed
    // two's complement.  I haven't considered whether this function might also
    // perhaps work for non-two's complement.  Note that non-two's complement
    // representation is incredibly incredibly rare these days, and C++20
    // mandates two's complement for signed integers.
    static_assert(static_cast<T>(-1) == ~(static_cast<T>(0)), "");
    // This function's logic requires arithmetic right shift for signed
    // integers.  By the C++ standard (at least prior to C++20), the behavior of
    // right shift on a negative number is implementation defined, and so we
    // must assert that the compiler uses arithmetic (sign extension) right
    // shift.  It would be rare for a compiler to not use it (perhaps due to
    // non-two's complement), but in such a case this function would need a
    // different design/implementation.  Hacker's Delight 2nd edition shows an
    // alternate way to multiply signed words to get a double-word product,
    // using unsigned multiplication and post-processing.  It's slightly less
    // efficient but would likely work if you need a different implementation.
    // Note that arithmetic right shift appears to be mandated by C++20.
    static_assert((static_cast<T>(-1) >> 1) == static_cast<T>(-1), "");

    using S = T;                                           // S for signed
    using U = typename extensible_make_unsigned<T>::type;  // U for unsigned

    // for example, if U==uint64_t, w ought to == 32
    static constexpr int w = ut_numeric_limits<U>::digits / 2;
    // for example, if U==uint64_t, lowmask ought to == 0xFFFFFFFF
    static constexpr S lowmask = (static_cast<S>(1)<<w) - 1;

    // This function is based upon the identity for a signed integer x that
    // x == pow(2,w)*(x>>w) + static_cast<U>(x & lowmask)
    //   == pow(2,w)*signed_x_high + unsigned_x_low
    // --------------------------------------------------------------------

    U u0 = static_cast<U>(u & lowmask);
    U v0 = static_cast<U>(v & lowmask);
    // arithmetic (sign-extending) right shift required for next two lines
    S u1 = u >> w;
    S v1 = v >> w;

    // Bounds for u0: [0, pow(2,w) - 1]
    // Bounds for v0: [0, pow(2,w) - 1]
    // Bounds for u1: [-pow(2,w-1), pow(2,w-1) - 1]
    // Bounds for v1: [-pow(2,w-1), pow(2,w-1) - 1]

    // Range of valid values for type U: [0, pow(2,2w) - 1]
    // Range of valid values for type S: [-pow(2,2w-1), pow(2,2w-1) - 1]

    // We can see u0, v0, u1, v1 are all valid.
    // Bounds for u0*v0: [0, pow(2,2w) - pow(2,w+1) + 1]
    // Bounds for u1*v0: [-pow(2,2w-1) + pow(2,w-1),
    //                      pow(2,2w-1) - pow(2,w) - pow(2,w-1) + 1]
    // Bounds for v1*u0: same as for u1*v0
    // Bounds for u1*v1: [-pow(2,2w-2) + pow(2,w-1), pow(2,2w-2)]
    // These all fit within the valid ranges for U or S.

    // Calculate the cross products.
    U lo_lo = u0 * v0;  // bounds: [0, pow(2,2w) - pow(2,w+1) + 1]
    S hi_lo = u1 * static_cast<S>(v0);
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]
    S lo_hi = v1 * static_cast<S>(u0);
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]
    S hi_hi = u1 * v1;  // bounds: [-pow(2,2w-2) + pow(2,w-1), pow(2,2w-2)]

    U lolo1 = lo_lo >> static_cast<U>(w);  // bounds: [0, pow(2,w) - 1]

    S tmp = hi_lo + static_cast<S>(lolo1);     // won't overflow (see next line)
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w-1)]
    U tmp0 = static_cast<U>(tmp & lowmask); // bounds: [0, pow(2,w) -1]
    S tmp1 = tmp >> w;  // bounds: [-pow(2,w-1), pow(2,w-1) - 1]

    S straddle = lo_hi + static_cast<S>(tmp0); // won't overflow (see next line)
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w-1)]
    S straddle1 = straddle >> w;  // bounds: [-pow(2,w-1), pow(2,w-1) - 1]

    lowProduct = (static_cast<U>(straddle) << static_cast<U>(w)) |
                 (lo_lo & static_cast<U>(lowmask));
    S highProduct = hi_hi + straddle1 + tmp1;  // won't overflow (see next line)
    // bounds: [-pow(2,2w-2) - pow(2,w-1), pow(2,2w-2) + pow(2,w) - 2]

    // Note that the bounds given so far do not necessarily mean that a result
    // can take on any value within bounds - instead, the bounds indicate that
    // it's impossible for a result to take on a value outside of the bounds.

    // Sanity check: when we multiply two type S signed integers (with bounds
    // [-pow(2,2w-1), pow(2,2w-1)-1]), the bounds for the double-width product
    // is [-pow(2,4w-2) + pow(2,2w-1), pow(2,4w-2)].  This means the high part
    // has bounds [-pow(2,2w-2) + pow(2,-1), pow(2,2w-2)]; this range of values
    // is a subset of the bounds for highProduct, so we've passed the sanity
    // check.
    return highProduct;
  }
};



// Intended as a helper for the functions below.
// minor note: uses static member function to disallow ADL.
struct smult_to_hilo_product {
  template <typename T, typename T2> HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u, T v)
  {
    static_assert(ut_numeric_limits<T>::is_integer, "");
    static_assert(ut_numeric_limits<T>::is_signed, "");
    static_assert(ut_numeric_limits<T2>::is_integer, "");
    static_assert(ut_numeric_limits<T2>::is_signed, "");
    static_assert(ut_numeric_limits<T2>::digits >=
                  2*ut_numeric_limits<T>::digits, "");
    T2 product = static_cast<T2>(static_cast<T2>(u) * static_cast<T2>(v));

    using U1 = typename extensible_make_unsigned<T>::type;
    // for example, if U1==uint64_t, digitsU1 ought to == 64
    static constexpr int digitsU1 = ut_numeric_limits<U1>::digits;
    // for example, if U1==uint64_t, maskU1 ought to == 0xFFFFFFFFFFFFFFFF
    static constexpr T2 maskU1 = (static_cast<T2>(1) << digitsU1) - 1;

    lowProduct = static_cast<U1>(product & maskU1);
    return static_cast<T>(product >> digitsU1);
  }
};


// primary template
template <typename T>
struct impl_signed_multiply_to_hilo_product {
#ifdef HURCHALLA_COMPILE_ERROR_ON_SLOW_MATH
  // cause a compile error instead of falling back to slow multiplication.
  HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u, T v)
      = delete;
#else
  HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u, T v)
  {
      return slow_signed_multiply_to_hilo_product::call(lowProduct, u, v);
  }
#endif
};

// Note that when using these simple specializations, the generated asm from
// clang/icc/gcc is generally quite good.
// GCC for ARM seems to make the worst generated asm, but it's not so bad as to
// make inline asm seem worthwhile.
template <> struct impl_signed_multiply_to_hilo_product<std::int8_t> {
  HURCHALLA_FORCE_INLINE static
  std::int8_t call(std::uint8_t& lowProduct, std::int8_t u, std::int8_t v)
  {
    // Note we could have used 'T2 = int' since 'int' is >= 16bit.
    using T2 = std::int16_t;
    return smult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
template <> struct impl_signed_multiply_to_hilo_product<std::int16_t> {
  HURCHALLA_FORCE_INLINE static
  std::int16_t call(std::uint16_t& lowProduct, std::int16_t u, std::int16_t v)
  {
    using T2 = std::int32_t;
    return smult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
// --------------------------------------------------------------------------
template <> struct impl_signed_multiply_to_hilo_product<std::int32_t> {
  HURCHALLA_FORCE_INLINE static
  std::int32_t call(std::uint32_t& lowProduct, std::int32_t u, std::int32_t v)
  {
    using T2 = std::int64_t;
    return smult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
// Note for MSVC: 'int32_t' specializations using intrinsics don't improve the
// asm generated compared to the implementation above, and so intrinsic versions
// are not present here.  For reference, the intrinsics would have been
// __emul (for X86) and _arm_smull (for ARM32).
// --------------------------------------------------------------------------


// The following fast 'int64_t' specializations use intrinsics (MSVC), or
// compiler extensions (__int128_t on GNU compatible compilers).
// Assembly versions for x86 or ARM aren't needed - clang/gcc/icc generate
// assembly that is good enough via __int128_t, and MSVC does well using
// the intrinsics.

// MSVC + x64
#if defined(_MSC_VER) && defined(HURCHALLA_TARGET_ISA_X86_64)
template <> struct impl_signed_multiply_to_hilo_product<std::int64_t> {
  HURCHALLA_FORCE_INLINE static
  std::int64_t call(std::uint64_t& lowProduct, std::int64_t u, std::int64_t v)
  {
    using std::int64_t;  using std::uint64_t;
    int64_t highProduct;
    lowProduct = static_cast<uint64_t>(_mul128(u, v, &highProduct));
    if (HPBC_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpLo;  int64_t tmpHi;
        tmpHi = slow_signed_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
    }
    return highProduct;
  }
};
// MSVC + ARM64
#elif defined(_MSC_VER) && defined(_M_ARM64)
template <> struct impl_signed_multiply_to_hilo_product<std::int64_t> {
  HURCHALLA_FORCE_INLINE static
  std::int64_t call(std::uint64_t& lowProduct, std::int64_t u, std::int64_t v)
  {
    using std::int64_t;  using std::uint64_t;
    int64_t highProduct = __mulh(u, v);
    // avoid undefined behavior that could result if uint64_t would be promoted
    // to (signed) 'int'.  Promotion of uint64_t is HIGHLY(!) unlikely, but
    // since the C++ standard doesn't disallow 'int' type larger than 64 bit, we
    // should write code that's safe after the C++ "usual arithmetic conversion"
    // rules apply.
    using P = typename safely_promote_unsigned<uint64_t>::type;
    lowProduct = static_cast<uint64_t>(static_cast<P>(u) * static_cast<P>(v));
    if (HPBC_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpLo;  int64_t tmpHi;
        tmpHi = slow_signed_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
    }
    return highProduct;
  }
};
#elif (HURCHALLA_COMPILER_HAS_UINT128_T())
template <> struct impl_signed_multiply_to_hilo_product<std::int64_t> {
  HURCHALLA_FORCE_INLINE static
  std::int64_t call(std::uint64_t& lowProduct, std::int64_t u, std::int64_t v)
  {
    using T2 = __int128_t;
    return smult_to_hilo_product::call<decltype(u),T2>(lowProduct, u, v);
  }
};
#endif


}} // end namespace


#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
