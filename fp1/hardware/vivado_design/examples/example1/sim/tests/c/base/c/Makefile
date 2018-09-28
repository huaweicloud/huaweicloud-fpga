#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "Project-License" file 
#      accompanying this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

SRCS     := ./src/*.c

INC      := ./include

MACROS   := VIVADO

OUTPUT   := ../../../../work/lib_test.so

CC       := gcc

RM       := rm -fv

CFLAGS   := -g3 -O0 -fPIC -Wall
LDFLAGS  := -shared
CFLAGS   += $(addprefix -I,$(INC))
CFLAGS   += $(addprefix -D,$(MACROS))

OBJS  	 := $(patsubst %.c,%.o,$(patsubst %.c,%.o,$(SRCS)))
DEPS   	 := $(patsubst %.o,%.d,$(OBJS))

%.d : %.c
	@$(CC) $(CFLAGS) -MM $< | \
	sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp
	@mv -f $@.tmp $@

.PHONY : all deps objs clean distclean rebuild help

all : $(OUTPUT)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM) $(DEPS)
	@$(RM) $(OBJS)

distclean : clean
	@$(RM) $(OUTPUT)

rebuild : distclean all

sinclude $(DEPS)

$(OUTPUT) : $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) $(LDFLAGS)
