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
#include "regs_func.h"
#include "pci_rw_tool_func.h"
#include "ul_get_port_status.h"

#define	STR_PARSE_ARG	"n:a:s:h"

static int parse_arg(int argc, char* argv[]);
static void help();

static unsigned int g_ddr_num = 0;
static unsigned int g_ddr_addr = 0;
static unsigned int g_port_id = 0;
static unsigned int g_slot_id = 0;

int main(int argc, char* argv[]) {
	int ret = 0;

	if (0 != parse_arg(argc, argv)) {
		return -EINVAL;
	}

    ret = pci_port_status_init_env();
    if(ret != 0) {
        printf("%s: pci_port_status_init_env failed(%d)\r\n", __FUNCTION__, ret);
		return ret;
    }

    ret = pci_slot_id_to_port_id(g_slot_id, &g_port_id);
    if(ret != 0) {
        printf("%s: convert_slot_to_port failed(%d)\r\n", __FUNCTION__, ret);
		return ret;
    }
    
    ret = pci_bar2_init_env(g_port_id);
    if (ret != 0) {
    	printf("%s: pci_bar2_init_env failed(%d)\r\n", __FUNCTION__, ret);
    	return ret;
    }

    (void)print_ddr_data(g_ddr_num, g_ddr_addr);

    (void)pci_bar2_uninit_env();
    
	return ret;
}

static int parse_arg(int argc, char* argv[]) {
    char* arg_val = NULL;
    int     ch;
        
	while ((ch=getopt(argc, argv, STR_PARSE_ARG)) != -1) {
        switch (ch) {
            case 'n': {
                assert(NULL != optarg);
                arg_val = optarg;
                g_ddr_num = strtoul(arg_val, NULL, 16);
                break;
            }
            
            case 'a': {
                assert(NULL != optarg);
                arg_val = optarg;
                g_ddr_addr = strtoul(arg_val, NULL, 16);
                break;
            }

            case 's': {
                assert(NULL != optarg);
                arg_val = optarg;
                g_slot_id = strtoul(arg_val, NULL, 0);
                break;
            }
            
            case 'h':
            default:
                goto parse_error;
                
        }
	}

    if (g_ddr_num >= 4)
    {
        printf("ddr num can only support 0/1/2/3\n");
        goto parse_error;
    }
    
    if (g_ddr_addr >= DDR_ADDRESS)
    {
        printf("the ddr can only support [0, 0x8000000) \n");
        goto parse_error;
    }
    
    return 0;
    
parse_error:
    help();
    return -EINVAL;
}

static void help() {
    printf(
        "-----------------------------------------------------------------------------------\r\n"
        "argument format: [-n ddr_num] [-a ddr_addr] [-s slot_id]\r\n"
        "\tddr_num: [0, 3]; ddr_addr: [0, 0x8000000)\r\n"
        "\tslot_id: the VF's slot id, 0 as default\r\n"
        "\t-h: print help\r\n"
        "-----------------------------------------------------------------------------------\r\n"
        );
}

