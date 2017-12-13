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


`ifndef _AXIL_MASTER_BFM_SV_
`define _AXIL_MASTER_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

class axil_master_bfm #(type REQ    = axi_data,
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

    local     mailbox #(REQ) m_wr_mlbx;
    local     mailbox #(REQ) m_rd_mlbx;

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

    protected REQ            m_rd_rsp;

    protected COV            m_axi_cov;

    virtual   axil_interface m_axil_vif;

    local     bit            m_start;
    local     bit            m_stop;

    protected string         m_bfm_name;

    //----------------------------------
    // Covergroup for master
    //----------------------------------

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    covergroup cg_axi_r_channel;
        // Single Coverpoint
        AXIL_R_RESP      : coverpoint m_axi_cov.resp {
            bins READ_RESP_OKAY   = {e_AXI_RESP_OKAY  };
            bins READ_RESP_EXOKAY = {e_AXI_RESP_EXOKAY};
            bins READ_RESP_SLVERR = {e_AXI_RESP_SLVERR};
            bins READ_RESP_DECERR = {e_AXI_RESP_DECERR};
        }
    endgroup : cg_axi_r_channel

    covergroup cg_axi_b_channel;
        // Single Coverpoint
        AXIL_B_RESP      : coverpoint m_axi_cov.resp {
            bins WRITE_RESP_OKAY  = {e_AXI_RESP_OKAY  };
            bins WRITE_RESP_EXOKAY= {e_AXI_RESP_EXOKAY};
            bins WRITE_RESP_SLVERR= {e_AXI_RESP_SLVERR};
            bins WRITE_RESP_DECERR= {e_AXI_RESP_DECERR};
        }
    endgroup : cg_axi_b_channel
    `endif

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string name = "axil_master_bfm");

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

    extern task wrapper_transmit_waddr(input ADDR_t addr);
    extern task wrapper_transmit_raddr(input ADDR_t addr);
    extern task wrapper_transmit_wdata(input DATA_t data, 
                                       input STRB_t strb);
    extern task wrapper_collect_rdata(output DATA_t data, 
                                      output RESP_t resp);
    extern task wrapper_collect_bresp(output RESP_t resp);

    // Wrapper task define end }}}

    extern task transmit();

    extern task transmit_write();

    extern task transmit_read();

    extern task transmit_waddr();

    extern task transmit_raddr();

    extern task transmit_wdata();

    extern task collect_rdata();

    extern task collect_bresp();

endclass : axil_master_bfm

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axil_master_bfm
//
//----------------------------------------------------------------------------------------------------------------------

function axil_master_bfm::new(string name = "axil_master_bfm");
    m_bfm_name= name ;
    m_start   = 'd0  ;
    m_stop    = 'd0  ;
    m_aw_lock = new();
    m_ar_lock = new();
    m_w_lock  = new();
    m_r_lock  = new();
    m_b_lock  = new();

    m_wr_mlbx = new(1);
    m_rd_mlbx = new(1);

    m_rd_rsp  = new();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_r_channel = new();
    cg_axi_b_channel = new();
    `endif

    m_wr_addr.delete();
    m_rd_addr.delete();
    m_wr_data.delete();
    m_rd_data.delete();
    m_wr_strb.delete();
endfunction : new

task axil_master_bfm::run();
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

task axil_master_bfm::start();
    m_start = 'd1;
    m_axil_vif.initial_bus_master();
    // Fork the main task
    fork
        run();
    join_none
endtask : start

