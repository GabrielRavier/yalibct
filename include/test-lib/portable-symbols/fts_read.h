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

#ifdef YALIBCT_LIBC_HAS_FTS_READ
#include <fts.h>
#else

#include "test-lib/portable-symbols/internal/gnulib/fts-common.h"
#include "test-lib/portable-symbols/internal/gnulib/SAME_INODE.h"
#include "test-lib/portable-symbols/internal/gnulib/hash_insert.h"
#include "test-lib/portable-symbols/internal/gnulib/hash_lookup.h"
#include "test-lib/portable-symbols/internal/gnulib/hash_remove.h"
#include "test-lib/portable-symbols/internal/gnulib/i_ring_push.h"
#include "test-lib/portable-symbols/internal/gnulib/cycle_check.h"
#include "test-lib/portable-symbols/internal/gnulib/CYCLE_CHECK_REFLECT_CHDIR_UP.h"
#include "test-lib/portable-symbols/internal/gnulib/FALLTHROUGH.h"
#include "test-lib/portable-symbols/internal/gnulib/_GL_CMP.h"
#include "test-lib/portable-symbols/internal/gnulib/opendirat.h"

#define Dprintf(x)

struct devino {
  intmax_t dev, ino;
};
#define PRINT_DEVINO "(%jd,%jd)"

static struct devino
getdevino (int fd)
{
  struct stat st;
  return (fd == AT_FDCWD
          ? (struct devino) { -1, 0 }
          : fstat (fd, &st) == 0
          ? (struct devino) { st.st_dev, st.st_ino }
          : (struct devino) { -1, errno });
}

static bool
same_fd (int fd1, int fd2)
{
  struct stat sb1, sb2;
  return (fstat (fd1, &sb1) == 0
          && fstat (fd2, &sb2) == 0
          && SAME_INODE (sb1, sb2));
}

/* Ensure that each file descriptor on the fd_ring matches a
   parent, grandparent, etc. of the current working directory.  */
