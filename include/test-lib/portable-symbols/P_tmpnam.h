// Derived from code with this license:
/* Copyright (C) 1999, 2001-2002, 2006, 2009-2023 Free Software Foundation,
   Inc.
   This file is part of the GNU C Library.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_P_TMPDIR
#include <stdio.h>
#else

#include <stdio.h>

#undef P_tmpdir
# ifdef _P_tmpdir /* native Windows */
#  define P_tmpdir _P_tmpdir
# else
#  define P_tmpdir "/tmp"
# endif


#endif
