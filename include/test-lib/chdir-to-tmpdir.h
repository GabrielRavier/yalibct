#pragma once

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

static inline void yalibct_chdir_to_tmpdir()
{
    char template[] = "/tmp/yalibct.XXXXXX";

    assert(mkdtemp(template) == template);
    assert(chdir(template) == 0);
}
