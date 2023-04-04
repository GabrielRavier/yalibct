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

#include "test-lib/portable-symbols/internal/gnulib/Hash_table.h"
#include "test-lib/portable-symbols/internal/gnulib/attribute.h"
#include "test-lib/portable-symbols/internal/gnulib/xalloc_oversized.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct hash_entry
  {
    void *data;
    struct hash_entry *next;
  };

/* A hash table contains many internal entries, each holding a pointer to
   some user-provided data (also called a user entry).  An entry indistinctly
   refers to both the internal entry and its associated user entry.  A user
   entry contents may be hashed by a randomization function (the hashing
   function, or just "hasher" for short) into a number (or "slot") between 0
   and the current table size.  At each slot position in the hash table,
   starts a linked chain of entries for which the user data all hash to this
   slot.  A bucket is the collection of all entries hashing to the same slot.

   A good "hasher" function will distribute entries rather evenly in buckets.
   In the ideal case, the length of each bucket is roughly the number of
   entries divided by the table size.  Finding the slot for a data is usually
   done in constant time by the "hasher", and the later finding of a precise
   entry is linear in time with the size of the bucket.  Consequently, a
   larger hash table size (that is, a larger number of buckets) is prone to
   yielding shorter chains, *given* the "hasher" function behaves properly.

   Long buckets slow down the lookup algorithm.  One might use big hash table
   sizes in hope to reduce the average length of buckets, but this might
   become inordinate, as unused slots in the hash table take some space.  The
   best bet is to make sure you are using a good "hasher" function (beware
   that those are not that easy to write! :-), and to use a table size
   larger than the actual number of entries.  */

/* If an insertion makes the ratio of nonempty buckets to table size larger
   than the growth threshold (a number between 0.0 and 1.0), then increase
   the table size by multiplying by the growth factor (a number greater than
   1.0).  The growth threshold defaults to 0.8, and the growth factor
   defaults to 1.414, meaning that the table will have doubled its size
   every second time 80% of the buckets get used.  */
#define DEFAULT_GROWTH_THRESHOLD 0.8f
#define DEFAULT_GROWTH_FACTOR 1.414f

/* If a deletion empties a bucket and causes the ratio of used buckets to
   table size to become smaller than the shrink threshold (a number between
   0.0 and 1.0), then shrink the table by multiplying by the shrink factor (a
   number greater than the shrink threshold but smaller than 1.0).  The shrink
   threshold and factor default to 0.0 and 1.0, meaning that the table never
   shrinks.  */
#define DEFAULT_SHRINK_THRESHOLD 0.0f
#define DEFAULT_SHRINK_FACTOR 1.0f

/* Use this to initialize or reset a TUNING structure to
   some sensible values. */
static const Hash_tuning default_tuning =
  {
    DEFAULT_SHRINK_THRESHOLD,
    DEFAULT_SHRINK_FACTOR,
    DEFAULT_GROWTH_THRESHOLD,
    DEFAULT_GROWTH_FACTOR,
    false
  };

/* For the given hash TABLE, check the user supplied tuning structure for
   reasonable values, and return true if there is no gross error with it.
   Otherwise, definitively reset the TUNING field to some acceptable default
   in the hash table (that is, the user loses the right of further modifying
   tuning arguments), and return false.  */

static bool
check_tuning (Hash_table *table)
{
  const Hash_tuning *tuning = table->tuning;
  float epsilon;
  if (tuning == &default_tuning)
    return true;

  /* Be a bit stricter than mathematics would require, so that
     rounding errors in size calculations do not cause allocations to
     fail to grow or shrink as they should.  The smallest allocation
     is 11 (due to next_prime's algorithm), so an epsilon of 0.1
     should be good enough.  */
  epsilon = 0.1f;

  if (epsilon < tuning->growth_threshold
      && tuning->growth_threshold < 1 - epsilon
      && 1 + epsilon < tuning->growth_factor
      && 0 <= tuning->shrink_threshold
      && tuning->shrink_threshold + epsilon < tuning->shrink_factor
      && tuning->shrink_factor <= 1
      && tuning->shrink_threshold + epsilon < tuning->growth_threshold)
    return true;

  table->tuning = &default_tuning;
  return false;
}

