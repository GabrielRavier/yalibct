// Derived from code with this license:
/* hash - hashing table processing.

   Copyright (C) 1998-2004, 2006-2007, 2009-2023 Free Software Foundation, Inc.

   Written by Jim Meyering, 1992.

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

/* A generic hash table package.  */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/hash-common.h"

void
hash_free (Hash_table *table)
{
  struct hash_entry *bucket;
  struct hash_entry *cursor;
  struct hash_entry *next;

  /* Call the user data_freer function.  */
  if (table->data_freer && table->n_entries)
    {
      for (bucket = table->bucket; bucket < table->bucket_limit; bucket++)
        {
          if (bucket->data)
            {
              for (cursor = bucket; cursor; cursor = cursor->next)
                table->data_freer (cursor->data);
            }
        }
    }

  /* Free all bucket overflowed entries.  */
  for (bucket = table->bucket; bucket < table->bucket_limit; bucket++)
    {
      for (cursor = bucket->next; cursor; cursor = next)
        {
          next = cursor->next;
          free (cursor);
        }
    }

  /* Also reclaim the internal list of previously freed entries.  */
  for (cursor = table->free_entry_list; cursor; cursor = next)
    {
      next = cursor->next;
      free (cursor);
    }

  /* Free the remainder of the hash table structure.  */
  free (table->bucket);
  free (table);
}
