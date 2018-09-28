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

#include "securec.h"
#include "FPGA_CmdCommon.h"
#include "FPGA_CmdMonitorMain.h"
#include "FPGA_CmdParse.h"

#ifdef    __cplusplus
extern "C"{
#endif


FPGA_CMD_PARA   g_strFpgaModule = { 0 };

COMMAND_PROC_FUNC g_pafnFpgaCmdList[CMD_PARSE_END] =
{
    [CMD_HFI_LOAD] = FPGA_MonitorLoadHfi,
    [CMD_HFI_CLEAR] = FPGA_MonitorClearHfi,
    [CMD_IMAGE_INQUIRE] = FPGA_MonitorInquireFpgaImageInfo,
    [CMD_RESOURSE_INQUIRE] = FPGA_MonitorDisplayDevice,
    [CMD_LED_STATUS_INQUIRE] = FPGA_MonitorInquireLEDStatus,
    [CMD_TOOL_VERSION] = NULL,
};
UINT32 g_ulparseParaFlag = 0;


/*******************************************************************************
Function     : FPGA_MonitorInitModule
Description  : Global variable initialize
Input        : None
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MonitorInitModule( void )
{
    INT32 lRet = ( INT32 )SDKRTN_MONITOR_ERROR_BASE;

    lRet = memset_s( &g_strFpgaModule, sizeof( g_strFpgaModule ), 0, sizeof (g_strFpgaModule ) );
    if ( OK != lRet )
    {
        return SDKRTN_MONITOR_MEMSET_ERROR;
    }
    g_strFpgaModule.ulOpcode= INIT_VALUE;
    g_strFpgaModule.ulSlotIndex= INIT_VALUE;
    g_strFpgaModule.bShowInfo= false;

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorExecuteCmd
Description  : Excute the cmd
Input        : None
Output       : None
Return       : 0:sucess other:fail    
*******************************************************************************/
UINT32 FPGA_MonitorExecuteCmd( void )
{
    if ( g_strFpgaModule.ulOpcode >= CMD_PARSE_END )
    {
        LOG_DEBUG( "Invalid cmd %d", g_strFpgaModule.ulOpcode );
        return SDKRTN_MONITOR_OPCODE_ERROR;
    }

    if ( g_pafnFpgaCmdList[g_strFpgaModule.ulOpcode] == NULL )
    {
        printf( "Opcode func is null.\r\n" );
        return SDKRTN_MONITOR_OPCODE_FUNC_ERROR;
    }

    return g_pafnFpgaCmdList[g_strFpgaModule.ulOpcode](  );
}

 /*******************************************************************************
 Function     : FPGA_MonitorExecuteCmd
 Description  : Display the vf information
 Input        : UINT32 ulSlotId, FpgaResourceMap *pstVfInfo
 Output       : None
 Return       : 0:sucess other:fail       
 *******************************************************************************/
 UINT32 FPGA_MonitorDisplayVfInfo( UINT32 ulSlotId, FpgaResourceMap *pstVfInfo )
{
    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo slot is out of range %d", ulSlotId );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstVfInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo pstVfInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( g_strFpgaModule.bShowInfo )
    {
        printf(" ----------------FPGA Information------------------\n");       
        printf("     Type\t\t\t%s\n","Fpga Device");
        printf("     Slot\t\t\t%u\n",ulSlotId);
        printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
        printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
        printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
        pstVfInfo->ucDev, pstVfInfo->ucFunc);
        printf(" --------------------------------------------------\n");
    }

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorDisplayDevice
Description  : Scan all the device and print the info to the stdout
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorDisplayDevice( void )
{
    INT32 i = 0;
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FpgaResourceMap strFpgaInfo[FPGA_SLOT_MAX]= { { 0 } };

    /* Scan all VF of this VM */
    ulRet = FPGA_PciScanAllSlots( strFpgaInfo, sizeof_array( strFpgaInfo ) );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots failed %d", ulRet );
        return ulRet;
    }

    for ( i = 0; i < ( int ) sizeof_array( strFpgaInfo ); i++ )
    {
        if ( 0 == strFpgaInfo[i].usVendorId )
        {
            continue;
        }

        /* Display vf information */
        ulRet = FPGA_MonitorDisplayVfInfo(i, &strFpgaInfo[i]);
        if ( SDKRTN_MONITOR_SUCCESS != ulRet )
        {
            LOG_ERROR( "Display VF Devices failed %d", ulRet );
            return ulRet;
        }
    }

    return ulRet;
}