/* Return true if CANDIDATE is a prime number.  CANDIDATE should be an odd
   number at least equal to 11.  */

static bool _GL_ATTRIBUTE_CONST
is_prime (size_t candidate)
{
  size_t divisor = 3;
  size_t square = divisor * divisor;

  while (square < candidate && (candidate % divisor))
    {
      divisor++;
      square += 4 * divisor;
      divisor++;
    }

  return ((candidate % divisor) ? true : false);
}

/* Round a given CANDIDATE number up to the nearest prime, and return that
   prime.  Primes lower than 10 are merely skipped.  */

static size_t _GL_ATTRIBUTE_CONST
next_prime (size_t candidate)
{
  /* Skip small primes.  */
  if (candidate < 10)
    candidate = 10;

  /* Make it definitely odd.  */
  candidate |= 1;

  while (SIZE_MAX != candidate && !is_prime (candidate))
    candidate += 2;

  return candidate;
}

/* Compute the size of the bucket array for the given CANDIDATE and
   TUNING, or return 0 if there is no possible way to allocate that
   many entries.  */

static size_t _GL_ATTRIBUTE_PURE
compute_bucket_size (size_t candidate, const Hash_tuning *tuning)
{
  if (!tuning->is_n_buckets)
    {
      float new_candidate = candidate / tuning->growth_threshold;
      if ((float) SIZE_MAX <= new_candidate)
        return 0;
      candidate = new_candidate;
    }
  candidate = next_prime (candidate);
  if (xalloc_oversized (candidate, sizeof (struct hash_entry *)))
    return 0;
  return candidate;
}

/* Hash KEY and return a pointer to the selected bucket.
   If TABLE->hasher misbehaves, abort.  */
static struct hash_entry *
safe_hasher (const Hash_table *table, const void *key)
{
  size_t n = table->hasher (key, table->n_buckets);
  if (! (n < table->n_buckets))
    abort ();
  return table->bucket + n;
}

/* Free a hash entry which was part of some bucket overflow,
   saving it for later recycling.  */

static void
free_entry (Hash_table *table, struct hash_entry *entry)
{
  entry->data = NULL;
  entry->next = table->free_entry_list;
  table->free_entry_list = entry;
}

/* This private function is used to help with insertion and deletion.  When
   ENTRY matches an entry in the table, return a pointer to the corresponding
   user data and set *BUCKET_HEAD to the head of the selected bucket.
   Otherwise, return NULL.  When DELETE is true and ENTRY matches an entry in
   the table, unlink the matching entry.  */

static void *
hash_find_entry (Hash_table *table, const void *entry,
                 struct hash_entry **bucket_head, bool delete)
{
  struct hash_entry *bucket = safe_hasher (table, entry);
  struct hash_entry *cursor;

  *bucket_head = bucket;

  /* Test for empty bucket.  */
  if (bucket->data == NULL)
    return NULL;

  /* See if the entry is the first in the bucket.  */
  if (entry == bucket->data || table->comparator (entry, bucket->data))
    {
      void *data = bucket->data;

      if (delete)
        {
          if (bucket->next)
            {
              struct hash_entry *next = bucket->next;

              /* Bump the first overflow entry into the bucket head, then save
                 the previous first overflow entry for later recycling.  */
              *bucket = *next;
              free_entry (table, next);
            }
          else
            {
              bucket->data = NULL;
            }
        }

      return data;
    }

  /* Scan the bucket overflow.  */
  for (cursor = bucket; cursor->next; cursor = cursor->next)
    {
      if (entry == cursor->next->data
          || table->comparator (entry, cursor->next->data))
        {
          void *data = cursor->next->data;

          if (delete)
            {
              struct hash_entry *next = cursor->next;

              /* Unlink the entry to delete, then save the freed entry for later
                 recycling.  */
              cursor->next = next->next;
              free_entry (table, next);
            }

          return data;
        }
    }

  /* No entry found.  */
  return NULL;
}
