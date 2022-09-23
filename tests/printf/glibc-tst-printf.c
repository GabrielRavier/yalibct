/* Copyright (C) 1991-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include "test-deps/glibc.h"
#include "test-lib/portable-symbols/NL_ARGMAX.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <float.h>

/* This whole file is picayune tests of corner cases of printf format strings.
   The compiler warnings are not useful here.  */
DIAG_IGNORE_NEEDS_COMMENT (4.9, "-Wformat");

#if __GNUC_PREREQ (7, 0)
/* Compiler warnings about snprintf output truncation should also be
   ignored.  */
DIAG_IGNORE_NEEDS_COMMENT (7.0, "-Wformat-truncation");
#endif

static void rfg1 (void);
static void rfg2 (void);
static void rfg3 (void);


static void
fmtchk (const char *fmt)
{
  (void) fputs(fmt, stdout);
  (void) printf(":\t`");
  (void) printf(fmt, 0x12);
  (void) printf("'\n");
}

static void
fmtst1chk (const char *fmt)
{
  (void) fputs(fmt, stdout);
  (void) printf(":\t`");
  (void) printf(fmt, 4, 0x12);
  (void) printf("'\n");
}

static void
fmtst2chk (const char *fmt)
{
  (void) fputs(fmt, stdout);
  (void) printf(":\t`");
  (void) printf(fmt, 4, 4, 0x12);
  (void) printf("'\n");
}

