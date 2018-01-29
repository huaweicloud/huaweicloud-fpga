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
#include <assert.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
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
#include <string.h>
#include <pthread.h>
#include "new_bd.h"
#include "run_business_rxtx.h"
#include "securec.h"

#define APP_VERSION "application LOG-Heterogeneous computing V100R001C10B065"
/* Seconds wait before DPDK memory init, 
    for that IP may have some job to do for the former task */
#define WAIT_TIME_BEFORE_DPDK_INIT 1

/* SHELL logic limitation: depth for each queue can only be 1024/2048/4096/8192 */
static uint16_t g_s_queue_desc_nb_valid_values[] = {
    1024,
    1024*2,
    1024*4, 
    1024*8
};

stBusinessArgs g_business_args;
stBusinessThreadArgs g_business_thread_args[VFS_MAX_NUM_EVERY_PF][QUEUES_MAX_NUM_EVERY_IP];

static void help();
static void set_default_args();
static int parse_arg(int argc, char* argv[]);

/* Called before rte_eal_init, do initial parameters checking */
static int check_args_step_one();
/* Called after rte_eal_init, do further parameters checking */
static int check_args_step_two();
static int check_queue_desc_nb_valid(uint16_t queue_desc_nb);

static void dev_info_dump(struct rte_eth_dev_info* dev_info);
static int parse_comma_string(char* str, uint32_t* output_array_values, uint32_t output_array_len, uint32_t* output_array_num);
static int packet_primary(uint32_t port_id);
static int packet_secondary(uint32_t port_id);
static int packet_process(void);

