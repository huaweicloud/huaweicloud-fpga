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
#include "FPGA_Common.h"
#include "FPGA_CmdMonitorMain.h"
#include "FPGA_CmdProcess.h"
#include "FPGA_CmdLog.h"
#include "FPGA_CmdPci.h"
#include "FPGA_CmdParse.h"

#ifdef    __cplusplus
extern "C"{
#endif


FPGA_CMD_PARA   g_strFpgaModule = { 0 };

COMMAND_PROC_FUNC g_pafnFpgaCmdList[CMD_PARSE_END] =
{
    [CMD_HFI_LOAD] = FPGA_MonitorLoadHfi,
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
Function     : FPGA_MonitorProcInit
Description  : Initialize manage module 
Input        : None
Output       : None
Return       : None  
*******************************************************************************/
void FPGA_MonitorProcInit( void )
{

    if ( g_strFpgaModule.ulOpcode == CMD_RESOURSE_INQUIRE )
    {
        return ;
    }

    FPGA_MmgtMboxOptInit(  );

    return ;
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
        
        if ( HW_VF_VENDOR_ID == g_astrFpgaInfo[ulSlotId].usVendorId && 
        HW_VF_DEVICE_ID == g_astrFpgaInfo[ulSlotId].usDeviceId )
        {
            printf("     Type\t\t\t%s\n","Fpga Device");
            printf("     Slot\t\t\t%u\n",ulSlotId);
            printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
            printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
            printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
            pstVfInfo->ucDev, pstVfInfo->ucFunc);
            printf(" --------------------------------------------------\n");            
        }
        else if ( HW_OCL_PF_VENDOR_ID == g_astrFpgaInfo[ulSlotId].usVendorId && 
        HW_OCL_PF_DEVICE_ID == g_astrFpgaInfo[ulSlotId].usDeviceId )
        {
            printf("     Type\t\t\t%s\n","Fpga Device");
            printf("     Slot\t\t\t%u\n",ulSlotId);
            printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
            printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
            printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
            pstVfInfo->ucDev, pstVfInfo->ucFunc);
            printf(" --------------------------------------------------\n");            
        }
        else
        {
            printf("FPGA shell Type error.\r\n");
            return SDKRTN_MONITOR_SHELL_TYPE_ERROR;                
        }
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
    if ( SDKRTN_PCI_SUCCESS != ulRet )
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
            LOG_ERROR( "Display vfs failed %d", ulRet );
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
    INT8 *pfpgaLoadStatusList[FPGA_LOAD_STATUS_END] =
    {
        [FPGA_LOAD_STATUS_NOT_PROGRAMMED] = "NOT_PROGRAMMED",
        [FPGA_LOAD_STATUS_LOADED] = "LOADED",
        [FPGA_LOAD_STATUS_LOAD_FAILED] = "FAILED",
        [FPGA_LOAD_STATUS_BUSY] = "BUSY",
        [FPGA_LOAD_STATUS_INVALID_ID] = "INVALID_ID",
    };

    INT8* pfpgaLoadErrNameList[FPGA_STATUS_END] =
    {
        [FPGA_STATUS_OK] = "OK",
        [FPGA_STATUS_COMPETENCE_ERR] = "COMPETENCE_ERR",
        [FPGA_STATUS_GETSERVICEID_ERR] = "GETSERVICEID_ERR",
        [FPGA_STATUS_GETPRJECTID_ERR] = "GETPRJECTID_ERR",
        [FPGA_STATUS_GETNOVAINFO_ERR] = "GETNOVAINFO_ERR",
        [FPGA_STATUS_GETVMUUID_ERR] = "GETVMUUID_ERR",
        [FPGA_STATUS_GETOBSINFO_ERR] = "GETOBSINFO_ERR",
        [FPGA_STATUS_GETFILE_ERR] = "GETFILE_ERR",
        [FPGA_STATUS_PARA_ERR] = "PARA_ERR",
        [FPGA_STATUS_LOAD_BUSY] = "LOAD_BUSY",
        [FPGA_STATUS_INNER_ERR] = "INNER_ERR",
        [FPGA_STATUS_CLEARD] = "CLEARD",
        [FPGA_STATUS_LOADING] = "LOADING",
        [FPGA_STATUS_MBOX_ERR] = "MBOX_ERR",
        [FPGA_STATUS_LOCK_ERR] = "OTHERSIDE_ERR",
        [FPGA_STATUS_GETNOVACFG_ERR] = "GETCFG_ERR",
        [FPGA_STATUS_AEI_MATCH_ERR] = "AEI_MATCH_ERR ",
        [FPGA_STATUS_FILE_ERR] = "AEI_FILE_ERR ",        
    };

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

    if( pstrImgInfo->ulHfiLoadStatus >= FPGA_LOAD_STATUS_END )
    {
        LOG_ERROR( "Load Status Code Out Of Range %d", pstrImgInfo->ulHfiLoadStatus );
        return SDKRTN_MONITOR_STATUS_NAME_ERROR;
    }

     if( pstrImgInfo->ulHfiLoadErr >= FPGA_STATUS_END )
    {
        LOG_ERROR( "Error Status Code Out Of Range %d", pstrImgInfo->ulHfiLoadErr);
        return SDKRTN_MONITOR_ERR_NAME_ERROR;
    }
     
    /* Use the Device ID and Vendor ID to distinguish different display information */
    if ( HW_VF_VENDOR_ID == g_astrFpgaInfo[ulSlotId].usVendorId && 
        HW_VF_DEVICE_ID == g_astrFpgaInfo[ulSlotId].usDeviceId )
    {
        printf(" -------------Image Information--------------------\n");
        printf("     Type\t\t\t%s\n","Fpga Device");
        printf("     Slot\t\t\t%u\n",ulSlotId);
        printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
        printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
        printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
            pstVfInfo->ucDev, pstVfInfo->ucFunc);
        printf("     ImageId\t\t\t%s\n",pstrImgInfo->acHfid);
        printf("     LoadStatusName\t\t%s\n",pfpgaLoadStatusList[pstrImgInfo->ulHfiLoadStatus]);
        printf("     LoadStatusCode\t\t%x\n",pstrImgInfo->ulHfiLoadStatus);
        printf("     LoadErrName\t\t%s\n",pfpgaLoadErrNameList[pstrImgInfo->ulHfiLoadErr]);
        printf("     LoadErrCode\t\t%u\n",pstrImgInfo->ulHfiLoadErr);
        
        printf("     Shell ID\t\t\t%08x\n", pstrImgInfo->ulShVer);
        printf(" --------------------------------------------------\n");    
    }
    else if ( HW_OCL_PF_VENDOR_ID == g_astrFpgaInfo[ulSlotId].usVendorId && 
        HW_OCL_PF_DEVICE_ID == g_astrFpgaInfo[ulSlotId].usDeviceId )
    {
        printf(" -------------Image Information--------------------\n");
        printf("     Type\t\t\t%s\n","Fpga Device");
        printf("     Slot\t\t\t%u\n",ulSlotId);
        printf("     VendorId\t\t\t0x%04x\n",pstVfInfo->usVendorId);
        printf("     DeviceId\t\t\t0x%04x\n",pstVfInfo->usDeviceId);
        printf("     DBDF\t\t\t%04x:%02x:%02x.%d\n",pstVfInfo->usDomain, pstVfInfo->ucBus,
            pstVfInfo->ucDev, pstVfInfo->ucFunc);
        printf("     ImageId\t\t\t%s\n",pstrImgInfo->acHfid);
        printf("     LoadStatusName\t\t%s\n",pfpgaLoadStatusList[pstrImgInfo->ulHfiLoadStatus]);
        printf("     LoadStatusCode\t\t%x\n",pstrImgInfo->ulHfiLoadStatus);
        printf("     LoadErrName\t\t%s\n",pfpgaLoadErrNameList[pstrImgInfo->ulHfiLoadErr]);
        printf("     LoadErrCode\t\t%u\n",pstrImgInfo->ulHfiLoadErr);
        
        printf("     Shell ID\t\t\t%08x\n", pstrImgInfo->ulShVer);
        printf(" --------------------------------------------------\n");    
    }
    else
    {
        /* Report type ID error after the device ID match fails */
        printf("FPGA shell Type error.\r\n");
        return SDKRTN_MONITOR_SHELL_TYPE_ERROR;                
    }

    return SDKRTN_MONITOR_SUCCESS;
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
    if ( SDKRTN_PROCESS_SUCCESS != ulRet )
    {
        LOG_ERROR( "FPGA_MgmtInquireFpgaImageInfo failed ulRet = 0x%x\r\n", ulRet );
        return ulRet;
    }

    ulRet = FPGA_MonitorDisplayFpgaImageInfo( g_strFpgaModule.ulSlotIndex, &pstrImgInfo );
    if ( SDKRTN_PROCESS_SUCCESS != ulRet )
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
    if ( SDKRTN_PROCESS_SUCCESS != ulRet )
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
    if ( argc < INPUT_PARAS_NUM_MIN )
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

    /* Initialize log */
    ulRet = FPGA_LogInit(  );
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
    
    /* Initialize manage module */
    FPGA_MonitorProcInit(  );

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
