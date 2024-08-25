#!/usr/bin/env bash
CC=musl-gcc \
    cmake \
        -B build/musl-system \
        -DCMAKE_C_FLAGS='-ggdb3' \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON `# musl doesn't have a bunch of locales and just pretends it does, so we can't check for them at runtime` \
        -DYALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_B_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_FCVT=OFF `# See https://www.openwall.com/lists/musl/2022/09/06/10` \
        -DYALIBCT_LIBC_HAS_ECVT=OFF `# See https://www.openwall.com/lists/musl/2022/09/06/4` \
&& \
    cmake --build build/musl-system --parallel 14
