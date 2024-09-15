#!/usr/bin/env bash
MODE=dbg CC='/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/cosmopolitan-libc/.cosmocc/3.8.0/bin/cosmocc' \
    cmake \
        -B build/cosmopolitan-latest-release \
        -DCMAKE_C_FLAGS='-ggdb3 -mdbg' \
        -DYALIBCT_LIBC_HAS_VSWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_FWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_VFWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_OPEN_MEMSTREAM=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_SBRK=OFF `# Not yet implemented` \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON `# cosmopolitan is like musl and doesn't have a bunch of locales and just pretends it does, so we can't check for them at runtime` \
        -DYALIBCT_LIBC_HAS_SWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_B_CONVERSION_SPECIFIER_TESTS=ON `# (PR link TODO)` \
        -DYALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1286, https://github.com/jart/cosmopolitan/pull/1287, (PR link TODO)` \
        -DYALIBCT_DISABLE_PRINTF_APOSTROPHE_FLAG_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1287` \
        -DYALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1273 and https://github.com/jart/cosmopolitan/pull/1283` \
        -DYALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_FCVT=OFF `# See https://github.com/jart/cosmopolitan/pull/1282` \
        -DYALIBCT_LIBC_HAS_ECVT=OFF `# See https://github.com/jart/cosmopolitan/pull/1282` \
        -DYALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1282` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1276` \
        -DYALIBCT_DISABLE_PRINTF_ERRNO_TESTS=ON `# See https://github.com/jart/cosmopolitan/pull/1275` \
&& \
    MODE=dbg \
        cmake --build build/cosmopolitan-latest-release --parallel 14
