#!/usr/bin/env bash
MODE=dbg CC='/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/cosmopolitan-libc/built-cosmocc-toolchain/bin/cosmocc' \
    cmake \
        -B build/cosmopolitan \
        -DCMAKE_C_FLAGS='-ggdb3 -mdbg' \
        -DYALIBCT_LIBC_HAS_VSWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_FWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_VFWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_OPEN_MEMSTREAM=OFF `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_SBRK=OFF `# Not yet implemented` \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON `# cosmopolitan is like musl and doesn't have a bunch of locales and just pretends it does, so we can't check for them at runtime` \
        -DYALIBCT_LIBC_HAS_SWPRINTF=OFF `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to diverge from the standard on this` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to diverge from the standard on this` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to diverge from the standard on this` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to diverge from the standard on this` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_B_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented (seems simple enough to do given what we already have)` \
        -DYALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS=ON `# Buggy (investigate later)` \
        -DYALIBCT_DISABLE_PRINTF_APOSTROPHE_FLAG_TESTS=ON `# Not yet implemented (seems simple enough)` \
        -DYALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS=ON `# Buggy (investigate later)` \
        -DYALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_HAS_FCVT=OFF `# Buggy (investigate later)` \
        -DYALIBCT_LIBC_HAS_ECVT=OFF `# Buggy (investigate later)` \
        -DYALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS=ON `# Buggy (investigate later)` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented (seems simple enough)` \
        -DYALIBCT_DISABLE_PRINTF_ERRNO_TESTS=ON `# Something is quite broken w.r.t. this...` \
&& \
    MODE=dbg \
        cmake --build build/cosmopolitan --parallel 14
