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
 
export MAKEROOT := $(shell pwd)

include ${MAKEROOT}/env.mk

MAKE_DIRS := ${DIR_FUNC_SRC} ${DIR_EXAMPLE1_SRC} ${DIR_EXAMPLE2_SRC} ${DIR_TOOLS_SRC} ${DIR_EXAMPLE3_SRC} ${DIR_EXAMPLE3_TEST_SRC}

define	build_obj
for SUBDIR in $(MAKE_DIRS); do		\
	if ! [ -d $$SUBDIR ]; then		\
		echo "The $$SUBDIR is not exist";	\
		exit 11;					\
	fi;								\
	echo "Buiding $$SUBDIR";		\
	make -C $$SUBDIR;				\
	if [ $$? -ne 0 ]; then			\
		echo "Build $$SUBDIR is failed!";	\
		exit 12;						\
	fi;								\
done
endef

all : 
	@$(call build_obj)

clean : 
	rm -rf $(DIR_FUNC_OBJS)/* $(DIR_EXECUTE_OBJS)/* $(DIR_BINS)/* ${DIR_LIBS}/* ${DIR_EXAMPLE3_SRC}/*.o ${DIR_EXAMPLE3_TEST_SRC}/*.o
