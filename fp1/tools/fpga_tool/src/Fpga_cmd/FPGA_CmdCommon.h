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

#ifndef __FPGA_COMMOM_H__
#define __FPGA_COMMOM_H__

#include <stdbool.h>

typedef char  INT8;
typedef short INT16;
typedef int   INT32;

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;

#define OK                                      0
#define ERROR                                   -1
#define INIT_VALUE                              -1
#define SDKRTN_ERR_BASE                         0xFFFFF000

#define FPGA_SLOT_MAX                           8
#define HFI_ID_LEN_MAX                          36
#define HFI_ID_LEN                              32
#define FPGA_VF_BAR_NUM_MAX                     1

#define HW_VF_VENDOR_ID                         0x19e5
#define HW_VF_DEVICE_ID                         0xd503
#define HW_OCL_PF_VENDOR_ID                     0x19e5
#define HW_OCL_PF_DEVICE_ID                     0xd512
#define HW_MUTEX_PATH                     "/tmp/fpga%d.lock"

#define sizeof_array( arr ) \
    ( sizeof( arr ) / sizeof( arr[0] ) )
    
#define LOG_ERROR(...)     FPGA_LogErr( __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...)   FPGA_LogWarning( __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)      FPGA_LogInfo(__LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)     FPGA_LogDebug(__LINE__, __func__, __VA_ARGS__)

typedef struct tagFpgaShellType
{
    UINT16 usVendorId;
    UINT16 usDeviceId;
}FpgaShellType;

typedef struct tagFpgaResourceMap
{
    UINT16 usVendorId;
    UINT16 usDeviceId;
    UINT16 usSubsystemId;
    UINT16 usVendorSubsystemId;

    UINT64  aullBarSize[FPGA_VF_BAR_NUM_MAX];

    UINT16  usDomain;
    UINT8   ucBus;
    UINT8   ucDev;
    UINT8   ucFunc;

    bool      bResourceBurstable[FPGA_VF_BAR_NUM_MAX];

    UINT8  ucReserved[6];
}FpgaResourceMap;

typedef struct tagFPGA_IMG_INFO{
    UINT32 ulCmdOpsStatus;                   /* the high 16 bit present the status of Load/clear command operation and 
                                                                     the low 16 bit present the error code of Load/clear command operation*/    
    UINT32 ulFpgaPrStatus;                     /* the current status of FPGA PR region */    
    INT8   acHfid[HFI_ID_LEN_MAX];        /* HFID information */
    UINT32 ulShellID;                               /* FPGA shell ID */
}FPGA_IMG_INFO;

/* the current status of FPGA PR region */
enum {
    FPGA_PR_STATUS_NOT_PROGRAMMED = 0,
    FPGA_PR_STATUS_PROGRAMMED = 1,
    FPGA_PR_STATUS_EXCEPTION = 2,
    FPGA_PR_STATUS_PROGRAMMING = 3,
    FPGA_PR_STATUS_END
};

/* the status of Load/clear command operation */
enum {
    FPGA_OPS_STATUS_INITIALIZED = 0,
    FPGA_OPS_STATUS_SUCCESS = 1,
    FPGA_OPS_STATUS_FAILURE = 2,
    FPGA_OPS_STATUS_PROCESSING = 3,
    FPGA_OPS_STATUS_END
};