/*******************************************************************************
 Function     : FPGA_MonitorDisplayImgInfo
 Description  : Print the inquired info
 Input        : UINT32 ulSlotId, FpgaResourceMap *pstVfInfo, FPGA_IMG_INFO *pstrImgInfo
 Output       : None
 Return       : 0:sucess other:fail   
 *******************************************************************************/
 UINT32 FPGA_MonitorDisplayImgInfo( UINT32 ulSlotId, FpgaResourceMap *pstVfInfo, FPGA_IMG_INFO *pstrImgInfo )
{
    UINT32 ulFpgaOpsStatus;
    UINT32 ulFpgaLoadErr;

    INT8 *pfpgaPrStatusList[FPGA_PR_STATUS_END] =
    {
        [FPGA_PR_STATUS_NOT_PROGRAMMED] = "NOT_PROGRAMMED",
        [FPGA_PR_STATUS_PROGRAMMED] = "PROGRAMMED",
        [FPGA_PR_STATUS_EXCEPTION] = "EXCEPTION",
        [FPGA_PR_STATUS_PROGRAMMING] = "PROGRAMMING",
    };
    
    INT8 *pfpgaOpsStatusList[FPGA_OPS_STATUS_END] =
    {
        [FPGA_OPS_STATUS_INITIALIZED] = "INITIALIZED",
        [FPGA_OPS_STATUS_SUCCESS] = "SUCCESS",
        [FPGA_OPS_STATUS_FAILURE] = "FAILURE",
        [FPGA_OPS_STATUS_PROCESSING] = "PROCESSING",
    };

    INT8* pfpgaLoadErrNameList[FPGA_CLEAR_ERROR_END] = {NULL};
    
    pfpgaLoadErrNameList[FPGA_LOAD_OK] = "OK";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_LOCK_BUSY] = "GET_LOCK_BUSY";
    pfpgaLoadErrNameList[FPGA_LOAD_WRITE_DB_ERR] = "WRITE_DB_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_HOSTID_ERR] = "GET_HOSTID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_NOVA_CFG_ERR] = "GET_NOVA_CFG_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_TOKEN_MATCH_ERR] = "TOKEN_MATCH_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_SERVICEID_ERR] = "GET_SERVICEID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_NOVAAPI_ERR] = "GET_NOVAAPI_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_UUID_ERR] = "GET_UUID_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_INVALID_AEIID] = "INVALID_AEI_ID";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_IMAGEPARA_ERR] = "GET_IMAGEPARA_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_AEI_CHECK_ERR] = "AEI_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_GET_AEIFILE_ERR] = "GET_AEIFILE_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_WR_MAILBOX_ERR] = "WR_MAILBOX_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PROGRAM_PARA_ERR] = "PROGRAM_PARA_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PROGRAM_ICAP_ERR] = "PROGRAM_ICAP_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_DDR_CHECK_ERR] = "DDR_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_FPGA_DISABLE_ERR ] = "FPGA_DISABLE_ERR";
    pfpgaLoadErrNameList[FPGA_LOAD_PUSH_QUEUE_ERR ] = "PUSH_QUEUE_ERR";
    pfpgaLoadErrNameList[FPGA_OTHER_EXCEPTION_ERR ] = "FPGA_EXCEPTION_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_GET_LOCK_BUSY] = "CLEAR_GET_LOCK_BUSY";
    pfpgaLoadErrNameList[FPGA_CLEAR_WRITE_DB_ERR] = "CLEAR_WRITE_DB_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_GET_BLANK_FILE_ERR] = "CLEAR_GET_BLANK_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_WR_MAILBOX_ERR] = "CLEAR_WR_MAILBOX_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PROGRAM_PARA_ERR] = "CLEAR_PROGRAM_PARA_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PROGRAM_ICAP_ERR] = "CLEAR_PROGRAM_ICAP_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_DDR_CHECK_ERR] = "CLEAR_DDR_CHECK_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_FPGA_DISABLE_ERR] = "CLEAR_FPGA_DISABLE_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_NOT_SUPPORT_ERR] = "CLEAR_NOT_SUPPORT_ERR";
    pfpgaLoadErrNameList[FPGA_CLEAR_PUSH_QUEUE_ERR ] = "CLEAR_PUSH_QUEUE_ERR";

    if ( ulSlotId >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_MonitorDisplayVfInfo slot is out of range %d", ulSlotId );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstVfInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayImgInfo pstVfInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    if ( NULL == pstrImgInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayImgInfo pstrImgInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* This is the command operation status */
    ulFpgaOpsStatus = ((pstrImgInfo->ulCmdOpsStatus) & FPGA_OPS_STATUS_MASK) >> FPGA_OPS_STATUS_SHIFT;
    /* This is the command operation error code */
    ulFpgaLoadErr = (pstrImgInfo->ulCmdOpsStatus) & FPGA_LOAD_ERROR_MASK;

    if( pstrImgInfo->ulFpgaPrStatus >= FPGA_PR_STATUS_END )
    {
        LOG_ERROR( "FPGA PR status Code Out Of Range %d", pstrImgInfo->ulFpgaPrStatus );
        return SDKRTN_MONITOR_PR_STATUS_ERROR;
    }

     if( ulFpgaOpsStatus >= FPGA_OPS_STATUS_END )
    {
        LOG_ERROR( "FPGA Cmd Ops Status Code Out Of Range %d", pstrImgInfo->ulCmdOpsStatus);
        return SDKRTN_MONITOR_CMD_OPS_ERROR;
    }

    if((( ulFpgaLoadErr >=  FPGA_LOAD_ERROR_END ) && ( ulFpgaLoadErr <  FPGA_OTHER_EXCEPTION_ERR )) ||
        (( ulFpgaLoadErr >=  FPGA_OTHER_ERROR_END ) && ( ulFpgaLoadErr <  FPGA_CLEAR_GET_LOCK_BUSY )) ||
        ( ulFpgaLoadErr >= FPGA_CLEAR_ERROR_END ))
    {
        LOG_ERROR( "FPGA Cmd Ops Status Code Out Of Range %d", pstrImgInfo->ulCmdOpsStatus);
        return SDKRTN_MONITOR_LOAD_ERRNAME_ERROR;
    }
     
    printf(" -------------Image Information--------------------\n");
    printf("     Type\t\t\t%s\n","Fpga Device");
    printf("     Slot\t\t\t%u\n",ulSlotId);
    printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
    printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
    printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
        pstVfInfo->ucDev, pstVfInfo->ucFunc);
    printf("     AEI ID\t\t\t%s\n",pstrImgInfo->acHfid);
    printf("     Shell ID\t\t\t%08x\n", pstrImgInfo->ulShellID);
    printf("     FPGA PR status\t\t%s\n",pfpgaPrStatusList[pstrImgInfo->ulFpgaPrStatus]);     /* the current status of FPGA PR region */
    printf("     Load/ClearOpsStatus\t%s\n",pfpgaOpsStatusList[ulFpgaOpsStatus]);            /* the result status of Load/clear command operation */
    if(FPGA_OPS_STATUS_FAILURE == ulFpgaOpsStatus)
    {
        printf("     Load/ClearOpsError\t\t%s\n",pfpgaLoadErrNameList[ulFpgaLoadErr]);         /* the error reason of Load/clear command operation */    
    }
    printf(" --------------------------------------------------\n");    

    return SDKRTN_MONITOR_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_MonitorClearHfi
