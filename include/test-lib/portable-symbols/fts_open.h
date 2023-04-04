// Based on code with this license:
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

#ifdef YALIBCT_LIBC_HAS_FTS_OPEN
#include <fts.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/fts-common.h"
#include "test-lib/portable-symbols/internal/gnulib/i_ring_init.h"
#include "test-lib/portable-symbols/MAX.h"

#define CLR(opt)        (sp->fts_options &= ~(opt))

static size_t
internal_function _GL_ATTRIBUTE_PURE
fts_maxarglen (char * const *argv)
{
        size_t len, max;

        for (max = 0; *argv; ++argv)
                if ((len = strlen(*argv)) > max)
                        max = len;
        return (max + 1);
}

FTS *
fts_open (char * const *argv,
          register int options,
          int (*compar) (FTSENT const **, FTSENT const **))
{
        register FTS *sp;
        register FTSENT *p, *root;
        register size_t nitems;
        FTSENT *parent = NULL;
        FTSENT *tmp = NULL;     /* pacify gcc */
        bool defer_stat;

        /* Options check. */
        if (options & ~FTS_OPTIONMASK) {
                yalibct_internal_gnulib___set_errno (EINVAL);
                return (NULL);
        }
        if ((options & FTS_NOCHDIR) && (options & FTS_CWDFD)) {
                yalibct_internal_gnulib___set_errno (EINVAL);
                return (NULL);
        }
        if ( ! (options & (FTS_LOGICAL | FTS_PHYSICAL))) {
                yalibct_internal_gnulib___set_errno (EINVAL);
                return (NULL);
        }

        /* Allocate/initialize the stream */
        sp = calloc (1, sizeof *sp);
        if (sp == NULL)
                return (NULL);
        sp->fts_compar = compar;
        sp->fts_options = options;

        /* Logical walks turn on NOCHDIR; symbolic links are too hard. */
        if (ISSET(FTS_LOGICAL)) {
                SET(FTS_NOCHDIR);
                CLR(FTS_CWDFD);
        }

        /* Initialize fts_cwd_fd.  */
        sp->fts_cwd_fd = AT_FDCWD;
#if 0
        if ( ISSET(FTS_CWDFD) && ! HAVE_OPENAT_SUPPORT)
          {
            /* While it isn't technically necessary to open "." this
               early, doing it here saves us the trouble of ensuring
               later (where it'd be messier) that "." can in fact
               be opened.  If not, revert to FTS_NOCHDIR mode.  */
            int fd = open (".", O_SEARCH | O_CLOEXEC);
            if (fd < 0)
              {
                /* Even if "." is unreadable, don't revert to FTS_NOCHDIR mode
                   on systems like Linux+PROC_FS, where our openat emulation
                   is good enough.  Note: on a system that emulates
                   openat via /proc, this technique can still fail, but
                   only in extreme conditions, e.g., when the working
                   directory cannot be saved (i.e. save_cwd fails) --
                   and that happens on Linux only when "." is unreadable
                   and the CWD would be longer than PATH_MAX.
                   FIXME: once Linux kernel openat support is well established,
                   replace the above open call and this entire if/else block
                   with the body of the if-block below.  */
                if ( openat_needs_fchdir ())
                  {
                    SET(FTS_NOCHDIR);
                    CLR(FTS_CWDFD);
                  }
              }
            else
              {
                close (fd);
              }
          }
#endif

        /*
         * Start out with 1K of file name space, and enough, in any case,
         * to hold the user's file names.
         */
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif
        {
          size_t maxarglen = fts_maxarglen(argv);
          if (! fts_palloc(sp, MAX(maxarglen, MAXPATHLEN)))
                  goto mem1;
        }

        /* Allocate/initialize root's parent. */
        if (*argv != NULL) {
                if ((parent = fts_alloc(sp, "", 0)) == NULL)
                        goto mem2;
                parent->fts_level = FTS_ROOTPARENTLEVEL;
          }

        /* The classic fts implementation would call fts_stat with
           a new entry for each iteration of the loop below.
           If the comparison function is not specified or if the
           FTS_DEFER_STAT option is in effect, don't stat any entry
           in this loop.  This is an attempt to minimize the interval
           between the initial stat/lstat/fstatat and the point at which
           a directory argument is first opened.  This matters for any
           directory command line argument that resides on a file system
           without genuine i-nodes.  If you specify FTS_DEFER_STAT along
           with a comparison function, that function must not access any
           data via the fts_statp pointer.  */
        defer_stat = (compar == NULL || ISSET(FTS_DEFER_STAT));

        /* Allocate/initialize root(s). */
        for (root = NULL, nitems = 0; *argv != NULL; ++argv, ++nitems) {
                /* *Do* allow zero-length file names. */
                size_t len = strlen(*argv);

                if ( ! (options & FTS_VERBATIM))
                  {
                    /* If there are two or more trailing slashes, trim all but one,
                       but don't change "//" to "/", and do map "///" to "/".  */
                    char const *v = *argv;
                    if (2 < len && v[len - 1] == '/')
                      while (1 < len && v[len - 2] == '/')
                        --len;
                  }

                if ((p = fts_alloc(sp, *argv, len)) == NULL)
                        goto mem3;
                p->fts_level = FTS_ROOTLEVEL;
                p->fts_parent = parent;
                p->fts_accpath = p->fts_name;
                /* Even when defer_stat is true, be sure to stat the first
                   command line argument, since fts_read (at least with
                   FTS_XDEV) requires that.  */
                if (defer_stat && root != NULL) {
                        p->fts_info = FTS_NSOK;
                        fts_set_stat_required(p, true);
                } else {
                        p->fts_info = fts_stat(sp, p, false);
                }

                /*
                 * If comparison routine supplied, traverse in sorted
                 * order; otherwise traverse in the order specified.
                 */
                if (compar) {
                        p->fts_link = root;
                        root = p;
                } else {
                        p->fts_link = NULL;
                        if (root == NULL)
                                tmp = root = p;
                        else {
                                tmp->fts_link = p;
                                tmp = p;
                        }
                }
        }
        if (compar && nitems > 1)
                root = fts_sort(sp, root, nitems);

        /*
         * Allocate a dummy pointer and make fts_read think that we've just
         * finished the node before the root(s); set p->fts_info to FTS_INIT
         * so that everything about the "current" node is ignored.
         */
        if ((sp->fts_cur = fts_alloc(sp, "", 0)) == NULL)
                goto mem3;
        sp->fts_cur->fts_link = root;
        sp->fts_cur->fts_info = FTS_INIT;
        sp->fts_cur->fts_level = 1;
        if (! setup_dir (sp))
                goto mem3;

        /*
         * If using chdir(2), grab a file descriptor pointing to dot to ensure
         * that we can get back here; this could be avoided for some file names,
         * but almost certainly not worth the effort.  Slashes, symbolic links,
         * and ".." are all fairly nasty problems.  Note, if we can't get the
         * descriptor we run anyway, just more slowly.
         */
        if (!ISSET(FTS_NOCHDIR) && !ISSET(FTS_CWDFD)
            && (sp->fts_rfd = diropen (sp, ".")) < 0)
                SET(FTS_NOCHDIR);

        i_ring_init (&sp->fts_fd_ring, -1);
        return (sp);

mem3:   fts_lfree(root);
        free(parent);
mem2:   free(sp->fts_path);
mem1:   free(sp);
        return (NULL);
}

#endif
