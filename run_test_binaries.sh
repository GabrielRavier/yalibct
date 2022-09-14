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

for i in \
    ./test-binaries/libc-starts-up ./test-binaries/printf-KOS-mk4-test-positional-printf ./test-binaries/printf-linux-kernel-test_printf do_printf_NetBSD_t_printf \
    ./test-binaries/printf-FreeBSD-printfloat_test ./test-binaries/printf-FreeBSD-atf-printf_test ./test-binaries/printf-FreeBSD-plain-printf_test \
    "do_output_diff_test ./test-binaries/printf-FreeBSD-tap-printf_test ./test-data/outputs/printf-FreeBSD-tap-printf_test" ./test-binaries/printf-fuchsia-printf_tests ./test-binaries/printf-illumos-gate-printf-6961 \
    ./test-binaries/printf-illumos-gate-printf-9511 ./test-binaries/printf-reactos-printf do_printf_littlekernel_printf_tests "do_output_diff_test ./test-binaries/printf-toaruos-test-printf ./test-data/outputs/printf-toaruos-test-printf" \
    ./test-binaries/printf-newsys-test-printf ./test-binaries/printf-osv-tst-printf ./test-binaries/printf-OpenBSD-fp ./test-binaries/printf-OpenBSD-int \
    ./test-binaries/printf-OpenBSD-string "do_output_diff_test ./test-binaries/printf-llvm-project-printf_test ./test-data/outputs/printf-llvm-project-printf_test" \
    "do_output_diff_test ./test-binaries/printf-gcc-printf ./test-data/outputs/printf-gcc-printf" "do_output_diff_test ./test-binaries/printf-gcc-printf-1 ./test-data/outputs/printf-gcc-printf-1" \
    ./test-binaries/printf-gcc-printf-2 "do_output_diff_test ./test-binaries/printf-llvm-test-suite-2002-04-17-PrintfChar ./test-data/outputs/printf-llvm-test-suite-2002-04-17-PrintfChar" \
    "do_output_diff_test ./test-binaries/printf-tcc-02_printf ./test-data/outputs/printf-tcc-02_printf" ./test-binaries/printf-wine-msvcrt-printf ./test-binaries/printf-wine-ucrtbase-printf \
    ./test-binaries/printf-glibc-tst-printf-binary ./test-binaries/printf-glibc-tst-obprintf do_printf_glibc_tst_printf_bz18872_sh_output ./test-binaries/printf-glibc-tst-printf-bz25691 \
    "do_mtrace_test ./test-binaries/printf-glibc-tst-printf-fp-free" "do_mtrace_test ./test-binaries/printf-glibc-tst-printf-fp-leak" ./test-binaries/printf-glibc-tst-printf-round \
    "do_output_diff_test ./test-binaries/printf-glibc-tst-printf ./test-data/outputs/printf-glibc-tst-printf" "./test-binaries/printf-glibc-tst-printfsz-islongdouble | diff -u - <(printf '2k4k')" "do_output_diff_test ./test-binaries/printf-glibc-tst-printfsz ./test-data/outputs/printf-glibc-tst-printfsz" "do_output_diff_test ./test-binaries/printf-glibc-tst-wc-printf ./test-data/outputs/printf-glibc-tst-wc-printf" do_printf_glibc_test_printf_ldbl_compat ./test-binaries/printf-glibc-tst-ldbl-nonnormal-printf "do_output_diff_test ./test-binaries/printf-newlib-nulprintf <(echo 'MMMMMMMM')"
do
    eval "$i" &
done

# Wait for all tests to be over before exiting
wait
