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


`ifndef _TB_INTERFACE_SV_
`define _TB_INTERFACE_SV_

`timescale 1ns/1ps

// ./bfm/axi4l/axil_interface.sv
`include "axil_interface.sv"

// ./bfm/axi4s/axis_interface.sv
`include "axis_interface.sv"

`ifdef USE_DDR_MODEL
// ./bfm/axi4/axi_interface.sv
`include "axi_interface.sv"
`endif

// ./bfm/ddr/ddr_interface.sv
`include "ddr_interface.sv"

interface tb_interface();

    logic clk_200m     ;
    logic rst_200m     ;
    logic rst_200m_done;

    logic clk_100m_p   ;
    logic clk_100m_n   ;
    logic rst_100m     ;
    logic rst_100m_done;

    // AXI4-Stream Interface for DMA begin {{{
    
    // Master AXI4-Stream Interface begin {{{
    // Interface with DMA Channel A(Command)
    axis_interface #(.DWIDTH (`AXI4S_DATA_WIDTH), 
                     .KWIDTH (`AXI4S_KEEP_WIDTH), 
                     .UWIDTH (`AXI4S_USER_WIDTH)) u_axismc_if(.clk (clk_200m ), 
                                                              .rst (rst_200m )); 
    // Interface with DMA Channel A(Data)
    axis_interface #(.DWIDTH (`AXI4_DATA_WIDTH ), 
                     .KWIDTH (`AXI4_STRB_WIDTH ), 
                     .UWIDTH (`AXI4S_USER_WIDTH)) u_axismd_if(.clk (clk_200m ), 
                                                              .rst (rst_200m )); 
    // Master AXI4-Stream Interface end }}}

    // Slave AXI4-Stream Interface begin {{{
    // Interface with DMA Channel A(Command)
    axis_interface #(.DWIDTH (`AXI4S_DATA_WIDTH), 
                     .KWIDTH (`AXI4S_KEEP_WIDTH), 
                     .UWIDTH (`AXI4S_USER_WIDTH)) u_axissc_if(.clk (clk_200m ), 
                                                              .rst (rst_200m )); 
    // Interface with DMA Channel A(Data)
    axis_interface #(.DWIDTH (`AXI4_DATA_WIDTH ), 
                     .KWIDTH (`AXI4_STRB_WIDTH ), 
                     .UWIDTH (`AXI4S_USER_WIDTH)) u_axissd_if(.clk (clk_200m ), 
                                                              .rst (rst_200m )); 
    // Slave AXI4-Stream Interface end }}}
    // AXI4-Stream Interface for DMA end }}}

`ifdef USE_DDR_MODEL
    // AXI4 Interface for DDR(Slave)
    axi_interface #(.AWIDTH (`AXI4_ADDR_WIDTH ), 
                    .DWIDTH (`AXI4_DATA_WIDTH ),
                    .SWIDTH (`AXI4_STRB_WIDTH ), 
                    .LWIDTH (`AXI4_LEN_WIDTH  ))  u_axisd_if(.clk (clk_200m ), 
                                                             .rst (rst_200m ));
`endif

    // AXI4-lite Interface for reg cfg(bar1)
    axil_interface #(.AWIDTH (`AXI4L_ADDR_WIDTH), 
                     .DWIDTH (`AXI4L_DATA_WIDTH), 
                     .SWIDTH (`AXI4L_STRB_WIDTH)) u_axil1_if(.clk (clk_200m ), 
                                                             .rst (rst_200m ));

    // AXI4-lite Interface for bar2(opencl?)
    axil_interface #(.AWIDTH (`AXI4L_ADDR_WIDTH), 
                     .DWIDTH (`AXI4L_DATA_WIDTH), 
                     .SWIDTH (`AXI4L_STRB_WIDTH)) u_axil2_if(.clk (clk_200m ), 
                                                             .rst (rst_200m ));

    // DDR Interface begin {{{
    // DDRA Interface begin {{{
    ddr_interface  #(.AWIDTH (`DDRA_ADDR_WIDTH ),
                     .DBYTES (`DDRA_DATA_BYTES ),
                     .RWIDTH (`DDRA_RANK_WIDTH ),
                     .BWIDTH (`DDRA_BANK_WIDTH ),
                     .GWIDTH (`DDRA_BG_WIDTH   )) u_ddra_if(.clk (clk_200m ), 
                                                            .rst (rst_200m ));
    // DDRA Interface end }}}
    // DDRB Interface begin {{{
    ddr_interface  #(.AWIDTH (`DDRB_ADDR_WIDTH ),
                     .DBYTES (`DDRB_DATA_BYTES ),
                     .RWIDTH (`DDRB_RANK_WIDTH ),
                     .BWIDTH (`DDRB_BANK_WIDTH ),
                     .GWIDTH (`DDRB_BG_WIDTH   )) u_ddrb_if(.clk (clk_200m ), 
                                                            .rst (rst_200m ));
    // DDRB Interface end }}}
    // DDRD Interface begin {{{
    ddr_interface  #(.AWIDTH (`DDRD_ADDR_WIDTH ),
                     .DBYTES (`DDRD_DATA_BYTES ),
                     .RWIDTH (`DDRD_RANK_WIDTH ),
                     .BWIDTH (`DDRD_BANK_WIDTH ),
                     .GWIDTH (`DDRD_BG_WIDTH   )) u_ddrd_if(.clk (clk_200m ), 
                                                            .rst (rst_200m ));
    // DDRD Interface end }}}
    // DDR Interface end }}}

endinterface : tb_interface

`endif // _TB_INTERFACE_SV_

