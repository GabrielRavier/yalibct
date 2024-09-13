#!/usr/bin/env bash
set -euo pipefail

# Execute tests from the directory that which contains the script
cd "$(dirname "$0")"

trap_exit ()
{
    echo "A command run from this script failed !"
}

trap trap_exit ERR




# Require 1 argument
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 directory-with-test-binaries"
    echo "Note: directory-with-test-binaries should normally be your CMake build directory"
    exit 1
fi

TESTS_BINARIES_DIRECTORY=$(realpath "$1")

# Commented out for now, but will likely have to be reintroduced whenether we start testing allocator failures
# export ASAN_OPTIONS=${ASAN_OPTIONS}:allocator_may_return_null=true

# Modify this function if you want to get something like valgrind or other running
executable_runner()
{
    "$1" "${@:2}"
}

get_test_executable_path()
{
    realpath "${TESTS_BINARIES_DIRECTORY}"/"$1"
}

test_runner()
{
    local TEST_EXECUTABLE_PATH
    TEST_EXECUTABLE_PATH="$(get_test_executable_path "$1")"
    executable_runner "${TEST_EXECUTABLE_PATH}" "${@:2}"
}

# Scrapped idea to have the runner create a temporary directory for each test - tests need to manage this themselves
# Note that it fails to check for mktemp failing for now, too
#test_runner()
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

# Needed to make tests based on MALLOC_TRACE (and stuff like mcheck) work
checked_add_to_ld_preload()
{
    # We need to temporarily disable pipefail so that the || test only and solely uses the grep process as the status for the tests
    set +o pipefail
    # shellcheck disable=SC2310
    { LD_PRELOAD="${LD_PRELOAD-} $1" test_runner libc-starts-up 2>&1 | grep -q . || LD_PRELOAD="${LD_PRELOAD-} $1" /bin/true 2>&1 | grep -q . || export LD_PRELOAD="${LD_PRELOAD-} $1"; } || true
    set -o pipefail
}

checked_add_to_ld_preload /lib64/libc_malloc_debug.so.0
checked_add_to_ld_preload /lib/libc_malloc_debug.so.0
checked_add_to_ld_preload /lib64/libc_malloc_debug.so
checked_add_to_ld_preload /lib/libc_malloc_debug.so
export MALLOC_CHECK_=3
export MALLOC_PERTURB_=$((RANDOM % 255 + 1))

do_mtrace_test()
{
    export MALLOC_TRACE
    MALLOC_TRACE=$(mktemp)
    test_runner "$@"

    local TEST_EXECUTABLE_PATH
    TEST_EXECUTABLE_PATH=$(get_test_executable_path "$1")
    mtrace "${TEST_EXECUTABLE_PATH}" "${MALLOC_TRACE}" | diff -u - <(echo 'No memory leaks.')

    rm "${MALLOC_TRACE}"
    unset MALLOC_TRACE
}

do_output_diff_test()
{
    eval "$1" | diff -u - "$2"
}

do_output_lines_only_in_executable_output_test()
{
    local TEMP_EXECUTABLE_SORTED_OUTPUT_FILE
    TEMP_EXECUTABLE_SORTED_OUTPUT_FILE=$(mktemp)
    eval "$1" | sort >"${TEMP_EXECUTABLE_SORTED_OUTPUT_FILE}"

    # Optimisation: if both the files are the same, it's impossible that the first file will contain any lines not in the second one
    cmp -s "${TEMP_EXECUTABLE_SORTED_OUTPUT_FILE}" "$2" && { rm "${TEMP_EXECUTABLE_SORTED_OUTPUT_FILE}"; return 0; }

    local THIS_FUNC_EXIT_STATUS
    # The (! command) syntax inverts the result of grep, meaning it will only exit with 0 if no matches are found (here, this means the input must be empty, e.g. that the are no lines that are only present in the test's output)
    <"${TEMP_EXECUTABLE_SORTED_OUTPUT_FILE}" comm -23 - "$2" | (! grep .)
    THIS_FUNC_EXIT_STATUS=$?

    rm "${TEMP_EXECUTABLE_SORTED_OUTPUT_FILE}"

    return "${THIS_FUNC_EXIT_STATUS}"
}

do_printf_littlekernel_tests()
{
    test_runner printf-littlekernel-tests | sed 's/0x1p-1074/0x0.0000000000001p-1022/;s/0x1.ffffffffffffep-1023/0x0.fffffffffffffp-1022/;s/0X1P-1074/0X0.0000000000001P-1022/;s/0X1.FFFFFFFFFFFFEP-1023/0X0.FFFFFFFFFFFFFP-1022/;s/0xf.ffffffffffffp-1026/0x0.fffffffffffffp-1022/;s/0XF.FFFFFFFFFFFFP-1026/0X0.FFFFFFFFFFFFFP-1022/' | diff -u - ./test-data/outputs/printf-littlekernel-tests
}

