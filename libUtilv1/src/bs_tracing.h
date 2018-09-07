/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UTIL_TRACING_H
#define UTIL_TRACING_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include "bs_types.h"

typedef uint8_t (*main_cleanup_f)(void);
typedef bs_time_t (*time_f)(void);

void bs_trace_disable_color(char * argv, int offset);
void bs_trace_enable_color(char * argv, int offset);
void bs_trace_force_color(char * argv, int offset);

void bs_trace_set_level(int new_trace_level);
int  bs_trace_will_it_be_traced(int this_message_trace_level);
void bs_trace_register_cleanup_function(main_cleanup_f cleanup_f);
void bs_trace_register_time_function(time_f t_f);
void bs_trace_set_prefix(const char* prefix);
void bs_trace_set_prefix_phy(const char* prefix);
void bs_trace_set_prefix_dev(int device_nbr);
void bs_trace_silent_exit(uint8_t code);

typedef enum {BS_TRACE_EXIT = 0, /*To Stdout, with EXIT prefix*/
              BS_TRACE_ERROR,    /*To Stderr, with ERROR prefix*/
              BS_TRACE_WARNING,  /*To Stderr, with WARNING prefix*/
              BS_TRACE_INFO,     /*To Stdout, with INFO prefix*/
              BS_TRACE_DEBUG,    /*To Stdout, with DEBUG prefix*/
              BS_TRACE_RAW,      /*To Stdout, no extra prefix */
              } base_trace_type_t;
typedef enum { BS_TRACE_NOTIME = 0, BS_TRACE_TIME_PROVIDED, BS_TRACE_AUTOTIME} base_trace_timed_type_t;

//This is the underlying function all tracing functions below call:
void bs_trace_print(base_trace_type_t type,
                    const char *caller_filename, unsigned int caller_line,
                    int this_message_trace_level,
                    base_trace_timed_type_t time_type, bs_time_t time,
                    const char *format, ...);
void bs_trace_vprint(base_trace_type_t type,
                     const char *caller_filename, unsigned int caller_line,
                     int this_message_trace_level,
                     base_trace_timed_type_t time_type, bs_time_t time,
                     const char *format, va_list variable_args);
/*
 * We provide a set of shorcuts for convineance,
 *  all of them follow this convention bs_trace_<A>[_line][_time] , where:
 *    A = {exit|error|warning|info|debug|raw}
 *    line => with caller file name and line number automatically inserted
 *    time => with time automatically inserted
 */

#define bs_trace_exit(...)                  bs_trace_print(BS_TRACE_EXIT   ,NULL,    0,       5,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_exit_line(...)             bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_exit_line_time(...)        bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_exit_time_line(...)        bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_exit_time(...)             bs_trace_print(BS_TRACE_EXIT   ,NULL,    0,       5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)

#define bs_trace_error(...)                 bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_error_line(...)            bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_error_line_time(...)       bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_error_time_line(...)       bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_error_time(...)            bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_error_manual_time(t,...)   bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_TIME_PROVIDED,t,__VA_ARGS__)

#define bs_trace_warning(...)               bs_trace_print(BS_TRACE_WARNING,NULL,    0,       0,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_warning_line(...)          bs_trace_print(BS_TRACE_WARNING,__FILE__,__LINE__,0,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_warning_line_time(...)     bs_trace_print(BS_TRACE_WARNING,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_warning_time_line(...)     bs_trace_print(BS_TRACE_WARNING,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_warning_time(...)          bs_trace_print(BS_TRACE_WARNING,NULL,    0,       0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_warning_manual_time(t,...) bs_trace_print(BS_TRACE_WARNING  ,NULL,    0,       0,BS_TRACE_TIME_PROVIDED,t,__VA_ARGS__)
#define bs_trace_warning_manual_time_line(t,...) bs_trace_print(BS_TRACE_WARNING  ,__FILE__,__LINE__,       0,BS_TRACE_TIME_PROVIDED,t,__VA_ARGS__)

#define bs_trace_info(l,...)                bs_trace_print(BS_TRACE_INFO   ,NULL,    0,       l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_info_line(l,...)           bs_trace_print(BS_TRACE_INFO   ,__FILE__,__LINE__,l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_info_line_time(l,...)      bs_trace_print(BS_TRACE_INFO   ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_info_time_line(l,...)      bs_trace_print(BS_TRACE_INFO   ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_info_time(l,...)           bs_trace_print(BS_TRACE_INFO   ,NULL,    0,       l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)

#define bs_trace_debug(l,...)               bs_trace_print(BS_TRACE_DEBUG  ,NULL,    0,       l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_debug_line(l,...)          bs_trace_print(BS_TRACE_DEBUG  ,__FILE__,__LINE__,l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_debug_line_time(l,...)     bs_trace_print(BS_TRACE_DEBUG  ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_debug_time_line(l,...)     bs_trace_print(BS_TRACE_DEBUG  ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_debug_time(l,...)          bs_trace_print(BS_TRACE_DEBUG  ,NULL,    0,       l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)

#define bs_trace_raw(l,...)                 bs_trace_print(BS_TRACE_RAW    ,NULL,    0,       l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_raw_line(l,...)            bs_trace_print(BS_TRACE_RAW    ,__FILE__,__LINE__,l,BS_TRACE_NOTIME,       0,__VA_ARGS__)
#define bs_trace_raw_line_time(l,...)       bs_trace_print(BS_TRACE_RAW    ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_raw_time_line(l,...)       bs_trace_print(BS_TRACE_RAW    ,__FILE__,__LINE__,l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_raw_time(l,...)            bs_trace_print(BS_TRACE_RAW    ,NULL,    0,       l,BS_TRACE_AUTOTIME,     0,__VA_ARGS__)
#define bs_trace_raw_manual_time(l,t,...)   bs_trace_print(BS_TRACE_RAW    ,NULL,    0,       l,BS_TRACE_TIME_PROVIDED,t,__VA_ARGS__)
//Note: the "manual" versions exists only to support bizarre use cases, better do not use them
//there is very few cases in which they would really be needed

#ifdef __cplusplus
}
#endif

#endif