static void
fd_ring_check (FTS const *sp)
{
    /*if (!fts_debug)
      return;*/

  /* Make a writable copy.  */
  I_ring fd_w = sp->fts_fd_ring;

  int cwd_fd = sp->fts_cwd_fd;
  cwd_fd = fcntl (cwd_fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
  struct devino dot = getdevino (cwd_fd);
  //fprintf (stderr, "===== check ===== cwd: "PRINT_DEVINO"\n",
  //         dot.dev, dot.ino);
  while ( ! i_ring_empty (&fd_w))
    {
      int fd = i_ring_pop (&fd_w);
      if (0 <= fd)
        {
          int open_flags = O_SEARCH | O_CLOEXEC;
          int parent_fd = openat (cwd_fd, "..", open_flags);
          if (parent_fd < 0)
            {
              // Warn?
              break;
            }
          if (!same_fd (fd, parent_fd))
            {
              struct devino cwd = getdevino (fd);
              fprintf (stderr, "ring  : "PRINT_DEVINO"\n", cwd.dev, cwd.ino);
              struct devino c2 = getdevino (parent_fd);
              fprintf (stderr, "parent: "PRINT_DEVINO"\n", c2.dev, c2.ino);
              fts_assert (0);
            }
          close (cwd_fd);
          cwd_fd = parent_fd;
        }
    }
  close (cwd_fd);
}

/*
 * Special case of "/" at the end of the file name so that slashes aren't
 * appended which would cause file names to be written as "....//foo".
 */
#define NAPPEND(p)                                                      \
        (p->fts_path[p->fts_pathlen - 1] == '/'                         \
            ? p->fts_pathlen - 1 : p->fts_pathlen)

#define LEAVE_DIR(Fts, Ent, Tag)                                \
  do                                                            \
    {                                                           \
      Dprintf (("  %s-leaving: %s\n", Tag, (Ent)->fts_path));   \
      leave_dir (Fts, Ent);                                     \
      fd_ring_check (Fts);                                      \
    }                                                           \
  while (false)

/* Leave a directory during a file tree walk.  */

static void
leave_dir (FTS *fts, FTSENT *ent)
{
  struct stat const *st = ent->fts_statp;
  if (fts->fts_options & (FTS_TIGHT_CYCLE_CHECK | FTS_LOGICAL))
    {
      struct Active_dir obj;
      void *found;
      obj.dev = st->st_dev;
      obj.ino = st->st_ino;
      found = hash_remove (fts->fts_cycle.ht, &obj);
      if (!found)
        abort ();
      free (found);
    }
  else
    {
      FTSENT *parent = ent->fts_parent;
      if (parent != NULL && 0 <= parent->fts_level)
        CYCLE_CHECK_REFLECT_CHDIR_UP (fts->fts_cycle.state,
                                      *(parent->fts_statp), *st);
    }
}

#define STREQ(a, b)     (strcmp (a, b) == 0)

# define fd_ring_print(a, b, c)

/* Virtual fchdir.  Advance SP's working directory file descriptor,
   SP->fts_cwd_fd, to FD, and push the previous value onto the fd_ring.
   CHDIR_DOWN_ONE is true if FD corresponds to an entry in the directory
   open on sp->fts_cwd_fd; i.e., to move the working directory one level
   down.  */
static void
internal_function
cwd_advance_fd (FTS *sp, int fd, bool chdir_down_one)
{
  int old = sp->fts_cwd_fd;
  fts_assert (old != fd || old == AT_FDCWD);

  if (chdir_down_one)
    {
      /* Push "old" onto the ring.
         If the displaced file descriptor is non-negative, close it.  */
      int prev_fd_in_slot = i_ring_push (&sp->fts_fd_ring, old);
      fd_ring_print (sp, stderr, "post-push");
      if (0 <= prev_fd_in_slot)
        close (prev_fd_in_slot); /* ignore any close failure */
    }
  else if ( ! ISSET (FTS_NOCHDIR))
    {
      if (0 <= old)
        close (old); /* ignore any close failure */
    }

  sp->fts_cwd_fd = fd;
}

/*
 * Change to dir specified by fd or file name without getting
 * tricked by someone changing the world out from underneath us.
 * Assumes p->fts_statp->st_dev and p->fts_statp->st_ino are filled in.
 * If FD is non-negative, expect it to be used after this function returns,
 * and to be closed eventually.  So don't pass e.g., 'dirfd(dirp)' and then
 * do closedir(dirp), because that would invalidate the saved FD.
 * Upon failure, close FD immediately and return nonzero.
 */
static int
internal_function
fts_safe_changedir (FTS *sp, FTSENT *p, int fd, char const *dir)
{
        int ret;
        bool is_dotdot = dir && STREQ (dir, "..");
        int newfd;

        /* This clause handles the unusual case in which FTS_NOCHDIR
           is specified, along with FTS_CWDFD.  In that case, there is
           no need to change even the virtual cwd file descriptor.
           However, if FD is non-negative, we do close it here.  */
        if (ISSET (FTS_NOCHDIR))
          {
            if (ISSET (FTS_CWDFD) && 0 <= fd)
              close (fd);
            return 0;
          }

        if (fd < 0 && is_dotdot && ISSET (FTS_CWDFD))
          {
            /* When possible, skip the diropen and subsequent fstat+dev/ino
               comparison.  I.e., when changing to parent directory
               (chdir ("..")), use a file descriptor from the ring and
               save the overhead of diropen+fstat, as well as avoiding
               failure when we lack "x" access to the virtual cwd.  */
            if ( ! i_ring_empty (&sp->fts_fd_ring))
              {
                int parent_fd;
                fd_ring_print (sp, stderr, "pre-pop");
                parent_fd = i_ring_pop (&sp->fts_fd_ring);
                if (0 <= parent_fd)
                  {
                    fd = parent_fd;
                    dir = NULL;
                  }
              }
          }

        if (fd >= 0)
            newfd = fd;
        else if ((newfd = diropen (sp, dir)) < 0)
          return -1;

        /* The following dev/inode check is necessary if we're doing a
           "logical" traversal (through symlinks, a la chown -L), if the
           system lacks O_NOFOLLOW support, or if we're changing to ".."
           (but not via a popped file descriptor).  When changing to the
           name "..", O_NOFOLLOW can't help.  In general, when the target is
           not "..", diropen's use of O_NOFOLLOW ensures we don't mistakenly
           follow a symlink, so we can avoid the expense of this fstat.  */
        if (ISSET(FTS_LOGICAL) //|| ! HAVE_WORKING_O_NOFOLLOW
            || (dir && STREQ (dir, "..")))
          {
            struct stat sb;
            if (fstat(newfd, &sb))
              {
                ret = -1;
                goto bail;
              }
            if (p->fts_statp->st_dev != sb.st_dev
                || p->fts_statp->st_ino != sb.st_ino)
              {
                yalibct_internal_gnulib___set_errno (ENOENT);           /* disinformation */
                ret = -1;
                goto bail;
              }
          }

        if (ISSET(FTS_CWDFD))
          {
            cwd_advance_fd (sp, newfd, ! is_dotdot);
            return 0;
          }

        ret = fchdir(newfd);
bail:
        if (fd < 0)
          {
            int oerrno = errno;
            (void)close(newfd);
            yalibct_internal_gnulib___set_errno (oerrno);
          }
        return ret;
}

#define closedir_and_clear(dirp)                \
  do                                            \
    {                                           \
      closedir (dirp);                          \
      dirp = NULL;                              \
    }                                           \
  while (0)

/* fts_build flags */
/* FIXME: make this an enum */
#define BCHILD          1               /* fts_children */
#define BNAMES          2               /* fts_children, names only */
#define BREAD           3               /* fts_read */

#define fts_opendir(file, Pdir_fd)                              \
        opendirat((! ISSET(FTS_NOCHDIR) && ISSET(FTS_CWDFD)     \
                   ? sp->fts_cwd_fd : AT_FDCWD),                \
                  file,                                         \
                  (((ISSET(FTS_PHYSICAL)                        \
                     && ! (ISSET(FTS_COMFOLLOW)                 \
                           && cur->fts_level == FTS_ROOTLEVEL)) \
                    ? O_NOFOLLOW : 0)),                         \
                  Pdir_fd)

/* If possible (see max_entries, below), read no more than this many directory
   entries at a time.  Without this limit (i.e., when using non-NULL
   fts_compar), processing a directory with 4,000,000 entries requires ~1GiB
   of memory, and handling 64M entries would require 16GiB of memory.  */
#ifndef FTS_MAX_READDIR_ENTRIES
# define FTS_MAX_READDIR_ENTRIES 100000
#endif

/* Enter a directory during a file tree walk.  */

static bool
enter_dir (FTS *fts, FTSENT *ent)
{
  if (fts->fts_options & (FTS_TIGHT_CYCLE_CHECK | FTS_LOGICAL))
    {
      struct stat const *st = ent->fts_statp;
      struct Active_dir *ad = malloc (sizeof *ad);
      struct Active_dir *ad_from_table;

      if (!ad)
        return false;

      ad->dev = st->st_dev;
      ad->ino = st->st_ino;
      ad->fts_ent = ent;

      /* See if we've already encountered this directory.
         This can happen when following symlinks as well as
         with a corrupted directory hierarchy. */
      ad_from_table = hash_insert (fts->fts_cycle.ht, ad);

      if (ad_from_table != ad)
        {
          free (ad);
          if (!ad_from_table)
            return false;

          /* There was an entry with matching dev/inode already in the table.
             Record the fact that we've found a cycle.  */
          ent->fts_cycle = ad_from_table->fts_ent;
          ent->fts_info = FTS_DC;
        }
    }
  else
    {
      if (cycle_check (fts->fts_cycle.state, ent->fts_statp))
        {
          /* FIXME: setting fts_cycle like this isn't proper.
             To do what the documentation requires, we'd have to
             go around the cycle again and find the right entry.
             But no callers in coreutils use the fts_cycle member. */
          ent->fts_cycle = ent;
          ent->fts_info = FTS_DC;
        }
    }

  return true;
}

/* Minimum link count of a traditional Unix directory.  When leaf
   optimization is OK and a directory's st_nlink == MIN_DIR_NLINK,
   then the directory has no subdirectories.  */
enum { MIN_DIR_NLINK = 2 };

/* Whether leaf optimization is OK for a directory.  */
enum leaf_optimization
  {
    /* st_nlink is not reliable for this directory's subdirectories.  */
    NO_LEAF_OPTIMIZATION,

    /* st_nlink == 2 means the directory lacks subdirectories.  */
    OK_LEAF_OPTIMIZATION
  };

#if (defined __linux__ || defined __ANDROID__) \
  && YALIBCT_LIBC_HAS_SYS_VFS_H && YALIBCT_LIBC_HAS_FSTATFS && YALIBCT_LIBC_HAS_STRUCT_STATFS_F_TYPE

# include <sys/vfs.h>

/* Linux-specific constants from coreutils' src/fs.h */
# define S_MAGIC_AFS 0x5346414F
# define S_MAGIC_CIFS 0xFF534D42
# define S_MAGIC_NFS 0x6969
# define S_MAGIC_PROC 0x9FA0
# define S_MAGIC_TMPFS 0x1021994

# ifdef YALIBCT_LIBC_HAS___FSWORD_T
typedef __fsword_t fsword;
# else
typedef long int fsword;
# endif

/* Map a stat.st_dev number to a file system type number f_ftype.  */
struct dev_type
{
  dev_t st_dev;
  fsword f_type;
};

/* Use a tiny initial size.  If a traversal encounters more than
   a few devices, the cost of growing/rehashing this table will be
   rendered negligible by the number of inodes processed.  */
enum { DEV_TYPE_HT_INITIAL_SIZE = 13 };

static size_t
dev_type_hash (void const *x, size_t table_size)
{
  struct dev_type const *ax = x;
  uintmax_t dev = ax->st_dev;
  return dev % table_size;
}

static bool
dev_type_compare (void const *x, void const *y)
{
  struct dev_type const *ax = x;
  struct dev_type const *ay = y;
  return ax->st_dev == ay->st_dev;
}

/* Return the file system type of P with file descriptor FD, or 0 if not known.
   If FD is negative, P's file descriptor is unavailable.
   Try to cache known values.  */

static fsword
filesystem_type (FTSENT const *p, int fd)
{
  FTS *sp = p->fts_fts;
  Hash_table *h = sp->fts_leaf_optimization_works_ht;
  struct dev_type *ent;
  struct statfs fs_buf;

  /* If we're not in CWDFD mode, don't bother with this optimization,
     since the caller is not serious about performance.  */
  if (!ISSET (FTS_CWDFD))
    return 0;

  if (! h)
    h = sp->fts_leaf_optimization_works_ht
      = hash_initialize (DEV_TYPE_HT_INITIAL_SIZE, NULL, dev_type_hash,
                         dev_type_compare, free);
  if (h)
    {
      struct dev_type tmp;
      tmp.st_dev = p->fts_statp->st_dev;
      ent = hash_lookup (h, &tmp);
      if (ent)
        return ent->f_type;
    }

  /* Look-up failed.  Query directly and cache the result.  */
  if (fd < 0 || fstatfs (fd, &fs_buf) != 0)
    return 0;

  if (h)
    {
      struct dev_type *t2 = malloc (sizeof *t2);
      if (t2)
        {
          t2->st_dev = p->fts_statp->st_dev;
          t2->f_type = fs_buf.f_type;

          ent = hash_insert (h, t2);
          if (ent)
            fts_assert (ent == t2);
          else
            free (t2);
        }
    }

  return fs_buf.f_type;
}

/* Return true if sorting dirents on inode numbers is known to improve
   traversal performance for the directory P with descriptor DIR_FD.
   Return false otherwise.  When in doubt, return true.
   DIR_FD is negative if unavailable.  */
static bool
dirent_inode_sort_may_be_useful (FTSENT const *p, int dir_fd)
{
  /* Skip the sort only if we can determine efficiently
     that skipping it is the right thing to do.
     The cost of performing an unnecessary sort is negligible,
     while the cost of *not* performing it can be O(N^2) with
     a very large constant.  */

  switch (filesystem_type (p, dir_fd))
    {
    case S_MAGIC_CIFS:
    case S_MAGIC_NFS:
    case S_MAGIC_TMPFS:
      /* On a file system of any of these types, sorting
         is unnecessary, and hence wasteful.  */
      return false;

    default:
      return true;
    }
}

/* Given an FTS entry P for a directory with descriptor DIR_FD,
   return whether it is valid to apply leaf optimization.
   The optimization is valid if a directory's st_nlink value equal
   to MIN_DIR_NLINK means the directory has no subdirectories.
   DIR_FD is negative if unavailable.  */
static enum leaf_optimization
leaf_optimization (FTSENT const *p, int dir_fd)
{
  switch (filesystem_type (p, dir_fd))
    {
    case 0:
      /* Leaf optimization is unsafe if the file system type is unknown.  */
      FALLTHROUGH;
    case S_MAGIC_AFS:
      /* Although AFS mount points are not counted in st_nlink, they
         act like directories.  See <https://bugs.debian.org/143111>.  */
      FALLTHROUGH;
    case S_MAGIC_CIFS:
      /* Leaf optimization causes 'find' to abort.  See
         <https://lists.gnu.org/r/bug-gnulib/2018-04/msg00015.html>.  */
      FALLTHROUGH;
    case S_MAGIC_NFS:
      /* NFS provides usable dirent.d_type but not necessarily for all entries
         of large directories, so as per <https://bugzilla.redhat.com/1252549>
         NFS should return true.  However st_nlink values are not accurate on
         all implementations as per <https://bugzilla.redhat.com/1299169>.  */
      FALLTHROUGH;
    case S_MAGIC_PROC:
      /* Per <https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=143111> /proc
         may have bogus stat.st_nlink values.  */
      return NO_LEAF_OPTIMIZATION;

    default:
      return OK_LEAF_OPTIMIZATION;
    }
}

#else
static bool
dirent_inode_sort_may_be_useful (_GL_UNUSED FTSENT const *p,
                                 _GL_UNUSED int dir_fd)
{
  return true;
}
static enum leaf_optimization
leaf_optimization (_GL_UNUSED FTSENT const *p, _GL_UNUSED int dir_fd)
{
  return NO_LEAF_OPTIMIZATION;
}
#endif

enum
{
  NOT_AN_INODE_NUMBER = 0
};

#if 1//def D_INO_IN_DIRENT
# define D_INO(dp) (dp)->d_ino
#else
/* Some systems don't have inodes, so fake them to avoid lots of ifdefs.  */
# define D_INO(dp) NOT_AN_INODE_NUMBER
#endif

#ifdef YALIBCT_LIBC_HAS_STRUCT_DIRENT_D_TYPE
/* True if the type of the directory entry D is known.  */
#define DT_IS_KNOWN(d) ((d)->d_type != DT_UNKNOWN)
/* True if the type of the directory entry D must be T.  */
#define DT_MUST_BE(d, t) ((d)->d_type == (t))
#define D_TYPE(d) ((d)->d_type)
#else
#define DT_IS_KNOWN(d) false
#define DT_MUST_BE(d, t) false
#define D_TYPE(d) DT_UNKNOWN
#endif

/* Map the dirent.d_type value, DTYPE, to the corresponding stat.st_mode
   S_IF* bit and set ST.st_mode, thus clearing all other bits in that field.  */
static void
set_stat_type (struct stat *st, unsigned int dtype)
{
  mode_t type;
  switch (dtype)
    {
    case DT_BLK:
      type = S_IFBLK;
      break;
    case DT_CHR:
      type = S_IFCHR;
      break;
    case DT_DIR:
      type = S_IFDIR;
      break;
    case DT_FIFO:
      type = S_IFIFO;
      break;
    case DT_LNK:
      type = S_IFLNK;
      break;
    case DT_REG:
      type = S_IFREG;
      break;
    case DT_SOCK:
      type = S_IFSOCK;
      break;
    default:
      type = 0;
    }
  st->st_mode = type;
}

/* If there are more than this many entries in a directory,
   and the conditions mentioned below are satisfied, then sort
   the entries on inode number before any further processing.  */
#ifndef FTS_INODE_SORT_DIR_ENTRIES_THRESHOLD
# define FTS_INODE_SORT_DIR_ENTRIES_THRESHOLD 10000
#endif

enum
{
  _FTS_INODE_SORT_DIR_ENTRIES_THRESHOLD = FTS_INODE_SORT_DIR_ENTRIES_THRESHOLD
};

/*
 * When the file name is realloc'd, have to fix all of the pointers in
 *  structures already returned.
 */
static void
internal_function
fts_padjust (FTS *sp, FTSENT *head)
{
        FTSENT *p;
        char *addr = sp->fts_path;

        /* This code looks at bit-patterns of freed pointers to
           relocate them, so it relies on undefined behavior.  If this
           trick does not work on your platform, please report a bug.  */

#define ADJUST(p) do {                                                  \
        uintptr_t old_accpath = (uintptr_t) (p)->fts_accpath;           \
        if (old_accpath != (uintptr_t) (p)->fts_name) {                 \
                (p)->fts_accpath =                                      \
                  addr + (old_accpath - (uintptr_t) (p)->fts_path);     \
        }                                                               \
        (p)->fts_path = addr;                                           \
} while (0)
        /* Adjust the current set of children. */
        for (p = sp->fts_child; p; p = p->fts_link)
                ADJUST(p);

        /* Adjust the rest of the tree, including the current level. */
        for (p = head; p->fts_level >= FTS_ROOTLEVEL;) {
                ADJUST(p);
                p = p->fts_link ? p->fts_link : p->fts_parent;
        }
}

