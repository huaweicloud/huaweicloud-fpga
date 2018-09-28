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


`ifndef _AXIS_INTERFACE_SV_
`define _AXIS_INTERFACE_SV_

`timescale 1ns/1ps

// ./common/common_axi.svh
`include "common_axi.svh"

interface axis_interface #(int DWIDTH = `AXI4S_DATA_WIDTH, // Data bus width
                           int KWIDTH = `AXI4S_KEEP_WIDTH, // Keep bus width
                           int UWIDTH = `AXI4S_USER_WIDTH ,// User bus width
                           int CHECK  = 'd0,               // Assertion check enable
                           int SETUP  = 'd1,               // Setup time
                           int HOLD   = 'd0)               // Hold time
                           (input logic clk, 
                            input logic rst); // {{{

    //----------------------------------
    // Parameter Define
    //----------------------------------
    
    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef logic [DWIDTH - 'd1 : 0] DATA_t ;
    typedef logic [KWIDTH - 'd1 : 0] KEEP_t ;
    typedef logic [UWIDTH - 'd1 : 0] USER_t ;
    typedef bit   [DWIDTH - 'd1 : 0] DATA2_t;

    //----------------------------------
    // Signal declaration
    //----------------------------------
    
    //
    // Global Signals
    //
    logic         aclk   ;
    logic         aresetn;     // Active low

    //
    // Data Channel
    //
    DATA_t        ddata;       // Data
    KEEP_t        dkeep;       // Keep
    USER_t        duser;       // User
    logic         dlast;       // Last
    logic         dvalid;      // Valid
    logic         dready;      // Ready

    // Inner used
    string        ifname;

    assign aclk    = clk ;
    assign aresetn = ~rst;

    //----------------------------------
    // Clocking define
    //----------------------------------

    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
    // Data Channel
    clocking dchn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  ddata;
        inout  dkeep;
        inout  duser;
        inout  dlast;
        inout  dvalid;
        inout  dready;
    endclocking
    `endif
    
    // ----------------------------------------------------
    // Assertion Property Define
    // ----------------------------------------------------

    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
    // Write Data can not be X/Z when wvalid and wready are all high.
    property propertyDataUnknow;
        @ (dchn)
        disable iff(!CHECK)
        (dchn.dvalid == 'd1 && dchn.dready == 'd1) |-> !$isunknown(dchn.ddata);
    endproperty

    // ----------------------------------------------------
    // Assertion Define
    // ----------------------------------------------------

    asserDataUnknow: assert property (propertyDataUnknow)
    else $error($psprintf("[%s]: Assertion fail! Data contains X or Z when valid and ready are all 1", ifname));
    `endif

    // ----------------------------------------------------
    // Task Define
    // ----------------------------------------------------

    // Innitial Bus to default value(Used for axis_master)
    task initial_bus_master();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Initial Data Channel
        dchn.ddata    <= 'd0;
        dchn.dkeep    <= 'd0;
        dchn.duser    <= 'd0;
        dchn.dlast    <= 'd0;
        dchn.dvalid   <= 'd0;
    `else
        // Initial W Channel
        ddata         <= 'd0;
        dkeep         <= 'd0;
        duser         <= 'd0;
        dlast         <= 'd0;
        dvalid        <= 'd0;
    `endif
    endtask : initial_bus_master

    // Innitial Bus to default value(Used for axis_slave)
    task initial_bus_slave();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Initial W Channel
        dchn.dready   <= 'd0;
    `else
        // Initial W Channel
        dready        <= 'd0;
    `endif
    endtask : initial_bus_slave

    // Wait a cycle
    task wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (dchn);
    `else
        repeat (num) @ (posedge aclk);
    `endif
    endtask : wait_clock

    // AXI4-Stream Master Function begin: {{{
 
    // Transmit write data(Transmit data to W channel, do not support
    // back to back mode)

    task transmit_data(input DATA2_t data[], 
                       input KEEP_t  keep, 
                       input USER_t  user);
        automatic bit last = 'd0;
        automatic int blen = data.size();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert wvalid signal
        dchn.dvalid <= 'd1;
        foreach (data[idx]) begin
            last = (blen - 'd1) == idx;
            dchn.ddata  <= data[idx];
            dchn.dkeep  <= last ? keep : {KWIDTH{1'd1}};
            dchn.duser  <= last ? user : {UWIDTH{1'd0}};
            dchn.dlast  <= last;
            // Hold the valid, data, keep and last when ready goes down
            if (dchn.dready == 'd1) begin
                @ (dchn);
            end else begin
                while (1) begin
                    if (dchn.dready == 'd1)  break;
                    @ (dchn);
                end
            end
        end
        // When last is valid, deassert the valid to low
        // Deasset the valid signal(Do not support back to back transpot)
        dchn.dvalid <= 'd0;
        dchn.dlast  <= 'd0;
    `else
    `endif
    endtask : transmit_data

    // AXI4-Stream Master Function end: }}}
    
    // AXI4-Stream Slave Function begin: {{{

    // Collect data(Read data from W channel, do not support
    // back to back mode)

    task collect_data(output DATA2_t data[], 
                      output KEEP_t  keep, 
                      output USER_t  user);
        automatic DATA2_t data_queue[$];
        automatic int     idx = 'd0;
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        while (dchn.dvalid != 'd1) begin
            @ (dchn);
        end
        if (dchn.dready != 'd1) begin
            // Assert ready when valid is high
            dchn.dready <= 'd1;
            @ (dchn);
        end
        while (1) begin
            // Get data when valid and ready are both active
            data_queue.push_back(dchn.ddata);
            keep = dchn.dkeep;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (dchn.dlast == 'd1) begin
                data = data_queue;
                user = dchn.duser;
                dchn.dready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
            @ (dchn);
        end
    `else
    `endif
    endtask : collect_data

    // AXI4-Stream Slave Function end: }}}

endinterface : axis_interface // }}}

`endif // _AXIS_INTERFACE_SV_

