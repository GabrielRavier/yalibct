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
#include <stdlib.h>
#include <string.h>

/* Ensures that INCREMENT bytes are available beyond the current used length
   of BUFFER.
   Returns 0, or -1 in case of out-of-memory error.  */
static int
sb_ensure_more_bytes (struct string_buffer *buffer, size_t increment)
{
  size_t incremented_length = buffer->length + increment;
  if (incremented_length < increment)
    /* Overflow.  */
    return -1;

  if (buffer->allocated < incremented_length)
    {
      size_t new_allocated = 2 * buffer->allocated;
      if (new_allocated < buffer->allocated)
        /* Overflow.  */
        return -1;
      if (new_allocated < incremented_length)
        new_allocated = incremented_length;

      char *new_data;
      if (buffer->data == buffer->space)
        {
          new_data = (char *) malloc (new_allocated);
          if (new_data == NULL)
            /* Out-of-memory.  */
            return -1;
          memcpy (new_data, buffer->data, buffer->length);
        }
      else
        {
          new_data = (char *) realloc (buffer->data, new_allocated);
          if (new_data == NULL)
            /* Out-of-memory.  */
            return -1;
        }
      buffer->data = new_data;
      buffer->allocated = new_allocated;
    }
  return 0;
}
