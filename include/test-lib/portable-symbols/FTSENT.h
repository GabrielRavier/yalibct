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

#if 0//def YALIBCT_LIBC_HAS_FTSENT
#include <fts.h>
#else

#include "test-lib/portable-symbols/FTS.h"
#include <dirent.h>

#  if __STDC_VERSION__ < 199901L
#   define __FLEXIBLE_ARRAY_MEMBER 1
#  else
#   define __FLEXIBLE_ARRAY_MEMBER
#  endif

typedef struct _ftsent {
        struct _ftsent *fts_cycle;      /* cycle node */
        struct _ftsent *fts_parent;     /* parent directory */
        struct _ftsent *fts_link;       /* next file in directory */
        DIR *fts_dirp;                  /* Dir pointer for any directory
                                           containing more entries than we
                                           read at one time.  */
        long fts_number;                /* local numeric value */
        void *fts_pointer;              /* local address value */
        char *fts_accpath;              /* access file name */
        char *fts_path;                 /* root name; == fts_fts->fts_path */
        int fts_errno;                  /* errno for this node */
        int fts_symfd;                  /* fd for symlink */
        size_t fts_pathlen;             /* strlen(fts_path) */

        FTS *fts_fts;                   /* the file hierarchy itself */

# define FTS_ROOTPARENTLEVEL    (-1)
# define FTS_ROOTLEVEL           0
        ptrdiff_t fts_level;            /* depth (-1 to N) */

        size_t fts_namelen;             /* strlen(fts_name) */

# define FTS_D           1              /* preorder directory */
# define FTS_DC          2              /* directory that causes cycles */
# define FTS_DEFAULT     3              /* none of the above */
# define FTS_DNR         4              /* unreadable directory */
# define FTS_DOT         5              /* dot or dot-dot */
# define FTS_DP          6              /* postorder directory */
# define FTS_ERR         7              /* error; errno is set */
# define FTS_F           8              /* regular file */
# define FTS_INIT        9              /* initialized only */
# define FTS_NS         10              /* stat(2) failed */
# define FTS_NSOK       11              /* no stat(2) requested */
# define FTS_SL         12              /* symbolic link */
# define FTS_SLNONE     13              /* symbolic link without target */
# define FTS_W          14              /* whiteout object */
        unsigned short int fts_info;    /* user flags for FTSENT structure */

# define FTS_DONTCHDIR   0x01           /* don't chdir .. to the parent */
# define FTS_SYMFOLLOW   0x02           /* followed a symlink to get here */
        unsigned short int fts_flags;   /* private flags for FTSENT structure */

# define FTS_AGAIN       1              /* read node again */
# define FTS_FOLLOW      2              /* follow symbolic link */
# define FTS_NOINSTR     3              /* no instructions */
# define FTS_SKIP        4              /* discard node */
        unsigned short int fts_instr;   /* fts_set() instructions */

        struct stat fts_statp[1];       /* stat(2) information */
        char fts_name[__FLEXIBLE_ARRAY_MEMBER]; /* file name */
} FTSENT;


#endif
