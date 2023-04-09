// Derived from code with this license:
/* Legacy test skeleton.
   Copyright (C) 1998-2022 Free Software Foundation, Inc.
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

/* This test skeleton is to support running existing tests.  New tests
   should use <support/test-driver.c> instead; see the documentation
   in that file for instructions, and <support/README-testing.c> for a
   minimal example.  */

/* This list of headers is needed so that tests which include
   "../test-skeleton.c" at the beginning still compile.  */

#pragma once

/* TEST_FUNCTION is no longer used. */
static int
legacy_test_function (int argc __attribute__ ((unused)),
              char **argv __attribute__ ((unused)))
{
#ifdef TEST_FUNCTION
  return TEST_FUNCTION;
# undef TEST_FUNCTION
#else
  return do_test (argc, argv);
#endif
}
#define TEST_FUNCTION_ARGV legacy_test_function

#include "test-deps/glibc/test-driver.h"
