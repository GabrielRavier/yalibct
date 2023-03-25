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
export MALLOC_CHECK_=3
export MALLOC_PERTURB_=$(($RANDOM % 255 + 1))



# Modify this function if you want to get something like valgrind or other running
executable_runner()
{
    "$1" "${@:2}"
}

# Scrapped idea to have the runner create a temporary directory for each test - tests need to manage this themselves
# Note that it fails to check for mktemp failing for now, too
#executable_runner()
#{
#    local EXECUTABLE_PATH=$(realpath "$1")
#    (
#        local TEST_TEMPORARY_DIRECTORY="$(mktemp -d)"
#        cd "$TEST_TEMPORARY_DIRECTORY" || return 1
#        executable_wrapper "$EXECUTABLE_PATH" "${@:2}"
#        local TEST_EXIT_STATUS=$?
#        rm -r "$TEST_TEMPORARY_DIRECTORY"
#        return "$TEST_EXIT_STATUS"
#    )
#}

do_mtrace_test()
{
    export MALLOC_TRACE=$(mktemp)
    executable_runner "$@"
    mtrace "$1" $MALLOC_TRACE | diff -u - <(echo 'No memory leaks.')
    unset MALLOC_TRACE
}

do_output_diff_test()
{
    eval "$1" | diff -u - "$2"
}

do_output_lines_only_in_executable_output_test()
{
    local TEMP_EXECUTABLE_SORTED_OUTPUT_FILE=$(mktemp)
    eval "$1" | sort >"$TEMP_EXECUTABLE_SORTED_OUTPUT_FILE"

    # Optimisation: if both the files are the same, it's impossible that the first file will contain any lines not in the second one
    cmp -s "$TEMP_EXECUTABLE_SORTED_OUTPUT_FILE" "$2" && { rm "$TEMP_EXECUTABLE_SORTED_OUTPUT_FILE"; return 0; }

    # The (! command) inverts the result of grep, meaning it will only exit with 0 if no matches are found (here, this means the input must be empty, e.g. that the are no lines that are only present in the test's output)
    <"$TEMP_EXECUTABLE_SORTED_OUTPUT_FILE" comm -23 - "$2" | (! grep .)
    THIS_FUNC_EXIT_STATUS=$?

    rm "$TEMP_EXECUTABLE_SORTED_OUTPUT_FILE"

    return "$THIS_FUNC_EXIT_STATUS"
}

do_printf_littlekernel_tests()
{
    executable_runner ./test-binaries/printf-littlekernel-tests | sed 's/0x1p-1074/0x0.0000000000001p-1022/;s/0x1.ffffffffffffep-1023/0x0.fffffffffffffp-1022/;s/0X1P-1074/0X0.0000000000001P-1022/;s/0X1.FFFFFFFFFFFFEP-1023/0X0.FFFFFFFFFFFFFP-1022/' | diff -u - ./test-data/outputs/printf-littlekernel-tests
}

do_printf_NetBSD_t()
{
    executable_runner ./test-binaries/printf-NetBSD-t 2>/dev/null | diff -u - <(printf 'printf = 0\n')
    executable_runner ./test-binaries/printf-NetBSD-t 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0')
}

do_printf_glibc_tst_bz18872_sh_output()
{
    do_output_diff_test "do_mtrace_test ./test-binaries/printf-glibc-tst-bz18872-sh-output" ./test-data/outputs/printf-glibc-tst-bz18872-sh-output
}

do_printf_glibc_test_ldbl_compat()
{
    do_output_diff_test "executable_runner ./test-binaries/printf-glibc-test-ldbl-compat | sed 's/\-0x1.0000000000p+0/-0x8.0000000000p-3/g'" ./test-data/outputs/printf-glibc-test-ldbl-compat
}

do_printf_gnulib_test_posix2()
{
    executable_runner ./test-binaries/printf-gnulib-test-posix2 1 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 1"
    executable_runner ./test-binaries/printf-gnulib-test-posix2 2 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 2"
    executable_runner ./test-binaries/printf-gnulib-test-posix2 3 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 3"
    executable_runner ./test-binaries/printf-gnulib-test-posix2 4 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 4"
    executable_runner ./test-binaries/printf-gnulib-test-posix2 5 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 5"
    executable_runner ./test-binaries/printf-gnulib-test-posix2 6 >/dev/null || echo "FAILED ./test-binaries/printf-gnulib-test-posix2 6"
}



