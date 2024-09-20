// Copyright 2018 The gVisor Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test-deps/googletest.h"
#include "test-deps/gvisor.h"
#include "test-deps/abseil.h"
#include <sys/socket.h>
#include <sys/syscall.h>
#include <limits.h>

#ifndef AT_STATX_FORCE_SYNC
#define AT_STATX_FORCE_SYNC 0x2000
#endif
#ifndef AT_STATX_DONT_SYNC
#define AT_STATX_DONT_SYNC 0x4000
#endif

/*namespace gvisor {
namespace testing {

namespace {

class StatTest : public FileTest {};*/

#define StatTest FileTest

TEST_F(StatTest, FstatatAbs) {
  struct stat st;

  // Check that the stat works.
  assert(fstatat(AT_FDCWD, self->test_file_name_, &st, 0) == 0);
  EXPECT_TRUE(S_ISREG(st.st_mode));
}

TEST_F(StatTest, FstatatEmptyPath) {
  struct stat st;
  const int fd = open(self->test_file_name_, O_RDONLY);
  assert(fd >= 0);

#if 0//ndef YALIBCT_LIBC_DOESNT_HAVE_FSTATAT
  // Check that the stat works.
  assert(fstatat(fd, "", &st, AT_EMPTY_PATH) == 0);
  EXPECT_TRUE(S_ISREG(st.st_mode));
#endif
  assert(close(fd) == 0);
}

TEST_F(StatTest, FstatatRel) {
  struct stat st;
  int dirfd;
  char *filename = Basename(self->test_file_name_);

  // Open the temporary directory read-only.
  dirfd = open(GetAbsoluteTestTmpdir(), O_RDONLY);
  assert(dirfd >= 0);

  // Check that the stat works.
  assert(fstatat(dirfd, filename, &st, 0) == 0);
  EXPECT_TRUE(S_ISREG(st.st_mode));
  free(filename);
  assert(close(dirfd) == 0);
}

