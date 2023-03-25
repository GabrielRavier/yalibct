/* Test memory leak in __printf_fp_l (bug 26215).
   Copyright (C) 2020-2022 Free Software Foundation, Inc.
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
#include "test-lib/portable-symbols/mtrace.h"
#include <stdio.h>
#include <stdlib.h>

int
do_test (void)
{
  mtrace ();
  FILE *fp = fopen ("/dev/full", "w");
  TEST_VERIFY_EXIT (fp != NULL);
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  TEST_COMPARE (fprintf (fp, "%.65536f", 1.0), -1);
#endif
  assert(fclose (fp) == 0);
  return 0;
}
