#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_LOG
#include <math.h>
#else

#include "test-lib/portable-symbols/frexp.h"
#include <math.h>
#include <errno.h>

#undef log
#define log yalibct_internal_log

// Very hard to determine correctness of simple implementations of this, so here's some numbers for later when we actually test this:
/*
  ./programs/emulators-and-shims/computers/posix/uwin/src/lib/libposix/log.c: 1244
  ./kernels/tuhs/oses/AUSAM/source/S/log.c: 1149
  ./libraries/libc/implementations/newlib/newlib/libm/common/log_data.c: 536
  ./libraries/sdl/main-lib/src/SDL_log.c: 515
  ./kernels/MINIX/minix3/lib/libm/noieee_src/n_log.c: 492
  ./kernels/NetBSD/lib/libm/noieee_src/n_log.c: 492
  ./kernels/LiteBSD/lib/libm/common_source/log.c: 486
  ./programs/programming-tools/compilers/open64/osprey/libm/mips/log.c: 462
  ./kernels/original-bsd/lib/libm/common_source/log.c: 460
  ./kernels/MINIX/minix3/lib/libm/src/b_log.c: 406
  ./kernels/NetBSD/lib/libm/src/b_log.c: 406
  ./kernels/FreeBSD/lib/msun/bsdsrc/b_log.c: 404
  ./kernels/sortix/libm/src/b_log.c: 402
  ./libraries/libc/implementations/cosmopolitan/libc/tinymath/log_data.c: 361
  ./programs/programming-tools/compilers/open64/osprey/libm/vlog.c: 359
  ./programs/programming-tools/compilers/ack/util/int/log.c: 353
  ./programs/programming-tools/compilers/OrangeC/src/clibs/math/log.c: 349
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_log_data.c: 347
  ./libraries/libc/implementations/musl/src/math/log_data.c: 328
  ./kernels/illumos-gate/usr/src/lib/libm/common/C/_TBL_log.c: 299
  ./kernels/illumos-gate/usr/src/lib/libm/common/C/log.c: 220
  ./libraries/libc/implementations/newlib/newlib/libm/common/pow_log_data.c: 209
  ./libraries/libc/implementations/glibc/math/w_log.c+./libraries/libc/implementations/glibc/math/w_log_template.c+./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_log.c: 8+45+142=195
  ./libraries/libc/implementations/glibc/sysdeps/ieee754/dbl-64/e_pow_log_data.c: 195
  ./kernels/sortix/libm/src/w_log.c+./kernels/sortix/libm/src/e_log.c: 44+136=180
  ./kernels/MINIX/minix3/lib/libm/src/w_log.c+./kernels/MINIX/minix3/lib/libm/src/e_log.c: 44+136=180
  ./kernels/NetBSD/lib/libm/src/w_log.c+./kernels/NetBSD/lib/libm/src/e_log.c: 44+136=180
  ./libraries/libc/implementations/newlib/newlib/libm/common/log.c: 179
  ./libraries/libc/implementations/newlib/newlib/libm/mathfp/s_log.c+./libraries/libc/implementations/newlib/newlib/libm/mathfp/s_logarithm.c: 28+132=160
  ./libraries/sdl/main-lib/src/libm/e_log.c: 152
  ./libraries/libc/implementations/cosmopolitan/libc/tinymath/log.c: 151
  ./libraries/libc/implementations/newlib/newlib/libm/math/e_log.c: 148
  ./kernels/MidnightBSD/lib/msun/src/e_log.c: 147
  ./kernels/FreeBSD/lib/msun/src/e_log.c: 147
  ./kernels/embeddable-linux-kernel-subset/libc/math/e_log.c: 146
  ./kernels/original-bsd/old/libm/libm/log.c: 146
  ./kernels/FUZIX/Library/libs/log.c: 139
  ./kernels/OpenBSD/lib/libm/src/e_log.c: 134
  ./programs/programming-tools/compilers/ack/modules/src/malloc/log.c: 133
  ./libraries/libc/implementations/musl/src/math/log.c: 112
  ./kernels/MINIX/minix3/lib/libm/noieee_src/n_log__L.c: 110
  ./kernels/NetBSD/lib/libm/noieee_src/n_log__L.c: 110
  ./libraries/libc/implementations/newlib/newlib/libm/common/sf_log.c: 100
  ./kernels/embeddable-linux-kernel-subset/libc/math/k_log.h: 100
  ./kernels/MidnightBSD/lib/msun/src/k_log.h: 100
  ./kernels/FreeBSD/lib/msun/src/k_log.h: 100
  ./programs/programming-tools/compilers/open-watcom-v2/bld/mathlib/c/log.c: 99
  ./libraries/libc/implementations/newlib/newlib/libm/math/ef_log.c: 94
  ./programs/programming-tools/compilers/ack/lang/cem/libcc.ansi/core/math/log.c: 81
  ./libraries/libc/implementations/newlib/newlib/libm/math/w_log.c: 79
  ./libraries/libc/implementations/plauger_libc/MATH/LOG.C+./libraries/libc/implementations/plauger_libc/MATH/XLOG.C: 7+71=78
  ./libraries/libc/implementations/XEOS-lib-c99/source/math/log.c: 76
  ./libraries/libc/implementations/newlib/winsup/cygwin/math/log.def.h: 69
  ./kernels/MINIX/minix12/src/lib/math/log.c: 67
  ./kernels/unix-v10/v10/libnm/log.s: 66
  ./programs/programming-tools/compilers/ack/lang/pc/libpc/log.c: 65
  ./kernels/MINIX/minix12/src/lib/libp/log.c: 65
  ./kernels/2.11BSD/usr/usr.lib/libom/log.c: 64
  ./kernels/original-bsd/old/libm/libom/log.c: 64
  ./kernels/plan9/sys/src/ape/lib/ap/math/log.c: 62
  ./libraries/libc/implementations/newlib/newlib/libm/math/wf_log.c: 62
  ./kernels/unix-v10/v10/libc/math/log.c: 62
  ./kernels/tuhs/oses/V9/libc/math/log.c: 62
  ./kernels/unix-v9/v9/libc/math/log.c: 62
  ./kernels/unix-v8/v8/usr/src/libc/math/log.c: 62
  ./kernels/venix/v7-usr/src/libm/log.c: 62
  ./kernels/tuhs/oses/V7M/src/libm/log.c: 62
  ./kernels/tuhs/oses/32V/usr/src/libm/log.c: 62
  ./kernels/tuhs/oses/V7/usr/src/libm/log.c: 62
  ./libraries/libc/implementations/glibc/math/w_log_compat.c: 61
  ./kernels/retrobsd-early/src/libm/log.c: 61
  ./kernels/retrobsd/src/libm/log.c: 61
  ./programs/programming-tools/compilers/ack/lang/cem/libcc/math/log.c: 59
*/

