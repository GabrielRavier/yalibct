#!/usr/bin/env bash
# TODO: make this pretend to not have functions that we have replacements for in test-lib
CC=gcc \
    cmake \
        -B build/gcc-glibc-without-many-functions \
        -Werror=dev -Werror=deprecated --warn-uninitialized `# All of the CMake warning options that seem useful` \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_DOESNT_HAVE_CKD_ADD=ON \
        -DYALIBCT_LIBC_DOESNT_HAVE_ECVT=ON \
        -DYALIBCT_LIBC_DOESNT_HAVE_O_SEARCH=ON \
        -DYALIBCT_DISABLE_SNPRINTF_SIZE_LARGER_THAN_ACTUAL_BUFFER_TESTS=ON \
&& \
    cmake --build build/gcc-glibc-without-many-functions --parallel 14
