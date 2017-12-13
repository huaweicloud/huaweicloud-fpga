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


`ifndef _TB_VIF_OBJ_SV_
`define _TB_VIF_OBJ_SV_

// ./bfm/tb_interface.sv
`include "tb_interface.sv"

// ./bfm/axi4l/axil_vif_obj.sv
`include "axil_vif_obj.sv"

// ./bfm/axi4s/axis_vif_obj.sv
`include "axis_vif_obj.sv"

`ifdef USE_DDR_MODEL
// ./bfm/axi4/axi_vif_obj.sv
`include "axi_vif_obj.sv"
`endif


typedef axis_vif_obj #(.DWIDTH (`AXI4S_DATA_WIDTH), 
                       .KWIDTH (`AXI4S_KEEP_WIDTH), 
                       .UWIDTH (`AXI4S_USER_WIDTH)) axisc_vif_t;

typedef axis_vif_obj #(.DWIDTH (`AXI4_DATA_WIDTH ), 
                       .KWIDTH (`AXI4_STRB_WIDTH ), 
                       .UWIDTH (`AXI4S_USER_WIDTH)) axisd_vif_t;

`ifdef USE_DDR_MODEL // {{{
typedef axi_vif_obj  #(.AWIDTH (`AXI4_ADDR_WIDTH ), 
                       .DWIDTH (`AXI4_DATA_WIDTH ), 
                       .SWIDTH (`AXI4_STRB_WIDTH ), 
                       .LWIDTH (`AXI4_LEN_WIDTH  )) axid_vif_t;
`endif // }}}

class tb_vif_obj;

    virtual tb_interface m_tb_vif;

    axil_vif_obj         m_axil_vif;

    axisc_vif_t          m_axismc_vif;
    axisd_vif_t          m_axismd_vif;
    axisc_vif_t          m_axissc_vif;
    axisd_vif_t          m_axissd_vif;

`ifdef USE_DDR_MODEL
    axid_vif_t           m_axisd_vif;
`endif

    function new(input virtual tb_interface tb_vif    , 
                 input axil_vif_obj         axil_vif  ,
                 input axisc_vif_t          axismc_vif,
                 input axisd_vif_t          axismd_vif,
                 input axisc_vif_t          axissc_vif,
                 input axisd_vif_t          axissd_vif
             `ifdef USE_DDR_MODEL
                ,input axid_vif_t           axisd_vif
             `endif
                 );
        m_tb_vif    = tb_vif   ;
        m_axil_vif  = axil_vif ;
        m_axismc_vif= axismc_vif;
        m_axismd_vif= axismd_vif;
        m_axissc_vif= axissc_vif;
        m_axissd_vif= axissd_vif;
    `ifdef USE_DDR_MODEL
        m_axisd_vif = axisd_vif ;
    `endif
    endfunction : new

endclass : tb_vif_obj

`endif // _TB_VIF_OBJ_SV_

