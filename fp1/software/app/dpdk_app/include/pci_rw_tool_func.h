/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *   All rights reserved.
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
#ifndef	_PCI_RW_TOOL_FUNC_H_
#define	_PCI_RW_TOOL_FUNC_H_


#define     SYS_PCI_DEVICES_DIR     "/sys/bus/pci/devices"
#define     VENDOR_ID_FILE              "vendor"
#define     DEVICE_ID_FILE              "device"

/* Vendor id and device id of VF device */
#define     STR_ACC_VENDOR_ID               "0x19e5"
#define     STR_ACC_DEVICE_ID               "0xd503"

/* Max bar number */
#define MAX_BAR_NUM                 (3)

int pci_barx_init_env(int vf_idx, int bar_idx);
int pci_barx_uninit_env(int bar_idx);
int pci_barx_write_regs(unsigned int* write_addrs, unsigned int* write_values, unsigned int write_addrs_num, int bar_idx);
int pci_barx_read_regs(unsigned int* read_addrs, unsigned int read_addrs_num, unsigned int* read_values, int bar_idx);

/* Keep these interface for compatibility */
int pci_bar2_init_env(int vf_idx);
int pci_bar2_uninit_env(void);
int pci_bar2_write_regs(unsigned int* write_addrs, unsigned int* write_values, unsigned int write_addrs_num);
int pci_bar2_read_regs(unsigned int* read_addrs, unsigned int read_addrs_num, unsigned int* read_values);

#endif