/*
 * (c) copyright 1989 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

#define YALIBCT_LOG_INTERNAL_POLYNOM1(x, a)  ((a)[1]*(x)+(a)[0])
#define YALIBCT_LOG_INTERNAL_POLYNOM2(x, a)  (YALIBCT_LOG_INTERNAL_POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define YALIBCT_LOG_INTERNAL_POLYNOM3(x, a)  (YALIBCT_LOG_INTERNAL_POLYNOM2((x),(a)+1)*(x)+(a)[0])
#define   YALIBCT_LOG_INTERNAL_M_1_SQRT2       0.70710678118654752440084436210484904

static inline double log(double x)
{
    /*      Algorithm and coefficients from:
            "Software manual for the elementary functions"
            by W.J. Cody and W. Waite, Prentice-Hall, 1980
    */
    static double a[] = {
        -0.64124943423745581147e2,
        0.16383943563021534222e2,
        -0.78956112887491257267e0
    };
    static double b[] = {
        -0.76949932108494879777e3,
        0.31203222091924532844e3,
        -0.35667977739034646171e2,
        1.0
    };

    double  znum, zden, z, w;
    int     exponent;

    if (x <= 0) {
        errno = EDOM;
        return 0;
    }

    x = frexp(x, &exponent);
    if (x > YALIBCT_LOG_INTERNAL_M_1_SQRT2) {
        znum = (x - 0.5) - 0.5;
        zden = x * 0.5 + 0.5;
    }
    else {
        znum = x - 0.5;
        zden = znum * 0.5 + 0.5;
        exponent--;
    }
    z = znum/zden; w = z * z;
    x = z + z * w * (YALIBCT_LOG_INTERNAL_POLYNOM2(w,a)/YALIBCT_LOG_INTERNAL_POLYNOM3(w,b));
    z = exponent;
    x += z * (-2.121944400546905827679e-4);
    return x + z * 0.693359375;
}

#endif
