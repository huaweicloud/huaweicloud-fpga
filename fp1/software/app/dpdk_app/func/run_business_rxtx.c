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
#include <unistd.h>
#include <linux/limits.h>
#include <stdint.h>
#include <sys/time.h>
#include <assert.h>
#include <rte_config.h>
#include <rte_eal.h>
#include <rte_mempool.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_debug.h>
#include "new_bd.h"
#include "run_business_rxtx.h"
#include "securec.h"
#include "regs_infos.h"
#include "pci_rw_tool_func.h"


/* get the number of bit 1 in data (lenth)*/
static int get_set_bits_number(uint8_t* data, uint16_t length, uint16_t* output);
int run_business_tx(uint16_t port_id, uint16_t queue_idx, 
        struct rte_mbuf** tx_mbufs, uint16_t require_tx_nb, 
        uint16_t* real_tx_nb);

int run_business_rx(uint16_t port_id, uint16_t queue_idx,
        struct rte_mbuf** rx_mbufs, uint16_t require_rx_nb, 
        uint16_t* real_rx_nb);

static int run_business_tx_thread_route(void* arg);
static int run_business_rx_thread_route(void* arg); 

/*
 * RX_VALID_VERIFY only used to check the consistency of TX/RX because it may result in low performance.
*/

/*
 *  CRC check is handled by logic. The user layer caculate the CRC and record it at the last two bytes of message.
 *  Logic also caculate it internally and compare those two value.
 *  Inside logic, there has registers to enable this function. There is also a warning register corresponding to 
 *  this function, this register will alarm if CRC check error. This function is closed by default. If you want enable
 *  it, you should do the following for the logic at the host:
 *  1¡¢ Set CRC register which address is 0x4000e to 1 ( 0 by defalut).
 *  2¡¢ If bit[25] of warning register is 1, it indicates CRC error.
 *  This function will reduce the performance of logic which only used in debug mode.
*/


#define MBUFS_NB_IN_POOL    SECONDARY_BD_NB_MBUF
typedef struct _st_mbufs_pool_ {
    struct rte_mbuf* tx_bd_mbufs[MBUFS_NB_IN_POOL];
    struct rte_mbuf* rx_bd_mbufs[MBUFS_NB_IN_POOL];
    struct rte_mbuf* tx_data_mbufs[MBUFS_NB_IN_POOL];
    struct rte_mbuf* rx_data_mbufs[MBUFS_NB_IN_POOL];

    uint16_t    head;        
    uint16_t    tail;        

    uint16_t    data_mbuf_len;
    uint32_t port_id;
    uint32_t queue_idx;
    struct rte_mempool* bd_mp;
    struct rte_mempool* data_mp;
} st_mbufs_pool, *pst_mbufs_pool;

st_mbufs_pool   g_mbufs_pool;
pst_mbufs_pool  g_p_mbufs_pool = &g_mbufs_pool;
st_mbufs_pool   g_mbufs_pools[VFS_MAX_NUM_EVERY_PF][QUEUES_MAX_NUM_EVERY_IP];

int mbufs_pool_init(pst_mbufs_pool p_mbufs_pool, pstBusinessThreadArgs p_business_thread_args);
void mbufs_pool_free(pst_mbufs_pool p_mbufs_pool);

static int get_set_bits_number(uint8_t* data, uint16_t length, uint16_t* output) {
    uint16_t byte_idx = 0;
    uint8_t byte_value = 0;
    uint8_t bit_idx = 0;
    uint16_t bit_set_nb = 0;
    if (NULL == data) {
        printf("\033[1;31;40minput data is NULL!\033[0m\r\n");
        return -EINVAL;
    }
    if (0 == length) {
        printf("\033[1;31;40mlenght is ZERO!\033[0m\r\n");
        return -EINVAL;
    }
    if (NULL == output) {
        printf("\033[1;31;40moutput is NULL!\033[0m\r\n");
        return -EINVAL;
    }
    for (byte_idx = 0; byte_idx < length; ++byte_idx) {
        byte_value = data[byte_idx];
        for (bit_idx = 0; bit_idx < 8; ++bit_idx) {
            if (1 == (byte_value & 0x01) ) {
                ++bit_set_nb;
            }
            byte_value = byte_value >> 1;
        }
    }
    *output = bit_set_nb;
    return 0;
}

