// Derived from code with this license:
/*
   Copyright 2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */
#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_CKD_ADD
#include <stdckdint.h>
#else

#include <stdbool.h>

// Check out gnulib's _GL_HAS_BUILTIN_ADD_OVERFLOW whenever we need to make this portable when we don't have __builtin_add_overflow
#define YALIBCT_INTERNAL__GL_INT_ADD_WRAPV __builtin_add_overflow

/* Store into *R the low-order bits of A + B.
   Return 1 if the result overflows, 0 otherwise.
   A, B, and *R can have any integer type other than char, bool, a
   bit-precise integer type, or an enumeration type.

   This is like the standard macro introduced in C23, except that
   arguments should not have side effects.  */

#undef ckd_add
#define ckd_add(r, a, b) ((bool) YALIBCT_INTERNAL__GL_INT_ADD_WRAPV (a, b, r))

#endif
