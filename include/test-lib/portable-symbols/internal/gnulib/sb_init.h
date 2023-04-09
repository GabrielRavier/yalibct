// Derived from code with this license:
/* A buffer that accumulates a string by piecewise concatenation.
   Copyright (C) 2021-2023 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/struct_string_buffer.h"

void
sb_init (struct string_buffer *buffer)
{
  buffer->data = buffer->space;
  buffer->length = 0;
  buffer->allocated = sizeof (buffer->space);
  buffer->error = false;
}
