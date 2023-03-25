//===-- Unittests for stat ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

/*#include "src/fcntl/open.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/ErrnoSetterMatcher.h"
#include "utils/UnitTest/Test.h"
#include "utils/testutils/FDReader.h"*/
#include "test-deps/llvm-project.h"
#include "test-lib/chdir-to-tmpdir.h"
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

TEST(LlvmLibcStatTest, CreatAndReadMode) {
    //using __llvm_libc::testing::ErrnoSetterMatcher::Fails;
    //using __llvm_libc::testing::ErrnoSetterMatcher::Succeeds;

  // The test file is initially writable. We open it for writing and ensure
  // that it indeed can be opened for writing. Next, we close the file and
  // make it readonly using chmod. We test that chmod actually succeeded by
  // trying to open the file for writing and failing.
  constexpr const char *TEST_FILE = "testdata/stat.test";
  errno = 0;

  int fd = __llvm_libc open(TEST_FILE, O_CREAT | O_WRONLY | O_CLOEXEC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(errno, 0);
  assert(close(fd) == 0);
  assert(errno == 0);//ASSERT_THAT(__llvm_libc close(fd), Succeeds(0));

  struct stat statbuf;
  assert(stat(TEST_FILE, &statbuf) == 0);//ASSERT_THAT(__llvm_libc stat(TEST_FILE, &statbuf), Succeeds(0));
  assert(errno == 0);

  ASSERT_EQ((int)(statbuf.st_mode), (int)(S_IRWXU | S_IFREG));

  assert(unlink(TEST_FILE) == 0);//ASSERT_THAT(__llvm_libc unlink(TEST_FILE), Succeeds(0));
  assert(errno == 0);
}

TEST(LlvmLibcStatTest, NonExistentFile) {
  errno = 0;
  //using __llvm_libc::testing::ErrnoSetterMatcher::Fails;
  struct stat statbuf;
  assert(stat("non-existent-file", &statbuf) == -1);//ASSERT_THAT(__llvm_libc::stat("non-existent-file", &statbuf), Fails(ENOENT));
  assert(errno == ENOENT);
  errno = 0;
}

int main()
{
    yalibct_llvm_project_chdir_to_tmpdir_and_create_testdata_dir();
    LlvmLibcStatTestCreatAndReadMode();
    LlvmLibcStatTestNonExistentFile();
    yalibct_llvm_project_chdir_to_tmpdir_remove_testdata_dir_and_tmpdir();
}
