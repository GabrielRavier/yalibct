/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2019 Keith Packard
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/log.h"
#include "test-lib/portable-symbols/pow.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

# define _WANT_IO_C99_FORMATS
# define _WANT_IO_LONG_LONG
# define _WANT_IO_POS_ARGS
# define printf_float(x) ((double) (x))

static const double test_vals[] = { 1.234567, 1.1, M_PI };

int
check_vsnprintf(char *str, size_t size, const char *format, ...)
{
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsnprintf(str, size, format, ap);
	va_end(ap);
	return i;
}

static void checked_fflush(FILE *fp)
{
    assert(fflush(fp) == 0);
}

int
main(void)
{
	int x = -35;
	int y;
	char	buf[256];
	int	errors = 0;

#if 0
	double	a;

	printf ("hello world\n");
	for (x = 1; x < 20; x++) {
		printf("%.*f\n", x, 9.99999999999);
	}

	for (a = 1e-10; a < 1e10; a *= 10.0) {
		printf("g format: %10.3g %10.3g\n", 1.2345678 * a, 1.1 * a);
		checked_fflush(stdout);
	}
	for (a = 1e-10; a < 1e10; a *= 10.0) {
		printf("f format: %10.3f %10.3f\n", 1.2345678 * a, 1.1 * a);
		checked_fflush(stdout);
	}
	for (a = 1e-10; a < 1e10; a *= 10.0) {
		printf("e format: %10.3e %10.3e\n", 1.2345678 * a, 1.1 * a);
		checked_fflush(stdout);
	}
	printf ("%g\n", exp(11));
#endif
#if defined(TINY_STDIO) || !defined(NO_FLOATING_POINT)
	assert(sprintf(buf, "%g", printf_float(0.0f)) == 1);
	if (strcmp(buf, "0") != 0) {
		printf("0: wanted \"0\" got \"%s\"\n", buf);
		errors++;
		checked_fflush(stdout);
	}
#endif

#ifndef YALIBCT_DISABLE_SCANF_NUMBERED_ARGUMENTS_TESTS
        x = y = 0;
        int r = sscanf("3 4", "%2$d %1$d", &x, &y);
        if (x != 4 || y != 3 || r != 2) {
            printf("pos: wanted %d %d (ret %d) got %d %d (ret %d)", 4, 3, 2, x, y, r);
            errors++;
            checked_fflush(stdout);
        }
#endif

	/*
	 * test snprintf and vsnprintf to make sure they don't
	 * overwrite the specified buffer length (even if that is
	 * zero)
	 */
	for (x = 0; x <= 6; x++) {
		for (y = 0; y < 2; y++) {
			char tbuf[10] = "xxxxxxxxx";
			const char ref[10] = "xxxxxxxxx";
			const char *name = (y == 0 ? "snprintf" : "vsnprintf");
			int i = (y == 0 ? snprintf : check_vsnprintf) (tbuf, x, "%s", "123");
			int y = x <= 4 ? x : 4;
			if (i != 3) {
				printf("%s(tbuf, %d, \"%%s\", \"123\") return %d instead of %d\n", name,
				       x, i, 3);
				errors++;
			}
			int l = strlen(tbuf);
			if (y > 0 && l != y - 1) {
				printf("%s: returned buffer len want %d got %d\n", name, y - 1, l);
				errors++;
			}
			if (y > 0 && strncmp(tbuf, "123", y - 1) != 0) {
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
				strncpy(buf, "123", y - 1);
				buf[y-1] = '\0';
				printf("%s: returned buffer want %s got %s\n", name, buf, tbuf);
				errors++;
			}
			if (memcmp(tbuf + y, ref + y, sizeof(tbuf) - y) != 0) {
				printf("%s: tail of buf mangled %s\n", name, tbuf + y);
				errors++;
			}
		}
	}

#define FMT(prefix,conv) "%" prefix conv

#undef VERIFY
#define VERIFY(prefix, conv) do {                                       \
        assert(sprintf(buf, FMT(prefix, conv), v) == strlen(buf));  \
        assert(sscanf(buf, FMT(prefix, conv), &r) == 1);                \
        if (v != r) {                                                   \
                printf("\t%3d: " prefix " " conv " wanted " FMT(prefix, conv) " got " FMT(prefix, conv) "\n", x, v, r); \
                errors++;                                               \
                checked_fflush(stdout);                                         \
        }                                                               \
} while(0)

#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_X_CONVERSION_SPECIFIER_TESTS
#define VERIFY_x(prefix) VERIFY(prefix, "x")
#else
#define VERIFY_x(prefix)
#endif

