#pragma once

#ifdef YALIBCT_LIBC_HAS_MCHECK_PEDANTIC
#include <mcheck.h>
#else

#include <assert.h>
#include <stddef.h>

static inline void mcheck_pedantic(void *abortfunc)
{
    assert(abortfunc == NULL);
}

#endif
