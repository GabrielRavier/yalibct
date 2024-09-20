#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___STRING
#include <sys/cdefs.h>
#else

#undef __STRING
#define __STRING(x) #x

#endif