int mbufs_pool_init(pst_mbufs_pool p_mbufs_pool, pstBusinessThreadArgs p_business_thread_args) {
    int status = 0;
    uint8_t* data_tx = NULL;
    uint8_t* data_rx = NULL;
    uint32_t data_pos_idx = 0;
    uint8_t  data_tx_value = 0;
    uint16_t mbuf_idx = 0;
    acc_tx_bd* bd_tx = NULL;
    uint8_t* bd_tx_parity  = NULL;
    uint16_t parity_cal_time = 0;
    uint16_t parity_cal_last_time_byte = 0;
    uint16_t parity_cal_idx = 0;
    uint16_t set_bits_nb = 0;
    uint8_t  parity_cal_valid_bytes = 0;
    uint8_t* partity_content = NULL;
    uint8_t  partity_content_value = 0;
    uint32_t tx_data_len = 0;
    uint32_t secondary_data_size = 0;

    /* Calculate the real data size in mbuf which will be sent to LOGIC, should be aligned as LOGIC requirement */
    secondary_data_size = p_business_thread_args->p_business_args->packet_len;
    SIZE_ALIGNED_BYTES(secondary_data_size, LOGIC_DATA_ALIGN);
	
    tx_data_len = p_business_thread_args->p_business_args->packet_len;

    /* Alloc bulk for bd_mbufs of TX*/
    status = rte_pktmbuf_alloc_bulk(p_mbufs_pool->bd_mp, p_mbufs_pool->tx_bd_mbufs, MBUFS_NB_IN_POOL);
    if (0 != status) {
        printf("\033[1;31;40mallocate tx bd mbufs failed\033[0m\r\n");
        goto init_error;
    }
    
    /* Alloc bulk for data_mbufs(payload) of TX*/
    status = rte_pktmbuf_alloc_bulk(p_mbufs_pool->data_mp, p_mbufs_pool->tx_data_mbufs, MBUFS_NB_IN_POOL);
    if (0 != status) {
        printf("\033[1;31;40mallocate tx data mbufs failed\033[0m\r\n");
        goto init_error;
    }

    /* Alloc bulk for data_mbufs(payload) of RX*/
    status = rte_pktmbuf_alloc_bulk(p_mbufs_pool->data_mp, p_mbufs_pool->rx_data_mbufs, MBUFS_NB_IN_POOL);
    if (0 != status) {
        printf("\033[1;31;40mallocate rx data mbufs failed\033[0m\r\n");
        goto init_error;
    }

    for (mbuf_idx = 0; mbuf_idx < MBUFS_NB_IN_POOL; ++mbuf_idx) {

        /*  tx_data_len is not recommend greater than 65535(0xFFFF), because rte_mbuf.pkt_len is uint32_t, but rte_mbuf.data_len is uint16_t,
         *  then rte_pktmbuf_data_len != rte_pktmbuf_pkt_len, so we recommend message which length greater than 32KB is sent in segmented format.
         */
        rte_pktmbuf_data_len(p_mbufs_pool->tx_data_mbufs[mbuf_idx]) = tx_data_len;
        rte_pktmbuf_pkt_len(p_mbufs_pool->tx_data_mbufs[mbuf_idx]) = tx_data_len;
        rte_pktmbuf_data_len(p_mbufs_pool->rx_data_mbufs[mbuf_idx]) = tx_data_len;
        rte_pktmbuf_pkt_len(p_mbufs_pool->rx_data_mbufs[mbuf_idx]) = tx_data_len;

        data_tx = rte_pktmbuf_mtod(p_mbufs_pool->tx_data_mbufs[mbuf_idx], uint8_t*);
        data_rx = rte_pktmbuf_mtod(p_mbufs_pool->rx_data_mbufs[mbuf_idx], uint8_t*);

        /* Initialization memory area of data_tx and data_rx(0xFF) */
        for (data_pos_idx = 0; data_pos_idx < tx_data_len; ++data_pos_idx) {
            data_tx[data_pos_idx] = data_tx_value;
            data_rx[data_pos_idx] = 0xFF;
        }
        /* data type of data_tx_value is uint8_t£¬when increased to 255, it will overflow to 0 */
        data_tx_value++;
        if (ENABLE == p_business_thread_args->p_business_args->fmmu_enable) {
            acc_second_bd *extend_data_tx = (acc_second_bd *)data_tx;

            uint64_t overflow_protect = 1;

            (void)memset(extend_data_tx, 0, sizeof(acc_second_bd));

            /* For FMMU, send data to FPGA_DDR_MODULE_NUM DDRs to balance the load,
               calucate the FPGA DDR addr with DDR idx, queue idx and mbuf idx */
            extend_data_tx->src_fpga_phy_addr = FPGA_DDR_BASE + 
                (mbuf_idx % FPGA_DDR_MODULE_NUM) * (FPGA_DDR_ALL_SIZE/FPGA_DDR_MODULE_NUM) + 
                (p_mbufs_pool->queue_idx) * MBUFS_NB_IN_POOL * secondary_data_size +
                (mbuf_idx / FPGA_DDR_MODULE_NUM * secondary_data_size);
    
            extend_data_tx->dst_fpga_phy_addr = extend_data_tx->src_fpga_phy_addr;
        }

        /* Initialization memory area of bd_tx */
        bd_tx = rte_pktmbuf_mtod(p_mbufs_pool->tx_bd_mbufs[mbuf_idx], acc_tx_bd*);
        (void)memset_s(bd_tx, sizeof(acc_tx_bd), 0, sizeof(acc_tx_bd));
        bd_tx->src_phy_addr = rte_pktmbuf_mtophys(p_mbufs_pool->tx_data_mbufs[mbuf_idx]);
        bd_tx->dst_phy_addr = rte_pktmbuf_mtophys(p_mbufs_pool->rx_data_mbufs[mbuf_idx]);
        bd_tx->length = rte_pktmbuf_pkt_len(p_mbufs_pool->tx_data_mbufs[mbuf_idx]);
        bd_tx->acc_type = 0x0;
        bd_tx->acc_length = 0x1;
        bd_tx->bd_code = 0x5a;
        rte_pktmbuf_data_len(p_mbufs_pool->tx_bd_mbufs[mbuf_idx]) = rte_pktmbuf_pkt_len(p_mbufs_pool->tx_bd_mbufs[mbuf_idx]) = sizeof(acc_tx_bd);

        /* parity check is performed every four bytes */
        bd_tx_parity = rte_pktmbuf_mtod(p_mbufs_pool->tx_bd_mbufs[mbuf_idx], uint8_t*);
        
        /* sizeof(uint8_t) is the storage area of parity value in BD, so it do not participate in caculation itself */
        parity_cal_time = (sizeof(acc_tx_bd)-sizeof(uint8_t)-1)/sizeof(uint32_t) + 1;
        parity_cal_valid_bytes = sizeof(acc_tx_bd)-sizeof(uint8_t);
        parity_cal_last_time_byte = (0 == (parity_cal_valid_bytes)%sizeof(uint32_t)) ? sizeof(uint32_t) : parity_cal_valid_bytes%sizeof(uint32_t);
        parity_cal_idx = 0;
        set_bits_nb = 0;
        partity_content_value = 0;
        partity_content = &bd_tx->odd_even.odd_even_all;

        for (parity_cal_idx=0; parity_cal_idx<parity_cal_time; ++parity_cal_idx) {
            set_bits_nb = 0;
            if (parity_cal_idx == (parity_cal_time-1)) {    /* last round */
                if (0 != get_set_bits_number(bd_tx_parity, parity_cal_last_time_byte, &set_bits_nb)) {
                    goto init_error;
                }
            } else {
                if (0 != get_set_bits_number(bd_tx_parity, sizeof(uint32_t), &set_bits_nb)) {
                    goto init_error;
                } else {
					bd_tx_parity += sizeof(uint32_t);
                }
            }
            if (1 == set_bits_nb%2) {
                /*partity_content |= (0x1 << parity_cal_idx);*/
                partity_content_value |= (0x1 << parity_cal_idx);
            }
        }
        *partity_content = partity_content_value;
    }
    p_mbufs_pool->head = p_mbufs_pool->tail = 0;
    p_mbufs_pool->data_mbuf_len = tx_data_len;
    return 0;
init_error:
    mbufs_pool_free(p_mbufs_pool);
    return -1;
}