do_printf_NetBSD_t()
{
    test_runner printf-NetBSD-t 2>/dev/null | diff -u - <(printf 'printf = 0\n')
    test_runner printf-NetBSD-t 2>&1 >/dev/null | diff -u - <(printf 'snprintf = 0')
}

do_printf_glibc_tst_bz18872_sh_output()
{
    do_output_diff_test "do_mtrace_test printf-glibc-tst-bz18872-sh-output" ./test-data/outputs/printf-glibc-tst-bz18872-sh-output
}

do_printf_glibc_test_ldbl_compat()
{
    do_output_diff_test "test_runner printf-glibc-test-ldbl-compat | sed 's/\-0x1.0000000000p+0/-0x8.0000000000p-3/g'" ./test-data/outputs/printf-glibc-test-ldbl-compat
}

# shellcheck disable=SC2310
do_printf_gnulib_test_posix2()
{
    test_runner printf-gnulib-test-posix2 1 >/dev/null || echo "FAILED printf-gnulib-test-posix2 1"
    test_runner printf-gnulib-test-posix2 2 >/dev/null || echo "FAILED printf-gnulib-test-posix2 2"
    test_runner printf-gnulib-test-posix2 3 >/dev/null || echo "FAILED printf-gnulib-test-posix2 3"
    test_runner printf-gnulib-test-posix2 4 >/dev/null || echo "FAILED printf-gnulib-test-posix2 4"
    test_runner printf-gnulib-test-posix2 5 >/dev/null || echo "FAILED printf-gnulib-test-posix2 5"
    test_runner printf-gnulib-test-posix2 6 >/dev/null || echo "FAILED printf-gnulib-test-posix2 6"
}


[[ ! -e "${TESTS_BINARIES_DIRECTORY}" ]] && { echo "Literally none of the tests will run correctly if the binaries aren't present, so please build this project such that the binaries are in '${TESTS_BINARIES_DIRECTORY}'..."; exit 1; }

TEMP_TESTS_RESULTS_FILE=$(mktemp)

run_one_test()
{
    # Try and only run as many tests as there are processors simultaneously
    # As well as reducing system load (which is likely to be very annoying if someone's trying to do anything else at all at the same time), this improves performance by avoiding contention between the tests
    # We also ask nproc to ignore 2 CPUs, in an attempt to minimize the contention a bit - this is effectively a hardcoded attempt to leave some resources for the system and its likely a better solution could be found (though likely it would be much more complex), and this is just what I found to give best performance on the laptop on which I tested these changes
    local JOBS_COUNT
    local PROCESSOR_COUNT
    JOBS_COUNT=$(jobs | wc -l)
    PROCESSOR_COUNT=$(nproc --ignore=2)

    while [[ "${JOBS_COUNT}" -gt "${PROCESSOR_COUNT}" ]]; do
        wait -n
        JOBS_COUNT=$(jobs | wc -l)
    done

    { eval "$1" |& { ! grep . 1>&2; } && printf "Test '%s' succeeded\n" "$1" | tee -a "${TEMP_TESTS_RESULTS_FILE}" >/dev/null; } || printf "Test '%s' failed with status $?\n" "$1" | tee -a "${TEMP_TESTS_RESULTS_FILE}" >/dev/stderr &
}

