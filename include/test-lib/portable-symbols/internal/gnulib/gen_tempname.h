// Derived from code with this license:
/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
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

#include "test-lib/portable-symbols/internal/gnulib/gen_tempname_len.h"

#define __gen_tempname gen_tempname

int
__gen_tempname (char *tmpl, int suffixlen, int flags, int kind)
{
  return gen_tempname_len (tmpl, suffixlen, flags, kind, 6);
}
