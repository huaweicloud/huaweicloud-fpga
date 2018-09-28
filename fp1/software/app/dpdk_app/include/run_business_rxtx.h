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
#ifndef	__RUN_BUSINESS_RXTX__
#define	__RUN_BUSINESS_RXTX__

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#define QUEUE_DESC_MIN_NB   (1024)
#define QUEUE_DESC_MAX_NB   (1024*8)

#define VFS_MAX_NUM_EVERY_PF    (8)
#define QUEUES_MAX_NUM_EVERY_IP (8)

#define PACKET_NUM_MAX      (UINT32_MAX - 1024)
#define PACKET_NUM_DEFAULT  (128)
#define PACKET_LEN_MAX      (1024*1024)
#define PACKET_LEN_DEFAULT  (64)
#define LOOP_TIME_DEFAULT   (1)

#define SHELL_LOGIC_PAGE_SIZE (4096)
#define LOGIC_DATA_ALIGN    (64)

#define PRIMARY_ENV_MEM_POOL_NAME   "pri_mp"
#define PRIMARY_NB_MBUF (1024*8*4)
#define PRIMARY_MBUF_SIZE   (64 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define PRIMARY_MBUF_CACHE_SIZE (64)

#define SECONDARY_BD_ENV_MEM_POOL_NAME   "sec_mp_bd"
#define SECONDARY_BD_NB_MBUF (2048)
#define SECONDARY_BD_MBUF_SIZE   (64 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define SECONDARY_BD_MBUF_CACHE_SIZE (64)

#define SECONDARY_DATA_ENV_MEM_POOL_NAME   "sec_mp_data"
#define SECONDARY_DATA_NB_MBUF (SECONDARY_BD_NB_MBUF*2)
#define SECONDARY_DATA_MBUF_CACHE_SIZE (64)


/* FMMU -- FPGA DDR spec*/
#define FPGA_DDR_BASE       (0x0)
#define FPGA_DDR_ALL_SIZE(overflow_protect)   ((overflow_protect + 1024*1024*1024 - overflow_protect) * 64)
#define FPGA_DDR_PART_SIZE  ((8*1024*1024*1024))

#define FPGA_DDR_MODULE_NUM (4)                 
#define FPGA_DDR_STATUS_OK  (0x180)

/* align with byte */
#define SIZE_ALIGNED_BYTES(size, align)                     \
    do {                                                    \
        size = ((size + (align - 1)) / (align)) * align;    \
    } while(0)

typedef enum _stENABLE_FLAG_ {
    DISABLE  = 0,
    ENABLE = 1
}ENABLE_FLAG;

typedef enum _stTHREAD_FLAG_ {
    DPDK_BIND = 0,
    NORMAL_BIND
}THREAD_FLAG;

typedef struct _stBusinessPrimaryArg_ {
    uint16_t    queue_desc_nb;
    uint8_t     port_nb;        /* VF's number that want to configure */
} stBusinessPrimaryArg, *pstBusinessPrimaryArg;

typedef struct _stBusinessArg_ {
    uint16_t    port_id;
    uint16_t    queue_idx;
    uint32_t    packet_len;
    uint32_t    packet_num;
    struct rte_mempool* business_bd_mp;
    struct rte_mempool* business_data_mp;
    
    uint8_t     not_tx_thread;
    uint8_t     not_rx_thread;

    /* Hexadecimal bitmask of cores that TX thread to run on, the RX thread should run on the rx_thread_lcore_id+1 */
    uint8_t     tx_thread_lcore_id; 
    uint8_t     rx_thread_lcore_id;

    uint16_t    loop_time;
} stBusinessArg, *pstBusinessArg;

typedef struct _stBusinessArgs_ {
    uint32_t    port_used;
    uint32_t    slot_used;
    uint32_t    queue_used;
    uint32_t    port_ids[VFS_MAX_NUM_EVERY_PF];
    uint32_t    slot_ids[VFS_MAX_NUM_EVERY_PF];
    uint32_t    queue_idxs[QUEUES_MAX_NUM_EVERY_IP];
    uint32_t    packet_len;
    uint32_t    packet_num;

    uint8_t     not_tx_thread;
    uint8_t     not_rx_thread;

    uint16_t    loop_time;
    uint32_t    queue_desc_nb;

    ENABLE_FLAG     fmmu_enable;
    THREAD_FLAG     thread_flag;
    uint8_t     cpu_nb;
} stBusinessArgs, *pstBusinessArgs;

typedef struct _stBusinessThreadArg_ {
    pstBusinessArg p_business_arg;
    uint32_t    real_ret_nb;
}stBusinessThreadArg, *pstBusinessThreadArg;

typedef struct _stBusinessThreadArgs_ {
    pstBusinessArgs p_business_args;
    uint32_t    real_rx_nb;
    uint32_t    real_tx_nb;
    uint32_t    port_id;
    uint32_t    queue_idx;
    uint32_t    cpu_bind;
    THREAD_FLAG     thread_flag;
    uint32_t    tx_cpu_bind;
    uint32_t    rx_cpu_bind;
    pthread_t   task_id;
    struct rte_mempool* business_bd_mp;
    struct rte_mempool* business_data_mp;
}stBusinessThreadArgs, *pstBusinessThreadArgs;

int run_business_thread(pstBusinessThreadArgs p_business_thread_args);

#endif  /* __RUN_BUSINESS_RXTX__ */