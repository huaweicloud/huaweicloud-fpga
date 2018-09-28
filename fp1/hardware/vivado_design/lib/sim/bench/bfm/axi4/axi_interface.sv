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


`ifndef _AXI_INTERFACE_SV_
`define _AXI_INTERFACE_SV_

`timescale 1ns/1ps

// ./common/common_axi.svh
`include "common_axi.svh"

interface axi_interface #(int AWIDTH = `AXI4_ADDR_WIDTH,  // Address bus width 
                          int DWIDTH = `AXI4_DATA_WIDTH,  // Data bus width
                          int SWIDTH = `AXI4_STRB_WIDTH,  // Strb bus width
                          int LWIDTH = `AXI4_LEN_WIDTH ,  // Burst len width
                          int CHECK  = 'd0,               // Assertion check enable
                          int SETUP  = 'd1,               // Setup time
                          int HOLD   = 'd0)               // Hold time
                          (input logic clk, 
                           input logic rst); // {{{

    //----------------------------------
    // Parameter Define
    //----------------------------------
    
    parameter IWIDTH = `AXI4_ID_WIDTH  ;  // ID width
    parameter RWIDTH = `AXI4_RESP_WIDTH;  // Resp width

    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef logic [AWIDTH - 'd1 : 0] ADDR_t ;
    typedef logic [DWIDTH - 'd1 : 0] DATA_t ;
    typedef logic [SWIDTH - 'd1 : 0] STRB_t ;
    typedef logic [LWIDTH - 'd1 : 0] LEN_t  ;
    typedef logic [IWIDTH - 'd1 : 0] ID_t   ;
    typedef logic [RWIDTH - 'd1 : 0] RESP_t ;
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
    // Write Address Channel
    //
    ID_t          awid;        // Address Write ID
    ADDR_t        awaddr;      // Write Address
    LEN_t         awlen;       // Burst Length
    logic [2 : 0] awsize;      // Burst Size
    logic [1 : 0] awburst;     // Burst Type
    logic [1 : 0] awlock;      // Lock Type
    logic [3 : 0] awcache;     // Cache Type
    logic [2 : 0] awprot;      // Protected Type
    logic         awvalid;     // Write Address Valid
    logic         awready;     // Write Address Ready

    //
    // Write Data Channel
    //
    ID_t          wid;         // Write ID
    DATA_t        wdata;       // Write Data
    STRB_t        wstrb;       // Write Strobe
    logic         wlast;       // Write Last
    logic         wvalid;      // Write Valid
    logic         wready;      // Write Ready

    //
    // Write Response Channel
    //
    ID_t          bid;         // Response ID
    RESP_t        bresp;       // Write Response
    logic         bvalid;      // Write Response Valid
    logic         bready;      // Response Ready

    //
    // Read Address Channel
    //
    ID_t          arid;        // Address Read ID
    ADDR_t        araddr;      // Read Address
    LEN_t         arlen;       // Burst Length
    logic [2 : 0] arsize;      // Burst Size
    logic [1 : 0] arburst;     // Burst Type
    logic [1 : 0] arlock;      // Lock Type
    logic [3 : 0] arcache;     // Cache Type
    logic [2 : 0] arprot;      // Protected Type
    logic         arvalid;     // Read Address Valid
    logic         arready;     // Read Address Ready

    //
    // Read Data Channel
    //
    ID_t          rid;         // Ready ID
    DATA_t        rdata;       // Ready Data
    STRB_t        rstrb;       // Ready Strobe
    RESP_t        rresp;       // Ready Response
    logic         rlast;       // Ready Last
    logic         rvalid;      // Ready Valid
    logic         rready;      // Ready Ready

    // Inner used
    string        ifname;

    assign aclk    = clk ;
    assign aresetn = ~rst;

    //----------------------------------
    // Clocking define
    //----------------------------------

    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
    // Address Write Channel
    clocking awchn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  awid;
        inout  awaddr;
        inout  awlen;
        inout  awsize;
        inout  awburst;
        inout  awlock;
        inout  awcache;
        inout  awprot;
        inout  awvalid;
        inout  awready;
    endclocking

    // Write Channel
    clocking wchn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  wid;
        inout  wdata;
        inout  wstrb;
        inout  wlast;
        inout  wvalid;
        inout  wready;
    endclocking

    // Write Response Channel
    clocking bchn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  bid;
        inout  bresp;
        inout  bvalid;
        inout  bready;
    endclocking

    // Address Read Channel
    clocking archn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  arid;
        inout  araddr;
        inout  arlen;
        inout  arsize;
        inout  arburst;
        inout  arlock;
        inout  arcache;
        inout  arprot;
        inout  arvalid;
        inout  arready;
    endclocking

    // Read Channel
    clocking rchn @ (posedge aclk);
        default input #(SETUP) output #(HOLD);
        inout  rid;
        inout  rdata;
        inout  rstrb;
        inout  rresp;
        inout  rlast;
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

    // Innitial Bus to default value(Used for axi_master)
    task initial_bus_master();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Initial AW Channel
        awchn.awid    <= 'd0;
        awchn.awaddr  <= 'd0;
        awchn.awlen   <= 'd0;
        awchn.awsize  <= 'd0;
        awchn.awburst <= 'd0;
        awchn.awlock  <= 'd0;
        awchn.awcache <= 'd0;
        awchn.awprot  <= 'd0;
        awchn.awvalid <= 'd0;
        // Initial AR Channel
        archn.arid    <= 'd0;
        archn.araddr  <= 'd0;
        archn.arlen   <= 'd0;
        archn.arsize  <= 'd0;
        archn.arburst <= 'd0;
        archn.arlock  <= 'd0;
        archn.arcache <= 'd0;
        archn.arprot  <= 'd0;
        archn.arvalid <= 'd0;
        // Initial W Channel
        wchn.wid      <= 'd0;
        wchn.wdata    <= 'd0;
        wchn.wstrb    <= 'd0;
        wchn.wlast    <= 'd0;
        wchn.wvalid   <= 'd0;
        // Initial R Channel
        rchn.rid      <= 'd0;
        rchn.rdata    <= 'd0;
        rchn.rstrb    <= 'd0;
        rchn.rresp    <= 'd0;
        rchn.rlast    <= 'd0;
        rchn.rvalid   <= 'd0;
        // Initial B Channel
        bchn.bid      <= 'd0;
        bchn.bresp    <= 'd0;
        bchn.bvalid   <= 'd0;
    `else
        // Initial AW Channel
        awid    <= 'd0;
        awaddr  <= 'd0;
        awlen   <= 'd0;
        awsize  <= 'd0;
        awburst <= 'd0;
        awlock  <= 'd0;
        awcache <= 'd0;
        awprot  <= 'd0;
        awvalid <= 'd0;
        // Initial AR Channel
        arid    <= 'd0;
        araddr  <= 'd0;
        arlen   <= 'd0;
        arsize  <= 'd0;
        arburst <= 'd0;
        arlock  <= 'd0;
        arcache <= 'd0;
        arprot  <= 'd0;
        arvalid <= 'd0;
        // Initial W Channel
        wid     <= 'd0;
        wdata   <= 'd0;
        wstrb   <= 'd0;
        wlast   <= 'd0;
        wvalid  <= 'd0;
        // Initial R Channel
        rid     <= 'd0;
        rdata   <= 'd0;
        rstrb   <= 'd0;
        rresp   <= 'd0;
        rlast   <= 'd0;
        rvalid  <= 'd0;
        // Initial B Channel
        bid     <= 'd0;
        bresp   <= 'd0;
        bvalid  <= 'd0;
    `endif
    endtask : initial_bus_master

    // Innitial Bus to default value(Used for axi_slave)
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
        rchn.rid      <= 'd0;
        rchn.rdata    <= 'd0;
        rchn.rstrb    <= 'd0;
        rchn.rlast    <= 'd0;
        rchn.rresp    <= 'd0;
        rchn.rvalid   <= 'd0;
        // Initial B Channel
        bchn.bid      <= 'd0;
        bchn.bresp    <= 'd0;
        bchn.bvalid   <= 'd0;
    `else
        // Initial AW Channel
        awready <= 'd0;
        // Initial AR Channel
        arready <= 'd0;
        // Initial W Channel
        wready  <= 'd0;
        // Initial R Channel
        rid     <= 'd0;
        rdata   <= 'd0;
        rstrb   <= 'd0;
        rlast   <= 'd0;
        rresp   <= 'd0;
        rvalid  <= 'd0;
        // Initial B Channel
        bid     <= 'd0;
        bresp   <= 'd0;
        bvalid  <= 'd0;
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

    // AXI4 Master Function begin: {{{
 
    // Transmit write address(Transmit a address to AW channel, do not support
    // back to back mode)

    task transmit_waddr(input ADDR_t addr, 
                        input LEN_t  len);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        awchn.awvalid <= 'd1 ;
        awchn.awaddr  <= addr;
        awchn.awlen   <= len ;
        while (awchn.awready != 'd1) begin
            @ (awchn);
        end
        @ (awchn);
        awchn.awvalid <= 'd0 ;
    `else
        awvalid <= 'd1 ;
        awaddr  <= addr;
        awlen   <= len ;
        while (awready != 'd1) begin
            @ (posedge aclk);
        end
        @ (posedge aclk);
        awvalid <= 'd0 ;
    `endif
    endtask : transmit_waddr

    // Transmit read address(Transmit a address to AR channel, do not support
    // back to back mode)

    task transmit_raddr(input ADDR_t addr, 
                        input LEN_t  len);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        archn.arvalid <= 'd1 ;
        archn.araddr  <= addr;
        archn.arlen   <= len ;
        while (archn.arready != 'd1) begin
            @ (archn);
        end
        @ (archn);
        archn.arvalid <= 'd0 ;
    `else
        arvalid <= 'd1 ;
        araddr  <= addr;
        arlen   <= len ;
        while (arready != 'd1) begin
            @ (posedge aclk);
        end
        @ (posedge aclk);
        arvalid <= 'd0 ;
    `endif
    endtask : transmit_raddr

    // Transmit write data(Transmit data to W channel, do not support
    // back to back mode)

    task transmit_wdata(input DATA2_t data[], 
                        input STRB_t  strb);
        static bit last = 'd0;
        static int blen = data.size();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert wvalid signal
        wchn.wvalid <= 'd1;
        foreach (data[idx]) begin
            last = (blen - 'd1) == idx;
            wchn.wdata  <= data[idx];
            wchn.wstrb  <= last ? strb : {SWIDTH{1'd1}};
            wchn.wlast  <= last;
            // Hold the valid, data, keep and last when ready goes down
            while (wchn.wready != 'd1) begin
                @ (wchn);
            end
            @ (wchn);
        end
        // When last is valid, deassert the valid to low
        // Deasset the valid signal(Do not support back to back transpot)
        wchn.wvalid <= 'd0;
        wchn.wlast  <= 'd0;
    `else
        // Assert wvalid signal
        wvalid <= 'd1;
        foreach (data[idx]) begin
            last = (blen - 'd1) == idx;
            wdata  <= data[idx];
            wstrb  <= last ? strb : {SWIDTH{1'd1}};
            wlast  <= last;
            // Hold the valid, data, keep and last when ready goes down
            while (wready != 'd1) begin
                @ (posedge aclk);
            end
            @ (posedge aclk);
        end
        // When last is valid, deassert the valid to low
        // Deasset the valid signal(Do not support back to back transpot)
        wvalid <= 'd0;
        wlast  <= 'd0;
    `endif
    endtask : transmit_wdata

    // Collect read data(Read data from R channel, do not support
    // back to back mode)

    task collect_rdata(output DATA2_t data[], 
                       output STRB_t  strb,
                       output RESP_t  resp);
        static DATA2_t data_queue[$];
        static int     idx = 'd0;
        data_queue.delete();
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        while (rchn.rvalid != 'd1) begin
            @ (rchn);
        end
        if (rchn.rready != 'd1) begin
            // Assert ready when valid is high
            rchn.rready <= 'd1;
            @ (rchn);
        end
        while (1) begin
            // Get data when valid and ready are both active
            data_queue.push_back(rchn.rdata);
            strb = rchn.rstrb;
            resp = rchn.rresp;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (rchn.rlast == 'd1) begin
                data = data_queue;
                rchn.rready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
            @ (rchn);
        end
        rchn.rready <= 'd0;
    `else
        while (rvalid != 'd1) begin
            @ (posedge aclk);
        end
        if (rready != 'd1) begin
            // Assert ready when valid is high
            rready <= 'd1;
            @ (posedge aclk);
        end
        while (1) begin
            // Get data when valid and ready are both active
            data_queue.push_back(rdata);
            strb = rstrb;
            resp = rresp;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (rlast == 'd1) begin
                data = data_queue;
                rready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
            @ (posedge aclk);
        end
        rready <= 'd0;
    `endif
    endtask : collect_rdata

    // Collect write response(Collect response from B channel, do not support
    // back to back mode)

    task collect_bresp(output RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        bchn.bready <= 'd1;
        while (bchn.bvalid != 'd1) begin
            @ (bchn);
        end
        resp = bchn.bresp;
        @ (bchn);
        bchn.bready <= 'd0;
    `else
        // Assert ready
        bready <= 'd1;
        while (bvalid != 'd1) begin
            @ (posedge aclk);
        end
        resp = bresp;
        @ (posedge aclk);
        bready <= 'd0;
    `endif
    endtask : collect_bresp

    // AXI4 Master Function end: }}}
    
    // AXI4 Slave Function begin: {{{

    // Colelct write address(Collect a address to AW channel, do not support
    // back to back mode)

    task collect_waddr(output ID_t   id,
                       output ADDR_t addr, 
                       output LEN_t  len);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        awchn.awready <= 'd1;
        // Wait valid signal
        while (awchn.awvalid != 'd1) begin
            @ (awchn);
        end
        id   = awchn.awid  ;
        addr = awchn.awaddr;
        len  = awchn.awlen ;
        @ (awchn);
        awchn.awready <= 'd0;
    `else
        // Assert ready
        awready <= 'd1;
        // Wait valid signal
        while (awvalid != 'd1) begin
            @ (posedge aclk);
        end
        addr = awaddr;
        len  = awlen ;
        @ (posedge aclk);
        awready <= 'd0;
    `endif
    endtask : collect_waddr

    // Colelct read address(Collect a address to AR channel, do not support
    // back to back mode)

    task collect_raddr(output ID_t   id,
                       output ADDR_t addr, 
                       output LEN_t  len);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert ready
        archn.arready <= 'd1;
        // Wait valid signal
        while (archn.arvalid != 'd1) begin
            @ (archn);
        end
        id   = archn.arid  ;
        addr = archn.araddr;
        len  = archn.arlen ;
        @ (archn);
        archn.arready <= 'd0;
    `else
        // Assert ready
        arready <= 'd1;
        // Wait valid signal
        while (arvalid != 'd1) begin
            @ (posedge aclk);
        end
        addr = araddr;
        len  = arlen ;
        @ (posedge aclk);
        arready <= 'd0;
    `endif
    endtask : collect_raddr

    // Collect write data(Read data from W channel, do not support
    // back to back mode)

    task collect_wdata(output DATA2_t data[], 
                       output STRB_t  strb);
        static DATA2_t data_queue[$];
        static int     idx = 'd0;
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        data_queue.delete();
        while (wchn.wvalid != 'd1) begin
            @ (wchn);
        end
        if (wchn.wready != 'd1) begin
            // Assert ready when valid is high
            wchn.wready <= 'd1;
            @ (wchn);
        end
        while (1) begin
            // Get data when valid and ready are both active
            data_queue.push_back(wchn.wdata);
            strb = wchn.wstrb;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (wchn.wlast == 'd1) begin
                data = data_queue;
                wchn.wready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
            @ (wchn);
        end
    `else
        while (wvalid != 'd1) begin
            @ (posedge aclk);
        end
        if (wready != 'd1) begin
            // Assert ready when valid is high
            wready <= 'd1;
            @ (posedge aclk);
        end
        while (1) begin
            // Get data when valid and ready are both active
            data_queue.push_back(wdata);
            strb = wstrb;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (wlast == 'd1) begin
                data = data_queue;
                wready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
            @ (posedge aclk);
        end
    `endif
    endtask : collect_wdata

    // Transmit read data(Transmit data to R channel, do not support
    // back to back mode)

    task transmit_rdata(input ID_t    id,
                        input DATA2_t data[], 
                        input STRB_t  strb, 
                        input RESP_t  resp);
        static bit last = 'd0;
        static int blen;
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert rvalid signal
        blen        = data.size();
        rchn.rvalid <= 'd1;
        rchn.rid    <= id ;
        foreach (data[idx]) begin
            last = (blen - 'd1) == idx;
            rchn.rdata  <= data[idx];
            rchn.rstrb  <= last ? strb : {SWIDTH{1'd1}};
            rchn.rlast  <= last;
            rchn.rresp  <= last ? resp : 'd0;
            // Hold the valid, data, keep and last when ready goes down
            if (rchn.rready == 'd1) begin
                @ (rchn);
            end else begin
                while (1) begin
                    if (rchn.rready == 'd1) break;
                    @ (rchn);
                end
            end
        end
        // When last is valid, deassert the valid to low
        // Deasset the valid signal(Do not support back to back transpot)
        rchn.rvalid <= 'd0;
        rchn.rlast  <= 'd0;
    `else
        // Assert rvalid signal
        rvalid <= 'd1;
        foreach (data[idx]) begin
            last = (blen - 'd1) == idx;
            rdata  <= data[idx];
            rstrb  <= last ? strb : {SWIDTH{1'd1}};
            rlast  <= last;
            rresp  <= last ? resp : 'd0;
            // Hold the valid, data, keep and last when ready goes down
            while (rready != 'd1) begin
                @ (posedge aclk);
            end
            @ (posedge aclk);
        end
        // When last is valid, deassert the valid to low
        // Deasset the valid signal(Do not support back to back transpot)
        rvalid <= 'd0;
        rlast  <= 'd0;
    `endif
    endtask : transmit_rdata

    // Transmit read data(Transmit data to R channel, do not support
    // back to back mode)

    task transmit_bresp(input ID_t   id,
                        input RESP_t resp);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        // Assert rvalid signal
        bchn.bvalid <= 'd1;
        bchn.bid    <= id  ;
        bchn.bresp  <= resp;
        // Hold the valid, data, keep and last when ready goes down
        while (bchn.bready != 'd1) begin
            @ (bchn);
        end
        @ (bchn);
        bchn.bvalid <= 'd0;
    `else
        // Assert rvalid signal
        bvalid <= 'd1;
        bresp  <= resp;
        // Hold the valid, data, keep and last when ready goes down
        while (bready != 'd1) begin
            @ (posedge aclk);
        end
        @ (posedge aclk);
        bvalid <= 'd0;
    `endif
    endtask : transmit_bresp

    // AXI4 Slave Function end: }}}

endinterface : axi_interface // }}}

`endif // _AXI_INTERFACE_SV_

