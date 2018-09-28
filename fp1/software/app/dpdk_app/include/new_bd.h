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
#ifndef	__NEW_BD_H__
#define	__NEW_BD_H__

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#pragma pack(1)
typedef struct _acc_second_bd_ {
    uint64_t    src_fpga_phy_addr;
    uint64_t    dst_fpga_phy_addr;
    uint8_t     rsv1[8];
    uint8_t     rsv2[8];
}acc_second_bd;

typedef struct _acc_bd_ {
    uint64_t    src_phy_addr;
    uint64_t    dst_phy_addr;
    uint32_t    length;

    /* AE fill ve_info_xxx */
    uint16_t    ve_info_queue_id;
    uint16_t    ve_info_pf_vf_id;
    uint16_t    ve_info_vm_id;

    uint8_t     acc_type;
    uint8_t     acc_length;
    uint8_t     rsv[2];

    uint8_t     bd_code;    /* fix: 0x5a */
    union {
    uint8_t     odd_even_all;
    uint8_t     odd_even_0_31 :     1;
    uint8_t     odd_even_32_63 :    1;
    uint8_t     odd_even_64_95 :    1;
    uint8_t     odd_even_96_127 :   1;
    uint8_t     odd_even_128_159 :  1;
    uint8_t     odd_even_160_191 :  1;
    uint8_t     odd_even_192_223 :  1;
    uint8_t     odd_even_224_247 :  1;
    }odd_even;
    
} acc_rx_bd, acc_tx_bd;
#pragma pack()

#endif  // __NEW_BD_H__