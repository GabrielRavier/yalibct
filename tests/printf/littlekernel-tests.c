/*
 * Copyright (c) 2008-2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "test-deps/littlekernel/littlekernel.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

// We're doing a few things here that the compiler doesn't like, so disable printf warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"

void printf_tests(void) {
    printf("printf tests\n");

    printf("numbers:\n");
#ifndef YALIBCT_DISABLE_PRINTF_HH_LENGTH_MODIFIER_TESTS
    printf("int8:  %hhd %hhd %hhd\n", -12, 0, 254);
    printf("uint8: %hhu %hhu %hhu\n", -12, 0, 254);
#else
    puts("int8:  -12 0 -2\nuint8: 244 0 254");
#endif
    printf("int16: %hd %hd %hd\n", -1234, 0, 1234);
    printf("uint16:%hu %hu %hu\n", -1234, 0, 1234);
    printf("int:   %d %d %d\n", -12345678, 0, 12345678);
    printf("uint:  %u %u %u\n", -12345678, 0, 12345678);
    printf("long:  %ld %ld %ld\n", -12345678L, 0L, 12345678L);
    printf("ulong: %lu %lu %lu\n", -12345678UL, 0UL, 12345678UL);

    printf("longlong: %lli %lli %lli\n", -12345678LL, 0LL, 12345678LL);
    printf("ulonglong: %llu %llu %llu\n", -12345678LL, 0LL, 12345678LL);
    printf("ssize_t: %zd %zd %zd\n", (ssize_t)-12345678, (ssize_t)0, (ssize_t)12345678);
    printf("usize_t: %zu %zu %zu\n", (size_t)-12345678, (size_t)0, (size_t)12345678);
    printf("intmax_t: %jd %jd %jd\n", (intmax_t)-12345678, (intmax_t)0, (intmax_t)12345678);
    printf("uintmax_t: %ju %ju %ju\n", (uintmax_t)-12345678, (uintmax_t)0, (uintmax_t)12345678);
#ifndef YALIBCT_DISABLE_PRINTF_T_LENGTH_MODIFIER_TESTS
    printf("ptrdiff_t: %td %td %td\n", (ptrdiff_t)-12345678, (ptrdiff_t)0, (ptrdiff_t)12345678);
    printf("ptrdiff_t (u): %tu %tu %tu\n", (ptrdiff_t)-12345678, (ptrdiff_t)0, (ptrdiff_t)12345678);
#else
    puts("ptrdiff_t: -12345678 0 12345678\nptrdiff_t (u): 18446744073697205938 0 12345678");
#endif

    printf("hex:\n");
    printf("uint8: %hhx %hhx %hhx\n", -12, 0, 254);
    printf("uint16:%hx %hx %hx\n", -1234, 0, 1234);
    printf("uint:  %x %x %x\n", -12345678, 0, 12345678);
    printf("ulong: %lx %lx %lx\n", -12345678UL, 0UL, 12345678UL);
    printf("ulong: %X %X %X\n", -12345678, 0, 12345678);
    printf("ulonglong: %llx %llx %llx\n", -12345678LL, 0LL, 12345678LL);
    printf("usize_t: %zx %zx %zx\n", (size_t)-12345678, (size_t)0, (size_t)12345678);

    printf("alt/sign:\n");
#if !defined(YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS) && !defined(YALIBCT_DISABLE_PRINTF_UPPERCASE_X_CONVERSION_SPECIFIER_TESTS)
    printf("uint: %#x %#X\n", 0xabcdef, 0xabcdef);
#else
    puts("uint: 0xabcdef 0XABCDEF");
#endif
    printf("int: %+d %+d\n", 12345678, -12345678);
    printf("int: % d %+d\n", 12345678, 12345678);

    printf("formatting\n");
    printf("int: a%8da\n", 12345678);
    printf("int: a%9da\n", 12345678);
    printf("int: a%-9da\n", 12345678);
    printf("int: a%10da\n", 12345678);
    printf("int: a%-10da\n", 12345678);
    printf("int: a%09da\n", 12345678);
    printf("int: a%010da\n", 12345678);
    printf("int: a%6da\n", 12345678);

    printf("a%1sa\n", "b");
    printf("a%9sa\n", "b");
    printf("a%-9sa\n", "b");
    printf("a%5sa\n", "thisisatest");

    printf("%03d\n", -2);       /* '-02' */
    printf("%0+3d\n", -2);      /* '-02' */
    printf("%0+3d\n", 2);       /* '+02' */
    printf("%+3d\n", 2);        /* ' +2' */
    printf("% 3d\n", -2000);    /* '-2000' */
    printf("% 3d\n", 2000);     /* ' 2000' */
    printf("%+3d\n", 2000);     /* '+2000' */
    printf("%10s\n", "test");   /* '      test' */
    printf("%010s\n", "test");  /* '      test' */
    printf("%-10s\n", "test");  /* 'test      ' */
    printf("%-010s\n", "test"); /* 'test      ' */

    int err;

    err = printf("a");
    printf(" returned %d\n", err);
    err = printf("ab");
    printf(" returned %d\n", err);
    err = printf("abc");
    printf(" returned %d\n", err);
    err = printf("abcd");
    printf(" returned %d\n", err);
    err = printf("abcde");
    printf(" returned %d\n", err);
    err = printf("abcdef");
    printf(" returned %d\n", err);

    /* make sure snprintf terminates at the right spot */
    char buf[32];

    memset(buf, 0, sizeof(buf));
    err = sprintf(buf, "0123456789abcdef012345678");
    printf("sprintf returns %d\n", err);
    hexdump8(buf, sizeof(buf));

    memset(buf, 0, sizeof(buf));
    err = snprintf(buf, 15, "0123456789abcdef012345678");
    printf("snprintf returns %d\n", err);
    hexdump8(buf, sizeof(buf));

}

