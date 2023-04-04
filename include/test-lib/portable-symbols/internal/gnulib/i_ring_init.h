// Derived from code with this license:
/* a simple ring buffer
   Copyright (C) 2006, 2009-2023 Free Software Foundation, Inc.

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

/* written by Jim Meyering */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/I_ring.h"

void
i_ring_init (I_ring *ir, int default_val)
{
  int i;
  ir->ir_empty = true;
  ir->ir_front = 0;
  ir->ir_back = 0;
  for (i = 0; i < I_RING_SIZE; i++)
    ir->ir_data[i] = default_val;
  ir->ir_default_val = default_val;
}
