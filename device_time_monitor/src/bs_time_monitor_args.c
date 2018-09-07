/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "bs_time_monitor_args.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"

char executable_name[] = "bs_device_time_monitor";

void component_print_post_help(){
  fprintf(stdout,"\n"
          "This device just connects to a phy and prints the time as\n"
          "it passes (for ex. if you want to monitor long simulations)\n\n");
}

time_monitor_args_t *args_g;

static void cmd_trace_lvl_found(char * argv, int offset){
  bs_trace_set_level(args_g->verb);
}

static void cmd_gdev_nbr_found(char * argv, int offset){
  bs_trace_set_prefix_dev(args_g->global_device_nbr);
}

static double interval;
static void cmd_interval_found(char * argv, int offset){
  args_g->interval = interval;
}

/**
 * Check the arguments provided in the command line: set args based on it
 * or defaults, and check they are correct
 */
void bs_time_monitor_argparse(int argc, char *argv[], time_monitor_args_t *args)
{
  args_g = args;
  bs_args_struct_t args_struct[] = {
      BS_BASIC_DEVICE_2G4_FAKE_OPTIONS_ARG_STRUCT,
      {false, false , false, "interval", "int", 'f', (void*)&interval, cmd_interval_found, "Monitoring interval, in microseconds"},
      ARG_TABLE_ENDMARKER
  };

  bs_args_typical_dev_set_defaults((bs_basic_dev_args_t *)args, args_struct);
  args->interval  = 150*1e6;
  static char default_phy[] ="2G4";

  bs_args_parse_cmd_line(argc, argv, args_struct);

  bs_args_typical_dev_post_check((bs_basic_dev_args_t *)args, args_struct, default_phy);
}
