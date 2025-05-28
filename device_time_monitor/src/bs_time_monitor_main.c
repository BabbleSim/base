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
#include "bs_time_monitor_args.h"

/**
 * This device monitors the running speed of the simulation
 */

/*
 * How many waits we will queue before waiting for the first response
 * Queuing them, reduces the overhead of the monitoring
 */
#define QUEUE_DEPTH 10

static pb_dev_state_t pb_dev_state = {0};

static uint8_t clean_up() {
  bs_trace_raw(8, "Cleaning up\n");
  pb_dev_clean_up(&pb_dev_state);
  return 0;
}

int main(int argc, char *argv[]) {
  time_monitor_args_t args;

  bs_trace_register_cleanup_function(clean_up);
  bs_time_monitor_argparse(argc, argv, &args);

  bs_trace_raw(9,"Connecting...\n");
  pb_dev_init_com(&pb_dev_state, args.device_nbr, args.s_id, args.p_id);

  bs_time_t time = args.interval;
  bs_time_t time_r = 0;
  pb_wait_t wait_s;
  struct timespec tv;

  bs_time_t tic_start, tic_end, tic_1st_start;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  tic_start = tic_1st_start = tv.tv_sec*1e6 + tv.tv_nsec/1000;

  for (int i = 0; i < QUEUE_DEPTH - 1; i++) {
    wait_s.end = time;
    if (pb_dev_request_wait_nonblock(&pb_dev_state, &wait_s) == -1) {
      bs_trace_raw(3,"We have been told to disconnect\n");
      break;
    }
    time += args.interval;
  }

  while (pb_dev_state.connected){
    wait_s.end = time;
    if (pb_dev_request_wait_nonblock(&pb_dev_state, &wait_s) == -1) {
      bs_trace_raw(3,"We have been told to disconnect\n");
      break;
    }
    if (pb_dev_pick_wait_resp(&pb_dev_state) == -1) {
      bs_trace_raw(3,"We have been told to disconnect\n");
      break;
    }
    time_r += args.interval;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    tic_end = tv.tv_sec*1e6 + tv.tv_nsec/1000;

    bs_trace_raw_manual_time(2, time_r,
                             "@%"PRItime"us reached (instantaneous speed=%06.2fx, average=%06.2fx)        \r",
                             time_r, args.interval/(double)(tic_end-tic_start), time_r/(double)(tic_end-tic_1st_start));
    fflush(stdout);
    time += args.interval;
    tic_start = tic_end;
  }

  bs_trace_raw_manual_time(2, time_r,
                           "@%"PRItime"us (end) reached (average speed %.2fx)                   \n",
                           time_r, time_r/(double)(tic_end-tic_1st_start));

  bs_trace_raw(9,"Disconnecting...\n");
  pb_dev_disconnect(&pb_dev_state);

  return 0;
}
