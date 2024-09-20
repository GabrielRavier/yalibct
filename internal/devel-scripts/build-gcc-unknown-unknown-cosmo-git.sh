#!/usr/bin/env bash
MODE=dbg CC='/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/cosmopolitan-libc/built-cosmocc-toolchain/bin/cosmocc' \
    cmake \
        -B build/cosmopolitan-git \
        -Werror=dev -Werror=deprecated --warn-uninitialized `# All of the CMake warning options that seem useful` \
        -DCMAKE_C_FLAGS='-ggdb3 -mdbg' \
        -DYALIBCT_LIBC_DOESNT_HAVE_VSWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_VFWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_OPEN_MEMSTREAM=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_SBRK=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON `# cosmopolitan is like musl and doesn't have a bunch of locales and just pretends it does, so we can't check for them at runtime` \
        -DYALIBCT_LIBC_DOESNT_HAVE_SWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS=ON `# cosmopolitan has deliberately decided to deviate from the standard on this, see https://github.com/jart/cosmopolitan/pull/1271` \
        -DYALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
&& \
    MODE=dbg \
        cmake --build build/cosmopolitan-git --parallel 14
