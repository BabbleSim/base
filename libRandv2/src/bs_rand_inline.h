/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BS_RAND_INLINE_H
#define BS_RAND_INLINE_H

#include <stdlib.h>
#include "bs_rand_main.h"
#include "bs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RAND_PROB_1 RAND_MAX

BSIM_INLINE uint bs_random_Bern(uint32_t probability);

/**
 * Do a Bernouilli drop
 *
 * probability: number between 0 (0.0) and RAND_PROB_1 (1.0)
 */
BSIM_INLINE uint bs_random_Bern(uint32_t probability){
  if ( (uint32_t)random() < probability )
    return 1;
  else
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