Description  : The entrance function of clearing image   
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorClearHfi(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    ulRet = FPGA_MgmtClearHfiImage( g_strFpgaModule.ulSlotIndex );
    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorLoadHfi
Description  : The entrance function of loading image   
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorLoadHfi(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    ulRet = FPGA_MgmtLoadHfiImage( g_strFpgaModule.ulSlotIndex, g_strFpgaModule.acHfiId );
    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorDisplayFpgaImageInfo
Description  : The entrance function of displaying image information 
Input        : UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorDisplayFpgaImageInfo( UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo )
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FpgaResourceMap strFpgaInfo[FPGA_SLOT_MAX]= { { 0 } };

    if ( NULL == pstrImgInfo )
    {
        LOG_ERROR( "FPGA_MonitorDisplayFpgaImageInfo pstrImgInfo is null" );
        return SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* Scan all VF of this VM */
    ulRet = FPGA_PciScanAllSlots( strFpgaInfo, sizeof_array( strFpgaInfo ) );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots failed %d", ulRet );
        return ulRet;
    }

    ulRet = FPGA_MonitorDisplayImgInfo(ulSlotIndex, &strFpgaInfo[ulSlotIndex], pstrImgInfo);
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "Display image info failed %d", ulRet );
        return ulRet;
    }

    return ulRet;
}

