#!/usr/bin/env bash
MODE=dbg CC='/home/gravier/src/libraries/standard-libraries/c/implementations/cosmopolitan/tool/scripts/cosmocc' \
    cmake \
        -B build/cosmopolitan \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_LIBC_HAS_FCVT=OFF \
        -DYALIBCT_LIBC_HAS_ECVT=OFF \
        -DYALIBCT_LIBC_HAS_VSWPRINTF=OFF \
        -DYALIBCT_LIBC_HAS_FWPRINTF=OFF \
        -DYALIBCT_LIBC_HAS_VFWPRINTF=OFF \
        -DYALIBCT_LIBC_HAS_OPEN_MEMSTREAM=OFF \
        -DYALIBCT_LIBC_HAS_TMPNAM=OFF \
        -DYALIBCT_LIBC_HAS_L_TMPNAM=OFF \
        -DYALIBCT_LIBC_HAS_SBRK=OFF \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_LC_NUMERIC_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_C_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_S_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_LIBC_HAS_SWPRINTF=OFF \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_B_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_APOSTROPHE_FLAG_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_ERRNO_TESTS=ON \
        -DYALIBCT_DISABLE_PRINTF_FIELD_LENGTH_ON_S_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS=ON \
        -DYALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS=ON \
&& \
    MODE=dbg \
        cmake --build build/cosmopolitan --parallel 14
