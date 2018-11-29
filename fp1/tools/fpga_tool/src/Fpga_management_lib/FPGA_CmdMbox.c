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


#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include "securec.h"
#include "FPGA_Common.h"
#include "FPGA_CmdMbox.h"
#include "FPGA_CmdLog.h"
#include "FPGA_CmdPci.h"
#include "FPGA_CmdProcess.h"


#ifdef    __cplusplus
extern "C"{
#endif

FPGA_MBOX_WAIT_TIME g_strMboxDelay = {0};

/*******************************************************************************
Function     : msleep
Description  : delay specify ms
Input        : INT32 lMs
Output       : None
Return       : None
*******************************************************************************/
void msleep(INT32 lMs)
{
    struct timespec strTime;
    strTime.tv_sec = 0;
    strTime.tv_nsec = ( long )( unsigned )( lMs * NS_COUNT_FOR_MS );

    nanosleep( &strTime, NULL );
    return;
}

/*******************************************************************************
Function     : FPGA_MboxDelayInit
Description  : Mailbox wait time initialize
Input        : FPGA_MBOX_WAIT_TIME *pstrMbox
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MboxDelayInit( FPGA_MBOX_WAIT_TIME *pstrMbox )
{
    if ( NULL == pstrMbox )
    {
         LOG_ERROR( "pstrMbox is NULL" );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    g_strMboxDelay.ulDelayMsec = pstrMbox->ulDelayMsec;
    g_strMboxDelay.ulTimeout= pstrMbox->ulTimeout;

    return OK;
}

/*******************************************************************************
Function     : FPGA_MboxReadReg
Description  : Read mailbox reg
Input        : UINT32 ulHandle, UINT32 ulOffset
Output       : UINT32 *pulValue
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MboxReadReg( UINT32 ulHandle, UINT32 ulOffset, UINT32 *pulValue )
{
    volatile UINT32 *pulRegAddr = NULL;
    FPGA_PCI_BAR_INFO *pStrBar = NULL;
    UINT32 ulOffsetTmp = 0;

    if ( NULL == pulValue )
    {
         LOG_ERROR( "FPGA_MboxReadReg pulValue is NULL" );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MboxReadReg Invalid ulHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }
    if ( 0 != ulOffset % MBOX_REG_SIZE )
    {
        LOG_ERROR( "FPGA_MboxReadReg Invalid ulOffset=%d", ulOffset );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    /* Use the Device ID and Vendor ID to distinguish the address of different shell register */
    if ( HW_VF_VENDOR_ID == g_astrShellType.usVendorId && HW_VF_DEVICE_ID == g_astrShellType.usDeviceId )
    {
        ulOffsetTmp = ulOffset;
    }
    else if ( HW_OCL_PF_VENDOR_ID == g_astrShellType.usVendorId && HW_OCL_PF_DEVICE_ID == g_astrShellType.usDeviceId )
    {
        ulOffsetTmp = FPGA_OCL_PF_BASE_REG + ulOffset;
    }
    else
    {
        /* Report type error after the device ID match failed */
        printf("FPGA shell type error.\r\n");
        return SDKRTN_MBX_SHELL_TYPE_ERROR;           
    }

    /* Get BAR info */
    pStrBar = FPGA_PciGetBar(ulHandle);
    if ( NULL == pStrBar )
    {
        LOG_ERROR( "pStrBar is NULL" );
        return SDKRTN_MBX_BAR_STRUCT_ERROR;
    }
    if ( false == pStrBar->bAllocatedFlag )
    {
        LOG_ERROR( "Bar is not allocated" );
        return SDKRTN_MBX_BAR_ALLOCATE_ERROR;
    }

    if ( NULL == pStrBar->pMemBase )
    {
        LOG_ERROR( "MemBase is NULL" );
        return SDKRTN_MBX_PADDR_NULL;
    }
    
    /* Offset + 4 bytes can not be greater than the size of the BAR space */
    if ( ( ( UINT64 )ulOffsetTmp + sizeof(UINT32) ) > pStrBar->ullMemSize )
    {
        LOG_ERROR( "Invalid is ulOffset %d", ulOffsetTmp );
        return SDKRTN_MBX_OFFSET_ERROR;
    }

    pulRegAddr = ( UINT32 * )( (UINT64 )pStrBar->pMemBase + ulOffsetTmp );

    *pulValue = *( volatile UINT32 * )pulRegAddr;

    return SDKRTN_MBX_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MboxWriteReg
Description  : Write mailbox reg
Input        : UINT32 ulHandle, UINT32 ulOffset, UINT32 ulValue
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MboxWriteReg( UINT32 ulHandle, UINT32 ulOffset, UINT32 ulValue )
{
    volatile UINT32 *pulRegAddr = NULL;
    FPGA_PCI_BAR_INFO *pStrBar = NULL;
    UINT32 ulOffsetTmp = 0;

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MboxWriteReg Invalid ulHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    if ( 0 != ulOffset % MBOX_REG_SIZE )
    {
        LOG_ERROR( "FPGA_MboxWriteReg Invalid ulOffset=%d", ulOffset );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    /* Use the Device ID and Vendor ID to distinguish the address of different shell register */
    if ( HW_VF_VENDOR_ID == g_astrShellType.usVendorId && HW_VF_DEVICE_ID == g_astrShellType.usDeviceId )
    {
        ulOffsetTmp = ulOffset;
    }
    else if ( HW_OCL_PF_VENDOR_ID == g_astrShellType.usVendorId && HW_OCL_PF_DEVICE_ID == g_astrShellType.usDeviceId )
    {
        ulOffsetTmp = FPGA_OCL_PF_BASE_REG + ulOffset;
    }
    else
    {
        /* Report type error after the device ID match failed */
        printf("FPGA shell type error.\r\n");
        return SDKRTN_MBX_SHELL_TYPE_ERROR;           	
    }

    /* Get BAR space info */
    pStrBar = FPGA_PciGetBar( ulHandle );
    if ( NULL == pStrBar )
    {
        LOG_ERROR( "pStrBar is NULL" );
        return SDKRTN_MBX_BAR_STRUCT_ERROR;
    }
    if ( false == pStrBar->bAllocatedFlag )
    {
        LOG_ERROR( "Bar is not allocated" );
        return SDKRTN_MBX_BAR_ALLOCATE_ERROR;
    }

    if ( NULL == pStrBar->pMemBase )
    {
        LOG_ERROR( "MemBase is NULL" );
        return SDKRTN_MBX_PADDR_NULL;
    }
    
    /* Offset + 4 bytes can not be greater than the size of the BAR space */
    if ( ( ( UINT64 )ulOffsetTmp + sizeof(UINT32) ) > pStrBar->ullMemSize )
    {
        LOG_ERROR( "Invalid is ulOffset" );
        return SDKRTN_MBX_OFFSET_ERROR;
    }

    pulRegAddr = ( UINT32 * )( ( UINT64 )pStrBar->pMemBase + ulOffsetTmp );

    *( volatile UINT32 * )pulRegAddr = ulValue;

    return SDKRTN_MBX_SUCCESS;
}


/*******************************************************************************
Function     : FPGA_MBoxVFLock
Description  : Lock the mailbox, the mailbox only can be used by VF
Input        : UINT32 ulHandle
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MBoxVFLock( UINT32 ulHandle )
{
    UINT32 ulRet = SDKRTN_MBX_FAIL;
    UINT32 ulValue = 0;
    UINT32 ulCount = 0;

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MBoxVFLock Invalid ulHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    /* Read VFU bit, check for new message in 5 seconds */
    ulCount = g_strMboxDelay.ulTimeout;
    while ( ulCount )
    {
        /* Lock VFU */
        ulRet = FPGA_MboxWriteReg( ulHandle, FPGA_VF_CNTRL_REG, FPGA_VF_VFU );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox write reg failed:%d", ulRet );
            return ulRet;
        }
        /* Read VFU */
        ulRet = FPGA_MboxReadReg( ulHandle, FPGA_VF_CNTRL_REG, &ulValue );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox read reg failed:%d", ulRet );
            return ulRet;
        }
        /* if has new message£¬then break */
        if ( ( ulValue & FPGA_VF_VFU ) )
        {
            ulRet = SDKRTN_MBX_SUCCESS;        
            break ;
        }

        msleep( ( INT32 )g_strMboxDelay.ulDelayMsec );
        ulCount--;
    }

    if ( 0 == ulCount )
    {
        LOG_ERROR( "Vf Lock failed, Timeout, VFU Value=0x%x", ulValue);
        return SDKRTN_MBX_VFLOCK_ERROR;
    }

    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MBoxVFUnlock
