#!/usr/bin/env bash
CC=clang \
    cmake \
        -B build/clang \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_HAS_CKD_ADD=OFF \
        -DYALIBCT_LIBC_HAS_ECVT=OFF \
        -DYALIBCT_LIBC_HAS_O_SEARCH=OFF \
        -DYALIBCT_LIBC_HAS_SNPRINTF=OFF \
&& \
    cmake --build build/clang --parallel 14
