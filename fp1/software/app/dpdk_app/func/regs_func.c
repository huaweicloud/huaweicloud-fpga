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
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <linux/limits.h>
#include "regs_infos.h"
#include "pci_rw_tool_func.h"
#include "regs_func.h"
#include "securec.h"

DDR_ADDR ddrs[4] = {
    {0x2000*4, 0x2001*4, 0x2002*4, 0x2003*4},
    {0x3000*4, 0x3001*4, 0x3002*4, 0x3003*4},
    {0x4000*4, 0x4001*4, 0x4002*4, 0x4003*4},
    {0x5000*4, 0x5001*4, 0x5002*4, 0x5003*4}       
};

#define VAL_DISABLE_ISO_EN      (0x00000000)

int print_demo1_version() {
    /*unsigned int addr[] = {REG_PF_DEMO1_VERSION};*/
    unsigned int addr[] = {REG_PF_DEMO1_VERSION_NEW};
    unsigned int val[] = {0};
    
    /*uio_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);*/
    (void)pci_bar2_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);

    printf("version: 0x%08x\r\n", val[0]);
    return 0;
}

int print_oppos_data() {
    /*unsigned int addr[] = {REG_PF_OPPOS_DATA};*/
    unsigned int addr[] = {REG_PF_OPPOS_DATA_NEW};
    unsigned int val[] = {0};
    
    /*uio_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);*/
    (void)pci_bar2_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);

    printf("oppos: 0x%08x\r\n", val[0]);
    return 0;
}

int set_oppos_data(unsigned int value) {
    /*unsigned int addr[] = {REG_PF_OPPOS_DATA};*/
    unsigned int addr[] = {REG_PF_OPPOS_DATA_NEW};
    unsigned int val[] = {value};
    
    /*uio_write_regs(addr, val, sizeof(addr)/sizeof(unsigned int));*/
    (void)pci_bar2_write_regs(addr, val, sizeof(addr)/sizeof(unsigned int));
    return 0;
}

int print_add_result_data() {
    /*unsigned int addr[] = {REG_PF_DEMO1_SUM_RDATA};*/
    unsigned int addr[] = {REG_PF_DEMO1_SUM_RDATA_NEW};
    unsigned int val[] = {0};
    
    /*uio_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);*/
    (void)pci_bar2_read_regs(addr, sizeof(addr)/sizeof(unsigned int), val);
        
    printf("add result: 0x%08x\r\n", val[0]);
    return 0;
}

int set_add_data(unsigned int data0, unsigned int data1) {
    /*unsigned int addr[] = {REG_PF_DEMO1_ADDER_CFG_WDATA0, REG_PF_DEMO1_ADDER_CFG_WDATA1};*/
    unsigned int addr[] = {REG_PF_DEMO1_ADDER_CFG_WDATA0_NEW, REG_PF_DEMO1_ADDER_CFG_WDATA1_NEW};
    unsigned int val[] = {data0, data1};
    
    /*uio_write_regs(addr, val, sizeof(addr)/sizeof(unsigned int));*/
    (void)pci_bar2_write_regs(addr, val, sizeof(addr)/sizeof(unsigned int));

    return 0;
}

int set_ddr_data(unsigned int num, unsigned int addr, unsigned int value) {
    unsigned int val[] = {0x03};
    unsigned int cmd_reg_value[] = {0};
    unsigned int idx = 0;
    struct timeval wait_time = {0, 10};
        
    (void)pci_bar2_write_regs(&(ddrs[num].addr), &addr, 1);
    (void)pci_bar2_write_regs(&(ddrs[num].write_data), &value, 1);
    (void)pci_bar2_write_regs(&(ddrs[num].cmd), val, 1);

    /* 
     * according to agreement of logic handshake, after command register is assigned, 
     * software can do next step until register reset to 0.
    */
    do {
        (void)pci_bar2_read_regs(&ddrs[num].cmd, 1, cmd_reg_value);
        if (0 == cmd_reg_value[0]) {
            return 0;
        }
        (void)select(0, NULL, NULL, NULL, &wait_time);
    } while (idx++ < 100);

    printf("\033[1;31;40mcmd_reg_value is not zero\033[0m\r\n");

    return -EIO;
}

int print_ddr_data(unsigned int num, unsigned int addr) {
    unsigned int val[] = {0x02};
    unsigned int value[] = {0};
    unsigned int cmd_reg_value[] = {0};
    unsigned int idx = 0;
    struct timeval wait_time = {0, 10};

    (void)pci_bar2_write_regs(&(ddrs[num].addr), &addr, 1);
    (void)pci_bar2_write_regs(&(ddrs[num].cmd), val, 1);
    
    /* 
     * according to agreement of logic handshake, after command register is assigned, 
     * software can do next step until register reset to 0.
    */
    do {
        (void)pci_bar2_read_regs(&ddrs[num].cmd, 1, cmd_reg_value);
        if (0 == cmd_reg_value[0]) {
            (void)pci_bar2_read_regs(&(ddrs[num].read_data), 1, value);
            printf("Value: 0x%08x\r\n", value[0]);
            return 0;
        }
        (void)select(0, NULL, NULL, NULL, &wait_time);
    } while (idx++ < 100);

    printf("\033[1;31;40mcmd_reg_value is not zero\033[0m\r\n");
        
    return -EIO;
}
