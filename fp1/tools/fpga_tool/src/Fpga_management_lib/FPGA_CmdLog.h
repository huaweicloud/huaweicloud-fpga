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

#ifndef __FPGA_CMDLOG_H__
#define __FPGA_CMDLOG_H__
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#define LOG_MAX_STRING           32
#define LOGGER_NUM               8
#define ERR_NUM_BUFFER_SIZE      64
#define LOG_MESSAGE_LENGTH_MAX   512

typedef enum tagLOG_LEVEL
{
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_END,
}LOG_LEVEL;

typedef struct tagLOGGER
{
    const INT8 *pcName;
    UINT32 (*pfnLogInit)(void);
    UINT32 (*pfnLog)( LOG_LEVEL enLevel, INT8 *pcMessage );
}LOGGER;

typedef struct tagLOG_RECORD
{
    INT8 acStr[LOG_MAX_STRING];
    LOGGER *pstrLoggers;
} LOG_RECORD;


/*************************  ERROR CODE  ************************************/
#define LOG_MODULE_ERROR                              2
#define SDKRTN_LOG_SUCCESS                            0
#define SDKRTN_LOG_ERROR_BASE                         ( SDKRTN_ERR_BASE  +  ( LOG_MODULE_ERROR << 8  ) )
#define SDKRTN_LOG_INPUT_ERROR                        ( SDKRTN_LOG_ERROR_BASE + 0x1 )
#define SDKRTN_LOG_WRITE_LOOP_ERROR                   ( SDKRTN_LOG_ERROR_BASE + 0x2 )
#define SDKRTN_LOG_SNPRINTF_ERROR                     ( SDKRTN_LOG_ERROR_BASE + 0x3 )
#define SDKRTN_LOG_VSNPRINTF_ERROR                    ( SDKRTN_LOG_ERROR_BASE + 0x4 )
#define SDKRTN_LOG_LOGGER_NUM_ERROR                   ( SDKRTN_LOG_ERROR_BASE + 0x5 )
#define SDKRTN_LOG_FUNC_ERROR                         ( SDKRTN_LOG_ERROR_BASE + 0x6 )
#define SDKRTN_LOG_OPEN_ERROR                         ( SDKRTN_LOG_ERROR_BASE + 0x7 )

UINT32 FPGA_LogEnable( void );
UINT32 FPGA_LogWriteKmsg( LOG_LEVEL enLevel, INT8 *pcMessage );
UINT32 FPGA_LogInitKmsg(void);
UINT32 FPGA_LogInit( void );

#endif
