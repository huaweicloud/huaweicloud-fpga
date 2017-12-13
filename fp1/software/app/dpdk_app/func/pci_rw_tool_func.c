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
#include "regs_func.h"
#include "securec.h"

#define PCI_RES_DEV_PRE "/sys/bus/pci/devices"
#define WRITE_MAX_REGS_COUNT        (256)
#define READ_MAX_REGS_COUNT         (256)

static int g_pci_barx_res_fd[MAX_BAR_NUM] = { -1 };
static char* g_pci_barx_acce_addr[MAX_BAR_NUM] = { NULL };
static long int g_pci_barx_size[MAX_BAR_NUM] = { 0 };

int find_special_device_dbdf(char* str_vendor_id, char* str_device_id, unsigned int idx, char* bdf) {
    char    vendor_id_value[17] = {0};
    int     vendor_id_value_len = 0;
    char    device_id_value[17] = {0};
    int     device_id_value_len = 0;
    int     vendor_id_fd = 0;
    int     device_id_fd = 0;
    char    vendor_id_file[PATH_MAX] = {0};
    char    device_id_file[PATH_MAX] = {0};
    DIR     *dir = NULL;
    struct dirent*dir_entry = NULL;
    char sub_dir_name[PATH_MAX] = {0};
    uint8_t cur_idx = 0;
    
    if (NULL == bdf) {
        printf("the input arg is null!\r\n");
        return -EINVAL;
    }
    
    if (NULL == (dir=opendir(SYS_PCI_DEVICES_DIR))) {
        printf("opendir %s error\r\n", SYS_PCI_DEVICES_DIR);
        return -EIO;
    }

    if (idx > UINT8_MAX) {
        printf("the vf idx(%u) is too large\r\n", idx);
        (void)closedir(dir);
        return -EINVAL;
    }

    while(NULL != (dir_entry=readdir(dir))) {
        if ((0==strcmp(dir_entry->d_name, ".")) || (0==strcmp(dir_entry->d_name, ".."))){
            continue;
        }
        if ((0 > snprintf_s(sub_dir_name, PATH_MAX, PATH_MAX-1, "%s/%s", SYS_PCI_DEVICES_DIR, dir_entry->d_name)) ||
            (0 > snprintf_s(vendor_id_file, PATH_MAX, PATH_MAX-1, "%s/%s", sub_dir_name, VENDOR_ID_FILE)) ||
            (0 > snprintf_s(device_id_file, PATH_MAX, PATH_MAX-1, "%s/%s", sub_dir_name, DEVICE_ID_FILE))) {
            printf("directory's path fail\n");
            (void)closedir(dir);
            return -EIO;
        }
        
        vendor_id_fd = open(vendor_id_file, O_RDONLY);
        if (vendor_id_fd < 0){
            continue;
        }
        
        device_id_fd = open(device_id_file, O_RDONLY);
        if (device_id_fd < 0) {
            close(vendor_id_fd);
            continue;
        }

        /*vendor_id_value stores 0x19e5;device_id_value stores 0xd503; at place of \[6], value is \n;*/
        if ((-1 != read(vendor_id_fd, vendor_id_value, sizeof(vendor_id_value) - 1)) &&
            (-1 != read(device_id_fd, device_id_value, sizeof(device_id_value) - 1))) {
            vendor_id_value[sizeof(device_id_value) - 1] = '\0';
            device_id_value[sizeof(device_id_value) - 1] = '\0';
            vendor_id_value_len = strnlen(vendor_id_value, sizeof(vendor_id_value));
            device_id_value_len = strnlen(device_id_value, sizeof(device_id_value));
            if ((0 == vendor_id_value_len) || (0 == device_id_value_len)) {
                close(vendor_id_fd);
                close(device_id_fd);
                continue;
            }
            /* vendor_id_fd/device_id_fd, last bytes of string read from file includes '\n' */
            if ('\n' == vendor_id_value[vendor_id_value_len-1]) {
                vendor_id_value[vendor_id_value_len-1] = '\0';
            }
            if ('\n' == device_id_value[device_id_value_len-1]) {
                device_id_value[device_id_value_len-1] = '\0';
            }
        }
        else {
            printf("read error\r\n");
            close(vendor_id_fd);
            close(device_id_fd);
            continue;
        }
        
        /*printf("vendor_id_value=%s, device_id_value=%s\r\n", vendor_id_value, device_id_value);*/
        if ((0==strcmp(vendor_id_value, str_vendor_id)) && (0==strcmp(device_id_value, str_device_id))) {  
            if (cur_idx == idx) {
                (void)strcpy_s(bdf, PATH_MAX, dir_entry->d_name);
                close(vendor_id_fd);
                close(device_id_fd);
                (void)closedir(dir);
                return 0;
            } else {
                close(vendor_id_fd);
                close(device_id_fd);
                ++cur_idx;
                continue;
            }
            
        }
        else {
            close(vendor_id_fd);
            close(device_id_fd);
        }
    }
    if (0 != cur_idx) {
        printf("There are only %u device for %s:%s, the parameter idx %u is too large\r\n", cur_idx, str_vendor_id, str_device_id, idx);
    }
    (void)closedir(dir);
    return -ENODEV;
}

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

    int ret = find_special_device_dbdf(STR_ACC_VENDOR_ID, STR_ACC_DEVICE_ID, vf_idx, acc_pci_bdf);
    if (0 != ret) {
    	printf("can not find acc device, please check");
    	return -ENODEV;
    }

    printf("to operate device %s\r\n", acc_pci_bdf);
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