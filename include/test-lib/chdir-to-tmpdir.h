#pragma once

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

static inline void gravier_tests_chdir_to_tmpdir()
{
    char template[] = "/tmp/gravier-tests.XXXXXX";

    assert(mkdtemp(template) == template);
    assert(chdir(template) == 0);
}
