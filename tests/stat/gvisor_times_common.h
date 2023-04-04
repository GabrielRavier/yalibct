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

#include "test-deps/googletest.h"
#include "test-deps/gvisor.h"
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>

struct GetTime_Retval {
    struct timespec atime, mtime, ctime;
};

struct GetTime_Retval GetTime(const char *file) {
  struct stat statbuf = {};
  assert(stat(file, &statbuf) == 0);

  struct GetTime_Retval result = {};
#ifdef YALIBCT_LIBC_HAS_STRUCT_STAT_ST_ATIM
  result.atime = statbuf.st_atim;
#else
  result.atime.tv_sec = statbuf.st_atime;
  result.atime.tv_nsec = 0;
#endif
#ifdef YALIBCT_LIBC_HAS_STRUCT_STAT_ST_MTIM
  result.mtime = statbuf.st_mtim;
#else
  result.mtime.tv_sec = statbuf.st_mtime;
  result.mtime.tv_nsec = 0;
#endif
#ifdef YALIBCT_LIBC_HAS_STRUCT_STAT_ST_CTIM
  result.ctime = statbuf.st_ctim;
#else
  result.ctime.tv_sec = statbuf.st_ctime;
  result.ctime.tv_nsec = 0;
#endif
  return result;
}

enum AtimeEffect {
  AtimeEffect_Unchanged,
  AtimeEffect_Changed,
};

enum MtimeEffect {
  MtimeEffect_Unchanged,
  MtimeEffect_Changed,
};

enum CtimeEffect {
  CtimeEffect_Unchanged,
  CtimeEffect_Changed,
};

// cosmopolitan also declares a timespec_cmp function
#ifdef YALIBCT_WORK_AROUND_NAMESPACE_VIOLATIONS
#define timespec_cmp timespec_cmp_avoid_cosmopolitan_namespace_violation
#endif

static int timespec_cmp(struct timespec time1, struct timespec time2)
{
    return time1.tv_sec != time2.tv_sec ? (time1.tv_sec > time2.tv_sec) - (time1.tv_sec < time2.tv_sec) : (time1.tv_nsec > time2.tv_nsec) - (time1.tv_nsec < time2.tv_nsec);
}

// Tests that fn modifies the atime/mtime/ctime of path as specified.
void CheckTimes(char **path, void (*fn)(void *fn_callback_data), void *fn_callback_data,
                enum AtimeEffect atime_effect, enum MtimeEffect mtime_effect,
                enum CtimeEffect ctime_effect) {
  struct timespec atime, mtime, ctime;
  struct GetTime_Retval GetTime_result = GetTime(*path);
  atime = GetTime_result.atime;
  mtime = GetTime_result.mtime;
  ctime = GetTime_result.ctime;

  // FIXME(b/132819225): gVisor filesystem timestamps inconsistently use the
  // internal or host clock, which may diverge slightly. Allow some slack on
  // times to account for the difference.
  //
  // Here we sleep for 1s so that initial creation of path doesn't fall within
  // the before slack window.
  sleep(1);

  const time_t before = time(NULL) - 1;

  // Perform the op.
  fn(fn_callback_data);

  const time_t after = time(NULL) + 1;

  struct timespec atime2, mtime2, ctime2;
  GetTime_result = GetTime(*path);
  atime2 = GetTime_result.atime;
  mtime2 = GetTime_result.mtime;
  ctime2 = GetTime_result.ctime;

  if (atime_effect == AtimeEffect_Changed) {
    EXPECT_LE(before, atime2.tv_sec);
    EXPECT_GE(after, atime2.tv_sec);
    EXPECT_GT(timespec_cmp(atime2, atime), 0);
  } else {
    EXPECT_EQ(timespec_cmp(atime2, atime), 0);
  }

  if (mtime_effect == MtimeEffect_Changed) {
    EXPECT_LE(before, mtime2.tv_sec);
    EXPECT_GE(after, mtime2.tv_sec);
    EXPECT_GT(timespec_cmp(mtime2, mtime), 0);
  } else {
    EXPECT_EQ(timespec_cmp(mtime2, mtime), 0);
  }

  if (ctime_effect == CtimeEffect_Changed) {
    EXPECT_LE(before, ctime2.tv_sec);
    EXPECT_GE(after, ctime2.tv_sec);
    EXPECT_GT(timespec_cmp(ctime2, ctime), 0);
  } else {
    EXPECT_EQ(timespec_cmp(ctime2, ctime), 0);
  }
}
