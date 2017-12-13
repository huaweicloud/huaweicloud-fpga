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

`ifndef _CPU_MODEL_CB_SVH_
`define _CPU_MODEL_CB_SVH_

`ifndef USER_DDR_NUM
  `define USER_DDR_NUM 'd4
`endif

// ./common/common_define.svh
`include "common_define.svh"

// ./stim/axi_data.svh
`include "axi_data.svh"

// ./stim/cpu_data.svh
`include "cpu_data.svh"

// ./stim/acc_data.svh
`include "acc_data.svh"

// ./rm/axi_rm.sv
`include "axi_rm.sv"

class cpu_model_cb #(type REQ = axi_data, 
                     type RSP = REQ);

    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef bit [63 : 0] ADDR_t;
    typedef bit [7  : 0] DATA_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------

    // Data Storage
    local     DATA_t         m_data[ADDR_t][$];
    // Acc Storage
    local     acc_data       m_acc[ADDR_t];
    // Descriptor Storage
    local     cpu_data       m_bd[int][$];

    protected ADDR_t         m_acc_addr[`USER_DDR_NUM];
    protected int            m_acc_id;

    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    protected mailbox #(REQ) m_ist_mlbx;
    protected mailbox #(REQ) m_chk_mlbx;

    // Connect to bfm
    mailbox #(REQ)           m_axismc_mlbx;
    mailbox #(REQ)           m_axismd_mlbx;

    mailbox #(RSP)           m_axissc_mlbx;
    mailbox #(RSP)           m_axissd_mlbx;

    protected bit            m_check_ok;

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new(string name = "cpu_model_cb");

    // 
    // Set the handle of the m_req_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_reqmlbx(ref mailbox #(REQ) req_mlbx);

    // 
    // Set the handle of the m_rsp_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);

    // 
    // Set the handle of the m_ist_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_istmlbx(ref mailbox #(REQ) ist_mlbx);

    // 
    // Set the handle of the m_chk_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_chkmlbx(ref mailbox #(REQ) chk_mlbx);

    // Process to request data(read response)

    extern virtual task request_process();

    // Process to response data(read response)

    extern virtual task response_process();

    // User Process

    extern virtual task user_process();

    // Generate Bd
    extern function cpu_data generate_bd(ref REQ req);

    // Get CPU Head
    extern function cpu_data get_cpuhead(ref REQ req);

    extern task send_bd2bfm(ref REQ req);
    
    extern task get_datarsp(ref REQ rsp);

    extern task get_bfmrsp(ref REQ rsp);

    extern virtual task insert(ref REQ req);

    extern virtual task check(ref REQ req);

endclass : cpu_model_cb

function cpu_model_cb::new(string name = "cpu_model_cb");
    for (int idx = 'd0; idx < `USER_DDR_NUM; idx++) begin
        int         addr = $urandom();
        m_acc_addr[idx][33 : 2 ]  = addr;
        m_acc_addr[idx][35 : 34]  = idx[1 : 0];
    end
    m_acc_id    = 'd0;
endfunction : new

function void cpu_model_cb::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void cpu_model_cb::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

