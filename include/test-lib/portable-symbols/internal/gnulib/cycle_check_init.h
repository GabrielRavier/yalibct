// Derived from code with this license:
/* help detect directory cycles efficiently

   Copyright (C) 2003-2006, 2009-2023 Free Software Foundation, Inc.

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

/* Written by Jim Meyering */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/cycle-check-common.h"
#include "test-lib/portable-symbols/internal/gnulib/cycle_check_state.h"

void
cycle_check_init (struct cycle_check_state *state)
{
  state->chdir_counter = 0;
  state->magic = CC_MAGIC;
}
