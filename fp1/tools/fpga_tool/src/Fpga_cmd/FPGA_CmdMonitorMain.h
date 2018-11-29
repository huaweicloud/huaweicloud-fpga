/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
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

#ifndef __FPGA_CMDMONITORMAIN_H__
#define __FPGA_CMDMONITORMAIN_H__
#include <stdbool.h>

/************************* Define error code  ************************************/
#define MONITOR_ERROR                            1
#define SDKRTN_MONITOR_SUCCESS                   0
#define SDKRTN_MONITOR_ERROR_BASE                ( SDKRTN_ERR_BASE  +  ( MONITOR_ERROR << 8  ) )
#define SDKRTN_MONITOR_INPUT_ERROR               ( SDKRTN_MONITOR_ERROR_BASE + 0x1 )
#define SDKRTN_MONITOR_MODULE_INIT_ERROR         ( SDKRTN_MONITOR_ERROR_BASE + 0x2 )
#define SDKRTN_MONITOR_MEMSET_ERROR              ( SDKRTN_MONITOR_ERROR_BASE + 0x3 )
#define SDKRTN_MONITOR_VSNPRINTF_ERROR           ( SDKRTN_MONITOR_ERROR_BASE + 0x4 )
#define SDKRTN_MONITOR_OPCODE_ERROR              ( SDKRTN_MONITOR_ERROR_BASE + 0x5 )
#define SDKRTN_MONITOR_OPCODE_FUNC_ERROR         ( SDKRTN_MONITOR_ERROR_BASE + 0x6 )
#define SDKRTN_MONITOR_MALLOC_ERROR              ( SDKRTN_MONITOR_ERROR_BASE + 0x7 )
#define SDKRTN_MONITOR_PR_STATUS_ERROR         ( SDKRTN_MONITOR_ERROR_BASE + 0x8 )
#define SDKRTN_MONITOR_CMD_OPS_ERROR            ( SDKRTN_MONITOR_ERROR_BASE + 0x9 )
#define SDKRTN_MONITOR_LOAD_ERRNAME_ERROR            ( SDKRTN_MONITOR_ERROR_BASE + 0xa )

#define FPGA_INPUT_PARAS_NUM_MIN                     2
#define OPTCODE_LENGTH_MAX                       2
#define LOAD_STATUS_NAME_LEN_MAX                 20
#define LOAD_ERR_NAME_LEN_MAX                    20
#define QUIT_FLAG                                1              
#define PARA_FLAG                                              1

#define FPGA_OPS_STATUS_MASK                  0xffff0000
#define FPGA_OPS_STATUS_SHIFT                 16
#define FPGA_LOAD_ERROR_MASK                  0xffff

typedef struct tagFPGA_CMD_PARA
{
    UINT32 ulSlotIndex;
    UINT32 ulOpcode;
    INT8    acHfiId[HFI_ID_LEN_MAX];
    bool     bShowInfo;
    UINT8  ulReserved[23];
}FPGA_CMD_PARA;

typedef enum tagUSER_CMD_LIST{
    CMD_HFI_LOAD,
    CMD_HFI_CLEAR,
    CMD_IMAGE_INQUIRE,
    CMD_RESOURSE_INQUIRE,
    CMD_LED_STATUS_INQUIRE,
    CMD_TOOL_VERSION,
    CMD_PARSE_END
}USER_CMD_LIST;

typedef UINT32 (*COMMAND_PROC_FUNC)(void);

extern FPGA_CMD_PARA   g_strFpgaModule;
UINT32 FPGA_MonitorLoadHfi(void);
UINT32 FPGA_MonitorInquireFpgaImageInfo(void);
UINT32 FPGA_MonitorDisplayDevice( void );
UINT32 FPGA_MonitorInquireLEDStatus(void);
UINT32 FPGA_MonitorClearHfi(void);

#endif
