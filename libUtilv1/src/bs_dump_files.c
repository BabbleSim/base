/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "bs_oswrap.h"
#include "bs_results.h"
#include "bs_tracing.h"
#include "bs_dump_files.h"

#define BS_ALLOC_CHUNK 10
static bs_dumpf_ctrl_t *df_ctrl; /* Array of dump file control elements */
static int bsdc_alloc_size; /* Number of allocated control elements */
static int number_of_dump_files; /* Number of used control elements/dump files */

int bs_dump_file_register(bs_dumpf_ctrl_t *f_ctrl) {
  if (number_of_dump_files + 1 >= bsdc_alloc_size) {
    bsdc_alloc_size += BS_ALLOC_CHUNK;
    df_ctrl = bs_realloc(df_ctrl, bsdc_alloc_size * sizeof(bs_dumpf_ctrl_t) );
  }
  memcpy(&df_ctrl[number_of_dump_files], f_ctrl, sizeof(bs_dumpf_ctrl_t));
  number_of_dump_files++;

  return number_of_dump_files - 1;
}

void bs_dump_files_set_dump_level(int newdumplevel) {
  for (unsigned int i = 0 ; i < number_of_dump_files; i ++) {
    if (df_ctrl[i].dump_level <= newdumplevel) {
      df_ctrl[i].enabled = true;
    }
  }
}

void bs_dump_files_activate_file(const char *filename) {
  bool all = false;
  bool none = false;

  if (strcasecmp(filename,"all") == 0) {
    all = true;
  } else if (strcasecmp(filename,"none") == 0) {
    none = true;
  }

  for (unsigned int i = 0; i < number_of_dump_files; i ++) {
    if (none) {
      df_ctrl[i].enabled = false;
      bs_trace_raw(9, "dump: Dump of file %s de-activated\n",
          df_ctrl[i].postfix);
    } else if (all || (strcmp(filename,df_ctrl[i].postfix) == 0)) {
      df_ctrl[i].enabled = true;
      bs_trace_raw(9,"dump: Dump of file %s activated\n",
          df_ctrl[i].postfix);
    }
  }
}

/**
 * Print to console all files names, their default dump level
 * and if they are currently active or not
 */
void bs_dump_files_print_files(void) {
  bs_trace_raw(0," File name (suffix)            , dump_level,"
      " activated\n");
  for (unsigned int i = 0; i < number_of_dump_files; i ++) {
    bs_trace_raw(0, " %-30s,     %2i    ,     %i\n",
                 df_ctrl[i].postfix,
                 df_ctrl[i].dump_level,
                 df_ctrl[i].enabled);
  }
}

void bs_dump_files_open(const char* s, const unsigned int dev_number) {
  char* results_path = NULL;
  bool results_folder_created = false;

  for (unsigned int i = 0 ; i < number_of_dump_files; i ++) {
    if (df_ctrl[i].enabled == false) {
      continue;
    }
    if (results_folder_created == false) {
      results_path = bs_create_result_folder(s);
      results_folder_created = true;
    }

    char filename[strlen(df_ctrl[i].postfix) + strlen(results_path) + 25];

    sprintf(filename, "%s/d_%02i.%s.csv", results_path,
        dev_number, df_ctrl[i].postfix);

    df_ctrl[i].fileptr = bs_fopen(filename, "wt");

    if (df_ctrl[i].header_f != NULL) {
      df_ctrl[i].header_f(df_ctrl[i].fileptr);
    }
  }

  if (results_path != NULL) {
    free(results_path);
  }
}

void bs_dump_files_close_all(void) {
  if (!df_ctrl) {
    return;
  }

  for (unsigned int i = 0 ; i < number_of_dump_files; i ++) {
    if (df_ctrl[i].fileptr != NULL) {
      fclose(df_ctrl[i].fileptr);
      df_ctrl[i].fileptr = NULL;
    }
  }

  free(df_ctrl);
  df_ctrl = NULL;
}


void bsdf_cmd_dump_found(char * argv, int offset) {
	bs_dump_files_activate_file(&argv[offset]);
}

void bsdf_cmd_printdumps_found(char * argv, int offset) {
	bs_dump_files_print_files();
}

unsigned int bsdf_dump_level;
void bsdf_cmd_dumplevel_found(char * argv, int offset) {
	bs_dump_files_set_dump_level(bsdf_dump_level);
}
