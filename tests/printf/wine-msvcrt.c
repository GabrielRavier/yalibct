/*
 * Conformance tests for *printf functions.
 *
 * Copyright 2002 Uwe Bonnes
 * Copyright 2004 Aneurin Price
 * Copyright 2005 Mike McCormack
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/* With Visual Studio >= 2005,  swprintf() takes an extra parameter unless
 * the following macro is defined.
 */
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "test-deps/wine.h"
#include "test-lib/chdir-to-tmpdir.h"
#include "test-lib/portable-symbols/wcscmp.h"
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <unistd.h>
#include <fenv.h>
#include <stdint.h>

#if defined(__GNUC__) && !defined(__clang__)
YALIBCT_DIAGNOSTIC_IGNORE("-Wunknown-pragmas")
#endif

#pragma STDC FENV_ACCESS ON

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat")
YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-security")
YALIBCT_DIAGNOSTIC_IGNORE("-Wcomment")

static inline float __port_ind(void)
{
    static const uint32_t __ind_bytes = 0xffc00000;
    // breaks strict aliasing rules
    //return *(const float *)&__ind_bytes;

    float result;
    memcpy(&result, &__ind_bytes, sizeof(float));
    return result;
}
#define IND __port_ind()

static int (__cdecl *p__vscprintf)(const char *format, va_list valist);
static int (__cdecl *p__vscwprintf)(const wchar_t *format, va_list valist);
static int (__cdecl *p__vsnwprintf_s)(wchar_t *str, size_t sizeOfBuffer,
                                      size_t count, const wchar_t *format,
                                      va_list valist);
static int (__cdecl *p__ecvt_s)(char *buffer, size_t length, double number,
                                int ndigits, int *decpt, int *sign);
static int (__cdecl *p__fcvt_s)(char *buffer, size_t length, double number,
                                int ndigits, int *decpt, int *sign);
static unsigned int (__cdecl *p__get_output_format)(void);
static unsigned int (__cdecl *p__set_output_format)(unsigned int);
#define p_sprintf sprintf
static int (__cdecl *p__vsprintf_p)(char*, size_t, const char*, va_list);
#define p_vswprintf vswprintf
#define p__vswprintf vswprintf
static int (__cdecl *p__vswprintf_l)(wchar_t *str, const wchar_t *format,
                                     void *locale, va_list valist);
static int (__cdecl *p__vswprintf_c)(wchar_t *str, size_t size, const wchar_t *format,
                                     va_list valist);
static int (__cdecl *p__vswprintf_c_l)(wchar_t *str, size_t size, const wchar_t *format,
                                       void *locale, va_list valist);
static int (__cdecl *p__vswprintf_p_l)(wchar_t *str, size_t size, const wchar_t *format,
                                       void *locale, va_list valist);

static void init( void )
{
}

