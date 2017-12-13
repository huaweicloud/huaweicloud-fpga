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


`ifndef _AXIL_SLAVE_BFM_SV_
`define _AXIL_SLAVE_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

class axil_slave_bfm #(type REQ    = axi_data,
                       type RSP    = REQ, 
                       type COV    = axi_cov, 
                       int  AWIDTH = `AXI4L_ADDR_WIDTH,
                       int  DWIDTH = `AXI4L_DATA_WIDTH,
                       int  SWIDTH = `AXI4L_STRB_WIDTH);

    //----------------------------------
    // Typedef for bfm
    //----------------------------------

    typedef bit [AWIDTH - 'd1 : 0] ADDR_t;
    typedef bit [DWIDTH - 'd1 : 0] DATA_t;
    typedef bit [SWIDTH - 'd1 : 0] STRB_t;
    typedef bit ['d1          : 0] RESP_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------

    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    protected semaphore      m_aw_lock;
    protected semaphore      m_w_lock;
    protected semaphore      m_b_lock;

    protected semaphore      m_ar_lock;
    protected semaphore      m_r_lock;

    protected ADDR_t         m_wr_addr[$];
    protected ADDR_t         m_rd_addr[$];

    protected DATA_t         m_wr_data[$];
    protected STRB_t         m_wr_strb[$];
    protected DATA_t         m_rd_data[$];

    protected COV            m_axi_cov;

    virtual   axil_interface m_axil_vif;

    local     bit            m_start;
    local     bit            m_stop;

    protected string         m_bfm_name;

    //----------------------------------
    // Covergroup for slave
    //----------------------------------

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    covergroup cg_axi_a_channel;
        // Single Coverpoint
        AXI_OPT_TYPE    : coverpoint m_axi_cov.opt {
            bins ADDR_WRITE     = {e_AXI_OPT_WR};
            bins ADDR_READ      = {e_AXI_OPT_RD};
        }
        AXI_ID          : coverpoint m_axi_cov.id {
            bins ADDR_ID[]      = {[0:'hf]};
        }
        AXI_ADDR        : coverpoint m_axi_cov.addr {
            bins ADDR_ZERO      = {0};
            bins ADDR_NON_ZERO[]= {[1:8'hff]};
        }

        // Cross coverpoint
        cross AXI_OPT_TYPE, AXI_ADDR       ;
        cross AXI_OPT_TYPE, AXI_ID         ;
    endgroup : cg_axi_a_channel

    covergroup cg_axi_w_channel;
        // Single Coverpoint
        AXI_W_STRB      : coverpoint m_axi_cov.strb {
            bins WRITE_STROBE[] = {[0 : SWIDTH]};
        }
    endgroup : cg_axi_w_channel
    `endif

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string name = "axil_slave_bfm");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    extern function void sample(COV        cov, 
                                axi_chan_t chan);
    `endif

    //
    // Set the handle of the m_req_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_reqmlbx(ref mailbox #(REQ) req_mlbx);

    // 
    // Set the handle of the m_rsp_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_rspmlbx(ref mailbox #(RSP) rsp_mlbx);

    extern task wait_clock(input int num = 'd1);
    
    // Using a wrapper task to avoid of the core dump when using vivado simulator
    // I figure that vivado simulator does not support invoking function/task in interface
    // Wrapper task defiene start {{{

    extern task wrapper_collect_waddr(output ADDR_t addr);
    extern task wrapper_collect_raddr(output ADDR_t addr);
    extern task wrapper_collect_wdata(output DATA_t data, 
                                      output STRB_t strb);
    extern task wrapper_transmit_rdata(input DATA_t data, 
                                       input RESP_t resp);
    extern task wrapper_transmit_bresp(input RESP_t resp);

    // Wrapper task define end }}}

    extern task collect();

    extern task collect_write();

    extern task collect_read();

    extern task collect_waddr();

    extern task collect_raddr();

    extern task collect_wdata();

    extern task transmit_rdata();

    extern task transmit_rsp();

endclass : axil_slave_bfm

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axil_slave_bfm
//
//----------------------------------------------------------------------------------------------------------------------

function axil_slave_bfm::new(string name = "axil_slave_bfm");
    m_bfm_name= name ;
    m_start   = 'd0  ;
    m_stop    = 'd0  ;
    m_aw_lock = new();
    m_ar_lock = new();
    m_w_lock  = new();
    m_r_lock  = new();
    m_b_lock  = new();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_a_channel = new();
    cg_axi_w_channel = new();
    `endif

    m_wr_addr.delete();
    m_rd_addr.delete();
    m_wr_data.delete();
    m_rd_data.delete();
    m_wr_strb.delete();
