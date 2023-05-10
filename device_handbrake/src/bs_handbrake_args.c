/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include "bs_handbrake_args.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"

char executable_name[] = "bs_device_handbrake";
void component_print_post_help(){
  fprintf(stdout,
"\nThis device slows down a simulation close to a ratio of the real time\n"
"By default it will slow it down to real time speed\n"
"The ratio between simulation time and real time can be set with -r=<real_time_ratio>\n"
"\n"
"Note1: This device actually _stalls_ the simulation every <poke_period>.\n"
"\n"
"Note2: Poking incurres in quite a lot of overhead, it will be difficult \n"
"to keep realtimeness if the poking is 2ms or less.\n"
"\n"
"Note3: The simulation will run for one poke period before the handbrake starts.\n"
"So expect to see simulations to last approx. 1 poke period less than specified\n"
"\n\n");
}

handbrake_args_t *args_g;

static void cmd_trace_lvl_found(char * argv, int offset){
  bs_trace_set_level(args_g->verb);
}

static void cmd_gdev_nbr_found(char * argv, int offset){
  bs_trace_set_prefix_dev(args_g->global_device_nbr);
}

static double poke_period;
static void cmd_poke_period_found(char * argv, int offset){
  args_g->poke_period = poke_period;
}

/**
 * Check the arguments provided in the command line: set args based on it or
 * defaults, and check they are correct
 */
void bs_handbrake_argparse(int argc, char *argv[], handbrake_args_t *args)
{

  args_g = args;
  bs_args_struct_t args_struct[] = {
      BS_BASIC_DEVICE_2G4_FAKE_OPTIONS_ARG_STRUCT,
      { false, false , false, "pp", "poke_period",    'f', (void*)&poke_period,    cmd_poke_period_found, "Period in which the simulation will be stalled (50e3 =50ms)"},
      { false, false , false, "r", "real_time_ratio", 'f', (void*)&(args->real_time_ratio),NULL, "Real timeness ratio (1); < 1: slower than real time; > 1: faster than real time"},
      ARG_TABLE_ENDMARKER
  };

  bs_args_typical_dev_set_defaults((bs_basic_dev_args_t *)args, args_struct);
  args->poke_period = 50e3;
  args->real_time_ratio = 1.0;
  static char default_phy[] ="2G4";

  bs_args_parse_cmd_line(argc, argv, args_struct);

  bs_args_typical_dev_post_check((bs_basic_dev_args_t *)args, args_struct, default_phy);
}
