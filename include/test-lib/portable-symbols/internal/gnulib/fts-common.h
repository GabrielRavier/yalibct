// Derived from code with this license:
/* Traverse a file hierarchy.

   Copyright (C) 2004-2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/*-
 * Copyright (c) 1990, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/FLEXSIZEOF.h"
#include "test-lib/portable-symbols/internal/gnulib/hash_initialize.h"
#include "test-lib/portable-symbols/internal/gnulib/cycle_check_init.h"
#include "test-lib/portable-symbols/internal/gnulib/i_ring_pop.h"
#include "test-lib/portable-symbols/internal/gnulib/hash_free.h"
#include "test-lib/portable-symbols/internal/gnulib/__set_errno.h"
#include "test-lib/portable-symbols/FTS.h"
#include "test-lib/portable-symbols/FTSENT.h"
#include "test-lib/portable-symbols/O_SEARCH.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define ISSET(opt) ((sp->fts_options & (opt)) != 0)

#define internal_function /* empty */

#define ISDOT(a)        (a[0] == '.' && (!a[1] || (a[1] == '.' && !a[2])))

static unsigned short int
internal_function
fts_stat(FTS *sp, register FTSENT *p, bool follow)
{
        struct stat *sbp = p->fts_statp;

        if (ISSET (FTS_LOGICAL)
            || (ISSET (FTS_COMFOLLOW) && p->fts_level == FTS_ROOTLEVEL))
                follow = true;

        /*
         * If doing a logical walk, or application requested FTS_FOLLOW, do
         * a stat(2).  If that fails, check for a non-existent symlink.  If
         * fail, set the errno from the stat call.
         */
        int flags = follow ? 0 : AT_SYMLINK_NOFOLLOW;
        if (fstatat (sp->fts_cwd_fd, p->fts_accpath, sbp, flags) < 0)
          {
            if (follow && errno == ENOENT
                && 0 <= fstatat (sp->fts_cwd_fd, p->fts_accpath, sbp,
                                 AT_SYMLINK_NOFOLLOW))
              {
                yalibct_internal_gnulib___set_errno (0);
                return FTS_SLNONE;
              }

            p->fts_errno = errno;
            memset (sbp, 0, sizeof *sbp);
            return FTS_NS;
          }

        if (S_ISDIR(sbp->st_mode)) {
                if (ISDOT(p->fts_name)) {
                        /* Command-line "." and ".." are real directories. */
                        return (p->fts_level == FTS_ROOTLEVEL ? FTS_D : FTS_DOT);
                }

                return (FTS_D);
        }
        if (S_ISLNK(sbp->st_mode))
                return (FTS_SL);
        if (S_ISREG(sbp->st_mode))
                return (FTS_F);
        return (FTS_DEFAULT);
}

/* Open the directory DIR if possible, and return a file
   descriptor.  Return -1 and set errno on failure.  It doesn't matter
   whether the file descriptor has read or write access.  */

static int
internal_function
diropen (FTS const *sp, char const *dir)
{
  int open_flags = (O_SEARCH | O_CLOEXEC | O_DIRECTORY | O_NOCTTY | O_NONBLOCK
                    | (ISSET (FTS_PHYSICAL) ? O_NOFOLLOW : 0));

  int fd = (ISSET (FTS_CWDFD)
            ? openat (sp->fts_cwd_fd, dir, open_flags)
            : open (dir, open_flags));
  return fd;
}

static void
internal_function
fts_lfree (register FTSENT *head)
{
        register FTSENT *p;

        /* Free a linked list of structures. */
        while ((p = head)) {
                head = head->fts_link;
                if (p->fts_dirp)
                        closedir (p->fts_dirp);
                free(p);
        }
}

/* Use each of these to map a device/inode pair to an FTSENT.  */
struct Active_dir
{
  dev_t dev;
  ino_t ino;
  FTSENT *fts_ent;
};

# define fts_assert(expr)       \
    do                          \
      {                         \
        if (!(expr))            \
          abort ();             \
      }                         \
    while (false)



static FTSENT *
internal_function
fts_alloc (FTS *sp, const char *name, register size_t namelen)
{
        register FTSENT *p;
        size_t len;

        /*
         * The file name is a variable length array.  Allocate the FTSENT
         * structure and the file name in one chunk.
         */
        len = FLEXSIZEOF(FTSENT, fts_name, namelen + 1);
        p = malloc(len);
        if (p == NULL)
                return (NULL);

        /* Copy the name and guarantee NUL termination. */
        memcpy(p->fts_name, name, namelen);
        p->fts_name[namelen] = '\0';

        p->fts_namelen = namelen;
        p->fts_fts = sp;
        p->fts_path = sp->fts_path;
        p->fts_errno = 0;
        p->fts_dirp = NULL;
        p->fts_flags = 0;
        p->fts_instr = FTS_NOINSTR;
        p->fts_number = 0;
        p->fts_pointer = NULL;
        return (p);
}

/*
 * Allow essentially unlimited file name lengths; find, rm, ls should
 * all work on any tree.  Most systems will allow creation of file
 * names much longer than MAXPATHLEN, even though the kernel won't
 * resolve them.  Add the size (not just what's needed) plus 256 bytes
 * so don't realloc the file name 2 bytes at a time.
 */
