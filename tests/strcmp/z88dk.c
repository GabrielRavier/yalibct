




//#include "string_tests.h"
#include "test-deps/z88dk.h"
#include <string.h>


void strcmp_equal()
{
    Assert(strcmp("equal","equal") == 0, "Should be == 0");
}

void strcmp_less()
{
    Assert(strcmp("equal","notequal") < 0, "Should be < 0");
}

void strcmp_greater()
{
    Assert(strcmp("equal","EQUAL") > 0, "Should be > 0");
}


int main()
{
    suite_setup("Strcmp Tests");

    suite_add_test(strcmp_equal);
    suite_add_test(strcmp_less);
    suite_add_test(strcmp_greater);

    return suite_run();
}
