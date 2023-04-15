// Derived from code with this license:
/* Copyright (C) 1999, 2001-2002, 2006, 2009-2023 Free Software Foundation,
   Inc.
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

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/__set_errno.h"
#include "test-lib/portable-symbols/internal/gnulib/ISSLASH.h"
#include "test-lib/portable-symbols/internal/gnulib/gen_tempname.h"
#include "test-lib/portable-symbols/secure_getenv.h"
#include "test-lib/portable-symbols/P_tmpnam.h"
#include "test-lib/portable-symbols/__glibc_unlikely.h"
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

# define struct_stat64 struct stat
# define __libc_secure_getenv secure_getenv
# define __xstat64(version, path, buf) stat (path, buf)

/* Return nonzero if DIR is an existent directory.  */
static bool
direxists (const char *dir)
{
  struct_stat64 buf;
  return __xstat64 (_STAT_VER, dir, &buf) == 0 && S_ISDIR (buf.st_mode);
}


/* Path search algorithm, for tmpnam, tmpfile, etc.  If DIR is
   non-null and exists, uses it; otherwise uses the first of $TMPDIR,
   P_tmpdir, /tmp that exists.  Copies into TMPL a template suitable
   for use with mk[s]temp.  Will fail (-1) if DIR is non-null and
   doesn't exist, none of the searched dirs exists, or there's not
   enough space in TMPL. */
int
path_search (char *tmpl, size_t tmpl_len, const char *dir, const char *pfx,
             bool try_tmpdir)
{
  const char *d;
  size_t dlen, plen;
  bool add_slash;

  if (!pfx || !pfx[0])
    {
      pfx = "file";
      plen = 4;
    }
  else
    {
      plen = strlen (pfx);
      if (plen > 5)
        plen = 5;
    }

  if (try_tmpdir)
    {
      d = __libc_secure_getenv ("TMPDIR");
      if (d != NULL && direxists (d))
        dir = d;
      else if (dir != NULL && direxists (dir))
        /* nothing */ ;
      else
        dir = NULL;
    }
  if (dir == NULL)
    {
#if defined _WIN32 && ! defined __CYGWIN__
      char dirbuf[PATH_MAX];
      DWORD retval;

      /* Find Windows temporary file directory.
         We try this before P_tmpdir because Windows defines P_tmpdir to "\\"
         and will therefore try to put all temporary files in the root
         directory (unless $TMPDIR is set).  */
      retval = GetTempPath (PATH_MAX, dirbuf);
      if (retval > 0 && retval < PATH_MAX && direxists (dirbuf))
        dir = dirbuf;
      else
#endif
      if (direxists (P_tmpdir))
        dir = P_tmpdir;
      else if (strcmp (P_tmpdir, "/tmp") != 0 && direxists ("/tmp"))
        dir = "/tmp";
      else
        {
          yalibct_internal_gnulib___set_errno (ENOENT);
          return -1;
        }
    }

  dlen = strlen (dir);
#ifdef __VMS
  add_slash = 0;
#else
  add_slash = dlen != 0 && !ISSLASH (dir[dlen - 1]);
#endif

  /* check we have room for "${dir}/${pfx}XXXXXX\0" */
  if (tmpl_len < dlen + add_slash + plen + 6 + 1)
    {
      yalibct_internal_gnulib___set_errno (EINVAL);
      return -1;
    }

  memcpy (tmpl, dir, dlen);
  sprintf (tmpl + dlen, &"/%.*sXXXXXX"[!add_slash], (int) plen, pfx);
  return 0;
}
