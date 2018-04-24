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
#include <sys/file.h>

#include "securec.h"
#include "FPGA_Common.h"
#include "FPGA_CmdProcess.h"
#include "FPGA_CmdLog.h"
#include "FPGA_CmdMbox.h"
#include "FPGA_CmdPci.h"

#ifdef    __cplusplus
extern "C"{
#endif

/*lint -e708*/

FPGA_MBOX_OPT_INFO g_stFpgaMboxOptInfo =
{
    .ulTimeout = FPGA_MBOX_TIMEOUT,
    .ulDelay = FPGA_MBOX_DELAY_MS
};

/*******************************************************************************
Function     : FPGA_MgmtOpsMutexRead
Description  : Use file lock to prevent multiple processes from calling the tool at the same time
Input        : INT32 ulSlotId
Output       : INT32 * plFd
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtOpsMutexRlock( UINT32 ulSlotId, INT32 * plFd )
{
    INT8 acLockFile[NAME_MAX + 1] = { 0 };
    UINT32 ulRet = 0;
    INT32 lRet = 0;
    INT32 lFd = 0;
    struct flock lock;

    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexRlock Invalid slot_id=%d", ulSlotId );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    if ( NULL == plFd )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexRlock plFd is null" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    
    /* To construct a name of file lock  */
    ulRet = snprintf_s( acLockFile, sizeof( acLockFile ), ( sizeof( acLockFile ) - 1 ), HW_MUTEX_PATH, ulSlotId );
    if ( (size_t)ulRet >= ( sizeof(acLockFile) - 1 ) )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexRlock Mutex path is too long %d.", ulRet );
        return SDKRTN_PROCESS_SPRINTF_FAIL;
    }

    /* Creat file lock */
    lFd = open( acLockFile, ( O_CREAT | O_RDWR ), 0644 );
    if ( lFd < 0 )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexRlock Open Fpga%d.lock file error.\r\n", ulSlotId);
        return SDKRTN_PROCESS_OPEN_FAIL;
    }

    /* initialize the flock struct */
    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0;

    /* control the file wlock */
    lRet = fcntl(lFd, F_SETLK, &lock);
    if ( 0 != lRet )    
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexRlock file lock failed lRet = 0x%x", lRet);    
        close( lFd );        
        return SDKRTN_PROCESS_OPEN_FAIL;    
    }

    *plFd = lFd;

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtOpsMutexWlock
Description  : Use file lock to prevent multiple processes from calling the tool at the same time
Input        : INT32 ulSlotId
Output       : INT32 * plFd
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtOpsMutexWlock( UINT32 ulSlotId, INT32 * plFd )
{
    INT8 acLockFile[NAME_MAX + 1] = { 0 };
    UINT32 ulRet = 0;
    INT32 lRet = 0;
    INT32 lFd = 0;
    struct flock lock;

    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexWlock Invalid slot_id=%d", ulSlotId );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    if ( NULL == plFd )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexWlock plFd is null" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    
    /* To construct a name of file lock  */
    ulRet = snprintf_s( acLockFile, sizeof( acLockFile ), ( sizeof( acLockFile ) - 1 ), HW_MUTEX_PATH, ulSlotId );
    if ( (size_t)ulRet >= ( sizeof(acLockFile) - 1 ) )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexWlock Mutex path is too long %d.", ulRet );
        return SDKRTN_PROCESS_SPRINTF_FAIL;
    }

    /* Creat file lock */
    lFd = open( acLockFile, ( O_CREAT | O_RDWR ), 0644 );
    if ( lFd < 0 )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexWlock Open Fpga%d.lock file error.\r\n", ulSlotId);
        return SDKRTN_PROCESS_OPEN_FAIL;
    }

    /* initialize the flock struct */
    lock.l_type = F_WRLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0;

    /* lock the file */
    lRet = fcntl(lFd, F_SETLK, &lock);
    if ( 0 != lRet )    
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexWlock file lock failed lRet = 0x%x", lRet);    
        close( lFd );        
        return SDKRTN_PROCESS_OPEN_FAIL;    
    }

    *plFd = lFd;

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtOpsMutexUnlock
Description  : Use file lock to unlock
Input        : INT32 * plFd
Output       : none
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtOpsMutexUnlock( INT32 lFd )
{
    INT32 lRet = 0;
    struct flock lock;

    if ( lFd < 0 )
    {
        LOG_ERROR( "FPGA_MgmtOpsMutexUnlock input lFd is error" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    
    /* initialize the flock struct */
    lock.l_type = F_UNLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0;

    /* lock the file */
    lRet = fcntl(lFd, F_SETLK, &lock);
    if ( 0 != lRet )    
    {       
        LOG_ERROR( "FPGA_MgmtOpsMutexUnlock file lock failed lRet = 0x%x", lRet);   
        close( lFd );
        return SDKRTN_PROCESS_OPEN_FAIL;    
    }

    close( lFd );
    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtCmdMutex
Description  : Use file lock to prevent multiple processes from calling the tool at the same time
Input        : INT32 ulSlotId
Output       : INT32 * plFd
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtCmdMutex( UINT32 ulSlotId, INT32 * plFd )
{
    INT8 acLockFile[NAME_MAX + 1] = { 0 };
    UINT32 ulRet = 0;
    INT32 lFd = 0;

    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "Invalid slot_id=%d", ulSlotId );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }
    if ( NULL == plFd )
    {
        LOG_ERROR( " plFd is null" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* To construct a name of file lock  */
    ulRet = snprintf_s( acLockFile, sizeof( acLockFile ), ( sizeof( acLockFile ) - 1 ),
        HFI_MUTEX_PATH, ulSlotId );
    if ( (size_t)ulRet >= ( sizeof(acLockFile) - 1 ) )
    {
        LOG_ERROR( "Mutex path too long %d.", ulRet );
        return SDKRTN_PROCESS_SPRINTF_FAIL;
    }

    /* Creat file lock */
    lFd = open( acLockFile, ( O_CREAT | O_RDWR | O_TRUNC ), 0644 );
    if ( lFd < 0 )
    {
        LOG_ERROR( "ERROR: Open FpgaSdk.lock file error.\r\n" );
        return SDKRTN_PROCESS_OPEN_FAIL;
    }
    
    /* Check if the file is locked */
    if ( 0 != flock( lFd, ( LOCK_EX | LOCK_NB ) ) )
    {
        printf( "Another command is being excuted,please wait.\r\n" );
        close( lFd );
        return SDKRTN_PROCESS_LOCK_FAIL;
    }

    *plFd = lFd;

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtPrintFunc
Description  : Print error info
Input        : UINT32 ulErrorCode
Output       : None
Return       : None
*******************************************************************************/
void FPGA_MgmtPrintFunc( UINT32 ulErrorCode )
{
    switch ( ulErrorCode )
    {
        case LOAD_GET_LOCK_BUSY :
        {
            printf("Error: (%u) Commond busy\r\n", ulErrorCode );
            break;
        }            
        case CLEAR_GET_LOCK_BUSY:
        {
            printf("Error: (%u) Commond busy\r\n", ulErrorCode );
            break;
        }
        case LOAD_AEIID_CHECK_ERR:
        {
            printf("Error: (%u) invalid AEI ID\r\n", ulErrorCode );
            break;
        }
        default :
        {
            printf("Error: (%u) internal error, please try FpgaCmdEntry IF -S <slot num> for details\r\n", ulErrorCode );
        }
    }

    return;
}

/*******************************************************************************
Function     : FPGA_MmgtMboxOptInit
Description  : Initialize option structure of mailbox
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void FPGA_MgmtMboxOptInit( void )
{
    UINT32 i = 0;
    
    for ( i = 0; i < sizeof_array(g_stFpgaMboxOptInfo.strSlots); i++ )
    {
        g_stFpgaMboxOptInfo.strSlots[i].ulHandle = INIT_VALUE;
    }
    return;
}

/*******************************************************************************
Function     : FPGA_MgmtInit
Description  : Initialize option structure of mailbox
Input        : None
Output       : None
Return       : None
*******************************************************************************/
UINT32 FPGA_MgmtInit( void )
{
    UINT32 ulRet = SDKRTN_PROCESS_ERROR_BASE;

    /* Initialize log */
    ulRet = FPGA_LogInit(  );
    if ( SDKRTN_PROCESS_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_LogInit  failed" );    
        return ( INT32 )ulRet;
    }

    /* Initialize Mailbox */
    FPGA_MgmtMboxOptInit(  );
    
    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtLoadMsgInit
Description  : Initialize the load AEI comand
Input        : INT8 *pcHfiId
Output       : MBOX_MSG_DATA *punMsg, UINT32 *pulLen
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtLoadMsgInit( MBOX_MSG_DATA *punMsg, UINT32 *pulLen, INT8 *pcHfiId )
{
    HfiLoadMsgReq *pstrMsgReq = NULL;
    UINT16 usPayLoadLen = 0;
    UINT32 ulRet = 0;

    if ( NULL == punMsg )
    {
        LOG_ERROR( "FPGA_MgmtLoadMsgInit pstMsg is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pulLen )
    {
        LOG_ERROR( "FPGA_MgmtLoadMsgInit pulLen is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pcHfiId )
    {
        LOG_ERROR( "FPGA_MgmtLoadMsgInit pcfiId is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( 0 == *pcHfiId )
    {
        printf( "Input AEI id is NULL\r\n" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Convert the message payload into a structure */
    pstrMsgReq = ( void * )punMsg->strCmdMsgInfo.aucBody;
    usPayLoadLen = sizeof( HfiLoadMsgReq );

    /* Fill in command header */
    punMsg->strCmdMsgInfo.strMsgHead.ulVersion = HFI_CMD_API_VERSION;
    punMsg->strCmdMsgInfo.strMsgHead.ulOpt = HFI_CMD_LOAD;
    punMsg->strCmdMsgInfo.strMsgHead.ulId = HFI_CMD_MSG_FLAG;
    
    /* Fill payload lenth and flag */
    punMsg->strCmdMsgInfo.strMsgHead.usLength = usPayLoadLen;
    punMsg->strCmdMsgInfo.strMsgHead.usFlag = SEND_MSG_LENGTH_FLAG;

    /* Fill in the message body */
    ulRet = strncpy_s( pstrMsgReq->acHfiId, HFI_ID_LEN_MAX, pcHfiId, ( sizeof( pstrMsgReq->acHfiId ) - 1 ) );
    if ( OK != ulRet )
    {
        return SDKRTN_PROCESS_STRNCOPY_ERROR;
    }

    pstrMsgReq->acHfiId[sizeof( pstrMsgReq->acHfiId ) - 1] = '\0';

    pstrMsgReq->ulFpgaMsgFlag = FPGA_MSG_FLAG_INIT_VALUE;
    
    /* Get message lenth */
    *pulLen = sizeof( HFI_CMD_HEAD ) + usPayLoadLen;

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtClearMsgInit
Description  : Initialize the stucture of message
Input        : MBOX_MSG_DATA *punMsg, UINT32 *pulLen
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtClearMsgInit( MBOX_MSG_DATA *punMsg, UINT32 *pulLen )
{
    if ( NULL == punMsg )
    {
        LOG_ERROR( "FPGA_MgmtClearMsgInit pstMsg is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pulLen )
    {
        LOG_ERROR( "FPGA_MgmtClearMsgInit pulLen is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Fill in the command header */
    punMsg->strCmdMsgInfo.strMsgHead.ulVersion = HFI_CMD_API_VERSION;
    punMsg->strCmdMsgInfo.strMsgHead.ulOpt = HFI_CMD_CLEAR;
    punMsg->strCmdMsgInfo.strMsgHead.ulId = HFI_CMD_MSG_FLAG;
    
    /* Fill in the payload lenth and flag */
    punMsg->strCmdMsgInfo.strMsgHead.usLength = FPGA_IMAGE_CLEAR_USLEN;
    punMsg->strCmdMsgInfo.strMsgHead.usFlag = SEND_MSG_LENGTH_FLAG;

    *pulLen = sizeof( HFI_CMD_HEAD );

    return SDKRTN_PROCESS_SUCCESS;
}



/*******************************************************************************
Function     : FPGA_MgmtLoadMsgInitForInquireImageInfo
Description  : Initialize the stucture of message
Input        : MBOX_MSG_DATA *punMsg, UINT32 *pulLen
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtInquireMsgInit( MBOX_MSG_DATA *punMsg, UINT32 *pulLen )
{
    if ( NULL == punMsg )
    {
        LOG_ERROR( "FPGA_MgmtInquireMsgInit pstMsg is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pulLen )
    {
        LOG_ERROR( "FPGA_MgmtInquireMsgInit pulLen is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Fill in the command header */
    punMsg->strCmdMsgInfo.strMsgHead.ulVersion = HFI_CMD_API_VERSION;
    punMsg->strCmdMsgInfo.strMsgHead.ulOpt = HFI_CMD_INQUIRE;
    punMsg->strCmdMsgInfo.strMsgHead.ulId = HFI_CMD_MSG_FLAG;
    
    /* Fill in the payload lenth and flag */
    punMsg->strCmdMsgInfo.strMsgHead.usLength = FPGA_IMAGE_INQUIRE_USLEN;
    punMsg->strCmdMsgInfo.strMsgHead.usFlag = SEND_MSG_LENGTH_FLAG;

    *pulLen = sizeof( HFI_CMD_HEAD );

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtEnableMbox
Description  : Enable mailbox of specify slot
Input        : UINT32 ulSlotIndex
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtEnableMbox( UINT32 ulSlotIndex )
{
        UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
        UINT32 ulBarHandle = 0;
        FPGA_MBOX_WAIT_TIME strMbox = { 0 };

        if ( ulSlotIndex >= FPGA_SLOT_MAX )
        {
            LOG_ERROR( "FPGA_MgmtEnableMbox Invalid slot_id=%d", ulSlotIndex );
            return SDKRTN_PROCESS_INTPUT_ERROR;
        }
        
        /* Get pci info and bar info of specify slot */
        ulRet = FPGA_PciEnableSlotsBar(ulSlotIndex, FPGA_MBOX_BAR_NUM, &ulBarHandle);
        if (OK != ulRet )
        {
            LOG_ERROR( "Unable %d slot BAR space %d", ulSlotIndex, ulRet );
            return ulRet;
        }

        /* The obtained BAR information is assigned to the global variable of the MBOX operation */
        g_stFpgaMboxOptInfo.strSlots[ulSlotIndex].ulHandle = ulBarHandle;

        /* Set the delay time and time-out  */
        strMbox .ulTimeout = g_stFpgaMboxOptInfo.ulTimeout,
        strMbox.ulDelayMsec = g_stFpgaMboxOptInfo.ulDelay,

        ulRet = FPGA_MboxDelayInit(&strMbox);
        if ( OK !=  ulRet )
        {
            LOG_ERROR( "Mbox delay init failed %d", ulRet );
            return ulRet;
        }

        return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtCheckMsg
Description  : Check the received message 
Input        : MBOX_MSG_DATA *punMsgSend, MBOX_MSG_DATA *punMsgRsp, UINT32 ulLength
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtCheckMsg( MBOX_MSG_DATA *punMsgSend, MBOX_MSG_DATA *punMsgRsp, UINT32 ulLength )
{
    FPGA_CMD_ERROR_INFO *pstrErrorInfo = NULL;

    if ( NULL == punMsgSend )
    {
        LOG_ERROR( "punMsgSend is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == punMsgRsp )
    {
        LOG_ERROR( "punMsgRsp is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Check the message flag */
    if ( REQ_MSG_LENGTH_FLAG != punMsgRsp->strCmdMsgInfo.strMsgHead.usFlag )
    {
        LOG_ERROR( "Msg is not response %d", punMsgRsp->strCmdMsgInfo.strMsgHead.usFlag );
        return SDKRTN_PROCESS_MSG_FLAG_ERROR;
    }

    /* Check the message lenth */
    if ( ( punMsgRsp->strCmdMsgInfo.strMsgHead.usLength + sizeof( HFI_CMD_HEAD ) ) > MBOX_MSG_DATA_LEN )
    {
         LOG_ERROR( "Body length is too big %d", ( punMsgRsp->strCmdMsgInfo.strMsgHead.usLength + sizeof( HFI_CMD_HEAD ) ) );
         return SDKRTN_PROCESS_MSG_LENGTH_ERROR;
    }
    
    if ( ulLength < sizeof( HFI_CMD_HEAD ) )
    {
         LOG_ERROR( "Msg length is too small %d", ulLength );
         return SDKRTN_PROCESS_MSG_LENGTH_ERROR;
    }

    /* Check the version */
    if ( punMsgRsp->strCmdMsgInfo.strMsgHead.ulVersion != punMsgSend->strCmdMsgInfo.strMsgHead.ulVersion )
    {
        LOG_ERROR( "Send msg ver(0x%x) is not match req msg ver(0x%x)", punMsgSend->strCmdMsgInfo.strMsgHead.ulVersion,
        punMsgRsp->strCmdMsgInfo.strMsgHead.ulVersion );
        return SDKRTN_PROCESS_MSG_VERSION_ERROR;
    }

    /* Check ID */
    if ( punMsgRsp->strCmdMsgInfo.strMsgHead.ulId != punMsgSend->strCmdMsgInfo.strMsgHead.ulId )
    {
        LOG_ERROR( "Send msg ID(0x%x) is not match req msg ID(0x%x)", punMsgSend->strCmdMsgInfo.strMsgHead.ulId,
        punMsgRsp->strCmdMsgInfo.strMsgHead.ulId );
        return SDKRTN_PROCESS_MSG_ID_ERROR;
    }

    /* Check the operation code */
    if ( punMsgRsp->strCmdMsgInfo.strMsgHead.ulOpt != punMsgSend->strCmdMsgInfo.strMsgHead.ulOpt )
    {
        LOG_ERROR( "Send opt(0x%x) is not match req opt(0x%x)", punMsgSend->strCmdMsgInfo.strMsgHead.ulOpt,
        punMsgRsp->strCmdMsgInfo.strMsgHead.ulOpt );
        
        /* Host has something wrong If the code equal to 0 */
        if ( HFI_CMD_ERROR == punMsgRsp->strCmdMsgInfo.strMsgHead.ulOpt )
        {
            pstrErrorInfo = ( FPGA_CMD_ERROR_INFO * )punMsgRsp->strCmdMsgInfo.aucBody;

            FPGA_MgmtPrintFunc( pstrErrorInfo->ulErroeCode );
        }
        return SDKRTN_PROCESS_MSG_OPT_ERROR;
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtSendMsg
Description  : Send message 
Input        : UINT32 ulSlotIndex, MBOX_MSG_DATA *punMsgSend, MBOX_MSG_DATA *punMsgRsp
Output       : UINT32 *pulLen
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtSendMsg( UINT32 ulSlotIndex, MBOX_MSG_DATA *punMsgSend, MBOX_MSG_DATA *punMsgRsp, UINT32 *pulLen )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    UINT32 ulHandle = INIT_VALUE;

    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MgmtSendMsg Invalid slot_id=%d", ulSlotIndex );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == punMsgSend )
    {
        LOG_ERROR( "FPGA_MgmtSendMsg punMsgSend is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == punMsgRsp )
    {
        LOG_ERROR( "FPGA_MgmtSendMsg punMsgRsp is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pulLen )
    {
        LOG_ERROR( "FPGA_MgmtSendMsg pulLen is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Get handle of the bar */
    ulHandle = g_stFpgaMboxOptInfo.strSlots[ulSlotIndex].ulHandle;

    /* Send message to PF */
    ulRet = FPGA_MboxSendMsg(ulHandle, (void *)punMsgSend, *pulLen);
    if (OK != ulRet)
    {
        LOG_ERROR( "FPGA_MboxSendMsg failed %d", ulRet );
        return ulRet;
    }

    /* Read message from PF */
    ulRet = FPGA_MboxRecvMsg(ulHandle, (void *)punMsgRsp, pulLen);
    if ( OK!= ulRet )
    {
        LOG_ERROR( "FPGA_MboxRecvMsg failed %d", ulRet );
        return ulRet;
    }

    /* Check the recieved message */
    ulRet = FPGA_MgmtCheckMsg(punMsgSend, punMsgRsp, *pulLen);
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MboxCheckMsg failed %d ", ulRet );
        return ulRet;
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtDisableMbox
Description  : Disable mailbox 
Input        : UINT32 ulSlotIndex
Output       : None
Return       : None
*******************************************************************************/
void  FPGA_MgmtDisableMbox( UINT32 ulSlotIndex )
{
    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "Invalid slot_id = %d", ulSlotIndex );
        return ;
    }

    if (g_stFpgaMboxOptInfo.strSlots[ulSlotIndex].ulHandle != ( UINT32 )INIT_VALUE)
    {
        g_stFpgaMboxOptInfo.strSlots[ulSlotIndex].ulHandle = (UINT32)INIT_VALUE;
    }

    return;
}

/*******************************************************************************
Function     : FPGA_MgmtSendMsg
Description  : Process the command 
Input        : UINT32 ulSlotIndex, MBOX_MSG_DATA *punMsgSend, MBOX_MSG_DATA *punMsgRsp
Output       : UINT32 *pulLen
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtProcCmd( UINT32 ulSlotIndex, MBOX_MSG_DATA *punMsgSend,  MBOX_MSG_DATA *punMsgRsp, UINT32 *ulLen )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    INT32   lFd = 0;

    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd Invalid slot_id=%d", ulSlotIndex );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == punMsgSend )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd punMsgSend is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == punMsgRsp )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd punMsgRsp is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == ulLen )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd ulLen is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Multi-process mutex using file lock */
    ulRet = FPGA_MgmtCmdMutex( ulSlotIndex, &lFd );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtCmdMutex failed" );
        return ulRet;
    }

    /* Enable mailbox */
    ulRet = FPGA_MgmtEnableMbox( ulSlotIndex );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtEnableMbox failed" );
        close( lFd );
        return ERROR;
    }

    /* Send message */
    ulRet = FPGA_MgmtSendMsg( ulSlotIndex, punMsgSend, punMsgRsp, ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtSendMsg failed %d", ulRet );
        FPGA_MgmtDisableMbox( ulSlotIndex );
        close( lFd );
        return ulRet;
    }

    close( lFd );
    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtClearHfiImage
Description  : Clear AEI image 
Input        : UINT32 ulSlotIndex
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtClearHfiImage( UINT32 ulSlotIndex )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    UINT32 ulLen = 0;
    MBOX_MSG_DATA unMsgSend = { { { 0 } } };
    MBOX_MSG_DATA unMsgRsp = { { { 0 } } };
    INT32 lFd = 0;
    
    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        printf( "[***TIPS***]Please input the correct slot number.\r\n" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Initialize message */
    ulRet = FPGA_MgmtClearMsgInit( &unMsgSend, &ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FpgaClearMsgInit failed" );
        return ulRet;
    }
    
    /* Set and recognize the file Wlock */
    ulRet = FPGA_MgmtOpsMutexWlock( ulSlotIndex, &lFd );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtClearHfiImage wlock failed" );
        return ulRet;
    }

    /* Send command to PF */
    ulRet = FPGA_MgmtProcCmd( ulSlotIndex, &unMsgSend, &unMsgRsp, &ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd failed, ulRet = 0x%x", ulRet );
        
        /* If Cmd failed , unlock the file Wlock and close the file*/
        if ( OK != FPGA_MgmtOpsMutexUnlock( lFd ))
        {
            LOG_ERROR( "FPGA_MgmtClearHfiImage unlock failed");
        }
        
        return ulRet;
    }

    /* Unlock the file Wlock and close the file*/
    ulRet = FPGA_MgmtOpsMutexUnlock( lFd );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtClearHfiImage unlock failed" );
        return ulRet;
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_CheckAEIId
Description  : Check AEI ID which must consist of '0'~'9' or 'a'~'f'  
Input        : INT8 *pcHfiId
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtCheckAeiId( INT8 *pcHfiId )
{
    INT32 i = 0;

    if ( NULL == pcHfiId ) 
    {
        LOG_ERROR( "Input Hfi id is null" );
        return SDKRTN_PROCESS_AEIID_ERROR;
    }
    
    if ( HFI_ID_LEN != strnlen( pcHfiId, HFI_ID_LEN + 1) )
    {
        LOG_ERROR( "The length[%d] of AEI ID is wrong", strnlen( pcHfiId, HFI_ID_LEN_MAX + 1) );
        return SDKRTN_PROCESS_AEIID_ERROR;
    }
    

    for ( i = 0; i < strnlen( pcHfiId, HFI_ID_LEN_MAX + 1); i++ )
    {
        if ( ( pcHfiId[i] < 'a' || pcHfiId[i] > 'f' ) && ( pcHfiId[i] < '0' || pcHfiId[i] > '9') )
        {
            LOG_ERROR("AEI ID contain a char not belong to '0'~'9' or 'a'~'f'\r\n");
            return SDKRTN_PROCESS_AEIID_ERROR;
        }
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtLoadHfiImage
Description  : Load AEI image 
Input        : UINT32 ulSlotIndex, INT8 *pcHfiId
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtLoadHfiImage( UINT32 ulSlotIndex, INT8 *pcHfiId )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    UINT32 ulLen = 0;
    MBOX_MSG_DATA unMsgSend = { { { 0 } } };
    MBOX_MSG_DATA unMsgRsp = { { { 0 } } };
    INT32 lFd = 0;
    
    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        printf( "[***TIPS***]Please input the correct slot number.\r\n" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pcHfiId )
    {
        LOG_ERROR( "pcfiId is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( 0 == *pcHfiId )
    {
        printf( "[***TIPS***]FPGA image ID shouldn't be NULL.\r\n" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    ulRet = FPGA_MgmtCheckAeiId( pcHfiId );
    if ( OK != ulRet)
    {
        printf("[***TIPS***]FPGA image ID is illegal.\r\n");
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Initialize message */
    ulRet = FPGA_MgmtLoadMsgInit( &unMsgSend, &ulLen, pcHfiId );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FpgaLoadMsgInit failed" );
        return ulRet;
    }

    /* Set and recognize the file Wlock */
    ulRet = FPGA_MgmtOpsMutexWlock( ulSlotIndex, &lFd );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtLoadHfiImage wlock failed" );
        return ulRet;
    }

    /* Send command to PF */
    ulRet = FPGA_MgmtProcCmd( ulSlotIndex, &unMsgSend, &unMsgRsp, &ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd failed, ulRet = 0x%x", ulRet );
        
        /* If Cmd failed , unlock the file Wlock and close the file*/
        if ( OK != FPGA_MgmtOpsMutexUnlock( lFd ) )
        {
            LOG_ERROR( "FPGA_MgmtLoadHfiImage unlock failed");
        }
        
        return ulRet;
    }

    /* Unlock the file Wlock and close the file*/
    ulRet = FPGA_MgmtOpsMutexUnlock( lFd );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtLoadHfiImage unlock failed" );
        return ulRet;
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtInquireFpgaImageInfo
Description  : Inquire the image info
Input        : UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtInquireFpgaImageInfo( UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    UINT32 ulLen = 0;
    MBOX_MSG_DATA unMsgSend = { { { 0 } } };
    MBOX_MSG_DATA unMsgRsp = { { { 0 } } };

    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MgmtInquireImageInfo Invalid slot_id=%d", ulSlotIndex );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    if ( NULL == pstrImgInfo )
    {
        LOG_ERROR( "pstrImgInfo is NULL" );
        return SDKRTN_PROCESS_INTPUT_ERROR;
    }

    /* Initialize the body of message  */
    ulRet = FPGA_MgmtInquireMsgInit( &unMsgSend, &ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtInquireMsgInit failed" );
        return ulRet;
    }

    /* Send command to PF */
    ulRet = FPGA_MgmtProcCmd( ulSlotIndex, &unMsgSend, &unMsgRsp, &ulLen );
    if ( OK != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtProcCmd failed" );
        return ulRet;
    }

    /* To determine whether the query operation */
    if(unMsgRsp.strCmdMsgInfo.strMsgHead.ulOpt == HFI_CMD_INQUIRE)
    {
        ulRet = memcpy_s( pstrImgInfo,  HFI_CMD_BODY_LENGTH, ( FPGA_IMG_INFO * )unMsgRsp.strCmdMsgInfo.aucBody,  HFI_CMD_BODY_LENGTH );
        if (EOK != ulRet)
        {
            LOG_ERROR("[pstrImgInfo] memcpy fail\n");
            return SDKRTN_PROCESS_MEMCPY_ERROR;
        }
    }

    return SDKRTN_PROCESS_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MgmtInquireLEDStatus
Description  : Inquire the status of virtus led
Input        : UINT32 ulSlotIndex
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_MgmtInquireLEDStatus( UINT32 ulSlotIndex )
{
    UINT32 ulRet = SDKRTN_PROCESS_SUCCESS;
    UINT32 ulBarHandle = 0;
    UINT32 ulValue= 0;
    UINT32 i=0;
    
    /* Get pci info and bar info of specify slot */
    ulRet = FPGA_PciEnableSlotsBar(ulSlotIndex, FPGA_MBOX_BAR_NUM, &ulBarHandle);
    if (OK != ulRet )
    {
        LOG_ERROR( "Unable %d slot BAR space %d", ulSlotIndex, ulRet );
        return ulRet;
    }

    if ( HW_VF_VENDOR_ID == g_astrShellType.usVendorId && HW_VF_DEVICE_ID == g_astrShellType.usDeviceId )
    {
        /* Read reg value of bar0 */
        ulRet = FPGA_MboxBaseBarReadReg(ulBarHandle,LED_OFFSET,&ulValue);

        if (SDKRTN_MBX_SUCCESS != ulRet )
        {
            LOG_ERROR( "Base BAR read reg failed:%d", ulRet );
            return ulRet;
        }

        /* Print reg value */
        printf( "LED Status(H): 0x%x\r\n",ulValue);
        printf( "LED Status(B): ");
        for( i = 0; i < LED_STATUS_REG_DIGITS; i++ )
        {
            if ( ulValue & LED_STATUS_TRANSFORM_TOOL )
            {
                printf( "1" );
            }
            else
            {
                printf( "0" );
            }
            ulValue = ulValue << LED_STATUS_LEFTSHIFT;
            if (( i % LED_STATUS_SEPERATE == ( LED_STATUS_SEPERATE - 1 ) ) && ( i!=( LED_STATUS_REG_DIGITS - 1 )))
            {
                printf( "|" );
            }
        }
        printf("\r\n");

        return ulRet;    
    }
    
    /* No lighting function for OCL */
    else if ( HW_OCL_PF_VENDOR_ID == g_astrShellType.usVendorId && HW_OCL_PF_DEVICE_ID == g_astrShellType.usDeviceId )	
    {
        printf("General purpose architecture device doesn't support user LED.\r\n");
        return SDKRTN_PROCESS_SUCCESS;
    }
    else
    {
        /* Report type ID error after the device ID match fails */
        printf("FPGA shell type error.\r\n");
        return SDKRTN_PROCESS_SHELL_TYPE_ERROR;           

    }
}

#ifdef    __cplusplus
}
#endif
