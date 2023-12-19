/* printf( const char *, ... )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include "test-deps/pdclib.h"
#include "test-lib/chdir-to-tmpdir.h"
#include "test-lib/portable-symbols/printf.h"
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
    FILE * target = freopen( testfile, "wb+", stdout );
    TESTCASE( target != NULL );
#include "pdclib-testcases.h"
    TESTCASE( fclose( target ) == 0 );
    TESTCASE( remove( testfile ) == 0 );

    yalibct_chdir_to_tmpdir_remove_tmpdir();

    return TEST_RESULTS;
}
