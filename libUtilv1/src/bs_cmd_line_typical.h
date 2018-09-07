/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Typical options used in most devices
 */

#ifndef _BS_CMD_TYPICAL_H
#define _BS_CMD_TYPICAL_H

#include <stdbool.h>
#include "bs_oswrap.h"
#include "bs_cmd_line.h"

#ifdef __cplusplus
extern "C" {
#endif

//Typical parameters:
#define ARG_S_ID      char* s_id; /*String identifying the simulation */
#define ARG_P_ID      char* p_id; /*String identifying the phy inside the simulation*/
#define ARG_DEV_NBR   unsigned int device_nbr; /*Number of this device in this phy*/
#define ARG_GDEV_NBR  unsigned int global_device_nbr; /*Overall device number in the simulation*/
#define ARG_VERB      unsigned int verb;  /*Level of verbosity when tracing*/
#define ARG_STARTO    double start_offset;/*Offset in time (at the start of the simulation) of this device*/
#define ARG_SEED      unsigned int rseed; /*Seed/initialization for the random numbers generators*/

//These basic arguments apply to almost all devices:
#define BS_BASIC_DEVICE_OPTIONS_FIELDS \
    ARG_S_ID \
    ARG_P_ID \
    ARG_DEV_NBR \
    ARG_GDEV_NBR \
    ARG_VERB \
    ARG_STARTO \
    ARG_SEED

typedef struct{
  BS_BASIC_DEVICE_OPTIONS_FIELDS
} bs_basic_dev_args_t;

    /*manual,mandatory,switch,option,     name ,               type,   destination,         callback,             , description*/
#define ARG_TABLE_S_ID \
    { false, false , false, "s",       "s_id",                 's', (void*)&args->s_id,      NULL,         "String which uniquely identifies the simulation"}
#define ARG_TABLE_P_ID_2G4 \
    { false, false , false, "p",       "p_id",                 's', (void*)&args->p_id,      NULL,         "(2G4) String which uniquely identifies the phy inside the simulation"}
#define ARG_TABLE_DEV_NBR \
    { false, true  , false, "d",       "device_number",        'u', (void*)&args->device_nbr,NULL,         "Device number (for this phy)"}
#define ARG_TABLE_GDEV_NBR \
    { false, false , false, "gd",      "global_device_number", 'u', (void*)&args->global_device_nbr, cmd_gdev_nbr_found, "(<device_number>) global device number (for tracing and so forth)"}
#define ARG_TABLE_VERB \
    { false, false , false, "v",       "trace_level",          'u', (void*)&args->verb, cmd_trace_lvl_found, "Set the verbosity/tracing/logging level to <trace_level> [0: almost nothing,..(2)..,9: everything]",}, \
    { false, false , false, "verbose", "trace_level",          'u', (void*)&args->verb, cmd_trace_lvl_found, "Alias for -v"}
#define ARG_TABLE_STARTO \
    { false, false , false, "start_offset","start_of",         'f', (void*)&args->start_offset,NULL,       "Offset in time (at the start of the simulation) of this device. At time 0 for the device, the phy will be at <start_of>"}
#define ARG_TABLE_STARTO_FAKE \
    { false, false , false, "start_offset","start_of",         'f', NULL,                    NULL,         "Not used in this device"}
#define ARG_TABLE_SEED \
    { false, false , false, "rs",      "rand_seed",            'u', (void*)&args->rseed,     NULL,         "Seed/initialization for the random numbers generators" },\
    { false, false , false, "random_seed","rand_seed",         'u', (void*)&args->rseed,     NULL,         "Alias for -rs" }
#define ARG_TABLE_SEED_FAKE \
    { false, false , false, "rs",      "rand_seed",            'u', NULL,                    NULL,         "Not used in this device" },\
    { false, false , false, "random_seed","rand_seed",         'u', NULL,                    NULL,         "Not used in this device" }
#define ARG_TABLE_COLOR \
    { false, false , true, "color",    "color",                'b', NULL,                    bs_trace_enable_color,  "(default) Enable color in traces if printing to console"}
#define ARG_TABLE_NOCOLOR \
    { false, false , true, "no-color", "no-color",             'b', NULL,                    bs_trace_disable_color, "Disable color in traces even if printing to console"}
#define ARG_TABLE_FORCECOLOR \
    { false, false , true, "force-color", "force-color",       'b', NULL,                    bs_trace_force_color,   "Enable color in traces even if printing to files/pipes"}

#define BS_BASIC_DEVICE_2G4_TYPICAL_OPTIONS_ARG_STRUCT \
    ARG_TABLE_S_ID,     \
    ARG_TABLE_P_ID_2G4, \
    ARG_TABLE_DEV_NBR,  \
    ARG_TABLE_GDEV_NBR, \
    ARG_TABLE_VERB,     \
    ARG_TABLE_STARTO,   \
    ARG_TABLE_SEED,     \
    ARG_TABLE_COLOR,    \
    ARG_TABLE_NOCOLOR,  \
    ARG_TABLE_FORCECOLOR

#define BS_BASIC_DEVICE_2G4_FAKE_OPTIONS_ARG_STRUCT \
    ARG_TABLE_S_ID,        \
    ARG_TABLE_P_ID_2G4,    \
    ARG_TABLE_DEV_NBR,     \
    ARG_TABLE_GDEV_NBR,    \
    ARG_TABLE_VERB,        \
    ARG_TABLE_STARTO_FAKE, \
    ARG_TABLE_SEED_FAKE,   \
    ARG_TABLE_COLOR,       \
    ARG_TABLE_NOCOLOR,     \
    ARG_TABLE_FORCECOLOR

void bs_args_typical_dev_post_check(bs_basic_dev_args_t *args, bs_args_struct_t args_struct[], char *default_phy);
void bs_args_typical_dev_set_defaults(bs_basic_dev_args_t *args, bs_args_struct_t args_struct[]);

#ifdef __cplusplus
}
#endif

#endif
