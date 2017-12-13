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


`ifndef _COMMON_AXI_SVH_
`define _COMMON_AXI_SVH_

`define AXI_COMPARE_OK       'd1
`define AXI_COMPARE_ERROR    'd1

`define AXI4_ADDR_WIDTH      'd64
`define AXI4_DATA_WIDTH      'd512
`define AXI4_STRB_WIDTH      (`AXI4_DATA_WIDTH >> 3)
`define AXI4_LEN_WIDTH       'd8
`define AXI4_ID_WIDTH        'd4
`define AXI4_RESP_WIDTH      'd2

`define AXI4S_DATA_WIDTH     'd256
`define AXI4S_KEEP_WIDTH     (`AXI4S_DATA_WIDTH >> 3)
`define AXI4S_USER_WIDTH     'd1

`define AXI4L_ADDR_WIDTH     'd32
`define AXI4L_DATA_WIDTH     'd32
`define AXI4L_STRB_WIDTH     (`AXI4L_DATA_WIDTH >> 3)
`define AXI4L_RESP_WIDTH     'd2

`define AXI4_MAX_LENGTH      ((`AXI4_DATA_WIDTH >> 3) * 'd256)


// AXI Burst Type

typedef enum bit [1 : 0] {
    e_AXI_BURST_FIX  = 2'b00,
    e_AXI_BURST_INCR = 2'b01,
    e_AXI_BURST_WRAP = 2'b10,
    e_AXI_BURST_RSV
} axi_burst_t;

// AXI Response Type

typedef enum bit [1 : 0] {
    e_AXI_RESP_OKAY  = 2'b00,
    e_AXI_RESP_EXOKAY= 2'b01,
    e_AXI_RESP_SLVERR= 2'b10,
    e_AXI_RESP_DECERR
} axi_resp_t;

typedef enum {
    e_AXI_AW_CHANNEL,
    e_AXI_W_CHANNEL,
    e_AXI_B_CHANNEL,
    e_AXI_AR_CHANNEL,
    e_AXI_R_CHANNEL
} axi_chan_t;

// AXI Operation type

typedef enum {
    e_AXI_OPT_RD = 'd0,
    e_AXI_OPT_WR = 'd1,
    e_AXI_OPT_NA
} axi_opt_t;

bit g_axi_cov_en = 'd1;

`endif

