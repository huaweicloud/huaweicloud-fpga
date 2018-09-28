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

CC = xcpp

HOST_SRCS = test-cl.cpp
HOST_EXE_DIR=./
HOST_EXE = mmult
HOST_CFLAGS = -Wall -DFPGA_DEVICE -DC_KERNEL 
HOST_LFLAGS = -std=c++14


XDEVICE=huawei_vu9p_dynamic_fp1_5_0
HOST_CFLAGS+=-DTARGET_DEVICE=\"${XDEVICE}\"

HOST_CFLAGS += -I${XILINX_SDX}/runtime/include/1_2
HOST_LFLAGS += -L${XILINX_SDX}/runtime/lib/x86_64
HOST_LFLAGS += -L${XILINX_SDX}/lib/lnx64.o/ -lxilinxopencl -lrt -pthread -lstdc++ -lboost_filesystem

OBJECTS := $(HOST_SRCS:.cpp=.o)

.PHONY: all

all: host 

host: ${HOST_EXE_DIR}/${HOST_EXE}

${HOST_EXE_DIR}/${HOST_EXE} : ${OBJECTS}
	${CC} ${HOST_LFLAGS} ${OBJECTS} -o $@ 
%.o: %.cpp
	${CC} ${HOST_CFLAGS} -c $< -o $@

clean:
	${RM} -rf *.csv *.html ${HOST_EXE} ${OBJECTS}
