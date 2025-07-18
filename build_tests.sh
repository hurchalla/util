#!/usr/bin/env bash

# --- This file is distributed under the MIT Open Source License, as detailed
# by the file "LICENSE.TXT" in the root of this repository ---


# Description of this script -----------
# This is my working convenience script for invoking the testing builds and then
# running the tests.
# The syntax is 
# ./build_tests [-c<compiler_name>] [-r] [-s] [-m<Release|Debug>] [-l<standard_library_name>]
#
# -c allows you to select the compiler, rather than using the default.
# -r specifies to run all tests after the build.  Without -r, no tests will run.
# -s applies to the conditional_select function, and specifies that you wish to
#    compile its default version with optimizations that use a series of bitwise
#    operations, instead of operations that typically would compile to
#    conditional select or conditional move instructions.
# -m allows you to choose between Release and Debug build configuration, rather
#    than using the default.
# -l allows you to choose between either libstdc++ or libc++ when using clang.
#
# Currently it supports clang, gcc, and icc but you'll need to customize the
# section under "#Compiler commands" to match the compilers on your system.  The
# script doesn't do anything fancy like auto detection or anything like that.
#
# Some examples of using the script:
# ./build_tests.sh -cicc -r -mrelease
# [The above line uses the intel compiler (icc) in a release config, and
# runs the tests after they're built.  No inline assembly is used in the build.]
#
# ./build_tests.sh -cclang -r
# [The above line uses the clang compiler in the default config (Debug), and
# runs the tests after they're built.]
#
# ./build_tests.sh -cgcc -mdebug
# [The above line uses the gcc compiler in a debug config.  Although it builds
# the tests, it does not run any of them.  No inline assembly is used.]



# ------------ How to get/install the various compilers on Linux --------------

# how to get gcc/g++ version 10, on ubuntu linux
# ----------------

# sudo add-apt-repository ppa:ubuntu-toolchain-r/test
# sudo apt-get update
# sudo apt install gcc-10
# sudo apt install g++-10
# sudo apt update
# sudo apt upgrade
# sudo apt install build-essential
# sudo apt update
# sudo apt upgrade
# sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10
# sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10
#  [ you may need to repeat the above two lines for any older gcc/g++ on your system ]
# sudo update-alternatives --config gcc
#  [choose the version of gcc you want as default]
# sudo update-alternatives --config g++
#  [choose the version of g++ you want as default]

#info taken from:
# https://askubuntu.com/questions/1192955/how-to-install-g-10-on-ubuntu-18-04
# https://www.fosslinux.com/39386/how-to-install-multiple-versions-of-gcc-and-g-on-ubuntu-20-04.htm
# https://linuxconfig.org/how-to-switch-between-multiple-gcc-and-g-compiler-versions-on-ubuntu-20-04-lts-focal-fossa



# how to get the latest version of clang (10 at the time of writing this)
# ----------------
# sudo apt update
# sudo apt upgrade
# bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
#   [The above line is what is recommended at https://apt.llvm.org/]
# sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-10 10
# sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-10 10
#  [ you may need to repeat the above two lines for any older clang/clang++ on
#    your system ]



