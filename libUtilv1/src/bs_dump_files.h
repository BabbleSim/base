/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BSIM_DUMP_FILES_H
#define BSIM_DUMP_FILES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function that will print the heading of the dump file */
typedef void (*bs_df_header_f_t)(FILE *);

/* Control structure defining for 1 dump file */
typedef struct {
  char *postfix; /* Postfix for the dump file filename */
	int dump_level; /* Level of the dump file */
	bool enabled; /* Is the dump file enabled or not (can be set to 1 to enable it by default) */
	FILE* fileptr; /* Pointer to the dump file itself */
	bs_df_header_f_t header_f; /* Function to print the dump file heading */
} bs_dumpf_ctrl_t;

#define DUMP_FILE_PTR(FILE_IDX) (f_ctrl[FILE_IDX].fileptr)
#define IS_DUMP_FILE_ACTIVE(FILE_IDX) (f_ctrl[FILE_IDX].fileptr != NULL)

/**
 * Register a new dump file
 *
 * Returns the index of the file which can later be used
 * in DUMP_FILE_PTR() & IS_DUMP_FILE_ACTIVE()
 */
int bs_dump_file_register(bs_dumpf_ctrl_t *f_ctrl);
/* Active all registered dump files with a level < new_dump_level*/
void bs_dump_files_set_dump_level(int new_dump_level);
/* Actiavte a given dump file from its postfix */
void bs_dump_files_activate_file(const char *postfix);
/* Open all active dump files */
void bs_dump_files_open(const char* s, const unsigned int d);
/* Close all dump files */
void bs_dump_files_close_all(void);
/* Print to console the list of registered dump files */
void bs_dump_files_print_files(void);


extern unsigned int bsdf_dump_level;
void bsdf_cmd_dump_found(char * argv, int offset);
void bsdf_cmd_printdumps_found(char * argv, int offset);
void bsdf_cmd_dumplevel_found(char * argv, int offset);

/* Command line arguments for controlling the dump files */
#define BS_DUMP_FILES_ARGS \
  {false, false, false,                                            \
  "dump", "file_postfix", 's',                                     \
  NULL, bsdf_cmd_dump_found,                                       \
  "Activate dump of a given file. Where <file_postfix> "           \
  "corresponds to the file name postfix of the file to be dumped " \
  "<file_postfix> can be \"all\" for all files, or \"none\" to "   \
  "clear all files"},                                              \
  {false, false, true,                                             \
  "printdumps", "", 'b',                                           \
  NULL, bsdf_cmd_printdumps_found,                                 \
  "Print the list of possible dump files, their dump level, and "  \
  "if with the current command line so far they would have been "  \
  "dumped or not"},                                                \
  {false, false, false,                                            \
  "dump_level", "level", 'u',                                      \
  (void*)&bsdf_dump_level, bsdf_cmd_dumplevel_found,               \
  "Set file dump level to <level> (default 0; all files with dump" \
  " level under level will be dumped)"}

#ifdef __cplusplus
}
#endif

#endif
