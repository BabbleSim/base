/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BS_PAUSE_ARGS_H
#define BS_PAUSE_ARGS_H

#include "bs_types.h"
#include "bs_cmd_line.h"
#include "bs_cmd_line_typical.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NBR_PAUSES 256
typedef struct {
  BS_BASIC_DEVICE_OPTIONS_FIELDS
  uint n_times;
  bs_time_t times[MAX_NBR_PAUSES];
} pause_args_t;

void bs_pause_argparse(int argc, char *argv[], pause_args_t *args);

#ifdef __cplusplus
}
#endif

#endif
