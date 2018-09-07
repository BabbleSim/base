/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_PC_BASECHANNEL_H
#define BS_PC_BASECHANNEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

uint *bs_open_back_channel(uint global_dev_nbr, uint* dev_nbrs, uint* channel_nbrs, uint number_of_channels);
void bs_clean_back_channels();

void bs_bc_send_msg(uint channel_id, uint8_t *ptr, size_t size);
int bs_bc_is_msg_received(uint channel_id);
void bs_bc_receive_msg(int channel_id , uint8_t *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
