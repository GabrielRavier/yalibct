// Derived from code with this license:
/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#pragma once

#include "test-lib/portable-symbols/internal/gnulib/try_tempname_len.h"
#include "test-lib/portable-symbols/internal/gnulib/GT_FILE.h"
#include "test-lib/portable-symbols/internal/gnulib/GT_DIR.h"
#include "test-lib/portable-symbols/internal/gnulib/GT_NOCREATE.h"
#include "test-lib/portable-symbols/internal/gnulib/_GL_UNUSED.h"
#include <fcntl.h>
#include <sys/stat.h>

# define __GT_FILE      GT_FILE
# define __GT_DIR       GT_DIR
# define __GT_NOCREATE GT_NOCREATE

# define __open open
# define __lstat64_time64(file, buf) lstat (file, buf)

static int
try_file (char *tmpl, void *flags)
{
  const int *openflags = flags;
  return __open (tmpl,
                 (*openflags & ~O_ACCMODE)
                 | O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
}

# define __mkdir mkdir

static int
try_dir (char *tmpl, _GL_UNUSED void *flags)
{
  return __mkdir (tmpl, S_IRUSR | S_IWUSR | S_IXUSR);
}

# define struct_stat64 struct stat

static int
try_nocreate (char *tmpl, _GL_UNUSED void *flags)
{
  struct_stat64 st;

  if (__lstat64_time64 (tmpl, &st) == 0 || errno == EOVERFLOW)
    yalibct_internal_gnulib___set_errno (EEXIST);
  return errno == ENOENT ? 0 : -1;
}


int
gen_tempname_len (char *tmpl, int suffixlen, int flags, int kind,
                  size_t x_suffix_len)
{
  static int (*const tryfunc[]) (char *, void *) =
    {
      [__GT_FILE] = try_file,
      [__GT_DIR] = try_dir,
      [__GT_NOCREATE] = try_nocreate
    };
  return try_tempname_len (tmpl, suffixlen, &flags, tryfunc[kind],
                           x_suffix_len);
}