Description  : unLock the mailbox
Input        : UINT32 ulHandle
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MBoxVFUnlock( UINT32 ulHandle )
{
    UINT32 ulValue = 0;
    UINT32 ulRet = SDKRTN_MBX_FAIL;

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MBoxVFUnlock Invalid ulHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }
    
    /* Clear VFU */
    ulValue = 0;
    ulRet = FPGA_MboxWriteReg( ulHandle, FPGA_VF_CNTRL_REG, ulValue );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        return SDKRTN_MBX_WRITE_ERROR;
    }

    return SDKRTN_MBX_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MboxClearAckReq
Description  : Clear the ack&req bit
Input        : UINT32 ulHandle
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MboxClearAckReq( UINT32 ulHandle )
{
    UINT32 ulRet = SDKRTN_MBX_FAIL;
    UINT32 ulValue = 0;

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MboxClearAckReq Invalid lHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    ulRet = FPGA_MboxReadReg( ulHandle, FPGA_VF_CNTRL_REG, &ulValue );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox Read reg failed:%d", ulRet );
        return SDKRTN_MBX_READ_ERROR;
    }
    
    ulValue = ulValue |FPGA_VF_PFACK |FPGA_VF_PFREQ;

    /* The PFACK and PFREQ bit are writing clear attribute, cleared by writing control register */
    ulRet = FPGA_MboxWriteReg( ulHandle, FPGA_VF_CNTRL_REG, ulValue );
    if (SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        return SDKRTN_MBX_WRITE_ERROR;
    }

    return SDKRTN_MBX_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MboxRecvMsg
