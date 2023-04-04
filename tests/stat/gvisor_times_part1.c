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

// File creation time is reflected in atime, mtime, and ctime.
TEST(StatTimesTest, FileCreation) {
  //const DisableSave ds;  // Timing-related test.

  // Get a time for when the file is created.
  //
  // FIXME(b/132819225): See above.
  const time_t before = time(NULL) - 1;
  char *path = TempPath_CreateFile();
  const time_t after = time(NULL) + 1;

  struct timespec atime, mtime, ctime;
  struct GetTime_Retval GetTime_result = GetTime(path);
  atime = GetTime_result.atime;
  mtime = GetTime_result.mtime;
  ctime = GetTime_result.ctime;

  EXPECT_LE(before, atime.tv_sec);
  EXPECT_LE(before, mtime.tv_sec);
  EXPECT_LE(before, ctime.tv_sec);
  EXPECT_GE(after, atime.tv_sec);
  EXPECT_GE(after, mtime.tv_sec);
  EXPECT_GE(after, ctime.tv_sec);

  TempPath_RemoveAndFreePath(path);
}

static void StatTimesTest_FileChmod_internal_lambda(void *callback_data)
{
    assert(chmod((char *)callback_data, 0666) == 0);
}

// Calling chmod on a file changes ctime.
TEST(StatTimesTest, FileChmod) {
  char *file = TempPath_CreateFile();

  CheckTimes(&file, StatTimesTest_FileChmod_internal_lambda, file, AtimeEffect_Unchanged, MtimeEffect_Unchanged,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(file);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
    StatTimesTest_FileCreation();
    StatTimesTest_FileChmod();
}
