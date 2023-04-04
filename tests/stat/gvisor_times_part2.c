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

struct StatTimesTest_FileRename_internal_lambda_callback_data {
    char **file;
    char *newpath;
};

static void StatTimesTest_FileRename_internal_lambda(void *callback_data_void_ptr)
{
    struct StatTimesTest_FileRename_internal_lambda_callback_data *callback_data = (struct StatTimesTest_FileRename_internal_lambda_callback_data *)callback_data_void_ptr;
    assert(rename(*callback_data->file, callback_data->newpath) == 0);
    free(*callback_data->file);
    *callback_data->file = callback_data->newpath;
}

// Renaming a file changes ctime.
TEST(StatTimesTest, FileRename) {
  char *file = TempPath_CreateFile();

  char *newpath = NewTempAbsPath();

  struct StatTimesTest_FileRename_internal_lambda_callback_data callback_data = {
      &file,
      newpath
  };

  CheckTimes(&file, StatTimesTest_FileRename_internal_lambda, &callback_data, AtimeEffect_Unchanged, MtimeEffect_Unchanged,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(file);
}

// Renaming a file changes ctime, even with an open FD.
//
// NOTE(b/132732387): This is a regression test for fs/gofer failing to update
// cached ctime.
TEST(StatTimesTest, FileRenameOpenFD) {
    char *file = TempPath_CreateFile();

  // Holding an FD shouldn't affect behavior.
  int fd =
      Open(file, O_RDONLY, 0);

  char *newpath = NewTempAbsPath();

  // FIXME(b/132814682): Restore fails with an uncached gofer and an open FD
  // across rename.
  //
  // N.B. The logic here looks backwards because it isn't possible to
  // conditionally disable save, only conditionally re-enable it.
  /*DisableSave ds;
  if (!getenv("GVISOR_GOFER_UNCACHED")) {
    ds.reset();
  }*/

  struct StatTimesTest_FileRename_internal_lambda_callback_data callback_data = {
      &file,
      newpath
  };

  CheckTimes(&file, StatTimesTest_FileRename_internal_lambda, &callback_data, AtimeEffect_Unchanged, MtimeEffect_Unchanged,
             CtimeEffect_Changed);

  assert(close(fd) == 0);
  TempPath_RemoveAndFreePath(file);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    StatTimesTest_FileRename();
    StatTimesTest_FileRenameOpenFD();
}
