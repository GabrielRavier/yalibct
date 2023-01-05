// Derived from code with this license:
/* Copyright (C) 2014-2022 Free Software Foundation, Inc.
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

#pragma once

#include "test-lib/hedley.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>

#define DIAG_PUSH_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_PUSH

/* Compare the strings LEFT and RIGHT and report a test failure if
   they are different.  Also report failure if one of the arguments is
   a null pointer and the other is not.  The strings should be
   reasonably short because on mismatch, both are printed.  */
#define TEST_COMPARE_STRING(left, right)                         \
  (support_test_compare_string (left, right, __FILE__, __LINE__, \
                                #left, #right))

static void support_record_failure()
{
}

/* Record a test failure, print the failure message and terminate with
   exit status 1.  */
#define FAIL_EXIT1(...) \
  support_exit_failure_impl (1, __FILE__, __LINE__, __VA_ARGS__)

enum
  {
    /* Test exit status which indicates that the feature is
       unsupported. */
    EXIT_UNSUPPORTED = 77,

    /* Default timeout is twenty seconds.  Tests should normally
       complete faster than this, but if they don't, that's abnormal
       (a bug) anyways.  */
    DEFAULT_TIMEOUT = 20,

    /* Used for command line argument parsing.  */
    OPT_DIRECT = 1000,
    OPT_TESTDIR,
  };

static void
print_failure (const char *file, int line, const char *format, va_list ap)
{
  int saved_errno = errno;
  printf ("error: %s:%d: ", file, line);
  vprintf (format, ap);
  puts ("");
  errno = saved_errno;
}

void
support_exit_failure_impl (int status, const char *file, int line,
                           const char *format, ...)
{
  if (status != EXIT_SUCCESS && status != EXIT_UNSUPPORTED)
    support_record_failure ();
  va_list ap;
  va_start (ap, format);
  print_failure (file, line, format, ap);
  va_end (ap);
  exit (status);
}

void
xfclose (FILE *fp)
{
  if (ferror (fp))
    FAIL_EXIT1 ("stdio stream closed with pending errors");
  if (fflush (fp) != 0)
    FAIL_EXIT1 ("fflush: %m");
  if (fclose (fp) != 0)
    FAIL_EXIT1 ("fclose: %m");
}

/* Compare the two integers LEFT and RIGHT and report failure if they
   are different.  */
#define TEST_COMPARE(left, right)                                       \
  ({                                                                    \
    /* + applies the integer promotions, for bitfield support.   */     \
    typedef __typeof__ (+ (left)) __left_type;                          \
    typedef __typeof__ (+ (right)) __right_type;                        \
    __left_type __left_value = (left);                                  \
    __right_type __right_value = (right);                               \
    int __left_is_positive = __left_value > 0;                          \
    int __right_is_positive = __right_value > 0;                        \
    /* Prevent use with floating-point types.  */                       \
    support_static_assert ((__left_type) 1.0 == (__left_type) 1.5,      \
                           "left value has floating-point type");       \
    support_static_assert ((__right_type) 1.0 == (__right_type) 1.5,    \
                           "right value has floating-point type");      \
    /* Prevent accidental use with larger-than-long long types.  */     \
    support_static_assert (sizeof (__left_value) <= sizeof (long long), \
                           "left value fits into long long");           \
    support_static_assert (sizeof (__right_value) <= sizeof (long long), \
                    "right value fits into long long");                 \
    /* Compare the value.  */                                           \
    if (__left_value != __right_value                                   \
        || __left_is_positive != __right_is_positive)                   \
      /* Pass the sign for printing the correct value.  */              \
      support_test_compare_failure                                      \
        (__FILE__, __LINE__,                                            \
         #left, __left_value, __left_is_positive, sizeof (__left_type), \
         #right, __right_value, __right_is_positive, sizeof (__right_type)); \
  })

/* Ignore the diagnostic OPTION.  VERSION is the most recent GCC
   version for which the diagnostic has been confirmed to appear in
   the absence of the pragma (in the form MAJOR.MINOR for GCC 4.x,
   just MAJOR for GCC 5 and later).  Uses of this pragma should be
   reviewed when the GCC version given is no longer supported for
   building glibc; the version number should always be on the same
   source line as the macro name, so such uses can be found with grep.
   Uses should come with a comment giving more details of the
   diagnostic, and an architecture on which it is seen if possibly
   optimization-related and not in architecture-specific code.  This
   macro should only be used if the diagnostic seems hard to fix (for
   example, optimization-related false positives).  */
#define DIAG_IGNORE_NEEDS_COMMENT(version, option)     \
  YALIBCT_DIAGNOSTIC_IGNORE(option)

#define DIAG_POP_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_POP

/* Record a test failure and exit if EXPR evaluates to false.  */
#define TEST_VERIFY_EXIT(expr)                                  \
  ({                                                            \
    if (expr)                                                   \
      ;                                                         \
    else                                                        \
      support_test_verify_exit_impl                             \
        (1, __FILE__, __LINE__, #expr);                         \
  })

void
support_test_verify_impl (const char *file, int line, const char *expr)
{
  int saved_errno = errno;
  support_record_failure ();
  printf ("error: %s:%d: not true: %s\n", file, line, expr);
  errno = saved_errno;
  assert(false);
}

void
support_test_verify_exit_impl (int status, const char *file, int line,
                               const char *expr)
{
  support_test_verify_impl (file, line, expr);
  exit (status);
}

static void
support_test_compare_failure_report (const char *which, const char *expr, long long value, int positive,
        int size)
{
  printf ("  %s: ", which);
  if (positive)
    printf ("%llu", (unsigned long long) value);
  else
    printf ("%lld", value);
  unsigned long long mask
    = (~0ULL) >> (8 * (sizeof (unsigned long long) - size));
  printf (" (0x%llx); from: %s\n", (unsigned long long) value & mask, expr);
}

void
support_test_compare_failure (const char *file, int line,
                              const char *left_expr,
                              long long left_value,
                              int left_positive,
                              int left_size,
                              const char *right_expr,
                              long long right_value,
                              int right_positive,
                              int right_size)
{
  int saved_errno = errno;
  support_record_failure ();
  if (left_size != right_size)
    printf ("%s:%d: numeric comparison failure (widths %d and %d)\n",
            file, line, left_size * 8, right_size * 8);
  else
    printf ("%s:%d: numeric comparison failure\n", file, line);
  support_test_compare_failure_report (" left", left_expr, left_value, left_positive, left_size);
  support_test_compare_failure_report ("right", right_expr, right_value, right_positive, right_size);
  errno = saved_errno;
  assert(false);
}

/* Compare [LEFT, LEFT + LEFT_LENGTH) with [RIGHT, RIGHT +
   RIGHT_LENGTH) and report a test failure if the arrays are
   different.  LEFT_LENGTH and RIGHT_LENGTH are measured in bytes.  If
   the length is null, the corresponding pointer is ignored (i.e., it
   can be NULL).  The blobs should be reasonably short because on
   mismatch, both are printed.  */
#define TEST_COMPARE_BLOB(left, left_length, right, right_length)       \
  (support_test_compare_blob (left, left_length, right, right_length,   \
                              __FILE__, __LINE__,                       \
                              #left, #left_length, #right, #right_length))

#define support_static_assert HEDLEY_STATIC_ASSERT

#define YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_CHAR unsigned char
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_(C) C
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_SUPPORT_QUOTE_BLOB support_quote_blob
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_WIDE 0

#include "glibc/internal/support_quote_blob_main.h"

static void
support_test_compare_blob_report_length (const char *what, unsigned long int length, const char *expr)
{
  printf ("  %s %lu bytes (from %s)\n", what, length, expr);
}

static void
support_test_compare_blob_report_blob (const char *what, const unsigned char *blob,
             unsigned long int length, const char *expr)
{
  if (blob == NULL && length > 0)
    printf ("  %s (evaluated from %s): NULL\n", what, expr);
  else if (length > 0)
    {
      printf ("  %s (evaluated from %s):\n", what, expr);
      char *quoted = support_quote_blob (blob, length);
      printf ("      \"%s\"\n", quoted);
      free (quoted);

      fputs ("     ", stdout);
      for (unsigned long i = 0; i < length; ++i)
        printf (" %02X", blob[i]);
      putc ('\n', stdout);
    }
}

void
support_test_compare_blob (const void *left, unsigned long int left_length,
                           const void *right, unsigned long int right_length,
                           const char *file, int line,
                           const char *left_expr, const char *left_len_expr,
                           const char *right_expr, const char *right_len_expr)
{
  /* No differences are possible if both lengths are null.  */
  if (left_length == 0 && right_length == 0)
    return;

  if (left_length != right_length || left == NULL || right == NULL
      || memcmp (left, right, left_length) != 0)
    {
      support_record_failure ();
      printf ("%s:%d: error: blob comparison failed\n", file, line);
      if (left_length == right_length)
        printf ("  blob length: %lu bytes\n", left_length);
      else
        {
          support_test_compare_blob_report_length ("left length: ", left_length, left_len_expr);
          support_test_compare_blob_report_length ("right length:", right_length, right_len_expr);
        }
      support_test_compare_blob_report_blob ("left", left, left_length, left_expr);
      support_test_compare_blob_report_blob ("right", right, right_length, right_expr);
      assert(false);
    }
}

#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_CHAR char
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_UCHAR unsigned char
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_LPREFIX ""
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_STRLEN strlen
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_MEMCMP memcmp
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_SUPPORT_QUOTE_BLOB support_quote_blob
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_SUPPORT_TEST_COMPARE_STRING support_test_compare_string
#define YALIBCT_GLIBC_INTERNAL_SUPPORT_TEST_COMPARE_STRING_MAIN_WIDE 0

#include "glibc/internal/support_test_compare_string_main.h"

/* array_length (VAR) is the number of elements in the array VAR.  VAR
   must evaluate to an array, not a pointer.  */
#define array_length(var)                                               \
  (sizeof (var) / sizeof ((var)[0])                                     \
   + 0 * sizeof (struct {                                               \
       _Static_assert (!__builtin_types_compatible_p                    \
                       (__typeof (var), __typeof (&(var)[0])),          \
                       "argument must be an array");                    \
   }))

#define do_test main

#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(major, minor) HEDLEY_GNUC_VERSION_CHECK((major), (minor), 0)
#endif
