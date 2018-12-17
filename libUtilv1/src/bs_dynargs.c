/*
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "bs_tracing.h"

// internal:
static int used_args;
static int args_aval;
#define ARGS_ALLOC_CHUNK_SIZE 20

void bs_cleanup_dynargs(bs_args_struct_t **args_struct)
{
  if (*args_struct != NULL) { /* LCOV_EXCL_BR_LINE */
    free(*args_struct);
    *args_struct = NULL;
  }
}

/**
 * Add a set of command line options to the program.
 *
 * Each option to be added is described in one entry of the input <args>
 * This input must be terminated with an entry containing ARG_TABLE_ENDMARKER.
 */
void bs_add_dynargs(bs_args_struct_t **all_args, bs_args_struct_t *new_args)
{
  int count = 0;

  while (new_args[count].option != NULL) {
    count++;
  }
  count++; /*for the end marker*/

  if (used_args + count >= args_aval) {
    int growby = count;
    /* reallocs are expensive let's do them only in big chunks */
    if (growby < ARGS_ALLOC_CHUNK_SIZE) {
      growby = ARGS_ALLOC_CHUNK_SIZE;
    }

    *all_args = realloc(*all_args, (args_aval + growby)*sizeof(bs_args_struct_t));
    args_aval += growby;
    /* LCOV_EXCL_START */
    if (*all_args == NULL) {
      bs_trace_error_line("Could not allocate memory");
    }
    /* LCOV_EXCL_STOP */
  }

  memcpy(&(*all_args)[used_args], new_args, count*sizeof(bs_args_struct_t));

  used_args += count - 1;
  /*
   * -1 as the end marker should be overwritten next time something
   * is added
   */
}
