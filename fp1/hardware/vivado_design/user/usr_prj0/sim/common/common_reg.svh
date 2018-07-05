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


`ifndef _COMMON_REG_SVH_
`define _COMMON_REG_SVH_

// ./tb_pkg.svh
`include "tb_pkg.svh"

typedef bit [`AXI4L_DATA_WIDTH - 1 : 0] REG_DATA_t;
typedef bit [`AXI4L_ADDR_WIDTH - 1 : 0] REG_ADDR_t;

// Demo Regs
const REG_ADDR_t g_reg_ver_time        = ('h0 << 2);
const REG_ADDR_t g_reg_ver_type        = ('h1 << 2);
const REG_ADDR_t g_reg_adder_cfg_wdata0= ('h2 << 2);
const REG_ADDR_t g_reg_adder_cfg_wdata1= ('h3 << 2);
const REG_ADDR_t g_reg_adder_sum_rdata = ('h4 << 2);
const REG_ADDR_t g_reg_oppos_data      = ('h5 << 2);
const REG_ADDR_t g_reg_vled_data       = ('h6 << 2);
const REG_ADDR_t g_reg_oppos_addr      = ('h7 << 2);

`endif // _COMMON_REG_SVH_

