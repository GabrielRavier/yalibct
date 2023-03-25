// Derived from code with this license:
/* Copyright (C) 2006-2023 Free Software Foundation, Inc.
   Written by Paul Eggert, Bruno Haible, Derek Price.
   This file is part of gnulib.

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

#ifdef YALIBCT_LIBC_HAS_SCNDFAST16
#include <inttypes.h>
#else

#include <stdint.h>

#undef SCNdFAST16
# if INT_FAST16_MAX > INT32_MAX
#  define SCNdFAST16 SCNd64
# elif INT_FAST16_MAX == 0x7fff
#  define SCNdFAST16 "hd"
# else
#  define SCNdFAST16 "d"
# endif


#endif
