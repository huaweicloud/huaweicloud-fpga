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
 
#ifndef __FPGA_CMD_PROCESS_H__
#define __FPGA_CMD_PROCESS_H__
#include <stdbool.h>


#define  MBOX_MSG_DATA_LEN                 64
#define  MBOX_MSG_HEAD_DATA_LEN            48
#define  SEND_MSG_LENGTH_FLAG              0x5A5A
#define  REQ_MSG_LENGTH_FLAG               0xA5A5

#define  FPGA_MSG_FLAG_INIT_VALUE          0
#define  FPGA_MBOX_BAR_NUM                 0

#define FPGA_MBOX_TIMEOUT                  5000                     /* MBOX time out counting times */
#define FPGA_MBOX_DELAY_MS                 1                        /* MBOX delay 1ms */

#define HFI_CMD_API_VERSION                0x100
#define HFI_CMD_BODY_LENGTH                48

#define HFI_CMD_MSG_FLAG                   0x4D534746               /* "MSGF" message verify flag */
#define LED_OFFSET                         0x00004400               /* Virtus led reg offset */

#define FPGA_IMAGE_CLEAR_USLEN           0                        /* The USLENGTH of clear fpga image */
#define FPGA_IMAGE_INQUIRE_USLEN           0                        /* The USLENGTH of inquiring fpga image */
#define HFI_MUTEX_PATH                     "/tmp/fpgasdk%d.lock"
#define LED_STATUS_REG_DIGITS              32                       /* The lenth of virtus led status reg */    
#define LED_STATUS_SEPERATE                4                        
#define LED_STATUS_LEFTSHIFT               1                        /* Shift one bit */
#define LED_STATUS_TRANSFORM_TOOL          0x80000000               /* Decimal conversion to binary */

#define LOAD_GET_LOCK_BUSY                 1  /* Get lock busy */
#define LOAD_AEIID_CHECK_ERR                     9  /* AEI ID is invalid */
#define CLEAR_GET_LOCK_BUSY                101  /* Clear command Get lock busy */

/************************* Define error code ************************************/
#define PROCESS_ERROR                          6
#define SDKRTN_PROCESS_SUCCESS                 0
#define SDKRTN_PROCESS_ERROR_BASE              ( SDKRTN_ERR_BASE  +  ( PROCESS_ERROR << 8  ) )
#define SDKRTN_PROCESS_INTPUT_ERROR            ( SDKRTN_PROCESS_ERROR_BASE + 0x1 )
#define SDKRTN_PROCESS_STRNCOPY_ERROR          ( SDKRTN_PROCESS_ERROR_BASE + 0x2 )
#define SDKRTN_PROCESS_MSG_FLAG_ERROR          ( SDKRTN_PROCESS_ERROR_BASE + 0x3 )
#define SDKRTN_PROCESS_MSG_LENGTH_ERROR        ( SDKRTN_PROCESS_ERROR_BASE + 0x4 )
#define SDKRTN_PROCESS_MSG_VERSION_ERROR       ( SDKRTN_PROCESS_ERROR_BASE + 0x5 )
#define SDKRTN_PROCESS_MSG_ID_ERROR            ( SDKRTN_PROCESS_ERROR_BASE + 0x6 )
#define SDKRTN_PROCESS_MSG_OPT_ERROR           ( SDKRTN_PROCESS_ERROR_BASE + 0x7 )
#define SDKRTN_PROCESS_MEMCPY_ERROR            ( SDKRTN_PROCESS_ERROR_BASE + 0x8 )
#define SDKRTN_PROCESS_LOCK_FAIL               ( SDKRTN_PROCESS_ERROR_BASE + 0x9 )
#define SDKRTN_PROCESS_OPEN_FAIL               ( SDKRTN_PROCESS_ERROR_BASE + 0xA )
#define SDKRTN_PROCESS_SPRINTF_FAIL            ( SDKRTN_PROCESS_ERROR_BASE + 0xB )
#define SDKRTN_PROCESS_SHELL_TYPE_ERROR        ( SDKRTN_PROCESS_ERROR_BASE + 0xC )
#define SDKRTN_PROCESS_AEIID_ERROR             ( SDKRTN_PROCESS_ERROR_BASE + 0xD )

typedef struct tagFPGA_MBOX_OPT_INFO
{
    struct
    {
        UINT32 ulHandle;
    } strSlots[FPGA_SLOT_MAX];
    UINT32 ulTimeout;
    UINT32 ulDelay;
}FPGA_MBOX_OPT_INFO;

typedef struct tagHFI_CMD_HEAD
{
    UINT32 ulVersion;
    UINT32 ulOpt;
    UINT32 ulId;
    UINT16 usLength;
    UINT16 usFlag;
} HFI_CMD_HEAD ;

typedef struct tagCMD_MSG_INFO
{
    HFI_CMD_HEAD strMsgHead;
    INT8               aucBody[HFI_CMD_BODY_LENGTH];
}CMD_MSG_INFO;


typedef union tagMBOX_MSG_DATA
{
    CMD_MSG_INFO strCmdMsgInfo;

    INT8  aucData[MBOX_MSG_DATA_LEN];
}MBOX_MSG_DATA;



typedef struct tagHfiLoadMsgReq
{
    INT8          acHfiId[HFI_ID_LEN_MAX];
    UINT32      ulFpgaMsgFlag;
    UINT32      ulReserved;
} HfiLoadMsgReq;

typedef enum tagFpgaCmdListForHost
{
    HFI_CMD_ERROR = 0,
    HFI_CMD_LOAD = 1,
    HFI_CMD_INQUIRE = 2,
    HFI_CMD_CLEAR = 3,
    HFI_CMD_END
}FpgaCmdListForHost;

typedef struct tagFPGA_CMD_ERROR_INFO
{
    UINT32   ulErroeCode;
    UINT32   ulInfo;
}FPGA_CMD_ERROR_INFO;

extern FPGA_MBOX_OPT_INFO g_stFpgaMboxOptInfo;

#endif