int
do_test (void)
{
  static char shortstr[] = "Hi, Z.";
  static char longstr[] = "Good morning, Doctor Chandra.  This is Hal.  \
I am ready for my first lesson today.";
  int result = 0;

  fmtchk("%.4x");
  fmtchk("%04x");
  fmtchk("%4.4x");
  fmtchk("%04.4x");
  fmtchk("%4.3x");
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
  fmtchk("%04.3x");
#else
  puts("%04.3x:	` 012'");
#endif

  fmtst1chk("%.*x");
  fmtst1chk("%0*x");
  fmtst2chk("%*.*x");
  fmtst2chk("%0*.*x");

  // UB and not widely supported
  /*printf("bad format:\t\"%v\"\n");
    printf("nil pointer (padded):\t\"%10p\"\n", (void *) NULL);*/

  printf("decimal negative:\t\"%d\"\n", -2345);
  printf("octal negative:\t\"%o\"\n", -2345);
  printf("hex negative:\t\"%x\"\n", -2345);
  printf("long decimal number:\t\"%ld\"\n", -123456L);
  printf("long octal negative:\t\"%lo\"\n", -2345L);
  printf("long unsigned decimal number:\t\"%lu\"\n", -123456L);
  printf("zero-padded LDN:\t\"%010ld\"\n", -123456L);
  printf("left-adjusted ZLDN:\t\"%-010ld\"\n", -123456L);
  printf("space-padded LDN:\t\"%10ld\"\n", -123456L);
  printf("left-adjusted SLDN:\t\"%-10ld\"\n", -123456L);

  printf("zero-padded string:\t\"%010s\"\n", shortstr);
  printf("left-adjusted Z string:\t\"%-010s\"\n", shortstr);
  printf("space-padded string:\t\"%10s\"\n", shortstr);
  printf("left-adjusted S string:\t\"%-10s\"\n", shortstr);
  /* GCC 9 warns about the NULL format argument; this is deliberately
     tested here.  */
  DIAG_PUSH_NEEDS_COMMENT;
#if __GNUC_PREREQ (7, 0)
  DIAG_IGNORE_NEEDS_COMMENT (9, "-Wformat-overflow=");
#endif
  printf("null string:\t\"%s\"\n", (char *)NULL);
  DIAG_POP_NEEDS_COMMENT;
  printf("limited string:\t\"%.22s\"\n", longstr);

#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
  printf("a-style max:\t\"%a\"\n", DBL_MAX);
  printf("a-style -max:\t\"%a\"\n", -DBL_MAX);
#else
  puts("a-style max:	\"0x1.fffffffffffffp+1023\"\na-style -max:	\"-0x1.fffffffffffffp+1023\"");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  printf("e-style >= 1:\t\"%e\"\n", 12.34);
  printf("e-style >= .1:\t\"%e\"\n", 0.1234);
  printf("e-style < .1:\t\"%e\"\n", 0.001234);
  printf("e-style big:\t\"%.60e\"\n", 1e20);
  printf ("e-style == .1:\t\"%e\"\n", 0.1);
#else
  puts("e-style >= 1:	\"1.234000e+01\"");
  puts("e-style >= .1:	\"1.234000e-01\"");
  puts("e-style < .1:	\"1.234000e-03\"");
  puts("e-style big:	\"1.000000000000000000000000000000000000000000000000000000000000e+20\"");
  puts("e-style == .1:	\"1.000000e-01\"");
#endif
  printf("f-style == 0:\t\"%f\"\n", 0.0);
  printf("f-style >= 1:\t\"%f\"\n", 12.34);
  printf("f-style >= .1:\t\"%f\"\n", 0.1234);
  printf("f-style < .1:\t\"%f\"\n", 0.001234);
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
  printf("g-style == 0:\t\"%g\"\n", 0.0);
  printf("g-style >= 1:\t\"%g\"\n", 12.34);
  printf("g-style >= .1:\t\"%g\"\n", 0.1234);
  printf("g-style < .1:\t\"%g\"\n", 0.001234);
  printf("g-style big:\t\"%.60g\"\n", 1e20);
#else
  puts("g-style == 0:	\"0\"\ng-style >= 1:	\"12.34\"\ng-style >= .1:	\"0.1234\"\ng-style < .1:	\"0.001234\"\ng-style big:	\"100000000000000000000\"");
#endif

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  printf("Lf-style == 0:\t\"%Lf\"\n", (long double) 0.0);
  printf("Lf-style >= 1:\t\"%Lf\"\n", (long double) 12.34);
  printf("Lf-style >= .1:\t\"%Lf\"\n", (long double) 0.1234);
  printf("Lf-style < .1:\t\"%Lf\"\n", (long double) 0.001234);
  printf("Lg-style == 0:\t\"%Lg\"\n", (long double) 0.0);
  printf("Lg-style >= 1:\t\"%Lg\"\n", (long double) 12.34);
  printf("Lg-style >= .1:\t\"%Lg\"\n", (long double) 0.1234);
  printf("Lg-style < .1:\t\"%Lg\"\n", (long double) 0.001234);
  printf("Lg-style big:\t\"%.60Lg\"\n", (long double) 1e20);
#else
  puts("Lf-style == 0:	\"0.000000\"\nLf-style >= 1:	\"12.340000\"\nLf-style >= .1:	\"0.123400\"\nLf-style < .1:	\"0.001234\"\nLg-style == 0:	\"0\"\nLg-style >= 1:	\"12.34\"\nLg-style >= .1:	\"0.1234\"\nLg-style < .1:	\"0.001234\"\nLg-style big:	\"100000000000000000000\"");
#endif

  printf (" %6.5f\n", .099999999860301614);
  printf (" %6.5f\n", .1);
  printf ("x%5.4fx\n", .5);

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  printf (" %6.5Lf\n", (long double) .099999999860301614);
  printf (" %6.5Lf\n", (long double) .1);
  printf ("x%5.4Lfx\n", (long double) .5);
#else
  puts(" 0.10000\n 0.10000\nx0.5000x");
#endif

  printf ("%#03x\n", 1);

#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  printf ("something really insane: %.10000f\n", 1.0);
  printf ("something really insane (long double): %.10000Lf\n",
	  (long double) 1.0);
#else
  printf("something really insane: 1.");
  for (int i = 0; i < 10000; ++i)
      putchar('0');
  printf("\nsomething really insane (long double): 1.");
  for (int i = 0; i < 10000; ++i)
      putchar('0');
  putchar('\n');
#endif

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  {
    double d = FLT_MIN;
    int niter = 17;

    while (niter-- != 0)
      printf ("%.17e\n", d / 2);
    assert(fflush (stdout) == 0);
  }

  printf ("%15.5e\n", 4.9406564584124654e-324);

#define FORMAT "|%12.4f|%12.4e|%12.4g|%12.4Lf|%12.4Lg|\n"
  printf (FORMAT, 0.0, 0.0, 0.0,
	  (long double) 0.0, (long double) 0.0);
  printf (FORMAT, 1.0, 1.0, 1.0,
	  (long double) 1.0, (long double) 1.0);
  printf (FORMAT, -1.0, -1.0, -1.0,
	  (long double) -1.0, (long double) -1.0);
  printf (FORMAT, 100.0, 100.0, 100.0,
	  (long double) 100.0, (long double) 100.0);
  printf (FORMAT, 1000.0, 1000.0, 1000.0,
	  (long double) 1000.0, (long double) 1000.0);
  printf (FORMAT, 10000.0, 10000.0, 10000.0,
	  (long double) 10000.0, (long double) 10000.0);
  printf (FORMAT, 12345.0, 12345.0, 12345.0,
	  (long double) 12345.0, (long double) 12345.0);
  printf (FORMAT, 100000.0, 100000.0, 100000.0,
	  (long double) 100000.0, (long double) 100000.0);
  printf (FORMAT, 123456.0, 123456.0, 123456.0,
	  (long double) 123456.0, (long double) 123456.0);
#undef	FORMAT
#else
  for (int i = 0; i < 17; ++i)
      puts("5.87747175411143754e-39");
  puts("   4.94066e-324");
  puts("|      0.0000|  0.0000e+00|           0|      0.0000|           0|");
  puts("|      1.0000|  1.0000e+00|           1|      1.0000|           1|");
  puts("|     -1.0000| -1.0000e+00|          -1|     -1.0000|          -1|");
  puts("|    100.0000|  1.0000e+02|         100|    100.0000|         100|");
  puts("|   1000.0000|  1.0000e+03|        1000|   1000.0000|        1000|");
  puts("|  10000.0000|  1.0000e+04|       1e+04|  10000.0000|       1e+04|");
  puts("|  12345.0000|  1.2345e+04|   1.234e+04|  12345.0000|   1.234e+04|");
  puts("| 100000.0000|  1.0000e+05|       1e+05| 100000.0000|       1e+05|");
  puts("| 123456.0000|  1.2346e+05|   1.235e+05| 123456.0000|   1.235e+05|");
#endif

#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  {
    char buf[20];
    char buf2[512];
    printf ("snprintf (\"%%30s\", \"foo\") == %d, \"%.*s\"\n",
	    snprintf (buf, sizeof (buf), "%30s", "foo"), (int) sizeof (buf),
	    buf);
    printf ("snprintf (\"%%.999999u\", 10) == %d\n",
	    snprintf (buf2, sizeof (buf2), "%.999999u", 10));
  }

