
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <rte_config.h>
#include <rte_eal.h>
#include <rte_memzone.h>
#include <rte_mempool.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_debug.h>
#include <rte_errno.h>

#include "fpga_ddr_rw_common.h"
#include "ul_get_port_status.h"
#include "memory_manager.h"
#include "securec.h"

static struct rte_ring *g_queue_ring;
static pthread_mutex_t g_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_enqueue_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct thread_info g_thread_info[THREAD_MAX_NUM];
static Callbackfunc g_resultcallback;
static pthread_t tx_thread;
static pthread_t rx_thread;
static unsigned int port_id_used_flag[8] = { 0 };

int dpdk_env_init(void);
int dpdk_queues_init(void);
int dpdk_bd_mempool_create(unsigned int thread_id);
int dpdk_tx_rx_thread_init(void);
void thread_id_resource_init(void);

static int check_args_step_two() {
    uint8_t dev_count = 0;

    /* get the number of vf device */
    dev_count = rte_eth_dev_count();
    if (0 == dev_count) {
        printf("there is not any vf device\r\n");
        return -EINVAL;
    }
    printf("there are %u vf may be used\r\n", dev_count);

    return 0;
}
static void dev_info_dump(struct rte_eth_dev_info* dev_info) {
    if (NULL == dev_info) return;

    printf("driver_name=%s\n"
        "max_rx_queues=%u, max_tx_queues=%u\n",
        dev_info->driver_name,
        dev_info->max_rx_queues, dev_info->max_tx_queues);

    return;
}

int memory_manager_global_init(void) {
    MEM_POOL_CFG_STRU mem_pool_stru;
    int8_t ret = 0;

    /* 4M */
    /*mem_pool_stru.buf_type = MBUF_4M;
    mem_pool_stru.buf_num = 0;
    ret = memory_manager_init(&mem_pool_stru);
    if(ret)
    {
        printf("call memory_manager_init 4M error.\n");
        return ret;
    }
    */

    /* 512K */
    //mem_pool_stru.buf_type = MBUF_512K;
    //mem_pool_stru.buf_num = 5000;
    mem_pool_stru.buf_type = MBUF_128B;
    mem_pool_stru.buf_num = 2000000;
    ret = memory_manager_init(&mem_pool_stru);
    if(ret)
    {
        printf("call memory_manager_init error.\n");
        return ret;
    }

    return 0;
}
int dpdk_gloal_env_init(Callbackfunc callback) {
    int8_t ret = 0;

    ret = dpdk_env_init();
    if(ret) {
        printf("call dpdk_env_init fail.\r\n"); 
        return ret;
    }

    ret = dpdk_queues_init();
    if(ret) {
        printf("call dpdk_queues_init fail.\r\n"); 
        return ret;
    }

    g_resultcallback = callback;

    ret = pci_port_status_init_env();
    if(ret != 0) {
        printf("%s: pci_port_status_init_env failed(%d)\r\n", __FUNCTION__, ret);
		return ret;
    }

    ret = dpdk_tx_rx_thread_init();
    if(ret) {
        printf("call dpdk_queues_init fail.\r\n"); 
        return ret;
    }

    (void)thread_id_resource_init();

    return 0;
}

