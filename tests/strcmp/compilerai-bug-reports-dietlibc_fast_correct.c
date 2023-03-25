#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    const char src[] = {64, 1, 0};
    const char dst[] = {64, 255, 0};
    int ret = strcmp(src, dst);
    assert(ret < 0);
    return 0;
}