/**
* @brief DPDK init and logic queue init
* @param[in] argc parameter number
* @param[in] argv parameters
*
* @return 0 for OK, others for error
* @note
*/
int main(int argc, char* argv[]) {
    int status = 0;
    uint32_t lcore_id = 0;
    uint32_t cpu_nb = 0;
    int argc_dpdk = 0;
    char str_cpu_nb[32] = {0};
    uint64_t cpu_mask = 0;
    uint16_t cpu_idx = 0;
    uint8_t  dev_count = 0;
    uint8_t port_idx = 0;
    char dev_name[256] = {0};
    static struct timespec ts;

    set_default_args();
        
    if (0 != parse_arg(argc, argv)) {
        return -1;
    }

    if (0 != check_args_step_one()) {
        help();
        return -1;
    }

    if(ENABLE == g_business_args.fmmu_enable) {
        g_business_args.packet_len += sizeof(acc_second_bd);
    }

    /* get the cpus of platform */
    cpu_nb = (uint32_t)get_nprocs();
    if (cpu_nb < 3) {
        rte_exit(EXIT_FAILURE, "The platform has only %d available cpus, need 3 available cpus at least\r\n", cpu_nb);
    }
    if (cpu_nb > (8*sizeof(uint64_t))) {  /* Here 8 refer to 8 bits/byte */
        printf("\033[1;31;40mWarning: the available cpus nubmer is too much, we only use %u available cpus\033[0m\r\n", sizeof(uint64_t)*8);
    }
    
    for (cpu_idx = 0; cpu_idx < cpu_nb; cpu_idx++) {
        cpu_mask = ((cpu_mask << 1) | 0x01);
    }
    (void)snprintf_s(str_cpu_nb, sizeof(str_cpu_nb), sizeof(str_cpu_nb) - 1, "-c%x", cpu_mask);
    printf("available cpu number: %d, cpu mask parameter: %s\r\n", cpu_nb, str_cpu_nb);
    
    char* argv_dpdk[]= {argv[0], str_cpu_nb, "-n", "1", "--proc-type=primary"};
    argc_dpdk = sizeof(argv_dpdk)/sizeof(argv_dpdk[0]);

    /* CPU 0 is reserved for primary, for other cpus, every queue have 2 CPUs to oneself, one for tx and another for rx */
    if ((g_business_args.port_used * g_business_args.queue_used)*2 > (cpu_nb-1)) {
        printf("\033[1;31;40mthe port and queue error, the required cpu number(%u) is larger than vm's dpdk tx/rx busincess core(%u)\033[0m\r\n",
            g_business_args.port_used * g_business_args.queue_used*2, (cpu_nb-1));
        return -1;
    }

    ts.tv_sec = WAIT_TIME_BEFORE_DPDK_INIT;
    ts.tv_nsec = 0;
    (void)nanosleep(&ts, NULL);
    status = rte_eal_init(argc_dpdk, argv_dpdk);
    if (0 > status) {
        printf("%s:%d: rte_eal_init failed: %d\r\n", __FUNCTION__, __LINE__, status);
        return status;
    }

    if (0 != check_args_step_two()) {
        help();
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

    lcore_id = rte_lcore_id();
    printf("%s:%d:lcore_id=%u\r\n", __FUNCTION__, __LINE__, lcore_id);

    (void)packet_process();
    
    return 0;
}
/**
* @brief dump device information based on dev_info
* @param[in] dev_info 
*
* @return void
* @note
*/
static void dev_info_dump(struct rte_eth_dev_info* dev_info) {
    if (NULL == dev_info) return;

    printf("driver_name=%s\n"
        "max_rx_queues=%u, max_tx_queues=%u\n",
        dev_info->driver_name,
        dev_info->max_rx_queues, dev_info->max_tx_queues);

    return;
}

/**
* @brief parse input parameter.
* @param[in] argc parameter number
* @param[in] argv parameters
*
* @return 0 for OK, -1 for error
* @note
*/
#define STR_PARSE_ARG   "d:p:q:l:n:x:fh"
static int parse_arg(int argc, char* argv[]) {
    char*   arg_val = NULL;
    int     ch = 0;
    unsigned long int value = 0;
    while ((ch=getopt(argc, argv, STR_PARSE_ARG)) != -1) {
        switch (ch) {
            case 'd': {
                assert(NULL != optarg);
                arg_val = optarg;
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    g_business_args.queue_desc_nb = value;
                }
                break;
            }
            case 'p': {
                assert(NULL != optarg);
                arg_val = optarg;
                if (0 != parse_comma_string(arg_val, g_business_args.port_ids, 
                                            sizeof(g_business_args.port_ids)/sizeof(uint32_t), 
                                            &g_business_args.port_used)) {
                    goto parse_error;
                }
                break;
            }
            case 'q': {
                assert(NULL != optarg);
                arg_val = optarg;
                if (0 != parse_comma_string(arg_val, g_business_args.queue_idxs, 
                                            sizeof(g_business_args.queue_idxs)/sizeof(uint32_t), 
                                            &g_business_args.queue_used)) {
                    goto parse_error;
                }
                break;
            }
            case 'l': {
                assert(NULL != optarg);
                arg_val = optarg; 
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    g_business_args.packet_len = value;
                }
                break;
            }
            case 'n': {
                assert(NULL != optarg);
                arg_val = optarg; 
                value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    g_business_args.packet_num = value;
                }
                break;
            }
            case 'x': {
                assert(NULL != optarg);
                arg_val = optarg; 
                 value = strtoul(arg_val, NULL, 10);
                /* input string is too long */
                if(errno == ERANGE) {
                    goto parse_error;
                } else {
                    g_business_args.loop_time = value;
                }
                break;
            }
            case 'f': {
                g_business_args.fmmu_enable= ENABLE;
                break;
            }
            case 'h':
            default:
                goto parse_error;
        }
    }
    
    return 0;
    
parse_error:
    help();
    return -1;
}

/*initialize the global variables and set the default values*/
static void set_default_args() {
    (void)memset_s(&g_business_args, sizeof(g_business_args), 0, sizeof(g_business_args));
    g_business_args.port_used = 1;
    g_business_args.queue_used = 1;
    (void)memset_s(g_business_args.port_ids, sizeof(g_business_args.port_ids), 0, sizeof(g_business_args.port_ids));
    g_business_args.port_ids[0] = 1;
    (void)memset_s(g_business_args.queue_idxs, sizeof(g_business_args.queue_idxs), 0, sizeof(g_business_args.queue_idxs));
    g_business_args.queue_idxs[0] = 1;
    g_business_args.packet_len = PACKET_LEN_DEFAULT;
    g_business_args.packet_num = PACKET_NUM_DEFAULT;

    g_business_args.not_tx_thread = 0;
    g_business_args.not_rx_thread = 0;

    g_business_args.queue_desc_nb = QUEUE_DESC_MAX_NB;
    g_business_args.loop_time = LOOP_TIME_DEFAULT;
    g_business_args.fmmu_enable = DISABLE;
}

