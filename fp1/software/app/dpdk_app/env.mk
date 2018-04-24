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
 
DIR_INC = $(MAKEROOT)/include
DIR_FUNC_SRC = $(MAKEROOT)/func
DIR_EXAMPLE1_SRC = $(MAKEROOT)/example1
DIR_EXAMPLE2_SRC = $(MAKEROOT)/example2
DIR_FUNC_OBJS = $(MAKEROOT)/func_objs
DIR_EXECUTE_OBJS = $(MAKEROOT)/execute_objs
DIR_BINS = $(MAKEROOT)/bin

DPDK_CFLAGS = -march=native -I$(DPDK_INCLUDE_HOME)
SECUREC_CFLAGS = -I$(SECUREC_INCLUDE_HOME)
CFLAGS += -g -fPIC -std=gnu99 -D_GNU_SOURCE -D__VU9P__ $(SECUREC_CFLAGS) $(DPDK_CFLAGS) -I$(DIR_INC)

DPDK_LDFLAGS = -L$(DPDK_LIB_HOME) -lethdev -lrte_mbuf -lrte_mempool -lrte_ring -lrte_eal -lrte_pmd_acc
SECUREC_LDFLAGS = -L$(SECUREC_LIB_HOME) -lsecurec
LDFLAGS += -g -fPIC -lpthread -lrt -ldl -fstack-protector -Wl,-z,relro -Wl,-z,noexecstack $(SECUREC_LDFLAGS) $(DPDK_LDFLAGS)




