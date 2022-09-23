/* Test of POSIX compatible vsprintf() and sprintf() functions.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include "test-deps/gnulib.h"
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

static void
test_function (int (*my_printf) (const char *, ...))
{
  /* Here we don't test output that may be platform dependent.
     The bulk of the tests is done as part of the 'vasnprintf-posix' module.  */

  /* Test support of size specifiers as in C99.  */

    assert(my_printf ("%ju %d\n", (uintmax_t) 12345671, 33, 44, 55) >= 0);

    assert(my_printf ("%zu %d\n", (size_t) 12345672, 33, 44, 55) >= 0);

#ifndef YALIBCT_DISABLE_PRINTF_T_LENGTH_MODIFIER_TESTS
    assert(my_printf ("%tu %d\n", (ptrdiff_t) 12345673, 33, 44, 55) >= 0);
#else
  puts("12345673 33");
#endif

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
  /* Positive zero.  */
  assert(my_printf ("%a %d\n", 0.0, 33, 44, 55) >= 0);

  /* Positive infinity.  */
  assert(my_printf ("%a %d\n", Infinityd (), 33, 44, 55) >= 0);

  /* Negative infinity.  */
  assert(my_printf ("%a %d\n", - Infinityd (), 33, 44, 55) >= 0);

  /* FLAG_ZERO with infinite number.  */
  /* "0000000inf 33" is not a valid result; see
     <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
  assert(my_printf ("%010a %d\n", Infinityd (), 33, 44, 55) >= 0);
#else
  puts("0x0p+0 33\n"
       "inf 33\n"
       "-inf 33\n"
       "       inf 33");
#endif

  /* Test the support of the %f format directive.  */

  /* A positive number.  */
  assert(my_printf ("%f %d\n", 12.75, 33, 44, 55) >= 0);

  /* A larger positive number.  */
  assert(my_printf ("%f %d\n", 1234567.0, 33, 44, 55) >= 0);

  /* A negative number.  */
  assert(my_printf ("%f %d\n", -0.03125, 33, 44, 55) >= 0);

  /* Positive zero.  */
  assert(my_printf ("%f %d\n", 0.0, 33, 44, 55) >= 0);

  /* FLAG_ZERO.  */
  assert(my_printf ("%015f %d\n", 1234.0, 33, 44, 55) >= 0);

  /* Precision.  */
  assert(my_printf ("%.f %d\n", 1234.0, 33, 44, 55) >= 0);

  /* Precision with no rounding.  */
  assert(my_printf ("%.2f %d\n", 999.95, 33, 44, 55) >= 0);

  /* Precision with rounding.  */
  assert(my_printf ("%.2f %d\n", 999.996, 33, 44, 55) >= 0);

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  /* A positive number.  */
  assert(my_printf ("%Lf %d\n", 12.75L, 33, 44, 55) >= 0);

  /* A larger positive number.  */
  assert(my_printf ("%Lf %d\n", 1234567.0L, 33, 44, 55) >= 0);

  /* A negative number.  */
  assert(my_printf ("%Lf %d\n", -0.03125L, 33, 44, 55) >= 0);

  /* Positive zero.  */
  assert(my_printf ("%Lf %d\n", 0.0L, 33, 44, 55) >= 0);

  /* FLAG_ZERO.  */
  assert(my_printf ("%015Lf %d\n", 1234.0L, 33, 44, 55) >= 0);

  /* Precision.  */
  assert(my_printf ("%.Lf %d\n", 1234.0L, 33, 44, 55) >= 0);

  /* Precision with no rounding.  */
  assert(my_printf ("%.2Lf %d\n", 999.95L, 33, 44, 55) >= 0);

  /* Precision with rounding.  */
  assert(my_printf ("%.2Lf %d\n", 999.996L, 33, 44, 55) >= 0);
#else
  puts("12.750000 33\n1234567.000000 33\n-0.031250 33\n0.000000 33\n00001234.000000 33\n1234 33\n999.95 33\n1000.00 33");
#endif

  /* Test the support of the %F format directive.  */

  /* A positive number.  */
  assert(my_printf ("%F %d\n", 12.75, 33, 44, 55) >= 0);

  /* A larger positive number.  */
  assert(my_printf ("%F %d\n", 1234567.0, 33, 44, 55) >= 0);

  /* A negative number.  */
  assert(my_printf ("%F %d\n", -0.03125, 33, 44, 55) >= 0);

  /* Positive zero.  */
  assert(my_printf ("%F %d\n", 0.0, 33, 44, 55) >= 0);

  /* FLAG_ZERO.  */
  assert(my_printf ("%015F %d\n", 1234.0, 33, 44, 55) >= 0);

  /* Precision.  */
  assert(my_printf ("%.F %d\n", 1234.0, 33, 44, 55) >= 0);

  /* Precision with no rounding.  */
  assert(my_printf ("%.2F %d\n", 999.95, 33, 44, 55) >= 0);

  /* Precision with rounding.  */
  assert(my_printf ("%.2F %d\n", 999.996, 33, 44, 55) >= 0);

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  /* A positive number.  */
  assert(my_printf ("%LF %d\n", 12.75L, 33, 44, 55) >= 0);

  /* A larger positive number.  */
  assert(my_printf ("%LF %d\n", 1234567.0L, 33, 44, 55) >= 0);

  /* A negative number.  */
  assert(my_printf ("%LF %d\n", -0.03125L, 33, 44, 55) >= 0);

  /* Positive zero.  */
  assert(my_printf ("%LF %d\n", 0.0L, 33, 44, 55) >= 0);

  /* FLAG_ZERO.  */
  assert(my_printf ("%015LF %d\n", 1234.0L, 33, 44, 55) >= 0);

  /* Precision.  */
  assert(my_printf ("%.LF %d\n", 1234.0L, 33, 44, 55) >= 0);

  /* Precision with no rounding.  */
  assert(my_printf ("%.2LF %d\n", 999.95L, 33, 44, 55) >= 0);

  /* Precision with rounding.  */
  assert(my_printf ("%.2LF %d\n", 999.996L, 33, 44, 55) >= 0);
#else
  puts("12.750000 33\n1234567.000000 33\n-0.031250 33\n0.000000 33\n00001234.000000 33\n1234 33\n999.95 33\n1000.00 33");
#endif

#ifndef YALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS
  /* Test the support of the POSIX/XSI format strings with positions.  */

  assert(my_printf ("%2$d %1$d\n", 33, 55) >= 0);
#else
  puts("55 33");
#endif
}
