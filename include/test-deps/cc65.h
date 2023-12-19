// Derived from code with this license:
/*****************************************************************************/
/*                                                                           */
/* (C) 2017 Christian Krueger                                                */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/

#pragma once

#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef COMMA
#define COMMA ,
#endif

#define TEST    int main(void) \
                {

#define ASSERT_AreEqual(a,b,c,d)            if ((a) != (b)) \
                                            {\
                                                printf("%s: ",__FILE__); \
                                                printf("Fail at line %d:\n",__LINE__);\
                                                printf(d);\
                                                printf("\n");\
                                                printf("Expected value: "c", but is "c"!\n", (a), (b));\
                                                exit(EXIT_FAILURE);\
                                            }

#define ASSERT_IsTrue(a,b)                  if (!(a)) \
                                            {\
                                                printf("%s: ",__FILE__); \
                                                printf("Fail at line %d:\n",__LINE__);\
                                                printf(b);\
                                                printf("\n");\
                                                printf("Expected status should be true but wasn't!\n");\
                                                exit(EXIT_FAILURE);\
                                            }

#define ENDTEST     return EXIT_SUCCESS; \
                }
