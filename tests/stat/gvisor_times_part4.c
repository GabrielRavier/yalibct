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

/*namespace gvisor {
namespace testing {

namespace {

using ::testing::IsEmpty;
using ::testing::Not;*/

#include "gvisor_times_common.h"

static void StatTimesTest_FileWrite_internal_lambda(void *callback_data)
{
    const char *contents = "all the single dollars";
    assert(WriteFd(*(const int *)callback_data, contents, strlen(contents)) == strlen(contents));
}

// Writing a file changes mtime and ctime.
TEST(StatTimesTest, FileWrite) {
  char *file =
      TempPath_CreateFileWith(GetAbsoluteTestTmpdir(), "yaaass", 0666);

  int fd =
      Open(file, O_RDWR, 0);
  CheckTimes(&file, StatTimesTest_FileWrite_internal_lambda, &fd, AtimeEffect_Unchanged, MtimeEffect_Changed,
             CtimeEffect_Changed);
  assert(close(fd) == 0);

  TempPath_RemoveAndFreePath(file);
}

struct StatTimesTest_FileRead_internal_lambda_callback_data {
    int fd;
    char *contents;
};

static void StatTimesTest_FileRead_internal_lambda(void *callback_data_void_ptr)
{
    struct StatTimesTest_FileRead_internal_lambda_callback_data *callback_data = (struct StatTimesTest_FileRead_internal_lambda_callback_data *)callback_data_void_ptr;

    char buf[20];
    assert(ReadFd(callback_data->fd, buf, sizeof(buf)) == strlen(callback_data->contents));
}

// Reading a file changes atime.
TEST(StatTimesTest, FileRead) {
  char *contents = "bills bills bills";
  char *file =
      TempPath_CreateFileWith(GetAbsoluteTestTmpdir(), contents, 0666);

  int fd =
      Open(file, O_RDONLY, 0);

  struct StatTimesTest_FileRead_internal_lambda_callback_data callback_data = { fd, contents };

  CheckTimes(&file, StatTimesTest_FileRead_internal_lambda, &callback_data, AtimeEffect_Changed, MtimeEffect_Unchanged,
             CtimeEffect_Unchanged);

  assert(close(fd) == 0);

  TempPath_RemoveAndFreePath(file);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    StatTimesTest_FileWrite();
    StatTimesTest_FileRead();
}
