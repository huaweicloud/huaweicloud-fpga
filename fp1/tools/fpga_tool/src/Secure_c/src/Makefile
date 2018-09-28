 #
 #   BSD LICENSE
 #
 #   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 #   All rights reserved.
 #
 #   Redistribution and use in source and binary forms, with or without
 #   modification, are permitted provided that the following conditions
 #   are met:
 #
 #     * Redistributions of source code must retain the above copyright
 #       notice, this list of conditions and the following disclaimer.
 #     * Redistributions in binary form must reproduce the above copyright
 #       notice, this list of conditions and the following disclaimer in
 #       the documentation and/or other materials provided with the
 #       distribution.
 #     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 #       contributors may be used to endorse or promote products derived
 #       from this software without specific prior written permission.
 #
 #   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 #   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 #   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 #   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 #   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 #   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 #   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 #   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 #   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 #   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 #   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 #
 

PROJECT=libsecurec.so
#if you need a debug version library, use "-g" instead of "-s -DNDEBUG -O2".
# If you compiler report a warning on "break strict-aliasing rules", there is no problem. If you need to clear all warnings, you can add "-fno-strict-aliasing" option to your compiler, but this will impact the performance a little.

CFLAG= -I ../include -Wall  -s -DNDEBUG -O2 
CXXFLAG= $(CFLAG)
CC=gcc
GCC=gcc
ARCH:=$(shell getconf LONG_BIT)

ifeq ($(ARCH), 64)
    CFLAG += -fPIC
    CXXFLAG += -fPIC
endif

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

$(PROJECT):$(OBJECTS)
	@mkdir -p ../obj
	mkdir -p ../lib
	$(GCC)  -shared -o ../lib/$@ $(patsubst %.o,../obj/%.o,$^) $(CFLAG) 
	ar crv ../lib/libsecurec.a $(patsubst %.o,../obj/%.o,$^)
	ranlib ../lib/libsecurec.a 
	#cp ../lib/libsecurec.so /usr/local/lib/libsecurec.so
	#you may add you custom commands here
	@echo "finish $(PROJECT)"
.c.o:
	@mkdir -p ../obj
	$(GCC) -c $< $(CFLAG) -o ../obj/$(patsubst %.c,%.o,$<)

clean:
	rm -rf *.o ../obj ../lib $(PROJECT)

