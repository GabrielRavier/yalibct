#!/usr/bin/env bash
CC=/home/gravier/src/libraries/programming/languages-implementations/standard-libraries/c/implementations/musl-libc/local-install-prefix/bin/musl-gcc \
    cmake \
        -B build/musl-git \
        -Werror=dev -Werror=deprecated --warn-uninitialized `# All of the CMake warning options that seem useful` \
        -DCMAKE_C_FLAGS='-ggdb3 -static' \
        -DYALIBCT_DISABLE_RARE_LOCALE_TESTS=ON `# musl doesn't have a bunch of locales and just pretends it does, so we can't check for them at runtime` \
        -DYALIBCT_LIBC_DOESNT_HAVE_FCVT=ON `# See https://www.openwall.com/lists/musl/2022/09/06/10` \
        -DYALIBCT_LIBC_DOESNT_HAVE_ECVT=ON `# See https://www.openwall.com/lists/musl/2022/09/06/4` \
&& \
    cmake --build build/musl-git --parallel 14
