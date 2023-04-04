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

int
hash_insert_if_absent (Hash_table *table, void const *entry,
                       void const **matched_ent)
{
  void *data;
  struct hash_entry *bucket;

  /* The caller cannot insert a NULL entry, since hash_lookup returns NULL
     to indicate "not found", and hash_find_entry uses "bucket->data == NULL"
     to indicate an empty bucket.  */
  if (! entry)
    abort ();

  /* If there's a matching entry already in the table, return that.  */
  if ((data = hash_find_entry (table, entry, &bucket, false)) != NULL)
    {
      if (matched_ent)
        *matched_ent = data;
      return 0;
    }

  /* If the growth threshold of the buckets in use has been reached, increase
     the table size and rehash.  There's no point in checking the number of
     entries:  if the hashing function is ill-conditioned, rehashing is not
     likely to improve it.  */

  if (table->n_buckets_used
      > table->tuning->growth_threshold * table->n_buckets)
    {
      /* Check more fully, before starting real work.  If tuning arguments
         became invalid, the second check will rely on proper defaults.  */
      check_tuning (table);
      if (table->n_buckets_used
          > table->tuning->growth_threshold * table->n_buckets)
        {
          const Hash_tuning *tuning = table->tuning;
          float candidate =
            (tuning->is_n_buckets
             ? (table->n_buckets * tuning->growth_factor)
             : (table->n_buckets * tuning->growth_factor
                * tuning->growth_threshold));

          if ((float) SIZE_MAX <= candidate)
            return -1;

          /* If the rehash fails, arrange to return NULL.  */
          if (!hash_rehash (table, candidate))
            return -1;

          /* Update the bucket we are interested in.  */
          if (hash_find_entry (table, entry, &bucket, false) != NULL)
            abort ();
        }
    }

  /* ENTRY is not matched, it should be inserted.  */

  if (bucket->data)
    {
      struct hash_entry *new_entry = allocate_entry (table);

      if (new_entry == NULL)
        return -1;

      /* Add ENTRY in the overflow of the bucket.  */

      new_entry->data = (void *) entry;
      new_entry->next = bucket->next;
      bucket->next = new_entry;
      table->n_entries++;
      return 1;
    }

  /* Add ENTRY right in the bucket head.  */

  bucket->data = (void *) entry;
  table->n_entries++;
  table->n_buckets_used++;

  return 1;
}
