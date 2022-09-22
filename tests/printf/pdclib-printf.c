/* printf( const char *, ... )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include "test-deps/pdclib.h"
#include "test-lib/chdir-to-tmpdir.h"
#include <stdarg.h>
#include <stdio.h>

#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>

#define testprintf( stream, ... ) printf( __VA_ARGS__ )
#define _PDCLIB_FILEID "stdio/printf.c"

int main( void )
{
    yalibct_chdir_to_tmpdir();
    FILE * target;
    TESTCASE( ( target = freopen( testfile, "wb+", stdout ) ) != NULL );
#include "pdclib-printf_testcases.h"
    TESTCASE( fclose( target ) == 0 );
    TESTCASE( remove( testfile ) == 0 );
    return TEST_RESULTS;
}
