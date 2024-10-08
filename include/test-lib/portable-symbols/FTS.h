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

/*
 * Copyright (c) 1989, 1993
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
 *
 *      @(#)fts.h       8.3 (Berkeley) 8/14/94
 */

#pragma once

#if 0//YALIBCT_LIBC_DOESNT_HAVE_FTS
#include <fts.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/I_ring.h"
#include <sys/stat.h>
#include <stddef.h>

typedef struct {
        struct _ftsent *fts_cur;        /* current node */
        struct _ftsent *fts_child;      /* linked list of children */
        struct _ftsent **fts_array;     /* sort array */
        dev_t fts_dev;                  /* starting device # */
        char *fts_path;                 /* file name for this descent */
        int fts_rfd;                    /* fd for root */
        int fts_cwd_fd;                 /* the file descriptor on which the
                                           virtual cwd is open, or AT_FDCWD */
        size_t fts_pathlen;             /* sizeof(path) */
        size_t fts_nitems;              /* elements in the sort array */
        int (*fts_compar) (struct _ftsent const **, struct _ftsent const **);
                                        /* compare fn */

# define FTS_COMFOLLOW  0x0001          /* follow command line symlinks */
# define FTS_LOGICAL    0x0002          /* logical walk */
# define FTS_NOCHDIR    0x0004          /* don't change directories */
# define FTS_NOSTAT     0x0008          /* don't get stat info */
# define FTS_PHYSICAL   0x0010          /* physical walk */
# define FTS_SEEDOT     0x0020          /* return dot and dot-dot */
# define FTS_XDEV       0x0040          /* don't cross devices */
# define FTS_WHITEOUT   0x0080          /* return whiteout information */

  /* There are two ways to detect cycles.
     The lazy way (which works only with FTS_PHYSICAL),
     with which one may process a directory that is a
     part of the cycle several times before detecting the cycle.
     The "tight" way, whereby fts uses more memory (proportional
     to number of "active" directories, aka distance from root
     of current tree to current directory -- see active_dir_ht)
     to detect any cycle right away.  For example, du must use
     this option to avoid counting disk space in a cycle multiple
     times, but chown -R need not.
     The default is to use the constant-memory lazy way, when possible
     (see below).

     However, with FTS_LOGICAL (when following symlinks, e.g., chown -L)
     using lazy cycle detection is inadequate.  For example, traversing
     a directory containing a symbolic link to a peer directory, it is
     possible to encounter the same directory twice even though there
     is no cycle:
     dir
     ...
     slink -> dir
     So, when FTS_LOGICAL is selected, we have to use a different
     mode of cycle detection: FTS_TIGHT_CYCLE_CHECK.  */
# define FTS_TIGHT_CYCLE_CHECK  0x0100

  /* Use this flag to enable semantics with which the parent
     application may be made both more efficient and more robust.
     Whereas the default is to visit each directory in a recursive
     traversal (via chdir), using this flag makes it so the initial
     working directory is never changed.  Instead, these functions
     perform the traversal via a virtual working directory, maintained
     through the file descriptor member, fts_cwd_fd.  */
# define FTS_CWDFD              0x0200

  /* Historically, for each directory that fts initially encounters, it would
     open it, read all entries, and stat each entry, storing the results, and
     then it would process the first entry.  But that behavior is bad for
     locality of reference, and also causes trouble with inode-simulating
     file systems like FAT, CIFS, FUSE-based ones, etc., when entries from
     their name/inode cache are flushed too early.
     Use this flag to make fts_open and fts_read defer the stat/lstat/fststat
     of each entry until it is actually processed.  However, note that if you
     use this option and also specify a comparison function, that function may
     not examine any data via fts_statp.  However, when fts_statp->st_mode is
     nonzero, the S_IFMT type bits are valid, with mapped dirent.d_type data.
     Of course, that happens only on file systems that provide useful
     dirent.d_type data.  */
# define FTS_DEFER_STAT         0x0400

  /* Use this flag to disable stripping of trailing slashes
     from input path names during fts_open initialization.  */
# define FTS_VERBATIM   0x0800

# define FTS_OPTIONMASK 0x0fff          /* valid user option mask */

# define FTS_NAMEONLY   0x1000          /* (private) child names only */
# define FTS_STOP       0x2000          /* (private) unrecoverable error */
        int fts_options;                /* fts_open options, global flags */

        /* Map a directory's device number to a boolean.  The boolean is
           true if for that file system (type determined by a single fstatfs
           call per FS) st_nlink can be used to calculate the number of
           sub-directory entries in a directory.
           Using this table is an optimization that permits us to look up
           file system type on a per-inode basis at the minimal cost of
           calling fstatfs only once per traversed device.  */
        struct hash_table *fts_leaf_optimization_works_ht;

        union {
                /* This data structure is used if FTS_TIGHT_CYCLE_CHECK is
                   specified.  It records the directories between a starting
                   point and the current directory.  I.e., a directory is
                   recorded here IFF we have visited it once, but we have not
                   yet completed processing of all its entries.  Every time we
                   visit a new directory, we add that directory to this set.
                   When we finish with a directory (usually by visiting it a
                   second time), we remove it from this set.  Each entry in
                   this data structure is a device/inode pair.  This data
                   structure is used to detect directory cycles efficiently and
                   promptly even when the depth of a hierarchy is in the tens
                   of thousands.  */
                struct hash_table *ht;

                /* FIXME: rename these two members to have the fts_ prefix */
                /* This data structure uses a lazy cycle-detection algorithm,
                   as done by rm via cycle-check.c.  It's the default,
                   but it's not appropriate for programs like du.  */
                struct cycle_check_state *state;
        } fts_cycle;

        /* A stack of the file descriptors corresponding to the
           most-recently traversed parent directories.
           Currently used only in FTS_CWDFD mode.  */
        I_ring fts_fd_ring;
} FTS;


#endif
