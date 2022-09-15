#pragma once

#ifdef YALIBCT_LIBC_HAS_EXP
#include <math.h>
#else

#include "test-lib/portable-symbols/ldexp.h"
#include "test-lib/portable-symbols/floor.h"
#include <errno.h>

#undef exp
#define exp yalibct_internal_exp

// Very hard to determine correctness of simple implementations of this, so here's some numbers for later when we actually test this:
/*
  ./kernels/reactos/ntoskrnl/ke/i386/exp.c: 1141
  ./programs/programming-tools/compilers/llvm-project/libc/AOR_v20.02/math/exp_data.c: 1121
  ./libraries/libc/implementations/newlib/newlib/libm/common/exp_data.c: 611
  ./programs/programming-tools/compilers/open64/osprey/libm/mips/exp.c: 385
  ./kernels/illumos-gate/usr/src/lib/libm/common/C/exp.c: 357
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libm/C/exp.c: 296
  ./kernels/reactos/ntoskrnl/ke/arm/exp.c: 271
  ./programs/programming-tools/compilers/open64/osprey/libm/vexp.c: 245
  ./kernels/MINIX/minix3/lib/libm/noieee_src/n_exp.c: 216
  ./kernels/NetBSD/lib/libm/noieee_src/n_exp.c: 216
  ./programs/programming-tools/compilers/OrangeC/src/clibs/math/exp.c: 210
  ./kernels/LiteBSD/lib/libm/common_source/exp.c: 203
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_exp_data.c: 196
  ./libraries/libc/implementations/uclibc-ng/libm/w_exp.c+./libraries/libc/implementations/uclibc-ng/libm/e_exp.c: 38+157=195
  ./libraries/sdl/main-lib/src/libm/e_exp.c: 191
  ./programs/programming-tools/compilers/llvm-project/libc/AOR_v20.02/math/exp.c: 177
  ./kernels/MidnightBSD/lib/msun/bsdsrc/b_exp.c: 177
  ./kernels/original-bsd/lib/libm/common_source/exp.c: 177
  ./libraries/libc/implementations/newlib/newlib/libm/common/exp.c: 173
  ./libraries/libc/implementations/cosmopolitan/libc/tinymath/exp.c: 173
  ./libraries/math/openlibm/bsdsrc/b_exp.c: 172
  ./libraries/math/openlibm/src/e_exp.c: 171
  ./kernels/embeddable-linux-kernel-subset/libc/math/e_exp.c: 171
  ./libraries/libc/implementations/newlib/newlib/libm/math/e_exp.c: 169
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_exp.c: 169
  ./kernels/littlekernel/external/lib/libm/e_exp.c: 166
  ./kernels/MidnightBSD/lib/msun/src/e_exp.c: 164
  ./kernels/FreeBSD/lib/msun/src/e_exp.c: 164
  ./kernels/sortix/libm/src/e_exp.c: 162
  ./kernels/MINIX/minix3/lib/libm/src/e_exp.c: 162
  ./kernels/NetBSD/lib/libm/src/e_exp.c: 162
  ./kernels/OpenBSD/lib/libm/src/e_exp.c: 159
  ./libraries/libc/implementations/newlib/winsup/cygwin/math/exp.def.h: 137
  ./kernels/MINIX/minix3/lib/libm/src/b_exp.c: 137
  ./kernels/NetBSD/lib/libm/src/b_exp.c: 137
  ./kernels/FUZIX/Library/libs/exp.c: 136
  ./kernels/LiteBSD/lib/libm/common_source/exp__E.c: 136
  ./kernels/MINIX/minix3/lib/libm/noieee_src/n_exp__E.c: 136
  ./kernels/NetBSD/lib/libm/noieee_src/n_exp__E.c: 136
  ./libraries/libc/implementations/musl/src/math/exp.c: 134
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libm/R/r_exp_.c: 134
  ./kernels/sortix/libm/src/b_exp.c: 133
  ./programs/programming-tools/compilers/llvm-project/libc/src/math/generic/exp_utils.cpp: 128
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libm/exp.c: 126
  ./kernels/FreeBSD/lib/msun/bsdsrc/b_exp.c: 126
  ./kernels/original-bsd/old/libm/libm/exp.c: 126
  ./libraries/libc/implementations/newlib/newlib/libm/mathfp/s_exp.c: 124
  ./programs/programming-tools/compilers/ack/lang/pc/libpc/exp.c: 124
  ./programs/programming-tools/compilers/open-watcom-v2/bld/mathlib/c/exp.c: 124
  ./kernels/OpenBSD/lib/libm/src/b_exp__D.c: 123
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libm/exp__E.c: 120
  ./kernels/original-bsd/old/libm/libm/exp__E.c: 120
  ./kernels/MINIX/minix12/src/lib/libp/exp.c: 118
  ./libraries/libc/implementations/newlib/newlib/libm/common/sf_exp.c: 112
  ./kernels/FreeBSD/lib/msun/src/k_exp.c: 111
  ./kernels/MidnightBSD/lib/msun/src/k_exp.c: 110
  ./kernels/original-bsd/lib/libm/common_source/exp__E.c: 110
  ./libraries/math/openlibm/src/k_exp.c: 108
  ./libraries/libc/implementations/newlib/newlib/libm/math/ef_exp.c: 102
  ./libraries/libc/implementations/newlib/newlib/libm/math/w_exp.c: 95
  ./libraries/libc/implementations/newlib/newlib/libm/mathfp/sf_exp.c: 91
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/liboldnm/exp.s: 88
  ./kernels/original-bsd/old/libm/liboldnm/exp.s: 88
  ./programs/programming-tools/compilers/ack/lang/cem/libcc.ansi/core/math/exp.c: 82
  ./kernels/unix-v10/v10/libnm/exp.s: 78
  ./kernels/unix-v10/v10/asm.libm/exp.s: 78
  ./kernels/tuhs/oses/SunOS-4.1.4/usr.lib/libsvidm/exp.c: 77
  ./kernels/tuhs/oses/Ultrix-3.1/src/libm/exp.c: 75
  ./libraries/libc/implementations/XEOS-lib-c99/source/math/exp.c: 74
  ./libraries/libc/implementations/newlib/newlib/libm/math/wf_exp.c: 73
  ./kernels/MINIX/minix12/src/lib/math/exp.c: 72
  ./programs/programming-tools/compilers/ack/lang/cem/libcc/math/exp.c: 69
  ./kernels/tuhs/oses/Ultrix-3.1/src/libm.v7/exp.c: 53
  ./kernels/sortix/libm/src/w_exp.c: 51
  ./kernels/MINIX/minix3/lib/libm/src/w_exp.c: 51
  ./kernels/NetBSD/lib/libm/src/w_exp.c: 51
  ./libraries/libc/implementations/glibc/math/w_exp_compat.c: 50
  ./libraries/libc/implementations/glibc/math/w_exp.c+./libraries/libc/implementations/glibc/math/w_exp_template.c: 8+41=49
  ./kernels/tuhs/oses/4.3BSD-UWisc/src/usr.lib/libom/exp.c: 47
  ./kernels/tuhs/oses/2.11BSD/src/usr.lib/libom/exp.c: 47
  ./kernels/2.11BSD/usr/usr.lib/libom/exp.c: 47
  ./kernels/original-bsd/old/libm/libom/exp.c: 47
  ./kernels/tuhs/oses/32V/usr/src/libm/exp.c: 45
  ./kernels/tuhs/oses/2.9BSD/usr/src/lib/m/exp.c: 45
  ./kernels/tuhs/oses/V7M/src/libm/exp.c: 45
  ./kernels/retrobsd/src/libm/exp.c: 45
  ./kernels/unix-v10/v10/libc/math/exp.c: 45
  ./kernels/tuhs/oses/V9/libc/math/exp.c: 45
  ./kernels/unix-v9/v9/libc/math/exp.c: 45
  ./kernels/tuhs/oses/V8/usr/src/libc/math/exp.c: 45
  ./kernels/unix-v8/v8/usr/src/libc/math/exp.c: 45
  ./kernels/venix/v7-usr/src/libm/exp.c: 45
  ./kernels/tuhs/oses/V7/usr/src/libm/exp.c: 45
  ./kernels/retrobsd-early/src/libm/exp.c: 44
  ./kernels/plan9/sys/src/ape/lib/ap/math/exp.c: 44
*/