static int check_args_step_one() {

    /* check the validity of  queue_desc_nb*/
    if (0 != check_queue_desc_nb_valid(g_business_args.queue_desc_nb)) {
        printf("-d parameter value (%u) is error\r\n", g_business_args.queue_desc_nb);
        return -EINVAL;
    }

    /* Check if the package length is in normal range */
    if ((g_business_args.packet_len > PACKET_LEN_MAX) || (g_business_args.packet_len < PACKET_LEN_DEFAULT)) {
        printf("tx/rx packet length error\r\n");
        return -EINVAL;
    }

    /* Check if the package number is in normal range */
    if ((0 == g_business_args.packet_num) || (g_business_args.packet_num > PACKET_NUM_MAX)) {
        printf("tx/rx packet num error,could not be zero or beyond %lu\r\n", PACKET_NUM_MAX);
        return -EINVAL;
    }

    /* loop time can't be zero */
    if (0 == g_business_args.loop_time) {
        printf("the -x paramters is error\r\n");
        return -EINVAL;
    }
    if(g_business_args.loop_time > (UINT16_MAX-1024)) {
        printf("the -x paramters is error\r\n");
        return -EINVAL;
    }
    
    return 0;
}

static int check_args_step_two() {
    uint8_t dev_count = 0;
    int8_t idx = 0;
    int8_t port_used_max_id = -1;

    /* get the number of vf device */
    dev_count = rte_eth_dev_count();
    if (0 == dev_count) {
        printf("there is not any vf device\r\n");
        return -EINVAL;
    }
    printf("there are %u vf may be used\r\n", dev_count);

    for (idx = sizeof(g_business_args.port_ids)/sizeof(uint32_t) - 1; idx >= 0; --idx) {
        if (1 == g_business_args.port_ids[idx]) {
            port_used_max_id = idx;
            break;
        }
    }
    if (port_used_max_id >= dev_count) {
        printf("the max index of device %u is equal or large than the real vf device's number %u\r\n", port_used_max_id, dev_count);
        return -EINVAL;
    }

    return 0;
}

static void help() {
    printf(
        "-----------------------------------------------------------------------------------\r\n"
        "app version: %s\r\n"
        "argument format:\n"
        "\t-d xxx   xxx: queue depth, should be 1024 or 2048 or 4096 or 8192, 8192 as default\r\n"
        "\t-p xxx   xxx: port id, logic only support vf0;\r\n"
        "\t-q xxx   xxx: queue idx, should be [0, 7], 0 as default;\r\n"
        "\t-l xxx   xxx: length for each packet to tx and rx (length's scope is [64, %u], 64 as default); \r\n"
        "\t-n xxx   xxx: number of packet to tx and rx; (128 as default, max=(%lu))\r\n"
        "\t-x xxx   xxx: the loop time for a full TX/RX business(loop's scope is [1, %u], 1 as default)\r\n"
        "\t-f: enable FMMU function(disable as default)\r\n"
        "\t-h: print help\n"
        "-----------------------------------------------------------------------------------\r\n",
        APP_VERSION, 
        PACKET_LEN_MAX, UINT32_MAX-1024, UINT16_MAX-1024);
}


static int parse_comma_string(char* str, uint32_t* output_array_values, uint32_t output_array_num, uint32_t* real_output_array_num) {
    uint32_t value = 0; 
    char *p = NULL;
    char *delim = ",";
    char* inner_use = NULL;
    if ((NULL == str) || (NULL == output_array_values) || (NULL == real_output_array_num)) {
        printf("some input params are null\n");
        return -EINVAL;
    }
    (void)memset_s(output_array_values, output_array_num*sizeof(uint32_t), 0, output_array_num*sizeof(uint32_t));
    *real_output_array_num = 0;
    p = strtok_s(str, delim, &inner_use);
    while (NULL != p) {
        value = strtoul(p, NULL, 10);
        if (errno == ERANGE){
            return -EINVAL;
        }
        if (value < output_array_num) {
            if (0 == output_array_values[value]) {
                output_array_values[value] = 1;
                *real_output_array_num += 1;
            }
        } else {
            return -EINVAL;
        }

        p = strtok_s(NULL, delim, &inner_use);
    }

    return 0;
}

