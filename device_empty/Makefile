# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

BSIM_BASE_PATH?=$(abspath ../ )
include ${BSIM_BASE_PATH}/common/pre.make.inc

EXE_NAME:=bs_device_empty
SRCS:=src/bs_empty_main.c 
A_LIBS:=${BSIM_LIBS_DIR}/libUtilv1.a \
        ${BSIM_LIBS_DIR}/libPhyComv1.a
SO_LIBS:=

INCLUDES:= -I${libUtilv1_COMP_PATH}/src/ \
           -I${libPhyComv1_COMP_PATH}/src/

DEBUG:=-g
OPT:=
ARCH:=
WARNINGS:=-Wall -pedantic
COVERAGE:=
CFLAGS:=${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=c99 ${INCLUDES}
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:=-D_POSIX_C_SOURCE=200809

include ${BSIM_BASE_PATH}/common/make.device.inc
