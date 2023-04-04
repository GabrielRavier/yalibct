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

struct StatTimesTest_DirCreateDir_internal_lambda_callback_data {
    char **dir2;
    char *dir;
};

static void StatTimesTest_DirCreateDir_internal_lambda(void *callback_data_void_ptr)
{
    struct StatTimesTest_DirCreateDir_internal_lambda_callback_data *callback_data = (struct StatTimesTest_DirCreateDir_internal_lambda_callback_data *)callback_data_void_ptr;

    *callback_data->dir2 = TempPath_CreateDirIn(callback_data->dir);
}

// Creating a directory in a directory changes mtime and ctime.
TEST(StatTimesTest, DirCreateDir) {
    char *dir = TempPath_CreateDir();

  char *dir2;
  struct StatTimesTest_DirCreateDir_internal_lambda_callback_data callback_data = { &dir2, dir };
  CheckTimes(&dir, StatTimesTest_DirCreateDir_internal_lambda, &callback_data, AtimeEffect_Unchanged, MtimeEffect_Changed,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(dir2);

  TempPath_RemoveAndFreePath(dir);
}

static void StatTimesTest_DirRemoveFile_internal_lambda(void *callback_data)
{
    TempPath_RemoveAndFreePath((char *)callback_data);
}

// Removing a file from a directory changes mtime and ctime.
TEST(StatTimesTest, DirRemoveFile) {
  char *dir = TempPath_CreateDir();

  char *file = TempPath_CreateFileIn(dir);
  CheckTimes(&dir, StatTimesTest_DirRemoveFile_internal_lambda, file, AtimeEffect_Unchanged, MtimeEffect_Changed,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(dir);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    StatTimesTest_DirCreateDir();
    StatTimesTest_DirRemoveFile();
}
