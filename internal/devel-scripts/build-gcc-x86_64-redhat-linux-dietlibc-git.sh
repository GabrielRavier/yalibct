#!/usr/bin/env bash
CC='/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/dietlibc/main-repo/bin-x86_64/diet gcc' \
    cmake \
        -B build/dietlibc-git \
        -Werror=dev -Werror=deprecated --warn-uninitialized `# All of the CMake warning options that seem useful` \
        -DCMAKE_C_FLAGS='-ggdb3 -Wno-deprecated-declarations' \
        -DYALIBCT_LIBC_DOESNT_HAVE_VSWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_SWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_VFWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_VSWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FWPRINTF=ON `# Not yet implemented` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FCVT=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_ECVT=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FESETROUND=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FEGETROUND=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_NL_ARGMAX=ON `# Not yet implemeneted` \
        -DYALIBCT_LIBC_DOESNT_HAVE_OPEN_MEMSTREAM=ON `# Not yet implemented in dietlibc (doesn't seem like they like that kind of thing...)` \
        -DYALIBCT_LIBC_DOESNT_HAVE_WCSCASECMP=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_STRUCT_STAT_ST_ATIM=ON `# Not yet implemented... seems really simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_STRUCT_STAT_ST_MTIM=ON `# Not yet implemented... seems really simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_STRUCT_STAT_ST_CTIM=ON `# Not yet implemented... seems really simple to contribute ?` \
        -DYALIBCT_LIBC_DOESNT_HAVE_O_SEARCH=ON `# Not yet implemented... might be easy to contribute whenever glibc/the Linux kernel gets around to it...` \
        -DYALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented... might be feasible to contribute ?` \
        -DYALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented... might be feasible to contribute ?` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented... might be feasible to contribute ?` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_S_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented... might be feasible to contribute ?` \
        -DYALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_PRINTF_ROUNDING_DIRECTION_TESTS=ON `# https://github.com/GabrielRavier/dietlibc-bug-reports-and-patches/blob/master/bug-report-printf-f-1-7976931348623157e-308-crashes-with-a-stack-overflow - though perhaps this part could be somewhat fixed ? idk, printing floating point is really difficult` \
        -DYALIBCT_DISABLE_PRINTF_PRECISION_TESTS=ON `# Seems like it could be elided ? Check this again later` \
        -DYALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS=ON `# https://github.com/GabrielRavier/dietlibc-bug-reports-and-patches/blob/master/bug-report-printf-g-1234567-8-does-not-print-correctly-and-fails-to-null-terminate-an-internal-buffer - seems easy to fix, look at this again later (I'd guess there's other issues, though...)` \
        -DYALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS=ON `# Not yet implemented (though only for floating point... might be able to check more selectively ? idk...)` \
        -DYALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS=ON `# Not yet implemented` \
        -DYALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented properly (doesn't parse numbers starting with 0x)` \
        -DYALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented... seems relatively simple to contribute ?` \
        -DYALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS=ON `# Not yet implemented properly (wrong on large precisions)` \
&& \
    cmake --build build/dietlibc-git --parallel 14
