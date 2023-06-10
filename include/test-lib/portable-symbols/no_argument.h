// Derived from code with this license:
/* Declarations for getopt (GNU extensions).
   Copyright (C) 1989-2023 Free Software Foundation, Inc.
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

#ifdef YALIBCT_LIBC_HAS_NO_ARGUMENT
#include <getopt.h>
#else

/* Names for the values of the 'has_arg' field of 'struct option'.  */

#undef no_argument
#define no_argument     0

#endif
