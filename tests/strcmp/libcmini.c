#include "test-lib/portable-symbols/memchr.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void testcmp(const char* s1, const char* s2, int expected_result)
{
    int cmp = strcmp(s1, s2);
    if (cmp>0) {
        assert(expected_result > 0);
    } else if (cmp<0) {
        assert(expected_result < 0);
    } else {
        assert(expected_result == 0);
    }
}

void testmemchr(void)
{
    char tb[] = "abcdef\0ghijkl";

    assert(memchr(tb, 'a', 10) == tb);
    assert(memchr(tb, 'g', 10) == tb + 7);
}

int main(void)
{
    testcmp("abc", "def", -1);
    testcmp("def", "def", 0);
    testcmp("def", "abc", 1);

    testcmp("abc", "", 1);
    testcmp("abc", "ab", 1);
    testcmp("", "abc", -1);
    testcmp("ab", "abc", -1);

    testmemchr();

    return 0;
}
