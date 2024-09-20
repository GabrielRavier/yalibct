/* Copyright (C) 2000-2022 Free Software Foundation, Inc.
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

/* We need to define:
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
*/

#include <assert.h>
#include <stddef.h>
#include <sys/stat.h>

static int
do_test (void)
{
#ifndef YALIBCT_LIBC_DOESNT_HAVE_STRUCT_STAT64
  /* With _FILE_OFFSET_BITS=64 struct stat and struct stat64 should
     be identical.  */
  static_assert (sizeof (struct stat)
	  == sizeof (struct stat64));
  static_assert (offsetof (struct stat, st_dev)
	  == offsetof (struct stat64, st_dev));
  static_assert (offsetof (struct stat, st_ino)
	  == offsetof (struct stat64, st_ino));
  static_assert (offsetof (struct stat, st_mode)
	  == offsetof (struct stat64, st_mode));
  static_assert (offsetof (struct stat, st_nlink)
	  == offsetof (struct stat64, st_nlink));
  static_assert (offsetof (struct stat, st_uid)
	  == offsetof (struct stat64, st_uid));
  static_assert (offsetof (struct stat, st_gid)
	  == offsetof (struct stat64, st_gid));
  static_assert (offsetof (struct stat, st_rdev)
	  == offsetof (struct stat64, st_rdev));
  static_assert (offsetof (struct stat, st_size)
	  == offsetof (struct stat64, st_size));
  static_assert (offsetof (struct stat, st_atime)
	  == offsetof (struct stat64, st_atime));
  static_assert (offsetof (struct stat, st_mtime)
	  == offsetof (struct stat64, st_mtime));
  static_assert (offsetof (struct stat, st_ctime)
	  == offsetof (struct stat64, st_ctime));
  static_assert (offsetof (struct stat, st_blksize)
	  == offsetof (struct stat64, st_blksize));
  static_assert (offsetof (struct stat, st_blocks)
	  == offsetof (struct stat64, st_blocks));
#if 0
  /* Some systems have st_fstype but not all.  Don't check it for now.  */
  static_assert (offsetof (struct stat, st_fstype)
	  == offsetof (struct stat64, st_fstype));
#endif
#endif
  return 0;
}

#define TEST_FUNCTION do_test ()
#include "test-deps/glibc/test-skeleton.h"