[ ! -e ./test-binaries ] && { echo "Literally none of the tests will run correctly if the binaries aren't present, so please build this project so there's something in ./test-binaries..."; exit 1; }

TEMP_TESTS_RESULTS_FILE=$(mktemp)

for i in \
    "executable_runner ./test-binaries/libc-starts-up" "executable_runner ./test-binaries/printf-KOS-mk4-test-positional" "executable_runner ./test-binaries/printf-linux-kernel-test" do_printf_NetBSD_t \
    "executable_runner ./test-binaries/printf-FreeBSD-printfloat_test" "executable_runner ./test-binaries/printf-FreeBSD-atf_test" "executable_runner ./test-binaries/printf-FreeBSD-plain_test" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-FreeBSD-tap_test' ./test-data/outputs/printf-FreeBSD-tap_test" "executable_runner ./test-binaries/printf-fuchsia-tests" \
    "executable_runner ./test-binaries/printf-illumos-gate-6961" "executable_runner ./test-binaries/printf-illumos-gate-9511" "executable_runner ./test-binaries/printf-reactos" do_printf_littlekernel_tests \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-toaruos-test' ./test-data/outputs/printf-toaruos-test" \
    "executable_runner ./test-binaries/printf-newsys-test" "executable_runner ./test-binaries/printf-osv-tst" "executable_runner ./test-binaries/printf-OpenBSD-fp" "executable_runner ./test-binaries/printf-OpenBSD-int" \
    "executable_runner ./test-binaries/printf-OpenBSD-string" "do_output_diff_test 'executable_runner ./test-binaries/printf-llvm-project-test' ./test-data/outputs/printf-llvm-project-test" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-gcc' ./test-data/outputs/printf-gcc" "do_output_diff_test 'executable_runner ./test-binaries/printf-gcc-1' ./test-data/outputs/printf-gcc-1" \
    "executable_runner ./test-binaries/printf-gcc-2" "do_output_diff_test 'executable_runner ./test-binaries/printf-llvm-test-suite-2002-04-17-Char' ./test-data/outputs/printf-llvm-test-suite-2002-04-17-Char" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-tcc-02' ./test-data/outputs/printf-tcc-02" "executable_runner ./test-binaries/printf-wine-msvcrt" "executable_runner ./test-binaries/printf-wine-ucrtbase" \
    "executable_runner ./test-binaries/printf-glibc-tst-binary" "executable_runner ./test-binaries/printf-glibc-tst-ob" do_printf_glibc_tst_bz18872_sh_output "executable_runner ./test-binaries/printf-glibc-tst-bz25691" \
    "do_mtrace_test ./test-binaries/printf-glibc-tst-fp-free" "do_mtrace_test ./test-binaries/printf-glibc-tst-fp-leak" "executable_runner ./test-binaries/printf-glibc-tst-round" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-glibc-tst' ./test-data/outputs/printf-glibc-tst" "executable_runner ./test-binaries/printf-glibc-tst-sz-islongdouble | diff -u - <(printf '2k4k')" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-glibc-tst-sz' ./test-data/outputs/printf-glibc-tst-sz" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-glibc-tst-wc' ./test-data/outputs/printf-glibc-tst-wc" do_printf_glibc_test_ldbl_compat "executable_runner ./test-binaries/printf-glibc-tst-ldbl-nonnormal" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-newlib-nul' <(echo 'MMMMMMMM')" "executable_runner ./test-binaries/printf-picolibc-nul" "executable_runner ./test-binaries/printf-picolibc-tests" \
    "executable_runner ./test-binaries/printf-picolibc-scanf" "executable_runner ./test-binaries/printf-dietlibc2" "executable_runner ./test-binaries/printf-dietlibc" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-dietlibctest' ./test-data/outputs/printf-dietlibctest" "executable_runner ./test-binaries/printf-cloudlibc-scanf_test" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-dklibc-test' ./test-data/outputs/printf-dklibc-test" "executable_runner ./test-binaries/printf-avr-libc-bug-35366-2_flt" \
    "executable_runner ./test-binaries/printf-libc-test-1e9-oob" "executable_runner ./test-binaries/printf-libc-test-fmt-g-round" \
    "executable_runner ./test-binaries/printf-libc-test-fmt-g-zeros ./test-binaries/printf-libc-test-fmt-n" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-gnulib-test-posix' ./test-data/outputs/printf-gnulib-test-posix" do_printf_gnulib_test_posix2 "executable_runner ./test-binaries/printf-pdclib" \
    "executable_runner ./test-binaries/printf-gnulib-sizes-c99" "executable_runner ./test-binaries/printf-gnulib-directive-a" "executable_runner ./test-binaries/printf-gnulib-directive-f" \
    "executable_runner ./test-binaries/printf-gnulib-directive-ls" "executable_runner ./test-binaries/printf-gnulib-directive-n" "executable_runner ./test-binaries/printf-gnulib-enomem >/dev/null" \
    "executable_runner ./test-binaries/printf-gnulib-flag-grouping" "executable_runner ./test-binaries/printf-gnulib-flag-leftadjust" "executable_runner ./test-binaries/printf-gnulib-flag-zero" \
    "executable_runner ./test-binaries/printf-gnulib-infinite" "executable_runner ./test-binaries/printf-gnulib-infinite-long-double" "executable_runner ./test-binaries/printf-gnulib-long-double" \
    "executable_runner ./test-binaries/printf-gnulib-positions" "executable_runner ./test-binaries/printf-gnulib-precision" "executable_runner ./test-binaries/printf-gnulib-sn-directive-n" \
    "executable_runner ./test-binaries/printf-gnulib-sn-retval-c99" "executable_runner ./test-binaries/printf-gnulib-sn-size1" "executable_runner ./test-binaries/printf-gnulib-sn-truncation-c99" \
    "executable_runner ./test-binaries/printf-gnulib-vsn-zerosize-c99" "do_output_diff_test 'executable_runner ./test-binaries/printf-mpaland-test_suite' ./test-data/outputs/printf-mpaland-test_suite" \
    "do_output_diff_test 'executable_runner ./test-binaries/printf-duktape-test' ./test-data/outputs/printf-duktape-test" "executable_runner ./test-binaries/printf-cosmopolitan-kprintf_test" \
    "do_output_lines_only_in_executable_output_test 'executable_runner ./test-binaries/printf-libcmini_formatting-1' ./test-data/outputs/printf-libcmini_formatting-1-presorted" \
    "do_output_lines_only_in_executable_output_test 'executable_runner ./test-binaries/printf-libcmini_formatting-2' ./test-data/outputs/printf-libcmini_formatting-2-presorted" \
    "do_output_lines_only_in_executable_output_test 'executable_runner ./test-binaries/printf-libcmini_formatting-3' ./test-data/outputs/printf-libcmini_formatting-3-presorted" \
    "do_output_lines_only_in_executable_output_test 'executable_runner ./test-binaries/printf-libcmini_formatting-4' ./test-data/outputs/printf-libcmini_formatting-4-presorted" \
    "do_output_lines_only_in_executable_output_test 'executable_runner ./test-binaries/printf-libcmini_formatting-5' ./test-data/outputs/printf-libcmini_formatting-5-presorted" \
    \
    "executable_runner ./test-binaries/strcat-NetBSD-t" "executable_runner ./test-binaries/strcat-llvm-project-test" "executable_runner ./test-binaries/strcat-gcc" "executable_runner ./test-binaries/strcat-cc65-lib_common" "executable_runner ./test-binaries/strcat-scc-0011" "executable_runner ./test-binaries/strcat-glibc-test" \
    "executable_runner ./test-binaries/strcat-cosmopolitan-test" "executable_runner ./test-binaries/strcat-cloudlibc-test" "executable_runner ./test-binaries/strcat-gnulib-test-u8" "executable_runner ./test-binaries/strcat-pdclib" "executable_runner ./test-binaries/strcat-embeddedartistry-libc" \
    \
    "executable_runner ./test-binaries/strlen-linux-kernel" "executable_runner ./test-binaries/strlen-open-posix-testsuite-1-1" "executable_runner ./test-binaries/strlen-NetBSD-t" \
    "executable_runner ./test-binaries/strlen-arm-optimized-routines" "executable_runner ./test-binaries/strlen-reactos-crt" "executable_runner ./test-binaries/strlen-llvm-project-test" \
    "executable_runner ./test-binaries/strlen-embeddedartistry-libc" "executable_runner ./test-binaries/strlen-gcc-builtins-2" "executable_runner ./test-binaries/strlen-gcc-builtins-3" \
    "executable_runner ./test-binaries/strlen-gcc-builtins" "executable_runner ./test-binaries/strlen-gcc-execute-1" "executable_runner ./test-binaries/strlen-gcc-execute-2" "executable_runner ./test-binaries/strlen-gcc-execute-3" \
    "executable_runner ./test-binaries/strlen-gcc-execute-4" "executable_runner ./test-binaries/strlen-gcc-execute-5" "executable_runner ./test-binaries/strlen-gcc-execute-6" "executable_runner ./test-binaries/strlen-gcc-execute-7" \
    "executable_runner ./test-binaries/strlen-gcc-ldist-1" "executable_runner ./test-binaries/strlen-gcc-ldist-2" "executable_runner ./test-binaries/strlen-gcc-opt-10" "executable_runner ./test-binaries/strlen-gcc-opt-11" \
    "executable_runner ./test-binaries/strlen-gcc-opt-12" "executable_runner ./test-binaries/strlen-gcc-opt-12g" "executable_runner ./test-binaries/strlen-gcc-opt-13" "executable_runner ./test-binaries/strlen-gcc-opt-14g" \
    "executable_runner ./test-binaries/strlen-gcc-opt-14gf" "executable_runner ./test-binaries/strlen-gcc-opt-15" "executable_runner ./test-binaries/strlen-gcc-opt-16g" "executable_runner ./test-binaries/strlen-gcc-opt-17g" \
    "executable_runner ./test-binaries/strlen-gcc-opt-18g" "executable_runner ./test-binaries/strlen-gcc-opt-19" "executable_runner ./test-binaries/strlen-gcc-opt-1" "executable_runner ./test-binaries/strlen-gcc-opt-1f" \
    "executable_runner ./test-binaries/strlen-gcc-opt-20" "executable_runner ./test-binaries/strlen-gcc-opt-21" "executable_runner ./test-binaries/strlen-gcc-opt-22" "executable_runner ./test-binaries/strlen-gcc-opt-22g" \
    "executable_runner ./test-binaries/strlen-gcc-opt-23" "executable_runner ./test-binaries/strlen-gcc-opt-24" "executable_runner ./test-binaries/strlen-gcc-opt-25" "executable_runner ./test-binaries/strlen-gcc-opt-26" \
    "executable_runner ./test-binaries/strlen-gcc-opt-27" "executable_runner ./test-binaries/strlen-gcc-opt-28" "executable_runner ./test-binaries/strlen-gcc-opt-29" "executable_runner ./test-binaries/strlen-gcc-opt-2" \
    "executable_runner ./test-binaries/strlen-gcc-opt-2f" "executable_runner ./test-binaries/strlen-gcc-opt-31" "executable_runner ./test-binaries/strlen-gcc-opt-31g" "executable_runner ./test-binaries/strlen-gcc-opt-32" \
    "executable_runner ./test-binaries/strlen-gcc-opt-33" "executable_runner ./test-binaries/strlen-gcc-opt-33g" "executable_runner ./test-binaries/strlen-gcc-opt-34" "executable_runner ./test-binaries/strlen-gcc-opt-35" \
    "executable_runner ./test-binaries/strlen-gcc-opt-3" "executable_runner ./test-binaries/strlen-gcc-opt-46" "executable_runner ./test-binaries/strlen-gcc-opt-4" "executable_runner ./test-binaries/strlen-gcc-opt-4g" \
    "executable_runner ./test-binaries/strlen-gcc-opt-4gf" "executable_runner ./test-binaries/strlen-gcc-opt-5" "executable_runner ./test-binaries/strlen-gcc-opt-63" "executable_runner ./test-binaries/strlen-gcc-opt-64" \
    "executable_runner ./test-binaries/strlen-gcc-opt-66" "executable_runner ./test-binaries/strlen-gcc-opt-68" "executable_runner ./test-binaries/strlen-gcc-opt-6" "executable_runner ./test-binaries/strlen-gcc-opt-71" \
    "executable_runner ./test-binaries/strlen-gcc-opt-74" "executable_runner ./test-binaries/strlen-gcc-opt-75" "executable_runner ./test-binaries/strlen-gcc-opt-76" "executable_runner ./test-binaries/strlen-gcc-opt-79" \
    "executable_runner ./test-binaries/strlen-gcc-opt-7" "executable_runner ./test-binaries/strlen-gcc-opt-81" "executable_runner ./test-binaries/strlen-gcc-opt-84" "executable_runner ./test-binaries/strlen-gcc-opt-87" \
    "executable_runner ./test-binaries/strlen-gcc-opt-88" "executable_runner ./test-binaries/strlen-gcc-opt-8" "executable_runner ./test-binaries/strlen-gcc-opt-92" "executable_runner ./test-binaries/strlen-gcc-opt-94" \
    "executable_runner ./test-binaries/strlen-gcc-opt-9" "executable_runner ./test-binaries/strlen-scc-0018" "executable_runner ./test-binaries/strlen-glibc-test" "executable_runner ./test-binaries/strlen-glibc-tst" \
    "executable_runner ./test-binaries/strlen-glibc-tst-rsi" "executable_runner ./test-binaries/strlen-cosmopolitan-test" "executable_runner ./test-binaries/strlen-nlibc" "executable_runner ./test-binaries/strlen-cloudlibc" \
    "executable_runner ./test-binaries/strlen-dietlibc" "executable_runner ./test-binaries/strlen-gnulib-test-u8" \
    \
    "executable_runner ./test-binaries/isdigit-llvm-project-test" "executable_runner ./test-binaries/isdigit-avr-libc-1" \
    "do_output_diff_test 'executable_runner ./test-binaries/isdigit-open-watcom-v2' ./test-data/outputs/isdigit-open-watcom-v2" "executable_runner ./test-binaries/isdigit-z88dk" \
    "executable_runner ./test-binaries/isdigit-embeddedartistry-libc" "executable_runner ./test-binaries/isdigit-pdclib" \
    \
    "executable_runner ./test-binaries/isalpha-llvm-project-test" "executable_runner ./test-binaries/isalpha-z88dk-test" "executable_runner ./test-binaries/isalpha-avr-libc-1" \
    "executable_runner ./test-binaries/isalpha-embeddedartistry-libc" "executable_runner ./test-binaries/isalpha-pdclib" \
    \
    "do_output_diff_test 'executable_runner ./test-binaries/putchar-pdclib' ./test-data/outputs/putchar-pdclib" \
    \
    "executable_runner ./test-binaries/strcmp-NetBSD-t" "executable_runner ./test-binaries/strcmp-llvm-project-test" "executable_runner ./test-binaries/strcmp-gcc-builtins" "executable_runner ./test-binaries/strcmp-gcc-execute-1" \
    "executable_runner ./test-binaries/strcmp-gcc-dg-1" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_1" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_2" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_3" \
    "executable_runner ./test-binaries/strcmp-gcc-dg-opt_5" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_6" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_9" "executable_runner ./test-binaries/strcmp-gcc-dg-opt_12" \
    "executable_runner ./test-binaries/strcmp-z88dk" "executable_runner ./test-binaries/strcmp-scc-0013" "executable_runner ./test-binaries/strcmp-glibc-test" "executable_runner ./test-binaries/strcmp-cosmopolitan-test" \
    "executable_runner ./test-binaries/strcmp-newlib-1" "executable_runner ./test-binaries/strcmp-nlibc" "executable_runner ./test-binaries/strcmp-cloudlibc-test" "executable_runner ./test-binaries/strcmp-avr-libc" \
    "executable_runner ./test-binaries/strcmp-arm-optimized-routines" "executable_runner ./test-binaries/strcmp-embeddedartistry-libc" "executable_runner ./test-binaries/strcmp-pdclib" \
    "executable_runner ./test-binaries/strcmp-libcmini" "executable_runner ./test-binaries/strcmp-llvm-project_fuzz" "executable_runner ./test-binaries/strcmp-compilerai-bug-reports-dietlibc_fast_correct" \
    \
    "executable_runner ./test-binaries/stat-llvm-project_test"
do
    eval "$i" && printf "Test '%s' succeeded\n" "$i" | tee -a "$TEMP_TESTS_RESULTS_FILE" >/dev/null || printf "Test '%s' failed with status $?\n" "$i" | tee -a "$TEMP_TESTS_RESULTS_FILE" >/dev/stderr &
done

# Wait for all tests to be over before exiting
wait

printf '%s tests failed out of %s tests\n' $(grep -cE '^Test .* failed with status .?.?.?$' "$TEMP_TESTS_RESULTS_FILE") $(<"$TEMP_TESTS_RESULTS_FILE" wc -l)
grep -qE '^Test .* failed with status .?.?.?$' "$TEMP_TESTS_RESULTS_FILE" && { echo 'Failed tests:'; grep -E '^Test .* failed with status .?.?.?$' "$TEMP_TESTS_RESULTS_FILE"; }
rm "$TEMP_TESTS_RESULTS_FILE"
