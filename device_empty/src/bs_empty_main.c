/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_cmd_line.h"
#include "bs_cmd_line_typical.h"
#include "bs_pc_base.h"

typedef bs_basic_dev_args_t empty_args_t;

char executable_name[] = "bs_device_empty";
void component_print_post_help(){
  fprintf(stdout,"\n"
          "This device just connects to a phy and disconnects, effectively\n"
          "using up one of its interfaces\n\n");
}

empty_args_t args;

void cmd_trace_lvl_found(char * argv, int offset){
  bs_trace_set_level(args.verb);
}

void cmd_gdev_nbr_found(char * argv, int offset){
  bs_trace_set_prefix_dev(args.global_device_nbr);
}

/**
 * Check the arguments provided in the command line: set args based on it or
 * defaults, and check they are correct
 */
void bs_empty_argparse(int argc, char *argv[], empty_args_t *args)
{
  bs_args_struct_t args_struct[] = {
      BS_BASIC_DEVICE_2G4_FAKE_OPTIONS_ARG_STRUCT,
      ARG_TABLE_ENDMARKER
  };

  bs_args_typical_dev_set_defaults((bs_basic_dev_args_t *)args, args_struct);
  static char default_phy[] ="2G4";

  bs_args_parse_cmd_line(argc, argv, args_struct);

  bs_args_typical_dev_post_check((bs_basic_dev_args_t *)args, args_struct, default_phy);
}

static pb_dev_state_t pcom_dev_state;

static uint8_t clean_up() {
  bs_trace_raw(8,"Cleaning up\n");
  pb_dev_clean_up(&pcom_dev_state);
  return 0;
}

/**
 * Handler for SIGTERM and SIGINT
 * We do not need to do anything in this empty device
 * The signal will cause any blocking libPhyCom op to return an error
 */
static void signal_end_handler(int sig)
{
  bs_trace_raw(2,"Signal \"%s\" received\n", strsignal(sig));
}

/**
 * This device just connects to a phy and disconnects, effectively
 * using up one of the phy interfaces
 */
int main(int argc, char *argv[]) {
  bs_trace_register_cleanup_function(clean_up);
  bs_set_sig_term_handler(signal_end_handler, (int[]){SIGTERM, SIGINT}, 2);

  bs_empty_argparse(argc, argv, &args);

  pb_dev_init_com(&pcom_dev_state, args.device_nbr, args.s_id, args.p_id);
  pb_dev_disconnect(&pcom_dev_state);
  return 0;
}