task axil_master_bfm::stop();
    m_stop  = 'd1;
    wait (m_start == 'd0);
    m_axil_vif.initial_bus_master();
endtask : stop

task axil_master_bfm::main();
    fork
        transmit();
        transmit_write();
        transmit_read();
        transmit_waddr();
        transmit_raddr();
        transmit_wdata();
        collect_rdata();
        collect_bresp();
    join_none
endtask : main

// Function coverage was not supportted by vivado xsim
`ifndef VIVADO
function void axil_master_bfm::sample(COV        cov,
                                      axi_chan_t chan);
    m_axi_cov = cov;
    case (chan)
        e_AXI_R_CHANNEL: cg_axi_r_channel.sample();
        e_AXI_B_CHANNEL: cg_axi_b_channel.sample();
    endcase
endfunction : sample
`endif

function void axil_master_bfm::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void axil_master_bfm::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

task axil_master_bfm::wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (m_axil_vif.awchn);
    `else
        repeat (num) @ (posedge m_axil_vif.aclk);
    `endif
endtask : wait_clock

task axil_master_bfm::wrapper_transmit_waddr(input ADDR_t addr);
`ifdef VIVADO
    m_axil_vif.awvalid <= 'd1 ;
    m_axil_vif.awaddr  <= addr;
    if (m_axil_vif.awready == 'd1) begin
        @ (posedge m_axil_vif.aclk);
    end else begin
        while (1) begin
            if (m_axil_vif.awready == 'd1) break;
            @ (posedge m_axil_vif.aclk);
        end
    end
    m_axil_vif.awvalid <= 'd0 ;
`else
    m_axil_vif.transmit_waddr(addr);
`endif
endtask : wrapper_transmit_waddr

task axil_master_bfm::wrapper_transmit_raddr(input ADDR_t addr);
`ifdef VIVADO
    m_axil_vif.arvalid <= 'd1 ;
    m_axil_vif.araddr  <= addr;
    if (m_axil_vif.arready == 'd1) begin
        @ (posedge m_axil_vif.aclk);
    end else begin
        while (1) begin
            if (m_axil_vif.arready == 'd1) break;
            @ (posedge m_axil_vif.aclk);
        end
    end
    m_axil_vif.arvalid <= 'd0 ;
`else
    m_axil_vif.transmit_raddr(addr);
`endif
endtask : wrapper_transmit_raddr

task axil_master_bfm::wrapper_transmit_wdata(input DATA_t data, 
                                             input STRB_t strb);
`ifdef VIVADO
    // Assert wvalid signal
    m_axil_vif.wvalid <= 'd1 ;
    m_axil_vif.wdata  <= data;
    m_axil_vif.wstrb  <= strb;
    // Hold the valid, data, keep and last when ready goes down
    if (m_axil_vif.wready == 'd1) begin
        @ (posedge m_axil_vif.aclk);
    end else begin
        while (1) begin
            if (m_axil_vif.wready == 'd1)  break;
            @ (posedge m_axil_vif.aclk);
        end
    end
    // Deasset the valid signal(Do not support back to back transpot)
    m_axil_vif.wvalid <= 'd0;
`else
    m_axil_vif.transmit_wdata(data, strb);
`endif
endtask : wrapper_transmit_wdata

task axil_master_bfm::wrapper_collect_rdata(output DATA_t data, 
                                            output RESP_t resp);
`ifdef VIVADO
    // Using '==' to avoid the x/z problem
    while (1) begin
        if (m_axil_vif.rvalid == 'd1) break;
        @ (posedge m_axil_vif.aclk);
    end
    if (m_axil_vif.rready != 'd1) begin
        // Assert ready when valid is high
        m_axil_vif.rready <= 'd1;
        @ (posedge m_axil_vif.aclk);
    end
    // Get data when valid and ready are both active
    data = m_axil_vif.rdata;
    resp = m_axil_vif.rresp;
    @ (posedge m_axil_vif.aclk);
    m_axil_vif.rready <= 'd0;
`else
    m_axil_vif.collect_rdata(data, resp);
`endif
endtask : wrapper_collect_rdata

task axil_master_bfm::wrapper_collect_bresp(output RESP_t resp);
`ifdef VIVADO
    // Assert ready
    m_axil_vif.bready <= 'd1;
    while (1) begin
        if (m_axil_vif.bvalid == 'd1) break;
        @ (posedge m_axil_vif.aclk);
    end
    resp = m_axil_vif.bresp;
    @ (posedge m_axil_vif.aclk);
    m_axil_vif.bready <= 'd0;
`else
    m_axil_vif.collect_bresp(resp);
`endif
endtask : wrapper_collect_bresp

task axil_master_bfm::transmit();
    REQ  req;
    forever begin
        m_req_mlbx.peek(req);
        if (req != null) begin
            case (req.opt)
                // Judge the operation
                e_AXI_OPT_WR : m_wr_mlbx.put(req);
                e_AXI_OPT_RD : m_rd_mlbx.put(req);
            endcase
        end
        m_req_mlbx.get(req);
    end
endtask : transmit

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

task axil_master_bfm::transmit_write();
    ADDR_t addr;
    DATA_t data;
    STRB_t strb;
    RESP_t resp;
    REQ    req = new();
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        m_wr_mlbx.peek(req);
        // Write response must be later than address write and data write
        addr = req.addr       ;
        data = {>>8{req.data}};
        strb = {SWIDTH{1'd1}} ;
        m_wr_addr.push_back(addr);
        m_wr_data.push_back(data);
        m_wr_strb.push_back(strb);
        // Add AW lock
        m_aw_lock.put();
        // Add WR lock
        m_w_lock.put();
        m_wr_mlbx.get(req);
        // Get B lock
        m_b_lock.get();
        // resp = m_wr_resp.pop_front();
        rsp.opt  = e_AXI_OPT_RD;
        rsp.addr = addr;
        rsp.resp = e_AXI_RESP_OKAY;
        m_rsp_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.strb = strb;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_B_CHANNEL);
        `endif
    end
endtask : transmit_write

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

task axil_master_bfm::transmit_read();
    ADDR_t addr;
    DATA_t data;
    RESP_t resp;
    REQ    req = new();
    COV    cov = new();
    forever begin
        bit [7 : 0] byte_queue[$];
        // Read response must be later than address read
        m_rd_mlbx.peek(req);
        addr  = req.addr;
        m_rd_addr.push_back(addr);
        // Add AR lock
        m_ar_lock.put();
        m_rd_mlbx.get(req);
        // get R lock
        m_r_lock.get();
        data = m_rd_data.pop_front();
        // resp = m_rd_resp.pop_front();
        m_rd_rsp.opt  = e_AXI_OPT_RD;
        m_rd_rsp.addr = addr;
        m_rd_rsp.resp = e_AXI_RESP_OKAY;
        {>>8{byte_queue}} = data;
        m_rd_rsp.data = byte_queue;
        m_rsp_mlbx.put(m_rd_rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_RD;
        cov.resp = m_rd_rsp.resp;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_R_CHANNEL);
        `endif
    end
endtask : transmit_read

task axil_master_bfm::transmit_waddr();
    ADDR_t addr;
    forever begin
        wait_clock();
        // Get AW lock
        if (!m_aw_lock.try_get()) continue;
        addr = m_wr_addr.pop_front();
        // Invoke task in axi_interface to transmit address and burst length to bus
        wrapper_transmit_waddr(addr);
    end
endtask : transmit_waddr

task axil_master_bfm::transmit_raddr();
    ADDR_t addr;
    forever begin
        wait_clock();
        // Get AR lock
        if (!m_ar_lock.try_get()) continue;
        addr = m_rd_addr.pop_front();
        // Invoke task in axi_interface to transmit address and burst length to bus
        wrapper_transmit_raddr(addr);
    end
endtask : transmit_raddr

task axil_master_bfm::transmit_wdata();
    DATA_t data;
    STRB_t strb;
    forever begin
        wait_clock();
        // Get WR lock
        if (!m_w_lock.try_get()) continue;
        // Get wdata and strobe from queue
        data = m_wr_data.pop_front();
        strb = m_wr_strb.pop_front();
        // Invoke task in axi_interface to transmit data and strobe to bus
        wrapper_transmit_wdata(data, strb);
    end
endtask : transmit_wdata

task axil_master_bfm::collect_rdata();
    DATA_t data; 
    RESP_t resp;
    forever begin
        wait_clock();
        // Collect rdata from rdata bus
        wrapper_collect_rdata(data, resp);
        // Push data and strobe to queue
        m_rd_data.push_back(data);
        // m_rd_resp.push_back(resp);
        m_r_lock.put();
    end
endtask: collect_rdata

task axil_master_bfm::collect_bresp();
    RESP_t resp;
    forever begin
        wait_clock();
        // Collect bresp from bresp bus
        wrapper_collect_bresp(resp);
        // Add WR Response lock
        m_b_lock.put();
    end
endtask : collect_bresp

`endif // _AXIL_MASTER_BFM_SV_