void thread_id_resource_init(void) {
    uint32_t idx = 0;

    for(idx = 0; idx < THREAD_MAX_NUM; idx++) {
        g_thread_info[idx].bd_mempool = NULL;
        g_thread_info[idx].thread_id = idx;
        g_thread_info[idx].used_flag = 0;
    }

    return;
}
int check_ddr_addr_valid(unsigned long fpga_ddr_addr, unsigned int length) {

    unsigned long long sixteen = 1024 * 1024 * 1024 * 16UL;
    if((fpga_ddr_addr < sixteen) && ((fpga_ddr_addr + length) > sixteen)) {
        return -1;
    }
    else if((fpga_ddr_addr < sixteen * 2) && ((fpga_ddr_addr + length) > sixteen * 2)) {
        return -2;
    }
    else if((fpga_ddr_addr < sixteen * 3) && ((fpga_ddr_addr + length) > sixteen * 3)) {
        return -3;
    }
    else if((fpga_ddr_addr < sixteen * 4) && ((fpga_ddr_addr + length) > sixteen * 4)) {
        return -4;
    }
    else {
        return 0;
    }
    
}
int check_thread_id_valid(unsigned int thread_id) {
    if(THREAD_MAX_NUM <= thread_id) {
        printf("check_thread_id_valid param thread_id %d error.\n", thread_id);
        return -1;
    }
    if(g_thread_info[thread_id].used_flag == 1) {
        return 0;
    }
    else {
        printf("thread_id %d not in use.\n", thread_id);
        return -2;
    }
}
int alloc_thread_id_resource(unsigned int *thread_id) {
    uint32_t idx = 0;
    int8_t ret = 0;

    (void)pthread_mutex_lock(&g_thread_mutex);
    for(idx = 0; idx < THREAD_MAX_NUM; idx++) {
        if(g_thread_info[idx].used_flag == 0) {
            break;
        }
    }

    if(idx >= THREAD_MAX_NUM) {
        (void)pthread_mutex_unlock(&g_thread_mutex);
        printf("it has not enough thread id .\n");
        return -1;
    }

    g_thread_info[idx].used_flag = 1;
    (void)pthread_mutex_unlock(&g_thread_mutex);

    ret = dpdk_bd_mempool_create(idx);
    if(ret) {
        printf("call dpdk_bd_mempool_create fail, thread id %d.\n", idx);
        return -2;
    }
    *thread_id = idx;
    
    return 0;
}

int free_thread_id_rersource(unsigned int thread_id) {

    (void)pthread_mutex_lock(&g_thread_mutex);
    if(g_thread_info[thread_id].used_flag == 0) {
        (void)pthread_mutex_unlock(&g_thread_mutex);
        printf("thread id not been used, can't free, thread id %d.\n", thread_id);
        return -1;
    }

    g_thread_info[thread_id].used_flag = 0;

    (void)pthread_mutex_unlock(&g_thread_mutex);

    return 0;
} 
void static_bd_pktmbuf_init(struct rte_mempool *mem_pool, void *arg, void *m, unsigned int mbuf_idx)
{
    struct rte_mbuf *mbuf = (struct rte_mbuf *)m;

    rte_pktmbuf_init(mem_pool, arg, m, mbuf_idx);

    mbuf->data_off = RTE_PKTMBUF_HEADROOM;

    rte_pktmbuf_data_len(mbuf) = sizeof(BD_MESSAGE_STRU);  
    rte_pktmbuf_pkt_len(mbuf) = sizeof(BD_MESSAGE_STRU);  

    return;
    
}

