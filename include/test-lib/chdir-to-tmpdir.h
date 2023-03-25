#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

char g_yalibct_chdir_to_tmpdir_tmpdir_template[] = "/tmp/yalibct.XXXXXX";

static inline void yalibct_chdir_to_tmpdir()
{
    assert(strcmp(g_yalibct_chdir_to_tmpdir_tmpdir_template, "/tmp/yalibct.XXXXXX") == 0);
    errno = 0;
    assert(mkdtemp(g_yalibct_chdir_to_tmpdir_tmpdir_template) == g_yalibct_chdir_to_tmpdir_tmpdir_template);
    assert(errno == 0);
    assert(chdir(g_yalibct_chdir_to_tmpdir_tmpdir_template) == 0);
    assert(errno == 0);
}

static inline void yalibct_chdir_to_tmpdir_remove_tmpdir()
{
    assert(strcmp(g_yalibct_chdir_to_tmpdir_tmpdir_template, "/tmp/yalibct.XXXXXX") != 0);
    assert(strncmp(g_yalibct_chdir_to_tmpdir_tmpdir_template, "/tmp/yalibct.", 13) == 0);
    assert(remove(g_yalibct_chdir_to_tmpdir_tmpdir_template) == 0);
}
