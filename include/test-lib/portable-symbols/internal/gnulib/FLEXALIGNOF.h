// Derived from code with this license:
/* Sizes of structs with flexible array members.

   Copyright 2016-2023 Free Software Foundation, Inc.

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
   <https://www.gnu.org/licenses/>.

   Written by Paul Eggert.  */

#pragma once

#include <stddef.h>

/* Nonzero multiple of alignment of TYPE, suitable for FLEXSIZEOF below.
   On older platforms without _Alignof, use a pessimistic bound that is
   safe in practice even if FLEXIBLE_ARRAY_MEMBER is 1.
   On newer platforms, use _Alignof to get a tighter bound.  */

#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 201112
# define FLEXALIGNOF(type) (sizeof (type) & ~ (sizeof (type) - 1))
#else
# define FLEXALIGNOF(type) _Alignof (type)
#endif
