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


`ifndef _COMMON_DEFINE_SVH_
`define _COMMON_DEFINE_SVH_

// Clock and reset define {{{

// }}}

`define COMPARE_OK       'd1
`define COMPARE_ERROR    'd1

`define tb_assert(ARG, MSG) \
    assert (ARG); \
    else begin \
        $display("[FATAL]: Assertion fail! `ARG not matched"); \
        finish(); \
    end

parameter FLOW_STIM_GEN    = 'd100;
parameter FLOW_AXI_S_WRITE = 'd200;
parameter FLOW_RM_CHECK    = 'd300;

`endif

