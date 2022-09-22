#!/usr/bin/env bash
set -euo pipefail

# Execute tests from the directory that which contains the script
cd "$(dirname "$0")"

trap_exit () {
    echo "A command run from this script failed !"
}

trap trap_exit ERR

# Commented out for now, but will likely have to be reintroduced whenether we start testing allocator failures
# export ASAN_OPTIONS=${ASAN_OPTIONS}:allocator_may_return_null=true

# Needed to make tests based on MALLOC_TRACE (and stuff like mcheck) work
checked_add_to_ld_preload()
{
    # We need to temporarily disable pipefail so that the || test only and solely uses the grep process as the status for the tests
    set +o pipefail
    { LD_PRELOAD="${LD_PRELOAD-} $1" ./test-binaries/libc-starts-up 2>&1 | grep -q . || LD_PRELOAD="${LD_PRELOAD-} $1" /bin/test -z 2>&1 | grep -q . || export LD_PRELOAD="${LD_PRELOAD-} $1"; } || true
    set -o pipefail
}

checked_add_to_ld_preload /lib64/libc_malloc_debug.so.0
checked_add_to_ld_preload /lib/libc_malloc_debug.so.0

do_mtrace_test()
{
    export MALLOC_TRACE=$(mktemp)
    "$1" "$@"
    mtrace "$1" $MALLOC_TRACE | diff -u - <(echo 'No memory leaks.')
    unset MALLOC_TRACE
}

do_output_diff_test()
{
    eval "$1" | diff -u - "$2"
}

do_printf_littlekernel_printf_tests()
{
    ./test-binaries/printf-littlekernel-printf_tests | sed 's/0x1p-1074/0x0.0000000000001p-1022/;s/0x1.ffffffffffffep-1023/0x0.fffffffffffffp-1022/;s/0X1P-1074/0X0.0000000000001P-1022/;s/0X1.FFFFFFFFFFFFEP-1023/0X0.FFFFFFFFFFFFFP-1022/' | diff -u - ./test-data/outputs/printf-littlekernel-printf_tests 
}

do_printf_NetBSD_t_printf()
{
    ./test-binaries/printf-NetBSD-t_printf 2>/dev/null | diff -u - <(printf 'printf = 0\n')
    ./test-binaries/printf-NetBSD-t_printf 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0')
}

do_printf_glibc_tst_printf_bz18872_sh_output()
{
    do_output_diff_test "do_mtrace_test ./test-binaries/printf-glibc-tst-printf-bz18872-sh-output" ./test-data/outputs/printf-glibc-tst-printf-bz18872-sh-output
}

do_printf_glibc_test_printf_ldbl_compat()
{
    do_output_diff_test "./test-binaries/printf-glibc-test-printf-ldbl-compat | sed 's/\-0x1.0000000000p+0/-0x8.0000000000p-3/g'" ./test-data/outputs/printf-glibc-test-printf-ldbl-compat
}

do_printf_gnulib_test_printf_posix2()
{
    ./test-binaries/printf-gnulib-test-printf-posix2 1 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 1"
    ./test-binaries/printf-gnulib-test-printf-posix2 2 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 2"
    ./test-binaries/printf-gnulib-test-printf-posix2 3 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 3"
    ./test-binaries/printf-gnulib-test-printf-posix2 4 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 4"
    ./test-binaries/printf-gnulib-test-printf-posix2 5 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 5"
    ./test-binaries/printf-gnulib-test-printf-posix2 6 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-printf-posix2 6"
}

