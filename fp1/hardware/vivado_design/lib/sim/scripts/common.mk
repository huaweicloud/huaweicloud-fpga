#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

SCRIPT_DIR = $(LIB_DIR)/sim/scripts
TEST_DIR = $(USER_DIR)/sim/tests
# Quiet mode switch
QUIET = 0
# Backgroud compile and running switch
BACKGROUD = 0

.PHONY: all

all: comp run

.PHONY: comp

comp:
	$(SCRIPT_DIR)/compile.sh $(TOOL) $(QUIET) $(BACKGROUD)

.PHONY: run

run:
	$(SCRIPT_DIR)/run.sh $(TOOL) $(QUIET) $(BACKGROUD)

.PHONY: clean

clean:
	$(SCRIPT_DIR)/clean.sh

.PHONY: distclean

distclean:
	rm -fr $(SCRIPT_DIR)/../precompiled/*
	$(SCRIPT_DIR)/clean.sh

.PHONY: wave

wave:
	$(SCRIPT_DIR)/wave.sh $(TOOL)

.PHONY: cov

cov:
	$(SCRIPT_DIR)/cov.sh $(TOOL)

.PHONY: lib

lib:
	$(SCRIPT_DIR)/simlib.sh $(TOOL)

.PHONY: list

list:
	source $(SCRIPT_DIR)/base.sh; get_valid_test $(TEST_DIR)

.PHONY: help

help :
	@echo "+------------------------------------------------------------------------+";
	@echo "|                     FACS HDK EXAMPLE MAKEFILE                          |";
	@echo "+------------------------------------------------------------------------+";
	@echo "|                                                                        |";
	@echo "| Usage:                                                                 |";
	@echo "|                                                                        |";
	@echo "| make TOOL=<target> [ TC=<testname>   : Testcase Name                   |";
	@echo "|                      TOOL=<simulator>: Select simulator                |";
	@echo "|                      QUIET=<1/0>     : Quiet Mode Enable(1 Enable)     |";
	@echo "|                      BACKGROUD=<1/0> : Backgroud Run Enable(1 Enable)] |";
	@echo "|                                                                        |";
	@echo "| Mandatory Arguments:                                                   |";
	@echo "| Available targets                                                      |";
	@echo "|   comp     : Compile testbench and testcase using specified simulator. |";
	@echo "|   run      : Execute a test using specified simulator.                 |";
	@echo "|   wave     : Open simulation waveform by specified simulator.          |";
	@echo "|   cov      : Generate coverage report by specified simulator.          |";
	@echo "|   clean    : Remove simulation files and directories.                  |";
	@echo "|   distclean: Remove simulation files, directories and precompiled libs.|";
	@echo "|   lib      : Precompile simulation library by specified simulator.     |";
	@echo "|   list     : Show all available testcase.                              |";
	@echo "|   help     : Show help information.                                    |";
	@echo "|                                                                        |";
	@echo "|                                                                        |";
	@echo "| Comment for TC                                                         |";
	@echo "|  Only available for target 'run' and 'wave'                            |";
	@echo "|  default is sv_demo_001                                                |";
	@echo "|                                                                        |";
	@echo "|                                                                        |";
	@echo "| Available simulator for TOOL                                           |";
	@echo "|  vivado, vcs and questa                                                |";
	@echo "|  default is vivado                                                     |";
	@echo "|  target cov is only available when TOOL is vcs or questa               |";
	@echo "|                                                                        |";
	@echo "+------------------------------------------------------------------------+";


