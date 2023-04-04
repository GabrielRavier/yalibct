// Derived from code with this license:
/* Run-time assert-like macros.

   Copyright (C) 2014-2023 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#pragma once

#include <assert.h>

/* Check E's value at runtime, and report an error and abort if not.
   However, do nothing if NDEBUG is defined.

   Unlike standard 'assert', this macro compiles E even when NDEBUG
   is defined, so as to catch typos and avoid some GCC warnings.
   Unlike 'affirm', it is OK for E to use hard-to-optimize features,
   since E is not executed if NDEBUG is defined.  */

#ifdef NDEBUG
# define assure(E) ((void) (0 && (E)))
#else
# define assure(E) assert (E)
#endif
