// Derived from code with this license:
/* xmalloc.c -- malloc with out of memory checking

   Copyright (C) 1990-2000, 2002-2006, 2008-2022 Free Software Foundation, Inc.

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

#ifndef YALIBCT_LIBC_DOESNT_HAVE_XMALLOC
#include <xalloc.h>
#else

#include "test-lib/hedley.h"
#include "test-lib/portable-symbols/internal/gnulib/xalloc_die.h"
#include <stdlib.h>

static inline void *HEDLEY_PURE
yalibct_internal_xalloc_nonnull (void *p)
{
  if (!p)
    yalibct_internal_gnulib_xalloc_die ();
  return p;
}

/* Allocate S bytes of memory dynamically, with error checking.  */

static inline void *
xmalloc (size_t s)
{
  return yalibct_internal_xalloc_nonnull (malloc (s));
}

#endif