#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
  printf("%.8f\n", DBL_MAX);
  printf("%.8f\n", -DBL_MAX);
#else
  puts("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.00000000\n-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.00000000");
#endif
#else
  puts("snprintf (\"%30s\", \"foo\") == 30, \"                   \"\n"
       "snprintf (\"%.999999u\", 10) == 999999");
  puts("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.00000000\n-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.00000000");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  printf ("%e should be 1.234568e+06\n", 1234567.8);
#else
  puts("1.234568e+06 should be 1.234568e+06");
#endif
  printf ("%f should be 1234567.800000\n", 1234567.8);
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
  printf ("%g should be 1.23457e+06\n", 1234567.8);
  printf ("%g should be 123.456\n", 123.456);
  printf ("%g should be 1e+06\n", 1000000.0);
  printf ("%g should be 10\n", 10.0);
  printf ("%g should be 0.02\n", 0.02);
#else
  puts("1.23457e+06 should be 1.23457e+06\n"
       "123.456 should be 123.456\n"
       "1e+06 should be 1e+06\n"
       "10 should be 10\n"
       "0.02 should be 0.02");
#endif

#if 0
  /* This test rather checks the way the compiler handles constant
     folding.  gcc behavior wrt to this changed in 3.2 so it is not a
     portable test.  */
  {
    double x=1.0;
    printf("%.17f\n",(1.0/x/10.0+1.0)*x-x);
  }
#endif

  {
    char buf[200];

    assert(sprintf(buf,"%*s%*s%*s",-1,"one",-20,"two",-30,"three") == strlen(buf));

    result |= strcmp (buf,
		      "onetwo                 three                         ");

    puts (result != 0 ? "Test failed!" : "Test ok.");
  }

  // UB and not widely supported
  /*{
    char buf[200];

    sprintf (buf, "%07Lo", 040000000000ll);
    printf ("sprintf (buf, \"%%07Lo\", 040000000000ll) = %s", buf);

    if (strcmp (buf, "40000000000") != 0)
      {
	result = 1;
	fputs ("\tFAILED", stdout);
      }
    puts ("");
    }*/

  printf ("printf (\"%%hhu\", %u) = %hhu\n", UCHAR_MAX + 2, UCHAR_MAX + 2);
  printf ("printf (\"%%hu\", %u) = %hu\n", USHRT_MAX + 2, USHRT_MAX + 2);
  printf ("printf (\"%%hhi\", %i) = %hhi\n", UCHAR_MAX + 2, UCHAR_MAX + 2);
  printf ("printf (\"%%hi\", %i) = %hi\n", USHRT_MAX + 2, USHRT_MAX + 2);

#ifndef YALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS
  printf ("printf (\"%%1$hhu\", %2$u) = %1$hhu\n",
	  UCHAR_MAX + 2, UCHAR_MAX + 2);
  printf ("printf (\"%%1$hu\", %2$u) = %1$hu\n", USHRT_MAX + 2, USHRT_MAX + 2);
  printf ("printf (\"%%1$hhi\", %2$i) = %1$hhi\n",
	  UCHAR_MAX + 2, UCHAR_MAX + 2);
  printf ("printf (\"%%1$hi\", %2$i) = %1$hi\n", USHRT_MAX + 2, USHRT_MAX + 2);
#else
  puts("printf (\"%1$hhu\", 257) = 1\n"
       "printf (\"%1$hu\", 65537) = 1\n"
       "printf (\"%1$hhi\", 257) = 1\n"
       "printf (\"%1$hi\", 65537) = 1");
