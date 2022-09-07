#pragma once

#include <stdio.h>

#define main_test main_test(); int main() { main_test(); } void main_test

static inline char *gcc_tmpnam(char *s)
{
  char *ret = tmpnam (s);
  // Windows sometimes prepends a backslash to denote the current directory,
  // so swallow that if it occurs
  if (ret[0] == '\\')
    ++ret;
  return ret;
}
