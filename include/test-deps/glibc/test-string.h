// Derived from code with this license:
/* Test and measure string and memory functions.
   Copyright (C) 1999-2022 Free Software Foundation, Inc.
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

#include "test-lib/portable-symbols/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>

#  define ITERATIONS iterations
#ifndef BUF1PAGES
# define BUF1PAGES 1
#endif


static int ret;
static size_t page_size;
static unsigned char *buf1, *buf2;
static size_t iterations = 100000;
static int do_srandom;
static unsigned int seed;

static inline void
test_init (void)
{
  page_size = 2 * getpagesize ();
#ifdef MIN_PAGE_SIZE
  if (page_size < MIN_PAGE_SIZE)
    page_size = MIN_PAGE_SIZE;
#endif
  buf1 = mmap (0, (BUF1PAGES + 1) * page_size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANON, -1, 0);
  if (buf1 == MAP_FAILED)
    error (EXIT_FAILURE, errno, "mmap failed");
  if (mprotect (buf1 + BUF1PAGES * page_size, page_size, PROT_NONE))
    error (EXIT_FAILURE, errno, "mprotect failed");
  buf2 = mmap (0, 2 * page_size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANON, -1, 0);
  if (buf2 == MAP_FAILED)
    error (EXIT_FAILURE, errno, "mmap failed");
  if (mprotect (buf2 + page_size, page_size, PROT_NONE))
    error (EXIT_FAILURE, errno, "mprotect failed");

#if 0
  int fdr = open ("/dev/urandom", O_RDONLY);
  if (fdr < 0 || read (fdr, &seed, sizeof (seed)) != sizeof (seed))
      seed = time (NULL);
  if (fdr >= 0)
      close (fdr);
  do_srandom = 1;
#endif

  if (do_srandom)
    {
      printf ("Setting seed to 0x%x\n", seed);
      srandom (seed);
    }

  memset (buf1, 0xa5, BUF1PAGES * page_size);
  memset (buf2, 0x5a, page_size);
}