endfunction : new

task axil_slave_bfm::run();
    fork
        begin
            wait (m_start == 'd1);
            main();
        end
        begin
            wait (m_start == 'd1);
            wait (m_stop  == 'd1);
            m_start = 'd0;
            disable fork;
        end
    join_none
endtask : run

task axil_slave_bfm::start();
    m_start = 'd1;
    m_axil_vif.initial_bus_slave();
    // Fork the main task
    fork
        run();
    join_none
endtask : start

task axil_slave_bfm::stop();
    m_stop  = 'd1;
    wait (m_start == 'd0);
    m_axil_vif.initial_bus_slave();
endtask : stop

task axil_slave_bfm::main();
    fork
        collect();
        collect_write();
        collect_read();
        collect_waddr();
        collect_raddr();
        collect_wdata();
        transmit_rdata();
        transmit_rsp();
    join_none
endtask : main

// Function coverage was not supportted by vivado xsim
`ifndef VIVADO
function void axil_slave_bfm::sample(COV        cov,
                                     axi_chan_t chan);
    m_axi_cov = cov;
    case (chan)
        e_AXI_AW_CHANNEL, e_AXI_AR_CHANNEL: cg_axi_a_channel.sample();
        e_AXI_W_CHANNEL:                    cg_axi_w_channel.sample();
    endcase
endfunction : sample
`endif

function void axil_slave_bfm::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void axil_slave_bfm::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

task axil_slave_bfm::wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (m_axil_vif.awchn);
    `else
        repeat (num) @ (posedge m_axil_vif.aclk);
    `endif
endtask : wait_clock

task axil_slave_bfm::wrapper_collect_waddr(output ADDR_t addr);
`ifdef VIVADO
    // Assert ready
    m_axil_vif.awready <= 'd1;
    // Wait valid signal
    while (1) begin
        if (m_axil_vif.awvalid == 'd1) break;
        @ (posedge m_axil_vif.aclk);
    end
    addr = m_axil_vif.awaddr;
    @ (posedge m_axil_vif.aclk);
    m_axil_vif.awready <= 'd0;
`else
    m_axil_vif.collect_waddr(addr);
`endif
endtask : wrapper_collect_waddr

task axil_slave_bfm::wrapper_collect_raddr(output ADDR_t addr);
`ifdef VIVADO
    // Assert ready
    m_axil_vif.arready <= 'd1;
    // Wait valid signal
    while (1) begin
        if (m_axil_vif.arvalid == 'd1) break;
        @ (posedge m_axil_vif.aclk);
    end
    addr = m_axil_vif.araddr;
    @ (posedge m_axil_vif.aclk);
    m_axil_vif.arready <= 'd0;
`else
    m_axil_vif.collect_raddr(addr);
`endif
endtask : wrapper_collect_raddr

task axil_slave_bfm::wrapper_collect_wdata(output DATA_t data, 
                                           output STRB_t strb);
`ifdef VIVADO
    while (1) begin
        if (m_axil_vif.wvalid == 'd1) break;
        @ (posedge m_axil_vif.aclk);
    end
    if (m_axil_vif.wready != 'd1) begin
        // Assert ready when valid is high
        m_axil_vif.wready <= 'd1;
        @ (posedge m_axil_vif.aclk);
    end
    // Get data when valid and ready are both active
    data = m_axil_vif.wdata;
    strb = m_axil_vif.wstrb;
    @ (posedge m_axil_vif.aclk);
    m_axil_vif.wready <= 'd0;
`else
    m_axil_vif.collect_wdata(data, strb);
`endif
endtask : wrapper_collect_wdata

task axil_slave_bfm::wrapper_transmit_rdata(input DATA_t data,
                                            input RESP_t resp);
`ifdef VIVADO
    // Assert rvalid signal
    m_axil_vif.rvalid <= 'd1;
    m_axil_vif.rdata  <= data;
    m_axil_vif.rresp  <= resp;
    // Hold the valid, data, keep and last when ready goes down
    if (m_axil_vif.rready == 'd1) begin
        @ (posedge m_axil_vif.aclk);
    end else begin
        while (1) begin
            if (m_axil_vif.rready == 'd1) break;
            @ (posedge m_axil_vif.aclk);
        end
    end
    // Deasset the valid signal(Do not support back to back transpot)
    m_axil_vif.rvalid <= 'd0;
`else
    m_axil_vif.transmit_rdata(data, resp);
`endif
endtask : wrapper_transmit_rdata

task axil_slave_bfm::wrapper_transmit_bresp(input RESP_t resp);
`ifdef VIVADO
    // Assert bvalid signal
    m_axil_vif.bvalid <= 'd1;
    m_axil_vif.bresp  <= resp;
    // Hold the valid, data, keep and last when ready goes down
    if (m_axil_vif.bready == 'd1) begin
        @ (posedge m_axil_vif.aclk);
    end else begin
        while (1) begin
            if (m_axil_vif.bready == 'd1) break;
            @ (posedge m_axil_vif.aclk);
        end
    end
    m_axil_vif.bvalid <= 'd0;
`else
    m_axil_vif.transmit_bresp(resp);
`endif
endtask : wrapper_transmit_bresp

task axil_slave_bfm::collect();
    REQ    req ;
    DATA_t data;
    forever begin
        m_req_mlbx.peek(req);
        if (req != null) begin
            data = {>>8{req.data}};
            m_rd_data.push_back(data);
            m_r_lock.put();
        end
        m_req_mlbx.get(req);
    end
endtask : collect

//  .   .   .   1       2       3       4       5       6       7       8       9      10      11      12  
//          +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   
// clk      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |      SYSTEM
//          +   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +----
// ==========================================================================================================
// Address Write Channel
// ==========================================================================================================
//                  +---------------+               +-------+       +-------+       +-------+
// avalid           |               |               |       |       |       |       |       |                  M->S
//          --------+               +---------------+       +-------+       +-------+       +----------------
//                          +-------------------------------+       +-------+       +-------+                
// aready                   |                               |       |       |       |       |                  S->M
//          ----------------+                               +-------+       +-------+       +----------------
//          --------- -------------- --------------- ------- ------- ------- --------------- ----------------
// addr              X     ADDR0    X     XXXXXX    X ADDR1 X       X ADDR2 X     ADDR3     X                  M->S
//          --------- -------------- --------------- ------- ------- ------- --------------- ----------------
// ==========================================================================================================
// Data Write Channel
// ==========================================================================================================
//                          +-------+       +-------------------------------+               +-------+
// dvalid                   |       |       |                               |               |       |          M->S
//          ----------------+       +-------+                               +---------------+       +--------
//                          +-------+       +---------------+       +-------+               +-------+
// dready                   |       |       |               |       |       |               |       |          S->M
//          ----------------+       +-------+               +-------+       +---------------+       +--------
//          --------- -------------- ----------------------- --------------- --------------- ------- --------
// data              X     DATA0    X         DATA1         X     DATA2     X               X DATA3 X          M->S
//          --------- -------------- ----------------------- --------------- --------------- ------- --------
//          --------- -------------------------------------- --------------- --------------------------------
// strb              X     'hffff                           X     STRB      X            'hffff                M->S
//          --------- -------------------------------------- --------------- --------------------------------
// ==========================================================================================================
// Write Response Channel
// ==========================================================================================================
//                                  +-------+               +-------------------------------+       +--------                       
// bvalid                           |       |               |                               |       |          S->M
//          ------------------------+       +---------------+                               +-------+        
//                                  +---------------+               +-------+       +------------------------                       
// bready                           |               |               |       |       |                          M->S
//          ------------------------+               +---------------+       +-------+                        
//          ------------------------ --------------- ------- ------------------------------- ------- --------
// brsp                             X      OKAY     X       X             OKAY              X       X  OKAY    S->M
//          ------------------------ --------------- ------- ------------------------------- ------- --------

// AXI3 Proctol need wvalid is later than awvalid & awready, but in AXI4
// it is not needed. wvalid can be earlier than awvalid.
// ---------------------------------------
// STEP1/2: Write Address Channel
// ---------------------------------------
// ---------------------------------------
// STEP1/2: Write Data Channel
// --------------------------------------
// It doesn't matter in AXI-3/AXI-4, bvalid must be later than AW and
// W channel complete.
// ---------------------------------------
// STEP3: Read write response
// ---------------------------------------

task axil_slave_bfm::collect_write();
    ADDR_t addr;
    DATA_t data;
    STRB_t strb;
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        // Write response must be later than address write and data write
        m_aw_lock.get();
        m_w_lock.get();
        addr = m_wr_addr.pop_front();
        data = m_wr_data.pop_front();
        strb = m_wr_strb.pop_front();
        rsp.opt         = e_AXI_OPT_WR;
        rsp.addr        = addr;
        {>>8{rsp.data}} = data;
        m_rsp_mlbx.put(rsp);
        m_b_lock.put();
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.addr = addr;
        cov.blen = 'd1 ;
        cov.strb = strb;
        // If coverage enable, collect AW and W channel coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) begin
                sample(cov, e_AXI_AW_CHANNEL);
                sample(cov, e_AXI_W_CHANNEL );
            end
        `endif
    end
endtask : collect_write

//  .   .   .   1       2       3       4       5       6       7       8       9      10      11      12  
//          +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   
// clk      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |      SYSTEM
//          +   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +----
// ==========================================================================================================
// Address Read Channel
// ==========================================================================================================
//                  +---------------+                                       +-------+
// avalid           |               |                                       |       |                          M->S
//          --------+               +---------------------------------------+       +------------------------
//                          +-------------------------------------------------------+                                       
// aready                   |                                                       |                          S->M
//          ----------------+                                                       +------------------------
//          --------- -------------- --------------------------------------- ------- ------------------------
// addr              X     ADDR0    X                    XXXXXX             X ADDR1 X                          M->S
//          --------- -------------- --------------------------------------- ------- ------------------------
// ==========================================================================================================
// Data Read Channel
// ==========================================================================================================
//                          +-------+       +-------------------------------+       +------------------------
// dvalid                   |       |       |                               |       |                          S->M
//          ----------------+       +-------+                               +-------+
//                          +-------+       +---------------+       +-------+       +------------------------
// dready                   |       |       |               |       |       |       |                          M->S
//          ----------------+       +-------+               +-------+       +-------+
//          --------- -------------- --------------- ------- --------------- ------- ------- ------- --------
// data              X     DATA0    X      DATA1    X DATA2 X     DATA3     X       X DATA0'X DATA1'X  DATA2'  S->M
//          --------- -------------- --------------- ------- --------------- ------- ------- ------- --------
//          ------------------------------------------------ --------------- --------------------------------
// rsp                                                      X      OK       X                                  S->M
//          ------------------------------------------------ --------------- --------------------------------

// ---------------------------------------
// STEP1: Read Address
// ---------------------------------------
// ---------------------------------------
// STEP2: Read Data
// ---------------------------------------

task axil_slave_bfm::collect_read();
    ADDR_t addr;
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        // Read response must be later than address read
        m_ar_lock.get();
        addr     = m_rd_addr.pop_front();
        rsp.opt  = e_AXI_OPT_RD;
        rsp.addr = addr;
        m_rsp_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_RD;
        cov.addr = addr;
        cov.blen = 'd1 ;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_AR_CHANNEL);
        `endif
    end
endtask : collect_read

task axil_slave_bfm::collect_waddr();
    ADDR_t addr;
    forever begin
        wait_clock();
        // Collect waddr and burst length from waddr bus
        wrapper_collect_waddr(addr);
        // Push addr and burst length to queue
        m_wr_addr.push_back(addr);
        // Add AW lock
        m_aw_lock.put();
    end
endtask : collect_waddr

task axil_slave_bfm::collect_raddr();
    ADDR_t addr;
    forever begin
        wait_clock();
        // Collect raddr and burst length from raddr bus
        wrapper_collect_raddr(addr);
        // Push addr and burst length to queue
        m_rd_addr.push_back(addr);
        // Add AR lock
        m_ar_lock.put();
    end
endtask : collect_raddr

task axil_slave_bfm::collect_wdata();
    DATA_t data;
    STRB_t strb;
    forever begin
        wait_clock();
        // Collect rdata from rdata bus
        wrapper_collect_wdata(data, strb);
        // Push data and strobe to queue
        m_wr_data.push_back(data);
        m_wr_strb.push_back(strb);
        // Add WR lock
        m_w_lock.put();
    end
endtask: collect_wdata

task axil_slave_bfm::transmit_rdata();
    DATA_t data; 
    RESP_t resp;
    forever begin
        wait_clock();
        // Get RD lock
        if (!m_r_lock.try_get()) continue;
        data = m_rd_data.pop_front();
        // resp = m_rd_resp.pop_front();
        // Invoke task in axil_interface to transmit data to bus
        wrapper_transmit_rdata(data, resp);
    end
endtask: transmit_rdata

task axil_slave_bfm::transmit_rsp();
    RESP_t resp;
    forever begin
        wait_clock();
        // Wait address write and data write finish
        if (!m_b_lock.try_get()) continue;
        // resp = m_b_resp.pop_front();
        // Invoke task in axi_interface to response bresp to bus
        wrapper_transmit_bresp(resp);
    end
endtask : transmit_rsp

`endif // _AXIL_SLAVE_BFM_SV_

