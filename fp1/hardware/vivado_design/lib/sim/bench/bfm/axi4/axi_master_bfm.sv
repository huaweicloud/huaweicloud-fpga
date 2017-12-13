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


`ifndef _AXI_MASTER_BFM_SV_
`define _AXI_MASTER_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

//------------------------------------------------------------------------------
//
// CLASS: axi_master_bfm
//
// The axi_master_bfm class is the bfm for axi4 master. This bfm contains 
// a driver and monitor. It is suitable for AXI4 Proctol(Do not support AXI3).
// No full feature of AXI4 will be supported, such as out of order, narrow
// mode, unalign mode, back to back transport an so on. If more verification
// features are required, please contact us for power pack or AXI4 VIP.
//
//------------------------------------------------------------------------------

class axi_master_bfm #(type REQ    = axi_data, 
                       type RSP    = REQ,
                       type COV    = axi_cov, 
                       int  AWIDTH = `AXI4_ADDR_WIDTH, 
                       int  DWIDTH = `AXI4_DATA_WIDTH, 
                       int  SWIDTH = `AXI4_STRB_WIDTH);

    //----------------------------------
    // Parameter Define
    //----------------------------------

    parameter DBYTES = DWIDTH >> 3;

    //----------------------------------
    // Usertype define
    //----------------------------------

    typedef bit [AWIDTH - 'd1 : 0] ADDR_t ;
    typedef bit [DWIDTH - 'd1 : 0] DATA_t ;
    typedef bit [SWIDTH - 'd1 : 0] STRB_t ;
    typedef bit ['d1          : 0] RESP_t ;
    typedef axi_opt_t              OPT_t  ; 

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
    protected int            m_wr_blen[$] ;
    protected int            m_rd_blen[$] ;

    protected DATA_t         m_wr_data[$][];
    protected STRB_t         m_wr_strb[$];
    protected DATA_t         m_rd_data[$][];
    protected STRB_t         m_rd_strb[$];

    protected COV            m_axi_cov;

    virtual   axi_interface  m_axi_vif;

    local     bit            m_start;
    local     bit            m_stop;

    protected string         m_bfm_name;

    //----------------------------------
    // Covergroup for master
    //----------------------------------

`ifndef VIVADO
    covergroup cg_axi_r_channel;
        // Single Coverpoint
        AXI_R_STRB      : coverpoint m_axi_cov.strb {
            bins READ_STROBE[]    = {[0 : SWIDTH]};
        }
        AXI_R_LMATCH    : coverpoint m_axi_cov.lmatch {
            bins READ_LMATCH      = {'d1};
            bins READ_LNOMATCH    = {'d0};
        }
        AXI_R_RESP      : coverpoint m_axi_cov.resp {
            bins READ_RESP_OKAY   = {e_AXI_RESP_OKAY  };
            bins READ_RESP_EXOKAY = {e_AXI_RESP_EXOKAY};
            bins READ_RESP_SLVERR = {e_AXI_RESP_SLVERR};
            bins READ_RESP_DECERR = {e_AXI_RESP_DECERR};
        }
    endgroup : cg_axi_r_channel

    covergroup cg_axi_b_channel;
        // Single Coverpoint
        AXI_B_RESP      : coverpoint m_axi_cov.resp {
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
 
    extern function new(string name = "axi_master_bfm");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

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

    // 
    // Desmb data(Extract data, address, burstlen from the axi_data)
    //

    extern function int desmb_data(const ref REQ    req, 
                                   output    OPT_t  opt,
                                   output    ADDR_t addr, 
                                   ref       DATA_t data[],
                                   output    STRB_t strb);

    // 
    // Asmb data(Combine data, address, burstlen to axi_data)
    //

    extern function int asmb_data(ref       RSP    rsp, 
                                  input     OPT_t  opt,
                                  input     ADDR_t addr, 
                                  const ref DATA_t data[],
                                  input     STRB_t strb);

    // 
    // Transmit data to bus(Write and read operation supportted)
    //
    
    extern task transmit();

    // 
    // Transmit write
    //
    
    extern task transmit_write();

    // 
    // Transmit read
    //
    
    extern task transmit_read();

    // 
    // Address write
    //
    
    extern task transmit_waddr();

    // 
    // Address read
    //
    
    extern task transmit_raddr();

    // 
    // Data write
    //
    
    extern task transmit_wdata();
    
    // 
    // Read rdata
    //
 
    extern task collect_rdata();

    // 
    // Read Write Response
    //
 
    extern task collect_bresp();

endclass : axi_master_bfm

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- axi_master_bfm
//
//------------------------------------------------------------------------------

function axi_master_bfm::new(string name = "axi_master_bfm");
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

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_r_channel = new();
    cg_axi_b_channel = new();
    `endif

    m_wr_addr.delete();
    m_rd_addr.delete();
    m_wr_blen.delete();
    m_rd_blen.delete();
    m_wr_data.delete();
    m_rd_data.delete();
    m_wr_strb.delete();
    m_rd_strb.delete();
endfunction : new

task axi_master_bfm::run();
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

task axi_master_bfm::start();
    m_start = 'd1;
    m_axi_vif.initial_bus_master();
    // Fork the main task
    fork
        run();
    join_none
endtask: start

task axi_master_bfm::stop();
    m_stop = 'd1;
    wait (m_start == 'd0);
    m_axi_vif.initial_bus_master();
endtask: stop

task axi_master_bfm::main();
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
endtask: main

`ifndef VIVADO
function void axi_master_bfm::sample(COV        cov, 
                                     axi_chan_t chan);
    m_axi_cov = cov;
    case (chan)
        e_AXI_R_CHANNEL: cg_axi_r_channel.sample();
        e_AXI_B_CHANNEL: cg_axi_b_channel.sample();
    endcase
endfunction : sample
`endif

function void axi_master_bfm::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void axi_master_bfm::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

function int axi_master_bfm::desmb_data(const ref REQ    req, 
                                        output    OPT_t  opt,
                                        output    ADDR_t addr, 
                                        ref       DATA_t data[],
                                        output    STRB_t strb);
    int len    = req.data.size();
    // Caculate how many slices of the data
    desmb_data = (len + DBYTES - 'd1) / DBYTES;
    addr       = req.addr;
    opt        = req.opt;
    // Caculate strobe of last cycle(Strobe will be all valid when it is not
    // last cycle)
    strb       = {DBYTES{1'd1}} >> (len % DBYTES);
    if (!len) return 'd0;
    // Convert byte data stream to slices
    for (int idx = 'd0; idx < desmb_data; idx++) begin
        bit [7 : 0] byte_array[] = req.data[idx * DBYTES +: DBYTES];
        byte_array = new[DBYTES];
        data[idx] = {>>8{byte_array}};
    end
endfunction : desmb_data

function int axi_master_bfm::asmb_data(ref       RSP    rsp, 
                                       input     OPT_t  opt,
                                       input     ADDR_t addr, 
                                       const ref DATA_t data[],
                                       input     STRB_t strb);
    int blen   = data.size();
    // Caculate how many slices of the data
    rsp.addr   = addr;
    rsp.opt    = opt ;
    // If no data existed, skip data asmb
    if (!blen) return 'd0;
    // Convert byte data stream to slices
    for (int idx = 'd0; idx < blen; idx++) begin
        bit [7 : 0] byte_queue[$];
        if (idx < blen - 'd1) begin
            {>>8{byte_queue}} = data[idx];
        end else begin
            for (int pos = 'd0; pos < DBYTES; pos++) begin
                if (strb[pos] == 'd1) 
                    byte_queue.push_back(data[idx][(pos + 'd1) * DBYTES - 'd1 -: DBYTES]);
            end
        end
        rsp.data = {rsp.data, byte_queue};
    end

    asmb_data  = rsp.data.size();
endfunction : asmb_data

task axi_master_bfm::transmit();
    REQ    req;
    forever begin
        m_req_mlbx.peek(req);
        if (req != null) begin
            // Judge the operation
            case (req.opt)
                // Write operation
                e_AXI_OPT_WR : m_wr_mlbx.put(req);
                // Read operation was not supportted yet
                e_AXI_OPT_RD : m_rd_mlbx.put(req);
            endcase
        end
        m_req_mlbx.get(req);
    end
endtask: transmit

//  .   .   .   1       2       3       4       5       6       7       8       9      10      11      12  
//          +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   
// clk      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |      SYSTEM
//          +   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +----
// ==========================================================================================================
// Address Write Channel
// ==========================================================================================================
//                  +---------------+                                                       +-------+
// avalid           |               |                                                       |       |          M->S
//          --------+               +-------------------------------------------------------+       +--------
//                          +-----------------------------------------------------------------------+                                       
// aready                   |                                                                       |          S->M
//          ----------------+                                                                       +--------
//          --------- -------------- ------------------------------------------------------- ------- --------
// addr              X     ADDR0    X                    XXXXXX                             X ADDR1 X          M->S
//          --------- -------------- ------------------------------------------------------- ------- --------
//          --------- -------------- ------------------------------------------------------- ------- --------
// burstlen          X     LEN0     X                                                       X LEN1  X          M->S
//          --------- -------------- ------------------------------------------------------- ------- --------
// ==========================================================================================================
// Data Write Channel
// ==========================================================================================================
//                          +-------+       +-------------------------------+                       +--------
// dvalid                   |       |       |                               |                       |          M->S
//          ----------------+       +-------+                               +-----------------------+
//                          +-------+       +---------------+       +-------+                       +--------
// dready                   |       |       |               |       |       |                       |          S->M
//          ----------------+       +-------+               +-------+       +-----------------------+
//          --------- -------------- --------------- ------- --------------- ----------------------- --------
// data              X     DATA0    X      DATA1    X DATA2 X     DATA3     X                       X  DATA0'  M->S
//          --------- -------------- --------------- ------- --------------- ----------------------- --------
//          --------- -------------------------------------- --------------- --------------------------------
// strb              X     'hffff                           X    STROBE     X                                  M->S
//          --------- -------------------------------------- --------------- --------------------------------
//                                                          +---------------+      
// last                                                     |               |                                  M->S
//          ------------------------------------------------+               +--------------------------------
// ==========================================================================================================
// Write Response Channel
// ==========================================================================================================
//                                                                          +---------------+                                       
// rvalid                                                                   |               |                  S->M
//          ----------------------------------------------------------------+               +----------------
//                                                                                  +-------+                                       
// rready                                                                           |       |                  M->S
//          ------------------------------------------------------------------------+       +----------------
//          ---------------------------------------------------------------- --------------- ----------------
// rsp                                                                      X      OK       X                  S->M
//          ---------------------------------------------------------------- --------------- ----------------

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

task axi_master_bfm::transmit_write();
    ADDR_t addr;
    DATA_t data[];
    STRB_t strb;
    RESP_t resp;
    int    blen;
    REQ    req = new();
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        m_wr_mlbx.peek(req);
        // Write response must be later than address write and data write
        blen = desmb_data(req, rsp.opt, addr, data, strb);
        m_wr_addr.push_back(addr);
        m_wr_blen.push_back(blen);
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
        data.delete();
        void'(asmb_data(rsp, e_AXI_OPT_WR, addr, data, strb));
        rsp.resp = e_AXI_RESP_OKAY;
        m_rsp_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.strb = strb;
        cov.resp = rsp.resp;
        // If coverage enable, collect Wresp channel coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_B_CHANNEL);
        `endif
    end
endtask: transmit_write

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
//          --------- -------------- --------------------------------------- ------- ------------------------
// burstlen          X     LEN0     X                                       X LEN1  X                          M->S
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
//          --------- -------------------------------------- --------------- --------------------------------
// keep              X     'hffff                           X     KEEP      X                                  S->M
//          --------- -------------------------------------- --------------- --------------------------------
//                                                          +---------------+      
// last                                                     |               |                                  S->M
//          ------------------------------------------------+               +--------------------------------
//          ------------------------------------------------ --------------- ---------------------------------
// rsp                                                      X      OK       X                                  S->M
//          ------------------------------------------------ --------------- ---------------------------------

// ---------------------------------------
// STEP1: Read Address
// ---------------------------------------
// ---------------------------------------
// STEP2: Read Data
// ---------------------------------------

task axi_master_bfm::transmit_read();
    ADDR_t addr;
    DATA_t data[];
    STRB_t strb;
    RESP_t resp;
    int    blen;
    REQ    req = new();
    RSP    rsp = new();
    COV    cov = new();
    forever begin
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
        strb = m_rd_strb.pop_front();
        // resp = m_rd_resp.pop_front();
        void'(asmb_data(rsp, e_AXI_OPT_RD, addr, data, strb));
        rsp.resp = e_AXI_RESP_OKAY;
        m_rsp_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_RD;
        cov.resp = rsp.resp;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_R_CHANNEL);
        `endif
    end
endtask: transmit_read

task axi_master_bfm::transmit_waddr();
    ADDR_t addr; 
    int    blen;
    forever begin
        m_axi_vif.wait_clock();
        // Get AW lock
        if (!m_aw_lock.try_get()) continue;
        addr = m_wr_addr.pop_front();
        blen = m_wr_blen.pop_front();
        // Invoke task in axi_interface to transmit address and burst length to bus
        m_axi_vif.transmit_waddr(addr, blen);
    end
endtask: transmit_waddr

task axi_master_bfm::transmit_raddr();
    ADDR_t addr; 
    int    blen;
    forever begin
        m_axi_vif.wait_clock();
        // Get AR lock
        if (!m_ar_lock.try_get()) continue;
        addr = m_rd_addr.pop_front();
        blen = m_rd_blen.pop_front();
        // Invoke task in axi_interface to transmit address and burst length to bus
        m_axi_vif.transmit_raddr(addr, blen);
    end
endtask: transmit_raddr

task axi_master_bfm::transmit_wdata();
    DATA_t data[$];
    STRB_t strb;
    forever begin 
        m_axi_vif.wait_clock();
        // Get WR lock
        if (!m_w_lock.try_get()) continue;
        // Get wdata and strobe from queue
        data = m_wr_data.pop_front();
        strb = m_wr_strb.pop_front();
        // Invoke task in axi_interface to transmit data and strobe to bus
        m_axi_vif.transmit_wdata(data, strb);
    end
endtask: transmit_wdata

task axi_master_bfm::collect_rdata();
    DATA_t data[$];
    STRB_t strb;
    RESP_t resp;
    forever begin
        m_axi_vif.wait_clock();
        // Collect rdata from rdata bus
        m_axi_vif.collect_rdata(data, strb, resp);
        // Push data and strobe to queue
        m_rd_strb.push_back(strb);
        m_rd_data.push_back(data);
        // Add RD lock
        m_r_lock.put();
    end
endtask : collect_rdata

task axi_master_bfm::collect_bresp();
    RESP_t resp;
    forever begin
        m_axi_vif.wait_clock();
        // Collect bresp from bresp bus
        m_axi_vif.collect_bresp(resp);
        // Add WR Response lock
        m_b_lock.put();
    end
endtask : collect_bresp

`endif // _AXI_MASTER_BFM_SV_
