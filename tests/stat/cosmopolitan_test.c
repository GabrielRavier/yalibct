/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/

#include "test-deps/cosmopolitan.h"
#include "test-lib/chdir-to-tmpdir.h"
#include <sys/stat.h>

//STATIC_YOINK("zip_uri_support");

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  //ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(stat_010, testEmptyFile_sizeIsZero) {
  struct stat st;
  memset(&st, -1, sizeof(st));
  ASSERT_SYS(0, 0, close(creat("hi", 0644)));
  EXPECT_SYS(0, 0, stat("hi", &st));
  EXPECT_EQ(0, st.st_size);
  EXPECT_EQ(0, remove("hi"));
}

TEST(stat, enoent) {
  struct stat stat_buffer;
  ASSERT_SYS(ENOENT, -1, stat("hi", &stat_buffer));
  ASSERT_SYS(ENOENT, -1, stat("o/doesnotexist", &stat_buffer));
}

TEST(stat, enotdir) {
  struct stat stat_buffer;
  ASSERT_SYS(0, 0, close(creat("yo", 0644)));
  ASSERT_SYS(ENOTDIR, -1, stat("yo/there", &stat_buffer));
  EXPECT_EQ(0, remove("yo"));
}

#if 0

TEST(stat, zipos) {
  struct stat st;
  EXPECT_SYS(0, 0,
             stat("/zip/.python/test/"
                  "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                  &st));
  EXPECT_SYS(0, 0, stat("/zip", &st));
  EXPECT_SYS(0, 0, stat("/zip/", &st));
  EXPECT_SYS(0, 0, stat("/zip/.python", &st));
  EXPECT_SYS(0, 0, stat("/zip/.python/", &st));
}

static long Stat(const char *path, struct stat *st) {
  long ax, di, si, dx;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"(__NR_stat), "1"(path), "2"(st)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

static long Fstatat(const char *path, struct stat *st) {
  long ax, di, si, dx;
  register long r10 asm("r10") = 0;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx), "+r"(r10)
               : "0"(__NR_fstatat), "1"(AT_FDCWD), "2"(path), "3"(st)
               : "rcx", "r8", "r9", "r11", "memory", "cc");
  return ax;
}

BENCH(stat, bench) {
  struct stat st;
  union metastat ms;
  EXPECT_SYS(0, 0, makedirs(".python/test", 0755));
  EZBENCH2("__stat2cosmo", donothing, __stat2cosmo(&st, &ms));
  EXPECT_SYS(0, 0,
             touch(".python/test/"
                   "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                   0644));
  if (!IsWindows() && !IsFreebsd()) {
    EZBENCH2("stat syscall", donothing,
             Stat(".python/test/"
                  "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                  &st));
    EZBENCH2("fstatat syscall", donothing,
             Fstatat(".python/test/"
                     "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                     &st));
  }
  EZBENCH2("stat() fs", donothing,
           stat(".python/test/"
                "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                &st));
  EZBENCH2("stat() zipos", donothing,
           stat("/zip/.python/test/"
                "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                &st));
}

#endif

int main()
{
  yalibct_chdir_to_tmpdir();
  stat_010_testEmptyFile_sizeIsZero();
  stat_enoent();
  stat_enotdir();
  yalibct_chdir_to_tmpdir_remove_tmpdir();
}
