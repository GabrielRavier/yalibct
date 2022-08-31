#!/usr/bin/env bash

./test-binaries/printf-KOS-mk4-test-positional-printf
./test-binaries/printf-linux-kernel-test_printf
./test-binaries/printf-NetBSD-t_printf 2>/dev/null | diff -u - <(printf 'printf = 0\n')
./test-binaries/printf-NetBSD-t_printf 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0')



