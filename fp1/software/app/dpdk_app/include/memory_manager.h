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

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define MEMORY_POOL_SEPERATE_RAGINE_NUM  (2)  //Used to optimize the maximum performance of the transceiver
#define MEMORY_POOL_READ_DATA_RAGINE  (0)
#define MEMORY_POOL_WRITE_DATA_RAGINE  MEMORY_POOL_READ_DATA_RAGINE
#define MEMORY_POOL_READ_HARD_ACC_RAGINE  (1)
#define MEMORY_POOL_WRITE_HARD_ACC_RAGINE  MEMORY_POOL_READ_HARD_ACC_RAGINE

#define MBUF_128B_SIZE (128)
#define MBUF_512K_SIZE (512*1024)
#define MBUF_1M_SIZE (1*1024*1024)
#define MBUF_2M_SIZE (2*1024*1024)
#define MBUF_4M_SIZE (4*1024*1024)
#define MBUF_8M_SIZE (8*1024*1024)
#define MBUF_16M_SIZE (16*1024*1024)
#define MBUF_32M_SIZE (32*1024*1024)
#define MBUF_64M_SIZE (64*1024*1024)
#define MBUF_256M_SIZE (256*1024*1024)
#define MBUF_512M_SIZE (512*1024*1024)

typedef enum tagMBUF_SIZE_ENUM
{
    MBUF_128B = 0,
    MBUF_512K,
    MBUF_1M,
    MBUF_2M,
    MBUF_4M,
    MBUF_8M,
    MBUF_16M,
    MBUF_32M,
    MBUF_64M,
    MBUF_256M,
    MBUF_512M,
    MBUF_MAX_NUM
}MBUF_SIZE_ENUM;

typedef struct tagMPoolCfg
{
    MBUF_SIZE_ENUM buf_type;
    unsigned int buf_num;
} MEM_POOL_CFG_STRU;

void memory_manager_exit(void);
int memory_manager_init( MEM_POOL_CFG_STRU * mem_pool_cfg );
int memory_manager_v2p( void* buff_vaddr, unsigned long long* buff_paddr );
int memory_manager_p2v( void* buff_paddr, unsigned long long* buff_vaddr );
/* For the limit case,memory pool without enough memory blocks, so divide the memory pool into N blocks to prevent the memory request from timing out. */
void * memory_manager_alloc_n_region(unsigned int buff_size, unsigned int reg_idx, unsigned int reg_num); 

void * memory_manager_alloc_block(unsigned int buff_size);
int memory_manager_free_block( void* buff_vaddr);

/*mbuf mempool max num*/
#define MAX_MEMPOOL_NUM     (MBUF_MAX_NUM)
#define MPOOL_NAME_SIZE     (32)

/*mbuf mempool management info struct*/
typedef struct tagMEM_POOL_CFG_DFX_STRU
{
    char name[MPOOL_NAME_SIZE];
    unsigned int buff_num;
    unsigned int buff_size;
    unsigned int used_num;
    unsigned int free_num;
    unsigned long long phy2virt_offset;
    struct rte_mempool* mem_pool;
    unsigned int flag_created;
    struct rte_mbuf * mbufs;
    void ** mbuf_addrs;
    unsigned long long buff_pool_start_addr;
    unsigned long long buff_pool_end_addr;
    unsigned long * bitmap;
    pthread_mutex_t alloc_mutex;
}MEM_POOL_CFG_DFX_STRU;

#endif
