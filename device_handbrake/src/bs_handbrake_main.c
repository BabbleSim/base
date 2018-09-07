/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <time.h>
#include <stdint.h>
#include "bs_tracing.h"
#include "bs_pc_base.h"
#include "bs_types.h"
#include "bs_handbrake_args.h"

/**
 * This device slows down a simulation close to a given ratio of the real time
 */
pb_dev_state_t pb_dev_state = {0};

static uint8_t clean_up() {
  bs_trace_raw(8,"Cleaning up\n");
  pb_dev_disconnect(&pb_dev_state);
  return 0;
}

int main(int argc, char *argv[]) {
  handbrake_args_t args;

  bs_trace_register_cleanup_function(clean_up);
  bs_handbrake_argparse(argc, argv, &args);

  bs_trace_raw(9,"Connecting...\n");
  pb_dev_init_com(&pb_dev_state, args.device_nbr, args.s_id, args.p_id);

  uint Stop = 0;
  pb_wait_t wait_s;
  struct timespec tv;

  //we first wait 1us to enable all devices to do their basic initialization
  wait_s.end = args.poke_period;
  if ( pb_dev_request_wait_block(&pb_dev_state, &wait_s) == -1 ){
    bs_trace_raw(3,"We have been terminated\n");
    Stop = 1;
  }

  clock_gettime(CLOCK_MONOTONIC, &tv);
  bs_time_t Expected_time = tv.tv_sec*1e6 + tv.tv_nsec/1000;

  while ( !Stop ) {
    wait_s.end += args.poke_period;
    if ( pb_dev_request_wait_block(&pb_dev_state, &wait_s) == -1 ){
      bs_trace_raw(3,"We have been terminated\n");
      break;
    }
    Expected_time += (bs_time_t)(( (double)args.poke_period ) / args.real_time_ratio);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    bs_time_t RealCurrentTime = tv.tv_sec*1e6 + tv.tv_nsec/1000;
    int64_t diff = Expected_time - RealCurrentTime;

    bs_trace_raw(7,"Diff = %li\n",diff);
    if ( diff > 0 ){
      struct timespec requested_time;
      struct timespec remaining;

      requested_time.tv_sec = diff / 1e6;
      requested_time.tv_nsec = ( diff - requested_time.tv_sec*1e6 )*1e3;
      int s = nanosleep(&requested_time, &remaining);
      if ( s == -1 ){
        bs_trace_warning_line("Interrupted or error\n");
        break;
      }
      bs_trace_raw(6,"@%"PRItime" Stalled until real time = %"PRIuMAX"\n", wait_s.end, (uintmax_t)Expected_time);
    }
  }

  bs_trace_raw(9,"Disconnecting...\n");
  pb_dev_disconnect(&pb_dev_state);

  bs_trace_silent_exit(0);
}
