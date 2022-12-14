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

do_printf_littlekernel_tests()
{
    ./test-binaries/printf-littlekernel-tests | sed 's/0x1p-1074/0x0.0000000000001p-1022/;s/0x1.ffffffffffffep-1023/0x0.fffffffffffffp-1022/;s/0X1P-1074/0X0.0000000000001P-1022/;s/0X1.FFFFFFFFFFFFEP-1023/0X0.FFFFFFFFFFFFFP-1022/' | diff -u - ./test-data/outputs/printf-littlekernel-tests
}

do_printf_NetBSD_t()
{
    ./test-binaries/printf-NetBSD-t 2>/dev/null | diff -u - <(printf 'printf = 0\n')
    ./test-binaries/printf-NetBSD-t 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0')
}

do_printf_glibc_tst_bz18872_sh_output()
{
    do_output_diff_test "do_mtrace_test ./test-binaries/printf-glibc-tst-bz18872-sh-output" ./test-data/outputs/printf-glibc-tst-bz18872-sh-output
}

do_printf_glibc_test_ldbl_compat()
{
    do_output_diff_test "./test-binaries/printf-glibc-test-ldbl-compat | sed 's/\-0x1.0000000000p+0/-0x8.0000000000p-3/g'" ./test-data/outputs/printf-glibc-test-ldbl-compat
}

do_printf_gnulib_test_posix2()
{
    ./test-binaries/printf-gnulib-test-posix2 1 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 1"
    ./test-binaries/printf-gnulib-test-posix2 2 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 2"
    ./test-binaries/printf-gnulib-test-posix2 3 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 3"
    ./test-binaries/printf-gnulib-test-posix2 4 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 4"
    ./test-binaries/printf-gnulib-test-posix2 5 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 5"
    ./test-binaries/printf-gnulib-test-posix2 6 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 6"
}



[ ! -e ./test-binaries ] && { echo "Literally none of the tests will run correctly if the binaries aren't present, so please build this project so there's something in ./test-binaries..."; exit 1; }

TEMP_WORK_FILE=$(mktemp)

