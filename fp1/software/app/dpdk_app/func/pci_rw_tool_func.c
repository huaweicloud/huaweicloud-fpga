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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/limits.h>
#include <dirent.h>

#include "pci_rw_tool_func.h"
#include "ul_get_port_status.h"
#include "regs_func.h"
#include "securec.h"

#define PCI_RES_DEV_PRE "/sys/bus/pci/devices"
#define WRITE_MAX_REGS_COUNT        (256)
#define READ_MAX_REGS_COUNT         (256)

static int g_pci_barx_res_fd[MAX_BAR_NUM] = { -1 };
static char* g_pci_barx_acce_addr[MAX_BAR_NUM] = { NULL };
static long int g_pci_barx_size[MAX_BAR_NUM] = { 0 };

/*
 * pci_barx_init_env: init pci barx resource device
*/
int pci_barx_init_env(int vf_idx, int bar_idx) {
    char acc_pci_bdf[PATH_MAX] = {0};
    char pci_barx_res_dev[PATH_MAX] = {0};

    if (bar_idx >= MAX_BAR_NUM) {
        printf("Invalid Bar num %d!", bar_idx);
        return -1;
    }

    int ret = get_device_dbdf_by_port_id(vf_idx, acc_pci_bdf);
    if (0 != ret) {
    	printf("call get_device_dbdf_by_port_id fail , please check");
    	return -ENODEV;
    }

    (void)snprintf_s(pci_barx_res_dev, PATH_MAX, PATH_MAX-1, "%s/%s/resource%d", PCI_RES_DEV_PRE, acc_pci_bdf, bar_idx);
    
    g_pci_barx_res_fd[bar_idx] = open(pci_barx_res_dev, O_RDWR);
    if( g_pci_barx_res_fd[bar_idx] < 0 ) {
        perror("open");
        return -EIO;
    }

    g_pci_barx_size[bar_idx] = lseek(g_pci_barx_res_fd[bar_idx], 0L, SEEK_END);
    if (-1 == g_pci_barx_size[bar_idx]) {
        printf("get the resource2 file size error\r\n");
        close(g_pci_barx_res_fd[bar_idx]);
        return -EIO;
    }
    if (-1 == lseek(g_pci_barx_res_fd[bar_idx], 0L, SEEK_SET)) {
        printf("seek the resource2 file to start error\r\n");
        close(g_pci_barx_res_fd[bar_idx]);
        return -EIO;
    }

    g_pci_barx_acce_addr[bar_idx] = mmap(NULL, g_pci_barx_size[bar_idx], PROT_READ | PROT_WRITE,MAP_SHARED, g_pci_barx_res_fd[bar_idx], 0);
    if ((void*)-1 == g_pci_barx_acce_addr[bar_idx]) {
        perror("mmap");
        close(g_pci_barx_res_fd[bar_idx]);
        exit(-1);
    }
	
    return 0;
}

int pci_barx_uninit_env(int bar_idx) {
    int res = 0;

    res = munmap(g_pci_barx_acce_addr[bar_idx], g_pci_barx_size[bar_idx]);
    if (-1 == res){
        perror("munmap");
    }
    g_pci_barx_acce_addr[bar_idx] = NULL;
    
    res = close(g_pci_barx_res_fd[bar_idx]);
    if (-1 == res){
        perror("close");
    }

    return 0;
}

int pci_barx_write_regs(unsigned int* write_addrs, unsigned int* write_values, unsigned int write_addrs_num, int bar_idx)
{
    unsigned int idx = 0;

    if (bar_idx >= MAX_BAR_NUM) {
        printf("Invalid Bar num %d!", bar_idx);
        return -1;
    }
    for (idx = 0; idx < write_addrs_num; ++idx) {
        *((unsigned int*)(g_pci_barx_acce_addr[bar_idx] + write_addrs[idx])) = write_values[idx];
    }
    return 0;
}

int pci_barx_read_regs(unsigned int* read_addrs, unsigned int read_addrs_num, unsigned int* read_values, int bar_idx)
{
    unsigned int idx = 0;

    if (bar_idx >= MAX_BAR_NUM) {
        printf("Invalid Bar num %d!", bar_idx);
        return -1;
    }

    for (idx = 0; idx < read_addrs_num; ++idx) {
        read_values[idx] = *((unsigned int*)(g_pci_barx_acce_addr[bar_idx] + read_addrs[idx]));
    }

    return 0;
}

/*
 * pci_bar2_init_env: init pci bar2 resource device
*/
int pci_bar2_init_env(int vf_idx)
{
    return pci_barx_init_env(vf_idx, 2);
}

int pci_bar2_uninit_env(void)
{
    return pci_barx_uninit_env(2);
}

int pci_bar2_write_regs(unsigned int* write_addrs, unsigned int* write_values, unsigned int write_addrs_num)
{
    return pci_barx_write_regs(write_addrs, write_values, write_addrs_num, 2);
}

int pci_bar2_read_regs(unsigned int* read_addrs, unsigned int read_addrs_num, unsigned int* read_values)
{
    return pci_barx_read_regs(read_addrs, read_addrs_num, read_values, 2);
}