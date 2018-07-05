
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

#include "fpga_ddr_rw_interface.h"
#include "fpga_ddr_rw_common.h"
#include "ul_get_port_status.h"
#include "pci_rw_tool_func.h"

/*gloabal mutex to avoid multi thread init*/
#define MAX_SLOT_ID_NUM     (8)
static pthread_mutex_t g_init_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_init_flag = 0;
static int g_port_id[MAX_SLOT_ID_NUM] = { -1 };
static int g_slot_id[MAX_SLOT_ID_NUM] = { -1 };

int fddr_access_mode_init(Callbackfunc callback) {

    int ret = 0;
    unsigned int idx;

    if(NULL == callback) {
        printf("fpga_ddr_rw_module_init param callback null.\n");
        return -1;
    }
    
    (void)pthread_mutex_lock(&g_init_lock);

    if(1 == g_init_flag) {
        (void)pthread_mutex_unlock(&g_init_lock);
        printf("fpga_ddr_rw_module_init has been done, no need to init again.\n");
        return 0;
    }

    ret = dpdk_gloal_env_init(callback);
    if(ret) {
        (void)pthread_mutex_unlock(&g_init_lock);
        printf("call dpdk_gloal_env_init fail.\n");
        return ret;
    }

    ret = memory_manager_global_init();
    if(ret) {
        (void)pthread_mutex_unlock(&g_init_lock);
        printf("call memory_manager_init fail.\n");
        return ret;
    }
    
    for(idx = 0; idx < MAX_SLOT_ID_NUM; idx++) {
        g_slot_id[idx] = idx;
        ret = pci_slot_id_to_port_id(g_slot_id[idx], (unsigned int*)&(g_port_id[idx]));
        if(ret != 0) {
            //printf("%s: pci_port_id_to_slot_id failed(%d)\r\n", __FUNCTION__, ret);
    		continue;
        }

        ret = pci_bar2_init_env(g_port_id[idx]);
        if (ret != 0) {
            printf("%s: pci_bar2_init_env failed(%d)\r\n", __FUNCTION__, ret);
            return ret;
        }
    } 
    
    g_init_flag = 1;
    (void)pthread_mutex_unlock(&g_init_lock);
    
    return 0;
}
int fddr_access_mode_uninit() {
    return pci_bar2_uninit_env();
}

int alloc_thread_id(unsigned int *thread_id) {
    if(NULL == thread_id) {
        printf("call alloc_thread_id fail.\n");
        return -1;
    }

    return alloc_thread_id_resource(thread_id);
}

int free_thread_id(unsigned int thread_id) {
    if(THREAD_MAX_NUM <= thread_id) {
        printf("free_thread_id param thread id %d not in normal range.\n", thread_id);
        return -1;
    }
    
    return free_thread_id_rersource(thread_id);
}

int read_data_from_fddr (unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data) {
    
    if(check_thread_id_valid(thread_id)) {
        printf("call check_thread_id_valid fail.\n");
        return -1;
    }

    if(rw_data.cpu_vir_dst_addr == 0 || rw_data.length == 0         \
        || rw_data.length > (4*1024*1024)) {
        printf("call check_thread_id_valid fail.\n");
        return -2;
    }

    if(check_ddr_addr_valid(rw_data.fpga_ddr_rd_addr, rw_data.length)) {
        printf("call check_ddr_addr_valid fail.\n");
        return -3;
    }

    if(g_port_id[slot_id] == -1) {
        printf("device of slot id %d not exist.\n", slot_id);
        return -4;
    }

    return read_data_from_ddr_func(thread_id, g_port_id[slot_id], rw_data);
}

int write_data_to_fddr(unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data) {
 
    if(check_thread_id_valid(thread_id)) {
        printf("call check_thread_id_valid fail.\n");
        return -1;
    }

    if(rw_data.cpu_vir_src_addr == 0 || rw_data.length == 0     \
        || rw_data.length > (4*1024*1024)) {
        printf("call check_thread_id_valid fail.\n");
        return -2;
    }

    if(check_ddr_addr_valid(rw_data.fpga_ddr_wr_addr, rw_data.length)) {
        printf("call check_ddr_addr_valid fail.\n");
        return -3;
    }

    if(g_port_id[slot_id] == -1) {
        printf("device of slot id %d not exist.\n", slot_id);
        return -4;
    }

    return write_data_to_ddr_func(thread_id, g_port_id[slot_id], rw_data);
}

int process_data_with_fpga(unsigned int thread_id, unsigned int slot_id, rw_ddr_data rw_data) {
    
    if(check_thread_id_valid(thread_id)) {
        printf("call check_thread_id_valid fail.\n");
        return -1;
    }

    /* app should provide src addr and dst addr to store result. */
    if(rw_data.cpu_vir_src_addr == 0    \
        || rw_data.length == 0      \
        || rw_data.cpu_vir_dst_addr == 0    \
        || rw_data.length > (4*1024*1024)) {
        printf("call check_thread_id_valid fail.\n");
        return -2;
    }

    if(check_ddr_addr_valid(rw_data.fpga_ddr_rd_addr, rw_data.length)) {
        printf("call check_ddr_addr_valid fail.\n");
        return -3;
    }
    
    if(check_ddr_addr_valid(rw_data.fpga_ddr_wr_addr, rw_data.length)) {
        printf("call check_ddr_addr_valid fail.\n");
        return -3;
    }

    if(g_port_id[slot_id] == -1) {
        printf("device of slot id %d not exist.\n", slot_id);
        return -4;
    }

    return process_data_with_fpga_func(thread_id, g_port_id[slot_id], rw_data);
}

int read_register(unsigned int slot_id, unsigned int addr, unsigned int *value) {
    int ret = 0;

    if(g_port_id[slot_id] == -1) {
        printf("device of slot id %d not exist.\n", slot_id);
        return -4;
    }
    
    (void)pci_bar2_read_regs(g_port_id[slot_id], &addr, sizeof(addr)/sizeof(unsigned int), value);
    printf("addr: 0x%08x, data: 0x%08x\r\n", addr, *value);

    return 0;
}
int write_register(unsigned int slot_id ,unsigned int addr, unsigned int value) {
    int ret = 0;
    
    if(g_port_id[slot_id] == -1) {
        printf("device of slot id %d not exist.\n", slot_id);
        return -4;
    }

    (void)pci_bar2_write_regs(g_port_id[slot_id], &addr, &value, sizeof(addr)/sizeof(unsigned int));

    return 0;
}
