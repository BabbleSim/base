/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bs_oswrap.h"
#include "bs_tracing.h"

/**
 * If the results folder for this sim_id does not exist create it
 * return a pointer to the path string
 * Exists with an ERROR if unable to complete
 */
char* bs_create_result_folder(const char* s_id){
  char* results_path;
  //we progressively check & build the directory structure up to the FIFOs (check if it is there and if not create)
  results_path = (char*)bs_calloc(15 + strlen(s_id), sizeof(char));
  sprintf(results_path,"../results");

  if (bs_createfolder(results_path) != 0) {
    bs_trace_error_line("Couldn't create results folder %s\n", results_path);
    free(results_path);
  }
  sprintf(results_path,"../results/%s",s_id);
  if (bs_createfolder(results_path) != 0) {
    bs_trace_error_line("Couldn't create results folder %s\n", results_path);
    free(results_path);
  }
  return results_path;
}

/**
 * Create a results file in the results folder
 * (if the results folder does not exist yet, it will be created)
 * and return a pointer to the file
 *
 * Mode can be either "w" "r" "a" (or "w+" or "r+" "a+")
 *
 * The file will be:
 *   <resultsfolderpath>/d_<device_number>.<postfix>.csv
 *
 *   <resultsfolderpath> = results/<s_id>
 */
FILE* bs_create_result_file(const char* s_id, const uint dev_nbr, const char* postfix, char* mode){
  char* results_path;
  FILE *file_ptr;

  //Ensure folder is there, if not create:
  results_path = bs_create_result_folder(s_id);

  //Create file in mode requested
  char filename[18 + strlen(results_path) + strlen(postfix)];

  sprintf(filename,"%s/d_%i.%s.csv",results_path, dev_nbr, postfix);
  free(results_path);

  file_ptr = bs_fopen(filename, mode);

  return file_ptr;
}
