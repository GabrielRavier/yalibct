#pragma once

#include "test-lib/should-always-be-included.h"

#ifdef YALIBCT_LIBC_HAS_PRINTF

#include <stdio.h>

#else

#error "TODO: implement printf lol"

#endif
