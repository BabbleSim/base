# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

OBJS=$(abspath $(addprefix $(COMPONENT_OUTPUT_DIR)/,${SRCS:.c=.o}))
LIBFILE_DYN=${LIB_NAME}.so

all: install

DEPENDFILES:=$(addsuffix .d,$(basename ${OBJS})) 

-include ${DEPENDFILES}

always_run_this_target: 
#phony target to trigger the rerun of the make of each library, but ensure that make checks if the library was regenerated (so it doesnt relink the binary if it wasnt)
# we could do like in the root Makefile, and just go first over all the libraries makefiles we may need instead, but this is slighly more efficient (although more messy)

.PHONY: all install compile lib clean clean_all ${DEPENDFILES} always_run_this_target

compile: $(COMPONENT_OUTPUT_DIR)/${LIBFILE_DYN} 

lib: compile

$(COMPONENT_OUTPUT_DIR)/%.o: %.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	@${CC} ${CPPFLAGS} ${CFLAGS} ${COVERAGE} -c $< -o $@

$(COMPONENT_OUTPUT_DIR)/${LIBFILE_DYN} : ${OBJS} $(A_LIBS)
	@${CC} -o $@ -fPIC -shared $^ ${LDFLAGS} ${SO_LIBS} ${A_LIBS} ${COVERAGE}

%.a:;
	$(error Required library ($@) not found. Run top level make to build all dependencies in order)

clean:
	@echo "Deleting intermediate compilation results"
	@find $(COMPONENT_OUTPUT_DIR) -name "*.a" -or -name "*.o" -or -name "*.so" -or -name "*.d" | xargs rm
	@rm $(COMPONENT_OUTPUT_DIR)/${LIBFILE_DYN} &> /dev/null ; true

clean_coverage:
	@find $(COMPONENT_OUTPUT_DIR) -name "*.gcda" -or -name "*.gcno" | xargs rm -f ; true

clean_all: clean clean_coverage

${BSIM_LIBS_DIR}/% : $(COMPONENT_OUTPUT_DIR)/%
	@cp $< $@ 

install: ${BSIM_LIBS_DIR}/${LIBFILE_DYN}

Makefile: ;