static void test_sprintf( void )
{
    enum {
        NO_ARG,
        INT_ARG,
        ULONGLONG_ARG,
        DOUBLE_ARG,
        PTR_ARG,
        TODO_FLAG = 0x1000
    };

    struct {
        const char *format;
        const char *out;
        const char *broken;
        int type;
        int arg_i;
        ULONGLONG arg_ull;
        double arg_d;
        const void *arg_ptr;
    } tests[] = {
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
        { "%+#23.15e", " +7.894561230000000e+08", 0, DOUBLE_ARG, 0, 0, 789456123 },
        { "%-#23.15e", "7.894561230000000e+08  ", 0, DOUBLE_ARG, 0, 0, 789456123 },
        { "%#23.15e", "  7.894561230000000e+08", 0, DOUBLE_ARG, 0, 0, 789456123 },
#endif
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
        { "%#1.1g", "8.e+08", 0, DOUBLE_ARG, 0, 0, 789456123 },
#endif
        // UB and not widely supported
        /*{ "%I64d", "-8589934591", 0, ULONGLONG_ARG, 0, ((ULONGLONG)0xffffffff)*0xffffffff },
        { "%+8I64d", "    +100", 0, ULONGLONG_ARG, 0, 100 },
        { "%+.8I64d", "+00000100", 0, ULONGLONG_ARG, 0, 100 },
        { "%+10.8I64d", " +00000100", 0, ULONGLONG_ARG, 0, 100 },
        { "%_1I64d", "_1I64d", 0, ULONGLONG_ARG, 0, 100 },
        { "%-1.5I64d", "-00100", 0, ULONGLONG_ARG, 0, -100 },
        { "%5I64d", "  100", 0, ULONGLONG_ARG, 0, 100 },
        { "%5I64d", " -100", 0, ULONGLONG_ARG, 0, -100 },
        { "%-5I64d", "100  ", 0, ULONGLONG_ARG, 0, 100 },
        { "%-5I64d", "-100 ", 0, ULONGLONG_ARG, 0, -100 },
        { "%-.5I64d", "00100", 0, ULONGLONG_ARG, 0, 100 },
        { "%-.5I64d", "-00100", 0, ULONGLONG_ARG, 0, -100 },
        { "%-8.5I64d", "00100   ", 0, ULONGLONG_ARG, 0, 100 },
        { "%-8.5I64d", "-00100  ", 0, ULONGLONG_ARG, 0, -100 },
        { "%05I64d", "00100", 0, ULONGLONG_ARG, 0, 100 },
        { "%05I64d", "-0100", 0, ULONGLONG_ARG, 0, -100 },
        { "% I64d", " 100", 0, ULONGLONG_ARG, 0, 100 },
        { "% I64d", "-100", 0, ULONGLONG_ARG, 0, -100 },
        { "% 5I64d", "  100", 0, ULONGLONG_ARG, 0, 100 },
        { "% 5I64d", " -100", 0, ULONGLONG_ARG, 0, -100 },
        { "% .5I64d", " 00100", 0, ULONGLONG_ARG, 0, 100 },
        { "% .5I64d", "-00100", 0, ULONGLONG_ARG, 0, -100 },
        { "% 8.5I64d", "   00100", 0, ULONGLONG_ARG, 0, 100 },
        { "% 8.5I64d", "  -00100", 0, ULONGLONG_ARG, 0, -100 },
        { "%.0I64d", "", 0, ULONGLONG_ARG },
        { "%#+21.18I64x", " 0x00ffffffffffffff9c", 0, ULONGLONG_ARG, 0, -100 },
        { "%#.25I64o", "0001777777777777777777634", 0, ULONGLONG_ARG, 0, -100 },
        { "%#+24.20I64o", " 01777777777777777777634", 0, ULONGLONG_ARG, 0, -100 },
        { "%#+18.21I64X", "0X00000FFFFFFFFFFFFFF9C", 0, ULONGLONG_ARG, 0, -100 },
        { "%#+20.24I64o", "001777777777777777777634", 0, ULONGLONG_ARG, 0, -100 },
        { "%#+25.22I64u", "   0018446744073709551615", 0, ULONGLONG_ARG, 0, -1 },
        { "%#+25.22I64u", "   0018446744073709551615", 0, ULONGLONG_ARG, 0, -1 },
        { "%#+30.25I64u", "     0000018446744073709551615", 0, ULONGLONG_ARG, 0, -1 },
        { "%+#25.22I64d", "  -0000000000000000000001", 0, ULONGLONG_ARG, 0, -1 },
        { "%#-8.5I64o", "00144   ", 0, ULONGLONG_ARG, 0, 100 },
        { "%#-+ 08.5I64d", "+00100  ", 0, ULONGLONG_ARG, 0, 100 },
        { "%.80I64d",
            "00000000000000000000000000000000000000000000000000000000000000000000000000000001",
            0, ULONGLONG_ARG, 0, 1 },
        { "% .80I64d",
            " 00000000000000000000000000000000000000000000000000000000000000000000000000000001",
            0, ULONGLONG_ARG, 0, 1 },*/
        { "% .80d",
            " 00000000000000000000000000000000000000000000000000000000000000000000000000000001",
            0, INT_ARG, 1 },
        // UB and not widely supported
        /*{ "%I", "I", 0, INT_ARG, 1 },
        { "%Iq", "Iq", 0, INT_ARG, 1 },
        { "%Ihd", "Ihd", 0, INT_ARG, 1 },
        { "%I0d", "I0d", 0, INT_ARG, 1 },
        { "%I64D", "D", 0, ULONGLONG_ARG, 0, -1 },
        { "%zx", "1", "zx", INT_ARG, 1 },
        { "%z", "z", 0, INT_ARG, 1 },
        { "%tx", "1", "tx", INT_ARG, 1 },
        { "%t", "t", 0, INT_ARG, 1 },*/
        { "% d", " 1", 0, INT_ARG, 1 },
        { "%+ d", "+1", 0, INT_ARG, 1 },
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_S_CONVERSION_SPECIFIER_TESTS
        { "%S", "wide", 0, PTR_ARG, 0, 0, 0, L"wide" },
#endif
        // UB and not widely supported
        /*{ "%04c", "0001", 0, INT_ARG, '1' },*/
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
        { "%-04c", "1   ", 0, INT_ARG, '1' },
#endif
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
        { "%#012x", "0x0000000001", 0, INT_ARG, 1 },
        { "%#012x", "000000000000", 0, INT_ARG, 0 },
        { "%#04.8x", "0x00000001", 0, INT_ARG, 1 },
        { "%#04.8x", "00000000", 0, INT_ARG, 0 },
#ifndef YALIBCT_DISABLE_PRINTF_MINUS_FLAG_TESTS
        { "%#-08.2x", "0x01    ", 0, INT_ARG, 1 },
        { "%#-08.2x", "00      ", 0, INT_ARG, 0 },
#endif
        { "%#.0x", "0x1", 0, INT_ARG, 1 },
        { "%#.0x", "", 0, INT_ARG, 0 },
        { "%#08o", "00000001", 0, INT_ARG, 1 },
        { "%#o", "01", 0, INT_ARG, 1 },
        { "%#o", "0", 0, INT_ARG, 0 },
#endif
        // UB and not widely supported
        //{ "%04s", "0foo", 0, PTR_ARG, 0, 0, 0, "foo" },
        { "%.1s", "f", 0, PTR_ARG, 0, 0, 0, "foo" },
        { "hello", "hello", 0, NO_ARG },
        // UB and not widely supported
        /*{ "%ws", "wide", 0, PTR_ARG, 0, 0, 0, L"wide" },
        { "%-10ws", "wide      ", 0, PTR_ARG, 0, 0, 0, L"wide" },
        { "%10ws", "      wide", 0, PTR_ARG, 0, 0, 0, L"wide" },
        { "%#+ -03whlls", "wide", 0, PTR_ARG, 0, 0, 0, L"wide" },
        { "%w0s", "0s", 0, PTR_ARG, 0, 0, 0, L"wide" },
        { "%w-s", "-s", 0, PTR_ARG, 0, 0, 0, L"wide" },*/
#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
        { "%ls", "wide", 0, PTR_ARG, 0, 0, 0, L"wide" },
#endif
        // UB and not widely supported
        /*{ "%Ls", "not wide", 0, PTR_ARG, 0, 0, 0, "not wide" },
          { "%b", "b", 0, NO_ARG },*/
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
        { "%3c", "  a", 0, INT_ARG, 'a' },
#endif
        { "%3d", "1234", 0, INT_ARG, 1234 },
        // UB and not widely supported
        /*{ "%3h", "", 0, NO_ARG },
          { "%k%m%q%r%t%v%y%z", "kmqrtvyz", 0, NO_ARG },*/
        { "%-1d", "2", 0, INT_ARG, 2 },
        { "%2.4f", "8.6000", 0, DOUBLE_ARG, 0, 0, 8.6 },
        { "%0f", "0.600000", 0, DOUBLE_ARG, 0, 0, 0.6 },
        { "%.0f", "1", 0, DOUBLE_ARG, 0, 0, 0.6 },
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
        { "%2.4e", "8.6000e+00", 0, DOUBLE_ARG, 0, 0, 8.6 },
        { "% 2.4e", " 8.6000e+00", 0, DOUBLE_ARG, 0, 0, 8.6 },
        { "% 014.4e", " 0008.6000e+00", 0, DOUBLE_ARG, 0, 0, 8.6 },
        { "% 2.4e", "-8.6000e+00", 0, DOUBLE_ARG, 0, 0, -8.6 },
        { "%+2.4e", "+8.6000e+00", 0, DOUBLE_ARG, 0, 0, 8.6 },
#endif
        { "%2.4g", "8.6", 0, DOUBLE_ARG, 0, 0, 8.6 },
        { "%-i", "-1", 0, INT_ARG, -1 },
        { "%-i", "1", 0, INT_ARG, 1 },
        { "%+i", "+1", 0, INT_ARG, 1 },
        { "%o", "12", 0, INT_ARG, 10 },
        { "%s", "(null)", 0, PTR_ARG, 0, 0, 0, NULL },
        { "%s", "%%%%", 0, PTR_ARG, 0, 0, 0, "%%%%" },
        { "%u", "4294967295", 0, INT_ARG, -1 },
        // UB and not widely supported
        /*{ "%w", "", 0, INT_ARG, -1 },
        { "%h", "", 0, INT_ARG, -1 },
        { "%j", "", "j", ULONGLONG_ARG, 0, -1 },
        { "%jd", "-1", "jd", ULONGLONG_ARG, 0, -1 },
        { "%F", "", 0, INT_ARG, -1 },
        { "%N", "", 0, INT_ARG, -1 },
        { "%H", "H", 0, INT_ARG, -1 },*/
        { "x%cx", "xXx", 0, INT_ARG, 0x100+'X' },
        { "%%0", "%0", 0, NO_ARG },
        { "%hx", "2345", 0, INT_ARG, 0x12345 },
        { "%hhx", "23", 0, INT_ARG, 0x123 },
        { "%hhx", "45", 0, INT_ARG, 0x12345 },
        // Non-standard and not widely supported
        /*{ "%lf", "-1.#IND00", 0, DOUBLE_ARG, 0, 0, IND },
        { "%lf", "1.#QNAN0", 0, DOUBLE_ARG, 0, 0, NAN },
        { "%lf", "1.#INF00", 0, DOUBLE_ARG, 0, 0, INFINITY },
        { "%le", "-1.#IND00e+000", 0, DOUBLE_ARG, 0, 0, IND },
        { "%le", "1.#QNAN0e+000", 0, DOUBLE_ARG, 0, 0, NAN },
        { "%le", "1.#INF00e+000", 0, DOUBLE_ARG, 0, 0, INFINITY },
        { "%lg", "-1.#IND", 0, DOUBLE_ARG, 0, 0, IND },
        { "%lg", "1.#QNAN", 0, DOUBLE_ARG, 0, 0, NAN },
        { "%lg", "1.#INF", 0, DOUBLE_ARG, 0, 0, INFINITY },
        { "%010.2lf", "-000001.#J", 0, DOUBLE_ARG, 0, 0, IND },
        { "%010.2lf", "0000001.#R", 0, DOUBLE_ARG, 0, 0, NAN },
        { "%010.2lf", "0000001.#J", 0, DOUBLE_ARG, 0, 0, INFINITY },*/
        { "%c", "a", 0, INT_ARG, 'a' },
        { "%c", "\x82", 0, INT_ARG, 0xa082 },
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS
        { "%C", "a", 0, INT_ARG, 'a' },
#endif
        // Implementation-defined and behavior is not usually like this
        /*{ "%C", "", 0, INT_ARG, 0x3042 },
          { "a%Cb", "ab", 0, INT_ARG, 0x3042 },*/
        { "%lld", "-8589934591", "1", ULONGLONG_ARG, 0, ((ULONGLONG)0xffffffff)*0xffffffff },
        // UB and not widely supported
        //{ "%I32d", "1", "I32d", INT_ARG, 1 },

#ifndef YALIBCT_DISABLE_PRINTF_ROUNDING_DIRECTION_TESTS
        { "%.0f", "-2", 0, DOUBLE_ARG, 0, 0, -1.5 },
        { "%.0f", "-0", 0, DOUBLE_ARG, 0, 0, -0.5 },
        { "%.0f", "0", 0, DOUBLE_ARG, 0, 0, 0.5 },
        { "%.0f", "2", 0, DOUBLE_ARG, 0, 0, 1.5 },
#endif

#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
        { "%.30f", "0.333333333333333314829616256247", 0, TODO_FLAG | DOUBLE_ARG, 0, 0, 1.0/3.0 },
        { "%.30lf", "1.414213562373095145474621858739", 0, TODO_FLAG | DOUBLE_ARG, 0, 0, sqrt(2) },
#endif
    };

    char buffer[100];
    int i, x, r;

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FESETROUND
    fesetround(FE_TONEAREST);
#endif
    for (i=0; i<ARRAY_SIZE(tests); i++) {
        memset(buffer, 'x', sizeof(buffer));
        switch(tests[i].type & 0xff) {
        case NO_ARG:
            r = p_sprintf(buffer, tests[i].format);
            break;
        case INT_ARG:
            r = p_sprintf(buffer, tests[i].format, tests[i].arg_i);
            break;
        case ULONGLONG_ARG:
            r = p_sprintf(buffer, tests[i].format, tests[i].arg_ull);
            break;
        case DOUBLE_ARG:
            r = p_sprintf(buffer, tests[i].format, tests[i].arg_d);
            break;
        case PTR_ARG:
            r = p_sprintf(buffer, tests[i].format, tests[i].arg_ptr);
            break;
        default:
            ok(0, "tests[%d].type = %x\n", i, tests[i].type);
            continue;
        }

        ok(r == strlen(buffer), "%d, expected '%s') r = %d, buffer = \"%s\"\n", i, tests[i].out, r, buffer);
        todo_wine_if(tests[i].type & TODO_FLAG)
        {
            ok(!strcmp(buffer, tests[i].out) ||
                    broken(tests[i].broken && !strcmp(buffer, tests[i].broken)),
               "%d, expected '%s') buffer = \"%s\"\n", i, tests[i].out, buffer);
        }
    }

    if (sizeof(void *) == 8)
    {
        r = p_sprintf(buffer, "%p", (void *)57);
        ok(r < ((sizeof(buffer) / 2) - 5), "Printed pointers are really large, this is just weird");
        ok(p_sprintf(buffer + sizeof(buffer) - r - 1, "%p", (void *)56) == r, "Pointers like this should probably print to the same size...");
        ok(strcmp(buffer, buffer + sizeof(buffer) - r - 1) != 0, "Different pointers should not print the same...");

        // Implementation defined and very inconsistent across implementations
        /*r = p_sprintf(buffer, "%#020p", (void *)57);
        ok(!strcmp(buffer,"  0X0000000000000039"),"Pointer formatted incorrectly '%s'\n", buffer);
        ok( r==20, "return count wrong\n");

        r = p_sprintf(buffer, "%Fp", (void *)57);
        ok(!strcmp(buffer,"0000000000000039"),"Pointer formatted incorrectly \"%s\"\n",buffer);
        ok( r==16, "return count wrong\n");

        r = p_sprintf(buffer, "%Np", (void *)57);
        ok(!strcmp(buffer,"0000000000000039"),"Pointer formatted incorrectly \"%s\"\n",buffer);
        ok( r==16, "return count wrong\n");

        r = p_sprintf(buffer, "%#-020p", (void *)57);
        ok(!strcmp(buffer,"0X0000000000000039  "),"Pointer formatted incorrectly\n");
        ok( r==20, "return count wrong\n");

        r = p_sprintf(buffer, "%Ix %d", (size_t)0x12345678123456,1);
        ok(!strcmp(buffer,"12345678123456 1"),"buffer = %s\n",buffer);
        ok( r==16, "return count wrong\n");

        r = p_sprintf(buffer, "%p", 0);
        ok(!strcmp(buffer,"0000000000000000"), "failed\n");
        ok( r==16, "return count wrong\n");*/
    }
    else
    {
        r = p_sprintf(buffer, "%p", (void *)57);
        ok(!strcmp(buffer,"00000039"),"Pointer formatted incorrectly \"%s\"\n",buffer);
        ok( r==8, "return count wrong\n");

        r = p_sprintf(buffer, "%#012p", (void *)57);
        ok(!strcmp(buffer,"  0X00000039"),"Pointer formatted incorrectly\n");
        ok( r==12, "return count wrong\n");

        r = p_sprintf(buffer, "%Fp", (void *)57);
        ok(!strcmp(buffer,"00000039"),"Pointer formatted incorrectly \"%s\"\n",buffer);
        ok( r==8, "return count wrong\n");

        r = p_sprintf(buffer, "%Np",(void *)57);
        ok(!strcmp(buffer,"00000039"),"Pointer formatted incorrectly \"%s\"\n",buffer);
        ok( r==8, "return count wrong\n");

        r = p_sprintf(buffer, "%#-012p", (void *)57);
        ok(!strcmp(buffer,"0X00000039  "),"Pointer formatted incorrectly\n");
        ok( r==12, "return count wrong\n");

        r = p_sprintf(buffer, "%Ix %d", 0x123456, 1);
        ok(!strcmp(buffer,"123456 1"),"buffer = %s\n",buffer);
        ok( r==8, "return count wrong\n");

        r = p_sprintf(buffer, "%p", (void *)0);
        ok(!strcmp(buffer,"00000000"), "failed\n");
        ok( r==8, "return count wrong\n");
    }

    r = p_sprintf(buffer, "%.*s", 1, "foo");
    ok(!strcmp(buffer,"f"),"Precision ignored \"%s\"\n",buffer);
    ok( r==1, "return count wrong\n");

    r = p_sprintf(buffer, "%*s", -5, "foo");
    ok(!strcmp(buffer,"foo  "),"Negative field width ignored \"%s\"\n",buffer);
    ok( r==5, "return count wrong\n");

#ifndef YALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS
    x = 0;
    r = p_sprintf(buffer, "asdf%n", &x );
    if (r == -1)
    {
        /* %n format is disabled by default on vista */
        /* FIXME: should test with _set_printf_count_output */
        ok(x == 0, "should not write to x: %d\n", x);
    }
    else
    {
        ok(x == 4, "should write to x: %d\n", x);
        ok(!strcmp(buffer,"asdf"), "failed\n");
        ok( r==4, "return count wrong: %d\n", r);
    }
#endif

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_S_CONVERSION_SPECIFIER_TESTS
    r = p_sprintf(buffer, "%S", L"\x3042");
    ok(r==-1 || r==3 || broken(!r), "r = %d\n", r);
#endif

    ok(setlocale(LC_ALL, "C") != NULL, "");

    // UB and not widely supported
    /*r = p_sprintf(buffer, "%*1d", 1, 3);
    ok(r==11, "r = %d\n", r);
    ok(!strcmp(buffer, "          3"), "failed: \"%s\"\n", buffer);

    r = p_sprintf(buffer, "%0*0d", 1, 2);
    ok(r==10, "r = %d\n", r);
    ok(!strcmp(buffer, "0000000002"), "failed: \"%s\"\n", buffer);

    r = p_sprintf(buffer, "% *2d", 0, 7);
    ok(r==2, "r = %d\n", r);
    ok(!strcmp(buffer, " 7"), "failed: \"%s\"\n", buffer);*/

    if(!setlocale(LC_ALL, "Japanese_Japan.932")) {
        win_skip("Japanese_Japan.932 locale not available\n");
        return;
    }

    r = p_sprintf(buffer, "%c", 0xa082);
    ok(r==1, "r = %d\n", r);
    ok(!strcmp(buffer, "\x82"), "failed: \"%s\"\n", buffer);

#ifndef YALIBCT_DISABLE_RARE_LOCALE_TESTS
    r = p_sprintf(buffer, "%C", 0x3042);
    ok(r==2, "r = %d\n", r);
    ok(!strcmp(buffer, "\x82\xa0"), "failed: \"%s\"\n", buffer);
#endif

    strcpy(buffer, " string to copy");
    r = p_sprintf(buffer, buffer+1);
    ok(r==14, "r = %d\n", r);
    ok(!strcmp(buffer, "string to copy"), "failed: \"%s\"\n", buffer);
}

