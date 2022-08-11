#include <stdlib.h>
#include <assert.h>

void (*srand_ptr)(unsigned) = srand;

// The rand function computes a sequence of pseudo-random integers in the range 0 to RAND_MAX inclusive
static int checked_rand()
{
    int result = rand();
    assert(result >= 0 && result <= RAND_MAX);

    return (result);
}

// From glibc
// Test SPDX: LGPL-2.1-or-later
/* The C standard says that "If rand is called before any calls to
   srand have been made, the same sequence shall be generated as
   when srand is first called with a seed value of 1." */
static void initial_seq_srand_1()
{
    int i1 = checked_rand();
    int i2 = checked_rand();
    int i3 = checked_rand();

    srand(1);

    int j1 = checked_rand();
    int j2 = checked_rand();
    int j3 = checked_rand();

    assert(i1 == j1);
    assert(i2 == j2);
    assert(i3 == j3);
}

int main()
{
    initial_seq_srand_1();
}
