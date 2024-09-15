#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>

int main (void)
{
	printf ("'%08x' '%20.5d' '%20s'\n", 0x1998, 123, "Hello, word!");
}
