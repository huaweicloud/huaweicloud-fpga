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


`ifndef _AXI_SLAVE_BFM_SV_
`define _AXI_SLAVE_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

//------------------------------------------------------------------------------
//
// CLASS: axi_slave_bfm
//
// The axi_master_driver class is the bfm for axi4 slave. This bfm contains 
// a driver and monitor. It is suitable for AXI4 Proctol(Do not support AXI3).
// No full feature of AXI4 will be supported, such as out of order, narrow
// mode, unalign mode, back to back transport an so on. If more verification
// features are required, please contact us for power pack or AXI4 VIP.
//
//------------------------------------------------------------------------------

class axi_slave_bfm #(type REQ    = axi_data,
                      type RSP    = REQ, 
                      type COV    = axi_cov, 
                      int  AWIDTH = `AXI4_ADDR_WIDTH,
                      int  DWIDTH = `AXI4_DATA_WIDTH,
                      int  SWIDTH = `AXI4_STRB_WIDTH);

    //----------------------------------
    // Parameter Define
    //----------------------------------
 
    parameter DBYTES = DWIDTH >> 3;
    parameter LWIDTH = `AXI4_LEN_WIDTH;

    //----------------------------------
    // Typedef for bfm
    //----------------------------------

    typedef bit [AWIDTH - 'd1 : 0] ADDR_t;
    typedef bit [DWIDTH - 'd1 : 0] DATA_t;
    typedef bit [SWIDTH - 'd1 : 0] STRB_t;
    typedef bit [LWIDTH - 'd1 : 0] LEN_t;
    typedef bit ['d1          : 0] RESP_t;
    typedef bit ['d3          : 0] ID_t;
    typedef axi_opt_t              OPT_t ; 

    //----------------------------------
    // Varible declaration
    //----------------------------------

    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    protected mailbox #(ID_t)m_aw_lock;
    protected semaphore      m_w_lock;
    protected mailbox #(ID_t)m_b_lock;

    protected mailbox #(ID_t)m_ar_lock;
    protected mailbox #(ID_t)m_r_lock;

    protected ADDR_t         m_wr_addr[$];
    protected ADDR_t         m_rd_addr[$];
    protected int            m_wr_blen[$];
    protected int            m_rd_blen[$];

    protected DATA_t         m_wr_data[$][];
    protected STRB_t         m_wr_strb[$];
    protected DATA_t         m_rd_data[$][];
    protected STRB_t         m_rd_strb[$];

    protected COV            m_axi_cov;

    virtual   axi_interface  m_axi_vif;

    local     bit            m_start;
    local     bit            m_stop;

    bit                      m_order;

    protected string         m_inst_name;

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
        AXI_BURST_LEN   : coverpoint m_axi_cov.blen {
            bins BURST_LENGTH[] = {[1 : 'hff]};
        }
        AXI_BURST_TYPE  : coverpoint m_axi_cov.btype {
            bins BURST_FIXED    = {e_AXI_BURST_FIX };
            bins BURST_INCR     = {e_AXI_BURST_INCR};
            bins BURST_WRAP     = {e_AXI_BURST_WRAP};
        }

        // Cross coverpoint
        cross AXI_OPT_TYPE, AXI_BURST_LEN  ;
        cross AXI_OPT_TYPE, AXI_BURST_TYPE ;
        cross AXI_OPT_TYPE, AXI_ADDR       ;
        cross AXI_OPT_TYPE, AXI_ID         ;
    endgroup : cg_axi_a_channel

    covergroup cg_axi_w_channel;
        // Single Coverpoint
        AXI_W_STRB      : coverpoint m_axi_cov.strb {
            bins WRITE_STROBE[] = {[0 : SWIDTH]};
        }
        AXI_W_LMATCH    : coverpoint m_axi_cov.lmatch {
            bins WRITE_LMATCH   = {'d1};
            bins WRITE_LNOMATCH = {'d0};
        }
    endgroup : cg_axi_w_channel
    `endif

    //----------------------------------
    // Task and function declaration
    //----------------------------------
 
    extern function new(string name = "axi_slave_bfm");

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

    // 
    // Desmb data(Extract data, address, burstlen from the axi_data)
    //

    extern function int desmb_data(const ref REQ    req, 
                                   output    ID_t   id,
                                   output    OPT_t  opt,
                                   output    ADDR_t addr, 
                                   ref       DATA_t data[],
                                   output    STRB_t strb);

    // 
    // Asmb data(Combine data, address, burstlen to axi_data)
    //

    extern function int asmb_data(ref       RSP    rsp, 
                                  input     ID_t   id,
                                  input     OPT_t  opt,
                                  input     ADDR_t addr, 
                                  const ref DATA_t data[],
                                  input     STRB_t strb);


    // Using a wrapper task to avoid of the core dump when using vivado simulator
    // I figure that vivado simulator does not support invoking function/task in interface
    // Wrapper task defiene start {{{

    extern task wrapper_collect_waddr(output ID_t   id,
                                      output ADDR_t addr, 
                                      output LEN_t  len);
    extern task wrapper_collect_raddr(output ID_t   id,
                                      output ADDR_t addr, 
                                      output LEN_t  len);
    extern task wrapper_collect_wdata(output DATA_t data[], 
                                      output STRB_t strb);
    extern task wrapper_transmit_rdata(input ID_t   id,
                                       input DATA_t data[], 
                                       input STRB_t strb,
                                       input RESP_t resp);
    extern task wrapper_transmit_bresp(input ID_t   id,
                                       input RESP_t resp);

    // Wrapper task define end }}}

    extern task collect();

    extern task collect_write();

    extern task collect_read();

    extern task collect_waddr();

    extern task collect_raddr();

    extern task collect_wdata();

    extern task transmit_rdata();

    extern task transmit_bresp();

endclass : axi_slave_bfm

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axi_slave_bfm
//
//----------------------------------------------------------------------------------------------------------------------

function axi_slave_bfm::new(string name = "axi_slave_bfm");
    m_inst_name= name ;
    m_start    = 'd0  ;
    m_stop     = 'd0  ;
    m_order    = 'd1  ;
    m_aw_lock  = new();
    m_ar_lock  = new();
    m_w_lock   = new();
    m_r_lock   = new();
    m_b_lock   = new();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_a_channel = new();
    cg_axi_w_channel = new();
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

task axi_slave_bfm::run();
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

task axi_slave_bfm::start();
    m_start = 'd1;
    m_axi_vif.initial_bus_slave();
    // Fork the main task
    fork
        run();
    join_none
endtask : start

task axi_slave_bfm::stop();
    m_stop  = 'd1;
    wait (m_start == 'd0);
    m_axi_vif.initial_bus_slave();
endtask : stop

task axi_slave_bfm::main();
    fork
        collect();
        collect_write();
        collect_read();
        collect_waddr();
        collect_raddr();
        collect_wdata();
        transmit_rdata();
        transmit_bresp();
    join_none
endtask : main

// Function coverage was not supportted by vivado xsim
`ifndef VIVADO
function void axi_slave_bfm::sample(COV        cov, 
                                    axi_chan_t chan);
    m_axi_cov = cov;
    case (chan)
        e_AXI_AW_CHANNEL, e_AXI_AR_CHANNEL: cg_axi_a_channel.sample();
        e_AXI_W_CHANNEL:                    cg_axi_w_channel.sample();
    endcase
endfunction : sample
`endif

function void axi_slave_bfm::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void axi_slave_bfm::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

task axi_slave_bfm::wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (m_axi_vif.awchn);
    `else
        repeat (num) @ (posedge m_axi_vif.aclk);
    `endif
endtask : wait_clock

function int axi_slave_bfm::desmb_data(const ref REQ    req, 
                                       output    ID_t   id,
                                       output    OPT_t  opt,
                                       output    ADDR_t addr, 
                                       ref       DATA_t data[],
                                       output    STRB_t strb);
    int len    = req.data.size();
    // Caculate how many slices of the data
    desmb_data = (len + DBYTES - 'd1) / DBYTES;
    addr       = req.addr;
    opt        = req.opt;
    id         = req.id ;
    if (!len) return 'd0;
    data = new[desmb_data];

    // Convert byte data stream to slices
    for (int idx = 'd0; idx < desmb_data; idx++) begin
        DATA_t byte_w;
        int    dsize  = len < DBYTES ? len : DBYTES;
        strb          = 'd0;
        // Fucking vivado xsim
        for (int bits = 'd0; bits < dsize; bits++) begin
            if (m_order) begin
                // LSB
                byte_w[(bits << 3) +: 'd8] = req.data[idx * DBYTES + bits];
                // Caculate strobe of last cycle(Strobe will be all valid when it is not
                // last cycle)
                strb[bits]                 = 1'd1;
            end else begin
                // MSB
                byte_w[DWIDTH - 'd1 - (bits << 3) -: 'd8] = req.data[idx * DBYTES + bits];
                // Caculate strobe of last cycle(Strobe will be all valid when it is not
                // last cycle)
                strb[SWIDTH - bits - 'd1]  = 1'd1;
            end
        end
        data[idx]  = byte_w;
        len -= dsize;
    end
endfunction : desmb_data

function int axi_slave_bfm::asmb_data(ref       RSP    rsp, 
                                      input     ID_t   id,
                                      input     OPT_t  opt,
                                      input     ADDR_t addr, 
                                      const ref DATA_t data[],
                                      input     STRB_t strb);
    int blen   = data.size();
    bit [7 : 0] byte_queue[$];
    // Caculate how many slices of the data
    rsp.id     = id  ;
    rsp.addr   = addr;
    rsp.opt    = opt ;
    rsp.data.delete();
    // If no data existed, skip data asmb
    if (!blen) return 'd0;
    // Convert byte data stream to slices
    for (int idx = 'd0; idx < blen; idx++) begin
        for (int pos = 'd0; pos < DBYTES; pos++) begin
            DATA_t byte_w = data[idx];
            if (m_order) begin
                // First transmit LSB
                // If strbe is valid, push byte of data to byte_queue. Ignore
                // the strbe if not the last index.
                if (idx < blen - 'd1 || strb[pos] == 'd1) 
                    byte_queue.push_back(byte_w[(pos << 3) +: 8]);
            end else begin
                // First transmit MSB
                // If strbe is valid, push byte of data to byte_queue. Ignore
                // the strbe if not the last index.
                if (idx < blen - 'd1 || strb[SWIDTH - pos - 'd1] == 'd1) 
                    byte_queue.push_back(byte_w[DWIDTH - 'd1 - (pos << 3) -: 8]);
            end
        end
    end
    rsp.data = byte_queue;
    asmb_data  = byte_queue.size();
endfunction : asmb_data

task axi_slave_bfm::wrapper_collect_waddr(output ID_t   id,
                                          output ADDR_t addr, 
                                          output LEN_t  len);
`ifdef VIVADO
    // Assert ready
    m_axi_vif.awready <= 'd1;
    // Wait valid signal
    while (1) begin
        if (m_axi_vif.awvalid == 'd1) break;
        @ (posedge m_axi_vif.aclk);
    end
    id   = m_axi_vif.awid  ;
    addr = m_axi_vif.awaddr;
    len  = m_axi_vif.awlen ;
    @ (posedge m_axi_vif.aclk);
    m_axi_vif.awready <= 'd0;
`else
    m_axi_vif.collect_waddr(id, addr, len);
`endif
endtask : wrapper_collect_waddr

task axi_slave_bfm::wrapper_collect_raddr(output ID_t   id,
                                          output ADDR_t addr, 
                                          output LEN_t  len);
`ifdef VIVADO
    // Assert ready
    m_axi_vif.arready <= 'd1;
    // Wait valid signal
    while (1) begin
        if (m_axi_vif.arvalid == 'd1) break;
        @ (posedge m_axi_vif.aclk);
    end
    id   = m_axi_vif.arid  ;
    addr = m_axi_vif.araddr;
    len  = m_axi_vif.arlen ;
    @ (posedge m_axi_vif.aclk);
    m_axi_vif.arready <= 'd0;
`else
    m_axi_vif.collect_raddr(id, addr, len);
`endif
endtask : wrapper_collect_raddr

task axi_slave_bfm::wrapper_collect_wdata(output DATA_t data[], 
                                          output STRB_t strb);
`ifdef VIVADO
    DATA_t data_queue[$];
    int    idx = 'd0;
    while (1) begin
        if (m_axi_vif.wvalid == 'd1) break;
        @ (posedge m_axi_vif.aclk);
    end
    if (m_axi_vif.wready != 'd1) begin
        // Assert ready when valid is high
        m_axi_vif.wready <= 'd1;
        @ (posedge m_axi_vif.aclk);
    end
    while (1) begin
        // Get data when valid and ready are both active
        if (m_axi_vif.wvalid == 'd1) begin
            data_queue.push_back(m_axi_vif.wdata);
            strb = m_axi_vif.wstrb;
            // When last is valid, deassert the valid to low
            // Collect wstrb only when wlast is valid
            if (m_axi_vif.wlast == 'd1) begin
                data = data_queue;
                m_axi_vif.wready <= 'd0;
                break;
            end else if (++idx >= `AXI4_MAX_LENGTH) begin
                return;
            end
        end
        @ (posedge m_axi_vif.aclk);
    end
`else
    m_axi_vif.collect_wdata(data, strb);
`endif
endtask : wrapper_collect_wdata

task axi_slave_bfm::wrapper_transmit_rdata(input ID_t   id,
                                           input DATA_t data[],
                                           input STRB_t strb,
                                           input RESP_t resp);
    bit last = 'd0;
    int blen = data.size();
`ifdef VIVADO
    // Assert rvalid signal
    m_axi_vif.rvalid <= 'd1;
    m_axi_vif.rid    <= id ;
    foreach (data[idx]) begin
        last = (blen - 'd1) == idx;
        m_axi_vif.rdata  <= data[idx];
        m_axi_vif.rstrb  <= last ? strb : {SWIDTH{1'd1}};
        m_axi_vif.rlast  <= last;
        m_axi_vif.rresp  <= last ? resp : 'd0;
        // Hold the valid, data, keep and last when ready goes down
        if (m_axi_vif.rready == 'd1) begin
            @ (posedge m_axi_vif.aclk);
        end else begin
            while (1) begin
                if (m_axi_vif.rready == 'd1) break;
                @ (posedge m_axi_vif.aclk);
            end
        end
    end
    // Deasset the valid signal(Do not support back to back transpot)
    m_axi_vif.rvalid <= 'd0;
    m_axi_vif.rlast  <= 'd0;
`else
    m_axi_vif.transmit_rdata(id, data, strb, resp);
`endif
endtask : wrapper_transmit_rdata

task axi_slave_bfm::wrapper_transmit_bresp(input ID_t   id,
                                           input RESP_t resp);
`ifdef VIVADO
    // Assert bvalid signal
    m_axi_vif.bvalid <= 'd1;
    m_axi_vif.bid    <= id ;
    m_axi_vif.bresp  <= resp;
    // Hold the valid, data, keep and last when ready goes down
    if (m_axi_vif.bready == 'd1) begin
        @ (posedge m_axi_vif.aclk);
    end else begin
        while (1) begin
            if (m_axi_vif.bready == 'd1) break;
            @ (posedge m_axi_vif.aclk);
        end
    end
    m_axi_vif.bvalid <= 'd0;
`else
    m_axi_vif.transmit_bresp(id, resp);
`endif
endtask : wrapper_transmit_bresp

task axi_slave_bfm::collect();
    REQ    req;
    ID_t   id  ;
    ADDR_t addr;
    DATA_t data[];
    STRB_t strb;
    forever begin
        axi_opt_t opt;
        m_req_mlbx.peek(req);
        if (req != null) begin
            void'(desmb_data(req, id, opt, addr, data, strb));
            if (opt == e_AXI_OPT_RD) begin
                m_rd_data.push_back(data);
                m_rd_strb.push_back(strb);
                m_r_lock.put(id);
                data.delete();
            end
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

task axi_slave_bfm::collect_write();
    ID_t   id  ;
    ADDR_t addr;
    DATA_t data[];
    STRB_t strb;
    int    blen;
    COV    cov  = new();
    forever begin
        RSP    rsp  = new();
        // Write response must be later than address write and data write
        m_aw_lock.get(id);
        m_w_lock.get();
        addr = m_wr_addr.pop_front();
        data = m_wr_data.pop_front();
        strb = m_wr_strb.pop_front();
        blen = m_wr_blen.pop_front();  
        // Compare burstlen in AW channel and actual len in W channel
        if (blen + 'd1 != data.size()) begin
            `tb_error(m_inst_name, "[ERROR]:Burstlen in AW is not the same")
        end
        // Asmb packet
        void'(asmb_data(rsp, id, e_AXI_OPT_WR, addr, data, strb));
        m_rsp_mlbx.put(rsp);
        m_b_lock.put(id);
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.addr = addr;
        cov.blen = blen;
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

task axi_slave_bfm::collect_read();
    ID_t   id  ;
    ADDR_t addr;
    DATA_t data[];
    STRB_t strb;
    int    blen;
    COV    cov  = new();
    forever begin
        RSP    rsp  = new();
        // Read response must be later than address read
        m_ar_lock.get(id);
        addr = m_rd_addr.pop_front();
        blen = m_rd_blen.pop_front();
        data = new[blen + 'd1];
        strb = {`AXI4_STRB_WIDTH{1'd1}};
        // Asmb packet
        void'(asmb_data(rsp, id, e_AXI_OPT_RD, addr, data, strb));
        m_rsp_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_RD;
        cov.addr = addr;
        cov.blen = blen;
        // If coverage enable, collect AR channel coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov, e_AXI_AR_CHANNEL);
        `endif
    end
endtask : collect_read

task axi_slave_bfm::collect_waddr();
    ID_t   id  ;
    ADDR_t addr;
    int    blen;
    // Ignore AWID
    forever begin
        wait_clock();
        // Collect waddr and burst length from waddr bus
        wrapper_collect_waddr(id, addr, blen);
        // Push addr and burst length to queue
        m_wr_addr.push_back(addr);
        m_wr_blen.push_back(blen);
        // Add AW lock
        m_aw_lock.put(id);
    end
endtask : collect_waddr

task axi_slave_bfm::collect_raddr();
    ID_t   id  ;
    ADDR_t addr;
    int    blen;
    forever begin
        wait_clock();
        // Collect raddr and burst length from raddr bus
        wrapper_collect_raddr(id, addr, blen);
        // Push addr and burst length to queue
        m_rd_addr.push_back(addr);
        m_rd_blen.push_back(blen);
        // Add AR lock
        m_ar_lock.put(id);
    end
endtask : collect_raddr

task axi_slave_bfm::collect_wdata();
    DATA_t data[$];
    STRB_t strb;
    forever begin
        wait_clock();
        // Collect rdata from rdata bus
        wrapper_collect_wdata(data, strb);
        // Push data and strobe to queue
        m_wr_strb.push_back(strb);
        m_wr_data.push_back(data);
        // Add WR lock
        m_w_lock.put();
    end
endtask: collect_wdata

task axi_slave_bfm::transmit_rdata();
    ID_t   id  ;
    DATA_t data[$]; 
    STRB_t strb;
    RESP_t resp;
    forever begin
        wait_clock();
        // Get RD lock
        if (!m_r_lock.try_get(id)) continue;
        data = m_rd_data.pop_front();
        strb = m_rd_strb.pop_front();
        // resp = m_rd_resp.pop_front();
        // Invoke task in axi_interface to transmit data and strobe to bus
        wrapper_transmit_rdata(id, data, strb, resp);
    end
endtask: transmit_rdata

task axi_slave_bfm::transmit_bresp();
    ID_t   id  ;
    RESP_t resp;
    forever begin
        wait_clock();
        // Wait address write and data write finish
        if (!m_b_lock.try_get(id)) continue;
        // resp = m_wr_resp.pop_front();
        // Invoke task in axi_interface to response bresp to bus
        wrapper_transmit_bresp(id, resp);
    end
endtask : transmit_bresp

`endif // _AXI_SLAVE_BFM_SV_

