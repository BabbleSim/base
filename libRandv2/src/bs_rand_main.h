/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_RAND_MAIN_H
#define BS_RAND_MAIN_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RAND_PROB_1 RAND_MAX

int bs_random_init(unsigned int seed);
void bs_random_free();
double bs_random_Gaus();

#ifdef complex
//Only for those who have included <complex.h>:
double complex bs_random_Gaus_c();
void bs_random_Gaus_c_buffer(double complex* buffer, uint size);
#endif

double bs_random_GPRND(double k, double sigma, double theta);

char bs_random_bit();
double bs_random_uniform();
double bs_random_uniformR(double a, double b);
int    bs_random_uniformRi(int a, int b);
uint32_t bs_random_uint32();

uint bs_random_Binomial(uint n, uint32_t probability);

#ifdef __cplusplus
}
#endif

#endif
