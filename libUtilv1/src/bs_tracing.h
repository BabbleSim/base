/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Utility functions for printing info/warning/error messages and exit execution.
 *
 * These functions provide an optional but recommended way of printing messages from
 * programs which use bsim. By using this functions you ensure both a consistent,
 * not-interleaved message output.
 *
 * Several types of traces can be done: Errors, Warnings, Info, Debug and "raw" messages.
 * By default, errors and warnings will be printed to the process stderr, the rest to stdout.
 *
 * Info, debug and "raw" messages have an associated verbosity level: an integer between 0 and 9.
 * Users have the option (thru the command line argument `-v=<verbosity_level>`) to select up to
 * which verbosity they want messages to be displayed. Any message over the selected verbosity
 * will be discarded.
 *
 * All messages will be prefixed with a small string given by the program
 * (typically 5 characters long).
 * For Physical layer simulation programs, this will be a short of the Phy name;
 * For devices typically "d_<%02i>:", where %02i will be the device number,
 * using at least 2 digits.
 *
 * Error and warning traces will, by default and when routed to a tty, be colored.
 * Coloring will be disabled by default when printing to a file or another process.
 * Coloring can be controlled with command line options.
 *
 * "Exit" traces can also be produced, this will print an error, and exit the program execution.
 * In this case the registered main exit cleanup function will be called.
 *
 * The main API for users are the bs_trace_{exit|error|warning|info|debug|raw}[_line][_time] macros.
 * The "_line" variants will print the file and line from which the trace originated together with the
 * message.
 * The "_time" variants will also print the current simulated time (as reported by the time function
 * registered with `bs_trace_register_time_function()` )
 * "_manual_time" versions exists only to support bizarre use cases, and it is recommended to not use
 * them in general.
 *
 * All these macros have printf() like semantics (after the verbosity level).
 */

#ifndef UTIL_BS_TRACING_H
#define UTIL_BS_TRACING_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include "bs_types.h"
#include "bs_utils.h"

typedef uint8_t (*main_cleanup_f)(void);
typedef bs_time_t (*time_f)(void);

/*
 * Disable color in traces
 *
 * This is an API meant for the controlling program.
 * Normal users of this functionality are not expected to call it.
 */
void bs_trace_disable_color(char * argv, int offset);

/*
 * Enable color in traces.
 * This will only enable color when printing to a tty.
 *
 * This is an API meant for the controlling program.
 * Normal users of this functionality are not expected to call it.
 */
void bs_trace_enable_color(char * argv, int offset);

/*
 * For color in traces enabled.
 * Even if not printing to a tty.
 *
 * This is an API meant for the controlling program.
 * Normal users of this functionality are not expected to call it.
 */
void bs_trace_force_color(char * argv, int offset);

/*
 * Is <file_number> connected to a tty.
 * Where file_number is 0 for stdout, 1 for stderr.
 *
 * returns: 1 if a tty, 0 otherwise.
 *
 * This is an API meant for the controlling program.
 * Normal users of this functionality are not expected to call it.
 */
int bs_trace_is_tty(int file_number);

/*
 * Set the tracing level.
 *
 * Normally this function will be only called from the command
 * from the command line argument parsing.
 */
void bs_trace_set_level(int new_trace_level);

/*
 * Will a message with a given verbosity level be printed or discarded
 *
 * Returns 1 if it will be printed, 0 otherwise.
 */
int bs_trace_will_it_be_traced(int this_message_trace_level);

/*
 * Register the function to be called from the exit() APIs to terminate
 * the program. If none is registered the libC standard exit() will be called.
 */
void bs_trace_register_cleanup_function(main_cleanup_f cleanup_f);

/*
 * Register a function which returns the current simulated time
 */
void bs_trace_register_time_function(time_f t_f);

/*
 * Set prefix for prints
 */
void bs_trace_set_prefix(const char* prefix);

/*
 * Set prefix for prints: Phy variant
 */
void bs_trace_set_prefix_phy(const char* prefix);

/*
 * Set prefix for prints: device variant
 */
void bs_trace_set_prefix_dev(int device_nbr);

/*
 * Exit without printing an error message
 */
void bs_trace_silent_exit(uint8_t code);

typedef enum {BS_TRACE_EXIT = 0, /*To Stdout, with EXIT prefix*/
              BS_TRACE_ERROR,    /*To Stderr, with ERROR prefix*/
              BS_TRACE_WARNING,  /*To Stderr, with WARNING prefix*/
              BS_TRACE_INFO,     /*To Stdout, with INFO prefix*/
              BS_TRACE_DEBUG,    /*To Stdout, with DEBUG prefix*/
              BS_TRACE_RAW,      /*To Stdout, no extra prefix */
              } base_trace_type_t;
typedef enum { BS_TRACE_NOTIME = 0, BS_TRACE_TIME_PROVIDED, BS_TRACE_AUTOTIME} base_trace_timed_type_t;

/*
 * Underlying functions for the bs_trace_ macros.
 *
 * It is recommended to use the bs_trace_ macros defined below instead
 */
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

#define bs_trace_exit(...)                  bs_trace_print(BS_TRACE_EXIT   ,NULL,    0,       5,BS_TRACE_NOTIME,       0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_exit_line(...)             bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_NOTIME,       0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_exit_line_time(...)        bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_exit_time_line(...)        bs_trace_print(BS_TRACE_EXIT   ,__FILE__,__LINE__,5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_exit_time(...)             bs_trace_print(BS_TRACE_EXIT   ,NULL,    0,       5,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE

#define bs_trace_error(...)                 bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_NOTIME,       0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_error_line(...)            bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_NOTIME,       0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_error_line_time(...)       bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_error_time_line(...)       bs_trace_print(BS_TRACE_ERROR  ,__FILE__,__LINE__,0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_error_time(...)            bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_AUTOTIME,     0,__VA_ARGS__); \
                                               BS_UNREACHABLE
#define bs_trace_error_manual_time(t,...)   bs_trace_print(BS_TRACE_ERROR  ,NULL,    0,       0,BS_TRACE_TIME_PROVIDED,t,__VA_ARGS__); \
                                               BS_UNREACHABLE

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

#ifdef __cplusplus
}
#endif

#endif /* UTIL_BS_TRACING_H */
