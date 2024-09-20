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

#ifndef YALIBCT_LIBC_DOESNT_HAVE_FTS_CLOSE
#include <fts.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/fts-common.h"
#include <unistd.h>

int
fts_close (FTS *sp)
{
        register FTSENT *freep, *p;
        int saved_errno = 0;

        /*
         * This still works if we haven't read anything -- the dummy structure
         * points to the root list, so we step through to the end of the root
         * list which has a valid parent pointer.
         */
        if (sp->fts_cur) {
                for (p = sp->fts_cur; p->fts_level >= FTS_ROOTLEVEL;) {
                        freep = p;
                        p = p->fts_link != NULL ? p->fts_link : p->fts_parent;
                        free(freep);
                }
                free(p);
        }

        /* Free up child linked list, sort array, file name buffer. */
        if (sp->fts_child)
                fts_lfree(sp->fts_child);
        free(sp->fts_array);
        free(sp->fts_path);

        if (ISSET(FTS_CWDFD))
          {
            if (0 <= sp->fts_cwd_fd)
              if (close (sp->fts_cwd_fd))
                saved_errno = errno;
          }
        else if (!ISSET(FTS_NOCHDIR))
          {
            /* Return to original directory, save errno if necessary. */
            if (fchdir(sp->fts_rfd))
              saved_errno = errno;

            /* If close fails, record errno only if saved_errno is zero,
               so that we report the probably-more-meaningful fchdir errno.  */
            if (close (sp->fts_rfd))
              if (saved_errno == 0)
                saved_errno = errno;
          }

        fd_ring_clear (&sp->fts_fd_ring);

        if (sp->fts_leaf_optimization_works_ht)
          hash_free (sp->fts_leaf_optimization_works_ht);

        free_dir (sp);

        /* Free up the stream pointer. */
        free(sp);

        /* Set errno and return. */
        if (saved_errno) {
                yalibct_internal_gnulib___set_errno (saved_errno);
                return (-1);
        }

        return (0);
}


#endif
