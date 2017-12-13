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


`ifndef _AXIS_VIF_OBJ_SV_
`define _AXIS_VIF_OBJ_SV_

// ./bfm/axi4s/axis_interface.sv
`include "axis_interface.sv"

class axis_vif_obj #(int DWIDTH = `AXI4S_DATA_WIDTH, // Data bus width
                     int KWIDTH = `AXI4S_KEEP_WIDTH, // Keep bus width
                     int UWIDTH = `AXI4S_USER_WIDTH ,// User bus width
                     int CHECK  = 'd0,               // Assertion check enable
                     int SETUP  = 'd1,               // Setup time
                     int HOLD   = 'd0);

    typedef virtual axis_interface #(.DWIDTH (DWIDTH), 
                                     .KWIDTH (KWIDTH), 
                                     .UWIDTH (UWIDTH), 
                                     .CHECK  (CHECK ), 
                                     .SETUP  (SETUP ), 
                                     .HOLD   (HOLD  )) axis_vif_t;

    axis_vif_t m_axis_vif;

    function new(input axis_vif_t axis_vif);
        m_axis_vif = axis_vif;
    endfunction : new

endclass : axis_vif_obj

`endif // _AXIS_VIF_OBJ_SV_

