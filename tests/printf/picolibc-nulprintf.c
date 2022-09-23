/*
 * Copyright (C) 2014 by ARM Ltd. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include "test-deps/newlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char m[8] = {'M','M','M','M','M','M','M','M'};

int main(void)
{
  char buf[20];
  assert(sprintf (buf, "%.*s\n", 8, m) == 9);
  CHECK(strcmp(buf, "MMMMMMMM\n") == 0);
  exit (0);
}
