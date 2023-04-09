// Derived from code with this license:
/* obstack.c - subroutines used implicitly by object stack macros
   Copyright (C) 1988-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#include "test-lib/portable-symbols/struct_obstack.h"

/* Call functions with either the traditional malloc/free calling
   interface, or the mmalloc/mfree interface (that adds an extra first
   argument), based on the value of use_extra_arg.  */

static void *
call_chunkfun (struct obstack *h, size_t size)
{
  if (h->use_extra_arg)
    return h->chunkfun.extra (h->extra_arg, size);
  else
    return h->chunkfun.plain (size);
}

#if __STDC_VERSION__ < 199901L
# define __FLEXIBLE_ARRAY_MEMBER 1
#else
# define __FLEXIBLE_ARRAY_MEMBER
#endif

struct _obstack_chunk           /* Lives at front of each chunk. */
{
  char *limit;                  /* 1 past end of this chunk */
  struct _obstack_chunk *prev;  /* address of prior chunk or NULL */
  char contents[__FLEXIBLE_ARRAY_MEMBER]; /* objects begin here */
};

/* If B is the base of an object addressed by P, return the result of
   aligning P to the next multiple of A + 1.  B and P must be of type
   char *.  A + 1 must be a power of 2.  */

#define __BPTR_ALIGN(B, P, A) ((B) + (((P) - (B) + (A)) & ~(A)))

/* Similar to __BPTR_ALIGN (B, P, A), except optimize the common case
   where pointers can be converted to integers, aligned as integers,
   and converted back again.  If ptrdiff_t is narrower than a
   pointer (e.g., the AS/400), play it safe and compute the alignment
   relative to B.  Otherwise, use the faster strategy of computing the
   alignment relative to 0.  */

#define __PTR_ALIGN(B, P, A)                              \
  __BPTR_ALIGN (sizeof (ptrdiff_t) < sizeof (void *) ? (B) : (char *) 0,      \
                P, A)