for i in \
    ./test-binaries/libc-starts-up ./test-binaries/printf-KOS-mk4-test-positional ./test-binaries/printf-linux-kernel-test do_printf_NetBSD_t \
    ./test-binaries/printf-FreeBSD-printfloat_test ./test-binaries/printf-FreeBSD-atf_test ./test-binaries/printf-FreeBSD-plain_test \
    "do_output_diff_test ./test-binaries/printf-FreeBSD-tap_test ./test-data/outputs/printf-FreeBSD-tap_test" ./test-binaries/printf-fuchsia-tests ./test-binaries/printf-illumos-gate-6961 \
    ./test-binaries/printf-illumos-gate-9511 ./test-binaries/printf-reactos do_printf_littlekernel_tests \
    "do_output_diff_test ./test-binaries/printf-toaruos-test ./test-data/outputs/printf-toaruos-test" \
    ./test-binaries/printf-newsys-test ./test-binaries/printf-osv-tst ./test-binaries/printf-OpenBSD-fp ./test-binaries/printf-OpenBSD-int \
    ./test-binaries/printf-OpenBSD-string "do_output_diff_test ./test-binaries/printf-llvm-project-test ./test-data/outputs/printf-llvm-project-test" \
    "do_output_diff_test ./test-binaries/printf-gcc ./test-data/outputs/printf-gcc" "do_output_diff_test ./test-binaries/printf-gcc-1 ./test-data/outputs/printf-gcc-1" \
    ./test-binaries/printf-gcc-2 "do_output_diff_test ./test-binaries/printf-llvm-test-suite-2002-04-17-Char ./test-data/outputs/printf-llvm-test-suite-2002-04-17-Char" \
    "do_output_diff_test ./test-binaries/printf-tcc-02 ./test-data/outputs/printf-tcc-02" ./test-binaries/printf-wine-msvcrt ./test-binaries/printf-wine-ucrtbase \
    ./test-binaries/printf-glibc-tst-binary ./test-binaries/printf-glibc-tst-ob do_printf_glibc_tst_bz18872_sh_output ./test-binaries/printf-glibc-tst-bz25691 \
    "do_mtrace_test ./test-binaries/printf-glibc-tst-fp-free" "do_mtrace_test ./test-binaries/printf-glibc-tst-fp-leak" ./test-binaries/printf-glibc-tst-round \
    "do_output_diff_test ./test-binaries/printf-glibc-tst ./test-data/outputs/printf-glibc-tst" "./test-binaries/printf-glibc-tst-sz-islongdouble | diff -u - <(printf '2k4k')" \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-sz ./test-data/outputs/printf-glibc-tst-sz" \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-wc ./test-data/outputs/printf-glibc-tst-wc" do_printf_glibc_test_ldbl_compat ./test-binaries/printf-glibc-tst-ldbl-nonnormal \
    "do_output_diff_test ./test-binaries/printf-newlib-nul <(echo 'MMMMMMMM')" ./test-binaries/printf-picolibc-nul ./test-binaries/printf-picolibc-tests ./test-binaries/printf-picolibc-scanf \
    ./test-binaries/printf-dietlibc2 ./test-binaries/printf-dietlibc "do_output_diff_test ./test-binaries/printf-dietlibctest ./test-data/outputs/printf-dietlibctest" \
    ./test-binaries/printf-cloudlibc-scanf_test "do_output_diff_test ./test-binaries/printf-dklibc-test ./test-data/outputs/printf-dklibc-test" ./test-binaries/printf-avr-libc-bug-35366-2_flt \
    ./test-binaries/printf-libc-test-1e9-oob ./test-binaries/printf-libc-test-fmt-g-round ./test-binaries/printf-libc-test-fmt-g-zeros ./test-binaries/printf-libc-test-fmt-n \
    "do_output_diff_test ./test-binaries/printf-gnulib-test-posix ./test-data/outputs/printf-gnulib-test-posix" do_printf_gnulib_test_posix2 ./test-binaries/printf-pdclib \
    ./test-binaries/printf-gnulib-sizes-c99 ./test-binaries/printf-gnulib-directive-a ./test-binaries/printf-gnulib-directive-f ./test-binaries/printf-gnulib-directive-ls \
    ./test-binaries/printf-gnulib-directive-n "./test-binaries/printf-gnulib-enomem >/dev/null" ./test-binaries/printf-gnulib-flag-grouping ./test-binaries/printf-gnulib-flag-leftadjust \
    ./test-binaries/printf-gnulib-flag-zero ./test-binaries/printf-gnulib-infinite ./test-binaries/printf-gnulib-infinite-long-double ./test-binaries/printf-gnulib-long-double \
    ./test-binaries/printf-gnulib-positions ./test-binaries/printf-gnulib-precision ./test-binaries/printf-gnulib-sn-directive-n ./test-binaries/printf-gnulib-sn-retval-c99 \
    ./test-binaries/printf-gnulib-sn-size1 ./test-binaries/printf-gnulib-sn-truncation-c99 ./test-binaries/printf-gnulib-vsn-zerosize-c99 \
    "do_output_diff_test ./test-binaries/printf-mpaland-test_suite ./test-data/outputs/printf-mpaland-test_suite" "do_output_diff_test ./test-binaries/printf-duktape-test ./test-data/outputs/printf-duktape-test" \
    ./test-binaries/printf-cosmopolitan-kprintf_test \
    \
    ./test-binaries/strcat-NetBSD-t ./test-binaries/strcat-llvm-project-test ./test-binaries/strcat-gcc ./test-binaries/strcat-cc65-lib_common ./test-binaries/strcat-scc-0011 ./test-binaries/strcat-glibc-test \
    ./test-binaries/strcat-cosmopolitan-test ./test-binaries/strcat-cloudlibc-test ./test-binaries/strcat-gnulib-test-u8 ./test-binaries/strcat-pdclib ./test-binaries/strcat-embeddedartistry-libc \
    \
    ./test-binaries/strlen-linux-kernel ./test-binaries/strlen-open-posix-testsuite-1-1 ./test-binaries/strlen-NetBSD-t ./test-binaries/strlen-arm-optimized-routines ./test-binaries/strlen-reactos-crt \
    ./test-binaries/strlen-llvm-project-test ./test-binaries/strlen-embeddedartistry-libc ./test-binaries/strlen-gcc-builtins-2 ./test-binaries/strlen-gcc-builtins-3 ./test-binaries/strlen-gcc-builtins \
    ./test-binaries/strlen-gcc-execute-1 ./test-binaries/strlen-gcc-execute-2 ./test-binaries/strlen-gcc-execute-3 ./test-binaries/strlen-gcc-execute-4 ./test-binaries/strlen-gcc-execute-5 ./test-binaries/strlen-gcc-execute-6 \
    ./test-binaries/strlen-gcc-execute-7 ./test-binaries/strlen-gcc-ldist-1 ./test-binaries/strlen-gcc-ldist-2 ./test-binaries/strlen-gcc-opt-10 \
    ./test-binaries/strlen-gcc-opt-11 ./test-binaries/strlen-gcc-opt-12 ./test-binaries/strlen-gcc-opt-12g ./test-binaries/strlen-gcc-opt-13 ./test-binaries/strlen-gcc-opt-14g ./test-binaries/strlen-gcc-opt-14gf \
    ./test-binaries/strlen-gcc-opt-15 ./test-binaries/strlen-gcc-opt-16g ./test-binaries/strlen-gcc-opt-17g ./test-binaries/strlen-gcc-opt-18g ./test-binaries/strlen-gcc-opt-19 ./test-binaries/strlen-gcc-opt-1 \
    ./test-binaries/strlen-gcc-opt-1f ./test-binaries/strlen-gcc-opt-20 ./test-binaries/strlen-gcc-opt-21 ./test-binaries/strlen-gcc-opt-22 ./test-binaries/strlen-gcc-opt-22g ./test-binaries/strlen-gcc-opt-23 \
    ./test-binaries/strlen-gcc-opt-24 ./test-binaries/strlen-gcc-opt-25 ./test-binaries/strlen-gcc-opt-26 ./test-binaries/strlen-gcc-opt-27 ./test-binaries/strlen-gcc-opt-28 ./test-binaries/strlen-gcc-opt-29 \
    ./test-binaries/strlen-gcc-opt-2 ./test-binaries/strlen-gcc-opt-2f ./test-binaries/strlen-gcc-opt-31 ./test-binaries/strlen-gcc-opt-31g ./test-binaries/strlen-gcc-opt-32 \
    ./test-binaries/strlen-gcc-opt-33 ./test-binaries/strlen-gcc-opt-33g ./test-binaries/strlen-gcc-opt-34 ./test-binaries/strlen-gcc-opt-35 \
    ./test-binaries/strlen-gcc-opt-3 ./test-binaries/strlen-gcc-opt-46 ./test-binaries/strlen-gcc-opt-4 ./test-binaries/strlen-gcc-opt-4g ./test-binaries/strlen-gcc-opt-4gf \
    ./test-binaries/strlen-gcc-opt-5 ./test-binaries/strlen-gcc-opt-63 ./test-binaries/strlen-gcc-opt-64 ./test-binaries/strlen-gcc-opt-66 ./test-binaries/strlen-gcc-opt-68 \
    ./test-binaries/strlen-gcc-opt-6 ./test-binaries/strlen-gcc-opt-71 ./test-binaries/strlen-gcc-opt-74 ./test-binaries/strlen-gcc-opt-75 ./test-binaries/strlen-gcc-opt-76 ./test-binaries/strlen-gcc-opt-79 \
    ./test-binaries/strlen-gcc-opt-7 ./test-binaries/strlen-gcc-opt-81 ./test-binaries/strlen-gcc-opt-84 ./test-binaries/strlen-gcc-opt-87 ./test-binaries/strlen-gcc-opt-88 ./test-binaries/strlen-gcc-opt-8 \
    ./test-binaries/strlen-gcc-opt-92 ./test-binaries/strlen-gcc-opt-94 ./test-binaries/strlen-gcc-opt-9 ./test-binaries/strlen-scc-0018 ./test-binaries/strlen-glibc-test ./test-binaries/strlen-glibc-tst \
    ./test-binaries/strlen-glibc-tst-rsi ./test-binaries/strlen-cosmopolitan-test ./test-binaries/strlen-nlibc ./test-binaries/strlen-cloudlibc ./test-binaries/strlen-dietlibc ./test-binaries/strlen-gnulib-test-u8 \
    \
    ./test-binaries/isdigit-llvm-project-test ./test-binaries/isdigit-avr-libc-1 "do_output_diff_test ./test-binaries/isdigit-open-watcom-v2 ./test-data/outputs/isdigit-open-watcom-v2" ./test-binaries/isdigit-z88dk \
    ./test-binaries/isdigit-embeddedartistry-libc ./test-binaries/isdigit-pdclib \
    \
    ./test-binaries/isalpha-llvm-project-test ./test-binaries/isalpha-z88dk-test ./test-binaries/isalpha-avr-libc-1 ./test-binaries/isalpha-embeddedartistry-libc ./test-binaries/isalpha-pdclib \
    \
    "do_output_diff_test ./test-binaries/putchar-pdclib ./test-data/outputs/putchar-pdclib" \
    \
    ./test-binaries/strcmp-NetBSD-t ./test-binaries/strcmp-llvm-project-test ./test-binaries/strcmp-gcc-builtins ./test-binaries/strcmp-gcc-execute-1 ./test-binaries/strcmp-gcc-dg-1 ./test-binaries/strcmp-gcc-dg-opt_1 \
    ./test-binaries/strcmp-gcc-dg-opt_2 ./test-binaries/strcmp-gcc-dg-opt_3 ./test-binaries/strcmp-gcc-dg-opt_5 ./test-binaries/strcmp-gcc-dg-opt_6 ./test-binaries/strcmp-gcc-dg-opt_9 ./test-binaries/strcmp-gcc-dg-opt_12 \
    ./test-binaries/strcmp-z88dk ./test-binaries/strcmp-scc-0013 ./test-binaries/strcmp-glibc-test ./test-binaries/strcmp-cosmopolitan-test ./test-binaries/strcmp-newlib-1 ./test-binaries/strcmp-nlibc \
    ./test-binaries/strcmp-cloudlibc-test ./test-binaries/strcmp-avr-libc ./test-binaries/strcmp-arm-optimized-routines ./test-binaries/strcmp-embeddedartistry-libc ./test-binaries/strcmp-pdclib
do
    eval "$i" && printf "Test '%s' succeeded\n" "$i" | tee -a "$TEMP_WORK_FILE" >/dev/null || printf "Test '%s' failed with status $?\n" "$i" | tee /dev/stderr >>"$TEMP_WORK_FILE" &
done

# Wait for all tests to be over before exiting
wait

printf '%s tests failed out of %s tests\n' $(grep -cE '^Test .* failed with status .?.?.?$' "$TEMP_WORK_FILE") $(<"$TEMP_WORK_FILE" wc -l)
grep -qE '^Test .* failed with status .?.?.?$' "$TEMP_WORK_FILE" && { echo 'Failed tests:'; grep -E '^Test .* failed with status .?.?.?$' "$TEMP_WORK_FILE"; }
rm "$TEMP_WORK_FILE"
