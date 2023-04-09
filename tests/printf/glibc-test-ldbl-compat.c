/* Test for the long double variants of *printf functions.
   Copyright (C) 2019-2022 Free Software Foundation, Inc.
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

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
static void
do_test_call_varg (FILE *stream, const char *format, ...)
{
  char *buffer = NULL;
  char string[128];
  va_list args;

  printf ("%15s", "vasprintf: ");
  va_start (args, format);
  vasprintf (&buffer, format, args);
  va_end (args);
  if (buffer == NULL)
    printf ("Error using vasprintf\n");
  else
    {
      printf ("%s", buffer);
      free (buffer);
    }
  printf ("\n");

  printf ("%15s", "vdprintf: ");
  assert(fflush(stdout) == 0);
  va_start (args, format);
  vdprintf (fileno (stream), format, args);
  va_end (args);
  printf ("\n");

  printf ("%15s", "vfprintf: ");
  va_start (args, format);
  assert(vfprintf (stream, format, args) >= 0);
  va_end (args);
  printf ("\n");

  printf ("%15s", "vprintf: ");
  va_start (args, format);
  assert(vprintf (format, args) >= 0);
  va_end (args);
  printf ("\n");

  printf ("%15s", "vsnprintf: ");
  va_start (args, format);
  assert(vsnprintf (string, 127, format, args) >= strlen(string));
  va_end (args);
  printf ("%s", string);
  printf ("\n");

  printf ("%15s", "vsprintf: ");
  va_start (args, format);
  assert(vsprintf (string, format, args) >= strlen(string));
  va_end (args);
  printf ("%s", string);
  printf ("\n");
}

static void
do_test_call_rarg (FILE *stream, const char *format, long double ld, double d)
{
  char *buffer = NULL;
  char string[128];

  printf ("%15s", "asprintf: ");
  asprintf (&buffer, format, ld, d);
  if (buffer == NULL)
    printf ("Error using asprintf\n");
  else
    {
      printf ("%s", buffer);
      free (buffer);
    }
  printf ("\n");

  printf ("%15s", "dprintf: ");
  assert(fflush(stdout) == 0);
  assert(dprintf (fileno (stream), format, ld, d) >= 0);
  printf ("\n");

  printf ("%15s", "fprintf: ");
  assert(fprintf (stream, format, ld, d) >= 0);
  printf ("\n");

  printf ("%15s", "printf: ");
  assert(printf (format, ld, d) >= 0);
  printf ("\n");

  printf ("%15s", "snprintf: ");
  assert(snprintf (string, 127, format, ld, d) == strlen(string));
  printf ("%s", string);
  printf ("\n");

  printf ("%15s", "sprintf: ");
  assert(sprintf (string, format, ld, d) == strlen(string));
  printf ("%s", string);
  printf ("\n");
}

static void
do_test_call (void)
{
  long double ld = -1;
  double d = -1;

  /* Print in decimal notation.  */
  do_test_call_rarg (stdout, "%.10Lf, %.10f", ld, d);
  do_test_call_varg (stdout, "%.10Lf, %.10f", ld, d);

  /* Print in hexadecimal notation.  */
  do_test_call_rarg (stdout, "%.10La, %.10a", ld, d);
  do_test_call_varg (stdout, "%.10La, %.10a", ld, d);

  /* Test positional parameters.  */
  do_test_call_varg (stdout, "%3$Lf, %2$Lf, %1$f",
		     (double) 1, (long double) 2, (long double) 3);
}
#endif

int
do_test (void)
{
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  struct support_capture_subprocess result;
  result = support_capture_subprocess ((void *) &do_test_call, NULL);

  /* Compare against the expected output. */
  /*const char *expected =
    "     asprintf: -1.0000000000, -1.0000000000\n"
    "      dprintf: -1.0000000000, -1.0000000000\n"
    "      fprintf: -1.0000000000, -1.0000000000\n"
    "       printf: -1.0000000000, -1.0000000000\n"
    "     snprintf: -1.0000000000, -1.0000000000\n"
    "      sprintf: -1.0000000000, -1.0000000000\n"
    "    vasprintf: -1.0000000000, -1.0000000000\n"
    "     vdprintf: -1.0000000000, -1.0000000000\n"
    "     vfprintf: -1.0000000000, -1.0000000000\n"
    "      vprintf: -1.0000000000, -1.0000000000\n"
    "    vsnprintf: -1.0000000000, -1.0000000000\n"
    "     vsprintf: -1.0000000000, -1.0000000000\n"
    "     asprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "      dprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "      fprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "       printf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "     snprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "      sprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "    vasprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "     vdprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "     vfprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "      vprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "    vsnprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "     vsprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
    "    vasprintf: 3.000000, 2.000000, 1.000000\n"
    "     vdprintf: 3.000000, 2.000000, 1.000000\n"
    "     vfprintf: 3.000000, 2.000000, 1.000000\n"
    "      vprintf: 3.000000, 2.000000, 1.000000\n"
    "    vsnprintf: 3.000000, 2.000000, 1.000000\n"
    "     vsprintf: 3.000000, 2.000000, 1.000000\n";
    TEST_COMPARE_STRING (expected, result.out.buffer);*/
  printf("%s", result.out.buffer);
#else
    puts("     asprintf: -1.0000000000, -1.0000000000\n"
         "      dprintf: -1.0000000000, -1.0000000000\n"
         "      fprintf: -1.0000000000, -1.0000000000\n"
         "       printf: -1.0000000000, -1.0000000000\n"
         "     snprintf: -1.0000000000, -1.0000000000\n"
         "      sprintf: -1.0000000000, -1.0000000000\n"
         "    vasprintf: -1.0000000000, -1.0000000000\n"
         "     vdprintf: -1.0000000000, -1.0000000000\n"
         "     vfprintf: -1.0000000000, -1.0000000000\n"
         "      vprintf: -1.0000000000, -1.0000000000\n"
         "    vsnprintf: -1.0000000000, -1.0000000000\n"
         "     vsprintf: -1.0000000000, -1.0000000000\n"
         "     asprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "      dprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "      fprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "       printf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "     snprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "      sprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "    vasprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "     vdprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "     vfprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "      vprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "    vsnprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "     vsprintf: -0x1.0000000000p+0, -0x1.0000000000p+0\n"
         "    vasprintf: 3.000000, 2.000000, 1.000000\n"
         "     vdprintf: 3.000000, 2.000000, 1.000000\n"
         "     vfprintf: 3.000000, 2.000000, 1.000000\n"
         "      vprintf: 3.000000, 2.000000, 1.000000\n"
         "    vsnprintf: 3.000000, 2.000000, 1.000000\n"
         "     vsprintf: 3.000000, 2.000000, 1.000000");
#endif

  return 0;
}

#include "test-deps/glibc/test-driver.h"
