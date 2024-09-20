// Derived from code with this license:
/* Getopt for GNU.
   Copyright (C) 1987-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library and is also part of gnulib.
   Patches to this file should be submitted to both projects.

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

#ifndef YALIBCT_LIBC_DOESNT_HAVE_GETOPT
#include <unistd.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/getopt-common.h"

/* glibc gets a LSB-compliant getopt and a POSIX-complaint __posix_getopt.
   Standalone applications just get a POSIX-compliant getopt.
   POSIX and LSB both require these functions to take 'char *const *argv'
   even though this is incorrect (because of the permutation).  */
#define GETOPT_ENTRY(NAME, POSIXLY_CORRECT)         \
  int                               \
  NAME (int argc, char *const *argv, const char *optstring) \
  {                             \
    return _getopt_internal (argc, (char **)argv, optstring,    \
                 0, 0, 0, POSIXLY_CORRECT);     \
  }

#if 0//def _LIBC
GETOPT_ENTRY(getopt, 0)
GETOPT_ENTRY(__posix_getopt, 1)
#else
GETOPT_ENTRY(getopt, 1)
#endif

#endif