void mbufs_pool_free(pst_mbufs_pool p_mbufs_pool) {
    uint16_t mbuf_idx = 0;
    for (mbuf_idx = 0; mbuf_idx < MBUFS_NB_IN_POOL; ++mbuf_idx) {
        if (NULL != p_mbufs_pool->tx_bd_mbufs[mbuf_idx]) {
            rte_pktmbuf_free(p_mbufs_pool->tx_bd_mbufs[mbuf_idx]);
            p_mbufs_pool->tx_bd_mbufs[mbuf_idx] = NULL;
        }
        if (NULL != p_mbufs_pool->tx_data_mbufs[mbuf_idx]) {
            rte_pktmbuf_free(p_mbufs_pool->tx_data_mbufs[mbuf_idx]);
            p_mbufs_pool->tx_data_mbufs[mbuf_idx] = NULL;
        }
        if (NULL != p_mbufs_pool->rx_data_mbufs[mbuf_idx]) {
            rte_pktmbuf_free(p_mbufs_pool->rx_data_mbufs[mbuf_idx]);
            p_mbufs_pool->rx_data_mbufs[mbuf_idx] = NULL;
        }
    }
}

int run_business_thread(pstBusinessThreadArgs p_business_thread_args) {
    uint32_t real_tx_nb = 0;
    uint32_t real_rx_nb = 0;
    uint32_t cpu_rx = 0;
    uint32_t cpu_tx = 0;
    uint32_t port_id = 0;
    uint32_t queue_idx = 0;

    port_id = p_business_thread_args->port_id;
    queue_idx = p_business_thread_args->queue_idx;
    g_mbufs_pools[port_id][queue_idx].port_id = port_id;
    g_mbufs_pools[port_id][queue_idx].queue_idx = queue_idx;
    g_mbufs_pools[port_id][queue_idx].bd_mp = p_business_thread_args->business_bd_mp;
    g_mbufs_pools[port_id][queue_idx].data_mp = p_business_thread_args->business_data_mp;

    /* Initialize the global g_mbufs_pools (only once) */
    if (0 != mbufs_pool_init(&g_mbufs_pools[port_id][queue_idx], p_business_thread_args)) {
        printf("mbufs_pool_init failed\r\n");
        return -1;
    }

    /* create RX thread, CPU sequential binding, cpu0 reserved */
    if (0 == p_business_thread_args->p_business_args->not_rx_thread) {
        cpu_rx = p_business_thread_args->cpu_bind;
        int ret = rte_eal_remote_launch(run_business_rx_thread_route, (void*)p_business_thread_args, cpu_rx);
        if (0 != ret) {
            printf("Create RX thread failed");
            goto error;
        }
    }
    
    /* create TX thread, CPU sequential binding */
    if (0 == p_business_thread_args->p_business_args->not_tx_thread) {
        cpu_tx = p_business_thread_args->cpu_bind+1;
        int ret = rte_eal_remote_launch(run_business_tx_thread_route, (void*)p_business_thread_args, cpu_tx);
        if (0 != ret) {
            printf("Create TX thread failed");
            goto error;
        }
    }

    if (0 == p_business_thread_args->p_business_args->not_rx_thread) {
        /*printf("rte_eal_wait_lcore rx for lcore %u\r\n", cpu_rx);*/
        (void)rte_eal_wait_lcore(cpu_rx);
    }
    if (0 == p_business_thread_args->p_business_args->not_tx_thread) {
        /*printf("rte_eal_wait_lcore tx for lcore %u\r\n", cpu_tx);*/
        (void)rte_eal_wait_lcore(cpu_tx);
    }
    real_tx_nb = p_business_thread_args->real_tx_nb;
    real_rx_nb = p_business_thread_args->real_rx_nb;
    if ((real_tx_nb == p_business_thread_args->p_business_args->packet_num) 
            && (real_rx_nb == p_business_thread_args->p_business_args->packet_num)) {
        printf("\033[1;32;40mport %u, queue %u TX/RX all success, all process %lu packets\033[0m\n\n\r\n",  \
            port_id, queue_idx, p_business_thread_args->p_business_args->packet_num);
    } else {
        printf("\033[1;31;40mport %u, queue %u TX/RX some error, all process TX %lu packets, RX %lu packets, Require %lu packets\033[0m\r\n",   \
            port_id, queue_idx, real_tx_nb, real_rx_nb, p_business_thread_args->p_business_args->packet_num);
        goto error;
    }

    /* Free the global g_mbufs_pools in the end */
    mbufs_pool_free(&g_mbufs_pools[port_id][queue_idx]);
    return 0;
    
error:
    mbufs_pool_free(&g_mbufs_pools[port_id][queue_idx]);
    return -1;
}

