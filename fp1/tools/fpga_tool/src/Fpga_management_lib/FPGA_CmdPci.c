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
#include "FPGA_CmdPci.h"
#include "FPGA_CmdLog.h"

#ifdef    __cplusplus
extern "C"{
#endif

FPGA_PCI_BAR_INFO g_strBars[FPGA_VF_BAR_NUM_MAX] = { { 0 } };
FpgaShellType g_astrShellType = { 0 };

/*******************************************************************************
Function     : FPGA_PciGetDBDF
Description  : Get the BDF of fpga
Input        : INT8 *pcDirName 
Output       : FpgaResourceMap *pstMap
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_PciGetDBDF( INT8 *pcDirName, FpgaResourceMap *pstMap )
{
    UINT32 ulDomain = 0;
    UINT32 ulBus = 0;
    UINT32 ulDev = 0;
    INT32 lFunc = 0;
    INT32 lRet = ( INT32 )SDKRTN_PCI_ERROR_BASE;
    
    if ( NULL == pcDirName )
    {
        LOG_ERROR( "FPGA_PciGetDBDF pcDirName is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pstMap )
    {
        LOG_ERROR( "FPGA_PciGetDBDF pstMap is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Read data of the formatted string */
    lRet = sscanf_s( pcDirName, PCI_DEV_FMT, &ulDomain, &ulBus, &ulDev, &lFunc );
    if ( lRet != 4 )
    {
        LOG_ERROR( "FPGA_PciGetDBDF sscanf_s failed %d", lRet );
        return SDKRTN_PCI_SSCANF_ERROR;
    }

    pstMap->usDomain = ( UINT16 )ulDomain;
    pstMap->ucBus = ( UINT8 )ulBus;
    pstMap->ucDev = ( UINT8 )ulDev;
    pstMap->ucFunc = ( UINT8 )lFunc;

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciGetDBDF
Description  : Get the ID of fpga
Input        : INT8 *pcDirName 
Output       : UINT16 *pusId
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_PciGetId( INT8 *pcPath, UINT16 *pusId )
{
    INT32 lRet = ( INT32 )SDKRTN_PCI_ERROR_BASE;
    FILE *pstrFp = NULL;
    UINT32 ulTmpId = INIT_VALUE;

    if ( NULL == pcPath )
    {
        LOG_ERROR( "FPGA_PciGetId cpPath is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pusId )
    {
        LOG_ERROR( "FPGA_PciGetId uspId is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }
    
    /* Open file */
    pstrFp = fopen( pcPath, "r" );
    if ( NULL == pstrFp )
    {
        LOG_DEBUG( "FPGA_PciGetId fopen failed %s", pcPath );
        return SDKRTN_PCI_FOPEN_ERROR;
    }
    
    /* Fetches the contents of the file and formats the contents in hexadecimal format */
    lRet = fscanf_s( pstrFp, "%x", &ulTmpId );
    if ( lRet < 0 )
    {
        LOG_ERROR( "FPGA_PciGetId fscanf_s failed %d", lRet );
        fclose(pstrFp);
        return SDKRTN_PCI_FSCANF_ERROR;
    }

    *pusId = ( UINT16 )ulTmpId;

    fclose(pstrFp);
    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciGetVfResourceInfo
Description  : Get bar size 
Input        : INT8 *pcDirName, UINT8 ucResourceNum
Output       : UINT64 *pullResourceSize
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciGetVfResourceInfo( INT8 *pcDirName,
                UINT8 ucResourceNum, UINT64 *pullResourceSize )
{
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;
    INT8 acSysFsName[NAME_MAX + 1] = { 0 };
    struct stat strFile_stat = { 0 };

    if ( NULL == pcDirName )
    {
        LOG_ERROR( "pcDirName is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pullResourceSize )
    {
        LOG_ERROR( "pullResourceSize is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( ucResourceNum >= FPGA_VF_BAR_NUM_MAX )
    {
        LOG_ERROR( "ucResourceNum is out of range %d", ucResourceNum );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Construct the file path of resourcex */
    ulRet = snprintf_s( acSysFsName, ( NAME_MAX + 1 ), ( sizeof( acSysFsName ) - 1 ),
        PCI_DEV_RESOURCE_PATH, pcDirName, ucResourceNum );
    if ( ( size_t )ulRet >= ( sizeof( acSysFsName ) - 1 ) )
    {
        LOG_ERROR( "Resource%u path too long %d.", ucResourceNum, ulRet );
        return SDKRTN_PCI_SNPRINTF_ERROR;
    }

    /* Check if the file exists and read the file size if it exists */
    ulRet = stat( acSysFsName, &strFile_stat );
    if ( OK != ulRet )
    {
        LOG_ERROR( "stat failed, path=%s", acSysFsName );
        return SDKRTN_PCI_STAT_ERROR;
    }

    *pullResourceSize = strFile_stat.st_size;

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciFindVfResources
Description  : Get all bar zone size
Input        : INT8 *pcDirName
Output       : FpgaResourceMap *pstrMap
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciFindVfResources( INT8 *pcDirName, FpgaResourceMap *pstrMap )
{
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;
    INT8 acResourceNums[FPGA_VF_BAR_NUM_MAX] = { 0 };
    INT32 i = 0;
    /* bool bBurstable = false;*/
    UINT64 ullRsourceSize = 0;

    if ( NULL == pstrMap )
    {
        LOG_ERROR( "FPGA_PciFindVfResources pstrMap is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pcDirName )
    {
        LOG_ERROR( "FPGA_PciFindVfResources pcDirName is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Get bar size */
    for ( i = 0; i < sizeof_array( acResourceNums ); i++ )
    {
        ulRet = FPGA_PciGetVfResourceInfo( pcDirName, acResourceNums[i],
                                     &ullRsourceSize );
        if ( ulRet )
        {
            LOG_DEBUG( "Unable to read resource information for %d", acResourceNums[i] );
        }
        pstrMap->aullBarSize[( INT32 )acResourceNums[i]] = ( UINT64 )ullRsourceSize;
        
        /* This feature is not currently enabled because hardware problem */
        /* pstrMap->bResourceBurstable[( INT32 )acResourceNums[i]] = ( bool )bBurstable; */
        /* bBurstable = false; */
        ullRsourceSize = 0;
    }
    
    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciFindVfByPciDirName
Description  : Find out the verdor ID, Device ID and DBDF according to the path
Input        : INT8 *pcDirName
Output       : FpgaResourceMap *pstrMap
Return       : 0:sucess other:fail
*******************************************************************************/
INT32 FPGA_PciFindVfByPciDirName( INT8 *pcDirName, FpgaResourceMap *pstFpgaMap )
{
    UINT16 usVendorId = 0;
    UINT16 usDeviceId = 0;
    INT8 acSysFsName[NAME_MAX + 1] = { 0 };
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;

    if ( NULL == pcDirName )
    {
        LOG_ERROR( "FPGA_PciFindVfByPciDirName pcDirName is null" );
        return ( INT32 )SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pstFpgaMap )
    {
        LOG_ERROR( "FPGA_PciFindVfByPciDirName pstFpgaMap is null" );
        return ( INT32 )SDKRTN_PCI_INPUT_ERROR;
    }

    /* Construct the path of vendor ID */
    ulRet = snprintf_s( acSysFsName, sizeof(acSysFsName), ( sizeof( acSysFsName ) - 1 ),
        PCI_DEV_VENDOR_PATH, pcDirName );
    if ( (size_t)ulRet >= ( sizeof(acSysFsName) - 1 ) )
    {
        LOG_ERROR( "Vendor path too long %d.", ulRet );
        return ( INT32 )SDKRTN_PCI_SNPRINTF_ERROR;
    }

    /* Get vendor ID */
    ulRet = FPGA_PciGetId(acSysFsName, &usVendorId );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Error %d retrieving vendor_id", ulRet );
        return ( INT32 )ulRet;
    }

    /* Construct the path of device ID */
    ulRet = snprintf_s( acSysFsName, sizeof( acSysFsName ), ( sizeof( acSysFsName ) - 1 ),
        PCI_DEV_DEVICE_PATH, pcDirName );
    if ( ulRet >= ( sizeof(acSysFsName) - 1 ) )
    {
        LOG_ERROR( "Device path too long %d.", ulRet );
        return ( INT32 )SDKRTN_PCI_SNPRINTF_ERROR;
    }

    /* Get device ID */
    ulRet = FPGA_PciGetId(acSysFsName, ( UINT16 * )&usDeviceId);
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Error %d retrieving device_id", ulRet );
        return ( INT32 )ulRet;
    }

    /* Get DBDF */
    ulRet = FPGA_PciGetDBDF( pcDirName, pstFpgaMap );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Error %d retrieving DBDF from dir_name=%s", ulRet, pcDirName );
        return ( INT32 )ulRet;
    }

    pstFpgaMap->usVendorId = ( UINT16 )usVendorId ;
    pstFpgaMap->usDeviceId = ( UINT16 )usDeviceId;

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciScanAllSlots
Description  : scan the fpga resource
Input        : FpgaResourceMap straFpgaArray[], UINT32 ulSize
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciScanAllSlots( FpgaResourceMap straFpgaArray[], UINT32 ulSize )
{
    bool bFpgaGet = false;
    const INT8 *pcPath = PCI_DEVICES_PATH;
    DIR *pstrDirp = NULL;
    struct dirent strEntry = { 0 };
    struct dirent *pstrResult = NULL;
    UINT32 ulSlotIndex = 0;
    FpgaResourceMap strFpgaMapGet = { 0 };
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;

    if ( NULL == straFpgaArray )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots straFpgaArray is null" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( ulSize > FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots ulSize is too big %d ulSize", ulSize );
        return SDKRTN_PCI_INPUT_ERROR;
    }
    
    pstrDirp = opendir( pcPath );
    if ( NULL == pstrDirp )
    {
        LOG_ERROR( "FPGA_PciScanAllSlots open direct failed" );
        return SDKRTN_PCI_OPENDIR_ERROR;
    }

    //lint -e716
    while ( true )
    {
        ulRet = memset_s( &strEntry, sizeof( struct dirent ), 0, sizeof( struct dirent ) );
        if ( ulRet != OK )
        {
            LOG_DEBUG( "FPGA_PciScanAllSlots memset_s failed, ulRet = 0x%x", ulRet );
        }
        
        /* Read the directory from the directory stream to strEntry */
        ulRet = readdir_r( pstrDirp, &strEntry, &pstrResult );
        if ( OK != ulRet )
        {
            LOG_ERROR( "FPGA_PciScanAllSlots readdir_r failed, ulRet = 0x%x", ulRet );
            closedir( pstrDirp );
            return SDKRTN_PCI_READDIR_ERROR;
        }
        
        /* End of the directory */
        if ( pstrResult == NULL )
        {
            break;
        }

        /* Scan the directory that matches the length condition */
        if ( DIR_NAME_MAX == strnlen( strEntry.d_name, ( DIR_NAME_MAX + 1 ) ) )
        {

            ulRet = memset_s( &strFpgaMapGet, sizeof( FpgaResourceMap ), 0, sizeof( FpgaResourceMap ) );
            if ( ulRet != OK )
            {
                LOG_DEBUG( "FPGA_PciScanAllSlots memset_s failed, ulRet = 0x%x", ulRet );
            }

            /* Go to the directory to get information about the PCI */
            ulRet = FPGA_PciFindVfByPciDirName( strEntry.d_name, &strFpgaMapGet );
            if ( ulRet != SDKRTN_PCI_SUCCESS )
            {
                /* Continue to query the next directory after the failure */
                continue;
            }

            /* Use the Device ID and Vendor ID to distinguish different shell type */
            if ( ( HW_VF_VENDOR_ID == strFpgaMapGet.usVendorId && HW_VF_DEVICE_ID == strFpgaMapGet.usDeviceId )
                 || ( HW_OCL_PF_VENDOR_ID == strFpgaMapGet.usVendorId && HW_OCL_PF_DEVICE_ID == strFpgaMapGet.usDeviceId ) )
            {                
                /* Get bar size */
                ulRet = FPGA_PciFindVfResources( strEntry.d_name, &strFpgaMapGet );
                if ( SDKRTN_PCI_SUCCESS != ulRet )
                {
                    closedir( pstrDirp );
                    LOG_ERROR( "Error retrieving resource information" );
                    return ulRet;
                }

                straFpgaArray[ulSlotIndex] = strFpgaMapGet;

                bFpgaGet = true;
                ulSlotIndex += 1;

                /* Found fpga */
                if ( ulSlotIndex >= ulSize )
                {
                    break;
                }
            }
        }
    }

    if ( true != bFpgaGet )
    {
        printf( "There is no FPGA Resource.\r\n" );
        closedir(pstrDirp);
        return SDKRTN_PCI_NONE_FPGA_ERROR;
    }

    closedir( pstrDirp );

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciGetMapInfo
Description  : Get the FPGA information of specify slot 
Input        : UINT32 ulSlotIndex
Output       : FpgaResourceMap *pstrMapSpec
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciGetMapInfo( UINT32 ulSlotIndex, FpgaResourceMap *pstrMapSpec )
{
    UINT32 ulSize = 0;
    FpgaResourceMap astrMapArray[FPGA_SLOT_MAX] = { { 0 } };
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;

    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_PciGetMapInfo invalid slot_id=%d", ulSlotIndex );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pstrMapSpec )
    {
        LOG_ERROR( "FPGA_PciGetMapInfo pstrMapSpec is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* The quantity of fpga */
    ulSize = ulSlotIndex + 1;

    ulRet = FPGA_PciScanAllSlots( astrMapArray, ulSize );
    if (OK != ulRet)
    {
        LOG_ERROR("Scan pci device failed %d.", ulRet);
        return ulRet;
    }

    if (0 == astrMapArray[ulSlotIndex].usVendorId )
    {
        LOG_ERROR("The pci vendor id of slot(%d) is null.", ulSlotIndex);
        printf("[***TIPS***]There is no fpga device in slot(%u).\r\n", ulSlotIndex);        
        return SDKRTN_PCI_VENDOR_ID_ERROR;
    }

    *pstrMapSpec = astrMapArray[ulSlotIndex];
    g_astrShellType.usVendorId = astrMapArray[ulSlotIndex].usVendorId;
    g_astrShellType.usDeviceId = astrMapArray[ulSlotIndex].usDeviceId;
    
    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciAllocBar
Description  : Alloc bar 
Input        : None
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciAllocBar( void )
{
    INT32 i = 0;
    FPGA_PCI_BAR_INFO *pstrbar = NULL;

    /* 分配未占用的BAR */
    for (i = 0; i < FPGA_VF_BAR_NUM_MAX; i++)
    {
        pstrbar = &g_strBars[i];

        if ( false == pstrbar->bAllocatedFlag )
        {
            pstrbar->bAllocatedFlag = true;

            return ( UINT32 )i;
        }
    }
    return SDKRTN_PCI_ALLOC_BAR_ERROR;
}

/*******************************************************************************
Function     : FPGA_PciGetBar
Description  : Return the address of bar 
Input        : UINT32 ulBarHandle
Output       : None
Return       : The address of bar
*******************************************************************************/
FPGA_PCI_BAR_INFO *FPGA_PciGetBar( UINT32 ulBarHandle )
{
    if ( ulBarHandle >= FPGA_VF_BAR_NUM_MAX )
    {
        LOG_ERROR( "Invalid handle=%d",  ulBarHandle );
        return NULL;
    }

    return &g_strBars[ulBarHandle];
}

/*******************************************************************************
Function     : FPGA_PciSetBarSpace
Description  : Set bar space 
Input        : UINT32 ulBarHandle, void *pMemBase, UINT64 ullMemSize
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciSetBarSpace( UINT32 ulBarHandle, void *pMemBase, UINT64 ullMemSize )
{
    FPGA_PCI_BAR_INFO *pstrBar = NULL;

    if ( ulBarHandle >= FPGA_VF_BAR_NUM_MAX )
    {
        LOG_ERROR( "FPGA_PciSetBarSpace invalid handle=%d",  ulBarHandle );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pMemBase )
    {
        LOG_ERROR( "FPGA_PciSetBarSpace memBase is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( 0 == ullMemSize )
    {
        LOG_ERROR( "FPGA_PciSetBarSpace ullMemSize is 0" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    pstrBar = FPGA_PciGetBar( ulBarHandle );
    if ( NULL == pstrBar )
    {
        LOG_ERROR( "pstrBar is NULL" );
        return SDKRTN_PCI_GET_BAR_ERROR;
    }

    pstrBar->pMemBase = pMemBase;
    pstrBar->ullMemSize= ullMemSize;

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciGetBarHandle
Description  : Get bar handle
Input        : UINT32 ulBarNum, UINT32 *pulBarHandle
Output       : FpgaResourceMap *pstrMapSpec
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciGetBarHandle( FpgaResourceMap *pstrMapSpec, UINT32 ulBarNum, UINT32 *pulBarHandle )
{
    void *pMemBase = NULL;
    FpgaResourceMap *pstrFpgaMap = NULL;
    INT8 acSysFsName[NAME_MAX + 1] = { 0 };
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;
    UINT32 ulRetTemp = SDKRTN_PCI_SUCCESS;
    INT32 lFd = ERROR;
    UINT32 ulTmpHandle = INIT_VALUE;

    if ( NULL == pstrMapSpec )
    {
         LOG_ERROR( "FPGA_PciGetBarHandle pstrMapSpec is NULL" );
         return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( ulBarNum >= FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_PciGetBarHandle Invalid lBarNum=%d", ulBarNum );
         return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pulBarHandle )
    {
         LOG_ERROR( "FPGA_PciGetBarHandle plBarHandle is NULL" );
         return SDKRTN_PCI_INPUT_ERROR;
    }
    pstrFpgaMap = pstrMapSpec;

    *pulBarHandle = INIT_VALUE;

    /* Construct the resource path */
    ulRet = snprintf_s(acSysFsName, ( NAME_MAX + 1 ), ( sizeof( acSysFsName ) - 1 ),
        PCI_DEVICES_PATH PCI_DEV_FMT "/resource%u",
    pstrFpgaMap->usDomain, pstrFpgaMap->ucBus, pstrFpgaMap->ucDev, pstrFpgaMap->ucFunc, ulBarNum);
    if ( ulRet >= ( sizeof( acSysFsName ) - 1 ) )
    {
        LOG_ERROR( "Resource%u path too long %d.", ulBarNum, ulRet );
        return SDKRTN_PCI_SNPRINTF_ERROR;
    }

    /* Open resource file */
    lFd = open( acSysFsName, O_RDWR | O_SYNC );
    if ( ERROR == lFd )
    {
        LOG_ERROR( "Open failed." );
        printf( "Try to execute cmd with 'sudo' or use 'root' account.\r\n" );
        return SDKRTN_PCI_OPEN_ERROR;
    }

    pMemBase = mmap(0, ( UINT32 )pstrFpgaMap->aullBarSize[ulBarNum], PROT_READ | PROT_WRITE,
        MAP_SHARED, lFd, ( off_t )0);
    if ( MAP_FAILED == pMemBase )
    {
        LOG_ERROR( "FPGA_PciGetBarHandle Mmap failed." );
        close( lFd );
        return SDKRTN_PCI_MMAP_ERROR;
    }

    ulTmpHandle = FPGA_PciAllocBar();
    if ( ( INT32 )ulTmpHandle < 0 )
    {
        LOG_ERROR( "Bar space alloc failed." );
        ulRetTemp = munmap( pMemBase, ( UINT32 )pstrFpgaMap->aullBarSize[ulBarNum] );
        if ( OK != ulRetTemp )
        {
            LOG_ERROR( "munmap failed" );
            close( lFd );
            return SDKRTN_PCI_MUNMAP_ERROR;
        }
        close( lFd );
        return ulTmpHandle;
    }

    /* Set base address and size of the bar */
    ulRet = FPGA_PciSetBarSpace( ulTmpHandle, pMemBase, pstrFpgaMap->aullBarSize[ulBarNum] );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Bar space set failed %d.", ulRet );
        ulRetTemp = munmap( pMemBase, ( UINT32 )pstrFpgaMap->aullBarSize[ulBarNum] );
        if ( OK != ulRetTemp )
        {
            LOG_ERROR( "munmap failed" );
            close( lFd );
            return SDKRTN_PCI_MUNMAP_ERROR;
        }
        close( lFd );
        return ulRet;
    }

    *pulBarHandle = ulTmpHandle;

    close( lFd );
    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciEnableSlotsBar
Description  : Enable the bar
Input        : UINT32 ulSlotIndex, UINT32 ulBarNum
Output       : UINT32 *pulBarHandle
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciEnableSlotsBar( UINT32 ulSlotIndex, UINT32 ulBarNum, UINT32 *pulBarHandle )
{
    FpgaResourceMap strFpgaMap = { 0 };
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;

    if ( NULL == pulBarHandle )
    {
         LOG_ERROR( "FPGA_PciEnableSlotsBar pstMsg is NULL" );
         return SDKRTN_PCI_INPUT_ERROR;
    }
    
    if ( ulBarNum >=  FPGA_VF_BAR_NUM_MAX )
    {
         LOG_ERROR( "FPGA_PciEnableSlotsBar invalid lBarNum=%d", ulBarNum );
         return SDKRTN_PCI_INPUT_ERROR;
    }
    
    if ( ulSlotIndex >= FPGA_SLOT_MAX )
    {
        LOG_ERROR( "FPGA_PciEnableSlotsBar invalid slot_id=%d", ulSlotIndex );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    ulRet = memset_s( g_strBars, sizeof( g_strBars ), 0, sizeof( g_strBars ) );
    if ( OK != ulRet )
    {
        LOG_DEBUG( "FPGA_PciEnableSlotsBar memset_s failed, ulRet = 0x%x", ulRet );
        return SDKRTN_PCI_MEMSET_ERROR;
    }

    /* Get pci info of specify slot */
    ulRet = FPGA_PciGetMapInfo( ulSlotIndex, &strFpgaMap );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Get pci map info failed =%d", ulRet );
        return ulRet;
    }
    
    ulRet = FPGA_PciGetBarHandle(&strFpgaMap, ulBarNum, pulBarHandle);
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Get bar handle failed =%d", ulRet );
        return ulRet;
    }

    return SDKRTN_PCI_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_PciJointDBDF
Description  : Joint the DBDF
Input        : UINT32 ulSlot
Output       : INT8 *pcDbdf
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciJointDBDF( INT8 *pcDbdf, FpgaResourceMap *pstMap )
{
    INT32 lLen = -1;
    INT32 lRet = -1;
    INT8 acDBDF[DBDF_LEN]= {0};
    
    if ( NULL == pcDbdf )
    {
        LOG_ERROR( "FPGA_PciGetDBDF pcDirName is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pstMap )
    {
        LOG_ERROR( "FPGA_PciGetDBDF pstMap is NULL" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Read data of the formatted string */
    lLen = snprintf_s( acDBDF, sizeof(acDBDF), sizeof(acDBDF)-1, PCI_DEV_FMT, pstMap->usDomain, pstMap->ucBus, pstMap->ucDev, pstMap->ucFunc );
    if ( ( 0 == lLen ) || ( ( size_t ) lLen >= sizeof( acDBDF ) ) )
    {
        LOG_ERROR( "FPGA_PciGetDBDF snprintf_s failed %d", lLen );
        return SDKRTN_PCI_SSCANF_ERROR;
    }

    lRet = strncpy_s(pcDbdf, DBDF_LEN, acDBDF, DIR_NAME_MAX);
    if ( EOK != lRet )
    {
         LOG_ERROR( "FPGA_PciGetDBDF strncpy failed %d", lLen );
         return SDKRTN_PCI_STRNCPY_ERROR;
    }
    pcDbdf[DIR_NAME_MAX] = '\0';

    return SDKRTN_PCI_SUCCESS;
}


/*******************************************************************************
Function     : FPGA_PciGetBdfBySlot
Description  : Get dbdf by slot
Input        : UINT32 ulSlot
Output       : INT8 *pcDbdf
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciGetBdfBySlot( UINT32 ulSlot, INT8 *pcDbdf )
{
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;
    FpgaResourceMap astrFpgaArray[FPGA_SLOT_MAX] = {{ 0 }};
    
    if ( FPGA_SLOT_MAX <= ulSlot )
    {
        LOG_ERROR( "Input slot[%d] is overrange", ulSlot );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    if ( NULL == pcDbdf )
    {
        LOG_ERROR( "Input bdf is null" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Rescan all fpga devices */
    ulRet = FPGA_PciScanAllSlots( astrFpgaArray, FPGA_SLOT_MAX - 1 );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Scan all slot failed, err code %x", ulRet);
        return ulRet;
    }

    /* Get dbdf */
    ulRet = FPGA_PciJointDBDF( pcDbdf, &astrFpgaArray[ulSlot] );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Get BDF failed, err code %x", ulRet);
        return ulRet;
    }

    return SDKRTN_PCI_SUCCESS;    
}

/*******************************************************************************
Function     : FPGA_PciGetSlotByBdf
Description  : Get Slot by Bdf
Input        : INT8 *pcDbdf
Output       : UINT32 *pulSlot
Return       : 0:sucess other:fail
*******************************************************************************/
UINT32 FPGA_PciGetSlotByBdf( INT8 *pcDbdf, UINT32 *pulSlot )
{
    UINT32 ulRet = SDKRTN_PCI_ERROR_BASE;
    UINT32 i = 0;
    INT8 acTempBDF[DBDF_LEN]= {0};
    
    FpgaResourceMap astrFpgaArray[FPGA_SLOT_MAX] = {{ 0 }};

    if ( ( NULL == pcDbdf ) ||  ( NULL == pulSlot ) )
    {
        LOG_ERROR( "Input pointer is null" );
        return SDKRTN_PCI_INPUT_ERROR;
    }

    /* Rescan all fpga devices */
    ulRet = FPGA_PciScanAllSlots( astrFpgaArray, FPGA_SLOT_MAX - 1 );
    if ( SDKRTN_PCI_SUCCESS != ulRet )
    {
        LOG_ERROR( "Scan all slot failed, err code %x", ulRet);
        return ulRet;
    }

    /* Get the slot */
    for ( i = 0; i < FPGA_SLOT_MAX; i++ )
    {
        ulRet = FPGA_PciJointDBDF( acTempBDF, &astrFpgaArray[i] );
        if ( SDKRTN_PCI_SUCCESS != ulRet )
        {
            LOG_ERROR( "Get BDF failed, err code %x", ulRet);
            continue;
        }
        
        if ( 0 == strncmp( pcDbdf, acTempBDF, DIR_NAME_MAX ) )
        {
            *pulSlot = i;
            return SDKRTN_PCI_SUCCESS;
        }
    }

    LOG_ERROR( "Did not found the slot of BDF:%s", pcDbdf );   
    return SDKRTN_PCI_GET_SLOT_ERROR;
}

#ifdef    __cplusplus
}
#endif
