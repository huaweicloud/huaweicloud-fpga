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


`ifndef _AXIS_MASTER_BFM_SV_
`define _AXIS_MASTER_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

class axis_master_bfm # (type REQ    = axi_data, 
                         type RSP    = REQ,
                         type COV    = axi_cov,
                         int  DWIDTH = `AXI4S_DATA_WIDTH, 
                         int  KWIDTH = `AXI4S_KEEP_WIDTH, 
                         int  UWIDTH = `AXI4S_USER_WIDTH);

    //----------------------------------
    // Parameter Define
    //----------------------------------

    parameter DBYTES = DWIDTH >> 3;

    //----------------------------------
    // Typedef for bfm
    //----------------------------------

    typedef bit [DWIDTH - 'd1 : 0] DATA_t;
    typedef bit [KWIDTH - 'd1 : 0] KEEP_t;
    typedef bit [UWIDTH - 'd1 : 0] USER_t;

    typedef virtual axis_interface #(.DWIDTH (DWIDTH), 
                                     .KWIDTH (KWIDTH), 
                                     .UWIDTH (UWIDTH)) axis_vif_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------

    protected mailbox #(REQ) m_req_mlbx;

    local     mailbox #(REQ) m_wr_mlbx;

    protected semaphore      m_w_lock;

    protected DATA_t         m_wr_data[$][];
    protected KEEP_t         m_wr_keep[$];
    protected USER_t         m_wr_user[$];

    protected COV            m_axi_cov;

    axis_vif_t               m_axis_vif;

    local     bit            m_start;
    local     bit            m_stop;

    bit                      m_order;

    protected string         m_inst_name;

    //----------------------------------
    // Covergroup 
    //----------------------------------

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    covergroup cg_axi_s_channel;
        // Single Coverpoint
        AXI_W_KEEP      : coverpoint m_axi_cov.strb {
            bins WRITE_KEEP[] = {[0 : KWIDTH]};
        }
    endgroup : cg_axi_s_channel
    `endif

    extern function new(string name = "axis_master_bfm");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    extern function void sample(COV        cov);
    `endif

    // 
    // Set the handle of the m_req_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_reqmlbx(ref mailbox #(REQ) req_mlbx);

    extern task wait_clock(input int num = 'd1);
    
    // Using a wrapper task to avoid of the core dump when using vivado simulator
    // I figure that vivado simulator does not support invoking function/task in interface
    // Wrapper task defiene start {{{

    extern task wrapper_transmit_data(input DATA_t data[], 
                                      input KEEP_t keep, 
                                      input USER_t user);

    // Wrapper task define end }}}
 
    // 
    // Desmb data(Extract data, address, burstlen from the axi_data)
    //

    extern function int desmb_data(const ref REQ    req, 
                                   ref       DATA_t data[],
                                   output    KEEP_t keep, 
                                   output    USER_t user);

    extern task transmit();

    extern task transmit_write();

    extern task transmit_data();

endclass : axis_master_bfm

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axis_master_bfm
//
//----------------------------------------------------------------------------------------------------------------------

function axis_master_bfm::new(string name = "axis_master_bfm");
    m_inst_name= name ;
    m_start    = 'd0  ;
    m_stop     = 'd0  ;
    m_order    = 'd1  ;
    m_w_lock   = new();

    m_wr_mlbx  = new(1);

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_s_channel = new();
    `endif

    m_wr_data.delete();
    m_wr_user.delete();
    m_wr_keep.delete();
endfunction : new

task axis_master_bfm::run();
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

task axis_master_bfm::start();
    m_start = 'd1;
    m_axis_vif.initial_bus_master();
    // Fork the main task
    fork
        run();
    join_none
endtask : start

