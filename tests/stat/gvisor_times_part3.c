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

#if !defined(YALIBCT_LIBC_DOESNT_HAVE_UTIME_NOW) && !defined(YALIBCT_LIBC_DOESNT_HAVE_UTIME_OMIT)

static void StatTimesTest_FileUtimes_internal_lambda(void *callback_data)
{
    const struct timespec ts[2] = {{0, UTIME_NOW}, {0, UTIME_OMIT}};
    assert(utimensat(AT_FDCWD, (char *)callback_data, ts, 0) == 0);
}

// Calling utimes on a file changes ctime and the time that we ask to change
// (atime to now in this case).
TEST(StatTimesTest, FileUtimes) {
    char *file = TempPath_CreateFile();

    CheckTimes(&file, StatTimesTest_FileUtimes_internal_lambda, file, AtimeEffect_Changed, MtimeEffect_Unchanged,
             CtimeEffect_Changed);

    TempPath_RemoveAndFreePath(file);
}

#endif

static void StatTimesTest_FileTruncate_internal_lambda(void *callback_data)
{
    assert(truncate((char *)callback_data, 0) == 0);
}

// Truncating a file changes mtime and ctime.
TEST(StatTimesTest, FileTruncate) {
  char *file =
      TempPath_CreateFileWith(GetAbsoluteTestTmpdir(), "yaaass", 0666);

  CheckTimes(&file, StatTimesTest_FileTruncate_internal_lambda, file, AtimeEffect_Unchanged, MtimeEffect_Changed,
             CtimeEffect_Changed);

  TempPath_RemoveAndFreePath(file);
}

/*}  // namespace

}  // namespace testing
}  // namespace gvisor*/

GVISOR_MAKE_MAIN() {
#if !defined(YALIBCT_LIBC_DOESNT_HAVE_UTIME_NOW) && !defined(YALIBCT_LIBC_DOESNT_HAVE_UTIME_OMIT)
    StatTimesTest_FileUtimes();
#endif
    StatTimesTest_FileTruncate();
}
