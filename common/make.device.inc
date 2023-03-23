# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

OBJS=$(abspath $(addprefix $(COMPONENT_OUTPUT_DIR)/,${SRCS:.c=.o}))
MAP_FILE_NAME=${EXE_NAME}.Tsymbols
VERSION_FILE:=${EXE_NAME}.version

all: install

DEPENDFILES:=$(addsuffix .d,$(basename ${OBJS})) 

-include ${DEPENDFILES}

.PHONY: all install compile clean clean_all ${DEPENDFILES} version
#setting the dependencies as phony targets will actually speed up things.. (otherwise make will check if there is implicit rules to remake them)

compile: $(COMPONENT_OUTPUT_DIR)/${EXE_NAME}

$(COMPONENT_OUTPUT_DIR):
	@mkdir -p $(COMPONENT_OUTPUT_DIR)

$(COMPONENT_OUTPUT_DIR)/%.o: %.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	@${CC} ${CPPFLAGS} ${CFLAGS} ${COVERAGE} -c $< -o $@

${A_LIBS}:;
	$(error Required library ($@) not found. Run top level make to build all dependencies in order)

$(COMPONENT_OUTPUT_DIR)/${EXE_NAME} : ${OBJS} ${A_LIBS} | $(COMPONENT_OUTPUT_DIR)
	@${CC} -o $@ $^ ${LDFLAGS} ${SO_LIBS} ${A_LIBS} ${COVERAGE}

$(COMPONENT_OUTPUT_DIR)/${MAP_FILE_NAME}: $(COMPONENT_OUTPUT_DIR)/${EXE_NAME}
	@nm ${COMPONENT_OUTPUT_DIR}/${EXE_NAME} | grep -v " [U|w] " | sort | cut -d" " -f1,3 > $(COMPONENT_OUTPUT_DIR)/${MAP_FILE_NAME} ; sed -i "1i $$(wc -l $(COMPONENT_OUTPUT_DIR)/${MAP_FILE_NAME} | cut -d" " -f1)" $(COMPONENT_OUTPUT_DIR)/${MAP_FILE_NAME}

clean:
	@echo "Deleting intermediate compilation results"
	@find $(COMPONENT_OUTPUT_DIR) -name "*.a" -or -name "*.o" -or -name "*.so" -or -name "*.d" -or -name "*.Tsymbols" | xargs rm
	@rm $(COMPONENT_OUTPUT_DIR)/${EXE_NAME}

clean_coverage:
	@find $(COMPONENT_OUTPUT_DIR) -name "*.gcda" -or -name "*.gcno" | xargs rm -f ; true

clean_all: clean clean_coverage

${BSIM_LIBS_DIR}/${VERSION_FILE}: version
	@if [[ -f "$<" ]]; then\
	  cp $< $@; \
	else \
	  echo "unknown" > $@; \
	fi

${BSIM_BIN_DIR}/% : $(COMPONENT_OUTPUT_DIR)/%
	@cp $< $@ 

install: ${BSIM_BIN_DIR}/${EXE_NAME}  ${BSIM_BIN_DIR}/${MAP_FILE_NAME} ${BSIM_LIBS_DIR}/${VERSION_FILE}

.PHONY: install clean clean_all compile all 
