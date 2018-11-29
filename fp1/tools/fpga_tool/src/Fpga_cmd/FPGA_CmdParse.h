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

#ifndef __FPGA_CMDPARSE_H__
#define __FPGA_CMDPARSE_H__

#include <stdbool.h>

#define FPGA_SLOT_INFO                      'S'
#define HFI_ID_INFO                         'I'
#define DISPLAY_FPGA_PHY_INFO               'D'
#define COMMAND_HELP_INFO                   'h'
#define COMMAND_HELP_INFO1                  '?'

#define HFI_TOOL_VERSION  "v105"

#define INPUT_PARAS_NUM_MIN                     2
#define INPUT_PARAS_FOR_PARSE_MIN               3
#define INPUT_PARAS_FOR_IL_MAX                  4
#define INPUT_PARAS_FOR_DF_MAX                  3
#define INPUT_PARAS_FOR_IF_MAX                  4
#define INPUT_PARAS_FOR_LF_MAX                  6
#define INPUT_PARAS_FOR_CF_MAX                  4
#define INPUT_OPTCODE_LENGTH_LIMIT              3 

/************************* Define error code ************************************/
#define PARSE_ERROR                              3
#define SDKRTN_PARSE_SUCCESS                     0
#define SDKRTN_PARSE_ERROR_BASE                  ( SDKRTN_ERR_BASE  +  ( PARSE_ERROR << 8  ) )
#define SDKRTN_PARSE_INPUT_ERROR                 ( SDKRTN_PARSE_ERROR_BASE + 0x1 )
#define SDKRTN_PARSE_SLOT_ERROR                  ( SDKRTN_PARSE_ERROR_BASE + 0x2 )
#define SDKRTN_PARSE_HFI_ID_ERROR                ( SDKRTN_PARSE_ERROR_BASE + 0x3 )
#define SDKRTN_PARSE_STRNCPY_ERROR               ( SDKRTN_PARSE_ERROR_BASE + 0x4 )
#define SDKRTN_PARSE_INVALID_PARA_ERROR          ( SDKRTN_PARSE_ERROR_BASE + 0x5 )
#define SDKRTN_PARSE_INVALID_CHAR_ERROR          ( SDKRTN_PARSE_ERROR_BASE + 0x6 )
#define SDKRTN_PARSE_INVALID_RANGE_ERROR         ( SDKRTN_PARSE_ERROR_BASE + 0x7 )
#define SDKRTN_PARSE_INVALID_VALUE_ERROR         ( SDKRTN_PARSE_ERROR_BASE + 0x8 )
#define SDKRTN_PARSE_INVALID_CODE_ERROR          ( SDKRTN_PARSE_ERROR_BASE + 0x9 )
#define PRINTED_COUNT                            0


typedef UINT32 (* PARSE_HOOK_CALLBACK)(INT32 argc, INT8 *argv[]);

typedef struct tagINPUT_COMMAND_PARSE
{
    INT8                                       *cpStr;
    UINT32                                   ulOpcode;
    PARSE_HOOK_CALLBACK           pfnFunc;
}INPUT_COMMAND_PARSE;

void  FPGA_ParsePrintHelpInfo( INT8 *pcCmdName,  INT8 *pcBuf[], UINT32 ulNum);
UINT32 FPGA_ParseCommand( INT32 argc, INT8 *argv[] );
UINT32 FPGA_ParseClearHfi( INT32 argc, INT8 *argv[] );


extern INT8 *g_pacCommandEntryHelp[12];
extern INT8 *g_pacHfiLoadHelp[16];
extern INT8 *g_pacHfiClearHelp[14];
extern INT8 *g_pacInquireFpgaHelp[12];
extern INT8 *g_pacInquireImageHelp[14];
extern INT8 *g_pacInquireLedStatusHelp[14];

#endif
