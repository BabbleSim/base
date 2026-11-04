# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

SHELL:=bash

COMPONENT_OUTPUT_DIR?=./#This should really be set by the top level makefile, but if it wasnt just to be safe we point here
BSIM_OUT_PATH?=../../#This should really be set by the top level makefile, but if it wasnt just to be safe we point to where it normally is
override BSIM_OUT_PATH:=$(realpath ${BSIM_OUT_PATH})
$(if $(realpath ${BSIM_OUT_PATH}),,$(error BSIM_OUT_PATH does not exist))
BSIM_LIBS_DIR:=${BSIM_OUT_PATH}/lib/
BSIM_BIN_DIR:=${BSIM_OUT_PATH}/bin/
#We assume whatever component we are compiling is placed inside the components folder:
BSIM_COMPONENTS_PATH?=$(abspath ../)
BSIM_BASE_PATH?=${BSIM_COMPONENTS_PATH}
override BSIM_COMPONENTS_PATH:=$(realpath ${BSIM_COMPONENTS_PATH})
$(if $(realpath ${BSIM_COMPONENTS_PATH}),,$(error BSIM_COMPONENTS_PATH does not exist))
override BSIM_BASE_PATH:=$(realpath ${BSIM_BASE_PATH})
$(if ${BSIM_BASE_PATH},,$(error BSIM_BASE_PATH does not exist))
libUtilv1_COMP_PATH:=$(abspath ${BSIM_BASE_PATH}/libUtilv1 )
libRandv2_COMP_PATH:=$(abspath ${BSIM_BASE_PATH}/libRandv2 )
libPhyComv1_COMP_PATH:=$(abspath ${BSIM_BASE_PATH}/libPhyComv1 )

CC:=gcc
AR:=ar
