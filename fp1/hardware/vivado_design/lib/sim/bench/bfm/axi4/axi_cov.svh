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


`ifndef _AXI_COV_SVH_
`define _AXI_COV_SVH_

// common/common_axi.svh
`include "common_axi.svh"

class axi_cov;

    bit [3  : 0] id    ;  // ID
    bit [63 : 0] addr  ;  // Address

    int          blen  ;  // Burst length
    axi_opt_t    opt   ;  // Operatio type
    axi_burst_t  btype ;  // Burst type

    axi_resp_t   resp  ;  // Response
    int          strb  ;  // Strobe value
    bit          lmatch;  // length match

    extern function new();

endclass : axi_cov

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axi_cov
//
//----------------------------------------------------------------------------------------------------------------------

function axi_cov::new();
    id    = 'd0;
    addr  = 'd0;
    blen  = 'd0;
    opt   = e_AXI_OPT_NA;
    btype = e_AXI_BURST_FIX;
    resp  = e_AXI_RESP_OKAY;
    strb  = 'd0;
    lmatch= 'd0;
endfunction : new

`endif // _AXI_COV_SVH_

