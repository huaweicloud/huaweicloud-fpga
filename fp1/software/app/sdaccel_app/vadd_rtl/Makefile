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
CXX := cpp

EXE := vadd
FLAGS := -Wall  

INC  := -I${XILINX_SDX}/runtime/include/1_2
LIBS := -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl -lstdc++

FLAGS += $(INC)  $(LIBS)


SRCS := $(wildcard *.$(CXX))
OBJS := $(patsubst %.$(CXX), %.o, $(SRCS))

-include $(OBJS:.o=.d)


all : $(EXE)

%.o: %.$(CXX)
	$(CC) $(FLAGS) -c $< -o $@
	$(CC) $(FLAGS) -c -MM $< -o $(patsubst %.o, %.d, $@)

$(EXE): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(FLAGS)

clean:
	rm -rf *.o *.d *.xo *.csv *.html $(EXE)   

.PHONY: all

.DEFAULT_GOAL := all

