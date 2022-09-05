#!/usr/bin/env bash

IS_PARALLEL=&

./test-binaries/printf-KOS-mk4-test-positional-printf $IS_PARALLEL
./test-binaries/printf-linux-kernel-test_printf $IS_PARALLEL
./test-binaries/printf-NetBSD-t_printf 2>/dev/null | diff -u - <(printf 'printf = 0\n') $IS_PARALLEL
./test-binaries/printf-NetBSD-t_printf 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0') $IS_PARALLEL
./test-binaries/printf-FreeBSD-printfloat_test $IS_PARALLEL
./test-binaries/printf-FreeBSD-atf-printf_test $IS_PARALLEL
./test-binaries/printf-FreeBSD-plain-printf_test $IS_PARALLEL
./test-binaries/printf-FreeBSD-tap-printf_test | diff -u - <(printf '1..7\nok 1\nok 2\nok 3\nok 4\nok 5\nok 6\nok 7\n') $IS_PARALLEL
./test-binaries/printf-fuchsia-printf_tests $IS_PARALLEL

# Wait for all tests to be over before exiting
wait
