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


#define FPGA_SLOT_MAX                           8
#define HFI_ID_LEN_MAX                          36

#define HW_VF_VENDOR_ID                         0x19e5
#define HW_VF_DEVICE_ID                         0xd503
#define HW_OCL_PF_VENDOR_ID                     0x19e5
#define HW_OCL_PF_DEVICE_ID                     0xd512

#define FPGA_SHELL_TYPE_SHIFT                   16
#define FPGA_SHELL_TYPE_MASK                    0xfff0000
#define FPGA_SHELL_VERSION_SHIFT                0
#define FPGA_SHELL_VERSION_MASK                 0xffff

#define FPGA_VF_BAR_NUM_MAX                     1
#define OK                                      0
#define ERROR                                   -1

#define HFI_TOOL_VERSION  "v102"

#define INIT_VALUE                              -1

#define INPUT_PARAS_NUM_MIN                     2
#define INPUT_PARAS_FOR_PARSE_MIN               3
#define INPUT_PARAS_FOR_IL_MAX                  4
#define INPUT_PARAS_FOR_DF_MAX                  3
#define INPUT_PARAS_FOR_IF_MAX                  4
#define INPUT_PARAS_FOR_LF_MAX                  6
#define INPUT_OPTCODE_LENGTH_LIMIT              3 


#define sizeof_array( arr ) \
    ( sizeof( arr ) / sizeof( arr[0] ) )


#define SDKRTN_ERR_BASE    0xFFFFF000

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


/* FPGA load state code */
enum {
    FPGA_LOAD_STATUS_NOT_PROGRAMMED = 0,
    FPGA_LOAD_STATUS_LOADED = 1,
    FPGA_LOAD_STATUS_LOAD_FAILED = 2,
    FPGA_LOAD_STATUS_BUSY = 3,
    FPGA_LOAD_STATUS_INVALID_ID = 4,
    FPGA_LOAD_STATUS_END = 5
};

/* FPGA state code */
enum {
    FPGA_STATUS_OK = 0,                    /* state ok */
    FPGA_STATUS_COMPETENCE_ERR = 1,        /* Authentication failed */
    FPGA_STATUS_GETSERVICEID_ERR = 2,      /* Get service id failed */
    FPGA_STATUS_GETPRJECTID_ERR = 3,       /* Get project id failed */
    FPGA_STATUS_GETNOVAINFO_ERR = 4,       /* Get nova api domain name failed */
    FPGA_STATUS_GETVMUUID_ERR = 5,         /* Get vm uuid failed */
    FPGA_STATUS_GETOBSINFO_ERR = 6,        /* Get OBS information failed */
    FPGA_STATUS_GETFILE_ERR = 7,           /* Get file failed */
    FPGA_STATUS_PARA_ERR = 8,              /* Input parameter error */
    FPGA_STATUS_LOAD_BUSY = 9,             /* Input parameter correct, but last load process is not finished */
    FPGA_STATUS_INNER_ERR = 10,            /* Load failed */
    FPGA_STATUS_CLEARD = 11,               /* Clear success */
    FPGA_STATUS_LOADING = 12,              /* Loading */
    FPGA_STATUS_MBOX_ERR = 13,             /* Write mailbox failed */
    FPGA_STATUS_LOCK_ERR = 14,             /* Lock failed */
    FPGA_STATUS_GETNOVACFG_ERR = 15,       /* Get nova configuration failed */
    FPGA_STATUS_AEI_MATCH_ERR = 16,        /* Match AEI info failed */
    FPGA_STATUS_FILE_ERR = 17,             /* Verify AEI file failed */    
    FPGA_STATUS_END
};

#endif
