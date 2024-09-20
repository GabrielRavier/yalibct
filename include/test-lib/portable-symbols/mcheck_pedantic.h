#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_MCHECK_PEDANTIC
#include <mcheck.h>
#else

#include <assert.h>
#include <stddef.h>

static inline void mcheck_pedantic(void *abortfunc)
{
    assert(abortfunc == NULL);
}

#endif
