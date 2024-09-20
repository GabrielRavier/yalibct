#pragma once

#include "test-lib/should-always-be-included.h"

#ifndef YALIBCT_LIBC_DOESNT_HAVE_PRINTF

#include <stdio.h>

#else

#error "TODO: implement printf lol"

#endif