/* the error code of Load command operation */
enum {
    FPGA_LOAD_OK = 0,  /* State ok */
    FPGA_LOAD_GET_LOCK_BUSY = 1,  /* Get lock busy */
    FPGA_LOAD_WRITE_DB_ERR = 2,  /* Write DB error */
    FPGA_LOAD_GET_HOSTID_ERR = 3,  /* Get host id error */
    FPGA_LOAD_GET_NOVA_CFG_ERR = 4,  /* Get Nova configuration error*/
    FPGA_LOAD_TOKEN_MATCH_ERR = 5,  /* Authentication failed*/
    FPGA_LOAD_GET_SERVICEID_ERR = 6,  /* Get service id error */
    FPGA_LOAD_GET_NOVAAPI_ERR = 7,  /* Get Nova API error */
    FPGA_LOAD_GET_UUID_ERR = 8,  /* Get UUID error */
    FPGA_LOAD_INVALID_AEIID = 9,  /* AEI ID is invalid */
    FPGA_LOAD_GET_IMAGEPARA_ERR = 10,  /* Get image parameter error */
    FPGA_LOAD_AEI_CHECK_ERR = 11,  /* AEI check error */
    FPGA_LOAD_GET_AEIFILE_ERR = 12,  /* Get AEI file error */
    FPGA_LOAD_WR_MAILBOX_ERR = 13,  /* Write Mailbox error */
    FPGA_LOAD_PROGRAM_PARA_ERR = 14,  /* AEI program parameter error*/
    FPGA_LOAD_PROGRAM_ICAP_ERR = 15,  /* AEI program ICAP error */
    FPGA_LOAD_DDR_CHECK_ERR = 16,  /* DDR check error */
    FPGA_LOAD_FPGA_DISABLE_ERR = 17,  /* FPGA card is disable */    
    FPGA_LOAD_PUSH_QUEUE_ERR = 18,  /* fpga push task to queue error */          
    FPGA_LOAD_ERROR_END
};

/* the error code of other operation */
enum {  
    FPGA_OTHER_EXCEPTION_ERR = 50,    /* fpga exception error */         
    FPGA_OTHER_ERROR_END
};

/* the error code of clear command operation */
enum {
    FPGA_CLEAR_GET_LOCK_BUSY = 101,  /* Clear command Get lock busy */
    FPGA_CLEAR_WRITE_DB_ERR = 102,   /* Clear command Write DB error */
    FPGA_CLEAR_GET_BLANK_FILE_ERR = 103,   /* Clear command Get blank file error */
    FPGA_CLEAR_WR_MAILBOX_ERR = 104,   /* Clear command Get blank file error */
    FPGA_CLEAR_PROGRAM_PARA_ERR = 105,  /* Clear command Program parameter error */
    FPGA_CLEAR_PROGRAM_ICAP_ERR = 106,   /* Clear command Program ICAP error */
    FPGA_CLEAR_DDR_CHECK_ERR = 107,   /* Clear command ddr check error */
    FPGA_CLEAR_FPGA_DISABLE_ERR = 108,   /* Clear command FPGA card is disable */    
    FPGA_CLEAR_NOT_SUPPORT_ERR = 109,   /* Clear command is unsupported */         
    FPGA_CLEAR_PUSH_QUEUE_ERR = 110,   /* Clear command fpga push task to queue error */         
    FPGA_CLEAR_ERROR_END
};

void FPGA_LogErr( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... );
void FPGA_LogWarning( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... );
void FPGA_LogInfo(  INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... );
void FPGA_LogDebug( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... );

UINT32 FPGA_MgmtInit( void );
UINT32 FPGA_MgmtLoadHfiImage( UINT32 ulSlotIndex, INT8 *pcHfiId );
UINT32 FPGA_MgmtInquireFpgaImageInfo( UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo );
UINT32 FPGA_MgmtInquireLEDStatus( UINT32 ulSlotIndex );
UINT32 FPGA_MgmtClearHfiImage( UINT32 ulSlotIndex );
UINT32 FPGA_PciScanAllSlots( FpgaResourceMap straFpgaArray[], UINT32 ulSize );
UINT32 FPGA_MgmtOpsMutexRlock( UINT32 ulSlotId, INT32 * plFd );
UINT32 FPGA_MgmtOpsMutexWlock( UINT32 ulSlotId, INT32 * plFd );
UINT32 FPGA_MgmtOpsMutexUnlock( INT32 lFd );

#endif
