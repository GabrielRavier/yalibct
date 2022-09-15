#pragma once

#ifdef YALIBCT_LIBC_HAS_POW
#include <math.h>
#else

#include "test-lib/portable-symbols/exp.h"
#include "test-lib/portable-symbols/log.h"
#include "test-lib/portable-symbols/floor.h"

#undef pow
#define pow yalibct_internal_pow

// Very hard to determine correctness of simple implementations of this, so here's some numbers for later when we actually test this:
/*
  ./programs/programming-tools/compilers/open64/osprey/libm/mips/pow.c: 1310
  ./programs/programming-tools/compilers/OrangeC/src/clibs/math/pow.c: 764
  ./libraries/libc/implementations/newlib/newlib/libm/common/pow.c: 395
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_pow.c: 390
  ./libraries/libc/implementations/cosmopolitan/libc/tinymath/pow.c: 383
  ./programs/programming-tools/compilers/llvm-project/libc/AOR_v20.02/math/pow.c: 381
  ./kernels/FreeBSD/contrib/arm-optimized-routines/math/pow.c: 380
  ./libraries/sdl/main-lib/src/libm/e_pow.c: 347
  ./libraries/libc/implementations/musl/src/math/pow.c: 343
  ./kernels/illumos-gate/usr/src/lib/libm/common/C/pow.c: 341
  ./programs/programming-tools/compilers/open64/osprey/libm/powtab.c: 334
  ./libraries/libc/implementations/newlib/newlib/libm/math/e_pow.c: 320
  ./libraries/math/openlibm/src/e_pow.c: 317
  ./kernels/embeddable-linux-kernel-subset/libc/math/e_pow.c: 317
  ./kernels/FUZIX/Library/libs/pow.c: 317
  ./kernels/MidnightBSD/lib/msun/src/e_pow.c: 314
  ./kernels/FreeBSD/lib/msun/src/e_pow.c: 314
  ./kernels/NetBSD/lib/libm/src/e_pow.c: 309
  ./kernels/littlekernel/external/lib/libm/e_pow.c: 304
  ./kernels/OpenBSD/lib/libm/src/e_pow.c: 304
  ./kernels/sortix/libm/src/e_pow.c: 303
  ./kernels/MINIX/minix3/lib/libm/src/e_pow.c: 303
  ./libraries/libc/implementations/uclibc-ng/libm/e_pow.c: 301
  ./libraries/libc/implementations/newlib/newlib/libm/math/ef_pow.c: 259
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libm/C/pow.c: 250
  ./libraries/libc/implementations/newlib/newlib/libm/common/sf_pow.c: 243
  ./kernels/MINIX/minix3/lib/libm/noieee_src/n_pow.c: 227
  ./kernels/NetBSD/lib/libm/noieee_src/n_pow.c: 227
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libm/pow.c: 226
  ./kernels/original-bsd/old/libm/libm/pow.c: 226
  ./libraries/libc/implementations/newlib/winsup/cygwin/math/pow.def.h: 219
  ./kernels/LiteBSD/lib/libm/common_source/pow.c: 215
  ./libraries/libc/implementations/newlib/newlib/libm/common/pow_log_data.c: 209
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libm/R/r_pow_.c: 206
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_pow_log_data.c: 195
  ./kernels/original-bsd/lib/libm/common_source/pow.c: 189
  ./programs/programming-tools/compilers/llvm-project/libc/AOR_v20.02/math/pow_log_data.c: 185
  ./kernels/FreeBSD/contrib/arm-optimized-routines/math/pow_log_data.c: 184
  ./libraries/libc/implementations/musl/src/math/pow_data.c: 180
  ./kernels/haiku/src/libs/mapm/mapm_pow.c: 178
  ./libraries/libc/implementations/newlib/newlib/libm/mathfp/s_pow.c: 154
  ./programs/programming-tools/compilers/ack/lang/cem/libcc.ansi/core/math/pow.c: 128
  ./libraries/libc/implementations/newlib/newlib/libm/mathfp/sf_pow.c: 125
  ./programs/programming-tools/compilers/open-watcom-v2/bld/mathlib/c/pow.c: 119
  ./libraries/libc/implementations/newlib/newlib/libm/math/w_pow.c: 117
  ./kernels/tuhs/oses/Ultrix-3.1/src/libape/pow.c: 113
  ./libraries/libc/implementations/plauger_libc/MATH/POW.C: 108
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libsvidm/pow.c: 98
  ./kernels/tuhs/oses/Ultrix-3.1/src/libm/pow.c: 96
  ./kernels/unix-v10/v10/cmd/cfront/libC/complex/pow.c: 92
  ./kernels/tuhs/oses/V9/cmd/cfront/libC/complex/pow.c: 92
  ./kernels/unix-v9/v9/cmd/cfront/libC/complex/pow.c: 92
  ./kernels/tuhs/oses/V8/usr/src/cmd/cfront/libC/complex/pow.c: 91
  ./kernels/unix-v8/v8/usr/src/cmd/cfront/libC/complex/pow.c: 91
  ./libraries/libc/implementations/newlib/newlib/libm/math/wf_pow.c: 84
  ./libraries/libc/implementations/uclibc-ng/libm/w_pow.c: 78
  ./programs/programming-tools/compilers/SmallerC/v0100/srclib/pow.c: 78
  ./kernels/illumos-gate/usr/src/lib/libmp/common/pow.c: 78
  ./libraries/libc/implementations/glibc/math/w_pow_compat.c: 77
  ./kernels/plan9/sys/src/ape/lib/ap/math/pow.c: 76
  ./libraries/libc/implementations/XEOS-lib-c99/source/math/pow.c: 75
  ./kernels/plan9/sys/src/libc/port/pow.c: 69
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libmp/pow.c: 66
  ./kernels/sortix/libm/src/w_pow.c: 62
  ./kernels/MINIX/minix3/lib/libm/src/w_pow.c: 62
  ./kernels/NetBSD/lib/libm/src/w_pow.c: 62
  ./kernels/tuhs/oses/2.9BSD/usr/src/lib/m/pow.c: 61
  ./libraries/libc/implementations/glibc/math/w_pow.c+./libraries/libc/implementations/glibc/math/w_pow_template.c: 8+52=60
  ./libraries/libc/implementations/newlib/newlib/libm/common/sf_pow_log2_data.c: 59
  ./programs/programming-tools/compilers/ack/lang/cem/libcc/math/pow.c: 59
  ./kernels/MINIX/minix12/src/lib/math/pow.c: 54
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libmp/pow.c: 50
  ./kernels/tuhs/oses/2.11BSD/src/usr.lib/libmp/pow.c: 50
  ./kernels/2.11BSD/usr/usr.lib/libmp/pow.c: 50
  ./kernels/tuhs/oses/Ultrix-3.1/src/libmp/pow.c: 48
  ./kernels/original-bsd/lib/libmp/pow.c: 48
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/liboldnm/pow.c: 47
  ./kernels/original-bsd/old/libm/liboldnm/pow.c: 47
  ./kernels/tuhs/oses/Ultrix-3.1/src/libm.v7/pow.c: 44
  ./libraries/libc/implementations/dietlibc-git-2022-09-09/git-repo/dietlibc/libm/pow.c: 42
  ./kernels/unix-v10/v10/libc/math/pow.c: 42
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libom/pow.c: 41
  ./kernels/tuhs/oses/2.11BSD/src/usr.lib/libom/pow.c: 41
  ./kernels/2.11BSD/usr/usr.lib/libom/pow.c: 41
  ./kernels/unix-v10/v10/libmp/pow.c: 41
  ./kernels/original-bsd/old/libm/libom/pow.c: 41
  ./kernels/tuhs/oses/32V/usr/src/libmp/pow.c: 40
  ./kernels/tuhs/oses/V8/usr/src/lib/libmp/pow.c: 40
  ./kernels/unix-v8/v8/usr/src/lib/libmp/pow.c: 40
  ./kernels/tuhs/oses/V7M/src/libmp/pow.c: 40
  ./kernels/tuhs/oses/V7/usr/src/libmp/pow.c: 40
  ./kernels/venix/v7-usr/src/libmp/pow.c: 40
  ./kernels/tuhs/oses/32V/usr/src/libm/pow.c: 36
  ./kernels/tuhs/oses/V8/usr/src/libc/math/pow.c: 36
  ./kernels/retrobsd-early/src/libm/pow.c: 36
  ./kernels/retrobsd/src/libm/pow.c: 36
  ./kernels/unix-v10/v10/libnm/pow.c: 36
  ./kernels/unix-v10/v10/asm.libm/pow.c: 36
  ./kernels/tuhs/oses/V9/libc/math/pow.c: 36
  ./kernels/unix-v9/v9/libc/math/pow.c: 36
  ./kernels/unix-v8/v8/usr/src/libc/math/pow.c: 36
  ./kernels/tuhs/oses/V7M/src/libm/pow.c: 36
  ./kernels/venix/v7-usr/src/libm/pow.c: 36
  ./kernels/tuhs/oses/V7/usr/src/libm/pow.c: 36
  ./kernels/Escape/source/lib/c/math/pow.c: 29
  ./libraries/libc/implementations/libc11/src/math/pow.c: 28
  ./programs/programming-tools/compilers/llvm-project/libc/AOR_v20.02/math/v_pow.c: 28
  ./kernels/FreeBSD/contrib/arm-optimized-routines/math/v_pow.c: 27
  ./libraries/libc/implementations/musl/src/math/pow_data.h: 22
  ./programs/programming-tools/compilers/z88dk/libsrc/math/cimpl/pow_valid.c: 21
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libm/D/d_pow_.c: 16
  ./programs/programming-tools/compilers/z88dk/libsrc/math/cimpl/pow.c: 15
*/

static inline double pow(double x, double y)
{
    if (y == 0.0) return 1.0;
    if (y == 1.0 ) return x;
    if (x <= 0.0) return 0.0;
    return exp(log(x)*y);
}

#endif