#define yalibct_exp_internal_p0  .2080384346694663001443843411e7
#define yalibct_exp_internal_p1  .3028697169744036299076048876e5
#define yalibct_exp_internal_p2  .6061485330061080841615584556e2
#define yalibct_exp_internal_q0  .6002720360238832528230907598e7
#define yalibct_exp_internal_q1  .3277251518082914423057964422e6
#define yalibct_exp_internal_q2  .1749287689093076403844945335e4
#define yalibct_exp_internal_log2e  1.4426950408889634073599247
#define yalibct_exp_internal_sqrt2  1.4142135623730950488016887
#define yalibct_exp_internal_maxf  10000

static inline double exp(double arg)
{
    double fract, temp1, temp2, xsq;
    int ent;

    if(arg == 0)
        return 1;
    if(arg < -yalibct_exp_internal_maxf) {
        errno = ERANGE;
        return 0;
    }
    if(arg > yalibct_exp_internal_maxf) {
        errno = ERANGE;
        return HUGE_VAL;
    }
    arg *= yalibct_exp_internal_log2e;
    ent = floor(arg);
    fract = (arg-ent) - 0.5;
    xsq = fract*fract;
    temp1 = ((yalibct_exp_internal_p2*xsq+yalibct_exp_internal_p1)*xsq+yalibct_exp_internal_p0)*fract;
    temp2 = ((xsq+yalibct_exp_internal_q2)*xsq+yalibct_exp_internal_q1)*xsq + yalibct_exp_internal_q0;
    return ldexp(yalibct_exp_internal_sqrt2*(temp2+temp1)/(temp2-temp1), ent);
}

#endif