static void test_swprintf( void )
{
    // Well this is some broken MSVC shit lmao
    /*wchar_t buffer[100];
    double pnumber = 789456123;
    const char string[] = "string";

    swprintf(buffer, L"%+#23.15e", pnumber);
    ok(wcsstr(buffer, L"e+008") != 0, "Sprintf different\n");
    swprintf(buffer, L"%I64d", ((ULONGLONG)0xffffffff)*0xffffffff);
    ok(wcslen(buffer) == 11, "Problem with long long\n");
    swprintf(buffer, L"%S", string);
    ok(wcslen(buffer) == 6, "Problem with \"%%S\" interpretation\n");
    swprintf(buffer, L"%hs", string);
    ok(!wcscmp(L"string", buffer), "swprintf failed with %%hs\n");*/
}

static void test_snprintf (void)
{
    struct snprintf_test {
        const char *format;
        int expected;
    };
    /* Pre-2.1 libc behaviour, not C99 compliant. */
    // Fixed to be C99 compliant
    const struct snprintf_test tests[] = {{"short", 5},
                                          {"justfit", 7},
                                          {"justfits", 8},
                                          {"muchlonger", 10}};
    char buffer[8];
    const int bufsiz = sizeof buffer;
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(tests); i++) {
        const char *fmt  = tests[i].format;
        const int expect = tests[i].expected;
        const int n      = snprintf (buffer, bufsiz, fmt);
        //const int valid  = n < 0 ? bufsiz : (n == bufsiz ? n : n+1);

        ok (n == expect, "\"%s\": expected %d, returned %d\n",
            fmt, expect, n);
    }
}

