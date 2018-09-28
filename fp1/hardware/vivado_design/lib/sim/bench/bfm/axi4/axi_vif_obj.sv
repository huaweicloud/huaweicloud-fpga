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


`ifndef _AXI_VIF_OBJ_SV_
`define _AXI_VIF_OBJ_SV_

// ./bfm/axi4/axi_interface.sv
`include "axi_interface.sv"

class axi_vif_obj #(int AWIDTH = `AXI4_ADDR_WIDTH,  // Address bus width 
                    int DWIDTH = `AXI4_DATA_WIDTH,  // Data bus width
                    int SWIDTH = `AXI4_STRB_WIDTH,  // Strb bus width
                    int LWIDTH = `AXI4_LEN_WIDTH ,  // Burst len width
                    int CHECK  = 'd0,               // Assertion check enable
                    int SETUP  = 'd1,               // Setup time
                    int HOLD   = 'd0);

    typedef virtual axi_interface #(.AWIDTH (AWIDTH), 
                                    .DWIDTH (DWIDTH), 
                                    .SWIDTH (SWIDTH), 
                                    .LWIDTH (LWIDTH), 
                                    .CHECK  (CHECK ), 
                                    .SETUP  (SETUP ), 
                                    .HOLD   (HOLD  )) axi_vif_t;

    axi_vif_t m_axi_vif;

    function new(input axi_vif_t axi_vif);
        m_axi_vif = axi_vif;
    endfunction : new

endclass : axi_vif_obj

`endif // _AXI_VIF_OBJ_SV_