for i in \
    ./test-binaries/libc-starts-up ./test-binaries/printf-KOS-mk4-test-positional-printf ./test-binaries/printf-linux-kernel-test_printf do_printf_NetBSD_t_printf \
    ./test-binaries/printf-FreeBSD-printfloat_test ./test-binaries/printf-FreeBSD-atf-printf_test ./test-binaries/printf-FreeBSD-plain-printf_test \
    "do_output_diff_test ./test-binaries/printf-FreeBSD-tap-printf_test ./test-data/outputs/printf-FreeBSD-tap-printf_test" ./test-binaries/printf-fuchsia-printf_tests ./test-binaries/printf-illumos-gate-printf-6961 \
    ./test-binaries/printf-illumos-gate-printf-9511 ./test-binaries/printf-reactos-printf do_printf_littlekernel_printf_tests \
    "do_output_diff_test ./test-binaries/printf-toaruos-test-printf ./test-data/outputs/printf-toaruos-test-printf" \
    ./test-binaries/printf-newsys-test-printf ./test-binaries/printf-osv-tst-printf ./test-binaries/printf-OpenBSD-fp ./test-binaries/printf-OpenBSD-int \
    ./test-binaries/printf-OpenBSD-string "do_output_diff_test ./test-binaries/printf-llvm-project-printf_test ./test-data/outputs/printf-llvm-project-printf_test" \
    "do_output_diff_test ./test-binaries/printf-gcc-printf ./test-data/outputs/printf-gcc-printf" "do_output_diff_test ./test-binaries/printf-gcc-printf-1 ./test-data/outputs/printf-gcc-printf-1" \
    ./test-binaries/printf-gcc-printf-2 "do_output_diff_test ./test-binaries/printf-llvm-test-suite-2002-04-17-PrintfChar ./test-data/outputs/printf-llvm-test-suite-2002-04-17-PrintfChar" \
    "do_output_diff_test ./test-binaries/printf-tcc-02_printf ./test-data/outputs/printf-tcc-02_printf" ./test-binaries/printf-wine-msvcrt-printf ./test-binaries/printf-wine-ucrtbase-printf \
    ./test-binaries/printf-glibc-tst-printf-binary ./test-binaries/printf-glibc-tst-obprintf do_printf_glibc_tst_printf_bz18872_sh_output ./test-binaries/printf-glibc-tst-printf-bz25691 \
    "do_mtrace_test ./test-binaries/printf-glibc-tst-printf-fp-free" "do_mtrace_test ./test-binaries/printf-glibc-tst-printf-fp-leak" ./test-binaries/printf-glibc-tst-printf-round \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-printf ./test-data/outputs/printf-glibc-tst-printf" "./test-binaries/printf-glibc-tst-printfsz-islongdouble | diff -u - <(printf '2k4k')" \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-printfsz ./test-data/outputs/printf-glibc-tst-printfsz" \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-wc-printf ./test-data/outputs/printf-glibc-tst-wc-printf" do_printf_glibc_test_printf_ldbl_compat ./test-binaries/printf-glibc-tst-ldbl-nonnormal-printf \
    "do_output_diff_test ./test-binaries/printf-newlib-nulprintf <(echo 'MMMMMMMM')" ./test-binaries/printf-picolibc-nulprintf ./test-binaries/printf-picolibc-printf-tests ./test-binaries/printf-picolibc-printf_scanf \
    ./test-binaries/printf-dietlibc-printf2 ./test-binaries/printf-dietlibc-printf "do_output_diff_test ./test-binaries/printf-dietlibc-printftest ./test-data/outputs/printf-dietlibc-printftest" \
    ./test-binaries/printf-cloudlibc-printf_scanf_test "do_output_diff_test ./test-binaries/printf-dklibc-test_printf ./test-data/outputs/printf-dklibc-test_printf" ./test-binaries/printf-avr-libc-bug-35366-2-printf_flt \
    ./test-binaries/printf-libc-test-printf-1e9-oob ./test-binaries/printf-libc-test-printf-fmt-g-round ./test-binaries/printf-libc-test-printf-fmt-g-zeros ./test-binaries/printf-libc-test-printf-fmt-n \
    "do_output_diff_test ./test-binaries/printf-gnulib-test-printf-posix ./test-data/outputs/printf-gnulib-test-printf-posix" do_printf_gnulib_test_printf_posix2 ./test-binaries/printf-pdclib-printf \
    ./test-binaries/printf-gnulib-printf-sizes-c99 ./test-binaries/printf-gnulib-printf-directive-a ./test-binaries/printf-gnulib-printf-directive-f ./test-binaries/printf-gnulib-printf-directive-ls \
    ./test-binaries/printf-gnulib-printf-directive-n "./test-binaries/printf-gnulib-printf-enomem >/dev/null" ./test-binaries/printf-gnulib-printf-flag-grouping ./test-binaries/printf-gnulib-printf-flag-leftadjust \
    ./test-binaries/printf-gnulib-printf-flag-zero ./test-binaries/printf-gnulib-printf-infinite ./test-binaries/printf-gnulib-printf-infinite-long-double ./test-binaries/printf-gnulib-printf-long-double \
    ./test-binaries/printf-gnulib-printf-positions ./test-binaries/printf-gnulib-printf-precision ./test-binaries/printf-gnulib-snprintf-directive-n ./test-binaries/printf-gnulib-snprintf-retval-c99 \
    ./test-binaries/printf-gnulib-snprintf-size1 ./test-binaries/printf-gnulib-snprintf-truncation-c99 ./test-binaries/printf-gnulib-vsnprintf-zerosize-c99 ./test-binaries/strcat-NetBSD-t_strcat \
    ./test-binaries/strcat-llvm-project-strcat_test ./test-binaries/strcat-gcc-strcat ./test-binaries/strcat-cc65-lib_common_strcat ./test-binaries/strcat-scc-0011-strcat ./test-binaries/strcat-glibc-test-strcat \
    ./test-binaries/strcat-cosmopolitan-strcat_test
do
    eval "$i" || echo "Test '$i' failed with status $?" &
done

# Wait for all tests to be over before exiting
wait