static void checked_fclose(FILE *stream)
{
    ok(fclose(stream) == 0, "");
}

static void checked_fgets(char *s, int size, FILE *stream)
{
    ok(fgets(s, size, stream) == s, "");
}

static void test_fprintf(void)
{
    FILE *fp = fopen("fprintf.tst", "wb");
    char buf[1024];
    int ret;

    ok(fp != NULL, "");

    ret = fprintf(fp, "simple test\n");
    ok(ret == 12, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 12, "ftell returned %d\n", ret);

    ret = fprintf(fp, "contains%cnull\n", '\0');
    ok(ret == 14, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 26, "ftell returned %d\n", ret);

    checked_fclose(fp);
    fp = fopen("fprintf.tst", "a");
    ok(fp != NULL, "");

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FWPRINTF
    ret = fwprintf(fp, L"unicode\n");
    ok(ret == 8, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 42 || ret == 34, "ftell returned %d\n", ret);
#endif

    checked_fclose(fp);

    fp = fopen("fprintf.tst", "rb");
#ifndef YALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS
    ret = fscanf(fp, "%[^\n] ", buf);
    ok(ret == 1, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 12, "ftell returned %d\n", ret);
    ok(!strcmp(buf, "simple test"), "buf = %s\n", buf);

    checked_fgets(buf, sizeof(buf), fp);
    ret = ftell(fp);
    ok(ret == 26, "ret = %d\n", ret);
    ok(!memcmp(buf, "contains\0null\n", 14), "buf = %s\n", buf);


#ifndef YALIBCT_LIBC_DOESNT_HAVE_FWPRINTF
    memset(buf, 0, sizeof(buf));
    checked_fgets(buf, sizeof(buf), fp);
    ret = ftell(fp);
    ok(ret == 41 || ret == 34, "ret = %d\n", ret);
    ok(!strcmp(buf, "unicode\n"), "buf = %ls\n", ((WCHAR*)buf));
#endif

#endif

    checked_fclose(fp);

    fp = fopen("fprintf.tst", "wt");

    ret = fprintf(fp, "simple test\n");
    ok(ret == 12, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 13 || ret == 12, "ftell returned %d\n", ret);

    ret = fprintf(fp, "contains%cnull\n", '\0');
    ok(ret == 14, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 28 || ret == 26, "ftell returned %d\n", ret);

    checked_fclose(fp);
    fp = fopen("fprintf.tst", "at");
    ok(fp != NULL, "");

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FWPRINTF
    ret = fwprintf(fp, L"unicode\n");
    ok(ret == 8, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 37 || ret == 34, "ftell returned %d\n", ret);
#endif

    checked_fclose(fp);

    fp = fopen("fprintf.tst", "rb");
#ifndef YALIBCT_DISABLE_SCANF_LEFT_SQUARE_BRACKET_CONVERSION_SPECIFIER_TESTS
    ret = fscanf(fp, "%[^\n] ", buf);
    ok(ret == 1, "ret = %d\n", ret);
    ret = ftell(fp);
    ok(ret == 13 || ret == 12, "ftell returned %d\n", ret);
    ok(!strcmp(buf, "simple test\r") || !strcmp(buf, "simple test"), "buf = %s\n", buf);

    checked_fgets(buf, sizeof(buf), fp);
    ret = ftell(fp);
    ok(ret == 28 || ret == 26, "ret = %d\n", ret);
    ok(!memcmp(buf, "contains\0null\r\n", 15) || !memcmp(buf, "contains\0null\n", 14), "buf = %s\n", buf);

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FWPRINTF
    checked_fgets(buf, sizeof(buf), fp);
    ret = ftell(fp);
    ok(ret == 37 || ret == 34, "ret = %d\n", ret);
    ok(!strcmp(buf, "unicode\r\n") || !strcmp(buf, "unicode\n"), "buf = %s\n", buf);
#endif

#endif

    checked_fclose(fp);
    unlink("fprintf.tst");
}

