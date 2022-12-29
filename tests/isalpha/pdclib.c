#include "test-deps/pdclib.h"
#include <ctype.h>

int main( void )
{
    TESTCASE( isalpha( 'a' ) );
    TESTCASE( isalpha( 'z' ) );
    TESTCASE( ! isalpha( ' ' ) );
    TESTCASE( ! isalpha( '1' ) );
    TESTCASE( ! isalpha( '@' ) );
    return TEST_RESULTS;
}
