/*
 * Copyright 2018 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BS_UTIL_OSWRAP_H
#define _BS_UTIL_OSWRAP_H

#include <stdlib.h>
#include <stdio.h>
#include "bs_types.h"
#include "bs_string.h"

#ifdef __cplusplus
extern "C"{
#endif

void bs_set_sig_term_handler(void (*f)(int), int signals[], int n);
#if defined(__linux)
uint64_t bs_get_process_start_time(long int pid);
#endif
void* bs_malloc(size_t size);
void* bs_aligned_alloc(size_t alignment, size_t size);
void* bs_calloc(size_t nmemb, size_t size);
void* bs_realloc(void *ptr, size_t size);
void bs_skipline(FILE *file);
void bs_readline(char *s, int size, FILE *stream);
int bs_createfolder(const char* folderpath);
FILE* bs_fopen(const char *file_path, const char *open_type);
int bs_create_folders_in_path(const char *path);

#ifdef __cplusplus
}
#endif

#endif