static void test_fcvt(void)
{
    char *str;
    int dec=100, sign=100;

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FCVT
    /* Numbers less than 1.0 with different precisions */
    str = fcvt(0.0001, 1, &dec, &sign );
    ok( 0 == strcmp(str,""), "bad return '%s'\n", str);
    ok( dec < 0, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.0001, -10, &dec, &sign );
    ok( str, "bad return '%s'\n", str);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.0001, 10, &dec, &sign );
    ok( 0 == strcmp(str,"1000000"), "bad return '%s'\n", str);
    ok( dec < 0, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    /* Basic sign test */
    str = fcvt(-111.0001, 5, &dec, &sign );
    ok( 0 == strcmp(str,"11100010"), "bad return '%s'\n", str);
    ok( 3 == dec, "dec wrong %d\n", dec);
    ok( 1 == sign, "sign wrong\n");

    str = fcvt(111.0001, 5, &dec, &sign );
    ok( 0 == strcmp(str,"11100010"), "bad return '%s'\n", str);
    ok( 3 == dec, "dec wrong\n");
    ok( 0 == sign, "sign wrong\n");

    /* 0.0 with different precisions */
    str = fcvt(0.0, 5, &dec, &sign );
    ok( 0 == strcmp(str,"00000") || !strcmp(str, "000000"), "bad return '%s'\n", str);
    ok( 0 == dec || dec == 1, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.0, 0, &dec, &sign );
    ok( 0 == strcmp(str,"") || !strcmp(str, "0"), "bad return '%s'\n", str);
    ok( 0 == dec || 1 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.0, -1, &dec, &sign );
    ok( str, "bad return '%s'\n", str);
    ok( 0 == sign, "sign wrong\n");

    /* Numbers > 1.0 with 0 or -ve precision */
    str = fcvt(-123.0001, 0, &dec, &sign );
    ok( 0 == strcmp(str,"123"), "bad return '%s'\n", str);
    ok( 3 == dec, "dec wrong %d\n", dec);
    ok( 1 == sign, "sign wrong\n");

    str = fcvt(-123.0001, -1, &dec, &sign );
    ok( str, "bad return '%s'\n", str);
    ok( 1 == sign, "sign wrong\n");

    str = fcvt(-123.0001, -2, &dec, &sign );
    ok( str, "bad return '%s'\n", str);
    ok( 1 == sign, "sign wrong\n");

    str = fcvt(-123.0001, -3, &dec, &sign );
    ok( str, "bad return '%s'\n", str);
    ok( 1 == sign, "sign wrong\n");

    /* Numbers > 1.0, but with rounding at the point of precision */
    str = fcvt(99.99, 1, &dec, &sign );
    ok( 0 == strcmp(str,"1000"), "bad return '%s'\n", str);
    ok( 3 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    /* Numbers < 1.0 where rounding occurs at the point of precision */
    str = fcvt(0.00636, 2, &dec, &sign );
    ok( 0 == strcmp(str,"1"), "bad return '%s'\n", str);
    ok( -1 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.00636, 3, &dec, &sign );
    ok( 0 == strcmp(str,"6"), "bad return '%s'\n", str);
    ok( -2 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.09999999996, 2, &dec, &sign );
    ok( 0 == strcmp(str,"10"), "bad return '%s'\n", str);
    ok( 0 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");

    str = fcvt(0.6, 0, &dec, &sign );
    ok( 0 == strcmp(str,"1"), "bad return '%s'\n", str);
    ok( 1 == dec, "dec wrong %d\n", dec);
    ok( 0 == sign, "sign wrong\n");
#endif
}

/* Don't test nrdigits < 0, msvcrt on Win9x and NT4 will corrupt memory by
 * writing outside allocated memory */
static struct {
    double value;
    int nrdigits;
    const char *expstr_e;
    const char *expstr_f;
    int expdecpt_e;
    int expdecpt_f;
    int expsign;
    const char *expstr_f2;
    int expdecpt_f2;
    int expdecpt_e2;
} test_cvt_testcases[] = {
    {          45.0,   2,        "45",           "4500",          2,      2,      0 },
    {        0.0001,   1,         "1",               "",         -3,     -3,     0 },
    {        0.0001,  10,"1000000000",        "1000000",         -3,     -3,     0 },
    {     -111.0001,   5,     "11100",       "11100010",          3,      3,     1 },
    {      111.0001,   5,     "11100",       "11100010",          3,      3,     0 },
    {        3333.3,   2,        "33",         "333330",          4,      4,     0 },
    {999999999999.9,   3,       "100","999999999999900",         13,     12,     0 },
    {           0.0,   5,     "00000",          "00000",          0,      0,     0, "000000", 1, 1 },
    {           0.0,   0,          "",               "",          0,      0,     0, "0", 1 },
    // negative ndigits is UB and not widely supported
//    {           0.0,  -1,          "",               "",          0,      0,     0, "0", 1 },
    {          -0.0,   5,     "00000",          "00000",          0,      0,     1, "000000", 1, 1 },
    {          -0.0,   0,          "",               "",          0,      0,     1, "0", 1 },
    // negative ndigits is UB and not widely supported
//    {          -0.0,  -1,          "",               "",          0,      0,     1, "0", 1 },
    {     -123.0001,   0,          "",            "123",          0,      3,     1, NULL, 0, 3 },
    // negative ndigits is UB and not widely supported
/*    {     -123.0001,  -1,          "",             "12",          3,      3,     1 },
    {     -123.0001,  -2,          "",              "1",          3,      3,     1 },
    {     -123.0001,  -3,          "",               "",          3,      3,     1 },*/
    {         99.99,   1,         "1",           "1000",          3,      3,     0 },
    {        0.0063,   2,        "63",              "1",         -2,     -1,     0 },
    {        0.0063,   3,        "630",             "6",         -2,     -2,     0 },
    { 0.09999999996,   2,        "10",             "10",          0,      0,     0 },
    {           0.6,   1,         "6",              "6",          0,      0,     0 },
    {           0.6,   0,          "",              "1",          0,      1,     0, NULL, 0, 1 },
    {           0.4,   0,          "",               "",          0,      0,     0, "0", 1 },
    {          0.49,   0,          "",               "",          0,      0,     0, "0", 1 },
    {          0.51,   0,          "",              "1",          0,      1,     0, NULL, 0, 1 },
    // Unspecified and inconsistent across implementations
    /*{           NAN,   2,        "1$",            "1#R",          1,      1,     0 },
    {           NAN,   5,     "1#QNB",         "1#QNAN",          1,      1,     0 },
    {          -NAN,   2,        "1$",            "1#J",          1,      1,     1 },
    {          -NAN,   5,     "1#IND",         "1#IND0",          1,      1,     1 },
    {      INFINITY,   2,        "1$",            "1#J",          1,      1,     0 },
    {      INFINITY,   5,     "1#INF",         "1#INF0",          1,      1,     0 },
    {     -INFINITY,   2,        "1$",            "1#J",          1,      1,     1 },
    {     -INFINITY,   5,     "1#INF",         "1#INF0",          1,      1,     1 },*/
    {           1.0,  30, "100000000000000000000000000000",
                      "1000000000000000000000000000000",          1,      1,      0},
    {           123456789012345678901.0,  30, "123456789012345680000000000000",
                      "123456789012345680000000000000000000000000000000000",         21,    21,      0},
    { 0, 0, "END"}
};

static void test_xcvt(void)
{
    char *str;
    int i, decpt, sign, err;

#ifndef YALIBCT_LIBC_DOESNT_HAVE_ECVT
    for( i = 0; strcmp( test_cvt_testcases[i].expstr_e, "END") != 0; i++){
        decpt = sign = 100;
        str = ecvt( test_cvt_testcases[i].value,
                test_cvt_testcases[i].nrdigits,
                &decpt,
                &sign);
        ok( !strncmp( str, test_cvt_testcases[i].expstr_e, 15),
               "%d) ecvt() bad return, got '%s' expected '%s'\n", i, str,
              test_cvt_testcases[i].expstr_e);
        ok( decpt == test_cvt_testcases[i].expdecpt_e || (test_cvt_testcases[i].expdecpt_e2 != 0 && decpt == test_cvt_testcases[i].expdecpt_e2),
                "%d) ecvt() decimal point wrong, got %d expected %d\n", i, decpt,
                test_cvt_testcases[i].expdecpt_e);
        ok( sign == test_cvt_testcases[i].expsign,
                "%d) ecvt() sign wrong, got %d expected %d\n", i, sign,
                test_cvt_testcases[i].expsign);
    }
#endif

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FCVT
    for( i = 0; strcmp( test_cvt_testcases[i].expstr_e, "END") != 0; i++){
        decpt = sign = 100;
        str = fcvt( test_cvt_testcases[i].value,
                test_cvt_testcases[i].nrdigits,
                &decpt,
                &sign);
        ok( !strncmp( str, test_cvt_testcases[i].expstr_f, 15) || (test_cvt_testcases[i].expstr_f2 && !strncmp( str, test_cvt_testcases[i].expstr_f2, 15)) || (test_cvt_testcases[i].nrdigits < 0 && str != NULL) ,
               "%d) fcvt() bad return, got '%s' expected '%s%s%s'\n", i, str,
            test_cvt_testcases[i].expstr_f, test_cvt_testcases[i].expstr_f2 != NULL ? "' or '" : "", test_cvt_testcases[i].expstr_f2 != NULL ? test_cvt_testcases[i].expstr_f2 : "");
        ok( decpt == test_cvt_testcases[i].expdecpt_f || (test_cvt_testcases[i].expdecpt_f < 0 && decpt < 0) || (test_cvt_testcases[i].expdecpt_f2 != 0 && test_cvt_testcases[i].expdecpt_f2 == decpt),
                "%d) fcvt() decimal point wrong, got %d expected %d\n", i, decpt,
                test_cvt_testcases[i].expdecpt_f);
        ok( sign == test_cvt_testcases[i].expsign,
                "%d) fcvt() sign wrong, got %d expected %d\n", i, sign,
                test_cvt_testcases[i].expsign);
    }
#endif

    if (p__ecvt_s)
    {
        str = malloc(1024);
        for( i = 0; strcmp( test_cvt_testcases[i].expstr_e, "END") != 0; i++){
            decpt = sign = 100;
            err = p__ecvt_s(str, 1024, test_cvt_testcases[i].value, test_cvt_testcases[i].nrdigits, &decpt, &sign);
            ok(err == 0, "_ecvt_s() failed with error code %d\n", err);
            ok( !strncmp( str, test_cvt_testcases[i].expstr_e, 15),
                   "%d) _ecvt_s() bad return, got '%s' expected '%s'\n", i, str,
                  test_cvt_testcases[i].expstr_e);
            ok( decpt == test_cvt_testcases[i].expdecpt_e,
                    "%d) _ecvt_s() decimal point wrong, got %d expected %d\n", i, decpt,
                    test_cvt_testcases[i].expdecpt_e);
            ok( sign == test_cvt_testcases[i].expsign,
                    "%d) _ecvt_s() sign wrong, got %d expected %d\n", i, sign,
                    test_cvt_testcases[i].expsign);
        }
        free(str);
    }
    else
        win_skip("_ecvt_s not available\n");

    if (p__fcvt_s)
    {
        int i;

        str = malloc(1024);

        /* invalid arguments */
        err = p__fcvt_s(NULL, 0, 0.0, 0, &i, &i);
        ok(err == EINVAL, "got %d, expected EINVAL\n", err);

        err = p__fcvt_s(str, 0, 0.0, 0, &i, &i);
        ok(err == EINVAL, "got %d, expected EINVAL\n", err);

        str[0] = ' ';
        str[1] = 0;
        err = p__fcvt_s(str, -1, 0.0, 0, &i, &i);
        ok(err == 0, "got %d, expected 0\n", err);
        ok(str[0] == 0, "got %c, expected 0\n", str[0]);
        ok(str[1] == 0, "got %c, expected 0\n", str[1]);

        err = p__fcvt_s(str, 1, 0.0, 0, NULL, &i);
        ok(err == EINVAL, "got %d, expected EINVAL\n", err);

        err = p__fcvt_s(str, 1, 0.0, 0, &i, NULL);
        ok(err == EINVAL, "got %d, expected EINVAL\n", err);

        for( i = 0; strcmp( test_cvt_testcases[i].expstr_e, "END") != 0; i++){
            decpt = sign = 100;
            err = p__fcvt_s(str, 1024, test_cvt_testcases[i].value, test_cvt_testcases[i].nrdigits, &decpt, &sign);
            ok(!err, "%d) _fcvt_s() failed with error code %d\n", i, err);
            ok( !strncmp( str, test_cvt_testcases[i].expstr_f, 15),
                   "%d) _fcvt_s() bad return, got '%s' expected '%s'. test %d\n", i, str,
                  test_cvt_testcases[i].expstr_f, i);
            ok( decpt == test_cvt_testcases[i].expdecpt_f,
                    "%d) _fcvt_s() decimal point wrong, got %d expected %d\n", i, decpt,
                    test_cvt_testcases[i].expdecpt_f);
            ok( sign == test_cvt_testcases[i].expsign,
                    "%d) _fcvt_s() sign wrong, got %d expected %d\n", i, sign,
                    test_cvt_testcases[i].expsign);
        }
        free(str);
    }
    else
        win_skip("_fcvt_s not available\n");
}

#ifndef YALIBCT_LIBC_DOESNT_HAVE_VSWPRINTF
static int WINAPIV _vsnwprintf_wrapper(wchar_t *str, size_t len, const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = vswprintf(str, len, format, valist);
    va_end(valist);
    return ret;
}
#endif

static void test_vsnwprintf(void)
{
    int ret;
    wchar_t str[32];
    char buf[32];

    // UB and not widely supported
    /*ret = _vsnwprintf_wrapper( str, ARRAY_SIZE(str), L"%ws%ws%ws", L"one", L"two", L"three" );
    ok( ret == 11, "got %d expected 11\n", ret );
    WideCharToMultiByte( CP_ACP, 0, str, -1, buf, sizeof(buf), NULL, NULL );
    ok( !strcmp(buf, "onetwothree"), "got %s expected 'onetwothree'\n", buf );

    ret = _vsnwprintf_wrapper( str, 0, L"%ws%ws%ws", L"one", L"two", L"three" );
    ok( ret == -1, "got %d, expected -1\n", ret );

    ret = _vsnwprintf_wrapper( NULL, 0, L"%ws%ws%ws", L"one", L"two", L"three" );
    ok( ret == 11 || broken(ret == -1 /* Win2k *//*), "got %d, expected 11\n", ret );*/
}

// Different declarations..
/*
static int WINAPIV vswprintf_wrapper(wchar_t *str, const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p_vswprintf(str, format, valist);
    va_end(valist);
    return ret;
}

static int WINAPIV _vswprintf_wrapper(wchar_t *str, const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vswprintf(str, format, valist);
    va_end(valist);
    return ret;
}
*/

static int WINAPIV _vswprintf_l_wrapper(wchar_t *str, const wchar_t *format, void *locale, ...)
{
    int ret;
    va_list valist;
    va_start(valist, locale);
    ret = p__vswprintf_l(str, format, locale, valist);
    va_end(valist);
    return ret;
}

static int WINAPIV _vswprintf_c_wrapper(wchar_t *str, size_t size, const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vswprintf_c(str, size, format, valist);
    va_end(valist);
    return ret;
}

static int WINAPIV _vswprintf_c_l_wrapper(wchar_t *str, size_t size, const wchar_t *format, void *locale, ...)
{
    int ret;
    va_list valist;
    va_start(valist, locale);
    ret = p__vswprintf_c_l(str, size, format, locale, valist);
    va_end(valist);
    return ret;
}

static int WINAPIV _vswprintf_p_l_wrapper(wchar_t *str, size_t size, const wchar_t *format, void *locale, ...)
{
    int ret;
    va_list valist;
    va_start(valist, locale);
    ret = p__vswprintf_p_l(str, size, format, locale, valist);
    va_end(valist);
    return ret;
}

static void test_vswprintf(void)
{
    wchar_t buf[20];
    int ret;

    if (!p__vswprintf_l ||!p__vswprintf_c
            || !p__vswprintf_c_l || !p__vswprintf_p_l)
    {
        win_skip("_vswprintf or vswprintf not available\n");
        return;
    }

    // Different declarations...
    /*
    ret = vswprintf_wrapper(buf, L"%s %d", L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));

    memset(buf, 0, sizeof(buf));
    ret = _vswprintf_wrapper(buf, L"%s %d", L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));
    */

    memset(buf, 0, sizeof(buf));
    ret = _vswprintf_l_wrapper(buf, L"%s %d", NULL, L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));

    memset(buf, 0, sizeof(buf));
    ret = _vswprintf_c_wrapper(buf, 20, L"%s %d", L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));

    memset(buf, 'x', sizeof(buf));
    ret = _vswprintf_c_wrapper(buf, 10, L"%s %d", L"number", 123);
    ok(ret == -1, "got %d, expected -1\n", ret);
    ok(!wcscmp(buf, L"number 12"), "buf = %ls\n", (buf));

    memset(buf, 0, sizeof(buf));
    ret = _vswprintf_c_l_wrapper(buf, 20, L"%s %d", NULL, L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));

    memset(buf, 0, sizeof(buf));
    ret = _vswprintf_p_l_wrapper(buf, 20, L"%s %d", NULL, L"number", 123);
    ok(ret == 10, "got %d, expected 10\n", ret);
    ok(!wcscmp(buf, L"number 123"), "buf = %ls\n", (buf));
}

