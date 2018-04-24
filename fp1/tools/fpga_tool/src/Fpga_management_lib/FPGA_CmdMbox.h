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

#ifndef _FPGA_CMDMBOX_H_
#define _FPGA_CMDMBOX_H_
#include <stdbool.h>


typedef struct tagFPGA_MBOX_WAIT_TIME
{
    UINT32  ulTimeout;
    UINT32  ulDelayMsec;
}FPGA_MBOX_WAIT_TIME;

#define MBOX_REG_SIZE                      4                     /* Reg size */
#define LENGTH_MASK                        0x00000004            /* The length is an integer multiple of 4 bytes */
#define NS_COUNT_FOR_MS                    1000000               /* 1ms */
#define  FPGA_MAILBOX_MEM_NUM              16                    /* message lenth:64byte = 32bit X 16  */

/* Delay time: 5*60=300s */
#define RECV_MSG_DELAY_MULTIPLE            60                    
#define WAIT_PFUNLOCK_TIMES                1000


/**************************    OCL User PF rag base **********************************/
#define FPGA_OCL_PF_BASE_REG               0x330000             /* OCL PF mailbox reg base */

/**************************     Define reg offset   **********************************/
#define FPGA_VF_CNTRL_REG                  0x00000000           /* VF mailbox control reg */
#define FPGA_VF_MEM_BASE_REG               0x00000040           /* vf memory start address£¨16 registers£¨64byte£¨reg_vf_mbmm_addr0 */

/***************************   Define reg bit *************************************/
#define FPGA_VF_PFACK                      0x00000020           /* 1:PF has recieved the message */
#define FPGA_VF_PFREQ                      0x00000010           /* 1:PF has send a message */
#define FPGA_VF_PFU                        0x00000008           /* PF occupies the communication channel */
#define FPGA_VF_VFU                        0x00000004           /* VF occupies the communication channel */
#define FPGA_VF_ACK                        0x00000002           /* The message has been read, informing the PF */
#define FPGA_VF_REQ                        0x00000001           /* The message has been written, informing the PF to receive the message */

#define FPGA_MBVFICR_VFREQ_MASK            0x00005555           
#define FPGA_MBVFICR_VFREQ_VF1             0x00000001           
#define FPGA_MBVFICR_VFACK_MASK            0x0000AAAA           
#define FPGA_MBVFICR_VFACK_VF1             0x00000002           
#define FPGA_MBMASK_VF1                    0x00000001           /* Enable one VF interrupt */


/*************************¥ÌŒÛ¬Î∂®“Â************************************/
#define MBX_ERROR                          4

#define SDKRTN_MBX_BASE                    ( SDKRTN_ERR_BASE + (MBX_ERROR << 8 ) )
#define SDKRTN_MBX_SUCCESS                 0
#define SDKRTN_MBX_FAIL                    ( SDKRTN_MBX_BASE )                
#define SDKRTN_MBX_INPUT_ERROR             ( SDKRTN_MBX_BASE + 0x01 )        
#define SDKRTN_MBX_INIT_ERROR              ( SDKRTN_MBX_BASE + 0x02 )          
#define SDKRTN_MBX_PADDR_NULL              ( SDKRTN_MBX_BASE + 0x03 )          
#define SDKRTN_MBX_TIMEOUT_ERROR           ( SDKRTN_MBX_BASE + 0x04 )         
#define SDKRTN_MBX_CLEAR_ERROR             ( SDKRTN_MBX_BASE + 0x05 )          
#define SDKRTN_MBX_MBOX_BUSY               ( SDKRTN_MBX_BASE + 0x06 )         
#define SDKRTN_MBX_OFFSET_ERROR            ( SDKRTN_MBX_BASE + 0x07 )         
#define SDKRTN_MBX_PDAT_NULL               ( SDKRTN_MBX_BASE + 0x08 )         
#define SDKRTN_MBX_SIZE_ERROR              ( SDKRTN_MBX_BASE + 0x09 )         
#define SDKRTN_MBX_CHKMSG_ERROR            ( SDKRTN_MBX_BASE + 0x0A )          
#define SDKRTN_MBX_CHKACK_ERROR            ( SDKRTN_MBX_BASE + 0x0B )          
#define SDKRTN_MBX_VFLOCK_ERROR            ( SDKRTN_MBX_BASE  + 0x0C )         
#define SDKRTN_MBX_VFUNLOCK_ERROR          ( SDKRTN_MBX_BASE  + 0x0D )         
#define SDKRTN_MBX_HANDLENUM_ERROR         ( SDKRTN_MBX_BASE  + 0x0E )         
#define SDKRTN_MBX_BAR_ALLOCATE_ERROR      ( SDKRTN_MBX_BASE  + 0x0F )         
#define SDKRTN_MBX_BAR_STRUCT_ERROR        ( SDKRTN_MBX_BASE  + 0x10 )        
#define SDKRTN_MBX_READ_ERROR              ( SDKRTN_MBX_BASE  + 0x11 )        
#define SDKRTN_MBX_WRITE_ERROR             ( SDKRTN_MBX_BASE  + 0x12 )         
#define SDKRTN_MBX_SIGNAL_ERROR            ( SDKRTN_MBX_BASE  + 0x13 )         
#define SDKRTN_MBX_SHELL_TYPE_ERROR            ( SDKRTN_MBX_BASE  + 0x14 )         

UINT32 FPGA_MboxDelayInit( FPGA_MBOX_WAIT_TIME *pstrMbox );
UINT32 FPGA_MboxRecvMsg(UINT32 ulHandle, void *pMsg, UINT32 * pulLength);
UINT32 FPGA_MboxSendMsg(UINT32 ulHandle, void *pMsg, UINT32 ulLength);
UINT32 FPGA_MboxBaseBarReadReg( UINT32 ulHandle, UINT32 ulOffset, UINT32 *pulValue );

#endif