#define MAX_CONTINUE_TXRX_ZERO_TIME     (100000)

int run_business_tx(uint16_t port_id, uint16_t queue_idx, 
        struct rte_mbuf** tx_mbufs, uint16_t require_tx_nb, 
        uint16_t* real_tx_nb) {
    uint16_t    remain_tx_nb = require_tx_nb;
    uint16_t    has_tx_nb = 0;
    uint32_t    continue_tx_zero_nb = 0;
    static struct timespec ts;        
    ts.tv_sec = 0;        
    ts.tv_nsec = 100000;
    *real_tx_nb = 0;
    while (remain_tx_nb > 0) {

        /* data flip is detected */
        if (has_tx_nb > require_tx_nb)
        {
            printf("\033[1;31;40mhas_tx_nb(%u) > require_tx_nb(%u), It's some fault\033[0m\r\n", has_tx_nb, require_tx_nb);
            return -1;
        }

        /* send the message to queue_idx of port_id(device)
         * return value curr_tx_nb is the real package number has been send(curr_tx_nb <= remain_tx_nb) */
        uint16_t curr_tx_nb = rte_eth_tx_burst(port_id, queue_idx, &tx_mbufs[has_tx_nb], remain_tx_nb);
        if (0 == curr_tx_nb) {        
            ++continue_tx_zero_nb;
            (void)nanosleep(&ts, NULL); 
            if (MAX_CONTINUE_TXRX_ZERO_TIME <= continue_tx_zero_nb) {
                printf("\033[1;31;40mTX Zero too many time, return; "
                    "port_id(%u), queue_idx(%u), require_tx_nb(%u), real_tx_nb(%u), has_tx_nb(%u)\033[0m\r\n", 
                    port_id, queue_idx, require_tx_nb, *real_tx_nb, has_tx_nb);
                return -1;
            }
        } else {

            /* update the statistical information */
            remain_tx_nb -= curr_tx_nb;
            has_tx_nb += curr_tx_nb;
            *real_tx_nb += curr_tx_nb;
            continue_tx_zero_nb = 0;
        }
    }
    
    /* Something must be wrong if  *real_tx_nb not equal require_tx_nb */
    if (*real_tx_nb != require_tx_nb) {
        printf("\033[1;31;40m*real_tx_nb(%u) != require_tx_nb(%u)\033[0m\r\n",*real_tx_nb, require_tx_nb);
        return -1;
    }

    return 0;
}

