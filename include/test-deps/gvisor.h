// Derived from code with this license:
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
#pragma once

#include "test-lib/should-always-be-included.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

static char GetAbsoluteTestTmpdir_internal_tmpdir_template[] = "/tmp/yalibct.gvisor.XXXXXX";

// Note: code using GetAbsoluteTempTmpdir MUST clean up and call GetAbsoluteTempTmpdir_remove_dir, otherwise the temporary directory will still be there after program termination
static char *GetAbsoluteTestTmpdir() {
    if (strcmp(GetAbsoluteTestTmpdir_internal_tmpdir_template, "/tmp/yalibct.gvisor.XXXXXX") == 0)
        assert(mkdtemp(GetAbsoluteTestTmpdir_internal_tmpdir_template) == GetAbsoluteTestTmpdir_internal_tmpdir_template);
    return GetAbsoluteTestTmpdir_internal_tmpdir_template;
}

static void GetAbsoluteTestTmpdir_remove_dir() {
    if (!strcmp(GetAbsoluteTestTmpdir_internal_tmpdir_template, "/tmp/yalibct.gvisor.XXXXXX"))
        return;
    assert(remove(GetAbsoluteTestTmpdir_internal_tmpdir_template) == 0);
}

static uintmax_t global_temp_file_number;

// The global file number helps maintain file naming consistency across
// different runs of a test.
//
// The timestamp is necessary because the test infrastructure invokes each
// test case in a separate process (resetting global_temp_file_number) and
// potentially in parallel, which allows for races between selecting and using a
// name.
// (yalibct note: we don't bother with the timestamp since we don't do the "each test case in a separate process" thing)
static char *NextTempBasename() {
    char *result;
    assert(asprintf(&result, "gvisor_test_temp_%ju", global_temp_file_number++) >= 0);
    return result;
                    /*return absl::StrCat("gvisor_test_temp_", global_temp_file_number++, "_",
                      absl::ToUnixNanos(absl::Now()));*/
}

char *JoinPath(const char *path1, const char *path2) {
    char *result;
    if (path1 == NULL || *path1 == '\0') {
        return strdup(path2);
    }
    if (path2 == NULL || *path2 == '\0') {
        return strdup(path1);
    }

    if (path1[strlen(path1) - 1] == '/') {
        if (path2[0] == '/') {
            assert(asprintf(&result, "%s%s", path1, path2 + 1) >= 0);
            return result;
        }
    } else {
        if (path2[0] != '/') {
            assert(asprintf(&result, "%s/%s", path1, path2) >= 0);
            return result;
        }
    }
    assert(asprintf(&result, "%s%s", path1, path2) >= 0);
    return result;
}

static char *NewTempAbsPathInDir(const char *dir) {
    char *tempBasename = NextTempBasename();
    char *result = JoinPath(dir, tempBasename);
    free(tempBasename);
    return result;
}

static char *NewTempAbsPath() {
    return NewTempAbsPathInDir(GetAbsoluteTestTmpdir());
}

struct FileTest {
    char *test_file_name_;
    int test_file_fd_;
};

