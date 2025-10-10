// Copyright (c) 2025 Jeffrey Hurchalla
// --- This file is distributed under the MIT Open Source License, as detailed
// by the file "LICENSE.TXT" in the root of this repository ---

#ifndef HURCHALLA_UTIL_IMPL_SIGNED_MULT_TO_HILO_H_INCLUDED
#define HURCHALLA_UTIL_IMPL_SIGNED_MULT_TO_HILO_H_INCLUDED


#include "hurchalla/util/traits/safely_promote_unsigned.h"
#include "hurchalla/util/traits/ut_numeric_limits.h"
#include "hurchalla/util/traits/extensible_make_unsigned.h"
#include "hurchalla/util/traits/extensible_make_signed.h"
#include "hurchalla/util/sized_uint.h"
#include "hurchalla/util/compiler_macros.h"
#include "hurchalla/util/detail/util_programming_by_contract.h"
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
//   - I adapted this function from https://web.archive.org/web/20190109005441/http://www.hackersdelight.org/hdcodetxt/muldws.c.txt
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
    static constexpr int shift = w;

    static_assert(is_valid_sized_uint<w>::value, "");
    using H = typename sized_uint<w>::type;   // unsigned half size of U
    using I = typename extensible_make_signed<H>::type;  // signed version of H

    // This function is based upon the identity for a signed integer x that
    // x == pow(2,w)*(x>>w) + static_cast<H>(x)
    //   == pow(2,w)*signed_x_high + unsigned_x_low
    // --------------------------------------------------------------------


    H u0 = static_cast<H>(u);
    H v0 = static_cast<H>(v);
    // arithmetic (sign-extending) right shift required for next two lines
    I u1 = static_cast<I>(u >> shift);
    I v1 = static_cast<I>(v >> shift);

    // Bounds for u0: [0, pow(2,w) - 1]
    // Bounds for v0: [0, pow(2,w) - 1]
    // Bounds for u1: [-pow(2,w-1), pow(2,w-1) - 1]
    // Bounds for v1: [-pow(2,w-1), pow(2,w-1) - 1]

    // Range of valid values for type U: [0, pow(2,2w) - 1]
    // Range of valid values for type S: [-pow(2,2w-1), pow(2,2w-1) - 1]

    // We can see u0, v0, u1, v1 are all valid.
    // Bounds for (U)u0 * (U)v0:  [0, pow(2,2w) - 2*pow(2,w) + 1]
    // Bounds for (S)u1 * (U)v0:  [-pow(2,2w-1) + pow(2,w-1),
    //                      pow(2,2w-1) - pow(2,w) - pow(2,w-1) + 1]
    // Bounds for (S)v1 * (U)u0:  same as for (S)u1 * (U)v0
    // Bounds for (S)u1 * (S)v1:  [-pow(2,2w-2) + pow(2,w-1), pow(2,2w-2)]
    // These all fit within the valid ranges for U or S.

#if 0
// Straightforward calculate hi_lo and lo_hi, but some compilers might create
// suboptimal machine code from this.

    U lo_lo = static_cast<U>(u0) * static_cast<U>(v0);  // bounds: [0, pow(2,2w) - 2*pow(2,w) + 1]

    S hi_lo = static_cast<S>(u1) * static_cast<S>(v0);
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]
    S lo_hi = static_cast<S>(v1) * static_cast<S>(u0);
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]