Description  : Read the message from mailbox
Input        : UINT32 ulHandle
Output       : void *pMsg, UINT32 * pulLength
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MboxRecvMsg( UINT32 ulHandle, void *pMsg, UINT32 * pulLength )
{
    UINT32 ulCount = 0;
    UINT32 ulRet = SDKRTN_MBX_FAIL;
    UINT32 ulValue = 0;
    UINT32 ulRegOffset = 0;
    INT32   i = 0;

    if ( ulHandle >= FPGA_VF_BAR_NUM_MAX )
    {
        LOG_ERROR( "FPGA_MboxRecvMsg Invalid slot_id=%d", ulHandle );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    if ( NULL == pMsg )
    {
        LOG_ERROR( "FPGA_MboxRecvMsg pMsg is NULL" );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    if ( NULL == pulLength )
    {
        LOG_ERROR( "FPGA_MboxRecvMsg pulLength is NULL" );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    /* step1: Read PFREQ bit, check for new message in 3 minutes */
    ulCount = g_strMboxDelay.ulTimeout * RECV_MSG_DELAY_MULTIPLE;
    ulRegOffset = FPGA_VF_CNTRL_REG;
    while ( ulCount )
    {
        ulRet = FPGA_MboxReadReg( ulHandle, ulRegOffset, &ulValue );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox read reg failed:%d", ulRet );
            return ulRet;
        }
        /* if has new message£¬then break */
        if ( ( ulValue & FPGA_VF_PFREQ ) )
        {
            break ;
        }

        msleep( ( INT32 )g_strMboxDelay.ulDelayMsec );
        ulCount--;
    }

    if ( 0 == ulCount )
    {
        LOG_ERROR( "PF send msg timeout" );
        return SDKRTN_MBX_TIMEOUT_ERROR;
    }

    /* step2:Read message */
    for ( i = 0; i < FPGA_MAILBOX_MEM_NUM; i++ )
    {
        /* Read 4 bytes each time */
        ulRegOffset = FPGA_VF_MEM_BASE_REG + ( UINT32 )( i * MBOX_REG_SIZE );
        ulRet = FPGA_MboxReadReg( ulHandle, ulRegOffset, ( UINT32 * )( ( UINT64 )pMsg +( UINT32 )( i * MBOX_REG_SIZE ) ) );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox read reg failed:%d", ulRet );
            return  ulRet;
        }
    }
    
    ulRegOffset = FPGA_VF_CNTRL_REG;
    /* Set PFREQ to clear this bit */
    ulRet = FPGA_MboxWriteReg( ulHandle,  ulRegOffset, FPGA_VF_PFREQ );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        return SDKRTN_MBX_WRITE_ERROR;
     }

    /* step3:Set ACK bit */
    ulRet = FPGA_MboxWriteReg( ulHandle,  ulRegOffset, ( FPGA_VF_ACK ) );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        return ulRet;
    }
    *pulLength = FPGA_MAILBOX_MEM_NUM * MBOX_REG_SIZE;

    return SDKRTN_MBX_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MboxSendMsg