static bool
internal_function
fts_palloc (FTS *sp, size_t more)
{
        char *p;
        size_t new_len = sp->fts_pathlen + more + 256;

        /*
         * See if fts_pathlen would overflow.
         */
        if (new_len < sp->fts_pathlen) {
                free(sp->fts_path);
                sp->fts_path = NULL;
                yalibct_internal_gnulib___set_errno (ENAMETOOLONG);
                return false;
        }
        sp->fts_pathlen = new_len;
        p = realloc(sp->fts_path, sp->fts_pathlen);
        if (p == NULL) {
                free(sp->fts_path);
                sp->fts_path = NULL;
                return false;
        }
        sp->fts_path = p;
        return true;
}

#define SET(opt)        (sp->fts_options |= (opt))

enum Fts_stat { FTS_NO_STAT_REQUIRED = 1, FTS_STAT_REQUIRED = 2 };

/* Overload the fts_statp->st_size member (otherwise unused, when
   fts_info is FTS_NSOK) to indicate whether fts_read should stat
   this entry or not.  */
static void
fts_set_stat_required (FTSENT *p, bool required)
{
  fts_assert (p->fts_info == FTS_NSOK);
  p->fts_statp->st_size = (required
                           ? FTS_STAT_REQUIRED
                           : FTS_NO_STAT_REQUIRED);
}

static int
fts_compar (void const *a, void const *b)
{
  /* Convert A and B to the correct types, to pacify the compiler, and
     for portability to bizarre hosts where "void const *" and "FTSENT
     const **" differ in runtime representation.  The comparison
     function cannot modify *a and *b, but there is no compile-time
     check for this.  */
  FTSENT const **pa = (FTSENT const **) a;
  FTSENT const **pb = (FTSENT const **) b;
  return pa[0]->fts_fts->fts_compar (pa, pb);
}

static FTSENT *
internal_function
fts_sort (FTS *sp, FTSENT *head, register size_t nitems)
{
        register FTSENT **ap, *p;

        /* On most modern hosts, void * and FTSENT ** have the same
           run-time representation, and one can convert sp->fts_compar to
           the type qsort expects without problem.  Use the heuristic that
           this is OK if the two pointer types are the same size, and if
           converting FTSENT ** to long int is the same as converting
           FTSENT ** to void * and then to long int.  This heuristic isn't
           valid in general but we don't know of any counterexamples.  */
        FTSENT *dummy;
        int (*compare) (void const *, void const *) =
          ((sizeof &dummy == sizeof (void *)
            && (long int) &dummy == (long int) (void *) &dummy)
           ? (int (*) (void const *, void const *)) sp->fts_compar
           : fts_compar);

        /*
         * Construct an array of pointers to the structures and call qsort(3).
         * Reassemble the array in the order returned by qsort.  If unable to
         * sort for memory reasons, return the directory entries in their
         * current order.  Allocate enough space for the current needs plus
         * 40 so don't realloc one entry at a time.
         */
        if (nitems > sp->fts_nitems) {
                FTSENT **a = NULL;

                sp->fts_nitems = nitems + 40;
                if (SIZE_MAX / sizeof(*a) >= sp->fts_nitems) // NOLINT(bugprone-sizeof-expression)
                    a = (FTSENT **)realloc ((void *)sp->fts_array, sp->fts_nitems * sizeof(*a)); // NOLINT(bugprone-sizeof-expression)
                if (!a) {
		        free((void *)sp->fts_array);
                        sp->fts_array = NULL;
                        sp->fts_nitems = 0;
                        return (head);
                }
                sp->fts_array = a;
        }
        for (ap = sp->fts_array, p = head; p; p = p->fts_link)
                *ap++ = p;
        qsort((void *)sp->fts_array, nitems, sizeof(FTSENT *), compare);
        for (head = *(ap = sp->fts_array); --nitems; ++ap)
                ap[0]->fts_link = ap[1];
        ap[0]->fts_link = NULL;
        return (head);
}

static bool
AD_compare (void const *x, void const *y)
{
  struct Active_dir const *ax = x;
  struct Active_dir const *ay = y;
  return ax->ino == ay->ino
      && ax->dev == ay->dev;
}

static size_t
AD_hash (void const *x, size_t table_size)
{
  struct Active_dir const *ax = x;
  return (uintmax_t) ax->ino % table_size;
}

/* Set up the cycle-detection machinery.  */

static bool
setup_dir (FTS *fts)
{
  if (fts->fts_options & (FTS_TIGHT_CYCLE_CHECK | FTS_LOGICAL))
    {
      enum { HT_INITIAL_SIZE = 31 };
      fts->fts_cycle.ht = hash_initialize (HT_INITIAL_SIZE, NULL, AD_hash,
                                           AD_compare, free);
      if (! fts->fts_cycle.ht)
        return false;
    }
  else
    {
      fts->fts_cycle.state = malloc (sizeof *fts->fts_cycle.state);
      if (! fts->fts_cycle.state)
        return false;
      cycle_check_init (fts->fts_cycle.state);
    }

  return true;
}

static void
fd_ring_clear (I_ring *fd_ring)
{
  while ( ! i_ring_empty (fd_ring))
    {
      int fd = i_ring_pop (fd_ring);
      if (0 <= fd)
        close (fd);
    }
}

/* Free any memory used for cycle detection.  */

static void
free_dir (FTS *sp)
{
  if (sp->fts_options & (FTS_TIGHT_CYCLE_CHECK | FTS_LOGICAL))
    {
      if (sp->fts_cycle.ht)
        hash_free (sp->fts_cycle.ht);
    }
  else
    free (sp->fts_cycle.state);
}
