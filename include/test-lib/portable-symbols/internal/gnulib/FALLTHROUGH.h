// Derived from code with this license:
/* ATTRIBUTE_* macros for using attributes in GCC and similar compilers

   Copyright 2020-2023 Free Software Foundation, Inc.

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

/* Provide public ATTRIBUTE_* names for the private _GL_ATTRIBUTE_*
   macros used within Gnulib.  */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/_GL_ATTRIBUTE_FALLTHROUGH.h"

/* Do not warn if control flow falls through to the immediately
   following 'case' or 'default' label.  */
/* Applies to: Empty statement (;), inside a 'switch' statement.  */
#define FALLTHROUGH _GL_ATTRIBUTE_FALLTHROUGH
