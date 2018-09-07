/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BS_PC_BASE_FIFO_USER_H
#define BS_PC_BASE_FIFO_USER_H

/**
 * For other modules which also want to create FIFOs in the com directory
 */

#ifdef __cplusplus
extern "C"{
#endif

extern char *pb_com_path;
extern int pb_com_path_length;

extern int pb_create_fifo_if_not_there(const char* fifo_name);

#ifdef __cplusplus
}
#endif

#endif