/* FIXME: FTS_NOCHDIR is now misnamed.
   Call it FTS_USE_FULL_RELATIVE_FILE_NAMES instead. */
#define FCHDIR(sp, fd)                                  \
  (!ISSET(FTS_NOCHDIR) && (ISSET(FTS_CWDFD)             \
                           ? (cwd_advance_fd ((sp), (fd), true), 0) \
                           : fchdir (fd)))

/* Restore the initial, pre-traversal, "working directory".
   In FTS_CWDFD mode, we merely call cwd_advance_fd, otherwise,
   we may actually change the working directory.
   Return 0 upon success. Upon failure, set errno and return nonzero.  */
static int
restore_initial_cwd (FTS *sp)
{
  int fail = FCHDIR (sp, ISSET (FTS_CWDFD) ? AT_FDCWD : sp->fts_rfd);
  fd_ring_clear (&(sp->fts_fd_ring));
  return fail;
}

/* A comparison function to sort on increasing inode number.
   For some file system types, sorting either way makes a huge
   performance difference for a directory with very many entries,
   but sorting on increasing values is slightly better than sorting
   on decreasing values.  The difference is in the 5% range.  */
static int
fts_compare_ino (struct _ftsent const **a, struct _ftsent const **b)
{
  return _GL_CMP (a[0]->fts_statp->st_ino, b[0]->fts_statp->st_ino);
}

