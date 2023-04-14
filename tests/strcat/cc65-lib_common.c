#include "test-deps/cc65.h"
#include <string.h>

enum {
    SourceStringSize = 257                            // test correct page passing (>256)
};

static char SourceString[SourceStringSize+1];           // +1 room for terminating null
static char DestinationString[2*SourceStringSize+1];    // will contain two times the source buffer


TEST
{
    unsigned i,j;
    char*    p;

    for (i=0; i < SourceStringSize; ++i)
      SourceString[i] = (i%128)+1;

    SourceString[i] = 0;

    ASSERT_AreEqual((size_t)SourceStringSize, strlen(SourceString), "%zu", "Source string initialization or 'strlen()' problem!");

    /* Ensure empty destination string */
    DestinationString[0] = 0;

    ASSERT_AreEqual((size_t)0, strlen(DestinationString), "%zu", "Destination string initialization or 'strlen()' problem!");

    /* Test concatenation to empty buffer */

    strcat(DestinationString, SourceString);

    ASSERT_AreEqual((size_t)SourceStringSize, strlen(DestinationString), "%zu", "Unexpected string length while string concatenation to empty buffer!");

    /* Test concatenation to non empty buffer */

    p = strcat(DestinationString, SourceString);

    ASSERT_AreEqual((size_t)(2*SourceStringSize), strlen(DestinationString), "%zu", "Unexpected string length while string concatenation to non-empty buffer!");

    /* Test return value */

    ASSERT_IsTrue(p == DestinationString,"Invalid return value!");

    /* Test contents */

    for(j=0; j <2; ++j)
        for(i=0; i < SourceStringSize; ++i)
        {
            unsigned position = j*SourceStringSize+i;
            unsigned current = (unsigned char)DestinationString[position];
            unsigned expected = (i%128)+1;
            ASSERT_AreEqual(expected, current, "%u", "Unexpected destination buffer contents at position %u!\n" COMMA position);
        }
}
ENDTEST
