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

#include "test-lib/portable-symbols/obstack_init.h"
#include "test-lib/portable-symbols/internal/gnulib/hash-common.h"
#include "test-lib/portable-symbols/internal/gnulib/rotr_sz.h"

/* If the user passes a NULL hasher, we hash the raw pointer.  */
static size_t
raw_hasher (const void *data, size_t n)
{
  /* When hashing unique pointers, it is often the case that they were
     generated by malloc and thus have the property that the low-order
     bits are 0.  As this tends to give poorer performance with small
     tables, we rotate the pointer value before performing division,
     in an attempt to improve hash quality.  */
  size_t val = rotr_sz ((size_t) data, 3);
  return val % n;
}

/* If the user passes a NULL comparator, we use pointer comparison.  */
static bool
raw_comparator (const void *a, const void *b)
{
  return a == b;
}

Hash_table *
hash_initialize (size_t candidate, const Hash_tuning *tuning,
                 Hash_hasher hasher, Hash_comparator comparator,
                 Hash_data_freer data_freer)
{
  Hash_table *table;

  if (hasher == NULL)
    hasher = raw_hasher;
  if (comparator == NULL)
    comparator = raw_comparator;

  table = malloc (sizeof *table);
  if (table == NULL)
    return NULL;

  if (!tuning)
    tuning = &default_tuning;
  table->tuning = tuning;
  if (!check_tuning (table))
    {
      /* Fail if the tuning options are invalid.  This is the only occasion
         when the user gets some feedback about it.  Once the table is created,
         if the user provides invalid tuning options, we silently revert to
         using the defaults, and ignore further request to change the tuning
         options.  */
      goto fail;
    }

  table->n_buckets = compute_bucket_size (candidate, tuning);
  if (!table->n_buckets)
    goto fail;

  table->bucket = calloc (table->n_buckets, sizeof *table->bucket);
  if (table->bucket == NULL)
    goto fail;
  table->bucket_limit = table->bucket + table->n_buckets;
  table->n_buckets_used = 0;
  table->n_entries = 0;

  table->hasher = hasher;
  table->comparator = comparator;
  table->data_freer = data_freer;

  table->free_entry_list = NULL;
#if 1//USE_OBSTACK
#  define obstack_chunk_alloc malloc
#  define obstack_chunk_free free
  obstack_init (&table->entry_stack);
#endif
  return table;

 fail:
  free (table);
  return NULL;
}
