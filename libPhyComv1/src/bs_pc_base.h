/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BS_PCOM_BASE_H
#define BS_PCOM_BASE_H

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include "bs_types.h"
#include "bs_pc_base_types.h"

#ifdef __cplusplus
extern "C"{
#endif

int pb_create_fifo_if_not_there(const char* fifo_name);
int pb_create_com_folder(const char* s);
void pb_send_payload(int ff, void *buf, size_t size);
INLINE void pb_send_msg(int ff, pc_header_t header,
                        void *s, size_t s_size);

typedef struct {
  char **ff_path_dtp;
  char **ff_path_ptd;
  int *ff_dtp;
  int *ff_ptd;
  unsigned int n_devices;
  bool *device_connected;
  char *lock_path;
} pb_phy_state_t;

int pb_phy_is_connected_to_device(pb_phy_state_t * this, uint d);
int pb_phy_initcom(pb_phy_state_t *this, const char* s, const char *p, uint n);
void pb_phy_disconnect_devices(pb_phy_state_t *this);
pc_header_t pb_phy_get_next_request(pb_phy_state_t *this, uint d);
void pb_phy_get_wait_s(pb_phy_state_t *this, uint d, pb_wait_t *wait_s);
void pb_phy_resp_wait(pb_phy_state_t *this, uint d);
void pb_phy_free_one_device(pb_phy_state_t *this, int d);

typedef struct {
  int ff_dtp;
  int ff_ptd;
  char *ff_path_dtp;
  char *ff_path_ptd;
  bool connected;
  unsigned int this_dev_nbr;
  char *lock_path;
} pb_dev_state_t;

int pb_dev_init_com(pb_dev_state_t *this, uint d, const char* s, const char *p);
void pb_dev_disconnect(pb_dev_state_t *this);
void pb_dev_terminate(pb_dev_state_t *this);
void pb_dev_clean_up(pb_dev_state_t *this);
int pb_dev_read(pb_dev_state_t *this, void *buf, size_t n_bytes);
int pb_dev_request_wait_block(pb_dev_state_t *this, pb_wait_t *wait_s);
int pb_dev_request_wait_nonblock(pb_dev_state_t *this, pb_wait_t *wait_s);
int pb_dev_pick_wait_resp(pb_dev_state_t *this);

INLINE void pb_send_msg(int ff, pc_header_t header,
                        void *s, size_t s_size) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    write(ff, &header, sizeof(header));
    write(ff, s, s_size);
#pragma GCC diagnostic pop
}

#define CHECK_CONNECTED(c) \
  if (!c){ \
    bs_trace_error_line("Programming error: Not connected\n"); \
    return -1; \
  }

#define COM_FAILED_ERROR "Low level communication with phy failed"

#define INVALID_RESP(header) \
    bs_trace_error_line(COM_FAILED_ERROR \
                        ": Received invalid response %i\n", header)

#ifdef __cplusplus
}
#endif

#endif
