/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include "bs_cmd_line.h"
#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_utils.h"
#include "bs_oswrap.h"

/*
 * A default executable name and a post help message
 * We define empty ones in case the component does not care to do so
 * But normally a component would redefine these
 */
char executable_name[] __attribute__((weak)) ="";
void  __attribute__((weak)) component_print_post_help() {
  fprintf(stdout, "\n");
}
static char empty_string[] ="";
static char *overriden_executable_name = NULL;
static char *trace_prefix = empty_string;

static void (*post_help)(void) = component_print_post_help;

/*
 * Dynamically override the component post help function
 */
void bs_override_post_help(void (*new_f)(void)) {
  post_help = new_f;
}


/**
 * Check if <arg> is the option <option>
 * The accepted syntax is:
 *   * For options without a value following:
 *       [-[-]]<option>
 *   * For options with value:
 *       [-[-]]<option>{:|=}<value>
 *
 * Returns 0 if it is not, or a number > 0 if it is.
 * The returned number is the number of characters it went through
 * to find the end of the option including the ':' or '=' character in case of
 * options with value
 *
 */
int bs_is_option(const char *arg, const char *option, int with_value) {
  int of = 0;
  size_t to_match_len = strlen(option);

  if (arg[of] == '-') {
    of++;
  }
  if (arg[of] == '-') {
    of++;
  }

  if (!with_value) {
    if (strcmp(&arg[of], option) != 0) {
      return 0;
    } else {
      return of + to_match_len;
    }
  }

  while (!(arg[of] == 0 && *option == 0)) {
    if (*option == 0) {
      if ((arg[of] == ':') || (arg[of] == '=')) {
        of++;
        break;
      }
      return 0;
    }
    if (arg[of] != *option) {
      return 0;
    }
    of++;
    option++;
  }

  if (arg[of] == 0) { /* we need a value to follow */
    return 0;
  }
  return of;
}

/**
 * Check if arg matches the option <option><x>
 * withArgs means it shall be followed by a = or :
 * e.g.: -d0=10         ("d", withargs=true) => index 0
 *       -argsmodem0    ("argsmodem", withargs=false) => index 0
 *       -pp10:0.1      ("pp", withargs=true) => index 10
 *
 * Returns 0 if it is not, or a number > 0 if it is.
 *  The number is the number of characters it went thru to find the option (if there is any paramter it follows)
 *
 * argv can start with - or --
 */
int bs_is_multi_opt(const char *arg, const char *option, uint* index, int with_value) {
  int of = 0;
  size_t to_match_len = strlen(option);
  if (arg[of] == '-') {
    of++;
  }
  if (arg[of] == '-') { //we accept options with either 1 or 2 -
    of++;
  }

  if (strncmp(&arg[of],option,to_match_len) == 0) {
    of += to_match_len;

    { //get the index
      uint c = of;
      uint n=0;
      while ( ( arg[c] != 0 ) && ( arg[c]!= ':' ) && ( arg[c]!= '=' ) ){
        if ( ( arg[c] >= '0' ) && ( arg[c] <= '9') ) {
          n = n*10 + ( arg[c] - '0');
        } else { //we found something else than a number
          c = of;
          break;
        }
        c++;
      }
      if ( c == of ) { //we didnt read any number out
        of = 0;
      } else {
        of = c;
        *index = n;
      }
    } //end of getting the index

    //check if the option finishes here or not:
    if ( with_value ) {
      if ( ( arg[of] == ':' ) || ( arg[of] == '=' ) ) {
        of++;
        if ( arg[of] == 0 ) { //we need an option to follow
          of = 0;
        }
      } else {
        of = 0;
      }
    } else {
      if ( arg[of] != 0 ) { //we dont accept any extra characters
        of = 0;
      }
    }
  } else {
    of = 0;
  }

  return of;
}

/**
 * Return 1 if <arg> matches an accepted help option.
 * 0 otherwise
 *
 * Valid help options are [-[-]]{?|h|help}
 * with the h or help in any case combination
 */
