// Derived from code with this license:
/* System definitions for code taken from the GNU C Library

   Copyright 2017-2023 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, see
   <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

#pragma once

#include <errno.h>

/* From glibc <errno.h>.  */
#ifndef yalibct_internal_gnulib___set_errno
# define yalibct_internal_gnulib___set_errno(val) (errno = (val))
#endif
