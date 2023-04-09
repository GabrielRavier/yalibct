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

#include "test-lib/portable-symbols/internal/gnulib/string-buffer-common.h"
#include "test-lib/portable-symbols/internal/gnulib/sb_free.h"

/* Returns the contents of BUFFER, and frees all other memory held
   by BUFFER.  Returns NULL upon failure or if there was an error earlier.  */
char *
sb_dupfree (struct string_buffer *buffer)
{
  if (buffer->error)
    goto fail;

  if (sb_ensure_more_bytes (buffer, 1) < 0)
    goto fail;
  buffer->data[buffer->length] = '\0';
  buffer->length++;

  if (buffer->data == buffer->space)
    {
      char *copy = (char *) malloc (buffer->length);
      if (copy == NULL)
        goto fail;
      memcpy (copy, buffer->data, buffer->length);
      return copy;
    }
  else
    {
      /* Shrink the string before returning it.  */
      char *contents = buffer->data;
      if (buffer->length < buffer->allocated)
        {
          contents = realloc (contents, buffer->length);
          if (contents == NULL)
            goto fail;
        }
      return contents;
    }

 fail:
  sb_free (buffer);
  return NULL;
}