int bs_is_help(const char *arg){
  if (arg[0] == '-') {
    arg++;
  }
  if (arg[0] == '-') {
    arg++;
  }
  if ((strcasecmp(arg, "?") == 0) ||
      (strcasecmp(arg, "h") == 0) ||
      (strcasecmp(arg, "help") == 0)) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Read out a the value of the option parameter from str, and store it into
 * <dest>
 * <type> indicates the type of paramter (and type of dest pointer)
 *   'b' : boolean
 *   's' : string (char *)
 *   'u' : unsigned integer
 *   'U' : 64 bit unsigned integer
 *   'i' : signed integer
 *   'I' : 64 bit signed integer
 *   'f'/'d' : *double* float
 *
 *  <long_d> is the long name of the option
 */
void bs_read_optionparam(const char* str, void *dest, const char type,
                         const char *long_d) {
  int error = 0;
  char *endptr;

  switch (type){
  case 'b':
    if (strcasecmp(str, "false") == 0) {
      *(bool *)dest = false;
      endptr = (char *)str + 5;
    } else if (strcmp(str, "0") == 0) {
      *(bool *)dest = false;
      endptr = (char *)str + 1;
    } else if (strcasecmp(str, "true") == 0) {
      *(bool *)dest = true;
      endptr = (char *)str + 4;
    } else if (strcmp(str, "1") == 0) {
      *(bool *)dest = true;
      endptr = (char *)str + 1;
    } else {
      error = 1;
    }
    break;
  case 's':
      *((char**)dest) = (char*)str;
      endptr = (char*)str + strlen(str);
      break;
    case 'u':
      *(uint32_t *)dest = strtoul(str, &endptr, 0);
      break;
    case 'U':
      *(uint64_t *)dest = strtoull(str, &endptr, 0);
      break;
    case 'i':
      *(int32_t *)dest = strtol(str, &endptr, 0);
      break;
    case 'I':
      *(int64_t *)dest = strtoll(str, &endptr, 0);
      break;
    case 'f':
    case 'd':
      *(double *)dest = strtod(str, &endptr);
      break;
    default:
      bs_trace_error_line("Coding error: type %c not acceptable for automatically read option\n", type);
      break;
  }

  if (!error && *endptr != 0) {
    error = 1;
  }

  if (error){
    bs_trace_error_line("Error reading option %s \"%s\"\nRun with --help for more information", long_d, str);
  }

  switch (type){
    case 's':
      bs_trace_raw(9,"%s%s set to %s\n", trace_prefix, long_d, *((char**)dest));
      break;
    case 'u':
      bs_trace_raw(9,"%s%s set to %u\n", trace_prefix, long_d, *((uint32_t*)dest));
      break;
    case 'U':
      bs_trace_raw(9,"%s%s set to %"PRIu64"\n", trace_prefix, long_d, *((uint64_t*)dest));
      break;
    case 'i':
      bs_trace_raw(9,"%s%s set to %i\n", trace_prefix, long_d, *((int32_t*)dest));
      break;
    case 'I':
      bs_trace_raw(9,"%s%s set to %"PRIi64"\n", trace_prefix, long_d, *((int64_t*)dest));
      break;
    case 'f':
    case 'd':
      bs_trace_raw(9,"%s%s set to %le\n", trace_prefix, long_d, *((double*)dest));
      break;
    default:
      break;
  }
}

/**
 * Initialize existing dest* to defaults based on type
 */
void bs_args_set_defaults(bs_args_struct_t args_struct[])
{
  int count = 0;

  while (args_struct[count].option != NULL) {

    if (args_struct[count].dest == NULL) {
      count++;
      continue;
    }

    switch ( args_struct[count].type ){
    case 0: //does not have an storage
      break;
    case 'b': //Boolean:
      *(bool*)args_struct[count].dest = false;
      break;
    case 's': //Ponter to string:
      *(char**)args_struct[count].dest = NULL;
      break;
    case 'u': //unsigned int
      *(unsigned int*)args_struct[count].dest = UINT_MAX;
      break;
    case 'U': //64 bit unsigned int
      *(uint64_t*)args_struct[count].dest = UINT64_MAX;
      break;
    case 'i': //integer
      *(int*)args_struct[count].dest = INT_MAX;
      break;
    case 'I':
      *(int64_t *)args_struct[count].dest = INT64_MAX;
      break;
    case 'd':
    case 'f': //double
      *(double*)args_struct[count].dest = NAN;
      break;
    case 'l': //list: We don't initialize it (theay are meant to be always manual)
      break;
    default:
      bs_trace_error_line("Coding error: type %c not known\n", args_struct[count].type);
      break;
    }
    count++;
  }
}

/**
 * For the help messages:
 * Generate a string containing how the option described by <args_s_el>
 * should be used
 *
 * The string is saved in <buf> which has been allocated <size> bytes by the
 * caller
 */
static void bs_gen_switch_syntax(char *buf, size_t size,
                                 bs_args_struct_t *args_s_el){
  int ret = 0;

  if (size <= 0) {
    return;
  }

  if ( args_s_el->is_mandatory == false ) {
    *buf++ = '[';
    size--;
  }

  if ( args_s_el->is_switch == true ) {
    ret = snprintf(buf, size, "-%s", args_s_el->option);
  } else {
    if ( args_s_el->type != 'l' ){
      ret = snprintf(buf, size, "-%s=<%s>", args_s_el->option, args_s_el->name);
    } else {
      ret = snprintf(buf, size, "-%s <%s>...", args_s_el->option, args_s_el->name);
    }
  }

  if (ret < 0) {
    bs_trace_error_line("Unexpected error in\n");
  }
  if (size - ret < 0) {
    /*
     * If we run out of space we can just stop,
     * this is not critical
     */
    return;
  }
  buf += ret;
  size -= ret;

  if ( args_s_el->is_mandatory == false ) {
    snprintf(buf, size,"] ");
  } else {
    snprintf(buf, size," ");
  }
}

/**
 * Print short list of avaliable switches
 */
void bs_args_print_switches_help(bs_args_struct_t args_struct[])
{
  int count = 0;
  int printed_in_line = strlen(_HELP_SWITCH) + 2;

  if ( overriden_executable_name ){
    printed_in_line += strlen(overriden_executable_name);
    fprintf(stdout, "%s %s ", overriden_executable_name, _HELP_SWITCH);
  } else {
    printed_in_line += strlen(executable_name);
    fprintf(stdout, "%s %s ", executable_name, _HELP_SWITCH);
  }

  while ( args_struct[count].option != NULL) {
    char stringy[_MAX_STRINGY_LEN];

    bs_gen_switch_syntax(stringy, _MAX_STRINGY_LEN,
                         &args_struct[count]);

    if ( printed_in_line + strlen(stringy) > _MAX_LINE_WIDTH ){
      fprintf(stdout,"\n");
      printed_in_line = 0;
    }

    fprintf(stdout,"%s", stringy);
    printed_in_line += strlen(stringy);
    count++;
  }

  fprintf(stdout,"\n");
}

/**
 * Print the long help message of the program
 */
void bs_args_print_long_help(bs_args_struct_t args_struct[]){
  int ret;
  int count = 0;
  int printed_in_line = 0;
  char stringy[_MAX_STRINGY_LEN];

  bs_args_print_switches_help(args_struct);

  fprintf(stdout, "\n %-*s:%s\n", _LONG_HELP_ALIGN-1,
    _HELP_SWITCH, _HELP_DESCR);

  while (args_struct[count].option != NULL) {
    int printed_right;
    char *toprint;
    int total_to_print;

    bs_gen_switch_syntax(stringy, _MAX_STRINGY_LEN, &args_struct[count]);

    ret = fprintf(stdout, " %-*s:",_LONG_HELP_ALIGN-1, stringy);
    printed_in_line = ret;
    printed_right = 0;
    toprint = args_struct[count].descript;
    total_to_print = strlen(toprint);
    ret = fprintf(stdout, "%.*s\n",
                  _MAX_LINE_WIDTH - printed_in_line,
                  &toprint[printed_right]);
    printed_right += ret - 1;

    while (printed_right < total_to_print) {
      fprintf(stdout, "%*s", _LONG_HELP_ALIGN, "");
      ret = fprintf(stdout, "%.*s\n",
              _MAX_LINE_WIDTH - _LONG_HELP_ALIGN,
              &toprint[printed_right]);
      printed_right += ret - 1;
    }
    count++;
  }
  post_help();
}

/**
 * Try to find if one argument is in the list (and it is not manual)
 * if it does, try to parse it and set dest accordingly, and return true
 * if it is not found, return false
 */
bool bs_args_parse_one_arg(char *argv, bs_args_struct_t args_struct[]){
  bool found = false;
  int offset;
  if ( bs_is_help(argv) ) {
    bs_args_print_long_help(args_struct);
    bs_trace_silent_exit(0);
  }

  int count = 0;
  while ( args_struct[count].option != NULL ){
    if ( ( !args_struct[count].manual ) &&
         ( offset = bs_is_option(argv, args_struct[count].option , !args_struct[count].is_switch) ) ){

      if ( args_struct[count].is_switch ){
        if ( args_struct[count].type == 'b' ){
          if ( args_struct[count].dest != NULL ) {
            *(bool*)args_struct[count].dest = true;
            bs_trace_raw(9,"%s%s set\n", trace_prefix, args_struct[count].name);
          }
        } else {
          bs_trace_error_line("Programming error: I only know how to automatically read boolean switches\n");
        }

      } else { //if not switch we need to read it
        if ( args_struct[count].dest != NULL ){
          bs_read_optionparam(&argv[offset],
              args_struct[count].dest,
              args_struct[count].type,
              args_struct[count].name);
        } else {
          bs_trace_warning_line("Programming error(?): while processign command line (%s) the destination pointer is NULL but the type is not manual\n", argv);
        }
      }
      if ( args_struct[count].call_when_found ){
        args_struct[count].call_when_found(argv, offset);
      }
      found = true;
      break;
    } //not manual and matches
    count++;
  }
  return found;
}

/**
 * Parse *all* provided arguments
 * Note that normally argv[0] is the program name, so you normally want to call bs_args_parse_cmd_line() instead
 */
void bs_args_parse_all_cmd_line(int argc, char *argv[], bs_args_struct_t args_struct[]){
  int i;
  bool found = false;
  for (i=0; i<argc; i++){ 
    found = bs_args_parse_one_arg(argv[i], args_struct);

    if ( !found ){
      bs_args_print_switches_help(args_struct);
      bs_trace_error_line("%sUnknown command line switch '%s'\n",trace_prefix,argv[i]);
    }
  }
}

/**
 * Try to parse all command line arguments of this program
 * Note that this loop cannot handle manual or list types
 * (in that case build your own loop calling bs_args_parse_one_arg directly)
 */
void bs_args_parse_cmd_line(int argc, char *argv[], bs_args_struct_t args_struct[]){
  bs_args_parse_all_cmd_line(argc-1, argv+1, args_struct); //Skip the program name
}

void bs_args_override_exe_name(char *name){
  overriden_executable_name = name;
}

void bs_args_set_trace_prefix(char *name){
  trace_prefix = name;
}