int dpdk_bd_mempool_create(unsigned int thread_id) {
    char  pool_name[32] = { 0 };
    
    (void)snprintf_s(pool_name, (sizeof(pool_name) - 1), (sizeof(pool_name) - 1), "%s_fpga_ddr_%d", BD_MEM_POOL_NAME, thread_id);
    g_thread_info[thread_id].bd_mempool = rte_mempool_lookup(pool_name);
    if(NULL == g_thread_info[thread_id].bd_mempool) {
        g_thread_info[thread_id].bd_mempool = rte_mempool_create(pool_name,   \
                        BD_MBUF_NUM_PER_MPOOL,          \
                        STATIC_BD_SIZE,                 \
                        0,                              \
                        sizeof(struct rte_pktmbuf_pool_private),    \
                        rte_pktmbuf_pool_init, NULL,    \
                        static_bd_pktmbuf_init, NULL, 0, 0);
        if ( NULL == g_thread_info[thread_id].bd_mempool ) {
            printf("[%s] line:%d [%s], rte_mempool_create failed\n", __FILE__, __LINE__, __FUNCTION__);
            return -1;
        }
    }

    return 0;
}
int dpdk_env_init(void) {
    uint32_t cpu_nb = 0;
    uint64_t cpu_mask = 0;
    uint16_t cpu_idx = 0;
    char str_cpu_nb[32] = {0};
    int argc_dpdk = 0;
    int status = 0;
    uint8_t  dev_count = 0;
    uint8_t port_idx = 0;
    char dev_name[256] = {0};
    
    cpu_nb = (uint32_t)get_nprocs();
    for (cpu_idx = 0; cpu_idx < cpu_nb; cpu_idx++) {
        cpu_mask = ((cpu_mask << 1) | 0x01);
    }
    (void)snprintf_s(str_cpu_nb, sizeof(str_cpu_nb), sizeof(str_cpu_nb) - 1, "-c%x", cpu_mask);
    printf("available cpu number: %d, cpu mask parameter: %s\r\n", cpu_nb, str_cpu_nb);
    
    char* argv_dpdk[]= {"fpga_ddr_rw", str_cpu_nb, "-n", "1", "--proc-type=primary"};
    argc_dpdk = sizeof(argv_dpdk)/sizeof(argv_dpdk[0]);

    /* dpdk env initialization */
    status = rte_eal_init(argc_dpdk, argv_dpdk);
    if (0 > status) {
        printf("%s:%d: rte_eal_init failed: %d\r\n", __FUNCTION__, __LINE__, status);
        return status;
    }

    /* check if any VF devices to use */
    if (0 != check_args_step_two()) {
        dev_count = rte_eth_dev_count();
        for(port_idx = 0; port_idx < dev_count; ++port_idx) {
            status = rte_eth_dev_detach(port_idx, dev_name);
            if (0 == status) {
                printf("rte_eth_dev_detach success for port: %u, dev_name: %s\r\n",port_idx, dev_name);
            } else {
                printf("\033[1;31;40mrte_eth_dev_detach fails for port: %u, dev_name: %s\033[0m\r\n",port_idx, dev_name);
            }
        }
        return -1;
    }

    return 0;
}