int run_business_rx(uint16_t port_id, uint16_t queue_idx,
        struct rte_mbuf** rx_mbufs, uint16_t require_rx_nb, 
        uint16_t* real_rx_nb) {
    uint16_t    remain_rx_nb = require_rx_nb;
    uint16_t    has_rx_nb = 0;
    uint32_t    continue_rx_zero_nb = 0;
    static struct timespec ts;        
    ts.tv_sec = 0;        
    ts.tv_nsec = 100000;
    *real_rx_nb = 0;
    while (remain_rx_nb > 0) {

        /* data flip is detected */
        if (has_rx_nb > require_rx_nb)
        {
            printf("\033[1;31;40mhas_rx_nb(%u) > require_rx_nb(%u), It's some fault\033[0m\r\n", has_rx_nb, require_rx_nb);
            return -1;
        }

        /* receive the message to queue_idx of port_id(device)
         * return value curr_rx_nb is the real package number has been received(curr_rx_nb <= remain_rx_nb) */
        uint16_t curr_rx_nb = rte_eth_rx_burst(port_id, queue_idx, &rx_mbufs[has_rx_nb], remain_rx_nb);
        if (0 == curr_rx_nb) {           
            ++continue_rx_zero_nb;
            (void)nanosleep(&ts, NULL); 
            if (MAX_CONTINUE_TXRX_ZERO_TIME <= continue_rx_zero_nb) {
                printf("\033[1;31;40mRX Zero too many time, return; "
                    "port_id(%u), queue_idx(%u), require_rx_nb(%u), real_rx_nb(%u), has_rx_nb(%u), continue_rx_zero_nb(%u)\033[0m\r\n", 
                    port_id, queue_idx, require_rx_nb, *real_rx_nb, has_rx_nb, continue_rx_zero_nb);
                return -1;
            }
        } else {
            /* update the statistical information */
            remain_rx_nb -= curr_rx_nb;
            has_rx_nb += curr_rx_nb;
            *real_rx_nb += curr_rx_nb;
            continue_rx_zero_nb = 0;
        }
    }

    /* Something must be wrong if  *real_rx_nb not equal require_rx_nb */
    if (*real_rx_nb != require_rx_nb) {
        printf("\033[1;31;40m*real_rx_nb(%u) != require_rx_nb(%u)\033[0m\r\n",*real_rx_nb, require_rx_nb);
        return -1;
    }

    return 0;
}

