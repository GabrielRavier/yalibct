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

#include "test-lib/compiler-features.h"
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <features.h>

#define DIAG_PUSH_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_PUSH
/* Compare the strings LEFT and RIGHT and report a test failure if
   they are different.  Also report failure if one of the arguments is
   a null pointer and the other is not.  The strings should be
   reasonably short because on mismatch, both are printed.  */
#define TEST_COMPARE_STRING(left, right)                         \
    assert(strcmp((left), (right)) == 0)

#define support_static_assert HEDLEY_STATIC_ASSERT

/* Compare the two integers LEFT and RIGHT and report failure if they
   are different.  */
#define TEST_COMPARE(left, right) assert((left) == (right) && (((left) < 0) == ((right) < 0)))

#define DIAG_IGNORE_NEEDS_COMMENT(version, option) YALIBCT_DIAGNOSTIC_IGNORE(option)

#define DIAG_POP_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_POP

#define do_test main

#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(major, minor) HEDLEY_GNUC_VERSION_CHECK(major, minor, 0)
#endif

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

static inline void
support_test_compare_blob(const void *left, unsigned long int left_length,
                          const void *right, unsigned long int right_length,
                          const char *file, int line, const char *left_exp,
                          const char *left_len_exp, const char *right_exp,
                          const char *right_len_exp)
{
      /* No differences are possible if both lengths are null.  */
  if (left_length == 0 && right_length == 0)
    return;

  assert(left_length == right_length);
  assert(left != NULL);
  assert(right != NULL);
  assert(memcmp(left, right, left_length) == 0);
}