static void FileTest_SetUp(struct FileTest *self)
{
    self->test_file_name_ = NewTempAbsPath();
    self->test_file_fd_ = open(self->test_file_name_, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    assert(self->test_file_fd_ >= 0);
}

static void FileTest_UnlinkFile(struct FileTest *self)
{
    if (self->test_file_name_ != NULL) {
        if (*self->test_file_name_ != '\0')
            assert(unlink(self->test_file_name_) == 0);
        free(self->test_file_name_);
        self->test_file_name_ = NULL;
    }
}

static void FileTest_CloseFile(struct FileTest *self)
{
    assert(close(self->test_file_fd_) == 0);
    self->test_file_fd_ = -1;
}

static void FileTest_TearDown(struct FileTest *self)
{
    FileTest_CloseFile(self);
    FileTest_UnlinkFile(self);
}

struct GetRelativePath_retval {
    char *first;
    char *second;
};

static struct GetRelativePath_retval SplitPath(
    const char *path) {
    size_t pos = strrchr(path, '/') != NULL ? strrchr(path, '/') - path : SIZE_MAX;
    struct GetRelativePath_retval result;

    // Handle the case with no '/' in 'path'.
    if (pos == SIZE_MAX) {
        result.first = NULL;
        result.second = strdup(path);
        return result;
    }

    // Handle the case with a single leading '/' in 'path'.
    if (pos == 0) {
        result.first = strdup("/");
        result.second = strdup(path + 1);
        return result;
    }

    result.first = strndup(path, pos);
    result.second = strdup((pos + 1 > strlen(path)) ? "" : path + pos + 1);
    return result;
}

static char *Basename(const char *path) {
    struct GetRelativePath_retval result = SplitPath(path);
    free(result.first);
    return result.second;
}

enum { TempPath_kDefaultDirMode = 0755 };

static char *TempPath_CreateIn(const char *parent, void (*f)(const char *path, void *callback_data), void *callback_data)
{
    char *path = NewTempAbsPathInDir(parent);
    f(path, callback_data);
    return path;
}

static void TempPath_CreateDirWith_internal_lambda(const char *path, void *callback_data)
{
    assert(mkdir(path, *(mode_t *)callback_data) == 0);
}

static char *TempPath_CreateDirWith(const char *parent, mode_t mode)
{
    return TempPath_CreateIn(parent, TempPath_CreateDirWith_internal_lambda, &mode);
}

static char *TempPath_CreateDirIn(const char *parent)
{
    return TempPath_CreateDirWith(parent, TempPath_kDefaultDirMode);
}

static char *TempPath_CreateDir()
{
    return TempPath_CreateDirIn(GetAbsoluteTestTmpdir());
}

static nlink_t Links(const char *path)
{
    struct stat stat_buffer;
    assert(stat(path, &stat_buffer) == 0);
    return stat_buffer.st_nlink;
}

enum { TempPath_kDefaultFileMode = 0644 };

static void WriteContentsToFD(int fd, const char *contents)
{
    size_t written = 0;
    while (written < strlen(contents)) {
        ssize_t wrote = write(fd, contents + written, strlen(contents) - written);
        if (wrote < 0) {
            assert(errno == EINTR);
            continue;
        }
        written += wrote;
    }
}

// Create a file with the given contents (if it does not already exist with the
// given mode) and then set the contents.
static void CreateWithContents(const char *path, const char *contents, mode_t mode)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    assert(fd >= 0);
    WriteContentsToFD(fd, contents);
    assert(close(fd) == 0);
}

struct TempPath_CreateFileWith_internal_lambda_callback_data {
    const char *contents;
    mode_t mode;
};

static void TempPath_CreateFileWith_internal_lambda(const char *path, void *callback_data_void_ptr)
{
    struct TempPath_CreateFileWith_internal_lambda_callback_data *callback_data = (struct TempPath_CreateFileWith_internal_lambda_callback_data *)callback_data_void_ptr;

    CreateWithContents(path, callback_data->contents, callback_data->mode | 0200);
    assert(chmod(path, callback_data->mode) == 0);
}

static char *TempPath_CreateFileWith(const char *parent, const char *contents, mode_t mode)
{
    struct TempPath_CreateFileWith_internal_lambda_callback_data callback_data = {
        contents,
        mode
    };
    return TempPath_CreateIn(parent, TempPath_CreateFileWith_internal_lambda, &callback_data);
}

static char *TempPath_CreateFileIn(const char *parent)
{
    return TempPath_CreateFileWith(parent, "", TempPath_kDefaultFileMode);
}

static void TempPath_RemoveAndFreePath(char *path)
{
    assert(remove(path) == 0);
    free(path);
}

static char *TempPath_CreateFile()
{
    return TempPath_CreateFileIn(GetAbsoluteTestTmpdir());
}

struct stat Lstat(const char *path) {
  struct stat stat_buf;
  int res = lstat(path, &stat_buf);
  assert(res == 0);
  return stat_buf;
}

// SKIP_IF may be used to skip a test case.
//
// These cases are still emitted, but a SKIPPED line will appear.
#define SKIP_IF(expr)                \
  do {                               \
    if (expr) GTEST_SKIP(); \
  } while (0)

