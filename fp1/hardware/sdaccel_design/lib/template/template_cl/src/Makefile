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

ifndef XILINX_SDX
$(error Environment variable XILINX_SDX is required and should point to SDAccel install area)
endif


SHELL = /bin/bash
VPATH = ./

#supported flow: cpu_emu, hw_emu, hw
CC = xcpp
CLCC =xocc

SDA_FLOW = cpu_emu
HOST_SRCS := $(wildcard *.cpp) 
HOST_EXE_DIR=./../prj/bin
LOG_DIR=./../prj/log
HOST_EXE = XXX
HOST_CFLAGS = -g -Wall -DFPGA_DEVICE -DC_KERNEL 
HOST_LFLAGS = 

KERNEL_SRCS := $(wildcard *.cl) 
KERNEL_NAME = krnl_XXX
KERNEL_DEFS = 
KERNEL_INCS =
 

#set target device for XCLBIN
XDEVICE=huawei_vu9p_dynamic_fp1_5_0
XDEVICE_REPO_PATH=
KEEP_TEMP=1
KERNEL_DEBUG=
XCLBIN_NAME=../prj/bin/bin_${HOST_EXE}
HOST_CFLAGS+=-DTARGET_DEVICE=\"${XDEVICE}\"

ifeq (${SDA_FLOW},cpu_emu)
    CLCC_OPT += -t sw_emu
    XCLBIN = ${XCLBIN_NAME}_cpu_emu.xclbin
    XO = ${XCLBIN_NAME}_cpu_emu.xo
else ifeq (${SDA_FLOW},hw_emu)
    CLCC_OPT += -t hw_emu
    XCLBIN = ${XCLBIN_NAME}_hw_emu.xclbin
    XO = ${XCLBIN_NAME}_hw_emu.xo
else ifeq (${SDA_FLOW},hw)
    XCLBIN = ${XCLBIN_NAME}_hw.xclbin
    XO = ${XCLBIN_NAME}_hw.xo
    CLCC_OPT += -t hw
endif

HOST_ARGS = ${XCLBIN}


ifeq ($(XDEVICE_REPO_PATH),)
#no device repo path set. do nothing
    DEVICE_REPO_OPT = 
else
    DEVICE_REPO_OPT = --xp prop:solution.device_repo_paths=${XDEVICE_REPO_PATH} 
endif

XOCC_DCP_GEN = --xp param:compiler.acceleratorBinaryContent=dcp
PLATFORM_PATH=./../../../lib/platform/huawei_vu9p_dynamic_fp1_5_0/huawei_vu9p_dynamic_fp1_5_0.xpfm
HOST_CFLAGS += -I${XILINX_SDX}/runtime/include/1_2
HOST_LFLAGS += -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl -lstdc++
CLCC_OPT += $(CLCC_OPT_LEVEL) ${DEVICE_REPO_OPT} -f ${PLATFORM_PATH} ${XOCC_DCP_GEN} ${KERNEL_DEFS} ${KERNEL_INCS}

ifeq (${KEEP_TEMP},1)
    CLCC_OPT += -s
endif

ifeq (${KERNEL_DEBUG},1)
    CLCC_OPT += -g
endif

CLCC_OPT += --kernel ${KERNEL_NAME}
OBJECTS := $(HOST_SRCS:.cpp=.o)

.PHONY: all

all: run 

host: ${HOST_EXE_DIR}/${HOST_EXE}

xbin_cpu_em:
	make SDA_FLOW=cpu_emu xbin

xbin_hw_em:
	make SDA_FLOW=hw_emu xbin

xbin_hw :
	make SDA_FLOW=hw xbin

xbin: ${XCLBIN}

cpu_em: xconfig host xbin_cpu_em

hw_em: xconfig host xbin_hw_em

hw: host xbin_hw

estimate: 
	${CLCC} -c -t hw_emu -f ${PLATFORM_PATH} --report estimate ${KERNEL_SRCS} --temp_dir ./tempd --log_dir ./logd --report_dir ./repd
		
xconfig : emconfig.json

emconfig.json:
	emconfigutil -f ${PLATFORM_PATH} ${DEVICE_REPO_OPT} --od ${HOST_EXE_DIR}

${HOST_EXE_DIR}/${HOST_EXE} : ${OBJECTS}
	${CC} ${HOST_LFLAGS} ${OBJECTS} -o $@ 

${XCLBIN}:
	${CLCC} -c ${CLCC_OPT} ${KERNEL_SRCS} -o $(XO) --temp_dir ./tempd --log_dir ./logd --report_dir ./repd
	${CLCC} -l ${CLCC_OPT} $(XO) -o $(XCLBIN) --temp_dir ./tempd --log_dir ./logd --report_dir ./repd

%.o: %.cpp
	${CC} ${HOST_CFLAGS} -c $< -o $@

clean:
	${RM} -rf ${HOST_EXE} ${OBJECTS} ${XO} ${XCLBIN} ${LOG_DIR}/* *.bat ${HOST_EXE_DIR}/emconfig.json _xocc_*.dir iprepo _* .Xil 
	${RM} ${HOST_EXE_DIR}/${HOST_EXE} ${HOST_EXE_DIR}/* _xocc_*  TempConfig *.log *.jou sdaccel_profile_summary.* *.dat
	${RM} -rf *.o *.d tempd logd repd apsys*.xml dr.* ${XDEVICE}.hpfm dsa.xml address_map.xml *.ltx *.bit

help:
	@echo "Compile  emulation using default huawei:vu9p_dynamic:fp1:5_0 DSA"
	@echo "make  cpu_em"
	@echo ""
	@echo "Compile hardware emulation using default huawei:vu9p_dynamic:fp1:5_0 DSA"
	@echo "make   hw_em"
	@echo ""
	@echo "Compile host executable only"
	@echo "make  host"
	@echo ""
	@echo "Compile XCLBIN file for system run only"
	@echo "make xbin_hw"
	@echo ""
	@echo "Compile CPU emulation using huawei:vu9p_dynamic:fp1:5_0 DSA"
	@echo "make  XDEVICE=huawei_vu9p_dynamic_fp1_5_0 run_cpu_em"
	@echo ""
	@echo "Clean working diretory"
	@echo "make  clean"
