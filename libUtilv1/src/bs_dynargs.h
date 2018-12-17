/*
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_DYNARGS_H
#define BS_DYNARGS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "bs_cmd_line.h"

void bs_cleanup_dynargs(bs_args_struct_t **args_struct);
void bs_add_dynargs(bs_args_struct_t **all_args, bs_args_struct_t *new_args);

//Devices which support this IF will typically provide this one:
void bs_add_extra_dynargs(bs_args_struct_t *args_struct_toadd);

#ifdef __cplusplus
}
#endif

#endif