for i in \
    libc-starts-up \
    \
    printf-{KOS-mk4-test-positional,linux-kernel-test,FreeBSD-{printfloat,atf,plain}_test,fuchsia-tests,illumos-gate-{6961,9511},reactos,newsys-test,osv-tst,OpenBSD-{fp,int,string},gcc-2,wine-{msvcrt,ucrtbase}} \
    printf-{glibc-tst-{binary,ob,bz25691{,-musl-regression},round,ldbl-nonnormal},picolibc-{nul,tests,scanf},dietlibc{2,},cloudlibc-scanf_test,avr-libc-bug-35366-2_flt,libc-test-{1e9-oob,fmt-{g-{round,zeros},n}},pdclib} \
    printf-gnulib-{sizes-c99,directive-{a,f,ls,n},flag-{grouping,leftadjust,zero},infinite{,-long-double},long-double,positions,precision,sn-{directive-n,retval-c99,size1,truncation-c99},vsn-zerosize-c99} \
    printf-{cosmopolitan-{kprintf_test,uppercase-{e,f,g}-static-yoink-test},nanoprintf-{conformance,unit_binary}} \
    \
    strcat-{NetBSD-t,llvm-project-test,gcc,cc65-lib_common,scc-0011,{glibc,cosmopolitan,cloudlibc}-test,gnulib-test-u8,pdclib,embeddedartistry-libc} \
    \
    strlen-{linux-kernel,open-posix-testsuite-1-1,NetBSD-t,arm-optimized-routines,reactos-crt,llvm-project-test,embeddedartistry-libc,gcc-{builtins{,-{2,3}},execute-{1,2,3,4,5,6,7},ldist-{1,2},opt-{10,11,12{,g},13,14g{,f}}}} \
    strlen-{gcc-opt-{15,16g,17g,18g,19,1,1f,20,21,22{,g},23,24,25,26,27,28,29,2,2f,31{,g},32,33{,g},34,35,3,46,4,4g,4gf,5,63,64,66,68,6,71,74,75,76,79,7,81,84,87,88,8,92,94,9},scc-0018,glibc-{test,tst{,-rsi}},cosmopolitan-test} \
    strlen-{nlibc,cloudlibc,dietlibc,gnulib-test-u8} \
    \
    isdigit-{llvm-project-test,avr-libc-1,z88dk,embeddedartistry-libc,pdclib} \
    \
    isalpha-{llvm-project-test,z88dk-test,avr-libc-1,embeddedartistry-libc,pdclib} \
    \
    strcmp-{NetBSD-t,llvm-project-test,gcc-{builtins,execute-1,dg-{1,opt_{1,2,3,5,6,9,12}}},z88dk,scc-0013,{glibc,cosmopolitan}-test,newlib-1,nlibc,cloudlibc-test,avr-libc,arm-optimized-routines,embeddedartistry-libc,pdclib} \
    strcmp-{libcmini,llvm-project_fuzz,compilerai-bug-reports-dietlibc_fast_correct} \
    \
    stat-{llvm-project_test,binutils-{1,2,3,4,5},cygwin-0{1,2,3,5,6},valgrind,gvisor{,_times_part{1,2,3,4,5,6}},linux-test-project-0{1,2},NetBSD-t,glibc-{test,tst{,-time64}},cosmopolitan_test,libc-test,gnulib}
do
    run_one_test "test_runner ${i}"
done

for i in \
    printf-{{FreeBSD-tap_,{toaruos,llvm-project}-}test,gcc{,-1},llvm-test-suite-2002-04-17-Char,tcc-02,glibc-tst{,-{sz,wc}},{dietlibc,dklibc-}test,gnulib-test-posix,mpaland-test_suite,duktape-test} \
    \
    isdigit-open-watcom-v2 \
    \
    putchar-pdclib \
    \
    stat-dietlibc
do
    run_one_test "do_output_diff_test 'test_runner ${i}' ./test-data/outputs/${i}"
done

for i in printf-glibc-tst-fp-{free,leak}
do
    run_one_test "do_mtrace_test ${i}"
done

for i in 1 2 3 4 5
do
    run_one_test "do_output_lines_only_in_executable_output_test 'test_runner printf-libcmini_formatting-${i}' ./test-data/outputs/printf-libcmini_formatting-${i}-presorted"
done

for i in \
    do_printf_NetBSD_t do_printf_littlekernel_tests "test_runner printf-glibc-tst-sz-islongdouble | diff -u - <(printf '2k4k')" do_printf_glibc_tst_bz18872_sh_output do_printf_glibc_test_ldbl_compat \
    "do_output_diff_test 'test_runner printf-newlib-nul' <(echo 'MMMMMMMM')" do_printf_gnulib_test_posix2 "test_runner printf-gnulib-enomem >/dev/null" \
    "do_output_diff_test 'test_runner stat-glibc-test-2 . CMakeLists.txt tests LICENSE TODO' test-data/outputs/stat-glibc-test-2"
do
    run_one_test "${i}"
done

# Wait for all tests to be over before exiting
wait


TESTS_RESULTS_FILE_LINES_COUNT="$(<"${TEMP_TESTS_RESULTS_FILE}" wc -l)"
printf '%s tests failed out of %s tests\n' "$(grep -cE '^Test .* failed with status .?.?.?$' "${TEMP_TESTS_RESULTS_FILE}" || true)" "${TESTS_RESULTS_FILE_LINES_COUNT}"
grep -qE '^Test .* failed with status .?.?.?$' "${TEMP_TESTS_RESULTS_FILE}" && { echo 'Failed tests:'; grep -E '^Test .* failed with status .?.?.?$' "${TEMP_TESTS_RESULTS_FILE}"; }
rm "${TEMP_TESTS_RESULTS_FILE}"
