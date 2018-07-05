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

#ifndef __FPGA_CMDPCI_H__
#define __FPGA_CMDPCI_H__
#include <stdbool.h>
#include "FPGA_Common.h"

#define PCI_DEV_FMT                           "%04x:%02x:%02x.%d"
#define PCI_DEV_RESOURCE_PATH                 "/sys/bus/pci/devices/%s/resource%u"
#define PCI_DEV_RESOURCE_WC_PATH              "/sys/bus/pci/devices/%s/resource%u_wc"
#define PCI_DEVICES_PATH                      "/sys/bus/pci/devices/"
#define PCI_DEV_VENDOR_PATH                   "/sys/bus/pci/devices/%s/vendor"
#define PCI_DEV_DEVICE_PATH                   "/sys/bus/pci/devices/%s/device"


#define DIR_NAME_MAX                          12                        /* lenth of directory */
#define DBDF_LEN                              16

/************************* Define error code ************************************/
#define PCI_MODULE_ERROR                        5
#define SDKRTN_PCI_SUCCESS                      0
#define SDKRTN_PCI_ERROR_BASE                   ( SDKRTN_ERR_BASE  +  ( PCI_MODULE_ERROR << 8  ) )
#define SDKRTN_PCI_INPUT_ERROR                  ( SDKRTN_PCI_ERROR_BASE + 0x1 )
#define SDKRTN_PCI_OPENDIR_ERROR                ( SDKRTN_PCI_ERROR_BASE + 0x2 )
#define SDKRTN_PCI_READDIR_ERROR                ( SDKRTN_PCI_ERROR_BASE + 0x3 )
#define SDKRTN_PCI_SNPRINTF_ERROR               ( SDKRTN_PCI_ERROR_BASE + 0x4 )
#define SDKRTN_PCI_STAT_ERROR                   ( SDKRTN_PCI_ERROR_BASE + 0x5 )
#define SDKRTN_PCI_NONE_FPGA_ERROR              ( SDKRTN_PCI_ERROR_BASE + 0x6 )
#define SDKRTN_PCI_FOPEN_ERROR                  ( SDKRTN_PCI_ERROR_BASE + 0x7 )
#define SDKRTN_PCI_FSCANF_ERROR                 ( SDKRTN_PCI_ERROR_BASE + 0x8 )
#define SDKRTN_PCI_SSCANF_ERROR                 ( SDKRTN_PCI_ERROR_BASE + 0x9 )
#define SDKRTN_PCI_OPEN_ERROR                   ( SDKRTN_PCI_ERROR_BASE + 0xA )
#define SDKRTN_PCI_MMAP_ERROR                   ( SDKRTN_PCI_ERROR_BASE + 0xB )
#define SDKRTN_PCI_MUNMAP_ERROR                 ( SDKRTN_PCI_ERROR_BASE + 0xC )
#define SDKRTN_PCI_ALLOC_BAR_ERROR              ( SDKRTN_PCI_ERROR_BASE + 0xD )
#define SDKRTN_PCI_GET_BAR_ERROR                ( SDKRTN_PCI_ERROR_BASE + 0xE )
#define SDKRTN_PCI_VENDOR_ID_ERROR              ( SDKRTN_PCI_ERROR_BASE + 0xF )
#define SDKRTN_PCI_READ_ERROR                   ( SDKRTN_PCI_ERROR_BASE + 0x10 )
#define SDKRTN_PCI_INVALID_VALUE_ERROR          ( SDKRTN_PCI_ERROR_BASE + 0x11 )
#define SDKRTN_PCI_MEMSET_ERROR                 ( SDKRTN_PCI_ERROR_BASE + 0x12 )
#define SDKRTN_PCI_STRNCPY_ERROR                ( SDKRTN_PCI_ERROR_BASE + 0x13 )
#define SDKRTN_PCI_GET_SLOT_ERROR               ( SDKRTN_PCI_ERROR_BASE + 0x14 )

typedef struct tagFPGA_PCI_BAR_INFO
{
    bool  bAllocatedFlag;
    void *pMemBase;
    UINT64 ullMemSize;
} FPGA_PCI_BAR_INFO;

UINT32 FPGA_PciEnableSlotsBar( UINT32 ulSlotIndex, UINT32 ulBarNum, UINT32 *pulBarHandle );
FPGA_PCI_BAR_INFO *FPGA_PciGetBar( UINT32 ulBarHandle );

extern FpgaShellType g_astrShellType;

#endif