TEST_F(StatTest, FstatatSymlink) {
    (void)self;

  struct stat st;

  // Check that the link is followed.
  assert(fstatat(AT_FDCWD, "/proc/self", &st, 0) == 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  // Check that the flag works.
  assert(fstatat(AT_FDCWD, "/proc/self", &st, AT_SYMLINK_NOFOLLOW) == 0);
  EXPECT_TRUE(S_ISLNK(st.st_mode));
  EXPECT_FALSE(S_ISDIR(st.st_mode));
}

TEST_F(StatTest, Nlinks) {
    (void)self;

  // Skip this test if we are testing overlayfs because overlayfs does not
  // (intentionally) return the correct nlink value for directories.
  // See fs/overlayfs/inode.c:ovl_getattr().
  //SKIP_IF(ASSERT_NO_ERRNO_AND_VALUE(IsOverlayfs(GetAbsoluteTestTmpdir())));

  char *basedir = TempPath_CreateDir();

  // Directory is initially empty, it should contain 2 links (one from itself,
  // one from ".").
  assert(Links(basedir) == 2);

  // Create a file in the test directory. Files shouldn't increase the link
  // count on the base directory.
  char *file1 =
      TempPath_CreateFileIn(basedir);
  assert(Links(basedir) == 2);

  // Create subdirectories. This should increase the link count by 1 per
  // subdirectory.
  char *dir1 =
      TempPath_CreateDirIn(basedir);
  assert(Links(basedir) == 3);
  char *dir2 =
      TempPath_CreateDirIn(basedir);
  assert(Links(basedir) == 4);

  // Removing directories should reduce the link count.
  TempPath_RemoveAndFreePath(dir1);
  assert(Links(basedir) == 3);
  TempPath_RemoveAndFreePath(dir2);
  assert(Links(basedir) == 2);

  // Removing files should have no effect on link count.
  TempPath_RemoveAndFreePath(file1);
  assert(Links(basedir) == 2);

  TempPath_RemoveAndFreePath(basedir);
}

TEST_F(StatTest, BlocksIncreaseOnWrite) {
  struct stat st;

  // Stat the empty file.
  assert(fstat(self->test_file_fd_, &st) == 0);

  const int initial_blocks = st.st_blocks;

  // Write to the file, making sure to exceed the block size.
  char *buf = malloc(2 * st.st_blksize);
  assert(buf != NULL);
  memset(buf, 'a', 2 * st.st_blksize);
  assert(write(self->test_file_fd_, buf, 2 * st.st_blksize) == 2 * st.st_blksize);
  free(buf);

  // Stat the file again, and verify that number of allocated blocks has
  // increased.
  assert(fstat(self->test_file_fd_, &st) == 0);
  EXPECT_GT(st.st_blocks, initial_blocks);
}

TEST_F(StatTest, PathNotCleaned) {
    (void)self;

    char *basedir = TempPath_CreateDir();

  // Create a file in the basedir.
  char *file =
      TempPath_CreateFileIn(basedir);

  // Stating the file directly should succeed.
  struct stat buf;
  assert(lstat(file, &buf) == 0);

  // Try to stat the file using a directory that does not exist followed by
  // "..".  If the path is cleaned prior to stating (which it should not be)
  // then this will succeed.
  char *bad_path = JoinPath("/does_not_exist/..", file);
  assert(lstat(bad_path, &buf) == -1 && errno == ENOENT);
  free(bad_path);

  TempPath_RemoveAndFreePath(file);
  TempPath_RemoveAndFreePath(basedir);
}

TEST_F(StatTest, PathCanContainDotDot) {
    (void)self;
    char *basedir = TempPath_CreateDir();
  char *subdir =
      TempPath_CreateDirIn(basedir);
  char *subdir_name = Basename(subdir);

  // Create a file in the subdir.
  char *file =
      TempPath_CreateFileIn(subdir);
  char *file_name = Basename(file);


  // Stat the file through a path that includes '..' and '.' but still resolves
  // to the file.
  char *tmp_path1 = JoinPath(basedir, subdir_name);
  char *tmp_path2 = JoinPath(tmp_path1, "..");
  free(tmp_path1);
  tmp_path1 = JoinPath(tmp_path2, subdir_name); // NOLINT(readability-suspicious-call-argument)
  free(tmp_path2);
  tmp_path2 = JoinPath(tmp_path1, ".");
  free(tmp_path1);
  char *good_path = JoinPath(tmp_path2, file_name); // NOLINT(readability-suspicious-call-argument)
  free(tmp_path2);

  struct stat buf;
  assert(lstat(good_path, &buf) == 0);
  free(good_path);

  free(file_name);
  TempPath_RemoveAndFreePath(file);

  free(subdir_name);
  TempPath_RemoveAndFreePath(subdir);
  TempPath_RemoveAndFreePath(basedir);
}

TEST_F(StatTest, PathCanContainEmptyComponent) {
    (void)self;

    char *basedir = TempPath_CreateDir();

  // Create a file in the basedir.
  char *file =
      TempPath_CreateFileIn(basedir);
  char *file_name = Basename(file);

  // Stat the file through a path that includes an empty component.  We have to
  // build this ourselves because JoinPath automatically removes empty
  // components.
  char *good_path;
  //= absl::StrCat(basedir.path(), "//", file_name);
  assert(asprintf(&good_path, "%s//%s", basedir, file_name) >= 0);
  struct stat buf;
  assert(lstat(good_path, &buf) == 0);

  free(good_path);

  free(file_name);
  TempPath_RemoveAndFreePath(file);

  TempPath_RemoveAndFreePath(basedir);
}

TEST_F(StatTest, TrailingSlashNotCleanedReturnsENOTDIR) {
    (void)self;

    char *basedir = TempPath_CreateDir();

  // Create a file in the basedir.
  char *file =
      TempPath_CreateFileIn(basedir);

  // Stat the file with an extra "/" on the end of it.  Since file is not a
  // directory, this should return ENOTDIR.
  char *bad_path;// = absl::StrCat(file.path(), "/");
  assert(asprintf(&bad_path, "%s/", file) >= 0);
  struct stat buf;
  assert(lstat(bad_path, &buf) == -1 && errno == ENOTDIR);

  free(bad_path);

  TempPath_RemoveAndFreePath(file);

  TempPath_RemoveAndFreePath(basedir);
}

TEST_F(StatTest, FstatFileWithOpath) {
    (void)self;

  struct stat st;
  char *tmpfile = TempPath_CreateFile();
  int fd =
      open(tmpfile, O_PATH);
  assert(fd >= 0);

  // Stat the directory.
  assert(fstat(fd, &st) == 0);

  assert(close(fd) == 0);
  TempPath_RemoveAndFreePath(tmpfile);
}

TEST_F(StatTest, FstatDirWithOpath) {
    (void)self;

  struct stat st;
  char *tmpdir = TempPath_CreateDir();
  int dirfd =
      open(tmpdir, O_PATH | O_DIRECTORY);
  assert(dirfd >= 0);

  // Stat the directory.
  assert(fstat(dirfd, &st) == 0);

  assert(close(dirfd) == 0);
  TempPath_RemoveAndFreePath(tmpdir);
}

// fstatat with an O_PATH fd
TEST_F(StatTest, FstatatDirWithOpath) {
    (void)self;

    char *tmpdir = TempPath_CreateDir();
  int dirfd =
      open(tmpdir, O_PATH | O_DIRECTORY);
  assert(dirfd >= 0);
  char *tmpfile = TempPath_CreateFile();

  struct stat st = {};
  assert(fstatat(dirfd, tmpfile, &st, 0) == 0);
  EXPECT_FALSE(S_ISDIR(st.st_mode));
  EXPECT_TRUE(S_ISREG(st.st_mode));

  TempPath_RemoveAndFreePath(tmpfile);
  assert(close(dirfd) == 0);
  TempPath_RemoveAndFreePath(tmpdir);
}

// Test fstatating a symlink directory.
TEST_F(StatTest, FstatatSymlinkDir) {
    (void)self;

  // Create a directory and symlink to it.
    char *dir = TempPath_CreateDir();

  char *symlink_to_dir = NewTempAbsPath();
  assert(symlink(dir, symlink_to_dir) == 0);

  // Fstatat the link with AT_SYMLINK_NOFOLLOW should return symlink data.
  struct stat st = {};
  assert(fstatat(AT_FDCWD, symlink_to_dir, &st, AT_SYMLINK_NOFOLLOW) == 0);
  EXPECT_FALSE(S_ISDIR(st.st_mode));
  EXPECT_TRUE(S_ISLNK(st.st_mode));

  // Fstatat the link should return dir data.
  assert(fstatat(AT_FDCWD, symlink_to_dir, &st, 0) == 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  assert(unlink(symlink_to_dir) == 0);
  free(symlink_to_dir);

  TempPath_RemoveAndFreePath(dir);
}

// Test fstatating a symlink directory with trailing slash.
TEST_F(StatTest, FstatatSymlinkDirWithTrailingSlash) {
    (void)self;

  // Create a directory and symlink to it.
  char *dir = TempPath_CreateDir();
  char *symlink_to_dir = NewTempAbsPath();
  assert(symlink(dir, symlink_to_dir) == 0);

  // Fstatat on the symlink with a trailing slash should return the directory
  // data.
  struct stat st = {};
  char *symlink_to_dir_with_trailing_slash;
  assert(asprintf(&symlink_to_dir_with_trailing_slash, "%s/", symlink_to_dir) >= 0);
  assert(
      fstatat(AT_FDCWD, symlink_to_dir_with_trailing_slash, &st, 0) == 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  // Fstatat on the symlink with a trailing slash with AT_SYMLINK_NOFOLLOW
  // should return the directory data.
  // Symlink to directory with trailing slash will ignore AT_SYMLINK_NOFOLLOW.
  assert(fstatat(AT_FDCWD, symlink_to_dir_with_trailing_slash, &st,
                 AT_SYMLINK_NOFOLLOW) == 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  free(symlink_to_dir_with_trailing_slash);

  assert(unlink(symlink_to_dir) == 0);
  free(symlink_to_dir);
  TempPath_RemoveAndFreePath(dir);
}

// Test fstatating a symlink directory with a trailing slash
// should return same stat data with fstatating directory.
TEST_F(StatTest, FstatatSymlinkDirWithTrailingSlashSameInode) {
    (void)self;

  // Create a directory and symlink to it.
  char *dir = TempPath_CreateDir();

  // We are going to assert that the symlink inode id is the same as the linked
  // dir's inode id. In order for the inode id to be stable across
  // save/restore, it must be kept open. The FileDescriptor type will do that
  // for us automatically.
  int fd = open(dir, O_RDONLY | O_DIRECTORY);
  assert(fd >= 0);

  char *symlink_to_dir = NewTempAbsPath();
  assert(symlink(dir, symlink_to_dir) == 0);

  // Fstatat on the symlink with a trailing slash should return the directory
  // data.
  struct stat st = {};
  char *symlink_to_dir_with_trailing_slash;
  assert(asprintf(&symlink_to_dir_with_trailing_slash, "%s/", symlink_to_dir) >= 0);
  assert(fstatat(AT_FDCWD, symlink_to_dir_with_trailing_slash, &st,
                 AT_SYMLINK_NOFOLLOW) == 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));

  free(symlink_to_dir_with_trailing_slash);

  // Dir and symlink should point to same inode.
  struct stat st_dir = {};
  assert(
      fstatat(AT_FDCWD, dir, &st_dir, AT_SYMLINK_NOFOLLOW) == 0);
  EXPECT_EQ(st.st_ino, st_dir.st_ino);

  assert(unlink(symlink_to_dir) == 0);
  free(symlink_to_dir);
  assert(close(fd) == 0);

  TempPath_RemoveAndFreePath(dir);
}

TEST_F(StatTest, LeadingDoubleSlash) {
    (void)self;

  // Create a file, and make sure we can stat it.
  char *file = TempPath_CreateFile();
  struct stat st;
  assert(lstat(file, &st) == 0);

  // Now add an extra leading slash.
  char *double_slash_path = absl_StrCat("/", file);
  ASSERT_TRUE(absl_StartsWith(double_slash_path, "//"));

  // We should be able to stat the new path, and it should resolve to the same
  // file (same device and inode).
  struct stat double_slash_st;
  assert(lstat(double_slash_path, &double_slash_st) == 0);
  EXPECT_EQ(st.st_dev, double_slash_st.st_dev);
  // Inode numbers for gofer-accessed files may change across save/restore.
  //if (!IsRunningWithSaveRestore()) {
    EXPECT_EQ(st.st_ino, double_slash_st.st_ino);
  //}

  free(double_slash_path);
  TempPath_RemoveAndFreePath(file);
}

// Test that a rename doesn't change the underlying file.
TEST_F(StatTest, StatDoesntChangeAfterRename) {
    (void)self;

    char *old_file = TempPath_CreateFile();
    char *new_path = NewTempAbsPath();

  struct stat st_old = {};
  struct stat st_new = {};

  assert(stat(old_file, &st_old) == 0);
  assert(rename(old_file, new_path) == 0);
  assert(stat(new_path, &st_new) == 0);

  EXPECT_EQ(st_old.st_nlink, st_new.st_nlink);
  EXPECT_EQ(st_old.st_dev, st_new.st_dev);
  // Inode numbers for gofer-accessed files on which no reference is held may
  // change across save/restore because the information that the gofer client
  // uses to track file identity (9P QID path) is inconsistent between gofer
  // processes, which are restarted across save/restore.
  //
  // Overlay filesystems may synthesize directory inode numbers on the fly.
  //if (!IsRunningWithSaveRestore() &&
  //    !ASSERT_NO_ERRNO_AND_VALUE(IsOverlayfs(GetAbsoluteTestTmpdir()))) {
    EXPECT_EQ(st_old.st_ino, st_new.st_ino);
  //}
  EXPECT_EQ(st_old.st_mode, st_new.st_mode);
  EXPECT_EQ(st_old.st_uid, st_new.st_uid);
  EXPECT_EQ(st_old.st_gid, st_new.st_gid);
  EXPECT_EQ(st_old.st_size, st_new.st_size);

  TempPath_RemoveAndFreePath(new_path);
  free(old_file);
}

// Test link counts with a regular file as the child.
TEST_F(StatTest, LinkCountsWithRegularFileChild) {
    (void)self;

    char *dir = TempPath_CreateDir();

  struct stat st_parent_before = {};
  assert(stat(dir, &st_parent_before) == 0);
  EXPECT_EQ(st_parent_before.st_nlink, 2);

  // Adding a regular file doesn't adjust the parent's link count.
  char *child = TempPath_CreateFileIn(dir);

  struct stat st_parent_after = {};
  assert(stat(dir, &st_parent_after) == 0);
  EXPECT_EQ(st_parent_after.st_nlink, 2);

  // The child should have a single link from the parent.
  struct stat st_child = {};
  assert(stat(child, &st_child) == 0);
  EXPECT_TRUE(S_ISREG(st_child.st_mode));
  EXPECT_EQ(st_child.st_nlink, 1);

  // Finally unlinking the child should not affect the parent's link count.
  assert(unlink(child) == 0);
  assert(stat(dir, &st_parent_after) == 0);
  EXPECT_EQ(st_parent_after.st_nlink, 2);

  free(child);

  TempPath_RemoveAndFreePath(dir);
}

// This test verifies that inodes remain around when there is an open fd
// after link count hits 0.
//
// It is marked NoSave because we don't support saving unlinked files.
TEST_F(StatTest, ZeroLinksOpenFdRegularFileChild_NoSave) {
    (void)self;

  // Setting the enviornment variable GVISOR_GOFER_UNCACHED to any value
  // will prevent this test from running, see the tmpfs lifecycle.
  //
  // We need to support this because when a file is unlinked and we forward
  // the stat to the gofer it would return ENOENT.
  //const char* uncached_gofer = getenv("GVISOR_GOFER_UNCACHED");
  //SKIP_IF(uncached_gofer != nullptr);

    char *dir = TempPath_CreateDir();
  char *child = TempPath_CreateFileWith(
      dir, "hello", TempPath_kDefaultFileMode);

  // The child should have a single link from the parent.
  struct stat st_child_before = {};
  assert(stat(child, &st_child_before) == 0);
  EXPECT_TRUE(S_ISREG(st_child_before.st_mode));
  EXPECT_EQ(st_child_before.st_nlink, 1);
  EXPECT_EQ(st_child_before.st_size, 5);  // Hello is 5 bytes.

  // Open the file so we can fstat after unlinking.
  int fd =
      open(child, O_RDONLY);
  assert(fd >= 0);

  // Now a stat should return ENOENT but we should still be able to stat
  // via the open fd and fstat.
  assert(unlink(child) == 0);

  // Since the file has no more links stat should fail.
  struct stat st_child_after = {};
  assert(stat(child, &st_child_after) == -1 && errno == ENOENT);

  // Fstat should still allow us to access the same file via the fd.
  struct stat st_child_fd = {};
  assert(fstat(fd, &st_child_fd) == 0);
  EXPECT_EQ(st_child_before.st_dev, st_child_fd.st_dev);
  EXPECT_EQ(st_child_before.st_ino, st_child_fd.st_ino);
  EXPECT_EQ(st_child_before.st_mode, st_child_fd.st_mode);
  EXPECT_EQ(st_child_before.st_uid, st_child_fd.st_uid);
  EXPECT_EQ(st_child_before.st_gid, st_child_fd.st_gid);
  EXPECT_EQ(st_child_before.st_size, st_child_fd.st_size);

  // TODO(b/34861058): This isn't ideal but since fstatfs(2) will always return
  // OVERLAYFS_SUPER_MAGIC we have no way to know if this fs is backed by a
  // gofer which doesn't support links.
  EXPECT_TRUE(st_child_fd.st_nlink == 0 || st_child_fd.st_nlink == 1);

  assert(close(fd) == 0);

  free(child);
  TempPath_RemoveAndFreePath(dir);
}

// Test link counts with a directory as the child.
TEST_F(StatTest, LinkCountsWithDirChild) {
    (void)self;

  // Skip this test if we are testing overlayfs because overlayfs does not
  // (intentionally) return the correct nlink value for directories.
  // See fs/overlayfs/inode.c:ovl_getattr().
  //SKIP_IF(ASSERT_NO_ERRNO_AND_VALUE(IsOverlayfs(GetAbsoluteTestTmpdir())));

    char *dir = TempPath_CreateDir();

  // Before a child is added the two links are "." and the link from the parent.
  struct stat st_parent_before = {};
  assert(stat(dir, &st_parent_before) == 0);
  EXPECT_EQ(st_parent_before.st_nlink, 2);

  // Create a subdirectory and stat for the parent link counts.
  char *sub_dir =
      TempPath_CreateDirIn(dir);

  // The three links are ".", the link from the parent, and the link from
  // the child as "..".
  struct stat st_parent_after = {};
  assert(stat(dir, &st_parent_after) == 0);
  EXPECT_EQ(st_parent_after.st_nlink, 3);

  // The child will have 1 link from the parent and 1 link which represents ".".
  struct stat st_child = {};
  assert(stat(sub_dir, &st_child) == 0);
  EXPECT_TRUE(S_ISDIR(st_child.st_mode));
  EXPECT_EQ(st_child.st_nlink, 2);

  // Finally delete the child dir and the parent link count should return to 2.
  assert(rmdir(sub_dir) == 0);
  assert(stat(dir, &st_parent_after) == 0);

  // Now we should only have links from the parent and "." since the subdir
  // has been removed.
  EXPECT_EQ(st_parent_after.st_nlink, 2);

  free(sub_dir);

  TempPath_RemoveAndFreePath(dir);
}

// Test statting a child of a non-directory.
TEST_F(StatTest, ChildOfNonDir) {
  // Create a path that has a child of a regular file.
  char *filename = JoinPath(self->test_file_name_, "child");

  // Statting the path should return ENOTDIR.
  struct stat st;
  assert(lstat(filename, &st) == -1 && errno == ENOTDIR);

  free(filename);
}

// Test lstating a symlink directory.
TEST_F(StatTest, LstatSymlinkDir) {
    (void)self;

  // Create a directory and symlink to it.
    char *dir = TempPath_CreateDir();
  char *symlink_to_dir = NewTempAbsPath();
  assert(symlink(dir, symlink_to_dir) == 0);

  // Lstat on the symlink should return symlink data.
  struct stat st = {};
  assert(lstat(symlink_to_dir, &st) == 0);
  EXPECT_FALSE(S_ISDIR(st.st_mode));
  EXPECT_TRUE(S_ISLNK(st.st_mode));

  // Lstat on the symlink with a trailing slash should return the directory
  // data.
  char *symlink_to_dir_with_trailing_slash = absl_StrCat(symlink_to_dir, "/");
  assert(lstat(symlink_to_dir_with_trailing_slash, &st) == 0);
  free(symlink_to_dir_with_trailing_slash);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  assert(unlink(symlink_to_dir) == 0);
  free(symlink_to_dir);
  TempPath_RemoveAndFreePath(dir);
}

// Verify that we get an ELOOP from too many symbolic links even when there
// are directories in the middle.
TEST_F(StatTest, LstatELOOPPath) {
    (void)self;

  char *dir = TempPath_CreateDir();
  char *subdir_base = "subdir";
  char *mkdir_arg = JoinPath(dir, subdir_base);
  assert(mkdir(mkdir_arg, 0755) == 0);

  char *target = JoinPath(mkdir_arg, subdir_base);
  char *dst = JoinPath("..", subdir_base);
  assert(symlink(dst, target) == 0);
  free(dst);

  // Now build a path which is /subdir/subdir/... repeated many times so that
  // we can build a path that is shorter than PATH_MAX but can still cause
  // too many symbolic links. Note: Every other subdir is actually a directory
  // so we're not in a situation where it's a -> b -> a -> b, where a and b
  // are symbolic links.
  char *path = strdup(dir);
  char *subdir_append = absl_StrCat("/", subdir_base);
  do {
    absl_StrAppend(&path, subdir_append);
    // Keep appending /subdir until we would overflow PATH_MAX.
  } while ((strlen(path) + strlen(subdir_append)) < PATH_MAX);
  free(subdir_append);

  struct stat s = {};
  assert(lstat(path, &s) == -1 && errno == ELOOP);
  free(path);

  assert(unlink(target) == 0);
  free(target);

  TempPath_RemoveAndFreePath(mkdir_arg);
  TempPath_RemoveAndFreePath(dir);
}

TEST(SimpleStatTest, DifferentFilesHaveDifferentDeviceInodeNumberPairs) {
    char *file1 = TempPath_CreateFile();
    char *file2 = TempPath_CreateFile();

    //MaybeSave();
  struct stat st1 = Lstat(file1);
  //MaybeSave();
  struct stat st2 = Lstat(file2);
  EXPECT_FALSE(st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino);

  TempPath_RemoveAndFreePath(file2);
  TempPath_RemoveAndFreePath(file1);
}

// Ensure that inode allocation for anonymous devices work correctly across
// save/restore. In particular, inode numbers should be unique across S/R.
TEST(SimpleStatTest, AnonDeviceAllocatesUniqueInodesAcrossSaveRestore) {
  // Use sockets as a convenient way to create inodes on an anonymous device.
  int fd;
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  assert(fd >= 0);
  int fd1 = fd;
  //MaybeSave();
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  assert(fd >= 0);
  int fd2 = fd;

  struct stat st1;
  struct stat st2;
  assert(fstat(fd1, &st1) == 0);
  assert(fstat(fd2, &st2) == 0);

  // The two fds should have different inode numbers.
  EXPECT_NE(st2.st_ino, st1.st_ino);

  // Verify again after another S/R cycle. The inode numbers should remain the
  // same.
  //MaybeSave();

  struct stat st1_after;
  struct stat st2_after;
  assert(fstat(fd1, &st1_after) == 0);
  assert(fstat(fd2, &st2_after) == 0);

  EXPECT_EQ(st1_after.st_ino, st1.st_ino);
  EXPECT_EQ(st2_after.st_ino, st2.st_ino);

  assert(close(fd2) == 0);
  assert(close(fd1) == 0);
}

#if !defined(YALIBCT_LIBC_DOESNT_HAVE_SYSCALL) && (defined(SYS_statx) || defined(__linux__))
#define HAS_STATX_TESTS

#ifndef SYS_statx
#if defined(__x86_64__)
#define SYS_statx 332
#elif defined(__aarch64__)
#define SYS_statx 291
#else
#error "Unknown architecture"
#endif
#endif  // SYS_statx

#ifndef STATX_ALL
#define STATX_ALL 0x00000fffU
#endif  // STATX_ALL

// struct kernel_statx_timestamp is a Linux statx_timestamp struct.
struct kernel_statx_timestamp {
  int64_t tv_sec;
  uint32_t tv_nsec;
  int32_t __reserved;
};

// struct kernel_statx is a Linux statx struct. Old versions of glibc do not
// expose it. See include/uapi/linux/stat.h
struct kernel_statx {
  uint32_t stx_mask;
  uint32_t stx_blksize;
  uint64_t stx_attributes;
  uint32_t stx_nlink;
  uint32_t stx_uid;
  uint32_t stx_gid;
  uint16_t stx_mode;
  uint16_t __spare0[1];
  uint64_t stx_ino;
  uint64_t stx_size;
  uint64_t stx_blocks;
  uint64_t stx_attributes_mask;
  struct kernel_statx_timestamp stx_atime;
  struct kernel_statx_timestamp stx_btime;
  struct kernel_statx_timestamp stx_ctime;
  struct kernel_statx_timestamp stx_mtime;
  uint32_t stx_rdev_major;
  uint32_t stx_rdev_minor;
  uint32_t stx_dev_major;
  uint32_t stx_dev_minor;
  uint64_t __spare2[14];
};

int gvisor_internal_test_statx(int dirfd, const char* pathname, int flags, unsigned int mask,
          struct kernel_statx* statxbuf) {
  return syscall(SYS_statx, dirfd, pathname, flags, mask, statxbuf);
}

TEST_F(StatTest, StatxAbsPath) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  struct kernel_statx stx;
  assert(gvisor_internal_test_statx(-1, self->test_file_name_, 0, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));
}

TEST_F(StatTest, StatxRelPathDirFD) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  struct kernel_statx stx;
  int dirfd =
      Open(GetAbsoluteTestTmpdir(), O_RDONLY, 0);
  char *filename = Basename(self->test_file_name_);

  assert(gvisor_internal_test_statx(dirfd, filename, 0, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));

  free(filename);
  assert(close(dirfd) == 0);
}

TEST_F(StatTest, StatxRelPathCwd) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  assert(chdir(GetAbsoluteTestTmpdir()) == 0);
  char *filename = Basename(self->test_file_name_);
  struct kernel_statx stx;
  assert(gvisor_internal_test_statx(AT_FDCWD, filename, 0, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));

  free(filename);
}

