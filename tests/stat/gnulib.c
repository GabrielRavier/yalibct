#include "test-lib/chdir-to-tmpdir.h"
#include <sys/stat.h>

int gnulib_test()
{
    int result = 0;
    struct stat st;
    if (!stat ("conftest.tmp/", &st))
        result |= 1;
#if 1//HAVE_LSTAT
    if (!stat ("conftest.lnk/", &st))
        result |= 2;
#endif
    return result;
}

int main()
{
    yalibct_chdir_to_tmpdir();

    FILE *fp = fopen("conftest.tmp", "w");
    assert(fp != NULL);
    assert(fclose(fp) == 0);
    assert(symlink("conftest.tmp", "conftest.lnk") == 0);

    int result = gnulib_test();

    assert(remove("conftest.lnk") == 0);
    assert(remove("conftest.tmp") == 0);
    yalibct_chdir_to_tmpdir_remove_tmpdir();
    return result;
}
