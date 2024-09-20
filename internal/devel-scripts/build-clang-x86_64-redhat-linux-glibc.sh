#!/usr/bin/env bash
CC=clang \
    cmake \
        -B build/clang-glibc \
        -Werror=dev -Werror=deprecated --warn-uninitialized `# All of the CMake warning options that seem useful` \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_DOESNT_HAVE_O_SEARCH=ON `# See https://sourceware.org/bugzilla/show_bug.cgi?id=18228` \
        -DYALIBCT_DISABLE_SNPRINTF_SIZE_LARGER_THAN_ACTUAL_BUFFER_TESTS=ON `# See https://sourceware.org/bugzilla/show_bug.cgi?id=30441` \
        -DYALIBCT_LIBC_DOESNT_HAVE_ECVT=ON `# See https://sourceware.org/bugzilla/show_bug.cgi?id=29556` \
&& \
    cmake --build build/clang-glibc --parallel 14
