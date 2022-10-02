/* This test needs runtime that provides stpcpy function.  */
/* { dg-do run { target *-*-linux* *-*-gnu* *-*-uclinux* } } */
/* { dg-options "-O2" } */

#define USE_GNU
#include "gcc-opt-12.c"