#define CHECK_RT(type, prefix) do {                                     \
        for (x = 0; x < (int) (sizeof(type) * 8); x++) {            \
                type v = (type) (0x123456789abcdef0LL >> (64 - sizeof(type) * 8)) >> x; \
                type r = ~v;                                            \
                VERIFY(prefix, "d");                                    \
                VERIFY(prefix, "u");                                    \
                VERIFY_x(prefix);                                       \
                VERIFY(prefix, "o");                                    \
        }                                                               \
        } while(0)

	CHECK_RT(unsigned char, "hh");
	CHECK_RT(unsigned short, "h");
        CHECK_RT(unsigned int, "");
        CHECK_RT(unsigned long, "l");
        CHECK_RT(unsigned long long, "ll");
#ifndef YALIBCT_DISABLE_SCANF_J_LENGTH_MODIFIER_TESTS
        CHECK_RT(intmax_t, "j");
#endif
#ifndef YALIBCT_DISABLE_SCANF_Z_LENGTH_MODIFIER_TESTS
        CHECK_RT(size_t, "z");
#endif
#ifndef YALIBCT_DISABLE_SCANF_T_LENGTH_MODIFIER_TESTS
        CHECK_RT(ptrdiff_t, "t");
#endif

#ifndef YALIBCT_DISABLE_SCANF_P_CONVERSION_SPECIFIER_TESTS
        {
            static int i_addr = 12;
            void *v = &i_addr;
            void *r = (void *) -1;
            VERIFY("", "p");
        }
#endif

#define float_type double
#define scanf_format "%lf"
#define ERROR_MAX 0
	for (x = -37; x <= 37; x++)
	{
                float_type r;
		unsigned t;
		for (t = 0; t < sizeof(test_vals)/sizeof(test_vals[0]); t++) {

			float_type v = (float_type) test_vals[t] * pow(10.0, (float_type) x);
			float_type e;

#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
			assert(sprintf(buf, "%.55f", printf_float(v)) >= 0);
#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
			assert(sscanf(buf, scanf_format, &r) == 1);
			e = fabs(v-r) / v;
			if (e > (float_type) ERROR_MAX) {
				printf("\tf %3d: wanted %.7e got %.7e (error %.7e, buf %s)\n", x,
				       printf_float(v), printf_float(r), printf_float(e), buf);
				errors++;
				checked_fflush(stdout);
			}
#endif
#endif


#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
			assert(sprintf(buf, "%.20e", printf_float(v)) >= 0);
#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
			assert(sscanf(buf, scanf_format, &r) == 1);
			e = fabs(v-r) / v;
			if (e > (float_type) ERROR_MAX)
			{
				printf("\te %3d: wanted %.7e got %.7e (error %.7e, buf %s)\n", x,
				       printf_float(v), printf_float(r), printf_float(e), buf);
				errors++;
				checked_fflush(stdout);
			}
#endif
#endif


#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
			assert(sprintf(buf, "%.20g", printf_float(v)) >= 0);
#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
			assert(sscanf(buf, scanf_format, &r) == 1);
			e = fabs(v-r) / v;
			if (e > (float_type) ERROR_MAX)
			{
				printf("\tg %3d: wanted %.7e got %.7e (error %.7e, buf %s)\n", x,
				       printf_float(v), printf_float(r), printf_float(e), buf);
				errors++;
				checked_fflush(stdout);
			}
#endif
#endif

#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
			assert(sprintf(buf, "%.20a", printf_float(v)) >= 0);
#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
			assert(sscanf(buf, scanf_format, &r) == 1);
			e = fabs(v-r) / v;
			if (e > (float_type) ERROR_MAX)
			{
				printf("\ta %3d: wanted %.7e got %.7e (error %.7e, buf %s)\n", x,
				       printf_float(v), printf_float(r), printf_float(e), buf);
				errors++;
				checked_fflush(stdout);
			}
#endif
#endif

		}
#ifndef YALIBCT_DISABLE_SCANF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
                assert(sprintf(buf, "0x0.0p%+d", x) >= 0);
                assert(sscanf(buf, scanf_format, &r) == 1);
                if (r != (float_type) 0.0)
                {
                    printf("\tg %3d: wanted 0.0 got %.7e (buf %s)\n", x,
                           printf_float(r), buf);
                    errors++;
                    checked_fflush(stdout);
                }

                assert(sprintf(buf, "0x1p%+d", x) >= 0);
                assert(sscanf(buf, scanf_format, &r) == 1);
                if (r != (float_type) ldexp(1.0, x))
                {
                    printf("\tg %3d: wanted 1 got %.7e (buf %s)\n", x,
                           printf_float(r), buf);
                    errors++;
                    checked_fflush(stdout);
                }
#endif
	}
	checked_fflush(stdout);
	return errors;
}