TEST_F(StatTest, StatxEmptyPath) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  int fd = Open(self->test_file_name_, O_RDONLY, 0);
  struct kernel_statx stx;
  assert(gvisor_internal_test_statx(fd, "", AT_EMPTY_PATH, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));

  assert(close(fd) == 0);
}

TEST_F(StatTest, StatxDoesNotRejectExtraneousMaskBits) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  struct kernel_statx stx;
  // Set all mask bits except for STATX__RESERVED.
  unsigned mask = 0xffffffff & ~0x80000000;
  assert(gvisor_internal_test_statx(-1, self->test_file_name_, 0, mask, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));
}

TEST_F(StatTest, StatxRejectsReservedMaskBit) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  struct kernel_statx stx;
  // Set STATX__RESERVED in the mask.
  assert(gvisor_internal_test_statx(-1, self->test_file_name_, 0, 0x80000000, &stx) == -1 && errno == EINVAL);
}

TEST_F(StatTest, StatxSymlink) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  char *parent_dir = GetAbsoluteTestTmpdir();
  char *link =
      TempPath_CreateSymlinkTo(parent_dir, self->test_file_name_);

  struct kernel_statx stx;
  assert(gvisor_internal_test_statx(AT_FDCWD, link, AT_SYMLINK_NOFOLLOW, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISLNK(stx.stx_mode));
  assert(gvisor_internal_test_statx(AT_FDCWD, link, 0, STATX_ALL, &stx) == 0);
  EXPECT_TRUE(S_ISREG(stx.stx_mode));

  TempPath_RemoveAndFreePath(link);
}

