# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

OBJS=$(abspath $(addprefix $(COMPONENT_OUTPUT_DIR)/,${SRCS:.c=.o}))
OBJS32=$(abspath $(addprefix $(COMPONENT_OUTPUT_DIR)/,${SRCS:.c=.32.o}))
LIBFILE=${LIB_NAME}.a
LIBFILE32=${LIB_NAME}.32.a
VERSION_FILE:=${LIB_NAME}.version

all: install

DEPENDFILES:=$(addsuffix .d,$(basename ${OBJS})) 
DEPENDFILES32:=$(addsuffix .32.d,$(basename ${OBJS})) 

-include ${DEPENDFILES}
-include ${DEPENDFILES32}

always_run_this_target: 
#phony target to trigger the rerun of the make of each library, but ensure that make checks if the library was regenerated (so it doesnt relink the binary if it wasnt)
# we could do like in the root Makefile, and just go first over all the libraries makefiles we may need instead, but this is slighly more efficient (although more messy)

.PHONY: all install compile lib clean clean_all ${DEPENDFILES} ${DEPENDFILES32} always_run_this_target version
#setting the dependencies as phony targets will actually speed up things.. (otherwise make will check if there is implicit rules to remake them)

$(COMPONENT_OUTPUT_DIR):
	@mkdir -p $(COMPONENT_OUTPUT_DIR)

${OBJS32}:$(COMPONENT_OUTPUT_DIR)/%.32.o: %.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	@${CC} ${CPPFLAGS} ${CFLAGS} ${COVERAGE} -m32 -c $< -o $@

${OBJS}:$(COMPONENT_OUTPUT_DIR)/%.o: %.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	@${CC} ${CPPFLAGS} ${CFLAGS} ${COVERAGE} -c $< -o $@

$(COMPONENT_OUTPUT_DIR)/${LIBFILE} : ${OBJS} ${A_LIBS}
	@rm $(COMPONENT_OUTPUT_DIR)/${LIBFILE} &> /dev/null ; true
	@${AR} -cr $(COMPONENT_OUTPUT_DIR)/${LIBFILE} ${OBJS} ${A_LIBS}

$(COMPONENT_OUTPUT_DIR)/${LIBFILE32} : ${OBJS32} ${A_LIBS32}
	@rm $(COMPONENT_OUTPUT_DIR)/${LIBFILE32} &> /dev/null ; true
	@${AR} -cr $(COMPONENT_OUTPUT_DIR)/${LIBFILE32} ${OBJS32} ${A_LIBS32}

compile: $(COMPONENT_OUTPUT_DIR)/${LIBFILE} $(COMPONENT_OUTPUT_DIR)/${LIBFILE32}

lib : compile

${A_LIBS32}:;
	$(error Required library ($@) not found. Run top level make to build all dependencies in order)

${A_LIBS}:;
	$(error Required library ($@) not found. Run top level make to build all dependencies in order)

clean:
	@echo "Deleting intermediate compilation results"
	@find $(COMPONENT_OUTPUT_DIR) -name "*.a" -or -name "*.o" -or -name "*.so" -or -name "*.d" | xargs rm
	@rm $(COMPONENT_OUTPUT_DIR)/${LIBFILE} $(COMPONENT_OUTPUT_DIR)/${LIBFILE32} &> /dev/null ; true

clean_coverage:
	@find $(COMPONENT_OUTPUT_DIR) -name "*.gcda" -or -name "*.gcno" | xargs rm -f ; true

clean_all: clean clean_coverage

${BSIM_LIBS_DIR}/${VERSION_FILE}: version
	@if [[ -f "$<" ]]; then\
	  cp $< $@; \
	else \
	  echo "unknown" > $@; \
	fi

${BSIM_LIBS_DIR}/% : $(COMPONENT_OUTPUT_DIR)/%
	@cp $< $@ 

install: ${BSIM_LIBS_DIR}/${LIBFILE}  ${BSIM_LIBS_DIR}/${LIBFILE32} ${BSIM_LIBS_DIR}/${VERSION_FILE}

Makefile: ;