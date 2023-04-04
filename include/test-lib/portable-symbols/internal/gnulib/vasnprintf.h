#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static char *yalibct_internal_gnulib_vasnprintf(char *result_buffer, size_t *length_ptr, const char *format, va_list arguments)
{
    int first_result = -1;

    first_result = vsnprintf(result_buffer, result_buffer != NULL ? *length_ptr : 0, format, arguments);
    if (first_result < 0)
        return NULL;
    if (first_result < *length_ptr)
        return result_buffer;

    *length_ptr = first_result + 1;
    char *big_enough_buffer = malloc(*length_ptr);
    int second_result = vsnprintf(big_enough_buffer, *length_ptr, format, arguments);
    assert(second_result >= 0);
    assert(second_result < *length_ptr);
    return (big_enough_buffer);
}
