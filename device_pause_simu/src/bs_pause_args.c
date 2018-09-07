/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "bs_pause_args.h"
#include "bs_tracing.h"
#include "bs_utils.h"

char executable_name[] = "bs_device_pause_simu";

void component_print_post_help(){
  fprintf(stdout,"\n"
    "This device pauses the simulation once for each element of the\n"
    "list provided from command line, while it awaits some user input\n\n"
  );
}

pause_args_t *args_g;

void cmd_trace_lvl_found(char * argv, int offset){
  bs_trace_set_level(args_g->verb);
}

void cmd_gdev_nbr_found(char * argv, int offset){
  bs_trace_set_prefix_dev(args_g->global_device_nbr);
}

/**
 * Check the arguments provided in the command line: set args based on it or
 * defaults, and check they are correct
 */
void bs_pause_argparse(int argc, char *argv[], pause_args_t *args)
{
  args_g = args;
  bs_args_struct_t args_struct[] = {
      BS_BASIC_DEVICE_2G4_FAKE_OPTIONS_ARG_STRUCT,
      { true, false , false, "times", "time", 'l', (void*)NULL, NULL, "Times at which the simulation will be stalled (up to " STR(MAX_NBR_PAUSES) ")"},
      ARG_TABLE_ENDMARKER
  };

  bs_args_typical_dev_set_defaults((bs_basic_dev_args_t *)args, args_struct);
  args->n_times = 0;
  static char default_phy[] ="2G4";

  for (int i=1; i<argc; i++){
    int offset;
    if ( !bs_args_parse_one_arg(argv[i], args_struct) ){

      if ( ( offset = bs_is_option(argv[i], "times", 0) ) ) {
        double time;
        while ( ( i + 1 < argc ) && ( argv[i+1][0] != '-' ) ) {
          i += 1;
          if ( sscanf(argv[i],"%lf",&time) != 1 ){
            bs_trace_error_line("Could not parse time entry nbr %i (%s)\n", args->n_times+1, argv[i]);
          }
          if (args->n_times >= MAX_NBR_PAUSES) {
            bs_trace_error_line("Too many pauses. Maximum is "STR(MAX_NBR_PAUSES)"\n");
          }
          args->times[args->n_times] = time;
          bs_trace_raw(9,"added time[%i] = %"PRItime" to list\n", args->n_times, args->times[args->n_times]);
          args->n_times += 1;
        }
      } else {
        bs_args_print_switches_help(args_struct);
        bs_trace_error_line("Unknown command line switch '%s'\n",argv[i]);
      }
    }
  }

  bs_args_typical_dev_post_check((bs_basic_dev_args_t *)args, args_struct, default_phy);
}
