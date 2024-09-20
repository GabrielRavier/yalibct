/* Basic tests for stat, lstat, fstat, and fstatat.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.
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

#include "test-deps/glibc.h"
#include <sys/sysmacros.h>

static void
stat_check (int fd, const char *path, struct stat *st)
{
    (void)fd;
  TEST_COMPARE (stat (path, st), 0);
}

static void
lstat_check (int fd, const char *path, struct stat *st)
{
    (void)fd;
  TEST_COMPARE (lstat (path, st), 0);
}

static void
fstat_check (int fd, const char *path, struct stat *st)
{
    (void)path;
  /* Test for invalid fstat input (BZ #27559).  */
  TEST_COMPARE (fstat (AT_FDCWD, st), -1);
  TEST_COMPARE (errno, EBADF);

  TEST_COMPARE (fstat (fd, st), 0);
}

static void
fstatat_check (int fd, const char *path, struct stat *st)
{
  TEST_COMPARE (fstatat (fd, "", st, 0), -1);
  TEST_COMPARE (errno, ENOENT);

  TEST_COMPARE (fstatat (fd, path, st, 0), 0);
}

typedef void (*test_t)(int, const char *path, struct stat *);

static int
do_test (void)
{
  char *path;
  int fd = create_temp_file ("tst-fstat.", &path);
  TEST_VERIFY_EXIT (fd >= 0);
  support_write_file_string (path, "abc");

  /* This should help to prevent delayed allocation, which may result
     in a spurious stx_blocks/st_blocks difference.  */
  assert(fsync (fd) == 0);

  bool check_ns = support_stat_nanoseconds (path);
  if (!check_ns)
    printf ("warning: timestamp with nanoseconds not supported\n");

#ifndef YALIBCT_LIBC_DOESNT_HAVE_STATX
  struct statx stx;
  TEST_COMPARE (statx (fd, path, 0, STATX_BASIC_STATS, &stx), 0);
#endif

  test_t tests[] = { stat_check, lstat_check, fstat_check, fstatat_check };

  for (int i = 0; i < array_length (tests); i++)
    {
      struct stat st;
      tests[i](fd, path, &st);

#ifndef YALIBCT_LIBC_DOESNT_HAVE_STATX
      TEST_COMPARE (stx.stx_dev_major, major (st.st_dev));
      TEST_COMPARE (stx.stx_dev_minor, minor (st.st_dev));
      TEST_COMPARE (stx.stx_ino, st.st_ino);
      TEST_COMPARE (stx.stx_mode, st.st_mode);
      TEST_COMPARE (stx.stx_nlink, st.st_nlink);
      TEST_COMPARE (stx.stx_uid, st.st_uid);
      TEST_COMPARE (stx.stx_gid, st.st_gid);
      TEST_COMPARE (stx.stx_rdev_major, major (st.st_rdev));
      TEST_COMPARE (stx.stx_rdev_minor, minor (st.st_rdev));
      TEST_COMPARE (stx.stx_blksize, st.st_blksize);
      TEST_COMPARE (stx.stx_blocks, st.st_blocks);

      TEST_COMPARE (stx.stx_ctime.tv_sec, st.st_ctim.tv_sec);
      TEST_COMPARE (stx.stx_mtime.tv_sec, st.st_mtim.tv_sec);
      if (check_ns)
	{
	  TEST_COMPARE (stx.stx_ctime.tv_nsec, st.st_ctim.tv_nsec);
	  TEST_COMPARE (stx.stx_mtime.tv_nsec, st.st_mtim.tv_nsec);
	}
#endif
    }

  return 0;
}

#include "test-deps/glibc/test-driver.h"
