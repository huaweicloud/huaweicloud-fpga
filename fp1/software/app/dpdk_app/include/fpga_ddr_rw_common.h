/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	_FPGA_DDR_RW_COMMON_H_
#define	_FPGA_DDR_RW_COMMON_H_

#include "fpga_ddr_rw_interface.h"

#define QUEUES_MAX_NUM_EVERY_IP     (8)
#define PRIMARY_ENV_MEM_POOL_NAME   "pri_mp"
#define PRIMARY_NB_MBUF (1024*8*4)
#define PRIMARY_MBUF_SIZE   (64 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define PRIMARY_MBUF_CACHE_SIZE (64)
#define QUEUE_DESC_MIN_NB   (1024)
#define QUEUE_DESC_MAX_NB   (1024*8)

#define BD_MEM_POOL_NAME "bd_pool"
#define BD_MBUF_NUM_PER_MPOOL  10240
#define STATIC_BD_SIZE         (64 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)

#define RING_QUEUE_NAME "queue_ring"
#define RING_QUEUE_NUM  65536
#define THREAD_MAX_NUM  10
#define QUEUE_USED      1
typedef struct thread_info
{
    struct rte_mempool *bd_mempool;
    unsigned int thread_id;
    int used_flag;
}THREAD_INFO;

#pragma pack(1)
typedef struct tagBD_MESSAGE_STRU
{
    unsigned long long src_addr         :64;        

    unsigned long long dst_addr         :64;        

    unsigned long long length           :32;        
    unsigned long long rsv1             :32;        
    
    unsigned long long rsv2             :16;        
    unsigned long long acc_type         :8;         
    unsigned long long acc_length       :8;         
    unsigned long long opcode           :2;         
    unsigned long long thread_id        :8;         
    unsigned long long rsv3             :6;        
    unsigned long long bd_code          :8;         
    unsigned long long rsv4             :8;         
} BD_MESSAGE_STRU;

typedef struct tagHARDACC_STRU {
    unsigned long long    src_fpga_phy_addr;
    unsigned long long    dst_fpga_phy_addr;
    unsigned short opcode       : 2;
    unsigned short thread_id    : 8;
    unsigned short rsv1         : 6;
    unsigned int length;
    unsigned long long dst_addr;
    unsigned char     rsv2[2];
}HARDACC_STRU;
#pragma pack()

typedef struct bd_msg_info
{
    struct rte_mbuf* tx_bd_mbuf;
    unsigned int port_id;
}BD_MSG_INFO;

typedef enum fpga_rw_mode
{
    READ_MODE = 0x1,
    WRITE_MODE = 0x2,
    LOOPBACK_MODE = 0x0
}FPGA_RW_MODE;

int dpdk_gloal_env_init(Callbackfunc callback);
int memory_manager_global_init(void);
int alloc_thread_id_resource(unsigned int *thread_id);
int free_thread_id_rersource(unsigned int thread_id);
int check_thread_id_valid(unsigned int thread_id);
int check_ddr_addr_valid(unsigned long fpga_ddr_addr, unsigned int length);
int read_data_from_ddr_func (unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data);
int write_data_to_ddr_func (unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data);
int process_data_with_fpga_func(unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data);
#endif