/*******************************************************************************
Function     : FPGA_MonitorInquireFpgaImageInfo
Description  : Inquire the information of fpga image
Input        : None
Output       : None
Return       : 0:sucess other:fail   
*******************************************************************************/
UINT32 FPGA_MonitorInquireFpgaImageInfo(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;
    FPGA_IMG_INFO pstrImgInfo = { 0 };

    ulRet = FPGA_MgmtInquireFpgaImageInfo( g_strFpgaModule.ulSlotIndex, &pstrImgInfo );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtInquireFpgaImageInfo failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }

    ulRet = FPGA_MonitorDisplayFpgaImageInfo( g_strFpgaModule.ulSlotIndex, &pstrImgInfo );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MonitorDisplayFpgaImageInfo failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }
    return ulRet;
}
/*******************************************************************************
Function     : FPGA_MonitorInquireLEDStatus
Description  : Inquire the status of virtual led
Input        : None
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_MonitorInquireLEDStatus(void)
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;

    ulRet = FPGA_MgmtInquireLEDStatus( g_strFpgaModule.ulSlotIndex );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MonitorInquireLEDStatus failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }

    return ulRet;

}
/*******************************************************************************
Function     : main
Description  : The entrance function of tool
Input        : INT32 argc, INT8 *argv[]
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
int main( INT32 argc, INT8 *argv[] )
{
    UINT32 ulRet = SDKRTN_MONITOR_ERROR_BASE;

    /* At least input 2 parameters  */
    if ( argc < FPGA_INPUT_PARAS_NUM_MIN )
    {
        printf( "[***TIPS***] Input parameter number should be 2 at least.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacCommandEntryHelp, sizeof_array( g_pacCommandEntryHelp ) );
        return ( INT32 )SDKRTN_MONITOR_INPUT_ERROR;
    }

    /* Initialize global variables */
    ulRet = FPGA_MonitorInitModule(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        printf( "Initialization failed.\r\n"  );
        return ( INT32 )ulRet;
    }

    /* Initialize libfpgamgmt */
    ulRet = FPGA_MgmtInit(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        return ( INT32 )ulRet;
    }

    /* Parse command */
    ulRet = FPGA_ParseCommand( argc, argv );
    if ( SDKRTN_PARSE_SUCCESS != ulRet )
    {
        printf( "Parse command failed.\r\n" );
        return ( INT32 )ulRet;
    }
    
    /* it is unnecessary to continue if commands like -V -h or -? are executed and the program will exit */
    if(g_ulparseParaFlag == QUIT_FLAG)
    {
        return ( INT32 )ulRet;
    }

    /* Eccute cmd */
    ulRet = FPGA_MonitorExecuteCmd(  );
    if ( SDKRTN_MONITOR_SUCCESS != ulRet )
    {
        printf( "Execute command failed.\r\n" );
        return ( INT32 )ulRet;
    }

    printf( "Command execution is complete.\r\n" );
    return ( INT32 )ulRet;

}

#ifdef    __cplusplus
}
#endif
