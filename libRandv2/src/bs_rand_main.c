/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_rand_inline.h"

/**
 * Initialize the random generators
 */
int bs_random_init(unsigned int seed){
  srandom(seed);
  srand(seed);
  return 0;
}

/**
 * Free any memory this module may have allocated
 */
void bs_random_free(){

}

/**
 * Generate a Gausian random number N(0,1): with mean 0 and variance 1
 * In general to get a N(ave,std) do:
 *   Random_Gaus()*std + ave;
 *
 * It uses the Marsaglia polar method:
 *  http://en.wikipedia.org/wiki/Marsaglia_polar_method
 *  Based on the "MIT like" licensed code
 *  ftp://ftp.taygeta.com/pub/c/boxmuller.c:
 *  "(c) Copyright 1994, Everett F. Carter Jr.
 *       Permission is granted by the author to use
 *       this software for any application provided this
 *       copyright notice is preserved."
 *
 *  but with div by 0 protection
 */
double bs_random_Gaus(){
  double x,w;
  static double y;
  static int use_last = 0;
  double toreturn;

  if (use_last == 0) {
    do {
      x = 2*((double)random() / RAND_MAX)-1;
      y = 2*((double)random() / RAND_MAX)-1;

      w = x * x + y * y;
    } while (w >= 1 || w == 0);

    w = sqrt(-2.0 * log(w) / w);
    toreturn = x * w;
    y = y * w;
  } else {
    toreturn = y;
  }

  use_last = 1 - use_last;

  return toreturn;
}

/**
 * Like Random_Gaus but return a complex double instead
 * Returns a sample of a N(0,1) + 1i*N(0,1) with uncorrelated I and Q parts
 */
double complex bs_random_Gaus_c(){
  double x, y, w;
  double complex toreturn;

    do {
      x = 2*((double)random() / RAND_MAX)-1;
      y = 2*((double)random() / RAND_MAX)-1;

      w = x * x + y * y;
    } while(w >= 1 || w == 0);
    w = sqrt(-2.0 * log(w) / w);

    toreturn = x * w + ( y * w )*I;

  return toreturn;
}

/**
 * Fill in a buffer of size <size> with complex random gausian numbers
 * each a sample of a N(0,1) + 1i*N(0,1) with uncorrelated I and Q parts
 */
void bs_random_Gaus_c_buffer(double complex* buffer, uint size){
  double x, y, w;
  uint i;

  //TOOPT: this takes a sizeable amount of time for the 2G4_channel_Indoorv1,
  // it could make sense to optimize it to avoid using log
  // roughly time is used as follows: log: 383/856, sqrt = 12/856,  random 133*2/856
  // an option would be: http://en.wikipedia.org/wiki/Ziggurat_algorithm

  for (i = 0 ; i < size; i++){
    do {
      x = 2*((double)random() / RAND_MAX)-1;
      y = 2*((double)random() / RAND_MAX)-1;

      w = x * x + y * y;
    } while(w >= 1 || w == 0);
    w = sqrt(-2.0 * log(w) / w);

    buffer[i] = x * w + ( y * w )*I;
  }
}

/**
 * Return a Generalized Pareto random number (double)
 * Where the generalized pareto parameters are (like in MATLAB)
 *  k: shape (-inf..inf) (xi for some authors)
 *  sigma: scale (0..inf)
 *  theta: location (-inf..inf) (mu for some authors)
 */
double bs_random_GPRND(double k, double sigma, double theta){
  double u = ( ((double)random()) / ((double)RAND_MAX)); //U(0,1) : uniformly distributed random number from 0 to 1
  double r;
  if ( fabs(k) < 0.005 ){ //effectively there is no difference in the shape of the distribution from k = 0
    r = -log(u);
  } else {
    r = expm1(-k*log(u)) / k; // ( u^(-k) - 1 ) / k
  }
  r = theta + sigma*r;
  return r;
}

/**
 * Draws a bit {0,1}
 */
char bs_random_bit(){
  return ( random() & 1 );
}

/**
 * Draws a number from a U[0,1]
 */
double bs_random_uniform(){
  return ( ((double)random()) / ((double)RAND_MAX));
}

/**
 * Draws a number from a U[a,b] (both double)
 */
double bs_random_uniformR(double a, double b){
  double u = ( ((double)random()) / ((double)RAND_MAX));
  u *=(b-a);
  u +=a;
  return u;
}

/**
 * Draws a number from a U[a,b] (both integer)
 *  b > a
 */
int bs_random_uniformRi(int a, int b){
  if ( b <= a )
    return a; //safety feature

  int i_u;
  while (1) {
    double u = ( ((double)random()) / ((double)RAND_MAX)); //[0..1]
    u *=(b+1-a); //[0.. (b+1-a)]
    u +=a;       //[a..b+1]
    i_u = floor(u); //[a,..,b+ups]
    if (i_u <= b) //although extremely unlikely, we could get RAND_MAX out of the random generator and otherwise produce b+1
      break;
  }
  return i_u;
}

uint32_t bs_random_uint32(){
  return (uint32_t)random();
}

/**
 * Do a binomial drop, that is produce a realization of B(n,p)
 *
 * n: number of independent trials
 * probability: probability p, for each trial, a number between 0 (0.0) and RAND_PROB_1 (1.0)
 */
uint bs_random_Binomial(uint n, uint32_t probability){
  uint acc = 0;
  for (uint i = 0; i < n; i ++) {
    acc += bs_random_Bern(probability);
  }
  return acc;
}
