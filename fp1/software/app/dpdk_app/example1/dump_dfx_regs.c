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
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "pci_rw_tool_func.h"

#define	STR_PARSE_ARG "p:h"
#define DEBUG_DFX_MAP_BAR (0)

typedef struct reg_mask {
    unsigned int reg;
    unsigned int mask;  /* Only mask bits valid */
    char desc[32];
} reg_mask_stru;

static int parse_arg(int argc, char* argv[]);
static void help();
static int g_port_id = 0;

static reg_mask_stru regs[] = {
    /* txqm regs */
    { 0x6080*4, 0x4, "txqm: reg_bdqm_err" },
    { 0x6081*4, 0xf, "txqm: reg_mulqm_err0" },
    { 0x6110*4, 0x30, "txqm: reg_bdqm_sta" },
    { 0x6182*4, 0xffffffff, "txqm: reg_r540_w288_c_cnt_en" },

    /* txm regs */
    { 0x7081*4, 0xc03, "txm: reg_txm_err" },
    { 0x7100*4, 0xf, "txm: reg_txm_status" },
    { 0x7180*4, 0xffffffff, "txm: reg_ae2txm_req_rgt_cnt" },
    { 0x7181*4, 0xffffffff, "txm: reg_ae2txm_req_err_cnt" },
    { 0x7182*4, 0xffffffff, "txm: reg_txm2ae_tx_cnt" },

    /* rxqm regs */

    /* rxm regs */
    { 0x9080*4, 0xc4, "rxm: reg_parrlt_err" },
    { 0x9102*4, 0x3, "rxm: reg_axi_fifo_sta" },
    { 0x918a*4, 0xffffffff, "rxm: reg_axi_dis_cnt" },
    { 0x918b*4, 0xffffffff, "rxm: reg_axi_rc_cnt" }
};

int main(int argc, char* argv[]) {
    int ret = 0;
    unsigned int i;
    unsigned int value = 0;

    if (0 != parse_arg(argc, argv)) {
        return -EINVAL;
    }

    ret = pci_barx_init_env(g_port_id, DEBUG_DFX_MAP_BAR);
    if (ret != 0) {
        printf("%s: pci_barx_init_env failed(%d)\r\n", __FUNCTION__, ret);
        return ret;
    }

    printf(" -------- Dump logic regs begin -------- \n");
    printf("\tReg addr      Value         Description\n");
    for(i = 0; i < sizeof(regs)/sizeof(reg_mask_stru); i++) {
        (void)pci_barx_read_regs(&regs[i].reg, 1, &value, (int)DEBUG_DFX_MAP_BAR);
        printf("\t[0x%08x]: 0x%08x  - %s\n", regs[i].reg, value & regs[i].mask, regs[i].desc);
    }
    printf(" -------- Dump logic regs end -------- \n");
	
    (void)pci_barx_uninit_env(DEBUG_DFX_MAP_BAR);
    
    return ret;
}

static int parse_arg(int argc, char* argv[])
{
    char* arg_val = NULL;
    int     ch;

    while ((ch=getopt(argc, argv, STR_PARSE_ARG)) != -1) 
    {
        switch (ch) {
            case 'p': {
                assert(NULL != optarg);
                arg_val = optarg;
                g_port_id = strtoul(arg_val, NULL, 0);
		printf("g_port_id is %d\n", g_port_id);
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
    return -EINVAL;
}

static void help() {
        printf(
        "-----------------------------------------------------------------------------------\r\n"
        "Dump all logic debug regs.\r\n"
        "argument format: [-p port_index]\r\n"
        "\tport_index: the VF's index, 0 as default\r\n"
        "\t-h: print help\r\n"
        "-----------------------------------------------------------------------------------\r\n"
        );
}