static int run_business_tx_thread_route(void* arg) {
    pstBusinessThreadArgs pst_tx_thead_args = (pstBusinessThreadArgs)arg;
    pst_tx_thead_args->real_tx_nb = 0;

    uint32_t    tx_time = pst_tx_thead_args->p_business_args->packet_num / MBUFS_NB_IN_POOL;
    uint16_t    tx_last_time_nb = pst_tx_thead_args->p_business_args->packet_num % MBUFS_NB_IN_POOL;
    uint32_t    tx_time_idx = 0;
    int         status = 0;
    struct timeval  tx_start_time, tx_end_time;
    (void)gettimeofday(&tx_start_time, NULL);

    for (tx_time_idx = 0; tx_time_idx < (tx_time + 1); ++tx_time_idx) {
        uint16_t    cur_require_tx_nb = (tx_time_idx==tx_time) ? tx_last_time_nb : MBUFS_NB_IN_POOL;
        uint16_t    curr_real_tx_nb = 0;
        if (cur_require_tx_nb == 0) {
            break;
        }
        status = run_business_tx(pst_tx_thead_args->port_id, pst_tx_thead_args->queue_idx, 
                g_mbufs_pools[pst_tx_thead_args->port_id][pst_tx_thead_args->queue_idx].tx_bd_mbufs, cur_require_tx_nb, &curr_real_tx_nb);
        pst_tx_thead_args->real_tx_nb += curr_real_tx_nb;
        if (status < 0) {
            printf("\033[1;31;40mrequire_tx_all(%lu), real_tx_all(%lu), "
                "require_loop_all(%lu), failed_loop_time(%lu)\033[0m\r\n",
                pst_tx_thead_args->p_business_args->packet_num, pst_tx_thead_args->real_tx_nb,
                tx_time+1, tx_time_idx);
            return 0;
        }
    }
    (void)gettimeofday(&tx_end_time, NULL);
    uint64_t diff = 1000000*(tx_end_time.tv_sec-tx_start_time.tv_sec) + tx_end_time.tv_usec - tx_start_time.tv_usec;
    printf("\033[1;32;40mport %u, queue %u run_business_tx_thread_route finish, time %lu(us)\033[0m\r\n", pst_tx_thead_args->port_id, pst_tx_thead_args->queue_idx, diff);
    
    return 0;
}

