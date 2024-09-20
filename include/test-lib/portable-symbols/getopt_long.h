// Derived from code with this license:
/* getopt_long and getopt_long_only entry points for GNU getopt.
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

#ifndef YALIBCT_LIBC_DOESNT_HAVE_GETOPT_LONG
#include <getopt.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/getopt-common.h"

/* The type of the 'argv' argument to getopt_long and getopt_long_only
   is properly 'char **', since both functions may write to the array
   (in order to move all the options to the beginning).  However, for
   compatibility with old versions of LSB, glibc has to use 'char *const *'
   instead.  */
#ifndef __getopt_argv_const
# define __getopt_argv_const const
#endif

/* When used standalone, do not attempt to use alloca.  */
# define __libc_use_alloca(size) 0

int
getopt_long (int argc, char *__getopt_argv_const *argv, const char *options,
         const struct option *long_options, int *opt_index)
{
  return _getopt_internal (argc, (char **) argv, options, long_options,
               opt_index, 0, 0);
}

#endif
