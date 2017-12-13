#
#-------------------------------------------------------------------------------
#      Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
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

CC := xcpp
CLCC := xocc

CXX := cpp

EXE_PATH := ../prj/bin
EXE := ${EXE_PATH}/host
LOG_DIR=./../prj/log

XPFM = xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xpfm
XPFM_PATH := ../../../lib/platform/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1
KERNEL_NAME = vadd
TARGET = hw_emu

XCLBIN := ${EXE_PATH}/${KERNEL_NAME}.${TARGET}.$(basename ${XPFM}).xclbin
XO := ${KERNEL_NAME}.${TARGET}.$(basename ${XPFM}).xo


FLAGS := -Wall  

INC  := -I${XILINX_SDX}/runtime/include/1_2
LIBS := -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl -lstdc++


ifeq (${TARGET}, hw)
    TARGET = hw
endif


FLAGS += $(INC)  $(LIBS)


SRCS := $(wildcard *.$(CXX))
OBJS := $(patsubst %.$(CXX), %.o, $(SRCS))

-include $(OBJS:.o=.d)


all : $(XCLBIN) xconfig $(EXE)

%.o: %.$(CXX)
	$(CC) $(FLAGS) -c $< -o $@
	$(CC) $(FLAGS) -c -MM $< -o $(patsubst %.o, %.d, $@)

$(EXE): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(FLAGS)

$(XCLBIN): $(XO)
	$(CLCC) -f $(XPFM_PATH)/$(XPFM) --link $(XO) -t $(TARGET) -o $(XCLBIN)
	
$(XO):
	vivado -mode batch -nojournal -notrace -nolog -source  gen_xo.tcl -tclargs $(XO) $(KERNEL_NAME) $(TARGET) $(basename ${XPFM})

xconfig : emconfig.json

emconfig.json:
	emconfigutil -f $(XPFM_PATH)/$(XPFM)  --od $(EXE_PATH)
		
#
#rm -fr packaged_kernel_$(KERNEL_NAME)_$(TARGET)_$(basename ${XPFM})/
#rm -fr tmp_kernel_pack_$(KERNEL_NAME)_$(TARGET)_$(basename ${XPFM})/
clean:
	rm -rf *.o *.d *.xo ${LOG_DIR}/* $(EXE) emconfig.json $(XO_FILE) $(XCLBIN)  
	rm -fr packaged_kernel_*/
	rm -fr tmp_kernel_pack_*/
	rm -fr $(EXE_PATH)/*
	rm -fr .Xil/ 

.PHONY: all

.DEFAULT_GOAL := all

help:
	@echo "Compile  emulation using default huawei:vu9p-fp1:4ddr-xpr:4.1 DSA"
	@echo "make"
	@echo ""
	@echo "Compile XCLBIN file"
	@echo "make TARGET=hw"
	@echo ""
	@echo "Clean working diretory"
	@echo "make  clean"