# You get the Intel C++ compiler by downloading/installing Parallel Studio XE.
# It is free to install and use for open source projects.  See-
#   https://software.intel.com/content/www/us/en/develop/articles/qualify-for-free-software.html#opensourcecontributor
# After installing intel compiler on Ubuntu (I used Parallel Studio XE 2020
# Update1 Professional Edition), I did:
# ----------------
# sudo apt update
# sudo apt upgrade
# source /opt/intel/bin/compilervars.sh intel64
#   [ Whenever you wish to compile with Intel C++ (icc/icpc), you will need to
#     first run this compilervars script.  Probably you could set up your system
#     so that compilervars.sh runs at boot time and so that its effects apply to
#     any shell you start, but I would recommend *against* doing so.  The
#     compilervars script alters the PATH environment variable and puts some
#     intel bin directories as the first entries in the PATH.  Unfortunately
#     those bin directories contain clang, clang++, gcc, and g++ symbolic links
#     to (evidently) binaries that the Parallel Studio XE installer put on the
#     system.  This seems to me to be a bad choice by intel, because after
#     sourcing the compilervars script, the commands gcc, g++, clang, clang++ no
#     longer refer to any of the gcc or clang compilers that you may have
#     installed on your system.  Instead these commands will refer to the intel-
#     provided gcc/clang compilers, which will likely be a completely different
#     version of those compilers from what you expect or want.
#     The solution to this problem I am using is to start a subshell, then
#     source compilervars.sh, then perform the compilation with icc, then exit
#     back to the parent shell.  The commands I use to do this are:
#          bash
#          source /opt/intel/bin/compilervars.sh intel64
#          ... run icc, or run cmake with icc, etc ...
#          exit
#     Any running script is already in a subshell (unless you use source or . ),
#     so creating another subshell isn't necessary.  This present script here
#     assumes it is already in a subshell, and just sources compilervars.sh if
#     icc is chosen. ]
# sudo update-alternatives --config gcc
#   [ The above problems with compilervars.sh weren't my only issues.  For
#     whatever reason, I have experienced that the intel version of gcc/g++ is
#     not always the one used after I have sourced compilervars.sh, presumably
#     because the script altered the PATH variable to place the intel compiler
#     directories *after* usr/bin where the normal gcc/g++ on my system lies.
#     This is a problem for icc, because Intel C++ uses and needs gcc's system
#     includes and standard library: see  https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compatibility-and-portability/gcc-compatibility-and-interoperability.html
#     If icc mistakenly ends up using a version of gcc (for example, the very
#     latest version of gcc) that is not compatible with icc, then icc may fail
#     with compile errors that make it pretty obvious it is using your system
#     gcc rather than the intel-placed gcc.  Therefore, the default version of
#     gcc that is invoked by the commands 'gcc' and 'g++' *MUST NOT* be more
#     recent than your installation of Intel C++ supports.  I don't know what
#     the most recent gcc version is that icc from Parallel Studio XE 2020 (icc
#     v19.1) supports, but I know that when update-alternatives was set to
#     gcc-10 I had errors when I tried to compile icc with -std=c++17, and I
#     know that when I set update-alternatives to gcc-7 all the errors with
#     -std=c++17 went away.  Everything has worked fine for me with IntelC++
#     after I set the default 'gcc' and 'g++' to be version 7.  I would guess
#     that none of this is an issue if/when compilervars.sh places the intel
#     directories as the first entries in PATH, but I don't know.  I'm not sure
#     why compilervars.sh seems to be inconsistent about entry placement in PATH
#     for me.  Regardless, for whatever version of gcc that you are currently
#     using as the system default (via update-alteratives), you'll have to try
#     compiling with icc using -std=c++17 in order to know if your default gcc
#     version is making icc vulnerable to compile errors.  If it's a problem,
#     then update-alternatives should work around it, or I suppose you could
#     figure out how to make sure compilervars.sh always consistently places
#     the intel directories first in PATH. ]
# sudo update-alternatives --config g++
#   [ You *MUST* set the default g++ to be the same version as you just chose
#     for update-alternatives --config gcc.  For me, that meant g++-7. ]
#
# [ I didn't need to run "update-alternatives --config icc" (or icpc), but if
#   you install multiple versions of icc, then I expect you'd need to run
#   update-alternatives for both icc and icpc. ]



while getopts ":m:l:c:h-:rs" opt; do
  case $opt in
    h)
      ;&
    -)
      echo "Usage: build_tests [-c<compiler_name>] [-r] [-s] [-m<Release|Debug>] [-l<standard_library_name>]" >&2
      exit 1
      ;;
    c)
      compiler=$OPTARG
      ;;
    m)
      mode=$OPTARG
      ;;
    l)
      library=$OPTARG
      ;;
    r)
      run_tests=true
      ;;
    s)
      test_avoid_cselect="-DHURCHALLA_AVOID_CSELECT=1"
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

