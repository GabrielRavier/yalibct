#!/usr/bin/env bash
CC=/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/musl/musl-install-prefix/bin/musl-gcc \
    cmake \
        -B build/musl-git \
        -DCMAKE_C_FLAGS='-ggdb3 -static' \
        -DYALIBCT_LIBC_HAS_CKD_ADD=OFF \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON \
        -DYALIBCT_LIBC_HAS_FCVT=OFF \
        -DYALIBCT_LIBC_HAS_ECVT=OFF \
&& \
    cmake --build build/musl-git --parallel 14
