/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_UTIL_SYMBOLS_H
#define BS_UTIL_SYMBOLS_H

#ifdef __cplusplus
extern "C"{
#endif

#ifndef _BS_TSYMBOLS_TRACE
#define _BS_TSYMBOLS_TRACE 1
#endif

#include <stddef.h>

#if (_BS_TSYMBOLS_TRACE )
void bs_read_function_names_from_Tsymbols(const char *probable_binary_name);
#else
#define cs_read_function_names_from_Tsymbols(a)
#endif

int bs_could_be_symbol(uint32_t ptr);
void bs_snprint_symbol_name_from_Tsymbols(void *fptr, char* ptr, size_t n);

//Internal definitions to Util library:
void bs_clear_Tsymbols();

#ifdef __cplusplus
}
#endif

#endif
