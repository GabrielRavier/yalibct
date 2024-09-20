#pragma once

#ifndef YALIBCT_DOESNT_HAVE_SYSTEM_HEDLEY_H
#include <hedley.h>
#else
#include "test-lib/hedley-local-copy.h"
#endif

#ifndef YALIBCT_LIBC_DOESNT_HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>

// dietlibc is mega stupid and decides to #define __noinline__ so we have to do this horribleness
#ifdef __noinline__
// This test is copied straight from hedley.h
#if \
  HEDLEY_HAS_ATTRIBUTE(noinline) || \
  HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
  HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
  HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  (HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
  (HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
  (HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
  (HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
  HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
  HEDLEY_IAR_VERSION_CHECK(8,10,0)
#undef HEDLEY_NEVER_INLINE
#define HEDLEY_NEVER_INLINE __attribute__((noinline)) // Modified to use noinline instead of __noinline__... let's hope nobody #defines noinline...
#endif
#endif
#endif