static int packet_secondary(uint32_t port_id) {
    int status = 0;
    uint32_t queue = 0;
    uint32_t cpu_bind = 1;
    pthread_t task_id = 0;
    struct rte_mempool* bd_mp = NULL;
    struct rte_mempool* data_mp = NULL;
    char z_name[RTE_MEMZONE_NAMESIZE]={0};
    uint32_t secondary_data_mbuf_size = 0;
    uint32_t secondary_data_size = 0;

    /* Calculate the real data size in mbuf which will be sent to LOGIC, should be aligned as LOGIC requirement */
    secondary_data_size = g_business_args.packet_len;
    SIZE_ALIGNED_BYTES(secondary_data_size, LOGIC_DATA_ALIGN);

    secondary_data_mbuf_size = secondary_data_size + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM;
    
    for (queue = 0; queue < QUEUES_MAX_NUM_EVERY_IP; queue++) {
        /* queue is out of using */
        if (0 == g_business_args.queue_idxs[queue]) {
            continue;
        }
        
        (void)snprintf_s(z_name, sizeof(z_name), sizeof(z_name) - 1, "%s_%u_%u", SECONDARY_BD_ENV_MEM_POOL_NAME, port_id, queue);

        /*  check if mempool which name is z_name exist.
         *  if already exist, just use it; if not, create one.
         */
        bd_mp = rte_mempool_lookup(z_name);
        if (NULL == bd_mp) {
            printf("Not find mempool %s, create it\r\n", z_name);
            bd_mp = rte_mempool_create(z_name,
                        SECONDARY_BD_NB_MBUF, SECONDARY_BD_MBUF_SIZE, SECONDARY_BD_MBUF_CACHE_SIZE,
                        sizeof(struct rte_pktmbuf_pool_private),
                        rte_pktmbuf_pool_init, NULL, 
                        rte_pktmbuf_init, NULL, 0, 
                        MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
            if (NULL == bd_mp) {
                rte_exit(EXIT_FAILURE, "Cannot init bd mbuf pool for port %u\r\n", port_id);
            }
        }    
        printf("mempool %s has %u available entries\r\n", z_name, rte_mempool_count(bd_mp));

        (void)snprintf_s(z_name, sizeof(z_name), sizeof(z_name) - 1, "%s_%u_%u", SECONDARY_DATA_ENV_MEM_POOL_NAME, port_id, queue);

        /*  check if mempool which name is z_name exist.
         *  if already exist, just use it; if not, create one.
         */
        data_mp = rte_mempool_lookup(z_name); 
        if (NULL == data_mp) {
            printf("Not find mempool %s, create it\r\n", z_name);
            data_mp = rte_mempool_create(z_name,
                        SECONDARY_DATA_NB_MBUF, secondary_data_mbuf_size, SECONDARY_DATA_MBUF_CACHE_SIZE,
                        sizeof(struct rte_pktmbuf_pool_private),
                        rte_pktmbuf_pool_init, NULL, 
                        rte_pktmbuf_init, NULL, 0, 
                        MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
            if (NULL == data_mp) {
                rte_exit(EXIT_FAILURE, "Cannot init data mbuf pool for port %u\r\n", port_id);
            }
        }
        printf("mempool %s has %u available entries\r\n", z_name, rte_mempool_count(data_mp));

        /* parallel threads */
        g_business_thread_args[port_id][queue].business_bd_mp = bd_mp;
        g_business_thread_args[port_id][queue].business_data_mp = data_mp;
        g_business_thread_args[port_id][queue].p_business_args = &g_business_args;
        g_business_thread_args[port_id][queue].port_id = port_id;
        g_business_thread_args[port_id][queue].queue_idx = queue;

    }

    /* Create mempool first and then start the thread */
    for (queue = 0; queue < QUEUES_MAX_NUM_EVERY_IP; queue++) {
        /* queue is out of using */
        if (0 == g_business_args.queue_idxs[queue]) {
            continue;
        }
        printf("\033[1;32;40m---------------- test for port %u, queue %u, %s ----------------\033[0m\r\n",   \
            port_id, queue, g_business_args.fmmu_enable?"[FMMU] ":" ");
        g_business_thread_args[port_id][queue].cpu_bind = cpu_bind;
        status = pthread_create(&task_id, NULL, (void *)run_business_thread, &g_business_thread_args[port_id][queue]);  //lint !e611
        if (0 != status){
            printf("can't create thread. errno = %d\n", errno);
            return -1;
        }

        g_business_thread_args[port_id][queue].task_id = task_id;
        cpu_bind += 2;
    }
    
    return 0;
}

static int packet_primary(uint32_t port_id) {
    uint16_t loop_time = 0;
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
    for (queue_idx = 0; queue_idx < dev_info.max_rx_queues; ++queue_idx) {
        /* queue is out of using */
        if (0 == g_business_args.queue_idxs[queue_idx]){
            continue;
        }
    
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
        if (0 != rte_eth_rx_queue_setup(port_id, queue_idx, g_business_args.queue_desc_nb, socket_id, NULL, mp)) {
            printf("\033[1;31;40mrte_eth_rx_queue_setup fails for port: %u, queue: %u\033[0m\r\n", port_id, queue_idx);
            return -1;
        }

        if (0 != rte_eth_tx_queue_setup(port_id, queue_idx, g_business_args.queue_desc_nb, socket_id, NULL)) {
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

    for (loop_time = 0; loop_time < g_business_args.loop_time; loop_time++){
        printf("\033[1;32;40m----------------TEST TIME %u for port %u----------------\033[0m\r\n", loop_time, port_id);
        status = packet_secondary(port_id);
        if (0 != status){
            return -1;
        }

        /* Waiting for the end of the thread */
        for (queue_idx = 0; queue_idx < QUEUES_MAX_NUM_EVERY_IP; queue_idx++) {
            /* queue is out of using */
            if (0 == g_business_args.queue_idxs[queue_idx]) {
                continue;
            }

            /* Do not care about the return value of thread here currently. */
            (void)pthread_join(g_business_thread_args[port_id][queue_idx].task_id, NULL);
        }

    }

    return 0;
}

static int packet_process(void){
    int status = 0;
    uint32_t port_id = 0;
    char dev_name[256] = {0};
    uint8_t dev_count = 0;

    for (port_id = 0; port_id < VFS_MAX_NUM_EVERY_PF; port_id++) {
        /* port is out of using */
        if (0 == g_business_args.port_ids[port_id]) {
            continue;
        }

        (void)packet_primary(port_id);
    }

    /* Resource free */
    for (port_id = 0; port_id < VFS_MAX_NUM_EVERY_PF; port_id++) {
        /* port is out of using */
        if (0 == g_business_args.port_ids[port_id]) {
            continue;
        }
    
        rte_eth_dev_stop(port_id);
        rte_eth_dev_close(port_id);    
    }

    dev_count = rte_eth_dev_count();
    for (port_id = 0; port_id < dev_count; ++port_id) {
        status = rte_eth_dev_detach(port_id, dev_name);
        if (0 == status) {
            printf("rte_eth_dev_detach success for port: %u, dev_name: %s\r\n",port_id, dev_name);
        } else {
            printf("\033[1;31;40mrte_eth_dev_detach fails for port: %u, dev_name: %s\033[0m\r\n",port_id, dev_name);
        }
    }

    return 0;
}

static int
check_queue_desc_nb_valid(uint16_t queue_desc_nb) {
    uint8_t idx = 0;
    
    for (idx = 0; idx < sizeof(g_s_queue_desc_nb_valid_values)/sizeof(g_s_queue_desc_nb_valid_values[0]); ++idx) {
        if (queue_desc_nb == g_s_queue_desc_nb_valid_values[idx]) {
            return 0;
        }
    }
    return -EINVAL;
}