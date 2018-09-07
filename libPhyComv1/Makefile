# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

BSIM_BASE_PATH?=$(abspath ../ )
include ${BSIM_BASE_PATH}/common/pre.make.inc

SRCS:=$(shell ls src/*.c)

INCLUDES:=-I${libUtilv1_COMP_PATH}/src/

LIB_NAME:=libPhyComv1
A_LIBS:=
A_LIBS32:=
SO_LIBS:=
DEBUG:=-g
OPT:=
ARCH:=
WARNINGS:=-Wall -pedantic
COVERAGE:=
CFLAGS:=${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=c99  -fPIC ${INCLUDES}
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:= -D_XOPEN_SOURCE=700

include ${BSIM_BASE_PATH}/common/make.lib_soeta64et32.inc