// Wrapper around open(2) that returns a FileDescriptor.
static inline int Open(const char *path, int flags,
                                         mode_t mode) {
  int fd = open(path, flags, mode);
  assert(fd >= 0);
  //MaybeSave();
  return fd;
}

static void TempPath_CreateSymlinkTo_internal_lambda(const char *path, void *callback_data_void_ptr)
{
    assert(symlink((const char *)callback_data_void_ptr, path) == 0);
}

static char *TempPath_CreateSymlinkTo(const char *parent, const char *dest) {
    return TempPath_CreateIn(parent, TempPath_CreateSymlinkTo_internal_lambda, (void *)dest);
}

#define GVISOR_MAKE_MAIN() static void real_user_main(); int main() { real_user_main(); GetAbsoluteTestTmpdir_remove_dir(); } static void real_user_main()

#define RetryEINTR(RetryEINTR_return_var, f, f_callback_data, ...)      \
    while (true) {                                                      \
        errno = 0;                                                      \
        RetryEINTR_return_var = (f)((f_callback_data), __VA_ARGS__);    \
        if (RetryEINTR_return_var != 1 || errno != EINTR)               \
            break;                                                      \
    }

ssize_t internal_ApplyFileIoSyscall(ssize_t (*f)(void *callback_data, size_t completed), void *f_callback_data, size_t const count) {
  size_t completed = 0;
  // `do ... while` because some callers actually want to make a syscall with a
  // count of 0.
  do {
      ssize_t cur;
      RetryEINTR(cur, f, f_callback_data, completed);
    if (cur < 0) {
      return cur;
    } else if (cur == 0) {
      break;
    }
    completed += cur;
  } while (completed < count);
  return completed;
}

struct WriteFd_internal_lambda_callback_data {
    int fd;
    const void *buf;
    size_t count;
};

static ssize_t WriteFd_internal_lambda(void *callback_data_void_ptr, size_t completed)
{
    struct WriteFd_internal_lambda_callback_data *callback_data = (struct WriteFd_internal_lambda_callback_data *)callback_data_void_ptr;

    return write(callback_data->fd, (char const*)(callback_data->buf) + completed,
                 callback_data->count - completed);
}

static inline ssize_t WriteFd(int fd, void const* buf, size_t count) {
    struct WriteFd_internal_lambda_callback_data callback_data = { fd, buf, count };

    return internal_ApplyFileIoSyscall(WriteFd_internal_lambda, &callback_data,
      count);
}

struct ReadFd_internal_lambda_callback_data {
    int fd;
    const void *buf;
    size_t count;
};

static ssize_t ReadFd_internal_lambda(void *callback_data_void_ptr, size_t completed)
{
    struct ReadFd_internal_lambda_callback_data *callback_data = (struct ReadFd_internal_lambda_callback_data *)callback_data_void_ptr;

    return read(callback_data->fd, (char*)(callback_data->buf) + completed, callback_data->count - completed);
}

static inline ssize_t ReadFd(int fd, void* buf, size_t count) {
    struct ReadFd_internal_lambda_callback_data callback_data = { fd, buf, count };
    return internal_ApplyFileIoSyscall(ReadFd_internal_lambda, &callback_data, count);
}

struct ListDir_retval {
    char **filenames;
    size_t filename_count;
};

struct ListDir_retval ListDir(const char *abspath, bool skipdots) {
    struct ListDir_retval files = { NULL, 0 };

  DIR* dir = opendir(abspath);
  assert(dir != NULL);
  while (true) {
    // Readdir(3): If the end of the directory stream is reached, NULL is
    // returned and errno is not changed.  If an error occurs, NULL is returned
    // and errno is set appropriately.  To distinguish end of stream and from an
    // error, set errno to zero before calling readdir() and then check the
    // value of errno if NULL is returned.
    errno = 0;
    struct dirent* dp = readdir(dir);
    if (dp == NULL) {
      assert(errno == 0);
      break;  // We're done.
    }

    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
      if (skipdots) {
        continue;
      }
    }
    files.filenames = realloc(files.filenames, ++files.filename_count * sizeof(char *));
    assert(files.filenames != NULL);
    files.filenames[files.filename_count - 1] = strdup(dp->d_name);
  }

  assert(closedir(dir) == 0);
  return files;
}
