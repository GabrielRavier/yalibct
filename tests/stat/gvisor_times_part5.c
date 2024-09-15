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

static void StatTimesTest_DirList_internal_lambda(void *callback_data) {
    struct ListDir_retval contents = ListDir((char *)callback_data, false);
    assert(contents.filename_count != 0);
    for (size_t i = 0; i < contents.filename_count; ++i)
        free(contents.filenames[i]);
    free((void *)contents.filenames);
};

// Listing files in a directory changes atime.
TEST(StatTimesTest, DirList) {
    char *dir = TempPath_CreateDir();
    char *file =
        TempPath_CreateFileIn(dir);

    CheckTimes(&dir, StatTimesTest_DirList_internal_lambda, dir, AtimeEffect_Changed, MtimeEffect_Unchanged,
               CtimeEffect_Unchanged);

    TempPath_RemoveAndFreePath(file);
    TempPath_RemoveAndFreePath(dir);
}

struct StatTimesTest_DirCreateFile_internal_lambda_callback_data {
    char **file;
    char *dir;
};

static void StatTimesTest_DirCreateFile_internal_lambda(void *callback_data_void_ptr)
{
    struct StatTimesTest_DirCreateFile_internal_lambda_callback_data *callback_data = (struct StatTimesTest_DirCreateFile_internal_lambda_callback_data *)callback_data_void_ptr;
    *callback_data->file = TempPath_CreateFileIn(callback_data->dir);
}

// Creating a file in a directory changes mtime and ctime.
TEST(StatTimesTest, DirCreateFile) {
    char *dir = TempPath_CreateDir();

  char *file;

  struct StatTimesTest_DirCreateFile_internal_lambda_callback_data callback_data = { &file, dir };

  CheckTimes(&dir, StatTimesTest_DirCreateFile_internal_lambda, &callback_data, AtimeEffect_Unchanged, MtimeEffect_Changed,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(file);

  TempPath_RemoveAndFreePath(dir);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    StatTimesTest_DirList();
    StatTimesTest_DirCreateFile();
}
