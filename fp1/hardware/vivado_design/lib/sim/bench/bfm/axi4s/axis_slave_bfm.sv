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


`ifndef _AXIS_SLAVE_BFM_SV_
`define _AXIS_SLAVE_BFM_SV_

// stimu/axi_data.svh
`include "axi_data.svh"

// common/axi_cov.svh
`include "axi_cov.svh"

class axis_slave_bfm # (type REQ    = axi_data, 
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

    protected mailbox #(RSP) m_rsp_mlbx;

    local     mailbox #(REQ) m_rd_mlbx;

    protected semaphore      m_r_lock;

    protected DATA_t         m_rd_data[$][];
    protected KEEP_t         m_rd_keep[$];
    protected USER_t         m_rd_user[$];

    protected COV            m_axi_cov;

    axis_vif_t               m_axis_vif;

    local     bit            m_start;
    local     bit            m_stop;

    bit                      m_order;

    protected string         m_bfm_name;

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

    extern function new(string name = "axis_slave_bfm");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    extern function void sample(COV        cov);
    `endif

    // 
    // Set the handle of the m_rsp_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_rspmlbx(ref mailbox #(RSP) rsp_mlbx);

    extern task wait_clock(input int num = 'd1);
    
    // Using a wrapper task to avoid of the core dump when using vivado simulator
    // I figure that vivado simulator does not support invoking function/task in interface
    // Wrapper task defiene start {{{

    extern task wrapper_collect_data(output DATA_t data[], 
                                     output KEEP_t keep, 
                                     output USER_t user);

    // Wrapper task define end }}}
 
    // 
    // Asmb data(Combine data, address, burstlen to axi_data)
    //

    extern function int asmb_data(ref       RSP    rsp, 
                                  const ref DATA_t data[],
                                  input     KEEP_t keep,
                                  input     USER_t user);

    extern task collect();

    extern task collect_write();

    extern task collect_data();

endclass : axis_slave_bfm

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axis_slave_bfm
//
//----------------------------------------------------------------------------------------------------------------------

function axis_slave_bfm::new(string name = "axis_slave_bfm");
    m_bfm_name= name ;
    m_start   = 'd0  ;
    m_stop    = 'd0  ;
    m_order   = 'd1  ;
    m_r_lock  = new();

    m_rd_mlbx = new(1);

    // Function coverage was not supportted by vivado xsim
    `ifndef VIVADO
    cg_axi_s_channel = new();
    `endif

    m_rd_data.delete();
    m_rd_user.delete();
    m_rd_keep.delete();
endfunction : new

task axis_slave_bfm::run();
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

task axis_slave_bfm::start();
    m_start = 'd1;
    m_axis_vif.initial_bus_slave();
    // Fork the main task
    // Vivado simulator do not support calling task by fork join_none in function
    fork
        run();
    join_none
endtask : start

task axis_slave_bfm::stop();
    m_stop  = 'd1;
    wait (m_start == 'd0);
    m_axis_vif.initial_bus_slave();
endtask : stop

task axis_slave_bfm::main();
    fork
        collect();
        collect_write();
        collect_data();
    join_none
endtask : main

// Function coverage was not supportted by vivado xsim
`ifndef VIVADO
function void axis_slave_bfm::sample(COV        cov);
    m_axi_cov = cov;
    cg_axi_s_channel.sample();
endfunction : sample
`endif

function void axis_slave_bfm::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

task axis_slave_bfm::wait_clock(input int num = 'd1);
    // Clocking was not supportted by vivado xsim
    `ifndef VIVADO
        repeat (num) @ (m_axis_vif.dchn);
    `else
        repeat (num) @ (posedge m_axis_vif.aclk);
    `endif
endtask : wait_clock

task axis_slave_bfm::wrapper_collect_data(output DATA_t data[], 
                                          output KEEP_t keep, 
                                          output USER_t user);
