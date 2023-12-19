/*
 * Copyright (C) 2014 by ARM Ltd. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const char m[8] = {'M','M','M','M','M','M','M','M'};

int main()
{
    assert(printf ("%.*s\n", 8, m) == 9);
  exit (0);
}
