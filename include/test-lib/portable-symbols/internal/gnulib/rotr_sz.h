// Derived from code with this license:
/* bitrotate.h - Rotate bits in integers
   Copyright (C) 2008-2023 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson <simon@josefsson.org>, 2008. */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/BITROTATE_INLINE.h"
#include <stdint.h>
#include <limits.h>
#include <stddef.h>

/* Given a size_t argument X, return the value corresponding
   to rotating the bits N steps to the right.  N must be between 1 to
   (CHAR_BIT * sizeof (size_t) - 1) inclusive.  */
BITROTATE_INLINE size_t
rotr_sz (size_t x, int n)
{
  return ((x >> n) | (x << ((CHAR_BIT * sizeof x) - n))) & SIZE_MAX;
}
