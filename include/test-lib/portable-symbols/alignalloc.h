// Derived from code with this license:
/* aligned memory allocation

   Copyright 2022 Free Software Foundation, Inc.

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

#pragma once

#ifdef YALIBCT_LIBC_HAS_ALIGNALLOC
#include <alignalloc.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/idx_t.h"
#include "test-lib/portable-symbols/ckd_add.h"
#include "test-lib/portable-symbols/alignof.h"
#include "test-lib/portable-symbols/static_assert.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/* Return P aligned down to ALIGNMENT, which should be a power of two.  */

static void *
yalibct_internal_alignalloc_align_down (void *p, yalibct_internal_gnulib_idx_t alignment)
{
  char *c = p;
  return c - ((uintptr_t) p & (alignment - 1));
}

/* If alignalloc returned R and the base of the originally-allocated
   storage is less than R - UCHAR_MAX, return the address of a pointer
   holding the base of the originally-allocated storage.  */

static void **
yalibct_internal_alignalloc_address_of_pointer_to_malloced (unsigned char *r)
{
  /* The pointer P is located at the highest address A such that A is
     aligned for pointers, and A + sizeof P < R so that there is room
     for a 0 byte at R - 1.  This approach assumes UCHAR_MAX is large
     enough so that there is room for P; although true on all
     plausible platforms, check the assumption to be safe.  */
    static_assert (sizeof (void *) + alignof (void *) - 1 <= UCHAR_MAX, "");

    return (void **)yalibct_internal_alignalloc_align_down (r - 1 - sizeof (void *), alignof (void *));
}

/* Return an ALIGNMENT-aligned pointer to new storage of size SIZE,
   or a null pointer (setting errno) if memory is exhausted.
   ALIGNMENT must be a power of two.
   If SIZE is zero, on success return a unique pointer each time.
   To free storage later, call alignfree.  */

void *
alignalloc (yalibct_internal_gnulib_idx_t alignment, yalibct_internal_gnulib_idx_t size)
{
  /* malloc (ALIGNMENT + SIZE); if it succeeds, there must be at least
     one byte available before the returned pointer.  It's OK if
     ALIGNMENT + SIZE fits in size_t but not idx_t.  */

  size_t malloc_size;
  unsigned char *q = NULL;
  if (!ckd_add (&malloc_size, size, alignment))
      q = malloc(malloc_size);
  if (!q)
    {
      errno = ENOMEM;
      return NULL;
    }

  unsigned char *r = yalibct_internal_alignalloc_align_down (q + alignment, alignment);
  yalibct_internal_gnulib_idx_t offset = r - q;

  if (offset <= UCHAR_MAX)
    r[-1] = offset;
  else
    {
      r[-1] = 0;
      *yalibct_internal_alignalloc_address_of_pointer_to_malloced (r) = q;
    }

  return r;
}

/* Free storage allocated via alignalloc.  Do nothing if PTR is null.  */

void
alignfree (void *ptr)
{
  if (ptr)
    {
      unsigned char *r = ptr;
      unsigned char offset = r[-1];
      void *q = offset ? r - offset : *yalibct_internal_alignalloc_address_of_pointer_to_malloced (r);
      free (q);
    }
}

#endif