#else
// Calculate hi_lo and lo_hi by hand, to help the compiler make better choices,
// and also to correspond to inline asm later in this file.

    // Explanation of the method used in this section to do signed by unsigned
    // halfword hilo multiplication:
    // Let signbit_x = (x>=0) ? 0 : 1
    // From twos complement we know:  a signed integer x == (unsigned)x - signbit_x * R
    // So a hilo mult of  unsigned_y * signed_x == unsigned_y * ((unsigned)x - signbit_x * R)
    // == hilo_product == unsigned_y * (unsigned)x - unsigned_y * signbit_x * R
    //    hilo_product == unsigned_hilo_product - unsigned_y * signbit_x * R
    // Thus, lo_product == static_cast<H>(unsigned_hilo_product)
    //   and hi_product == static_cast<H>(unsigned_hilo_product >> shift) - unsigned_y * signbit_x

    // for efficiency we use sign masks rather than directly using the sign bits
    H sign_mask_u = static_cast<H>(u1 >> (shift - 1));
    H sign_mask_v = static_cast<H>(v1 >> (shift - 1));
    H v0_or_zero = sign_mask_u & v0;
    H u0_or_zero = sign_mask_v & u0;

    U lo_lo = static_cast<U>(u0) * static_cast<U>(v0);  // bounds: [0, pow(2,2w) - 2*pow(2,w) + 1]

    U unfinished_hi_lo = static_cast<U>(static_cast<H>(u1)) * static_cast<U>(v0);
    H hi_lo_1 = static_cast<H>(unfinished_hi_lo);
    H unfinished_hi_lo_2 = static_cast<H>(unfinished_hi_lo >> shift);

    U unfinished_lo_hi = static_cast<U>(u0) * static_cast<U>(static_cast<H>(v1));
    H lo_hi_1 = static_cast<H>(unfinished_lo_hi);
    H unfinished_lo_hi_2 = static_cast<H>(unfinished_lo_hi >> shift);

    I hi_lo_2 = static_cast<I>(unfinished_hi_lo_2 - v0_or_zero);
    I lo_hi_2 = static_cast<I>(unfinished_lo_hi_2 - u0_or_zero);

    S hi_lo = static_cast<S>((static_cast<U>(static_cast<H>(hi_lo_2)) << shift) |
                             static_cast<U>(hi_lo_1));
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]
    S lo_hi = static_cast<S>((static_cast<U>(static_cast<H>(lo_hi_2)) << shift) |
                             static_cast<U>(lo_hi_1));
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w) -pow(2,w-1) + 1]
#endif

    S hi_hi = static_cast<S>(u1) * static_cast<S>(v1);  // bounds: [-pow(2,2w-2) + pow(2,w-1), pow(2,2w-2)]

    H lo_lo_0 = static_cast<H>(lo_lo);           // bounds: [0, pow(2,w) - 1]
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);  // bounds: [0, pow(2,w) - 2]

    hi_lo = hi_lo + static_cast<S>(lo_lo_1);     // won't overflow (see next line)
    // bounds: [-pow(2,2w-1) + pow(2,w-1), pow(2,2w-1) - pow(2,w-1) - 1]

    H tmp0 = static_cast<H>(hi_lo); // bounds: [0, pow(2,w) -1]
    S tmp1 = hi_lo >> shift;  // bounds: [-pow(2,w-1), pow(2,w-1) - 1]

    lo_hi = lo_hi + static_cast<S>(tmp0); // won't overflow (see next line)
    // bounds: [-pow(2,2w-1) +pow(2,w-1), pow(2,2w-1) -pow(2,w-1)]
    S tmp2 = lo_hi >> shift;  // bounds: [-pow(2,w-1), pow(2,w-1) - 1]

    lowProduct = (static_cast<U>(lo_hi) << shift) | static_cast<U>(lo_lo_0);

    S highProduct = hi_hi + tmp2 + tmp1;  // won't overflow (see next line)
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
    static_assert(ut_numeric_limits<T2>::digits ==
                  2*ut_numeric_limits<T>::digits + 1, "");

    using U1 = typename extensible_make_unsigned<T>::type;
    using U2 = typename extensible_make_unsigned<T2>::type;
    static constexpr int digitsU1 = ut_numeric_limits<U1>::digits;
    // for example, if U1==uint64_t, digitsU1 ought to == 64
    static_assert(ut_numeric_limits<U2>::digits == 2 * digitsU1, "");

    U2 product = static_cast<U2>(static_cast<T2>(u) * static_cast<T2>(v));

    lowProduct = static_cast<U1>(product);
    U1 hiProduct = static_cast<U1>(product >> digitsU1);

    // Assert the CPU architecture uses two's complement representation for
    // signed integers.  We need two's complement because before c++20, the next
    // cast from unsigned to signed would be implementation defined.
    static_assert(static_cast<T>(-1) == ~(static_cast<T>(0)), "");

    return static_cast<T>(hiProduct);
  }
};


