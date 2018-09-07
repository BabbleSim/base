/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_tracing.h"
#include "bs_pc_base.h"
#include "bs_pause_args.h"

/**
 * This device pauses the simulation once for each element of the
 * list provided from command line, while it awaits some user input
 */

static pb_dev_state_t pb_dev_state = {0};

static uint8_t clean_up() {
  bs_trace_raw(8, "Cleaning up\n");
  pb_dev_disconnect(&pb_dev_state);
  return 0;
}

int main(int argc, char *argv[]) {
  pause_args_t args;

  bs_trace_register_cleanup_function(clean_up);
  bs_pause_argparse(argc, argv, &args);

  bs_trace_raw(9,"Connecting...\n");
  pb_dev_init_com(&pb_dev_state, args.device_nbr, args.s_id, args.p_id);

  bs_time_t now = 0;
  pb_wait_t wait_s;

  for (int i = 0 ; i < args.n_times ; i++) {
    if ( args.times[i] < now ) {
      bs_trace_error_line("The list of times needs to be ordered (index %i: %i < %i)\n",i, args.times[i], now);
    }
    wait_s.end = now = args.times[i];
    if ( pb_dev_request_wait_block(&pb_dev_state, &wait_s) == -1 ){
      bs_trace_raw(3,"We have been told to disconnect\n");
      break;
    } else {
      char ret;
      bs_trace_raw(2,"@%"PRItime" Press \"ENTER\" to continue...\n", now);
      fflush(stdout);
      ret = getc(stdin);
      if ( ret == EOF ) {
        bs_trace_warning_line("break in stdin. Are you running me in a script without feeding something to it?\n");
        break;
      }
    }
  }

  bs_trace_raw(9,"Disconnecting...\n");
  pb_dev_disconnect(&pb_dev_state);

  return 0;
}
