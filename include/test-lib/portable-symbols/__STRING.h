#pragma once

#ifdef YALIBCT_LIBC_HAS___STRING
#include <sys/cdefs.h>
#else

#undef __STRING
#define __STRING(x) #x

#endif