static int run_business_rx_thread_route(void* arg) {
    uint32_t port_id = 0;
    uint32_t queue_idx = 0;
    pstBusinessThreadArgs pst_rx_thead_args = (pstBusinessThreadArgs)arg;
    pst_rx_thead_args->real_rx_nb = 0;
    uint32_t    rx_time = pst_rx_thead_args->p_business_args->packet_num / MBUFS_NB_IN_POOL;
    uint16_t    rx_last_time_nb = pst_rx_thead_args->p_business_args->packet_num % MBUFS_NB_IN_POOL;
    uint32_t    rx_time_idx = 0;
    uint16_t    curr_real_rx_nb = 0; 
    uint16_t    cur_require_rx_nb = 0;
    int         status = 0;
    struct timeval  rx_start_time, rx_end_time;

    port_id = pst_rx_thead_args->port_id;
    queue_idx = pst_rx_thead_args->queue_idx;

    (void)gettimeofday(&rx_start_time, NULL);
    for (rx_time_idx = 0; rx_time_idx < (rx_time + 1); ++rx_time_idx) {
        cur_require_rx_nb = (rx_time_idx==rx_time) ? rx_last_time_nb : MBUFS_NB_IN_POOL;
        curr_real_rx_nb = 0;
        if (cur_require_rx_nb == 0) {
            break;
        }
        status = run_business_rx(port_id, queue_idx, 
                g_mbufs_pools[port_id][queue_idx].rx_bd_mbufs, cur_require_rx_nb, &curr_real_rx_nb);
        pst_rx_thead_args->real_rx_nb += curr_real_rx_nb;

        if (status < 0) {
            printf("\033[1;31;40mrequire_rx_all(%lu), real_rx_all(%lu), "
                "require_loop_all(%lu), failed_loop_time(%lu)\033[0m\r\n",
                pst_rx_thead_args->p_business_args->packet_num, pst_rx_thead_args->real_rx_nb,
                rx_time+1, rx_time_idx);
            return 0;
        }
    }
    (void)gettimeofday(&rx_end_time, NULL);
    uint64_t diff = 1000000*(rx_end_time.tv_sec-rx_start_time.tv_sec) + rx_end_time.tv_usec - rx_start_time.tv_usec;
    printf("\033[1;32;40mport %u, queue %u run_business_rx_thread_route finish, time %lu(us)\033[0m\r\n", port_id, queue_idx, diff);
    /*
     * caculation os gbps: (number of receive packages) * (length of message(bytes)) * 8 / 1000 / 1000 / 1000 / (time consumed(second))
     */
    
    printf("\033[1;32;40m----------------port %u, queue %u rx_packet_len %u, packet_num: %lu, performance = %lu gbps----------------\033[0m\r\n",
        port_id, queue_idx, \
        pst_rx_thead_args->p_business_args->packet_len,
        pst_rx_thead_args->p_business_args->packet_num,
        ((uint64_t)pst_rx_thead_args->real_rx_nb)*((uint64_t)g_mbufs_pools[port_id][queue_idx].data_mbuf_len)*8/1000/1000/1000*1000*1000/diff);
    return 0;
}

