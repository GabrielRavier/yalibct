#!/usr/bin/env bash
CC=clang \
    cmake \
        -B build/clang-glibc \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_HAS_O_SEARCH=OFF `# See https://sourceware.org/bugzilla/show_bug.cgi?id=18228` \
        -DYALIBCT_DISABLE_SNPRINTF_SIZE_LARGER_THAN_ACTUAL_BUFFER_TESTS=ON `# See https://sourceware.org/bugzilla/show_bug.cgi?id=30441` \
        -DYALIBCT_LIBC_HAS_ECVT=OFF `# See https://sourceware.org/bugzilla/show_bug.cgi?id=29556` \
&& \
    cmake --build build/clang-glibc --parallel 14
