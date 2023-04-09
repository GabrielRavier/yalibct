/* Check two strings for equality.
   Copyright (C) 2018-2022 Free Software Foundation, Inc.
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

static void
yalibct_internal_glibc_support_test_compare_string_report_length (const char *what, const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR *str, size_t length)
{
  if (str == NULL)
    printf ("  %s string: NULL\n", what);
  else
    printf ("  %s string: %zu %s\n", what, length,
	    WIDE ? "wide characters" : "bytes");
}

static void
report_string (const char *what, const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_UCHAR *blob,
               size_t length, const char *expr)
{
  if (length > 0)
    {
      printf ("  %s (evaluated from %s):\n", what, expr);
      char *quoted = SUPPORT_QUOTE_BLOB (blob, length);
      printf ("      %s\"%s\"\n", LPREFIX, quoted);
      free (quoted);

      fputs ("     ", stdout);
      for (size_t i = 0; i < length; ++i)
        printf (" %02X", (unsigned int) blob[i]);
      putc ('\n', stdout);
    }
}

static size_t
string_length_or_zero (const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR *str)
{
  if (str == NULL)
    return 0;
  else
    return YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_STRLEN (str);
}

void
SUPPORT_TEST_COMPARE_STRING (const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR *left, const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR *right,
                             const char *file, int line,
                             const char *left_expr, const char *right_expr)
{
  /* Two null pointers are accepted.  */
  if (left == NULL && right == NULL)
    return;

  size_t left_length = string_length_or_zero (left);
  size_t right_length = string_length_or_zero (right);

  if (left_length != right_length || left == NULL || right == NULL
      || MEMCMP (left, right, left_length) != 0)
    {
      support_record_failure ();
      printf ("%s:%d: error: string comparison failed\n", file, line);
      if (left_length == right_length && right != NULL && left != NULL)
        printf ("  string length: %zu %s\n", left_length,
		WIDE ? "wide characters" : "bytes");
      else
        {
          yalibct_internal_glibc_support_test_compare_string_report_length ("left", left, left_length);
          yalibct_internal_glibc_support_test_compare_string_report_length ("right", right, right_length);
        }
      report_string ("left", (const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_UCHAR *) left,
                     left_length, left_expr);
      report_string ("right", (const YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_UCHAR *) right,
                     right_length, right_expr);
    }
}

#undef YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR
#undef YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_UCHAR
#undef LPREFIX
#undef YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_STRLEN
#undef MEMCMP
#undef SUPPORT_QUOTE_BLOB
#undef SUPPORT_TEST_COMPARE_STRING
#undef WIDE
