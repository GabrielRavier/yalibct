// Derived from code with this license:
/* help detect directory cycles efficiently

   Copyright (C) 2003-2004, 2006, 2009-2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/dev_ino.h"
#include <stdint.h>

struct cycle_check_state
{
  struct dev_ino dev_ino;
  uintmax_t chdir_counter;
  int magic;
};
