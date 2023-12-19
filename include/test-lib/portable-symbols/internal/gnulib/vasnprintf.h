#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static char *yalibct_internal_gnulib_vasnprintf(char *result_buffer, size_t *length_ptr, const char *format, va_list arguments)
{
    int first_result = -1;
    size_t initial_size = result_buffer != NULL ? *length_ptr : 0;

    first_result = vsnprintf(result_buffer, initial_size, format, arguments);
    if (first_result < 0)
        return NULL;

    *length_ptr = first_result + 1;
    if (first_result < initial_size)
        return result_buffer;

    char *big_enough_buffer = malloc(*length_ptr);
    int second_result = vsnprintf(big_enough_buffer, *length_ptr, format, arguments);
    assert(second_result == first_result);
    return (big_enough_buffer);
}