// primary template
template <typename T>
struct impl_signed_multiply_to_hilo_product {
  HURCHALLA_FORCE_INLINE static
  T call(typename extensible_make_unsigned<T>::type& lowProduct, T u, T v)
  {
#ifdef HURCHALLA_COMPILE_ERROR_ON_SLOW_MATH
#  ifndef HURCHALLA_TARGET_BIT_WIDTH
#    error "HURCHALLA_TARGET_BIT_WIDTH must be defined"
#  endif
      static_assert(ut_numeric_limits<T>::is_integer, "");
      static_assert(ut_numeric_limits<T>::is_signed, "");
      // Since T is signed, ut_numeric_limits<T>::digits is always
      // T's bit width minus one.  E.g. int64_t has 63 digits.
      static constexpr int T_bit_width = ut_numeric_limits<T>::digits + 1;
      static_assert(T_bit_width > HURCHALLA_TARGET_BIT_WIDTH,
        "For T with bit width <= the native bit width, it makes sense to issue the compile error requested by HURCHALLA_COMPILE_ERROR_ON_SLOW_MATH.  Note for T larger than the native bit depth, there would be no way to avoid using a slow math routine.");
#endif
      return slow_signed_multiply_to_hilo_product::call(lowProduct, u, v);
  }
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
    if (HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpLo;  int64_t tmpHi;
        tmpHi = slow_signed_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_UTIL_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
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
    if (HPBC_UTIL_POSTCONDITION3_MACRO_IS_ACTIVE) {
        uint64_t tmpLo;  int64_t tmpHi;
        tmpHi = slow_signed_multiply_to_hilo_product::call(tmpLo, u, v);
        HPBC_UTIL_POSTCONDITION3(highProduct == tmpHi && lowProduct == tmpLo);
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




// Inline asm summary/conclusion from these ARM64 timings - for ARM64 we should
// enable the all-asm version, for both gcc and clang.  A side benefit is the
// compiler (usually gcc) is less likely to have the bad luck cases where it
// makes bad decisions and produces terrible machine code, if we use all-asm.
//
// -- Benchmark Timings --
// Monthalf two pow array:
// gcc with no-asm square: no-asm 1.2126  partial-asm 1.2040  *all-asm 1.2076
// gcc with all-asm square: no-asm 1.0577  partial-asm 1.0649  *all-asm 1.0600
// clang with no-asm square: no-asm 1.0553  partial-asm 1.0553  *all-asm 1.0551
// clang with all-asm square: no-asm 1.0172  partial-asm 1.0163  *all-asm 1.0168
//
// Monthalf two pow scalar:
// gcc with no-asm square: no-asm 2.5643  partial-asm 2.5405  *all-asm 2.5426
// gcc with all-asm square: no-asm 2.5290  partial-asm 2.5122  *all-asm 2.4986
// clang with no-asm square: no-asm 2.3892  partial-asm 2.3837  *all-asm 2.3717
// clang with all-asm square: no-asm 2.3814  partial-asm 2.3793  *all-asm 2.3589


#if (HURCHALLA_COMPILER_HAS_UINT128_T()) && \
    defined(HURCHALLA_TARGET_ISA_ARM_64) && \
    defined(HURCHALLA_ALLOW_INLINE_ASM_MULTIPLY_TO_HILO)

template <> struct impl_signed_multiply_to_hilo_product<__int128_t> {
  HURCHALLA_FORCE_INLINE static
  __int128_t call(__uint128_t& lowProduct, __int128_t u, __int128_t v)
  {
    using T = __int128_t;

    // Assert the CPU uses two's complement, and arithmetic right shift
    static_assert(static_cast<T>(-1) == ~(static_cast<T>(0)), "");
    static_assert((static_cast<T>(-1) >> 1) == static_cast<T>(-1), "");

    using S = T;  // S for signed
    using U = typename extensible_make_unsigned<T>::type;  // U for unsigned

    static constexpr int digitsU = ut_numeric_limits<U>::digits;

    static_assert(is_valid_sized_uint<digitsU/2>::value, "");
    using H = typename sized_uint<digitsU/2>::type;   // unsigned half size of U
    using I = typename extensible_make_signed<H>::type;  // signed version of H

    static constexpr int shift = digitsU / 2;

    H u0 = static_cast<H>(u);
    // arithmetic (sign-extending) right shift required
    I u1 = static_cast<I>(u >> shift);
    H v0 = static_cast<H>(v);
    I v1 = static_cast<I>(v >> shift);

# if 1
// all-asm

    H lo_lo_0, lo_lo_1, hi_lo_1, lo_hi_1, hi_lo_2, lo_hi_2;
    __asm__ (
             "mul %[lo_lo_0], %[u0], %[v0] \n\t"
             "umulh %[lo_lo_1], %[u0], %[v0] \n\t"
             "mul %[hi_lo_1], %[u1], %[v0] \n\t"
             "mul %[lo_hi_1], %[u0], %[v1] \n\t"
             "umulh %[hi_lo_2], %[u1], %[v0] \n\t"
             "umulh %[lo_hi_2], %[u0], %[v1] \n\t"
             "and %[v0], %[v0], %[u1], asr #63 \n\t"   /* v0 = v0_or_zero,  2 cycles */
             "and %[u0], %[u0], %[v1], asr #63 \n\t"   /* u0 = u0_or_zero,  2 cycles */
             "sub %[hi_lo_2], %[hi_lo_2], %[v0] \n\t"  /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "sub %[lo_hi_2], %[lo_hi_2], %[u0] \n\t"  /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "mul %[v0], %[u1], %[v1] \n\t"            /* v0 = hi_hi_2 */
             "smulh %[u1], %[u1], %[v1] \n\t"          /* u1 = hi_hi_3 */
             "asr %[u0], %[hi_lo_2], #63 \n\t"         /* u0 = sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "asr %[v1], %[lo_hi_2], #63 \n\t"         /* v1 = sign_extension_lo_hi_2 = lo_hi_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[lo_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[lo_hi_2] \n\t"
             "adc %[u0], %[u0], %[v1] \n\t"            /* result3 = carry + sign_extension_hi_lo_2 + sign_extension_lo_hi_2 */
             "adds %[hi_lo_1], %[lo_hi_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[v0], %[hi_lo_2] \n\t"
             "adc %[u0], %[u1], %[u0] \n\t"            /* result3 = hi_hi_3 + result3 + carry */
             : [u0]"+&r"(u0), [v0]"+&r"(v0),
               [lo_lo_0]"=&r"(lo_lo_0), [lo_lo_1]"=&r"(lo_lo_1),
               [u1]"+&r"(u1), [hi_lo_1]"=&r"(hi_lo_1),
               [v1]"+&r"(v1), [lo_hi_1]"=&r"(lo_hi_1),
               [hi_lo_2]"=&r"(hi_lo_2), [lo_hi_2]"=&r"(lo_hi_2)
             :
             : "cc");
    lowProduct = (static_cast<U>(hi_lo_1) << shift) | static_cast<U>(lo_lo_0);
    S highProduct = static_cast<S>((static_cast<U>(u0) << shift) |
                                   (static_cast<U>(hi_lo_2)));

# else
// In limited tests benchmarking montgomery two_pow, this partial-asm version
// tended to be a little slower than the full-asm version.  Thus we disable
// this version.

    H sign_mask_u = static_cast<H>(u1 >> (shift - 1));
    H sign_mask_v = static_cast<H>(v1 >> (shift - 1));
    H v0_or_zero = sign_mask_u & v0;
    H u0_or_zero = sign_mask_v & u0;

    U lo_lo = static_cast<U>(u0) * static_cast<U>(v0);

    U unfinished_hi_lo = static_cast<U>(static_cast<H>(u1)) * static_cast<U>(v0);
    H hi_lo_1 = static_cast<H>(unfinished_hi_lo);
    H unfinished_hi_lo_2 = static_cast<H>(unfinished_hi_lo >> shift);

    U unfinished_lo_hi = static_cast<U>(u0) * static_cast<U>(static_cast<H>(v1));
    H lo_hi_1 = static_cast<H>(unfinished_lo_hi);
    H unfinished_lo_hi_2 = static_cast<H>(unfinished_lo_hi >> shift);

#  if 0
// no-asm   get rid of this - the slow_mult at top is the same

    I hi_lo_2 = static_cast<I>(unfinished_hi_lo_2 - v0_or_zero);
    I lo_hi_2 = static_cast<I>(unfinished_lo_hi_2 - u0_or_zero);

    U hi_lo = (static_cast<U>(static_cast<H>(hi_lo_2)) << shift) | static_cast<U>(hi_lo_1);
    U lo_hi = (static_cast<U>(static_cast<H>(lo_hi_2)) << shift) | static_cast<U>(lo_hi_1);

    S hi_hi = static_cast<S>(u1) * static_cast<S>(v1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    hi_lo = hi_lo + lo_lo_1;
    H tmp0 = static_cast<H>(hi_lo);
    S tmp1 = static_cast<S>(hi_lo) >> shift;

    lo_hi = lo_hi + tmp0;
    S tmp2 = static_cast<S>(lo_hi) >> shift;

    lowProduct = (lo_hi << shift) | lo_lo_0;
    S highProduct = hi_hi + tmp2 + tmp1;

#  else
// partial-asm

    S hi_hi = static_cast<S>(u1) * static_cast<S>(v1);

    H lo_lo_0 = static_cast<H>(lo_lo);
    H lo_lo_1 = static_cast<H>(lo_lo >> shift);

    H hi_hi_2 = static_cast<H>(hi_hi);
    H hi_hi_3 = static_cast<H>(hi_hi >> shift);

    H tmp3A = v0_or_zero;
    H tmp3B = u0_or_zero;
    H hi_lo_2 = unfinished_hi_lo_2;
    H lo_hi_2 = unfinished_lo_hi_2;

    __asm__ ("sub %[hi_lo_2], %[hi_lo_2], %[tmp3A] \n\t"   /* hi_lo_2 = unfinished_hi_lo_2 - v0_or_zero */
             "sub %[lo_hi_2], %[lo_hi_2], %[tmp3B] \n\t"   /* lo_hi_2 = unfinished_lo_hi_2 - u0_or_zero */
             "asr %[tmp3A], %[hi_lo_2], #63 \n\t"          /* sign_extension_hi_lo_2 = hi_lo_2 >> 63 */
             "asr %[tmp3B], %[lo_hi_2], #63 \n\t"          /* sign_extension_lo_hi_2 = lo_hi_2 >> 63 */
             "adds %[hi_lo_1], %[hi_lo_1], %[lo_hi_1] \n\t"
             "adcs %[hi_lo_2], %[hi_lo_2], %[lo_hi_2] \n\t"
             "adc %[tmp3A], %[tmp3A], %[tmp3B] \n\t"
             "adds %[hi_lo_1], %[lo_lo_1], %[hi_lo_1] \n\t"
             "adcs %[hi_lo_2], %[hi_hi_2], %[hi_lo_2] \n\t"
             "adc %[tmp3A], %[hi_hi_3], %[tmp3A] \n\t"
             : [hi_lo_2]"+&r"(hi_lo_2), [tmp3A]"+&r"(tmp3A),
               [lo_hi_2]"+&r"(lo_hi_2), [tmp3B]"+&r"(tmp3B),
               [hi_lo_1]"+&r"(hi_lo_1)
             : [lo_hi_1]"r"(lo_hi_1), [lo_lo_1]"r"(lo_lo_1),
               [hi_hi_2]"r"(hi_hi_2), [hi_hi_3]"r"(hi_hi_3)
             : "cc");
    lowProduct = (static_cast<U>(hi_lo_1) << shift) | static_cast<U>(lo_lo_0);
    S highProduct = static_cast<S>((static_cast<U>(tmp3A) << shift) |
                                   (static_cast<U>(hi_lo_2)));
#  endif
# endif


    if (HPBC_UTIL_POSTCONDITION2_MACRO_IS_ACTIVE) {
        U low2;
        S high2 = slow_signed_multiply_to_hilo_product::call(low2, u, v);
        HPBC_UTIL_POSTCONDITION2(lowProduct == low2 && highProduct == high2);
    }
    return highProduct;
  }
};

#endif



}} // end namespace


#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