Description  : Send the message by mailbox
Input        : UINT32 ulHandle, void *pMsg, UINT32 ulLength
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MboxSendMsg( UINT32 ulHandle, void *pMsg, UINT32 ulLength )
{
    UINT32 ulRet = SDKRTN_MBX_FAIL;
    INT32 i = 0;
    UINT32 ulRegOffset = 0;
    UINT32 ulSendNum = 0;
    UINT32 ulCount = 0;
    UINT32 ulValue= 0;

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MboxSendMsg Invalid lHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }
    if ( NULL == pMsg )
    {
         LOG_ERROR( "FPGA_MboxSendMsg pMsg is NULL" );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    /* Check if the length is less than 4 bytes or more than 64 bytes */
    if ( ( ulLength > MBOX_MSG_DATA_LEN ) || ( 0 != ulLength % LENGTH_MASK ) || ( ulLength < MBOX_REG_SIZE ) )
    {
         LOG_ERROR( "FPGA_MboxSendMsg Invalid ulLength=%d", ulLength );
         return SDKRTN_MBX_INPUT_ERROR;
    }


    /* step1:Set VFU */
    ulRet = FPGA_MBoxVFLock( ulHandle );
    if ( ulRet )
    {
        LOG_ERROR( "Mbox lock failed :%d", ulRet );
        return ulRet;
    }

    /* step2: Clear Ack and req */
    ulRet = FPGA_MboxClearAckReq( ulHandle );
    if ( ulRet )
    {
        LOG_ERROR( "Mbox Clear failed :%d", ulRet );
        return SDKRTN_MBX_CLEAR_ERROR;
    }

    /* step3:Write message according to the specified length */
    ulSendNum = ulLength / LENGTH_MASK;
    for ( i = 0; i < ulSendNum ; i++ )
    {
        ulRegOffset = FPGA_VF_MEM_BASE_REG + ( i * MBOX_REG_SIZE );
        ulRet = FPGA_MboxWriteReg( ulHandle, ulRegOffset, ( ( UINT32 * )pMsg )[i] );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox write reg failed:%d", ulRet );

            ulRet = FPGA_MBoxVFUnlock( ulHandle );
            if ( SDKRTN_MBX_SUCCESS != ulRet )
            {
                LOG_ERROR( "Mbox write reg failed:%d", ulRet );
                return  ulRet;
            }
            return SDKRTN_MBX_WRITE_ERROR;
        }
    }

    /* step4:Set REQ bit */
    ulRegOffset = FPGA_VF_CNTRL_REG;
    ulRet = FPGA_MboxWriteReg( ulHandle,  ulRegOffset, (FPGA_VF_REQ | FPGA_VF_VFU) );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        ulRet = FPGA_MBoxVFUnlock( ulHandle );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox write reg failed:%d", ulRet );
            return  ulRet;
        }
        return SDKRTN_MBX_WRITE_ERROR;
    }

    /* step5:Check if PF has recieved the message */
    ulCount = g_strMboxDelay.ulTimeout * RECV_MSG_DELAY_MULTIPLE;
    ulRegOffset = FPGA_VF_CNTRL_REG;
    while (ulCount)
    {
        ulRet = FPGA_MboxReadReg( ulHandle, ulRegOffset, &ulValue );
        if (SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox read reg failed:%d", ulRet );
            ulRet = FPGA_MBoxVFUnlock( ulHandle );
            if ( SDKRTN_MBX_SUCCESS != ulRet )
            {
                LOG_ERROR( "Mbox write reg failed:%d", ulRet );
                return  ulRet;
            }
            return SDKRTN_MBX_WRITE_ERROR;
        }
        
        /* If PF has recieved the message, then break */
        if ( (ulValue & FPGA_VF_PFACK) )
        {
             /* Set ACK to clear this bit */
             ulRet = FPGA_MboxWriteReg( ulHandle,  ulRegOffset, ( FPGA_VF_PFACK | FPGA_VF_VFU ) );
             if ( SDKRTN_MBX_SUCCESS != ulRet )
             {
                 LOG_ERROR( "Mbox write reg failed:%d", ulRet );
                 ulRet = FPGA_MBoxVFUnlock( ulHandle );
                 if ( SDKRTN_MBX_SUCCESS != ulRet )
                 {
                     LOG_ERROR( "Mbox write reg failed:%d", ulRet );
                     return  ulRet;
                 }
                 return SDKRTN_MBX_WRITE_ERROR;
             }

             break ;
        }

        msleep( ( INT32 )g_strMboxDelay.ulDelayMsec );
        ulCount--;
    }

    if ( 0 == ulCount )
    {
        LOG_ERROR( "PF recv msg timeout" );
        ulRet = FPGA_MBoxVFUnlock( ulHandle );
        if ( SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Mbox write reg failed:%d", ulRet );
            return  ulRet;
        }
        return SDKRTN_MBX_TIMEOUT_ERROR;
    }

    /* step6:Clear VFU */
    ulRet = FPGA_MBoxVFUnlock( ulHandle );
    if ( SDKRTN_MBX_SUCCESS != ulRet )
    {
        LOG_ERROR( "Mbox write reg failed:%d", ulRet );
        return  ulRet;
    }
    
    return SDKRTN_MBX_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MboxBaseBarReadReg