static int WINAPIV _vscprintf_wrapper(const char *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vscprintf(format, valist);
    va_end(valist);
    return ret;
}

static void test_vscprintf(void)
{
    int ret;

    if (!p__vscprintf)
    {
       win_skip("_vscprintf not available\n");
       return;
    }

    ret = _vscprintf_wrapper( "%s %d", "number", 1 );
    ok( ret == 8, "got %d expected 8\n", ret );
}

static int WINAPIV _vscwprintf_wrapper(const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vscwprintf(format, valist);
    va_end(valist);
    return ret;
}

static void test_vscwprintf(void)
{
    int ret;

    if (!p__vscwprintf)
    {
        win_skip("_vscwprintf not available\n");
        return;
    }

    ret = _vscwprintf_wrapper(L"%s %d", L"number", 1 );
    ok( ret == 8, "got %d expected 8\n", ret );
}

static int WINAPIV _vsnwprintf_s_wrapper(wchar_t *str, size_t sizeOfBuffer,
                                 size_t count, const wchar_t *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vsnwprintf_s(str, sizeOfBuffer, count, format, valist);
    va_end(valist);
    return ret;
}

static void test_vsnwprintf_s(void)
{
    wchar_t buffer[14] = { 0 };
    int ret;

    if (!p__vsnwprintf_s)
    {
        win_skip("_vsnwprintf_s not available\n");
        return;
    }

    // Non-standard and not widely supported
    /*
      /* Enough room. *//*
    ret = _vsnwprintf_s_wrapper(buffer, 14, _TRUNCATE, L"AB%uC", 123);
    ok( ret == 6, "length wrong, expect=6, got=%d\n", ret);
    ok( !wcscmp(L"AB123C", buffer), "buffer wrong, got=%ls\n", (buffer));

    ret = _vsnwprintf_s_wrapper(buffer, 12, _TRUNCATE, L"AB%uC", 123);
    ok( ret == 6, "length wrong, expect=6, got=%d\n", ret);
    ok( !wcscmp(L"AB123C", buffer), "buffer wrong, got=%ls\n", (buffer));

    ret = _vsnwprintf_s_wrapper(buffer, 7, _TRUNCATE, L"AB%uC", 123);
    ok( ret == 6, "length wrong, expect=6, got=%d\n", ret);
    ok( !wcscmp(L"AB123C", buffer), "buffer wrong, got=%ls\n", (buffer));

    /* Not enough room. *//*
    ret = _vsnwprintf_s_wrapper(buffer, 6, _TRUNCATE, L"AB%uC", 123);
    ok( ret == -1, "length wrong, expect=-1, got=%d\n", ret);
    ok( !wcscmp(L"AB123", buffer), "buffer wrong, got=%ls\n", (buffer));

    ret = _vsnwprintf_s_wrapper(buffer, 2, _TRUNCATE, L"AB%uC", 123);
    ok( ret == -1, "length wrong, expect=-1, got=%d\n", ret);
    ok( !wcscmp(L"A", buffer), "buffer wrong, got=%ls\n", (buffer));

    ret = _vsnwprintf_s_wrapper(buffer, 1, _TRUNCATE, L"AB%uC", 123);
    ok( ret == -1, "length wrong, expect=-1, got=%d\n", ret);
    ok( !wcscmp(L"", buffer), "buffer wrong, got=%ls\n", (buffer));*/
}

