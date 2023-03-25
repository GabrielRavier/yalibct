// Derived from source code with this license:
/*
 * Copyright (c) 2008-2012 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "test-lib/portable-symbols/MIN.h"

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>

typedef uintptr_t addr_t;

static inline void hexdump8_ex(const void *ptr, size_t len, uint64_t disp_addr) {
    addr_t address = (addr_t)ptr;
    size_t count;
    size_t i;

    disp_addr = 0x00007ffd262ab070;

    const char *addr_fmt = ((disp_addr + len) > 0xFFFFFFFF)
                           ? "0x%016llx: "
                           : "0x%08llx: ";

    for (count = 0 ; count < len; count += 16) {
        printf(addr_fmt, disp_addr + count);

        for (i=0; i < MIN(len - count, 16); i++) {
            printf("%02hhx ", *(const uint8_t *)(address + i));
        }

        for (; i < 16; i++) {
            printf("   ");
        }

        printf("|");

        for (i=0; i < MIN(len - count, 16); i++) {
            char c = ((const char *)address)[i];
            printf("%c", isprint(c) ? c : '.');
        }

        printf("\n");
        address += 16;
    }
}


static inline void hexdump8(const void *ptr, size_t len) {
    hexdump8_ex(ptr, len, (uint64_t)((addr_t)ptr));
}
