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

int
i_ring_push (I_ring *ir, int val)
{
  unsigned int dest_idx = (ir->ir_front + !ir->ir_empty) % I_RING_SIZE;
  int old_val = ir->ir_data[dest_idx];
  ir->ir_data[dest_idx] = val;
  ir->ir_front = dest_idx;
  if (dest_idx == ir->ir_back)
    ir->ir_back = (ir->ir_back + !ir->ir_empty) % I_RING_SIZE;
  ir->ir_empty = false;
  return old_val;
}
