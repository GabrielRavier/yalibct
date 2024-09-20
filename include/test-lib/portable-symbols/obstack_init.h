// Derived from code with this license:
/* obstack.h - object stack macros
   Copyright (C) 1988-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
/* Summary:

   All the apparent functions defined here are macros. The idea
   is that you would use these pre-tested macros to solve a
   very specific set of problems, and they would run fast.
   Caution: no side-effects in arguments please!! They may be
   evaluated MANY times!!

   These macros operate a stack of objects.  Each object starts life
   small, and may grow to maturity.  (Consider building a word syllable
   by syllable.)  An object can move while it is growing.  Once it has
   been "finished" it never changes address again.  So the "top of the
   stack" is typically an immature growing object, while the rest of the
   stack is of mature, fixed size and fixed address objects.

   These routines grab large chunks of memory, using a function you
   supply, called 'obstack_chunk_alloc'.  On occasion, they free chunks,
   by calling 'obstack_chunk_free'.  You must define them and declare
   them before using any obstack macros.

   Each independent stack is represented by a 'struct obstack'.
   Each of the obstack macros expects a pointer to such a structure
   as the first argument.

   One motivation for this package is the problem of growing char strings
   in symbol tables.  Unless you are "fascist pig with a read-only mind"
   --Gosper's immortal quote from HAKMEM item 154, out of context--you
   would not like to put any arbitrary upper limit on the length of your
   symbols.

   In practice this often means you will build many short symbols and a
   few long symbols.  At the time you are reading a symbol you don't know
   how long it is.  One traditional method is to read a symbol into a
   buffer, realloc()ating the buffer every time you try to read a symbol
   that is longer than the buffer.  This is beaut, but you still will
   want to copy the symbol from the buffer to a more permanent
   symbol-table entry say about half the time.

   With obstacks, you can work differently.  Use one obstack for all symbol
   names.  As you read a symbol, grow the name in the obstack gradually.
   When the name is complete, finalize it.  Then, if the symbol exists already,
   free the newly read name.

   The way we do this is to take a large chunk, allocating memory from
   low addresses.  When you want to build a symbol in the chunk you just
   add chars above the current "high water mark" in the chunk.  When you
   have finished adding chars, because you got to the end of the symbol,
   you know how long the chars are, and you can create a new object.
   Mostly the chars will not burst over the highest address of the chunk,
   because you would typically expect a chunk to be (say) 100 times as
   long as an average object.

   In case that isn't clear, when we have enough chars to make up
   the object, THEY ARE ALREADY CONTIGUOUS IN THE CHUNK (guaranteed)
   so we just point to it where it lies.  No moving of chars is
   needed and this is the second win: potentially long strings need
   never be explicitly shuffled. Once an object is formed, it does not
   change its address during its lifetime.

   When the chars burst over a chunk boundary, we allocate a larger
   chunk, and then copy the partly formed object from the end of the old
   chunk to the beginning of the new larger chunk.  We then carry on
   accreting characters to the end of the object as we normally would.

   A special macro is provided to add a single char at a time to a
   growing object.  This allows the use of register variables, which
   break the ordinary 'growth' macro.

   Summary:
        We allocate large chunks.
        We carve out one object at a time from the current chunk.
        Once carved, an object never moves.
        We are free to append data of any size to the currently
          growing object.
        Exactly one object is growing in an obstack at any one time.
        You can run one obstack per control block.
        You may have as many control blocks as you dare.
        Because of the way we do it, you can "unwind" an obstack
          back to a previous state. (You may remove objects much
          as you would with a stack.)
 */

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_OBSTACK_INIT
#include <obstack.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/obstack-common.h"
#include "test-lib/portable-symbols/obstack_alloc_failed_handler.h"
#include "test-lib/portable-symbols/MAX.h"
#include <stdint.h>

/* To prevent prototype warnings provide complete argument list.  */
#undef obstack_init
#define obstack_init(h)                               \
  _obstack_begin ((h), 0, 0,                              \
                  _OBSTACK_CAST (void *(*) (size_t), obstack_chunk_alloc),    \
                  _OBSTACK_CAST (void (*) (void *), obstack_chunk_free))

/* Determine default alignment.  */

/* If malloc were really smart, it would round addresses to DEFAULT_ALIGNMENT.
   But in fact it might be less smart and round addresses to as much as
   DEFAULT_ROUNDING.  So we prepare for it to do that.

   DEFAULT_ALIGNMENT cannot be an enum constant; see gnulib's alignof.h.  */
#define DEFAULT_ALIGNMENT MAX (__alignof__ (long double),             \
                               MAX (__alignof__ (uintmax_t),              \
                                    __alignof__ (void *)))
#define DEFAULT_ROUNDING MAX (sizeof (long double),               \
                               MAX (sizeof (uintmax_t),               \
                                    sizeof (void *)))

/* Initialize an obstack H for use.  Specify chunk size SIZE (0 means default).
   Objects start on multiples of ALIGNMENT (0 means use default).

   Return nonzero if successful, calls obstack_alloc_failed_handler if
   allocation fails.  */

static int
_obstack_begin_worker (struct obstack *h,
                       _OBSTACK_SIZE_T size, _OBSTACK_SIZE_T alignment)
{
  struct _obstack_chunk *chunk; /* points to new chunk */

  if (alignment == 0)
    alignment = DEFAULT_ALIGNMENT;
  if (size == 0)
    /* Default size is what GNU malloc can fit in a 4096-byte block.  */
    {
      /* 12 is sizeof (mhead) and 4 is EXTRA from GNU malloc.
         Use the values for range checking, because if range checking is off,
         the extra bytes won't be missed terribly, but if range checking is on
         and we used a larger request, a whole extra 4096 bytes would be
         allocated.

         These number are irrelevant to the new GNU malloc.  I suspect it is
         less sensitive to the size of the request.  */
      int extra = ((((12 + DEFAULT_ROUNDING - 1) & ~(DEFAULT_ROUNDING - 1))
                    + 4 + DEFAULT_ROUNDING - 1)
                   & ~(DEFAULT_ROUNDING - 1));
      size = 4096 - extra;
    }

  h->chunk_size = size;
  h->alignment_mask = alignment - 1;

  chunk = h->chunk = call_chunkfun (h, h->chunk_size);
  if (!chunk)
    (*obstack_alloc_failed_handler) ();
  h->next_free = h->object_base = __PTR_ALIGN ((char *) chunk, chunk->contents,
                                               alignment - 1);
  h->chunk_limit = chunk->limit = (char *) chunk + h->chunk_size;
  chunk->prev = 0;
  /* The initial chunk now contains no empty object.  */
  h->maybe_empty_object = 0;
  h->alloc_failed = 0;
  return 1;
}

int
_obstack_begin (struct obstack *h,
                _OBSTACK_SIZE_T size, _OBSTACK_SIZE_T alignment,
                void *(*chunkfun) (size_t),
                void (*freefun) (void *))
{
  h->chunkfun.plain = chunkfun;
  h->freefun.plain = freefun;
  h->use_extra_arg = 0;
  return _obstack_begin_worker (h, size, alignment);
}

#endif
