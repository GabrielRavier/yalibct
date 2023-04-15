// Derived from code with this license:
/* basename.c -- return the last element in a file name

   Copyright (C) 1990, 1998-2001, 2003-2006, 2009-2023 Free Software
   Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */
#pragma once

#include "test-lib/portable-symbols/internal/gnulib/ISSLASH.h"
#include <stdbool.h>

/* Filename support.
   HAS_DEVICE(Filename)        tests whether Filename contains a device
                               specification.
   FILE_SYSTEM_PREFIX_LEN(Filename)  length of the device specification
                                     at the beginning of Filename,
                                     index of the part consisting of
                                     alternating components and slashes.
 */
#if defined _WIN32 || defined __CYGWIN__ \
    || defined __EMX__ || defined __MSDOS__ || defined __DJGPP__
  /* Internal macro: Tests whether a character is a drive letter.  */
# define yalibct_internal_last_component__IS_DRIVE_LETTER(C) \
    (((C) >= 'A' && (C) <= 'Z') || ((C) >= 'a' && (C) <= 'z'))
  /* Help the compiler optimizing it.  This assumes ASCII.  */
# undef yalibct_internal_last_component__IS_DRIVE_LETTER
# define yalibct_internal_last_component__IS_DRIVE_LETTER(C) \
    (((unsigned int) (C) | ('a' - 'A')) - 'a' <= 'z' - 'a')
# define yalibct_internal_last_component_HAS_DEVICE(Filename) \
    (yalibct_internal_last_component__IS_DRIVE_LETTER ((Filename)[0]) && (Filename)[1] == ':')
# define yalibct_internal_last_component_FILE_SYSTEM_PREFIX_LEN(Filename) (yalibct_internal_last_component_HAS_DEVICE (Filename) ? 2 : 0)
#else
  /* Unix */
# define yalibct_internal_last_component_HAS_DEVICE(Filename) ((void) (Filename), 0)
# define yalibct_internal_last_component_FILE_SYSTEM_PREFIX_LEN(Filename) ((void) (Filename), 0)
#endif

char *
yalibct_internal_gnulib_last_component (char const *name)
{
  char const *base = name + yalibct_internal_last_component_FILE_SYSTEM_PREFIX_LEN (name);
  char const *p;
  bool last_was_slash = false;

  while (ISSLASH (*base))
    base++;

  for (p = base; *p; p++)
    {
      if (ISSLASH (*p))
        last_was_slash = true;
      else if (last_was_slash)
        {
          base = p;
          last_was_slash = false;
        }
    }

  return (char *) base;
}
