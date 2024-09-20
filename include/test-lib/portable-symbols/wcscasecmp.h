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

#ifndef YALIBCT_LIBC_DOESNT_HAVE_WCSCASECMP
#include <wchar.h>
#else

#include <wctype.h>

#undef wcscasecmp
#define wcscasecmp yalibct_internal_wcscasecmp

/* Compare S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int
wcscasecmp (const wchar_t *s1, const wchar_t *s2)
{
  wint_t c1, c2;

  if (s1 == s2)
    return 0;

  do
    {
      c1 = towlower (*s1++);
      c2 = towlower (*s2++);
      if (c1 == L'\0')
        break;
    }
  while (c1 == c2);

  return c1 - c2;
}


#endif