Description  : Read the base bar reg
Input        : UINT32 ulHandle, UINT32 ulOffset
Output       : UINT32 *pulValue
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MboxBaseBarReadReg( UINT32 ulHandle, UINT32 ulOffset, UINT32 *pulValue )
{
    volatile UINT32 *pulRegAddr = NULL;
    FPGA_PCI_BAR_INFO *pStrBar = NULL;

    if ( NULL == pulValue )
    {
         LOG_ERROR( "FPGA_MboxReadReg pulValue is NULL" );
         return SDKRTN_MBX_INPUT_ERROR;
    }

    if ( ulHandle >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_MboxReadReg Invalid ulHandle=%d", ulHandle );
         return SDKRTN_MBX_INPUT_ERROR;
    }
    
    if ( 0 != ulOffset % MBOX_REG_SIZE )
    {
        LOG_ERROR( "FPGA_MboxReadReg Invalid ulOffset=%d", ulOffset );
        return SDKRTN_MBX_INPUT_ERROR;
    }

    /* Get bar information */
    pStrBar = FPGA_PciGetBar(ulHandle);
    if ( NULL == pStrBar )
    {
        LOG_ERROR( "pStrBar is NULL" );
        return SDKRTN_MBX_BAR_STRUCT_ERROR;
    }
    
    if ( false == pStrBar->bAllocatedFlag )
    {
        LOG_ERROR( "Bar is not allocated" );
        return SDKRTN_MBX_BAR_ALLOCATE_ERROR;
    }

    if ( NULL == pStrBar->pMemBase )
    {
        LOG_ERROR( "MemBase is NULL" );
        return SDKRTN_MBX_PADDR_NULL;
    }
    
    /* Offset + 4 bytes can not be greater than the size of the BAR space */
    if ( ( ( UINT64 )ulOffset + sizeof(UINT32) ) > pStrBar->ullMemSize )
    {
        LOG_ERROR( "Invalid is ulOffset %d", ulOffset );
        return SDKRTN_MBX_OFFSET_ERROR;
    }

    pulRegAddr = ( UINT32 * )( (UINT64 )pStrBar->pMemBase + ulOffset );

    *pulValue = *( volatile UINT32 * )pulRegAddr;

    return SDKRTN_MBX_SUCCESS;

}
#ifdef    __cplusplus
}
#endif
