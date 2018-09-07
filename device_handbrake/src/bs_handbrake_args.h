/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_HANDBRAKE_ARGS_H
#define BS_HANDBRAKE_ARGS_H

#include "bs_types.h"
#include "bs_cmd_line.h"
#include "bs_cmd_line_typical.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
  BS_BASIC_DEVICE_OPTIONS_FIELDS
  bs_time_t poke_period;
  double real_time_ratio;
} handbrake_args_t;

void bs_handbrake_argparse(int argc, char *argv[], handbrake_args_t *args);

#ifdef __cplusplus
}
#endif

#endif
