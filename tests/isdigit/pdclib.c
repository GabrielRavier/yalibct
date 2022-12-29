#include "test-deps/pdclib.h"
#include <ctype.h>

int main( void )
{
    TESTCASE( isdigit( '0' ) );
    TESTCASE( isdigit( '9' ) );
    TESTCASE( ! isdigit( ' ' ) );
    TESTCASE( ! isdigit( 'a' ) );
    TESTCASE( ! isdigit( '@' ) );
    return TEST_RESULTS;
}