static int WINAPIV _vsprintf_p_wrapper(char *str, size_t sizeOfBuffer,
                                 const char *format, ...)
{
    int ret;
    va_list valist;
    va_start(valist, format);
    ret = p__vsprintf_p(str, sizeOfBuffer, format, valist);
    va_end(valist);
    return ret;
}

static void test_vsprintf_p(void)
{
    char buf[1024];
    int ret;

    if(!p__vsprintf_p) {
        win_skip("vsprintf_p not available\n");
        return;
    }

    ret = _vsprintf_p_wrapper(buf, sizeof(buf), "%s %d", "test", 1234);
    ok(ret == 9, "ret = %d\n", ret);
    ok(!memcmp(buf, "test 1234", 10), "buf = %s\n", buf);

    ret = _vsprintf_p_wrapper(buf, sizeof(buf), "%1$d", 1234, "additional param");
    ok(ret == 4, "ret = %d\n", ret);
    ok(!memcmp(buf, "1234", 5), "buf = %s\n", buf);

    ret = _vsprintf_p_wrapper(buf, sizeof(buf), "%2$s %1$d", 1234, "test");
    ok(ret == 9, "ret = %d\n", ret);
    ok(!memcmp(buf, "test 1234", 10), "buf = %s\n", buf);

    ret = _vsprintf_p_wrapper(buf, sizeof(buf), "%2$*3$s %2$.*1$s", 2, "test", 3);
    ok(ret == 7, "ret = %d\n", ret);
    ok(!memcmp(buf, "test te", 8), "buf = %s\n", buf);

    /* Following test invokes invalid parameter handler */
    /* ret = _vsprintf_p_wrapper(buf, sizeof(buf), "%d %1$d", 1234); */
}