`ifdef VIVADO
    bit    last = 'd0;
    int    idx  = 'd0;
    int    blen = data.size();
    DATA_t data_queue[$];
    while (m_axis_vif.dvalid != 'd1) begin
        @ (posedge m_axis_vif.aclk);
    end
    if (m_axis_vif.dready != 'd1) begin
        // Assert ready when valid is high
        m_axis_vif.dready <= 'd1;
        @ (posedge m_axis_vif.aclk);
    end
    while (1) begin
        // Get data when valid and ready are both active
        data_queue.push_back(m_axis_vif.ddata);
        keep = m_axis_vif.dkeep;
        // When last is valid, deassert the valid to low
        // Collect wstrb only when wlast is valid
        if (m_axis_vif.dlast == 'd1) begin
            data = data_queue;
            m_axis_vif.dready <= 'd0;
            break;
        end else if (++idx >= `AXI4_MAX_LENGTH) begin
            return;
        end
        @ (posedge m_axis_vif.aclk);
    end
`else
    m_axis_vif.collect_data(data, keep, user);
`endif
endtask : wrapper_collect_data

function int axis_slave_bfm::asmb_data(ref       RSP    rsp, 
                                       const ref DATA_t data[],
                                       input     KEEP_t keep,
                                       input     USER_t user);
    int blen   = data.size();
    bit [7 : 0] byte_queue[$];
    // Caculate how many slices of the data
    // If no data existed, skip data asmb
    if (!blen) return 'd0;
    rsp.data.delete();
    // Convert byte data stream to slices
    for (int idx = 'd0; idx < blen; idx++) begin
        for (int pos = 'd0; pos < DBYTES; pos++) begin
            DATA_t byte_w = data[idx];
            if (m_order) begin
                // First transmit LSB
                // If keep is valid, push byte of data to byte_queue. Ignore
                // the keep if not the last index.
                if (idx < blen - 'd1 || keep[pos] == 'd1) 
                    byte_queue.push_back(byte_w[(pos << 3) +: 8]);
            end else begin
                // First transmit MSB
                // If keep is valid, push byte of data to byte_queue. Ignore
                // the keep if not the last index.
                if (idx < blen - 'd1 || keep[KWIDTH - pos - 'd1] == 'd1) 
                    byte_queue.push_back(byte_w[DWIDTH - 'd1 - (pos << 3) -: 8]);
            end
        end
    end
    rsp.data = byte_queue;
    asmb_data  = byte_queue.size();
endfunction : asmb_data

task axis_slave_bfm::collect();
    RSP  rsp;
    forever begin
        m_rd_mlbx.peek(rsp);
        if (rsp != null) begin
            m_rsp_mlbx.put(rsp);
        end
        m_rd_mlbx.get(rsp);
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

task axis_slave_bfm::collect_write();
    DATA_t data[];
    KEEP_t keep;
    USER_t user;
    RSP    rsp = new();
    COV    cov = new();
    forever begin
        // Write response must be later than address write and data write
        m_r_lock.get();
        data = m_rd_data.pop_front();
        keep = m_rd_keep.pop_front();
        user = m_rd_user.pop_front();
        // Asmb packet
        void'(asmb_data(rsp, data, keep, user));
        m_rd_mlbx.put(rsp);
        // Coverage
        cov.opt  = e_AXI_OPT_WR;
        cov.strb = keep;
        // If coverage enable, collect coverage.
        // Function coverage was not supportted by vivado xsim
        `ifndef VIVADO
            if (g_axi_cov_en) sample(cov);
        `endif
    end
endtask : collect_write

task axis_slave_bfm::collect_data();
    DATA_t data[];
    KEEP_t keep;
    USER_t user;
    forever begin
        wait_clock();
        // Collect rdata from rdata bus
        wrapper_collect_data(data, keep, user);
        // Push data and strobe to queue
        m_rd_data.push_back(data);
        m_rd_keep.push_back(keep);
        m_rd_user.push_back(user);
        // Add WR lock
        m_r_lock.put();
    end
endtask : collect_data

`endif //  _AXIS_MASTER_BFM_SV_

