//
//------------------------------------------------------------------------------
//     Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the Huawei Software License (the "License").
//     A copy of the License is located in the "LICENSE" file accompanying 
//     this file.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//     Huawei Software License for more details. 
//------------------------------------------------------------------------------

#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

// Demo Regs
const uint32_t g_reg_ver_time        = (0x0 << 2);
const uint32_t g_reg_ver_type        = (0x1 << 2);
const uint32_t g_reg_adder_cfg_wdata0= (0x2 << 2);
const uint32_t g_reg_adder_cfg_wdata1= (0x3 << 2);
const uint32_t g_reg_adder_sum_rdata = (0x4 << 2);
const uint32_t g_reg_oppos_data      = (0x5 << 2);
const uint32_t g_reg_vled_data       = (0x6 << 2);
const uint32_t g_reg_oppos_addr      = (0x7 << 2);

#endif // _TEST_COMMON_H_
