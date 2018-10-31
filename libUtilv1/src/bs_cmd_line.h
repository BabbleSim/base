/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_CMD_LINE_H
#define BS_CMD_LINE_H

#include <stdbool.h>
#include "bs_oswrap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _MAX_LINE_WIDTH 120 /*Total width of the help message*/
/* Horizontal alignment of the 2nd column of the help message */
#define _LONG_HELP_ALIGN 30


#define _MAXOPT_SWITCH_LEN  32 /* Maximum allowed length for a switch name */
#define _MAXOPT_NAME_LEN    32 /* Maximum allowed length for a variable name */

#define _HELP_SWITCH  "[-h] [--h] [--help] [-?]"
#define _HELP_DESCR   "Print help"

#define _MAX_STRINGY_LEN (_MAXOPT_SWITCH_LEN+_MAXOPT_NAME_LEN+2+1+2+1)


/**
 * Prototype for a callback function when an option is found:
 * inputs:
 *  argv: Whole argv[i] option as received in main
 *  offset: Offset to the end of the option string
 *          (including a possible ':' or '=')
 *  If the option had a value, it would be placed in &argv[offset]
 */
typedef void (*option_found_callback_f)(char *argv, int offset);

/*
 * Structure defining each command line option
 */
typedef struct {
  /*
   * if manual is set bs_args_parse_*() will ignore it except for
   * displaying it the help messages and initializing <dest> to its
   * default
   */
  bool manual;
  /* For help messages, should it be wrapped in "[]" */
  bool is_mandatory;
  /* It is just a switch: it does not have something to store after */
  bool is_switch;
  /* Option name we search for: --<option> */
  char *option;
  /*
   * Name of the option destination in the help messages:
   * "--<option>=<name>"
   */
  char *name;
  /* Type of option (see bs_read_optionparam()),
   * including also l for lists & b for booleans */
  char type;
  /* Pointer to where the read value will be stored (may be NULL) */
  void *dest;
  /* Optional callback to be called when the switch is found */
  option_found_callback_f call_when_found;
  /* Long description for the help messages */
  char *descript;
} bs_args_struct_t;

#define ARG_TABLE_ENDMARKER \
    {false, false, false, NULL, NULL, 0, NULL, NULL, NULL}

int bs_is_option(const char *arg, const char *option, int with_value);
int bs_is_multi_opt(const char *arg, const char *option, uint* index, int with_value);
int bs_is_help(const char *arg);
void bs_read_optionparam(const char* str, void *dest, const char type, const char *long_d);

void bs_args_set_defaults(bs_args_struct_t args_struct[]);
void bs_args_print_switches_help(bs_args_struct_t args_struct[]);
void bs_args_print_long_help(bs_args_struct_t args_struct[]);
void bs_args_parse_cmd_line(int argc, char *argv[], bs_args_struct_t args_struct[]);
void bs_args_parse_all_cmd_line(int argc, char *argv[], bs_args_struct_t args_struct[]);
bool bs_args_parse_one_arg(char *argv, bs_args_struct_t args_struct[]);
void bs_args_override_exe_name(char *name);
void bs_args_set_trace_prefix(char *name);
void bs_override_post_help(void (*new_f)(void));

//Users can define these 2, so they will be used in the help message:
//  char executable_name[];
//  void component_print_post_help();

#ifdef __cplusplus
}
#endif

#endif
