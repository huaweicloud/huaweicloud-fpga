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


`ifndef _AXIL_INTERFACE_SV_
`define _AXIL_INTERFACE_SV_

`timescale 1ns/1ps

// ./common/common_axi.svh
`include "common_axi.svh"

interface axil_interface #(int AWIDTH = `AXI4L_ADDR_WIDTH,  // Address bus width 
                           int DWIDTH = `AXI4L_DATA_WIDTH,  // Data bus width
                           int SWIDTH = `AXI4L_STRB_WIDTH,  // Strobe bue width
                           int CHECK  = 'd0,                // Assertion check Enable
                           int SETUP  = 'd1,                // Setup time
                           int HOLD   = 'd0)                // Hold time
                           (input logic clk, 
                            input logic rst); // {{{

    //----------------------------------
    // Parameter Define
    //----------------------------------
    
    parameter RWIDTH = `AXI4L_RESP_WIDTH;  // Resp width

    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef logic [AWIDTH - 'd1 : 0] ADDR_t;
    typedef logic [DWIDTH - 'd1 : 0] DATA_t;
    typedef logic [SWIDTH - 'd1 : 0] STRB_t;
    typedef logic [RWIDTH - 'd1 : 0] RESP_t;

    //----------------------------------
    // Signal declaration
    //----------------------------------
    
    //
    // Global Signals
    //
    logic         aclk;
    logic         aresetn;     // Active low

    //
    // Write Address Channel
    //
    ADDR_t        awaddr;
    logic [2 : 0] awprot;
    logic         awvalid;
    logic         awready;

    //
    // Write Data Channel
    //
    DATA_t        wdata;
    STRB_t        wstrb;
    logic         wvalid;
    logic         wready;

    //
    // Write Response Channel
    //
    RESP_t        bresp;
    logic         bvalid;
    logic         bready;

    //
    // Read Address Channel
    //
    ADDR_t        araddr;
    logic [2 : 0] arprot;
    logic         arvalid;
    logic         arready;

    //
    // Read Data Channel
    //
    DATA_t        rdata;
    RESP_t        rresp;
    logic         rvalid;
    logic         rready;

    // Inner used
    string        ifname;

    assign aclk    = clk ;
    assign aresetn = ~rst;

    //----------------------------------
    // Clocking define
    //----------------------------------

    // Clocking was not supportted by vivado simulator
    `ifndef VIVADO
    // Address Write Channel
    clocking awchn @ (posedge aclk);
        default input #1 output #0;
        inout  awaddr;
        inout  awprot;
        inout  awvalid;
        inout  awready;
    endclocking

    // Write Channel
    clocking wchn @ (posedge aclk);
        default input #1 output #0;
        inout  wdata;
        inout  wstrb;
        inout  wvalid;
        inout  wready;
    endclocking

    // Write Response Channel
    clocking bchn @ (posedge aclk);
        default input #1 output #0;
        inout  bresp;
        inout  bvalid;
        inout  bready;
    endclocking

    // Address Read Channel
    clocking archn @ (posedge aclk);
        default input #1 output #0;
        inout  araddr;
        inout  arprot;
        inout  arvalid;
        inout  arready;
    endclocking

    // Read Channel
    clocking rchn @ (posedge aclk);
        default input #1 output #0;
        inout  rdata;
        inout  rresp;
        inout  rvalid;
        inout  rready;
    endclocking
    `endif

    // ----------------------------------------------------
    // Assertion Property Define
    // ----------------------------------------------------

    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
    // Write Address can not be X/Z when awvalid and awready are all high.
    property propertyWaddrUnknow;
        @ (awchn)
        disable iff(!CHECK)
        (awchn.awvalid == 'd1 && awchn.awready == 'd1) |-> !$isunknown(awchn.awaddr);
    endproperty

    // Write Data can not be X/Z when wvalid and wready are all high.
    property propertyWdataUnknow;
        @ (wchn)
        disable iff(!CHECK)
        (wchn.wvalid == 'd1 && wchn.wready == 'd1) |-> !$isunknown(wchn.wdata);
    endproperty

    // Write Response can not be X/Z when bvalid and bready are all high.
    property propertyBrespUnknow;
        @ (bchn)
        disable iff(!CHECK)
        (bchn.bvalid == 'd1 && bchn.bready == 'd1) |-> !$isunknown(bchn.bresp);
    endproperty

    // Read Address can not be X/Z when arvalid and arready are all high.
    property propertyRaddrUnknow;
        @ (archn)
        disable iff(!CHECK)
        (archn.arvalid == 'd1 && archn.arready == 'd1) |-> !$isunknown(archn.araddr);
    endproperty

    // Read Data can not be X/Z when rvalid and rready are all high.
    property propertyRdataUnknow;
        @ (rchn)
        disable iff(!CHECK)
        (rchn.rvalid == 'd1 && rchn.rready == 'd1) |-> !$isunknown(rchn.rdata);
    endproperty

    // Read Response can not be X/Z when rvalid and rready are all high.
    property propertyRrespUnknow;
        @ (rchn)
        disable iff(!CHECK)
        (rchn.rvalid == 'd1 && rchn.rready == 'd1) |-> !$isunknown(rchn.rresp);
    endproperty

    // ----------------------------------------------------
    // Assertion Define
    // ----------------------------------------------------

    asserWaddrUnknow: assert property (propertyWaddrUnknow)
    else $error($psprintf("[%s]: Assertion fail! Awaddr contains X or Z when awvalid and awready are all 1", ifname));

    asserWdataUnknow: assert property (propertyWdataUnknow)
    else $error($psprintf("[%s]: Assertion fail! Wdata contains X or Z when wvalid and wready are all 1", ifname));

    asserBrespUnknow: assert property (propertyBrespUnknow)
    else $error($psprintf("[%s]: Assertion fail! Bresp contains X or Z when bvalid and bready are all 1", ifname));

    asserRaddrUnknow: assert property (propertyRaddrUnknow)
    else $error($psprintf("[%s]: Assertion fail! Rwaddr contains X or Z when arvalid and arready are all 1", ifname));

    asserRdataUnknow: assert property (propertyRdataUnknow)
    else $error($psprintf("[%s]: Assertion fail! Rdata contains X or Z when rvalid and rready are all 1", ifname));

    asserRrespUnknow: assert property (propertyRrespUnknow)
    else $error($psprintf("[%s]: Assertion fail! Rresp contains X or Z when rvalid and rready are all 1", ifname));
    `endif

    // ----------------------------------------------------
    // Task Define
    // ----------------------------------------------------
 
    // Innitial Bus to default value(Used for axil_master)
    task initial_bus_master();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Initial AW Channel
        awchn.awaddr  <= 'd0;
        awchn.awprot  <= 'd0;
        awchn.awvalid <= 'd0;
        // Initial AR Channel
        archn.araddr  <= 'd0;
        archn.arprot  <= 'd0;
        archn.arvalid <= 'd0;
        // Initial W Channel
        wchn.wdata    <= 'd0;
        wchn.wstrb    <= 'd0;
        wchn.wvalid   <= 'd0;
        // Initial R Channel
        rchn.rready   <= 'd0;
        // Initial B Channel
        bchn.bready   <= 'd0;
        @ (awchn);
    `else
        // Initial AW Channel
        awaddr  <= 'd0;
        awprot  <= 'd0;
        awvalid <= 'd0;
        // Initial AR Channel
        araddr  <= 'd0;
        arprot  <= 'd0;
        arvalid <= 'd0;
        // Initial W Channel
        wdata   <= 'd0;
        wstrb   <= 'd0;
        wvalid  <= 'd0;
        // Initial R Channel
        rready  <= 'd0;
        // Initial B Channel
        bready  <= 'd0;
        @ (posedge aclk);
    `endif
    endtask : initial_bus_master

    // Innitial Bus to default value(Used for axil_slave)
    task initial_bus_slave();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Initial AW Channel
        awchn.awready <= 'd0;
        // Initial AR Channel
        archn.arready <= 'd0;
        // Initial W Channel
        wchn.wready   <= 'd0;
        // Initial R Channel
        rchn.rdata    <= 'd0;
        rchn.rresp    <= 'd0;
        rchn.rvalid   <= 'd0;
        // Initial B Channel
        bchn.bresp    <= 'd0;
        bchn.bvalid   <= 'd0;
        @ (awchn);
    `else
        // Initial AW Channel
        awready <= 'd0;
        // Initial AR Channel
        arready <= 'd0;
        // Initial W Channel
        wready  <= 'd0;
        // Initial R Channel
        rdata   <= 'd0;
        rresp   <= 'd0;
        rvalid  <= 'd0;
        // Initial B Channel
        bresp   <= 'd0;
        bvalid  <= 'd0;
        @ (posedge aclk);
    `endif
    endtask : initial_bus_slave

    // Wait a cycle
    task wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (awchn);
    `else
        repeat (num) @ (posedge aclk);
    `endif
    endtask : wait_clock

    // There are some bugs which may cause the core dump if using vivado 
    // simulator. So these tasks will not be used with vivado simulator.

    // AXI4-lite Master Function begin: {{{
 
    // Transmit write address(Transmit a address to AW channel, do not support
    // back to back mode)

    task transmit_waddr(input ADDR_t addr);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        awchn.awvalid <= 'd1 ;
        awchn.awaddr  <= addr;
        if (awchn.awready == 'd1) begin
            @ (awchn);
        end else begin
            while (1) begin
                if (awchn.awready == 'd1) break;
                @ (awchn);
            end
        end
        awchn.awvalid <= 'd0 ;
    `endif
    endtask : transmit_waddr

    // Transmit read address(Transmit a address to AR channel, do not support
    // back to back mode)

    task transmit_raddr(input ADDR_t addr);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        archn.arvalid <= 'd1 ;
        archn.araddr  <= addr;
        if (archn.arready == 'd1) begin
            @ (archn);
        end else begin
            while (1) begin
                if (archn.arready == 'd1) break;
                @ (archn);
            end
        end
        archn.arvalid <= 'd0 ;
    `endif
    endtask : transmit_raddr

    // Transmit write data(Transmit data to W channel, do not support
    // back to back mode)

    task transmit_wdata(input DATA_t data, 
                        input STRB_t strb);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert wvalid signal
        wchn.wvalid <= 'd1 ;
        wchn.wdata  <= data;
        wchn.wstrb  <= strb;
        // Hold the valid, data, keep and last when ready goes down
        if (wchn.wready == 'd1) begin
            @ (wchn);
        end else begin
            while (1) begin
                if (wchn.wready == 'd1)  break;
                @ (wchn);
            end
        end
        // Deasset the valid signal(Do not support back to back transpot)
        wchn.wvalid <= 'd0;
    `endif
    endtask : transmit_wdata

    // Collect read data(Read data from R channel, do not support
    // back to back mode)

    task collect_rdata(output DATA_t data, 
                       output RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Using '==' to avoid the x/z problem
        while (1) begin
            if (rchn.rvalid == 'd1) break;
            @ (rchn);
        end
        if (rchn.rready != 'd1) begin
            // Assert ready when valid is high
            rchn.rready <= 'd1;
            @ (rchn);
        end
        // Get data when valid and ready are both active
        data = rchn.rdata;
        resp = rchn.rresp;
        @ (rchn);
        rchn.rready <= 'd0;
    `endif
    endtask : collect_rdata

    // Collect write response(Collect response from B channel, do not support
    // back to back mode)

    task collect_bresp(output RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        bchn.bready <= 'd1;
        while (1) begin
            if (bchn.bvalid == 'd1) break;
            @ (bchn);
        end
        resp = bchn.bresp;
        @ (bchn);
        bchn.bready <= 'd0;
    `endif
    endtask : collect_bresp

    // AXI4-lite Master Function end: }}}
    
    // AXI4-lite Slave Function begin: {{{

    // Colelct write address(Collect a address to AW channel, do not support
    // back to back mode)

    task collect_waddr(output ADDR_t addr);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        awchn.awready <= 'd1;
        // Wait valid signal
        while (1) begin
            if (awchn.awvalid == 'd1) break;
            @ (awchn);
        end
        addr = awchn.awaddr;
        @ (awchn);
        awchn.awready <= 'd0;
    `endif
    endtask : collect_waddr

    // Colelct read address(Collect a address to AR channel, do not support
    // back to back mode)

    task collect_raddr(output ADDR_t addr);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        archn.arready <= 'd1;
        // Wait valid signal
        while (1) begin
            if (archn.arvalid == 'd1) break;
            @ (archn);
        end
        addr = archn.araddr;
        @ (archn);
        archn.arready <= 'd0;
    `endif
    endtask : collect_raddr

    // Collect write data(Read data from W channel, do not support
    // back to back mode)

    task collect_wdata(output DATA_t data, 
                       output STRB_t strb);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        while (1) begin
            if (wchn.wvalid == 'd1) break;
            @ (wchn);
        end
        if (wchn.wready != 'd1) begin
            // Assert ready when valid is high
            wchn.wready <= 'd1;
            @ (wchn);
        end
        // Get data when valid and ready are both active
        data = wchn.wdata;
        strb = wchn.wstrb;
        @ (wchn);
        wchn.wready <= 'd0;
    `endif
    endtask : collect_wdata

    // Transmit read data(Transmit data to R channel, do not support
    // back to back mode)

    task transmit_rdata(input DATA_t data, 
                        input RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert rvalid signal
        rchn.rvalid <= 'd1;
        rchn.rdata  <= data;
        rchn.rresp  <= resp;
        // Hold the valid, data, keep and last when ready goes down
        if (rchn.rready == 'd1) begin
            @ (rchn);
        end else begin
            while (1) begin
                if (rchn.rready == 'd1) break;
                @ (rchn);
            end
        end
        // Deasset the valid signal(Do not support back to back transpot)
        rchn.rvalid <= 'd0;
    `endif
    endtask : transmit_rdata

    // Transmit read resp(Transmit resp to R channel, do not support
    // back to back mode)

    task transmit_bresp(input RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert rvalid signal
        bchn.bvalid <= 'd1;
        bchn.bresp  <= resp;
        // Hold the valid, data, keep and last when ready goes down
        if (bchn.bready == 'd1) begin
            @ (bchn);
        end else begin
            while (1) begin
                if (bchn.bready == 'd1) break;
                @ (bchn);
            end
        end
        bchn.bvalid <= 'd0;
    `endif
    endtask : transmit_bresp

    // AXI4-lite Slave Function end: }}}

endinterface : axil_interface // }}}

`endif // _AXIL_INTERFACE_SV_

