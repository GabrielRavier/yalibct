#pragma once

#include "test-lib/assert-with-message.h"
#include "test-lib/compiler-features.h"
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wcomment")

#define __cdecl
#define __ms_va_list va_list
#define WINAPIV
#define ok YALIBCT_ASSERT_WITH_MESSAGE
#define broken(expr) (false)
#define win_skip(string)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define wine_dbgstr_w We aint got some magic function to convert from wide to narrow here, pls probably replace this with appropriate usage of %ls
#define __ms_va_start va_start
#define __ms_va_end va_end
#define START_TEST(name) int main()
#define todo_wine_if(expr)
enum {
    FALSE = 0,
    TRUE = 1,
};
#define __int64 long long
#define _Return_type_success_(count)
#define ok_int(expression, result) ok_dec(expression, result)
#define ok_dec_(file, line, expression, result) \
    do { \
        int _value = (expression); \
        int _result = (result); \
        ok_(file, line)(_value == _result, "Wrong value for '%s', expected: " #result " (%d), got: %d\n", \
           #expression, _result, _value); \
    } while (0)
#define ok_dec(expression, result)      ok_dec_(__FILE__, __LINE__, expression, result)
#define ok_(file, line)       (winetest_set_location(file, line), 0) ? (void)0 : winetest_ok
#define __winetest_cdecl
#define __winetest_va_list va_list
# define __winetest_va_start(list,arg) va_start(list,arg)
# define __winetest_va_end(list) va_end(list)
# define __winetest_file_line_prefix "%s:%d"

typedef wchar_t WCHAR;
typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;
typedef int BOOL;

static inline void winetest_set_location( const char* file, int line )
{
    /*tls_data* data=get_tls_data();
#if defined(WINETEST_MSVC_IDE_FORMATTING)
    data->current_file = file;
#else
    data->current_file=strrchr(file,'/');
    if (data->current_file==NULL)
        data->current_file=strrchr(file,'\\');
    if (data->current_file==NULL)
        data->current_file=file;
    else
        data->current_file++;
#endif
    data->current_line=line;*/
}

static inline void __winetest_cdecl winetest_printf( const char *msg, ... )
{
    //tls_data *data = get_tls_data();
    __winetest_va_list valist;

    assert(fprintf( stdout, /* __winetest_file_line_prefix */ "(unknown): "/* ,  data->current_file, data->current_line */ ) == 11);
    __winetest_va_start( valist, msg );
    assert(vfprintf( stdout, msg, valist ) >= 0);
    __winetest_va_end( valist );
}


static inline void __winetest_cdecl winetest_print_context( const char *msgtype )
{
    //tls_data *data = get_tls_data();
    unsigned int i;

    winetest_printf( "%s", msgtype );
    /*for (i = 0; i < data->context_count; ++i)
        fprintf( stdout, "%s: ", data->context[i] );*/
}


/*
 * Checks condition.
 * Parameters:
 *   - condition - condition to check;
 *   - msg test description;
 *   - file - test application source code file name of the check
 *   - line - test application source code file line number of the check
 * Return:
 *   0 if condition does not have the expected value, 1 otherwise
 */
static inline int winetest_vok( int condition, const char *msg, __winetest_va_list args )
{
    /*if (data->todo_level)
    {
        if (condition)
        {
            winetest_print_context( "Test succeeded inside todo block: " );
            vfprintf(stdout, msg, args);
            if ((data->nocount_level & 2) == 0)
            InterlockedIncrement(&todo_failures);
            return 0;
        }
        else
        {
            /* show todos even if traces are disabled*/
            /*if (winetest_debug > 0)*//*
            {
                winetest_print_context( "Test marked todo: " );
                vfprintf(stdout, msg, args);
            }
            if ((data->nocount_level & 1) == 0)
            InterlockedIncrement(&todo_successes);
            return 1;
        }
    }
    else
    {*/
        if (!condition)
        {
            winetest_print_context( "Test failed: " );
            assert(vfprintf(stdout, msg, args) >= 0);
            /*if ((data->nocount_level & 2) == 0)
            InterlockedIncrement(&failures);*/
            return 0;
        }
        else
        {
            /*if (report_success && (data->nocount_level & 1) == 0)
            {
                winetest_printf("Test succeeded\n");
            }
            if ((data->nocount_level & 1) == 0)
            InterlockedIncrement(&successes);*/
            return 1;
        }
    //}
}


static inline void __winetest_cdecl winetest_ok( int condition, const char *msg, ... )
{
    __winetest_va_list valist;

    __winetest_va_start(valist, msg);
    winetest_vok(condition, msg, valist);
    __winetest_va_end(valist);
}
