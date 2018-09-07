/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BS_UTIL_TYPES_H
#define BS_UTIL_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
/*It is relatively likely that we will be redefining uint, so let's void the warning*/
typedef unsigned int uint;
#pragma GCC diagnostic pop

typedef uint64_t bs_time_t;

#define TIME_NEVER UINT64_MAX
#define PRItime PRIu64
#define SCNtime SCNu64

#ifndef INLINE
  #define INLINE static __attribute__((__always_inline__)) inline
#endif

#ifdef __cplusplus
}
#endif

#endif
