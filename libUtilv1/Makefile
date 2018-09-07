# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

BSIM_BASE_PATH?=$(abspath ../ )
include ${BSIM_BASE_PATH}/common/pre.make.inc

LIB_NAME:=libUtilv1
SRCS:=$(shell ls src/*.c)
A_LIBS:=
A_LIBS32:=
SO_LIBS:=

DEBUG:=-g
OPT:=
ARCH:=
WARNINGS:=-Wall -pedantic
COVERAGE:=
#if you want to compile for gcov add --coverage (remember to not have optimizations)
CFLAGS:=${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=c99  -fPIC
LDFLAGS:=${ARCH}
CPPFLAGS:=-D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809

include ${BSIM_BASE_PATH}/common/make.lib_soeta64et32.inc