int dpdk_queues_port_init(uint32_t port_id) {
    int status = 0;
    int socket_id = 0;
    uint32_t queue_idx = 0;
    struct rte_mempool* mp = NULL;
    struct rte_eth_conf dev_conf; 
    struct rte_eth_dev_info dev_info;
    char z_name[RTE_MEMZONE_NAMESIZE]={0};
    
    (void)memset_s(&dev_conf, sizeof(struct rte_eth_conf), 0, sizeof(struct rte_eth_conf));
    (void)memset_s(&dev_info, sizeof(struct rte_eth_dev_info), 0, sizeof(struct rte_eth_dev_info));

    /* configure the dev, including Rx max queues and Tx max queues */
    status = rte_eth_dev_configure(port_id, QUEUES_MAX_NUM_EVERY_IP, QUEUES_MAX_NUM_EVERY_IP, &dev_conf);
    if (0 != status) {
       printf("rte_eth_dev_configure fails for port: %u\r\n", port_id); 
       return -1;
    }

    /* get the device information of port_id */
    rte_eth_dev_info_get(port_id, &dev_info);
    dev_info_dump(&dev_info);

    socket_id = rte_eth_dev_socket_id(port_id);
    printf("socket_id=%d for port: %u\r\n", socket_id, port_id);

    /* create mempool of tx/rx queue and setup the tx/rx queue */
    for (queue_idx = 0; queue_idx < QUEUE_USED; ++queue_idx) {
    
        status = snprintf_s(z_name, sizeof(z_name), sizeof(z_name) - 1, "%s_port_%u_queue_%u", PRIMARY_ENV_MEM_POOL_NAME, port_id, queue_idx);
        if(0 > status) {
            printf("\033[1;31;40msnprintf_s z_name: %s has some wrong\033[0m\r\n", z_name);
            return -1;
        }
                    
        mp = rte_mempool_create(z_name,
                PRIMARY_NB_MBUF, PRIMARY_MBUF_SIZE, PRIMARY_MBUF_CACHE_SIZE,
                sizeof(struct rte_pktmbuf_pool_private),
                rte_pktmbuf_pool_init, NULL, 
                rte_pktmbuf_init, NULL, 0, 
                MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
        if (NULL == mp) {
            printf("\033[1;31;40mcannot init mbuf(%s) pool for port: %u, queue %u\033[0m\r\n", 
                z_name, port_id, queue_idx);
            return -1;
        }
        printf("create port %u queue %u mempool(%s) success\r\n", port_id, queue_idx, z_name);
        if (0 != rte_eth_rx_queue_setup(port_id, queue_idx, QUEUE_DESC_MAX_NB, socket_id, NULL, mp)) {
            printf("\033[1;31;40mrte_eth_rx_queue_setup fails for port: %u, queue: %u\033[0m\r\n", port_id, queue_idx);
            return -1;
        }

        if (0 != rte_eth_tx_queue_setup(port_id, queue_idx, QUEUE_DESC_MAX_NB, socket_id, NULL)) {
            printf("\033[1;31;40mrte_eth_tx_queue_setup fails for port: %u, queue: %u\033[0m\r\n", port_id, queue_idx);
            return -1;
        }
    }

    /* after configuration, start the device */
    status = rte_eth_dev_start(port_id);
    if (0 != status) {
        printf("\033[1;31;40mrte_eth_dev_start failed %u\033[0m\r\n",status);
        return -1;
    }
    
    return 0;
}

int dpdk_queues_init(void) {
    uint8_t dev_count = 0;
    int8_t idx = 0;
    int8_t ret = 0;

    /* get the number of vf device */
    dev_count = rte_eth_dev_count();
    if (0 == dev_count) {
        printf("there is not any vf device\r\n");
        return -EINVAL;
    }

    for(idx = 0; idx < dev_count; idx++) {
        ret = dpdk_queues_port_init(idx);
        if(ret) {
            printf("call dpdk_queues_port_init fails for port: %u\r\n", idx);
            return ret;
        }
    }

    return 0;
}
void *fpga_ddr_rw_tx_process_thread(void *arg) {
    int8_t ret = 0;
    void *tx_msg;
    BD_MSG_INFO *bd_msg;
    int real_tx_nb = 0;
    int tx_retry_cnt = 0;
    (void)arg;
    
    while(1) {
        ret = rte_ring_sc_dequeue(g_queue_ring, &tx_msg);
        if (ret == -ENOENT) {
            (void)usleep(10);         
            continue; 
        }
        else if(ret != 0) {
            printf("call rte_ring_sc_dequeue error.\n");
            return NULL;
        }
        bd_msg = (BD_MSG_INFO*)tx_msg;

        /* send msg to recv thread to recv result */
        port_id_used_flag[bd_msg->port_id] = 0x1;

        tx_retry_cnt = 0;
        while(1) {
            real_tx_nb = rte_eth_tx_burst(bd_msg->port_id, 0, &(bd_msg->tx_bd_mbuf), 1);
            if(real_tx_nb == 1) {
                break;
            }
            else if(real_tx_nb == 0) {
                tx_retry_cnt++;
                if(tx_retry_cnt > 10000) {
                    printf("call rte_eth_tx_burst fail.!!!!!!!!!!\n");
                    return NULL;
                }
                (void)usleep(10);
                continue;
            }
            else {
                printf("rte_eth_tx_burst return value error.!!!!!!!!!!\n");
                return NULL;
            }
        }
        rte_pktmbuf_free(bd_msg->tx_bd_mbuf);
        free(tx_msg);
    }
}

void *fpga_ddr_rw_rx_process_thread(void *arg) {
    int rx_pktnum;
    struct rte_mbuf *mbuf = NULL;
    BD_MESSAGE_STRU *rx_bd = NULL;
    rw_ddr_data rw_data;
    unsigned long long vir_addr;
    int8_t ret = 0;
    HARDACC_STRU *hard_acc_src = NULL;
    HARDACC_STRU *hard_acc_dst = NULL;
    uint8_t dev_count = 0;
    uint8_t idx = 0;
    unsigned int port_id[8] = { 0 };
    unsigned int slot_id[8] = { 0 };
    unsigned int port_id_empty = 0;
    (void)arg;

    /* get device num and convert port_id to slot_id */
    dev_count = rte_eth_dev_count();
    for(idx = 0; idx < dev_count; idx++) {
        port_id[idx] = idx;
        ret = pci_port_id_to_slot_id(port_id[idx], &(slot_id[idx]));
        if(ret != 0) {
            printf("%s: pci_port_id_to_slot_id failed(%d)\r\n", __FUNCTION__, ret);
    		return NULL;
        }
    }
    
    while(1) {

        port_id_empty = 0;
        /* scan all devices to recv packages */
        for(idx = 0; idx < dev_count; idx++) {

            /* if no data send to XX port_id, just ignore it */
            if(port_id_used_flag[idx] == 0) {
                continue;
            }

            port_id_empty = 1;
            rx_pktnum = rte_eth_rx_burst(port_id[idx], 0, &mbuf, 1);
            if (rx_pktnum == 0) {
                (void)usleep(10);
                continue;
            }

            if (mbuf == NULL) {
                printf("rte_eth_rx_burst mbuf is NULL!\n");
                return NULL;
            }

            rx_bd = rte_pktmbuf_mtod(mbuf, BD_MESSAGE_STRU*);
            
            /* get src_addr of bd */
            ret = memory_manager_p2v((void*)(rx_bd->src_addr), &vir_addr);
            if(ret) {
                printf("call memory_manager_p2v src_addr fail!\n");
                return NULL;
            }
            hard_acc_src = (HARDACC_STRU*)vir_addr;

            /* get dst_addr of bd */
            ret = memory_manager_p2v((void*)(hard_acc_src->dst_addr), &vir_addr);
            if(ret) {
                printf("call memory_manager_p2v dst_addr fail!\n");
                return NULL;
            }
            hard_acc_dst = (HARDACC_STRU*)vir_addr;

            /*read data from fpga*/
            rw_data.length = hard_acc_dst->length - sizeof(HARDACC_STRU);
            if(hard_acc_dst->opcode == READ_MODE) {
                rw_data.cpu_vir_dst_addr = (unsigned long)vir_addr + sizeof(HARDACC_STRU);
                rw_data.cpu_vir_src_addr = 0x0;
                rw_data.fpga_ddr_rd_addr = hard_acc_dst->dst_fpga_phy_addr;
                rw_data.length = hard_acc_dst->length - sizeof(HARDACC_STRU);
                (void)memory_manager_free_bulk((void*)hard_acc_src);
                g_resultcallback(hard_acc_dst->thread_id, slot_id[idx], rw_data, READ_MODE);
            }
            else if(hard_acc_dst->opcode == WRITE_MODE) {
                rw_data.cpu_vir_dst_addr = 0x0;
                rw_data.cpu_vir_src_addr = (unsigned long long)((char*)hard_acc_src + sizeof(HARDACC_STRU));
                rw_data.fpga_ddr_wr_addr = hard_acc_dst->src_fpga_phy_addr;
                rw_data.length = hard_acc_dst->length - sizeof(HARDACC_STRU);
                (void)memory_manager_free_bulk((void*)vir_addr);
                g_resultcallback(hard_acc_dst->thread_id, slot_id[idx], rw_data, WRITE_MODE);
            }
            else if(hard_acc_dst->opcode == LOOPBACK_MODE) {
                rw_data.cpu_vir_dst_addr = (unsigned long long)vir_addr + sizeof(HARDACC_STRU);
                rw_data.cpu_vir_src_addr = (unsigned long long)((char*)hard_acc_src + sizeof(HARDACC_STRU));
                rw_data.fpga_ddr_rd_addr = hard_acc_dst->dst_fpga_phy_addr;
                rw_data.fpga_ddr_wr_addr = hard_acc_dst->src_fpga_phy_addr;
                rw_data.length = hard_acc_dst->length - sizeof(HARDACC_STRU);
                g_resultcallback(hard_acc_dst->thread_id, slot_id[idx], rw_data, LOOPBACK_MODE);
            }
            else {
                printf("something must be wrong, unvalid opcode\n");
            }
        }

        /* if no package comes, just delay 1ms */
        if(port_id_empty == 0) {
            (void)usleep(1000);
        }
    }
}


int dpdk_tx_rx_thread_init(void) {
    char  ring_name[32] = { 0 };
    int8_t ret = 0;
    
    (void)snprintf_s(ring_name, (sizeof(ring_name) - 1), (sizeof(ring_name) - 1), "%s_fpga_ddr", RING_QUEUE_NAME);
    g_queue_ring = rte_ring_lookup(ring_name);
    if(NULL == g_queue_ring) {
        g_queue_ring = rte_ring_create(ring_name, RING_QUEUE_NUM, SOCKET_ID_ANY, RING_F_SC_DEQ);
        if (g_queue_ring == NULL) {
            printf("rte_ring_create %s failed!\n", ring_name);
            return -1;
        }
    }

    /* create tx thread */
    ret = pthread_create(&tx_thread, NULL, fpga_ddr_rw_tx_process_thread, NULL);
    if (ret) {
        printf("pthread_create tx thread failed!\n");
        return ret;
    }

    /* create rx thread */
    ret = pthread_create(&rx_thread, NULL, fpga_ddr_rw_rx_process_thread, NULL);
    if (ret) {
        printf("pthread_create rx thread failed!\n");
        return ret;
    }

    return 0;
}
int read_data_from_ddr_func (unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data) {
    struct rte_mbuf *mbuf_bd = NULL;
    BD_MESSAGE_STRU *data_pos_in_mbuf = NULL;
    HARDACC_STRU *hard_acc = NULL;
    unsigned long long phy_addr;
    BD_MSG_INFO *bd_msg = NULL;
    int8_t ret = 0;

    /*alloc bd to shell.*/
    mbuf_bd = rte_pktmbuf_alloc(g_thread_info[thread_id].bd_mempool);
    if (NULL == mbuf_bd) {
        //printf("alloc shell bd resource fail.\n");
        return -1;
    }

    rte_pktmbuf_data_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);
    rte_pktmbuf_pkt_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);

    data_pos_in_mbuf = rte_pktmbuf_mtod(mbuf_bd, BD_MESSAGE_STRU*);
    (void)memset_s(data_pos_in_mbuf, sizeof(BD_MESSAGE_STRU), 0, sizeof(BD_MESSAGE_STRU));
 
    /*alloc hardacc mbuf */
    hard_acc = (HARDACC_STRU*)memory_manager_alloc_n_region(sizeof(HARDACC_STRU) + 32, MEMORY_POOL_READ_HARD_ACC_RAGINE, MEMORY_POOL_SEPERATE_RAGINE_NUM);
    if (NULL == hard_acc) {
        rte_pktmbuf_free(mbuf_bd);
        //printf("alloc hardacc resource fail.\n");
        return -2;
    }
    hard_acc->dst_fpga_phy_addr = rw_data.fpga_ddr_rd_addr;

    ret = memory_manager_v2p((void*)hard_acc, &phy_addr);
    if(ret) {
        
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        printf("call memory_manager_v2p hard_acc fail .\n");
        return -3;
    }
    /*write src_addr*/
    data_pos_in_mbuf->src_addr = phy_addr;

    ret = memory_manager_v2p((void*)(rw_data.cpu_vir_dst_addr - sizeof(HARDACC_STRU)), &phy_addr);
    if(ret) {
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        printf("call memory_manager_v2p cpu_vir_dst_addr fail .\n");
        return -4;
    }
    hard_acc->dst_addr = phy_addr;
    data_pos_in_mbuf->dst_addr = phy_addr;
    data_pos_in_mbuf->opcode = READ_MODE;         /*read data from fpga*/
    data_pos_in_mbuf->length = rw_data.length + sizeof(HARDACC_STRU);
    data_pos_in_mbuf->acc_type = 0x0;
    data_pos_in_mbuf->acc_length = 0x1;
    data_pos_in_mbuf->bd_code = 0x5a;
    data_pos_in_mbuf->thread_id = thread_id;

    bd_msg = (BD_MSG_INFO*)malloc(sizeof(BD_MSG_INFO));
    if(NULL == bd_msg) {
        printf("malloc bd_msg failed\n");
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        return -5;
    }
    bd_msg->port_id = port_id;
    bd_msg->tx_bd_mbuf = mbuf_bd;

    (void)pthread_mutex_lock(&g_enqueue_mutex);
    ret = rte_ring_mp_enqueue(g_queue_ring, bd_msg);
    if(ret) {
        printf("rte_ring_mp_enqueue failed\n");
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        free(bd_msg);
        (void)pthread_mutex_unlock(&g_enqueue_mutex);
        return ret;
    }
    (void)pthread_mutex_unlock(&g_enqueue_mutex);

    return 0;
}

