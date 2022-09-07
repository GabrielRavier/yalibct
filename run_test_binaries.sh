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
./test-binaries/printf-illumos-gate-printf-6961 $IS_PARALLEL
./test-binaries/printf-illumos-gate-printf-9511 $IS_PARALLEL
./test-binaries/printf-reactos-printf $IS_PARALLEL
./test-binaries/printf-littlekernel-printf_tests | sed 's/0x1p-1074/0x0.0000000000001p-1022/;s/0x1.ffffffffffffep-1023/0x0.fffffffffffffp-1022/;s/0X1P-1074/0X0.0000000000001P-1022/;s/0X1.FFFFFFFFFFFFEP-1023/0X0.FFFFFFFFFFFFFP-1022/' | diff -u - <(cat <<EOF
printf tests
numbers:
int8:  -12 0 -2
uint8: 244 0 254
int16: -1234 0 1234
uint16:64302 0 1234
int:   -12345678 0 12345678
uint:  4282621618 0 12345678
long:  -12345678 0 12345678
ulong: 18446744073697205938 0 12345678
longlong: -12345678 0 12345678
ulonglong: 18446744073697205938 0 12345678
ssize_t: -12345678 0 12345678
usize_t: 18446744073697205938 0 12345678
intmax_t: -12345678 0 12345678
uintmax_t: 18446744073697205938 0 12345678
ptrdiff_t: -12345678 0 12345678
ptrdiff_t (u): 18446744073697205938 0 12345678
hex:
uint8: f4 0 fe
uint16:fb2e 0 4d2
uint:  ff439eb2 0 bc614e
ulong: ffffffffff439eb2 0 bc614e
ulong: FF439EB2 0 BC614E
ulonglong: ffffffffff439eb2 0 bc614e
usize_t: ffffffffff439eb2 0 bc614e
alt/sign:
uint: 0xabcdef 0XABCDEF
int: +12345678 -12345678
int:  12345678 +12345678
formatting
int: a12345678a
int: a 12345678a
int: a12345678 a
int: a  12345678a
int: a12345678  a
int: a012345678a
int: a0012345678a
int: a12345678a
aba
a        ba
ab        a
athisisatesta
-02
-02
+02
 +2
-2000
 2000
+2000
      test
      test
test      
test      
a returned 1
ab returned 2
abc returned 3
abcd returned 4
abcde returned 5
abcdef returned 6
sprintf returns 25
0x00007ffd262ab070: 30 31 32 33 34 35 36 37 38 39 61 62 63 64 65 66 |0123456789abcdef
0x00007ffd262ab080: 30 31 32 33 34 35 36 37 38 00 00 00 00 00 00 00 |012345678.......
snprintf returns 25
0x00007ffd262ab070: 30 31 32 33 34 35 36 37 38 39 61 62 63 64 00 00 |0123456789abcd..
0x00007ffd262ab080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................
floating point printf tests
0xc000000000000000 -2.000000 -2.000000 -0x1p+1 -0X1P+1
0xbff0000000000000 -1.000000 -1.000000 -0x1p+0 -0X1P+0
0xbfe0000000000000 -0.500000 -0.500000 -0x1p-1 -0X1P-1
0x8000000000000000 -0.000000 -0.000000 -0x0p+0 -0X0P+0
0x0000000000000000 0.000000 0.000000 0x0p+0 0X0P+0
0x3f847ae147ae147b 0.010000 0.010000 0x1.47ae147ae147bp-7 0X1.47AE147AE147BP-7
0x3fb999999999999a 0.100000 0.100000 0x1.999999999999ap-4 0X1.999999999999AP-4
0x3fc999999999999a 0.200000 0.200000 0x1.999999999999ap-3 0X1.999999999999AP-3
0x3fd0000000000000 0.250000 0.250000 0x1p-2 0X1P-2
0x3fe0000000000000 0.500000 0.500000 0x1p-1 0X1P-1
0x3fe8000000000000 0.750000 0.750000 0x1.8p-1 0X1.8P-1
0x3ff0000000000000 1.000000 1.000000 0x1p+0 0X1P+0
0x4000000000000000 2.000000 2.000000 0x1p+1 0X1P+1
0x4008000000000000 3.000000 3.000000 0x1.8p+1 0X1.8P+1
0x4024000000000000 10.000000 10.000000 0x1.4p+3 0X1.4P+3
0x4059000000000000 100.000000 100.000000 0x1.9p+6 0X1.9P+6
0x40fe240000000000 123456.000000 123456.000000 0x1.e24p+16 0X1.E24P+16
0xc0fe240000000000 -123456.000000 -123456.000000 -0x1.e24p+16 -0X1.E24P+16
0x4081148440000000 546.564575 546.564575 0x1.114844p+9 0X1.114844P+9
0xc081148440000000 -546.564575 -546.564575 -0x1.114844p+9 -0X1.114844P+9
0x3fbf9a6b50b0f27c 0.123450 0.123450 0x1.f9a6b50b0f27cp-4 0X1.F9A6B50B0F27CP-4
0x3eb4b6231abfd271 0.000001 0.000001 0x1.4b6231abfd271p-20 0X1.4B6231ABFD271P-20
0x3ec0c6c0a6f639de 0.000002 0.000002 0x1.0c6c0a6f639dep-19 0X1.0C6C0A6F639DEP-19
0x3eb92a737110e454 0.000002 0.000002 0x1.92a737110e454p-20 0X1.92A737110E454P-20
0x4005bf0a8b145649 2.718282 2.718282 0x1.5bf0a8b145649p+1 0X1.5BF0A8B145649P+1
0x400921fb54442d18 3.141593 3.141593 0x1.921fb54442d18p+1 0X1.921FB54442D18P+1
0x43f0000000000000 18446744073709551616.000000 18446744073709551616.000000 0x1p+64 0X1P+64
0x7fefffffffffffff 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000 0x1.fffffffffffffp+1023 0X1.FFFFFFFFFFFFFP+1023
0x0010000000000000 0.000000 0.000000 0x1p-1022 0X1P-1022
0x0000000000000001 0.000000 0.000000 0x0.0000000000001p-1022 0X0.0000000000001P-1022
0x000fffffffffffff 0.000000 0.000000 0x0.fffffffffffffp-1022 0X0.FFFFFFFFFFFFFP-1022
0x7ff0000000000001 nan NAN nan NAN
0x7ff7ffffffffffff nan NAN nan NAN
0x7ff8000000000000 nan NAN nan NAN
0x7fffffffffffffff nan NAN nan NAN
0xfff0000000000000 -inf -INF -inf -INF
0x7ff0000000000000 inf INF inf INF
EOF
                                                                                                                                                                                                                                                     ) $IS_PARALLEL
./test-binaries/printf-toaruos-test-printf | diff -u - <(cat <<EOF
042
0000012345
0

1
123
EOF
                                                       ) $IS_PARALLEL
./test-binaries/printf-newsys-test-printf $IS_PARALLEL
./test-binaries/printf-osv-tst-printf $IS_PARALLEL
./test-binaries/printf-OpenBSD-fp $IS_PARALLEL
./test-binaries/printf-OpenBSD-int $IS_PARALLEL
./test-binaries/printf-OpenBSD-string $IS_PARALLEL
./test-binaries/printf-llvm-project-printf_test | diff -u - <(cat <<EOF
A simple string with no conversions.
1234567890
1234 and more
EOF
                                                              ) $IS_PARALLEL
./test-binaries/printf-gcc-printf | diff -u - <(cat <<EOF
hello
hello world
hello world

hh

hello world
hello world
hello

hello
EOF
                                                ) $IS_PARALLEL

# Wait for all tests to be over before exiting
wait
