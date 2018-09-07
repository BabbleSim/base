/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BS_UTIL_STRING_H
#define BS_UTIL_STRING_H

#include "bs_types.h"

char * bs_time_to_str( char* dest, bs_time_t time);

int bs_number_strlen(long long int a);

void bs_read_hex_dump(char *s,uint8_t *buffer, uint size);
void bs_hex_dump(char* buffer, const uint8_t *bytes, size_t nbytes);

#endif