function void cpu_model_cb::set_istmlbx(ref mailbox #(REQ) ist_mlbx);
    m_ist_mlbx = ist_mlbx;
endfunction: set_istmlbx

function void cpu_model_cb::set_chkmlbx(ref mailbox #(REQ) chk_mlbx);
    m_chk_mlbx = chk_mlbx;
endfunction: set_chkmlbx

task cpu_model_cb::request_process();
    REQ req;
    forever begin
        // Get request
        m_req_mlbx.get(req);
        if (req == null) continue;
        // Data from stimulate
        send_bd2bfm(req);
    end
endtask : request_process

task cpu_model_cb::response_process();
    RSP rsp;
    forever begin
        // Get response
        m_axissd_mlbx.get(rsp);
        if (rsp == null) continue;
        // Send data to scb
        get_datarsp(rsp);
    end
endtask : response_process

task cpu_model_cb::user_process();
    REQ rsp;
    forever begin
        // Get response
        m_axissc_mlbx.get(rsp);
        if (rsp == null) continue;
        // Send data to DUT
        get_bfmrsp(rsp);
    end
endtask : user_process

function cpu_data cpu_model_cb::generate_bd(ref REQ req);
    DATA_t byte_array[] = req.data;
    cpu_data head = new();
    head.ve_info  = 'd1;
    head.data_len = byte_array.size();
    head.addr     = req.addr;
    head.dest_addr= req.addr;
    head.acc_type = 'd0;
    head.cpu_type = cpu_data::e_CPU_WR_BD;
    generate_bd   = head;
    // void'(generate_bd.unpack_bytes(byte_array));
endfunction : generate_bd

function cpu_data cpu_model_cb::get_cpuhead(ref REQ req);
    // DATA_t byte_array[] = req.data;
    DATA_t byte_array[];
    get_cpuhead = new();
    if (req.stream == FLOW_RM_CHECK) begin
        int head_size = `AXI4S_DATA_WIDTH >> 3;
        get_cpuhead.cpu_type = cpu_data::e_CPU_RD_DATA;
    `ifdef VIVADO
        byte_array = new[head_size];
        foreach (byte_array[idx]) begin
            byte_array[idx] = req.data[idx];
        end
    `else
        byte_array = new[head_size](req.data);
    `endif
    end else begin
        get_cpuhead.cpu_type = cpu_data::e_CPU_WR_REQ;
        byte_array = req.data;
    end
    void'(get_cpuhead.unpack_bytes(byte_array));
endfunction : get_cpuhead

task cpu_model_cb::send_bd2bfm(ref REQ req);
    REQ      trans;
    REQ      data;
    cpu_data bd = generate_bd(req);
    acc_data acc= new();
    DATA_t   byte_array[];
    int      head_len;
    int      data_len;
    bd.data_len+= 'd32;
    head_len    = bd.pack_bytes(byte_array);
    data_len    = req.data.size();
    bd.cpu_type = cpu_data::e_CPU_RD_DATA;
    // Add BD to queue
    m_bd[bd.ve_info].push_back(bd);
    // Add hardacc to pool
    acc.addr        = m_acc_addr[m_acc_id];
    acc.dest_addr   = m_acc_addr[m_acc_id];
    m_acc[bd.addr]  = acc;
    m_acc_addr[m_acc_id++] += 'h4096;
    if (m_acc_id >= `USER_DDR_NUM) m_acc_id = 'd0;
`ifndef USE_DDR_MODEL
    if (m_acc_id == 'd2) m_acc_id++;
`endif
    // Add data to pool
    m_data[bd.addr + 'd32]  = req.data;
    // Insert to RM
    data = new();
`ifndef VIVADO
    data.data  = req.data;
`else
    data.set_data(req.data);
`endif
    insert(data);
    // Write BD to BFM
    trans      = new();
`ifndef VIVADO
    trans.data = byte_array; 
`else
    trans.set_data(byte_array);
`endif
    // trans.addr = ;
    m_axismc_mlbx.put(trans);
endtask : send_bd2bfm

task cpu_model_cb::get_datarsp(ref RSP rsp);
    REQ      trans;
    cpu_data head ;
    DATA_t   byte_queue[$];
    string   info;
    rsp.stream   = FLOW_RM_CHECK;
    head = get_cpuhead(rsp);
    if (m_acc.exists(head.addr)) begin
        int head_len = `AXI4_DATA_WIDTH >> 3;
        bit [7 : 0] byte_array[];
        acc_data cmp = new();
        acc_data exp = m_acc[head.addr];
        byte_queue   = rsp.data;
    `ifndef VIVADO
        byte_array = byte_queue[`AXI4_DATA_WIDTH >> 3 : $];
    `else
        for (int idx = 0; idx < head_len; idx++) begin
            void'(byte_queue.pop_front());
        end
        byte_array = byte_queue;
    `endif
        void'(cmp.unpack_bytes(byte_array));
        if (!exp.compare(cmp)) begin
            string info = "Hardacc Compare Error:\n";
            $sformat(info, "%s------------------------------------\n", info);
            $sformat(info, "%s[Expect Acc] is \n%s\n", info, exp.psdisplay());
            $sformat(info, "%s[Actual Acc] is \n%s\n", info, cmp.psdisplay());
            $sformat(info, "%s------------------------------------\n", info);
            `tb_error("cpu_model_cb", info)
        end
        m_acc.delete(head.addr);
    end else if (m_data.exists(head.addr)) begin
        int data_len = byte_queue.size();
        int head_len = `AXI4_DATA_WIDTH >> 3;
        trans        = rsp.copy();
        byte_queue   = rsp.data;
    `ifndef VIVADO
        trans.data = byte_queue[`AXI4_DATA_WIDTH >> 3 : $];
    `else
        for (int idx = 0; idx < head_len; idx++) begin
            void'(byte_queue.pop_front());
        end
        begin
            bit [7 : 0] byte_array[] = byte_queue;
            trans.set_data(byte_array);
        end
    `endif
        check(trans);
        m_data.delete(head.addr);
    end else begin
        `tb_error("cpu_model_cb", "Data can not be found!")
    end
endtask : get_datarsp

task cpu_model_cb::get_bfmrsp(ref REQ rsp);

    cpu_data head ;
    acc_data acc  ;
    DATA_t   byte_array[];
    DATA_t   byte_head[];
    DATA_t   byte_trans[];
    int      head_len;
    int      byte_len;
    rsp.stream   = FLOW_AXI_S_WRITE;
    head = get_cpuhead(rsp);
    if (m_acc.exists(head.addr)) begin
        REQ trans  = new();
        int head_size;
        acc        = m_acc[head.addr];
        byte_len   = acc.pack_bytes(byte_array);
        head.cpu_type = cpu_data::e_CPU_WR_DATA;
        head.done_flag= 'd1;
        head_size  = head.pack_bytes(byte_head);
        // Pad head to data width
        head_len   = `AXI4_DATA_WIDTH >> 3;
        byte_len   = head_len + byte_len;
        byte_trans = new[byte_len];
        for (int idx = 'd0; idx < byte_len; idx++) begin
            if (idx < head_size) begin
                byte_trans[idx] = byte_head[idx];
            end else if (idx >= head_len) begin
                byte_trans[idx] = byte_array[idx - head_len];
            end
        end
    `ifdef VIVADO
        trans.set_data(byte_trans);
    `else
        trans.data = byte_trans;
    `endif
        m_axismd_mlbx.put(trans);
    end else if (m_data.exists(head.addr)) begin
        REQ trans  = new();
        int head_size;
        byte_array = m_data[head.addr];
        head.cpu_type = cpu_data::e_CPU_WR_DATA;
        head.done_flag= 'd1;
        head_size  = head.pack_bytes(byte_head);
        // Pad head to data width
        head_len   = `AXI4_DATA_WIDTH >> 3;
        byte_len   = head_len + byte_array.size();
        // byte_trans = new[byte_len](byte_head);
        byte_trans = new[byte_len];
        for (int idx = 'd0; idx < byte_len; idx++) begin
            if (idx < head_size) begin
                byte_trans[idx] = byte_head[idx];
            end else if (idx >= head_len) begin
                byte_trans[idx] = byte_array[idx - head_len];
            end
        end
    `ifdef VIVADO
        trans.set_data(byte_trans);
    `else
        trans.data = byte_trans;
    `endif
        m_axismd_mlbx.put(trans);
    end else begin
        `tb_error("cpu_model_cb", "Data rsp is invalid, data is null!")
    end
endtask : get_bfmrsp

task cpu_model_cb::insert(ref REQ req);
    req.fsn   = 'd0;
    req.stream= FLOW_RM_CHECK;
    m_ist_mlbx.put(req);
endtask : insert

task cpu_model_cb::check(ref REQ req);
    req.fsn   = 'd0;
    req.stream= FLOW_RM_CHECK;
    m_chk_mlbx.put(req);
endtask : check

`endif // _CPU_MODEL_CB_SVH_