#endif

  puts ("--- Should be no further output. ---");
  rfg1 ();
  rfg2 ();
  rfg3 ();

#ifndef YALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS
  {
    char bytes[7];
    char buf[20];

    memset (bytes, '\xff', sizeof bytes);
    assert(sprintf (buf, "foo%hhn\n", &bytes[3]) == strlen(buf));
    if (bytes[0] != '\xff' || bytes[1] != '\xff' || bytes[2] != '\xff'
	|| bytes[4] != '\xff' || bytes[5] != '\xff' || bytes[6] != '\xff')
      {
	puts ("%hhn overwrite more bytes");
	result = 1;
      }
    if (bytes[3] != 3)
      {
	puts ("%hhn wrote incorrect value");
	result = 1;
      }
  }
#endif

  return result != 0;
}

static void
rfg1 (void)
{
  char buf[100];

  assert(sprintf (buf, "%5.s", "xyz") == strlen(buf));
  if (strcmp (buf, "     ") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "     ");
  assert(sprintf (buf, "%5.f", 33.3) == strlen(buf));
  if (strcmp (buf, "   33") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "   33");
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  assert(sprintf (buf, "%5.Lf", (long double) 33.3) == strlen(buf));
  if (strcmp (buf, "   33") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "   33");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  assert(sprintf (buf, "%8.e", 33.3e7) == strlen(buf));
  if (strcmp (buf, "   3e+08") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "   3e+08");
  assert(sprintf (buf, "%8.E", 33.3e7) == strlen(buf));
  if (strcmp (buf, "   3E+08") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "   3E+08");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
  assert(sprintf (buf, "%.g", 33.3) == strlen(buf));
  if (strcmp (buf, "3e+01") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3e+01");
  assert(sprintf (buf, "%.Lg", (long double) 33.3) == strlen(buf));
  if (strcmp (buf, "3e+01") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3e+01");
  assert(sprintf (buf, "%.G", 33.3) == strlen(buf));
  if (strcmp (buf, "3E+01") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3E+01");
#endif
}

static void
rfg2 (void)
{
  int prec;
  char buf[100];

  prec = 0;
  assert(sprintf (buf, "%.*g", prec, 3.3) == strlen(buf));
  if (strcmp (buf, "3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3");
  prec = 0;
  assert(sprintf (buf, "%.*G", prec, 3.3) == strlen(buf));
  if (strcmp (buf, "3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3");
  prec = 0;
  assert(sprintf (buf, "%7.*G", prec, 3.33) == strlen(buf));
  if (strcmp (buf, "      3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "      3");
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  prec = 0;
  assert(sprintf (buf, "%.*Lg", prec, (long double) 3.3) == strlen(buf));
  if (strcmp (buf, "3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3");
  prec = 0;
  assert(sprintf (buf, "%.*LG", prec, (long double) 3.3) == strlen(buf));
  if (strcmp (buf, "3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "3");
  prec = 0;
  assert(sprintf (buf, "%7.*LG", prec, (long double) 3.33) == strlen(buf));
  if (strcmp (buf, "      3") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "      3");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
  prec = 3;
  assert(sprintf (buf, "%04.*o", prec, 33) == strlen(buf));
  if (strcmp (buf, " 041") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, " 041");
  prec = 7;
  assert(sprintf (buf, "%09.*u", prec, 33) == strlen(buf));
  if (strcmp (buf, "  0000033") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, "  0000033");
  prec = 3;
  assert(sprintf (buf, "%04.*x", prec, 33) == strlen(buf));
  if (strcmp (buf, " 021") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, " 021");
  prec = 3;
  assert(sprintf (buf, "%04.*X", prec, 33) == strlen(buf));
  if (strcmp (buf, " 021") != 0)
    printf ("got: '%s', expected: '%s'\n", buf, " 021");
#endif
}

static void
rfg3 (void)
{
  char buf[100];
  double g = 5.0000001;
  unsigned long l = 1234567890;
  double d = 321.7654321;
  const char s[] = "test-string";
  int i = 12345;
  int h = 1234;

  if (NL_ARGMAX < 13)
      return;
  assert(sprintf (buf,
	   "%1$*5$d %2$*6$hi %3$*7$lo %4$*8$f %9$*12$e %10$*13$g %11$*14$s",
	   i, h, l, d, 8, 5, 14, 14, d, g, s, 14, 3, 14) == strlen(buf));
  if (strcmp (buf,
	      "   12345  1234    11145401322     321.765432   3.217654e+02   5    test-string") != 0)
    printf ("got: '%s', expected: '%s'\n", buf,
	    "   12345  1234    11145401322     321.765432   3.217654e+02   5    test-string");
}