task axis_master_bfm::stop();
    m_stop  = 'd1;
    wait (m_start == 'd0);
    m_axis_vif.initial_bus_master();
endtask : stop

task axis_master_bfm::main();
    fork
        transmit();
        transmit_write();
        transmit_data();
    join_none
endtask : main

// Function coverage was not supportted by vivado xsim
`ifndef VIVADO
function void axis_master_bfm::sample(COV        cov);
    m_axi_cov = cov;
    cg_axi_s_channel.sample();
endfunction : sample
`endif

function void axis_master_bfm::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

task axis_master_bfm::wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (m_axis_vif.dchn);
    `else
        repeat (num) @ (posedge m_axis_vif.aclk);
    `endif
endtask : wait_clock

task axis_master_bfm::wrapper_transmit_data(input DATA_t data[], 
                                            input KEEP_t keep, 
                                            input USER_t user);
`ifdef VIVADO
    bit last = 'd0;
    int blen = data.size();
    // Assert wvalid signal
    m_axis_vif.dvalid <= 'd1 ;
    foreach (data[idx]) begin
        last = (blen - 'd1) == idx;
        m_axis_vif.ddata  <= data[idx];
        m_axis_vif.dkeep  <= last ? keep : {KWIDTH{1'd1}};
        m_axis_vif.duser  <= last ? user : {UWIDTH{1'd0}};
        m_axis_vif.dlast  <= last;
        // Hold the valid, data, keep and last when ready goes down
        if (m_axis_vif.dready == 'd1) begin
            @ (posedge m_axis_vif.aclk);
        end else begin
            while (1) begin
                if (m_axis_vif.dready == 'd1)  break;
                @ (posedge m_axis_vif.aclk);
            end
        end
    end
    // Deasset the valid signal(Do not support back to back transpot)
    m_axis_vif.dvalid <= 'd0;
    m_axis_vif.dlast  <= 'd0;
`else
    m_axis_vif.transmit_data(data, keep, user);
`endif
endtask : wrapper_transmit_data

function int axis_master_bfm::desmb_data(const ref REQ    req, 
                                         ref       DATA_t data[],
                                         output    KEEP_t keep, 
                                         output    USER_t user);
    int len    = req.data.size();
    // Caculate how many slices of the data
    desmb_data = (len + DBYTES - 'd1) / DBYTES;
    user       = 'd0;
    if (!len) return 'd0;
    data = new[desmb_data];

    // Convert byte data stream to slices
    for (int idx = 'd0; idx < desmb_data; idx++) begin
        DATA_t byte_w;
        int    dsize  = len < DBYTES ? len : DBYTES;
        keep          = 'd0;
        // Fucking vivado xsim
        for (int bits = 'd0; bits < dsize; bits++) begin
            if (m_order) begin
                // LSB
                byte_w[(bits << 3) +: 'd8] = req.data[idx * DBYTES + bits];
                // Caculate keep of last cycle(Keep will be all valid when it is not
                // last cycle)
                keep[bits]                 = 1'd1;
            end else begin
                // MSB
                byte_w[DWIDTH - 'd1 - (bits << 3) -: 'd8] = req.data[idx * DBYTES + bits];
                // Caculate keep of last cycle(Keep will be all valid when it is not
                // last cycle)
                keep[KWIDTH - bits - 'd1]  = 1'd1;
            end
        end
        data[idx]  = byte_w;
        len -= dsize;
    end
endfunction : desmb_data

task axis_master_bfm::transmit();
    REQ  req;
    forever begin
        m_req_mlbx.peek(req);
        if (req != null) begin
            m_wr_mlbx.put(req);
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

task axis_master_bfm::transmit_write();
    DATA_t data[];
    KEEP_t keep;
    USER_t user;
    int    blen;
    REQ    req = new();
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        m_wr_mlbx.peek(req);
        // Write response must be later than address write and data write
        blen = desmb_data(req, data, keep, user);
        m_wr_data.push_back(data);
        m_wr_keep.push_back(keep);
        m_wr_user.push_back(user);
        // Add WR lock
        m_w_lock.put();
        m_wr_mlbx.get(req);
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.strb = keep;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov);
        `endif
    end
endtask : transmit_write

task axis_master_bfm::transmit_data();
    DATA_t data[];
    KEEP_t keep;
    USER_t user;
    forever begin
        wait_clock();
        // Get WR lock
        if (!m_w_lock.try_get()) continue;
        // Get wdata and strobe from queue
        data = m_wr_data.pop_front();
        keep = m_wr_keep.pop_front();
        user = m_wr_user.pop_front();
        // Invoke task in axi_interface to transmit data and strobe to bus
        wrapper_transmit_data(data, keep, user);
    end
endtask : transmit_data

`endif //  _AXIS_MASTER_BFM_SV_

