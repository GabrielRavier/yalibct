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
#include "test-lib/portable-symbols/obstack_alloc.h"

/* Insertion and deletion.  */

/* Get a new hash entry for a bucket overflow, possibly by recycling a
   previously freed one.  If this is not possible, allocate a new one.  */

static struct hash_entry *
allocate_entry (Hash_table *table)
{
  struct hash_entry *new;

  if (table->free_entry_list)
    {
      new = table->free_entry_list;
      table->free_entry_list = new->next;
    }
  else
    {
#if 1//USE_OBSTACK
      new = obstack_alloc (&table->entry_stack, sizeof *new);
#else
      new = malloc (sizeof *new);
#endif

    }

  return new;
}

/* Internal helper, to move entries from SRC to DST.  Both tables must
   share the same free entry list.  If SAFE, only move overflow
   entries, saving bucket heads for later, so that no allocations will
   occur.  Return false if the free entry list is exhausted and an
   allocation fails.  */

static bool
transfer_entries (Hash_table *dst, Hash_table *src, bool safe)
{
  struct hash_entry *bucket;
  struct hash_entry *cursor;
  struct hash_entry *next;
  for (bucket = src->bucket; bucket < src->bucket_limit; bucket++)
    if (bucket->data)
      {
        void *data;
        struct hash_entry *new_bucket;

        /* Within each bucket, transfer overflow entries first and
           then the bucket head, to minimize memory pressure.  After
           all, the only time we might allocate is when moving the
           bucket head, but moving overflow entries first may create
           free entries that can be recycled by the time we finally
           get to the bucket head.  */
        for (cursor = bucket->next; cursor; cursor = next)
          {
            data = cursor->data;
            new_bucket = safe_hasher (dst, data);

            next = cursor->next;

            if (new_bucket->data)
              {
                /* Merely relink an existing entry, when moving from a
                   bucket overflow into a bucket overflow.  */
                cursor->next = new_bucket->next;
                new_bucket->next = cursor;
              }
            else
              {
                /* Free an existing entry, when moving from a bucket
                   overflow into a bucket header.  */
                new_bucket->data = data;
                dst->n_buckets_used++;
                free_entry (dst, cursor);
              }
          }
        /* Now move the bucket head.  Be sure that if we fail due to
           allocation failure that the src table is in a consistent
           state.  */
        data = bucket->data;
        bucket->next = NULL;
        if (safe)
          continue;
        new_bucket = safe_hasher (dst, data);

        if (new_bucket->data)
          {
            /* Allocate or recycle an entry, when moving from a bucket
               header into a bucket overflow.  */
            struct hash_entry *new_entry = allocate_entry (dst);

            if (new_entry == NULL)
              return false;

            new_entry->data = data;
            new_entry->next = new_bucket->next;
            new_bucket->next = new_entry;
          }
        else
          {
            /* Move from one bucket header to another.  */
            new_bucket->data = data;
            dst->n_buckets_used++;
          }
        bucket->data = NULL;
        src->n_buckets_used--;
      }
  return true;
}

bool
hash_rehash (Hash_table *table, size_t candidate)
{
  Hash_table storage;
  Hash_table *new_table;
  size_t new_size = compute_bucket_size (candidate, table->tuning);

  if (!new_size)
    return false;
  if (new_size == table->n_buckets)
    return true;
  new_table = &storage;
  new_table->bucket = calloc (new_size, sizeof *new_table->bucket);
  if (new_table->bucket == NULL)
    return false;
  new_table->n_buckets = new_size;
  new_table->bucket_limit = new_table->bucket + new_size;
  new_table->n_buckets_used = 0;
  new_table->n_entries = 0;
  new_table->tuning = table->tuning;
  new_table->hasher = table->hasher;
  new_table->comparator = table->comparator;
  new_table->data_freer = table->data_freer;

  /* In order for the transfer to successfully complete, we need
     additional overflow entries when distinct buckets in the old
     table collide into a common bucket in the new table.  The worst
     case possible is a hasher that gives a good spread with the old
     size, but returns a constant with the new size; if we were to
     guarantee table->n_buckets_used-1 free entries in advance, then
     the transfer would be guaranteed to not allocate memory.
     However, for large tables, a guarantee of no further allocation
     introduces a lot of extra memory pressure, all for an unlikely
     corner case (most rehashes reduce, rather than increase, the
     number of overflow entries needed).  So, we instead ensure that
     the transfer process can be reversed if we hit a memory
     allocation failure mid-transfer.  */

  /* Merely reuse the extra old space into the new table.  */
#if 1//USE_OBSTACK
  new_table->entry_stack = table->entry_stack;
#endif

  new_table->free_entry_list = table->free_entry_list;

  if (transfer_entries (new_table, table, false))
    {
      /* Entries transferred successfully; tie up the loose ends.  */
      free (table->bucket);
      table->bucket = new_table->bucket;
      table->bucket_limit = new_table->bucket_limit;
      table->n_buckets = new_table->n_buckets;
      table->n_buckets_used = new_table->n_buckets_used;
      table->free_entry_list = new_table->free_entry_list;
      /* table->n_entries and table->entry_stack already hold their value.  */
      return true;
    }

  /* We've allocated new_table->bucket (and possibly some entries),
     exhausted the free list, and moved some but not all entries into
     new_table.  We must undo the partial move before returning
     failure.  The only way to get into this situation is if new_table
     uses fewer buckets than the old table, so we will reclaim some
     free entries as overflows in the new table are put back into
     distinct buckets in the old table.

     There are some pathological cases where a single pass through the
     table requires more intermediate overflow entries than using two
     passes.  Two passes give worse cache performance and takes
     longer, but at this point, we're already out of memory, so slow
     and safe is better than failure.  */
  table->free_entry_list = new_table->free_entry_list;
  if (! (transfer_entries (table, new_table, true)
         && transfer_entries (table, new_table, false)))
    abort ();
  /* table->n_entries already holds its value.  */
  free (new_table->bucket);
  return false;
}
