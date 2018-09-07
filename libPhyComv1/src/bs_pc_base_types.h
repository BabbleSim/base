/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_PC_BASE_TYPES_H
#define BS_PC_BASE_TYPES_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef uint32_t pc_header_t;

/** Message headers: **/
/* The device wants to wait */
#define PB_MSG_WAIT          0x01
/* The device is disconnecting from the phy or viceversa: */
#define PB_MSG_DISCONNECT  0xFFFF
/* The device is disconnecting from the phy, and is requesting the phy to end the simulation ASAP */
#define PB_MSG_TERMINATE   0xFFFE
/* The requested time tick has just finished */
#define PB_MSG_WAIT_END      0x81

/**
 * Structure following a PB_MSG_WAIT command
 */
typedef struct __attribute__ ((packed)) {
  bs_time_t end;
} pb_wait_t;

#ifdef __cplusplus
}
#endif

#endif
