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

#include "test-lib/portable-symbols/internal/gnulib/hash_rehash.h"

void *
hash_remove (Hash_table *table, const void *entry)
{
  void *data;
  struct hash_entry *bucket;

  data = hash_find_entry (table, entry, &bucket, true);
  if (!data)
    return NULL;

  table->n_entries--;
  if (!bucket->data)
    {
      table->n_buckets_used--;

      /* If the shrink threshold of the buckets in use has been reached,
         rehash into a smaller table.  */

      if (table->n_buckets_used
          < table->tuning->shrink_threshold * table->n_buckets)
        {
          /* Check more fully, before starting real work.  If tuning arguments
             became invalid, the second check will rely on proper defaults.  */
          check_tuning (table);
          if (table->n_buckets_used
              < table->tuning->shrink_threshold * table->n_buckets)
            {
              const Hash_tuning *tuning = table->tuning;
              size_t candidate =
                (tuning->is_n_buckets
                 ? table->n_buckets * tuning->shrink_factor
                 : (table->n_buckets * tuning->shrink_factor
                    * tuning->growth_threshold));

              if (!hash_rehash (table, candidate))
                {
                  /* Failure to allocate memory in an attempt to
                     shrink the table is not fatal.  But since memory
                     is low, we can at least be kind and free any
                     spare entries, rather than keeping them tied up
                     in the free entry list.  */
                  struct hash_entry *cursor = table->free_entry_list;
                  struct hash_entry *next;
                  while (cursor)
                    {
                      next = cursor->next;
                      free (cursor);
                      cursor = next;
                    }
                  table->free_entry_list = NULL;
                }
            }
        }
    }

  return data;
}
