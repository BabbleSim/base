# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0
#
# Top level BabbleSim Makefile:
# it calls make for each component to be compiled

# In case of trouble with making, this may help: make -n --no-builtin-rules --warn-undefined-variables -d
# or limit with --debug=b  (see https://www.gnu.org/software/make/manual/html_node/Options-Summary.html)
# you can also install and run with remake or if need be remake -d
define LF


endef

SHELL:=bash

BSIM_OUT_PATH?=./#by default all the output of compiling the simulator is placed under this same folder. If you want the output in some other place set this environment variable
export BSIM_OUT_PATH:=$(abspath ${BSIM_OUT_PATH})

no_default:
	@echo "There is no default rule, please specify which component you want to build"

BSIM_COMPONENTS_PATH?=$(abspath components/)
ifeq ("$(wildcard $(BSIM_COMPONENTS_PATH))","")
$(error BSIM_COMPONENTS_PATH (${BSIM_COMPONENTS_PATH}) does not point to a folder${LF}	Do not run me directly inside the common folder; or set BSIM_COMPONENTS_PATH first)
endif
export BSIM_COMPONENTS_PATH

COMPONENTS:=$(shell ls ${BSIM_COMPONENTS_PATH} --hide=common)

DEPENDFILES:=$(addprefix components/,$(addsuffix /Depends,${COMPONENTS}))

.PHONY: $(COMPONENTS) all clean clean_results ultraclean help no_default everything

Makefile: ;

${BSIM_OUT_PATH}:
	@mkdir ${BSIM_OUT_PATH} -p
${BSIM_OUT_PATH}/bin/: | ${BSIM_OUT_PATH}
	mkdir ${BSIM_OUT_PATH}/bin/
${BSIM_OUT_PATH}/lib/: | ${BSIM_OUT_PATH}
	@mkdir ${BSIM_OUT_PATH}/lib/
${BSIM_OUT_PATH}/results/: | ${BSIM_OUT_PATH}
	@mkdir ${BSIM_OUT_PATH}/results/

all: everything

everything: ${COMPONENTS}

include ${DEPENDFILES}
#we complain if the depend file is missing (and we complain more below, but we dont break)

$(COMPONENTS): | ${BSIM_OUT_PATH} ${BSIM_OUT_PATH}/bin/ ${BSIM_OUT_PATH}/lib/ ${BSIM_OUT_PATH}/results/
	@if [ ! -d "${BSIM_OUT_PATH}/components/$@" ];then mkdir -p  ${BSIM_OUT_PATH}/components/$@ ; fi #let's create the output mirror directory of each component if it doesnt exist 
	@export COMPONENT_OUTPUT_DIR=$(abspath ${BSIM_OUT_PATH}/components/$@) ; ${MAKE} --no-builtin-rules --warn-undefined-variables -C components/$@; ( [ $$? -ne 0 ] && echo "*** NOTE: $@ could NOT be built, continuing ***" ); true

${DEPENDFILES}:
	$(warning $@ does not exist. Either you have a left over component folder or you forgot to add a depend file to a new component, either way please fix it (remove the empty component or create a proper Depend file) )

clean_results:
	@rm ${BSIM_OUT_PATH}/results/* -r ; true

clean:
	@echo "Deleting intermediate compilation results + libraries + executables (*.d .o .a .so & bin/bs_*)"
	@find -L ${BSIM_OUT_PATH} -path *fftw-* -prune -o \( -name "*.a" -o -name "*.o" -o -name "*.so" -o -name "*~" -o -name "*.d" -o -name "*.Tsymbols" \)  | grep -v fftw  | grep -i -v libCrypto | xargs rm -f  \;  
	@#avoid looking into the library directories we dont want to need to rebuild each time somebody goes and runs make clean
	@cd ${BSIM_OUT_PATH}/bin/ ; rm bs_* >& /dev/null ; true

clean_coverage:
	@find $(BSIM_OUT_PATH) -name "*.gcda" -or -name "*.gcno" | xargs rm -f ; true

clean_all: clean clean_coverage

help:
	@echo "********************************"
	@echo "* BabbleSim top level makefile *"
	@echo "********************************"
	@echo "Provided rules:"
	@echo " all             : everything (see below)"
	@echo " clean           : clean all components and their installed output"
	@echo " clean_coverage  : clean gcov input files"
	@echo " clean_all       : clean & clean_coverage"
	@echo " clean_results   : clean results folder"
	@echo " <component>     : compile that component and its dependencies"
	@echo " everything      : compile all components present in the components folder"
	@echo "Note that you can use TAB to autocomplete rules in the command line in modern OSs"

#.SILENT:; #let's not print entering and leaving directory messages
