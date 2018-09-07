/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BS_UTIL_UTILS_H
#define _BS_UTIL_UTILS_H

#include "bs_types.h"

#ifndef STR
#define __STR_2(a) #a
#define STR(a) __STR_2(a)
#endif

//MIN and MAX macros similar to the definition in sys/param.h
//see for example http://stackoverflow.com/questions/3437404/min-and-max-in-c for a long discussion about a trivial piece of code
//Note that the solution with the compound statement is not necessarily portable to other compilers
#ifndef BS_MAX
#define BS_MAX( x, y ) ( ( (x) > (y) ) ? (x) : (y) ) //Safe'ish MAX macro (be careful that a and b are evaluated twice)
#endif

#ifndef BS_MIN
#define BS_MIN( x, y ) ( ( (x) < (y) ) ? (x) : (y) ) //Safe'ish MIN macro (be careful that a and b are evaluated twice)
#endif

#ifndef BS_MOD
#define BS_MOD( x ,y ) ( ( ( (x) % (y) ) + (y) ) % (y) ) //Use this instead of just % to avoid problems with rem( % in C99 ) != mod
#endif

#endif
