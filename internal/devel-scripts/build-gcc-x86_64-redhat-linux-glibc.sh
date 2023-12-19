#!/usr/bin/env bash
CC=gcc \
    cmake \
        -B build/gcc \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_HAS_CKD_ADD=OFF \
        -DYALIBCT_LIBC_HAS_ECVT=OFF \
        -DYALIBCT_LIBC_HAS_O_SEARCH=OFF \
        -DYALIBCT_DISABLE_SNPRINTF_SIZE_LARGER_THAN_ACTUAL_BUFFER_TESTS=ON \
&& \
    cmake --build build/gcc --parallel 14