int write_data_to_ddr_func (unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data) {
    struct rte_mbuf *mbuf_bd = NULL;
    BD_MESSAGE_STRU *data_pos_in_mbuf = NULL;
    HARDACC_STRU *hard_acc = NULL;
    void *dst_addr = NULL;
    unsigned long long phy_addr;
    BD_MSG_INFO *bd_msg = NULL;
    int8_t ret = 0;

    /*alloc bd to shell.*/
    mbuf_bd = rte_pktmbuf_alloc(g_thread_info[thread_id].bd_mempool);
    if (NULL == mbuf_bd) {
        //printf("alloc shell bd resource fail.\n");
        return -1;
    }

    rte_pktmbuf_data_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);
    rte_pktmbuf_pkt_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);

    data_pos_in_mbuf = rte_pktmbuf_mtod(mbuf_bd, BD_MESSAGE_STRU*);
    (void)memset_s(data_pos_in_mbuf, sizeof(BD_MESSAGE_STRU), 0, sizeof(BD_MESSAGE_STRU));
    
    /*alloc hardacc mbuf */
    hard_acc = (HARDACC_STRU*)(rw_data.cpu_vir_src_addr - sizeof(HARDACC_STRU));
    if (NULL == hard_acc) {
        rte_pktmbuf_free(mbuf_bd);
        printf("write_data_to_ddr_func hard_acc null.\n");
        return -2;
    }
    ret = memory_manager_v2p((void*)hard_acc, &phy_addr);
    if(ret) {
        
        rte_pktmbuf_free(mbuf_bd);
        printf("call memory_manager_v2p hard_acc fail .\n");
        return -3;
    }

    /*write src_addr*/
    data_pos_in_mbuf->src_addr = phy_addr;
    hard_acc->src_fpga_phy_addr = rw_data.fpga_ddr_wr_addr;
    
    /*alloc dst_addr mbuf */
    dst_addr = (HARDACC_STRU*)memory_manager_alloc_n_region(sizeof(HARDACC_STRU) + 32, MEMORY_POOL_WRITE_HARD_ACC_RAGINE, MEMORY_POOL_SEPERATE_RAGINE_NUM);
    if (NULL == dst_addr) {
        rte_pktmbuf_free(mbuf_bd);
        //printf("alloc hardacc resource fail.\n");
        return -2;
    }

    ret = memory_manager_v2p((void*)(dst_addr), &phy_addr);
    if(ret) {
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)dst_addr);
        printf("call memory_manager_v2p dst_addr fail .\n");
        return -3;
    }

    hard_acc->dst_addr = phy_addr;
    data_pos_in_mbuf->dst_addr = phy_addr;
    data_pos_in_mbuf->opcode = WRITE_MODE;          /*write data to fpga*/
    data_pos_in_mbuf->length = rw_data.length + sizeof(HARDACC_STRU);
    data_pos_in_mbuf->acc_type = 0x00;
    data_pos_in_mbuf->bd_code = 0x5a;
    data_pos_in_mbuf->thread_id = thread_id;

    bd_msg = (BD_MSG_INFO*)malloc(sizeof(BD_MSG_INFO));
    if(NULL == bd_msg) {
        printf("malloc bd_msg failed\n");
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        return -4;
    }
    bd_msg->port_id = port_id;
    bd_msg->tx_bd_mbuf = mbuf_bd;

    (void)pthread_mutex_lock(&g_enqueue_mutex);
    ret = rte_ring_mp_enqueue(g_queue_ring, bd_msg);
    if(ret) {
        printf("rte_ring_mp_enqueue failed\n");
        (void)memory_manager_free_bulk((void*)hard_acc);
        free(bd_msg);
        rte_pktmbuf_free(mbuf_bd);
        (void)pthread_mutex_unlock(&g_enqueue_mutex);
        return ret;
    }
    (void)pthread_mutex_unlock(&g_enqueue_mutex);

    return 0;    
}

