#include "string.h"
#include "assert.h"

int main()
{
	assert(strlen("hello")==5);
	assert(strlen("abcdefghijklmnopqrstuvwxyz")==26);
	return 0;
}
