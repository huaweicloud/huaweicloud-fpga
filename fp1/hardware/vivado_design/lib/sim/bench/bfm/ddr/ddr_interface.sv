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


`ifndef _DDR_INTERFACE_SV_
`define _DDR_INTERFACE_SV_

`timescale 1ns/1ps

// ./common/common_ddr.svh
`include "common_ddr.svh"

interface ddr_interface #(int AWIDTH = `DDRA_ADDR_WIDTH, 
                          int DBYTES = `DDRA_DATA_BYTES,
                          int RWIDTH = `DDRA_RANK_WIDTH,
                          int BWIDTH = `DDRA_BANK_WIDTH,
                          int GWIDTH = `DDRA_BG_WIDTH,
                          int CHECK  = 'd0,               // Assertion check enable
                          int SETUP  = 'd1,               // Setup time
                          int HOLD   = 'd0)               // Hold time
                          (input logic clk, 
                           input logic rst); // {{{

    //----------------------------------
    // Parameter Define
    //----------------------------------
    parameter DWIDTH = DBYTES << 3;  // Data width
   
    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef logic [AWIDTH - 'd1 : 0] ADDR_t ;
    typedef logic [DWIDTH - 'd1 : 0] DATA_t ;
    typedef logic [DBYTES - 'd1 : 0] MASK_t ;
    typedef logic [DBYTES - 'd1 : 0] DQS_t  ;
    typedef logic [RWIDTH - 'd1 : 0] RANK_t ;
    typedef logic [BWIDTH - 'd1 : 0] BANK_t ;
    typedef logic [GWIDTH - 'd1 : 0] BG_t   ;

    //----------------------------------
    // Signal declaration
    //----------------------------------
    
    //
    // DDR4 Signals
    //
    // All address and control input signals are sampled on the crossing of the 
    // positive edge of CK_t and negative edge of CK_c
    wire                    ddr4_ck_t;     // DDR4 Clk Posedge
    wire                    ddr4_ck_c;     // DDR4 Clk Negedge

    // CKE HIGH activates, and CKE Low deactivates, internal clock signals and
    // device input buffers and output drivers. Taking CKE Low provides Precharge 
    // Power-Down and Self-Refresh operation (all banks idle), or Active Power-Down 
    // (row Active in any bank). CKE is synchronous for Self-Refresh exit.
    wire [RWIDTH - 'd1 : 0] ddr4_cke ;     // DDR4 Clk Enable
    wire [RWIDTH - 'd1 : 0] ddr4_cs_n;     // DDR4 Chip Select
    wire [RWIDTH - 'd1 : 0] ddr4_odt ;     // DDR4 Ondie Termination
    wire                    ddr4_act_n;    // DDR4 Activation Command Input
    wire [DBYTES - 'd1 : 0] ddr4_dm_dbi_n; // DDR4 Input Data Mask and Data Bus Inversion
    wire [GWIDTH - 'd1 : 0] ddr4_bg;       // DDR4 Bank Group Inputs
    wire [BWIDTH - 'd1 : 0] ddr4_ba;       // DDR4 Bank Address Inputs
    wire [AWIDTH - 'd1 : 0] ddr4_addr;     // DDR4 Address Inputs
    wire                    ddr4_reset_n;  // DDR4 Active Low Asynchronous Reset
    wire [DWIDTH - 'd1 : 0] ddr4_dq;       // DDR4 Data Input/Output
    wire [DBYTES - 'd1 : 0] ddr4_dqs_t;    // DDR4 Data Strobe Posedge
    wire [DBYTES - 'd1 : 0] ddr4_dqs_c;    // DDR4 Data Strobe Negedge
    wire [DBYTES - 'd1 : 0] ddr4_tdqs_t;   // DDR4 Termination Data Strobe Posedge
    wire [DBYTES - 'd1 : 0] ddr4_tdqs_c;   // DDR4 Termination Data Strobe Negedge
    wire                    ddr4_par;      // DDR4 Command and Address Parity Input
    wire                    ddr4_alert_n;  // DDR4 Alert
    // Do not connect
    wire                    ddr4_ten;      // DDR4 Connectivity Test Mode Enable

    //
    // Inner Signals for debug
    //
    logic                   ddr4_ras_n;    // DDR4 Row Active Select
    logic                   ddr4_cas_n;    // DDR4 Coloum Active Select
    logic                   ddr4_we_n ;    // DDR4 Write/Read
    logic                   ddr4_ap   ;    // DDR4 Auto-precharge
    logic                   ddr4_bc_n ;    // DDR4 Burst Chopped Enable

    logic                   ddr4_mrs  ;    // DDR4 Mode Resgister Set
    logic                   ddr4_ref  ;    // DDR4 Refresh
    logic                   ddr4_sre  ;    // DDR4 Self Refresh Entry
    logic                   ddr4_srx  ;    // DDR4 Self Refresh Exit
    logic                   ddr4_pre  ;    // DDR4 Single Bank Precharge
    logic                   ddr4_prea ;    // DDR4 Precharge For All Banks
    logic                   ddr4_rfu  ;    // DDR4 RFU
    logic                   ddr4_act  ;    // DDR4 Bank Active
    logic                   ddr4_wr   ;    // DDR4 Write
    logic                   ddr4_wrs4 ;    // DDR4 Write
    logic                   ddr4_wrs8 ;    // DDR4 Write
    logic                   ddr4_wra  ;    // DDR4 Write with Precharge
    logic                   ddr4_wras4;    // DDR4 Write with Precharge
    logic                   ddr4_wras8;    // DDR4 Write with Precharge
    logic                   ddr4_rd   ;    // DDR4 Read
    logic                   ddr4_rds4 ;    // DDR4 Read
    logic                   ddr4_rds8 ;    // DDR4 Read
    logic                   ddr4_rda  ;    // DDR4 Read with Auto Precharge
    logic                   ddr4_rdas4;    // DDR4 Read with Auto Precharge
    logic                   ddr4_rdas8;    // DDR4 Read with Auto Precharge
    logic                   ddr4_nop  ;    // DDR4 NOP
    logic                   ddr4_des  ;    // DDR4 Device Deselect
    logic                   ddr4_pde  ;    // DDR4 Powerdown Entry
    logic                   ddr4_pdx  ;    // DDR4 Powerdown Exit
    logic                   ddr4_zqcl ;    // DDR4 ZQ Calibration Long
    logic                   ddr4_zqcs ;    // DDR4 ZQ Calibration Short

    logic [9 : 0]           ddr4_coloum;
    ADDR_t                  ddr4_row   ;


    assign ddr4_ras_n = ddr4_addr[16];
    assign ddr4_cas_n = ddr4_addr[15];
    assign ddr4_we_n  = ddr4_addr[14];
    assign ddr4_ap    = ddr4_addr[10];
    assign ddr4_bc_n  = ddr4_addr[12];

    assign ddr4_mrs   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n & ~ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n;
    assign ddr4_ref   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n & ~ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n;
    assign ddr4_pre   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n & ~ddr4_ras_n &  ddr4_cas_n & ~ddr4_we_n & ~ddr4_ap;
    assign ddr4_prea  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n & ~ddr4_ras_n &  ddr4_cas_n & ~ddr4_we_n &  ddr4_ap;
    assign ddr4_rfu   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n & ~ddr4_ras_n &  ddr4_cas_n &  ddr4_we_n;
    assign ddr4_act   = ddr4_cke & ~ddr4_cs_n &~ddr4_act_n;
    assign ddr4_wr    = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n & ~ddr4_ap;
    assign ddr4_wrs4  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n & ~ddr4_ap & ~ddr4_bc_n;
    assign ddr4_wrs8  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n & ~ddr4_ap &  ddr4_bc_n;
    assign ddr4_wra   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n &  ddr4_ap;
    assign ddr4_wras4 = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n &  ddr4_ap & ~ddr4_bc_n;
    assign ddr4_wras8 = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n & ~ddr4_we_n &  ddr4_ap &  ddr4_bc_n;
    assign ddr4_rd    = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n & ~ddr4_ap;
    assign ddr4_rds4  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n & ~ddr4_ap & ~ddr4_bc_n;
    assign ddr4_rds8  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n & ~ddr4_ap &  ddr4_bc_n;
    assign ddr4_rda   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n &  ddr4_ap;
    assign ddr4_rdas4 = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n &  ddr4_ap & ~ddr4_bc_n;
    assign ddr4_rdas8 = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n & ~ddr4_cas_n &  ddr4_we_n &  ddr4_ap &  ddr4_bc_n;
    assign ddr4_nop   = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n &  ddr4_cas_n &  ddr4_we_n;
    assign ddr4_des   = ddr4_cke &  ddr4_cs_n & ddr4_act_n;
    assign ddr4_zqcl  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n &  ddr4_cas_n & ~ddr4_we_n &  ddr4_ap;
    assign ddr4_zqcs  = ddr4_cke & ~ddr4_cs_n & ddr4_act_n &  ddr4_ras_n &  ddr4_cas_n & ~ddr4_we_n &  ddr4_ap;

    assign ddr4_coloum= (ddr4_wr | ddr4_wrs4 | ddr4_wrs8 | ddr4_wra | ddr4_wras4 | ddr4_wras8 | 
                         ddr4_rd | ddr4_rds4 | ddr4_rds8 | ddr4_rda | ddr4_rdas4 | ddr4_rdas8) ? ddr4_addr[9 : 0] : 'd0;
    assign ddr4_row   = ddr4_act ? ddr4_addr : 'd0;

endinterface : ddr_interface // }}}

`endif // _DDR_INTERFACE_SV_