#ifndef _D_EXACT_NAMLEN
# define _D_EXACT_NAMLEN(dirent) strlen ((dirent)->d_name)
#endif

/*
 * This is the tricky part -- do not casually change *anything* in here.  The
 * idea is to build the linked list of entries that are used by fts_children
 * and fts_read.  There are lots of special cases.
 *
 * The real slowdown in walking the tree is the stat calls.  If FTS_NOSTAT is
 * set and it's a physical walk (so that symbolic links can't be directories),
 * we can do things quickly.  First, if it's a 4.4BSD file system, the type
 * of the file is in the directory entry.  Otherwise, we assume that the number
 * of subdirectories in a node is equal to the number of links to the parent.
 * The former skips all stat calls.  The latter skips stat calls in any leaf
 * directories and for any files after the subdirectories in the directory have
 * been found, cutting the stat calls by about 2/3.
 */
static FTSENT *
internal_function
fts_build (register FTS *sp, int type)
{
        register FTSENT *p, *head;
        register size_t nitems;
        FTSENT *tail;
        int saved_errno;
        bool descend;
        bool doadjust;
        ptrdiff_t level;
        size_t len, maxlen, new_len;
        char *cp;
        int dir_fd;
        FTSENT *cur = sp->fts_cur;
        bool continue_readdir = !!cur->fts_dirp;
        bool sort_by_inode = false;
        size_t max_entries;

        /* When cur->fts_dirp is non-NULL, that means we should
           continue calling readdir on that existing DIR* pointer
           rather than opening a new one.  */
        if (continue_readdir)
          {
            DIR *dp = cur->fts_dirp;
            dir_fd = dirfd (dp);
            if (dir_fd < 0)
              {
                int dirfd_errno = errno;
                closedir_and_clear (cur->fts_dirp);
                if (type == BREAD)
                  {
                    cur->fts_info = FTS_DNR;
                    cur->fts_errno = dirfd_errno;
                  }
                return NULL;
              }
          }
        else
          {
            /* Open the directory for reading.  If this fails, we're done.
               If being called from fts_read, set the fts_info field. */
            if ((cur->fts_dirp = fts_opendir(cur->fts_accpath, &dir_fd)) == NULL)
              {
                if (type == BREAD)
                  {
                    cur->fts_info = FTS_DNR;
                    cur->fts_errno = errno;
                  }
                return NULL;
              }
            /* Rather than calling fts_stat for each and every entry encountered
               in the readdir loop (below), stat each directory only right after
               opening it.  */
            bool stat_optimization = cur->fts_info == FTS_NSOK;

            if (stat_optimization
                /* Also read the stat info again after opening a directory to
                   reveal eventual changes caused by a submount triggered by
                   the traversal.  But do it only for utilities which use
                   FTS_TIGHT_CYCLE_CHECK.  Therefore, only find and du
                   benefit/suffer from this feature for now.  */
                || ISSET (FTS_TIGHT_CYCLE_CHECK))
              {
                if (!stat_optimization)
                  LEAVE_DIR (sp, cur, "4");
                if (fstat (dir_fd, cur->fts_statp) != 0)
                  {
                    int fstat_errno = errno;
                    closedir_and_clear (cur->fts_dirp);
                    if (type == BREAD)
                      {
                        cur->fts_errno = fstat_errno;
                        cur->fts_info = FTS_NS;
                      }
                    yalibct_internal_gnulib___set_errno (fstat_errno);
                    return NULL;
                  }
                if (stat_optimization)
                  cur->fts_info = FTS_D;
                else if (! enter_dir (sp, cur))
                  {
                    closedir_and_clear (cur->fts_dirp);
                    yalibct_internal_gnulib___set_errno (ENOMEM);
                    return NULL;
                  }
              }
          }

        /* Maximum number of readdir entries to read at one time.  This
           limitation is to avoid reading millions of entries into memory
           at once.  When an fts_compar function is specified, we have no
           choice: we must read all entries into memory before calling that
           function.  But when no such function is specified, we can read
           entries in batches that are large enough to help us with inode-
           sorting, yet not so large that we risk exhausting memory.  */
        max_entries = sp->fts_compar ? SIZE_MAX : FTS_MAX_READDIR_ENTRIES;

        /*
         * If we're going to need to stat anything or we want to descend
         * and stay in the directory, chdir.  If this fails we keep going,
         * but set a flag so we don't chdir after the post-order visit.
         * We won't be able to stat anything, but we can still return the
         * names themselves.  Note, that since fts_read won't be able to
         * chdir into the directory, it will have to return different file
         * names than before, i.e. "a/b" instead of "b".  Since the node
         * has already been visited in pre-order, have to wait until the
         * post-order visit to return the error.  There is a special case
         * here, if there was nothing to stat then it's not an error to
         * not be able to stat.  This is all fairly nasty.  If a program
         * needed sorted entries or stat information, they had better be
         * checking FTS_NS on the returned nodes.
         */
        if (continue_readdir)
          {
            /* When resuming a short readdir run, we already have
               the required dirp and dir_fd.  */
            descend = true;
          }
        else
          {
            /* Try to descend unless it is a names-only fts_children,
               or the directory is known to lack subdirectories.  */
            descend = (type != BNAMES
                       && ! (ISSET (FTS_NOSTAT) && ISSET (FTS_PHYSICAL)
                             && ! ISSET (FTS_SEEDOT)
                             && cur->fts_statp->st_nlink == MIN_DIR_NLINK
                             && (leaf_optimization (cur, dir_fd)
                                 != NO_LEAF_OPTIMIZATION)));
            if (descend || type == BREAD)
              {
                if (ISSET(FTS_CWDFD))
                  dir_fd = fcntl (dir_fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
                if (dir_fd < 0 || fts_safe_changedir(sp, cur, dir_fd, NULL)) {
                        if (descend && type == BREAD)
                                cur->fts_errno = errno;
                        cur->fts_flags |= FTS_DONTCHDIR;
                        descend = false;
                        closedir_and_clear(cur->fts_dirp);
                        if (ISSET(FTS_CWDFD) && 0 <= dir_fd)
                                close (dir_fd);
                        cur->fts_dirp = NULL;
                } else
                        descend = true;
              }
          }

        /*
         * Figure out the max file name length that can be stored in the
         * current buffer -- the inner loop allocates more space as necessary.
         * We really wouldn't have to do the maxlen calculations here, we
         * could do them in fts_read before returning the name, but it's a
         * lot easier here since the length is part of the dirent structure.
         *
         * If not changing directories set a pointer so that can just append
         * each new component into the file name.
         */
        len = NAPPEND(cur);
        if (ISSET(FTS_NOCHDIR)) {
                cp = sp->fts_path + len;
                *cp++ = '/';
        } else {
                /* GCC, you're too verbose. */
                cp = NULL;
        }
        len++;
        maxlen = sp->fts_pathlen - len;

        level = cur->fts_level + 1;

        /* Read the directory, attaching each entry to the "link" pointer. */
        doadjust = false;
        head = NULL;
        tail = NULL;
        nitems = 0;
        while (cur->fts_dirp) {
                size_t d_namelen;
                yalibct_internal_gnulib___set_errno (0);
                struct dirent *dp = readdir(cur->fts_dirp);
                if (dp == NULL) {
                        if (errno) {
                                cur->fts_errno = errno;
                                /* If we've not read any items yet, treat
                                   the error as if we can't access the dir.  */
                                cur->fts_info = (continue_readdir || nitems)
                                                ? FTS_ERR : FTS_DNR;
                        }
                        closedir_and_clear(cur->fts_dirp);
                        break;
                }
                if (!ISSET(FTS_SEEDOT) && ISDOT(dp->d_name))
                        continue;

                d_namelen = _D_EXACT_NAMLEN (dp);
                p = fts_alloc (sp, dp->d_name, d_namelen);
                if (!p)
                        goto mem1;
                if (d_namelen >= maxlen) {
                        /* include space for NUL */
                        uintptr_t oldaddr = (uintptr_t) sp->fts_path;
                        if (! fts_palloc(sp, d_namelen + len + 1)) {
                                /*
                                 * No more memory.  Save
                                 * errno, free up the current structure and the
                                 * structures already allocated.
                                 */
mem1:                           saved_errno = errno;
                                free(p);
                                fts_lfree(head);
                                closedir_and_clear(cur->fts_dirp);
                                cur->fts_info = FTS_ERR;
                                SET(FTS_STOP);
                                yalibct_internal_gnulib___set_errno (saved_errno);
                                return (NULL);
                        }
                        /* Did realloc() change the pointer? */
                        if (oldaddr != (uintptr_t) sp->fts_path) {
                                doadjust = true;
                                if (ISSET(FTS_NOCHDIR))
                                        cp = sp->fts_path + len;
                        }
                        maxlen = sp->fts_pathlen - len;
                }

                new_len = len + d_namelen;
                if (new_len < len) {
                        /*
                         * In the unlikely event that we would end up
                         * with a file name longer than SIZE_MAX, free up
                         * the current structure and the structures already
                         * allocated, then error out with ENAMETOOLONG.
                         */
                        free(p);
                        fts_lfree(head);
                        closedir_and_clear(cur->fts_dirp);
                        cur->fts_info = FTS_ERR;
                        SET(FTS_STOP);
                        yalibct_internal_gnulib___set_errno (ENAMETOOLONG);
                        return (NULL);
                }
                p->fts_level = level;
                p->fts_parent = sp->fts_cur;
                p->fts_pathlen = new_len;

                /* Store dirent.d_ino, in case we need to sort
                   entries before processing them.  */
                p->fts_statp->st_ino = D_INO (dp);

                /* Build a file name for fts_stat to stat. */
                if (ISSET(FTS_NOCHDIR)) {
                        p->fts_accpath = p->fts_path;
                        memmove(cp, p->fts_name, p->fts_namelen + 1);
                } else
                        p->fts_accpath = p->fts_name;

                if (sp->fts_compar == NULL || ISSET(FTS_DEFER_STAT)) {
                        /* Record what fts_read will have to do with this
                           entry. In many cases, it will simply fts_stat it,
                           but we can take advantage of any d_type information
                           to optimize away the unnecessary stat calls.  I.e.,
                           if FTS_NOSTAT is in effect and we're not following
                           symlinks (FTS_PHYSICAL) and d_type indicates this
                           is *not* a directory, then we won't have to stat it
                           at all.  If it *is* a directory, then (currently)
                           we stat it regardless, in order to get device and
                           inode numbers.  Some day we might optimize that
                           away, too, for directories where d_ino is known to
                           be valid.  */
                        bool skip_stat = (ISSET(FTS_NOSTAT)
                                          && DT_IS_KNOWN(dp)
                                          && ! DT_MUST_BE(dp, DT_DIR)
                                          && (ISSET(FTS_PHYSICAL)
                                              || ! DT_MUST_BE(dp, DT_LNK)));
                        p->fts_info = FTS_NSOK;
                        /* Propagate dirent.d_type information back
                           to caller, when possible.  */
                        set_stat_type (p->fts_statp, D_TYPE (dp));
                        fts_set_stat_required(p, !skip_stat);
                } else {
                        p->fts_info = fts_stat(sp, p, false);
                }

                /* We walk in directory order so "ls -f" doesn't get upset. */
                p->fts_link = NULL;
                if (head == NULL)
                        head = tail = p;
                else {
                        tail->fts_link = p;
                        tail = p;
                }

                /* If there are many entries, no sorting function has been
                   specified, and this file system is of a type that may be
                   slow with a large number of entries, arrange to sort the
                   directory entries on increasing inode numbers.

                   The NITEMS comparison uses ==, not >, because the test
                   needs to be tried at most once once, and NITEMS will exceed
                   the threshold after it is incremented below.  */
                if (nitems == _FTS_INODE_SORT_DIR_ENTRIES_THRESHOLD
                    && !sp->fts_compar)
                  sort_by_inode = dirent_inode_sort_may_be_useful (cur, dir_fd);

                ++nitems;
                if (max_entries <= nitems) {
                        /* When there are too many dir entries, leave
                           fts_dirp open, so that a subsequent fts_read
                           can take up where we leave off.  */
                        break;
                }
        }

        /*
         * If realloc() changed the address of the file name, adjust the
         * addresses for the rest of the tree and the dir list.
         */
        if (doadjust)
                fts_padjust(sp, head);

        /*
         * If not changing directories, reset the file name back to original
         * state.
         */
        if (ISSET(FTS_NOCHDIR)) {
                if (len == sp->fts_pathlen || nitems == 0)
                        --cp;
                *cp = '\0';
        }

        /*
         * If descended after called from fts_children or after called from
         * fts_read and nothing found, get back.  At the root level we use
         * the saved fd; if one of fts_open()'s arguments is a relative name
         * to an empty directory, we wind up here with no other way back.  If
         * can't get back, we're done.
         */
        if (!continue_readdir && descend && (type == BCHILD || !nitems) &&
            (cur->fts_level == FTS_ROOTLEVEL
             ? restore_initial_cwd(sp)
             : fts_safe_changedir(sp, cur->fts_parent, -1, ".."))) {
                cur->fts_info = FTS_ERR;
                SET(FTS_STOP);
                fts_lfree(head);
                return (NULL);
        }

        /* If didn't find anything, return NULL. */
        if (!nitems) {
                if (type == BREAD
                    && cur->fts_info != FTS_DNR && cur->fts_info != FTS_ERR)
                        cur->fts_info = FTS_DP;
                fts_lfree(head);
                return (NULL);
        }

        if (sort_by_inode) {
                sp->fts_compar = fts_compare_ino;
                head = fts_sort (sp, head, nitems);
                sp->fts_compar = NULL;
        }

        /* Sort the entries. */
        if (sp->fts_compar && nitems > 1)
                head = fts_sort(sp, head, nitems);
        return (head);
}

static void
internal_function
fts_load (FTS *sp, register FTSENT *p)
{
        register size_t len;
        register char *cp;

        /*
         * Load the stream structure for the next traversal.  Since we don't
         * actually enter the directory until after the preorder visit, set
         * the fts_accpath field specially so the chdir gets done to the right
         * place and the user can access the first node.  From fts_open it's
         * known that the file name will fit.
         */
        len = p->fts_pathlen = p->fts_namelen;
        memmove(sp->fts_path, p->fts_name, len + 1);
        if ((cp = strrchr(p->fts_name, '/')) && (cp != p->fts_name || cp[1])) {
                len = strlen(++cp);
                memmove(p->fts_name, cp, len + 1);
                p->fts_namelen = len;
        }
        p->fts_accpath = p->fts_path = sp->fts_path;
}

FTSENT *
fts_read (register FTS *sp)
{
        register FTSENT *p, *tmp;
        register unsigned short int instr;
        register char *t;

        /* If finished or unrecoverable error, return NULL. */
        if (sp->fts_cur == NULL || ISSET(FTS_STOP))
                return (NULL);

        /* Set current node pointer. */
        p = sp->fts_cur;

        /* Save and zero out user instructions. */
        instr = p->fts_instr;
        p->fts_instr = FTS_NOINSTR;

        /* Any type of file may be re-visited; re-stat and re-turn. */
        if (instr == FTS_AGAIN) {
                p->fts_info = fts_stat(sp, p, false);
                return (p);
        }
        Dprintf (("fts_read: p=%s\n",
                  p->fts_info == FTS_INIT ? "" : p->fts_path));

        /*
         * Following a symlink -- SLNONE test allows application to see
         * SLNONE and recover.  If indirecting through a symlink, have
         * keep a pointer to current location.  If unable to get that
         * pointer, follow fails.
         */
        if (instr == FTS_FOLLOW &&
            (p->fts_info == FTS_SL || p->fts_info == FTS_SLNONE)) {
                p->fts_info = fts_stat(sp, p, true);
                if (p->fts_info == FTS_D && !ISSET(FTS_NOCHDIR)) {
                        if ((p->fts_symfd = diropen (sp, ".")) < 0) {
                                p->fts_errno = errno;
                                p->fts_info = FTS_ERR;
                        } else
                                p->fts_flags |= FTS_SYMFOLLOW;
                }
                goto check_for_dir;
        }

        /* Directory in pre-order. */
        if (p->fts_info == FTS_D) {
                /* If skipped or crossed mount point, do post-order visit. */
                if (instr == FTS_SKIP ||
                    (ISSET(FTS_XDEV) && p->fts_statp->st_dev != sp->fts_dev)) {
                        if (p->fts_flags & FTS_SYMFOLLOW)
                                (void)close(p->fts_symfd);
                        if (sp->fts_child) {
                                fts_lfree(sp->fts_child);
                                sp->fts_child = NULL;
                        }
                        p->fts_info = FTS_DP;
                        LEAVE_DIR (sp, p, "1");
                        return (p);
                }

                /* Rebuild if only read the names and now traversing. */
                if (sp->fts_child != NULL && ISSET(FTS_NAMEONLY)) {
                        CLR(FTS_NAMEONLY);
                        fts_lfree(sp->fts_child);
                        sp->fts_child = NULL;
                }

                /*
                 * Cd to the subdirectory.
                 *
                 * If have already read and now fail to chdir, whack the list
                 * to make the names come out right, and set the parent errno
                 * so the application will eventually get an error condition.
                 * Set the FTS_DONTCHDIR flag so that when we logically change
                 * directories back to the parent we don't do a chdir.
                 *
                 * If haven't read do so.  If the read fails, fts_build sets
                 * FTS_STOP or the fts_info field of the node.
                 */
                if (sp->fts_child != NULL) {
                        if (fts_safe_changedir(sp, p, -1, p->fts_accpath)) {
                                p->fts_errno = errno;
                                p->fts_flags |= FTS_DONTCHDIR;
                                for (p = sp->fts_child; p != NULL;
                                     p = p->fts_link)
                                        p->fts_accpath =
                                            p->fts_parent->fts_accpath;
                        }
                } else if ((sp->fts_child = fts_build(sp, BREAD)) == NULL) {
                        if (ISSET(FTS_STOP))
                                return (NULL);
                        /* If fts_build's call to fts_safe_changedir failed
                           because it was not able to fchdir into a
                           subdirectory, tell the caller.  */
                        if (p->fts_errno && p->fts_info != FTS_DNR)
                                p->fts_info = FTS_ERR;
                        LEAVE_DIR (sp, p, "2");
                        return (p);
                }
                p = sp->fts_child;
                sp->fts_child = NULL;
                goto name;
        }

        /* Move to the next node on this level. */
next:   tmp = p;

        /* If we have so many directory entries that we're reading them
           in batches, and we've reached the end of the current batch,
           read in a new batch.  */
        if (p->fts_link == NULL && p->fts_parent->fts_dirp)
          {
            p = tmp->fts_parent;
            sp->fts_cur = p;
            sp->fts_path[p->fts_pathlen] = '\0';

            if ((p = fts_build (sp, BREAD)) == NULL)
              {
                if (ISSET(FTS_STOP))
                  return NULL;
                goto cd_dot_dot;
              }

            free(tmp);
            goto name;
          }

        if ((p = p->fts_link) != NULL) {
                sp->fts_cur = p;
                free(tmp);

                /*
                 * If reached the top, return to the original directory (or
                 * the root of the tree), and load the file names for the next
                 * root.
                 */
                if (p->fts_level == FTS_ROOTLEVEL) {
                        if (restore_initial_cwd(sp)) {
                                SET(FTS_STOP);
                                return (NULL);
                        }
                        free_dir(sp);
                        fts_load(sp, p);
                        setup_dir(sp);
                        goto check_for_dir;
                }

                /*
                 * User may have called fts_set on the node.  If skipped,
                 * ignore.  If followed, get a file descriptor so we can
                 * get back if necessary.
                 */
                if (p->fts_instr == FTS_SKIP)
                        goto next;
                if (p->fts_instr == FTS_FOLLOW) {
                        p->fts_info = fts_stat(sp, p, true);
                        if (p->fts_info == FTS_D && !ISSET(FTS_NOCHDIR)) {
                                if ((p->fts_symfd = diropen (sp, ".")) < 0) {
                                        p->fts_errno = errno;
                                        p->fts_info = FTS_ERR;
                                } else
                                        p->fts_flags |= FTS_SYMFOLLOW;
                        }
                        p->fts_instr = FTS_NOINSTR;
                }

name:           t = sp->fts_path + NAPPEND(p->fts_parent);
                *t++ = '/';
                memmove(t, p->fts_name, p->fts_namelen + 1);
check_for_dir:
                sp->fts_cur = p;
                if (p->fts_info == FTS_NSOK)
                  {
                    if (p->fts_statp->st_size == FTS_STAT_REQUIRED)
                      p->fts_info = fts_stat(sp, p, false);
                    else
                      fts_assert (p->fts_statp->st_size == FTS_NO_STAT_REQUIRED);
                  }

                if (p->fts_info == FTS_D)
                  {
                    /* Now that P->fts_statp is guaranteed to be valid,
                       if this is a command-line directory, record its
                       device number, to be used for FTS_XDEV.  */
                    if (p->fts_level == FTS_ROOTLEVEL)
                      sp->fts_dev = p->fts_statp->st_dev;
                    Dprintf (("  entering: %s\n", p->fts_path));
                    if (! enter_dir (sp, p))
                      {
                        yalibct_internal_gnulib___set_errno (ENOMEM);
                        return NULL;
                      }
                  }
                return p;
        }
cd_dot_dot:

        /* Move up to the parent node. */
        p = tmp->fts_parent;
        sp->fts_cur = p;
        free(tmp);

        if (p->fts_level == FTS_ROOTPARENTLEVEL) {
                /*
                 * Done; free everything up and set errno to 0 so the user
                 * can distinguish between error and EOF.
v                 */
                free(p);
                yalibct_internal_gnulib___set_errno (0);
                return (sp->fts_cur = NULL);
        }

        fts_assert (p->fts_info != FTS_NSOK);

        /* NUL terminate the file name.  */
        sp->fts_path[p->fts_pathlen] = '\0';

        /*
         * Return to the parent directory.  If at a root node, restore
         * the initial working directory.  If we came through a symlink,
         * go back through the file descriptor.  Otherwise, move up
         * one level, via "..".
         */
        if (p->fts_level == FTS_ROOTLEVEL) {
                if (restore_initial_cwd(sp)) {
                        p->fts_errno = errno;
                        SET(FTS_STOP);
                }
        } else if (p->fts_flags & FTS_SYMFOLLOW) {
                if (FCHDIR(sp, p->fts_symfd)) {
                        p->fts_errno = errno;
                        SET(FTS_STOP);
                }
                (void)close(p->fts_symfd);
        } else if (!(p->fts_flags & FTS_DONTCHDIR) &&
                   fts_safe_changedir(sp, p->fts_parent, -1, "..")) {
                p->fts_errno = errno;
                SET(FTS_STOP);
        }

        /* If the directory causes a cycle, preserve the FTS_DC flag and keep
           the corresponding dev/ino pair in the hash table.  It is going to be
           removed when leaving the original directory.  */
        if (p->fts_info != FTS_DC) {
                p->fts_info = p->fts_errno ? FTS_ERR : FTS_DP;
                if (p->fts_errno == 0)
                        LEAVE_DIR (sp, p, "3");
        }
        return ISSET(FTS_STOP) ? NULL : p;
}

#endif