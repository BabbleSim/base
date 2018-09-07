/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_utils.h"
#include "bs_oswrap.h"
#include "bs_symbols.h"

static int is_a_tty[2] = {-1,-1}; //-1 = we do not know yet ; Indexed 0:stdout, 1:stderr

static int trace_level = 0;
static main_cleanup_f main_cleanup_fpr = NULL; //Function from the application which will be called on exit or error
static time_f time_fpr = NULL; //Function from the application to get the current time

#define MAX_PREFIX_LEN 40
static char prefix_s[MAX_PREFIX_LEN]="?_??:"; //In case somebody throws messages before initializing the prefix we initialize it to this (will happen if verbosity is high, during command line parsing)
static char prefix_s_color[MAX_PREFIX_LEN]="?_??:"; //like prefix_s, but with colors

void bs_trace_disable_color(char * argv, int offset){
  is_a_tty[0] = 0;
  is_a_tty[1] = 0;
}

void bs_trace_enable_color(char * argv, int offset){
  //In the next print it will be checked again if this is a tty or not
  is_a_tty[0] = -1;
  is_a_tty[1] = -1;
}

void bs_trace_force_color(char * argv, int offset){
  is_a_tty[0] = 1;
  is_a_tty[1] = 1;
}

void bs_trace_set_level(int new_trace_level) {
  trace_level = BS_MAX(new_trace_level,0);
}

int bs_trace_will_it_be_traced(int this_message_level) {
  return ( this_message_level <= trace_level );
}

void bs_trace_register_cleanup_function(main_cleanup_f cleanup_f) {
  main_cleanup_fpr = cleanup_f;
}

void bs_trace_register_time_function(time_f t_function) {
  time_fpr = t_function;
}

void bs_trace_set_prefix(const char* prefix) {
  strncpy(prefix_s, prefix, MAX_PREFIX_LEN-1);
  int size = BS_MIN(strlen(prefix), MAX_PREFIX_LEN-1);
  prefix_s[size] = 0;

  strncpy(prefix_s_color, prefix, MAX_PREFIX_LEN-1);
  size = BS_MIN(strlen(prefix), MAX_PREFIX_LEN-1);
  prefix_s_color[size] = 0;
}

void bs_trace_set_color_prefix(const char* prefix) {
  strncpy(prefix_s_color, prefix, MAX_PREFIX_LEN-1);
  int size = BS_MIN(strlen(prefix), MAX_PREFIX_LEN-1);
  prefix_s_color[size] = 0;
}

void bs_trace_set_prefix_phy(const char *phy_id) {
  char trace_prefix[strlen(phy_id) + 5];
  sprintf(trace_prefix, "p_%s:", phy_id);
  bs_trace_set_prefix(trace_prefix);
}

void bs_trace_set_prefix_dev(int dev_nbr) {
  static const char * const device_colortable[8] = {
      "\x1b[0;97;41m",
      "\x1b[0;97;42m",
      "\x1b[0;97;43m",
      "\x1b[0;97;44m",
      "\x1b[0;97;45m",
      "\x1b[0;97;46m",
      "\x1b[0;97;47m",
      "\x1b[0;97;40m"
  };

  char prefix[40]; //32 bit uint can have max 10 digits
  snprintf(prefix, 14 , "d_%02i:", dev_nbr);
  bs_trace_set_prefix(prefix);

  snprintf(prefix, 40, "%sd_%02i:\x1b[0;39m", device_colortable[dev_nbr & 0x7], dev_nbr);
  bs_trace_set_color_prefix(prefix);
}

static bs_time_t get_time(){
  if( time_fpr != NULL ){
    return time_fpr();
  } else {
    return TIME_NEVER;
  }
}

void bs_trace_silent_exit(uint8_t code){
  #if (_CS_TSYMBOLS_TRACE )
    cs_clear_Tsymbols();
  #endif
  uint8_t main_code = 0;
  if ( main_cleanup_fpr != NULL ){
    main_code = main_cleanup_fpr();
  }
  exit(BS_MAX(code, main_code));
}

static const char *base_trace_type_prefixes[] = {
  "EXIT:", "ERROR:", "WARNING:", "INFO:", "DEBUG:", ""
};

static const char *trace_type_esc_start[] = {
 "\x1b[0;39m", //EXIT     //reset all styles
 "\x1b[1;31m", //ERROR    //Foreground color = red, bold
 "\x1b[95m",   //WARNING  //Foreground color = magenta
 "\x1b[0;39m", //INFO     //reset all styles
 "\x1b[0;39m", //DEBUG    //reset all styles
 "\x1b[0;39m"  //RAW      //reset all styles
};

static const char trace_esc_end[] = "\x1b[0;39m"; //reset all styles

void bs_trace_vprint(base_trace_type_t type,
                     const char *caller_filename, unsigned int caller_line,
                     int this_message_trace_level,
                     base_trace_timed_type_t time_type, bs_time_t time,
                     const char *format, va_list variable_args){

  uint file_index = 0; //by default thru stdout
  if (( type == BS_TRACE_ERROR ) || ( type == BS_TRACE_WARNING )) {
    this_message_trace_level = 0; //we promote the message, so it is always printed
    file_index = 1; //errors and warnings thru stderr
  }
  if ( this_message_trace_level <= trace_level ) {
    FILE* fptrs[2] = {stdout, stderr};

    char time_s[20] = {0};
    if ( time_type > BS_TRACE_NOTIME ) {
      time_s[0] = ' ';
      time_s[1] = '@';
      if ( time_type == BS_TRACE_AUTOTIME ){
        time = get_time();
      }
      bs_time_to_str(&time_s[2], time);
    }

    if ( is_a_tty[file_index] == -1 ){
      is_a_tty[file_index] = isatty(fileno(fptrs[file_index]));
    }
    if ( is_a_tty[file_index] ){
      fprintf(fptrs[file_index],"%s%s",prefix_s_color, trace_type_esc_start[type]);
    } else {
      fprintf(fptrs[file_index],"%s",prefix_s);
    }
    fprintf(fptrs[file_index], "%s %s ", time_s, base_trace_type_prefixes[type]);
    if ( caller_filename != NULL ) {
      fprintf(fptrs[file_index], "(%s:%u): ", caller_filename, caller_line);
    }
    vfprintf(fptrs[file_index], format, variable_args);

    if ( is_a_tty[file_index] ) {
      fprintf(fptrs[file_index],"%s", trace_esc_end);
    }
  }

  if ( type == BS_TRACE_EXIT ) {
    bs_trace_silent_exit(0);
  } else if ( type == BS_TRACE_ERROR ) {
    bs_trace_silent_exit(255);
  }
}

void bs_trace_print(base_trace_type_t type,
                        const char *caller_filename, unsigned int caller_line,
                        int this_message_trace_level,
                        base_trace_timed_type_t time_type, bs_time_t time,
                        const char *format, ...){
  if ((type == BS_TRACE_ERROR) || (type == BS_TRACE_WARNING)) {
    this_message_trace_level = 0; //we promote the message, so it is always printed
  }
  if ((this_message_trace_level <= trace_level) || (type == BS_TRACE_EXIT)) {
    va_list variable_args;
    va_start(variable_args, format);
    bs_trace_vprint(type,
                        caller_filename, caller_line,
                        this_message_trace_level,
                        time_type, time,
                        format, variable_args);
    va_end(variable_args);
  }
}
