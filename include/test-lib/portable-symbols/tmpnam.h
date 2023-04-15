// Derived from code with this license:
/* Copyright (C) 1991-2022 Free Software Foundation, Inc.
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

#ifdef YALIBCT_LIBC_HAS_TMPNAM
#include <stdio.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/gen_tempname.h"
#include "test-lib/portable-symbols/internal/gnulib/path_search.h"
#include "test-lib/portable-symbols/L_tmpnam.h"

static char tmpnam_buffer[L_tmpnam];

/* Generate a unique filename in P_tmpdir.

   This function is *not* thread safe!  */
char *
tmpnam (char s[L_tmpnam])
{
  /* By using two buffers we manage to be thread safe in the case
     where S != NULL.  */
  char tmpbufmem[L_tmpnam];
  char *tmpbuf = s ?: tmpbufmem;

  /* In the following call we use the buffer pointed to by S if
     non-NULL although we don't know the size.  But we limit the size
     to L_tmpnam characters in any case.  */
  if (__builtin_expect (path_search (tmpbuf, L_tmpnam, NULL, NULL, 0),
            0))
    return NULL;

  if (__glibc_unlikely (gen_tempname (tmpbuf, 0, 0, __GT_NOCREATE)))
    return NULL;

  if (s == NULL)
    return (char *) memcpy (tmpnam_buffer, tmpbuf, L_tmpnam);

  return s;
}

#endif