if [ -z "$compiler" ]; then
  compiler=clang
fi
if [ -z "$mode" ]; then
  mode=Debug
fi

if [ -n "$library" ]; then
  cpp_stdlib="-stdlib=$library"
fi



# Compiler commands
if [ "${compiler,,}" = "gcc" ] || [ "${compiler,,}" = "g++" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=g++
  cmake_c_compiler=-DCMAKE_C_COMPILER=gcc
  compiler_name=gcc
elif [ "${compiler,,}" = "gcc-7" ] || [ "${compiler,,}" = "g++-7" ] ||
     [ "${compiler,,}" = "gcc7" ] || [ "${compiler,,}" = "g++7" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=g++-7
  cmake_c_compiler=-DCMAKE_C_COMPILER=gcc-7
  compiler_name=gcc7
elif [ "${compiler,,}" = "gcc-10" ] || [ "${compiler,,}" = "g++-10" ] ||
     [ "${compiler,,}" = "gcc10" ] || [ "${compiler,,}" = "g++10" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=g++-10
  cmake_c_compiler=-DCMAKE_C_COMPILER=gcc-10
  compiler_name=gcc10
elif [ "${compiler,,}" = "gcc-13" ] || [ "${compiler,,}" = "g++-13" ] ||
     [ "${compiler,,}" = "gcc13" ] || [ "${compiler,,}" = "g++13" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=g++-13
  cmake_c_compiler=-DCMAKE_C_COMPILER=gcc-13
  compiler_name=gcc13
elif [ "${compiler,,}" = "clang" ] || [ "${compiler,,}" = "clang++" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=clang++
  cmake_c_compiler=-DCMAKE_C_COMPILER=clang
  compiler_name=clang
elif [ "${compiler,,}" = "clang-3" ] || [ "${compiler,,}" = "clang++-3" ] ||
     [ "${compiler,,}" = "clang3" ] || [ "${compiler,,}" = "clang++3" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=clang++-3.9
  cmake_c_compiler=-DCMAKE_C_COMPILER=clang-3.9
  compiler_name=clang3
elif [ "${compiler,,}" = "clang-6" ] || [ "${compiler,,}" = "clang++-6" ] ||
     [ "${compiler,,}" = "clang6" ] || [ "${compiler,,}" = "clang++6" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=clang++-6.0
  cmake_c_compiler=-DCMAKE_C_COMPILER=clang-6.0
  compiler_name=clang6
elif [ "${compiler,,}" = "clang-10" ] || [ "${compiler,,}" = "clang++-10" ] ||
     [ "${compiler,,}" = "clang10" ] || [ "${compiler,,}" = "clang++10" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=clang++-10
  cmake_c_compiler=-DCMAKE_C_COMPILER=clang-10
  compiler_name=clang10
elif [ "${compiler,,}" = "clang-18" ] || [ "${compiler,,}" = "clang++-18" ] ||
     [ "${compiler,,}" = "clang18" ] || [ "${compiler,,}" = "clang++18" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=clang++-18
  cmake_c_compiler=-DCMAKE_C_COMPILER=clang-18
  compiler_name=clang18
elif [ "${compiler,,}" = "icc" ] || [ "${compiler,,}" = "icpc" ]; then
  cmake_cpp_compiler=-DCMAKE_CXX_COMPILER=icpc
  cmake_c_compiler=-DCMAKE_C_COMPILER=icc
  compiler_name=icc
  source /opt/intel/bin/compilervars.sh intel64
elif [ -n "$compiler" ]; then
  echo "Invalid argument for option -c: $compiler"
  exit 1
fi

echo Using compiler $compiler_name ...
echo Using build mode $mode ...


# Note: Gcc static analyzer was added in gcc version 10

#static analyzers
#-----
if [ "$compiler_name" = "gcc" ]; then
#  gcc_static_analysis="-fanalyzer"
# !!! g++-10 with -fanalyzer locks the system on gtest !!!
# g++-10 with -Wanalyzer-too-complex had compiler errors on gtest

  : # do nothing, at least for now

# note that clang-tidy includes the clang static analyzer
elif [ "$compiler_name" = "clang" ]; then
#  clang_static_analysis=(-DCMAKE_CXX_CLANG_TIDY="clang-tidy;-extra-arg=-Wno-unknown-warning-option;-checks=*,clang-analyzer-*")
  : # do nothing, at least for now
fi
#-analyzer-checker=core
#-analyzer-checker=cpp
#-analyzer-checker=unix
#-analyzer-checker=deadcode


#undefined behavior sanitizers
#-----------------------------
# note according to https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html:
# "[The] UndefinedBehaviorSanitizer ... test suite is integrated into the CMake
# build and can be run with check-ubsan command."

if [ "$compiler_name" = "gcc" ]; then
  if [[ $(uname -m) == 'arm64' ]]; then
    # At the time of this writing, gcc does not seem to have implemented sanitizers
    # (at least not ubsan) for Silicon MacOS.  I get link errors if compiling
    # with them on mac.  See  https://github.com/orgs/Homebrew/discussions/3384
    # https://github.com/orgs/Homebrew/discussions/3260
    # https://stackoverflow.com/questions/65259300/detect-apple-silicon-from-command-line

    : # do nothing, at least for now
  else
    gcc_ubsan="-fsanitize=undefined -fno-sanitize-recover \
             -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow"
  fi
elif [ "$compiler_name" = "clang" ] && [[ $compiler_version -ge 6 ]]; then
  # clang6 doesn't support -fsanitize=implicit-conversion.  Clang10 does support
  # it.  I don't know if clang7,8,9 support it.
  if [[ $compiler_version -ge 10 ]]; then
    clang_ubsan="-fsanitize=undefined -fsanitize=nullability -fsanitize=bounds \
                 -fsanitize=float-divide-by-zero -fsanitize=implicit-conversion"
  else
    clang_ubsan="-fsanitize=undefined -fsanitize=nullability -fsanitize=bounds \
                 -fsanitize=float-divide-by-zero"
  fi
  # The next line in a perfect world wouldn't be needed, but for some versions
  # of clang (clang 10 for me), the linker doesn't find __muloti4 when using the
  # undefined behavior sanitizers.  __muloti4 is defined in compiler-rt.
  # See https://bugs.llvm.org/show_bug.cgi?id=16404
  clang_ubsan_link_flags="-rtlib=compiler-rt -lgcc_s"
fi


#address sanitizers
#------------------
clang_asan=""
gcc_asan="-fsanitize=address"
# clang -fsanitize=address -O1 -fno-omit-frame-pointer -g   tests/use-after-free.c
# clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer example_UseAfterFree.cc

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
# but modern cmake would be to use target_compile_options

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

#target_link_libraries(MyTarget
#  -fsanitize=address
#)

# prefer target_link_options instead?

#target_compile_options(your_target
#  PRIVATE
#  $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:Debug>:${CMAKE_CXX_FLAGS_RELEASE}>>

# target_compile_options(tgt PRIVATE "/MD$<$<CONFIG:Debug>:d>")


# target_compile_options(foo PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
# target_compile_options(foo PUBLIC "$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")

# target_compile_options(MyLib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)

# target_compile_options(MyLib PUBLIC "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/MDd>")


# if(MSVC)
#     add_compile_options("/W4" "$<$<CONFIG:RELEASE>:/O2>")
# else()
#     add_compile_options("-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>")
#     if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#         add_compile_options("-stdlib=libc++")
#     else()
#         # nothing special for gcc at the moment
#     endif()
# endif()


#LeakSanitizer (LSan)
#ThreadSanitizer (TSan)
#MemorySanitizer (MSan)

#modes
# 1. Asan+UBsan+Lsan
# 2. Tsan
# 3. Msan
# 4. Valgrind (can't be used with other sanitizers)

# a run of "splint" and/or cppcheck
# cpplint
# include what you use (iwyu), and lwyu (link what you use)
# Clang-Tidy
# CppCoreCheck

# doxygen

# <LANG>_CLANG_TIDY: CMake 3.6+
# <LANG>_CPPCHECK
# <LANG>_CPPLINT
# <LANG>_INCLUDE_WHAT_YOU_USE
# LINK_WHAT_YOU_USE

# fuzz testing

# valgrind/purity

# code coverage tools - gcov



exit_on_failure () {
  if [ $? -ne 0 ]; then
    exit 1
  fi
}


# https://stackoverflow.com/questions/59895/how-to-get-the-source-directory-of-a-bash-script-from-within-the-script-itself
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"


# We don't usually want to force c++11 standard, since that requires that we use
# an older version of googletest that was the final version to support C++11.
# That googletest version's CMakeLists.txt isn't updated for more recent CMake
# versions, and so we have to work around CMake deprecation warnings and errors
# (you can see how this is done in FetchGoogleTest.cmake), which isn't a good
# normal practice to do.  It also excludes test_BitpackedUintVector from the
# unit testing, since BitpackedUintVector needs C++14 support.  However, it is
# good to prove that the library (mostly) is C++11 compatible from time to time.
# To force c+11, change the line below to true.
force_cpp11=false


if [ "$force_cpp11" = true ]; then
  force_cpp11_testing="ON"
else
  force_cpp11_testing="OFF"
fi


if [ "${mode,,}" = "release" ]; then
    pushd script_dir > /dev/null 2>&1
    build_dir=build/release_$compiler_name
    mkdir -p $build_dir
    cmake -S. -B./$build_dir -DTEST_HURCHALLA_UTIL=ON \
            -DFORCE_TEST_HURCHALLA_CPP11_STANDARD=$force_cpp11_testing \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_CXX_FLAGS="$cpp_stdlib  $test_avoid_cselect \
            $gcc_static_analysis"  "${clang_static_analysis[@]}" \
            $cmake_cpp_compiler $cmake_c_compiler
    exit_on_failure
    cmake --build ./$build_dir --config Release
    exit_on_failure
    popd > /dev/null 2>&1
elif [ "${mode,,}" = "debug" ]; then
    pushd script_dir > /dev/null 2>&1
    build_dir=build/debug_$compiler_name
    mkdir -p $build_dir
    cmake -S. -B./$build_dir -DTEST_HURCHALLA_UTIL=ON \
            -DFORCE_TEST_HURCHALLA_CPP11_STANDARD=$force_cpp11_testing \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_EXE_LINKER_FLAGS="$clang_ubsan_link_flags" \
            -DCMAKE_CXX_FLAGS="$cpp_stdlib  $clang_ubsan  \
            $gcc_ubsan  $test_avoid_cselect \
            $gcc_static_analysis"  "${clang_static_analysis[@]}" \
            $cmake_cpp_compiler $cmake_c_compiler
    exit_on_failure
    cmake --build ./$build_dir --config Debug
    exit_on_failure
    popd > /dev/null 2>&1
else
    echo "Invalid argument for option -m: $mode"
    exit 1
fi


# -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
# cmake  -S.  -B./build_tmp  -DCMAKE_CXX_FLAGS="-std=c++17"  -DTEST_HURCHALLA_UTIL=ON  -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_CXX_COMPILER=icpc  -DCMAKE_C_COMPILER=icc
# cmake --build ./build_tmp --config Debug


if [ "$run_tests" = true ]; then
  ./$build_dir/test_ndebug_programming_by_contract --gtest_break_on_failure
  exit_on_failure
  ./$build_dir/test_programming_by_contract --gtest_break_on_failure
  exit_on_failure
  ./$build_dir/test_hurchalla_util --gtest_break_on_failure
  exit_on_failure
  if ! [ "$force_cpp11" = true ]; then
    ./$build_dir/test_hurchalla_util_cpp14 --gtest_break_on_failure
    exit_on_failure
  fi
fi