#include "test-deps/littlekernel/float_test_vec.h"

void printf_tests_float(void) {
    printf("floating point printf tests\n");

#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
    for (size_t i = 0; i < float_test_vec_size; i++)
        PRINT_FLOAT;
#else
    puts("0xc000000000000000 -2.000000 -2.000000 -0x1p+1 -0X1P+1\n"
         "0xbff0000000000000 -1.000000 -1.000000 -0x1p+0 -0X1P+0\n"
         "0xbfe0000000000000 -0.500000 -0.500000 -0x1p-1 -0X1P-1\n"
         "0x8000000000000000 -0.000000 -0.000000 -0x0p+0 -0X0P+0\n"
         "0x0000000000000000 0.000000 0.000000 0x0p+0 0X0P+0\n"
         "0x3f847ae147ae147b 0.010000 0.010000 0x1.47ae147ae147bp-7 0X1.47AE147AE147BP-7\n"
         "0x3fb999999999999a 0.100000 0.100000 0x1.999999999999ap-4 0X1.999999999999AP-4\n"
         "0x3fc999999999999a 0.200000 0.200000 0x1.999999999999ap-3 0X1.999999999999AP-3\n"
         "0x3fd0000000000000 0.250000 0.250000 0x1p-2 0X1P-2\n"
         "0x3fe0000000000000 0.500000 0.500000 0x1p-1 0X1P-1\n"
         "0x3fe8000000000000 0.750000 0.750000 0x1.8p-1 0X1.8P-1\n"
         "0x3ff0000000000000 1.000000 1.000000 0x1p+0 0X1P+0\n"
         "0x4000000000000000 2.000000 2.000000 0x1p+1 0X1P+1\n"
         "0x4008000000000000 3.000000 3.000000 0x1.8p+1 0X1.8P+1\n"
         "0x4024000000000000 10.000000 10.000000 0x1.4p+3 0X1.4P+3\n"
         "0x4059000000000000 100.000000 100.000000 0x1.9p+6 0X1.9P+6\n"
         "0x40fe240000000000 123456.000000 123456.000000 0x1.e24p+16 0X1.E24P+16\n"
         "0xc0fe240000000000 -123456.000000 -123456.000000 -0x1.e24p+16 -0X1.E24P+16\n"
         "0x4081148440000000 546.564575 546.564575 0x1.114844p+9 0X1.114844P+9\n"
         "0xc081148440000000 -546.564575 -546.564575 -0x1.114844p+9 -0X1.114844P+9\n"
         "0x3fbf9a6b50b0f27c 0.123450 0.123450 0x1.f9a6b50b0f27cp-4 0X1.F9A6B50B0F27CP-4\n"
         "0x3eb4b6231abfd271 0.000001 0.000001 0x1.4b6231abfd271p-20 0X1.4B6231ABFD271P-20\n"
         "0x3ec0c6c0a6f639de 0.000002 0.000002 0x1.0c6c0a6f639dep-19 0X1.0C6C0A6F639DEP-19\n"
         "0x3eb92a737110e454 0.000002 0.000002 0x1.92a737110e454p-20 0X1.92A737110E454P-20\n"
         "0x4005bf0a8b145649 2.718282 2.718282 0x1.5bf0a8b145649p+1 0X1.5BF0A8B145649P+1\n"
         "0x400921fb54442d18 3.141593 3.141593 0x1.921fb54442d18p+1 0X1.921FB54442D18P+1\n"
         "0x43f0000000000000 18446744073709551616.000000 18446744073709551616.000000 0x1p+64 0X1P+64\n"
         "0x7fefffffffffffff 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000 0x1.fffffffffffffp+1023 0X1.FFFFFFFFFFFFFP+1023\n"
         "0x0010000000000000 0.000000 0.000000 0x1p-1022 0X1P-1022\n"
         "0x0000000000000001 0.000000 0.000000 0x0.0000000000001p-1022 0X0.0000000000001P-1022\n"
         "0x000fffffffffffff 0.000000 0.000000 0x0.fffffffffffffp-1022 0X0.FFFFFFFFFFFFFP-1022\n"
         "0x7ff0000000000001 nan NAN nan NAN\n"
         "0x7ff7ffffffffffff nan NAN nan NAN\n"
         "0x7ff8000000000000 nan NAN nan NAN\n"
         "0x7fffffffffffffff nan NAN nan NAN\n"
         "0xfff0000000000000 -inf -INF -inf -INF\n"
         "0x7ff0000000000000 inf INF inf INF");
#endif

}

int main()
{
    printf_tests();
    printf_tests_float();
}

#pragma GCC diagnostic pop