int process_data_with_fpga_func(unsigned int thread_id, unsigned int port_id, rw_ddr_data rw_data) {
    struct rte_mbuf *mbuf_bd = NULL;
    BD_MESSAGE_STRU *data_pos_in_mbuf = NULL;
    HARDACC_STRU *hard_acc = NULL;
    unsigned long long phy_addr;
    BD_MSG_INFO *bd_msg = NULL;
    int8_t ret = 0;

    /*alloc bd to shell.*/
    mbuf_bd = rte_pktmbuf_alloc(g_thread_info[thread_id].bd_mempool);
    if (NULL == mbuf_bd) {
        //printf("alloc shell bd resource fail.\n");
        return -1;
    }

    rte_pktmbuf_data_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);
    rte_pktmbuf_pkt_len(mbuf_bd) = sizeof(BD_MESSAGE_STRU);

    data_pos_in_mbuf = rte_pktmbuf_mtod(mbuf_bd, BD_MESSAGE_STRU*);
    (void)memset_s(data_pos_in_mbuf, sizeof(BD_MESSAGE_STRU), 0, sizeof(BD_MESSAGE_STRU));

    /*alloc hardacc mbuf */
    hard_acc = (HARDACC_STRU*)(rw_data.cpu_vir_src_addr - sizeof(HARDACC_STRU));
    if (NULL == hard_acc) {
        rte_pktmbuf_free(mbuf_bd);
        printf("process_data_with_fpga_func hard_acc null .\n");
        return -2;
    }
    ret = memory_manager_v2p((void*)hard_acc, &phy_addr);
    if(ret) {
        
        rte_pktmbuf_free(mbuf_bd);
        printf("call memory_manager_v2p hard_acc fail .\n");
        return -3;
    }

    /*write src_addr*/
    data_pos_in_mbuf->src_addr = phy_addr;
    
    hard_acc->src_fpga_phy_addr = rw_data.fpga_ddr_wr_addr;
    hard_acc->dst_fpga_phy_addr = rw_data.fpga_ddr_rd_addr;

    ret = memory_manager_v2p((void*)(rw_data.cpu_vir_dst_addr - sizeof(HARDACC_STRU)), &phy_addr);
    if(ret) {
        
        rte_pktmbuf_free(mbuf_bd);
        printf("call memory_manager_v2p cpu_vir_dst_addr fail .\n");
        return -4;
    }
    hard_acc->dst_addr = phy_addr;
    data_pos_in_mbuf->dst_addr = phy_addr;
    data_pos_in_mbuf->opcode = LOOPBACK_MODE;         /*process data with fpga*/
    data_pos_in_mbuf->length = rw_data.length + sizeof(HARDACC_STRU);
    data_pos_in_mbuf->acc_type = 0x00;
    data_pos_in_mbuf->bd_code = 0x5a;
    data_pos_in_mbuf->thread_id = thread_id;

    bd_msg = (BD_MSG_INFO*)malloc(sizeof(BD_MSG_INFO));
    if(NULL == bd_msg) {
        printf("malloc bd_msg failed\n");
        rte_pktmbuf_free(mbuf_bd);
        (void)memory_manager_free_bulk((void*)hard_acc);
        return -5;
    }
    bd_msg->port_id = port_id;
    bd_msg->tx_bd_mbuf = mbuf_bd;

    (void)pthread_mutex_lock(&g_enqueue_mutex);
    ret = rte_ring_mp_enqueue(g_queue_ring, bd_msg);
    if(ret) {
        printf("rte_ring_mp_enqueue failed\n");
        rte_pktmbuf_free(mbuf_bd);
        free(bd_msg);
        (void)pthread_mutex_unlock(&g_enqueue_mutex);
        return ret;
    }
    (void)pthread_mutex_unlock(&g_enqueue_mutex);

    return 0; 
}
