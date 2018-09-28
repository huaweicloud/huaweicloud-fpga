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


`ifndef _AXIL_VIF_OBJ_SV_
`define _AXIL_VIF_OBJ_SV_

// ./bfm/axi4l/axil_interface.sv
`include "axil_interface.sv"

class axil_vif_obj;

    virtual axil_interface m_axil_vif;

    function new(input virtual axil_interface axil_vif);
        m_axil_vif = axil_vif;
    endfunction : new

endclass : axil_vif_obj

`endif // _AXIL_VIF_OBJ_SV_