TEST_F(StatTest, StatxInvalidFlags) {
  SKIP_IF(gvisor_internal_test_statx(-1, NULL, 0, 0, NULL) < 0 &&
          errno == ENOSYS);

  struct kernel_statx stx;
  assert(gvisor_internal_test_statx(AT_FDCWD, self->test_file_name_, 12345, 0, &stx) == -1 && errno == EINVAL);

  // Sync flags are mutually exclusive.
  assert(gvisor_internal_test_statx(AT_FDCWD, self->test_file_name_,
                                    AT_STATX_FORCE_SYNC | AT_STATX_DONT_SYNC, 0, &stx) && errno == EINVAL);
}

#endif

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    FileTest_FstatatAbs();
    FileTest_FstatatEmptyPath();
    FileTest_FstatatRel();
    FileTest_FstatatSymlink();
    FileTest_Nlinks();
    FileTest_BlocksIncreaseOnWrite();
    FileTest_PathNotCleaned();
    FileTest_PathCanContainDotDot();
    FileTest_PathCanContainEmptyComponent();
    FileTest_TrailingSlashNotCleanedReturnsENOTDIR();
    FileTest_FstatFileWithOpath();
    FileTest_FstatDirWithOpath();
    FileTest_FstatatDirWithOpath();
    FileTest_FstatatSymlinkDir();
    FileTest_FstatatSymlinkDirWithTrailingSlash();
    FileTest_FstatatSymlinkDirWithTrailingSlashSameInode();
    FileTest_LeadingDoubleSlash();
    FileTest_StatDoesntChangeAfterRename();
    FileTest_LinkCountsWithRegularFileChild();
    FileTest_ZeroLinksOpenFdRegularFileChild_NoSave();
    FileTest_LinkCountsWithDirChild();
    FileTest_ChildOfNonDir();
    FileTest_LstatSymlinkDir();
    FileTest_LstatELOOPPath();
#ifdef HAS_STATX_TESTS
    FileTest_StatxAbsPath();
    FileTest_StatxRelPathDirFD();
    FileTest_StatxRelPathCwd();
    FileTest_StatxEmptyPath();
    FileTest_StatxDoesNotRejectExtraneousMaskBits();
    FileTest_StatxRejectsReservedMaskBit();
    FileTest_StatxSymlink();
    FileTest_StatxInvalidFlags();
#endif
}
