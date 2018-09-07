/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <limits.h>
#include "bs_cmd_line_typical.h"
#include "bs_tracing.h"

/**
 * For most devices,
 * (devices for which the args_struct starts with a BS_BASIC_DEVICE_*_OPTIONS_ARG_STRUCT)
 * we need to check that the parameters below have been set
 */
void bs_args_typical_dev_post_check(bs_basic_dev_args_t *a, bs_args_struct_t args_struct[], char *default_phy) {
  if (a->device_nbr == UINT_MAX) {
    bs_args_print_switches_help(args_struct);
    bs_trace_error_line("The command line option <device number> needs to be set\n");
  }
  if (a->global_device_nbr == UINT_MAX) {
    a->global_device_nbr = a->device_nbr;
    bs_trace_set_prefix_dev(a->global_device_nbr);
  }
  if (!a->s_id) {
    bs_args_print_switches_help(args_struct);
    bs_trace_error_line("The command line option <simulation ID> needs to be set\n");
  }
  if (!a->p_id) {
    a->p_id = default_phy;
  }
}

/**
 * For devices for which the args_struct starts with a BS_BASIC_DEVICE_*_OPTIONS_ARG_STRUCT,
 * this function sets their defaults properly
 */
void bs_args_typical_dev_set_defaults(bs_basic_dev_args_t *a, bs_args_struct_t args_struct[]) {
  bs_args_set_defaults(args_struct);
  a->verb         = 2;
  bs_trace_set_level(a->verb);
  a->rseed        = 0xFFFF;
  a->start_offset = 0;
}
