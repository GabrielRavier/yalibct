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

#include "test-lib/portable-symbols/internal/gnulib/i_ring_empty.h"
#include <stdlib.h>

int
i_ring_pop (I_ring *ir)
{
  int top_val;
  if (i_ring_empty (ir))
    abort ();
  top_val = ir->ir_data[ir->ir_front];
  ir->ir_data[ir->ir_front] = ir->ir_default_val;
  if (ir->ir_front == ir->ir_back)
    ir->ir_empty = true;
  else
    ir->ir_front = ((ir->ir_front + I_RING_SIZE - 1) % I_RING_SIZE);
  return top_val;
}