static void test__get_output_format(void)
{
    unsigned int ret;
    char buf[64];
    int c;

    if (!p__get_output_format || !p__set_output_format)
    {
        win_skip("_get_output_format or _set_output_format is not available\n");
        return;
    }

    /*ret = p__get_output_format();
      ok(ret == 0, "got %d\n", ret);*/

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
    c = p_sprintf(buf, "%E", 1.23);
    ok(c == 13, "c = %d\n", c);
    ok(!strcmp(buf, "1.230000E+000"), "buf = %s\n", buf);
#endif

    // Non-standard and not widely supported
    /*ret = p__set_output_format(_TWO_DIGIT_EXPONENT);
      ok(ret == 0, "got %d\n", ret);*/

    c = p_sprintf(buf, "%E", 1.23);
    ok(c == 12, "c = %d\n", c);
    ok(!strcmp(buf, "1.230000E+00"), "buf = %s\n", buf);

    // Non-standard and not widely supported
    /*ret = p__get_output_format();
    ok(ret == _TWO_DIGIT_EXPONENT, "got %d\n", ret);

    ret = p__set_output_format(_TWO_DIGIT_EXPONENT);
    ok(ret == _TWO_DIGIT_EXPONENT, "got %d\n", ret);*/
}

START_TEST(printf)
{
    yalibct_chdir_to_tmpdir();
    init();

    test_sprintf();
    test_swprintf();
    test_snprintf();
    test_fprintf();
    test_fcvt();
    test_xcvt();
    test_vsnwprintf();
    test_vscprintf();
    test_vscwprintf();
    test_vswprintf();
    test_vsnwprintf_s();
    test_vsprintf_p();
    test__get_output_format();

    yalibct_chdir_to_tmpdir_remove_tmpdir();
}
