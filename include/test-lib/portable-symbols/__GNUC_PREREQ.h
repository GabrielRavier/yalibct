#pragma once

#ifdef YALIBCT_LIBC_HAS___GNUC_PREREQ
#include <features.h>
#else

#undef __GNUC_PREREQ
#define __GNUC_PREREQ(major, minor) HEDLEY_GNUC_VERSION_CHECK((major), (minor), 0)

#endif
