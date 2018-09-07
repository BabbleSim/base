/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_UTIL_RESULTS_H
#define BS_UTIL_RESULTS_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>

char* bs_create_result_folder(const char* s_id);
FILE* bs_create_result_file(const char* s_id, const unsigned int dev_nbr,
                            const char* postfix, char* mode);

#ifdef __cplusplus
}
#endif

#endif
